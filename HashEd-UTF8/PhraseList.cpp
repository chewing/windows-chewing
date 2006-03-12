#include ".\phraselist.h"
#include <windowsx.h>
#include <tchar.h>

using namespace std;

static const TCHAR phrase_list_class[] = _T("PhraseList");

const int Y_MARGIN = 2;
const int X_MARGIN = 2;
const int ITEM_WIDTH = 100;

CPhraseList::CPhraseList(void)
	: hwnd(NULL), sel(-1), itemsPerRow(0), lineCount(0),
	lock(false), itemHeight(16)
{
}

CPhraseList::~CPhraseList(void)
{
}

BOOL CPhraseList::init(void)
{
	WNDCLASS wc = {0};
	wc.hbrBackground = HBRUSH(COLOR_WINDOW+1);
	wc.hCursor = LoadCursor( NULL, IDC_ARROW );
	wc.hInstance = (HINSTANCE)GetModuleHandle( NULL );
	wc.lpfnWndProc = (WNDPROC)CPhraseList::wndProc;
	wc.lpszClassName = phrase_list_class;
	wc.style = CS_HREDRAW|CS_VREDRAW;

	return RegisterClass( &wc );
}

LRESULT CPhraseList::wndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	CPhraseList* pl = CPhraseList::fromHandle( hwnd );
	if( !pl ) {
		pl = new CPhraseList;
		pl->hwnd = hwnd;
		SetWindowLongPtr( hwnd, GWL_USERDATA, (LONG_PTR)pl );
	}
	return pl->wndProc( msg, wp, lp );
}

LRESULT CPhraseList::wndProc( UINT msg, WPARAM wp, LPARAM lp )
{
	switch( msg )
	{
	case WM_PAINT:
		onPaint();
		break;
	case WM_LBUTTONDOWN:
		onLButtonDown( wp, GET_X_LPARAM(lp), GET_Y_LPARAM(lp) );
		break;
	case WM_VSCROLL:
		onVScroll( LOWORD(wp), HIWORD(wp) );
		break;
	case WM_MOUSEWHEEL:
		onMouseWheel( (short)HIWORD(wp) );
		break;
	case WM_SIZE:
		onSize( wp, LOWORD(lp), HIWORD(lp) );
		break;
	case WM_SETFONT:
		onSetFont( (HFONT)wp );
		break;
	case WM_KILLFOCUS:
	case WM_SETFOCUS:
		if( sel >= 0 ) {
			RECT rc;
			getItemRect( sel, rc );
			long scroll_pos = GetScrollPos( hwnd, SB_VERT ) * itemHeight;
			OffsetRect( &rc, 0, -scroll_pos );
			InvalidateRect( hwnd, &rc, TRUE );	// Redraw
		}
		break;
	case WM_GETDLGCODE:
		return DLGC_WANTARROWS;
	case WM_CREATE:
		ShowScrollBar( hwnd, SB_VERT, TRUE );
		break;
	case WM_KEYDOWN:
		if( onKeyDown( wp, lp ) )
			break;
	default:
		return DefWindowProc( hwnd, msg, wp, lp );
	}
	return 0;
}


void CPhraseList::onSize( WPARAM wp, long cx, long cy )
{
	// Recalculate layout
	itemsPerRow = cx / ITEM_WIDTH;
	lineCount = (count() / itemsPerRow) + (cy % itemsPerRow ? 1 : 0);

	SCROLLINFO si = {0};
	si.cbSize = sizeof(si);
	si.fMask = SIF_RANGE|SIF_PAGE;
	si.nPage = 4;
	si.nMax = lineCount;
	si.nMin = 0;
	SetScrollInfo( hwnd, SB_VERT, &si, TRUE );
}

void CPhraseList::onVScroll( int code, int pos )
{
	SCROLLINFO si;
	si.fMask = SIF_ALL;
	GetScrollInfo( hwnd, SB_VERT, &si );
	si.fMask = SIF_POS;
	int oldPos = si.nPos;
	switch( code )
	{
	case SB_BOTTOM: // Scrolls to the lower right. 
		si.nPos = si.nMax - si.nPage;
		break;
	case SB_ENDSCROLL: // Ends scroll. 
		break;
	case SB_LINEDOWN: // Scrolls one line down. 
		++si.nPos;
		break;
	case SB_LINEUP: // Scrolls one line up. 
		--si.nPos;
		break;
	case SB_PAGEDOWN: // Scrolls one page down. 
		si.nPos += si.nPage;
		break;
	case SB_PAGEUP: // Scrolls one page up. 
		si.nPos -= si.nPage;
		break;
	case SB_THUMBPOSITION: // The user has dragged the scroll box (thumb) and released the mouse button. The nPos parameter indicates the position of the scroll box at the end of the drag operation. 
		si.nPos = pos;
		break;
	case SB_THUMBTRACK: // The user is dragging the scroll box. This message is sent repeatedly until the user releases the mouse button. The nPos parameter indicates the position that the scroll box has been dragged to. 
		si.nPos = pos;
		break;
	case SB_TOP: // Scrolls to the upper left. 
		si.nPos = 0;
		break;
	}
	if( si.nPos < 0 )
		si.nPos = 0;
	else if( si.nPos > (si.nMax - si.nPage) )
		si.nPos = (si.nMax - si.nPage);

	if( oldPos != si.nPos ) {
		SetScrollInfo( hwnd, SB_VERT, &si, TRUE );
		InvalidateRect( hwnd, NULL, TRUE );
	}
}

void CPhraseList::onPaint(void)
{
	// Draw content
	PAINTSTRUCT ps;
	BeginPaint( hwnd, &ps );

	RECT rc;
	GetClientRect( hwnd, &rc );

	HGDIOBJ oldf = SelectObject( ps.hdc, font );

	RECT irc;
	RECT item_rc;
	item_rc.left = X_MARGIN;
	item_rc.top = 0;
	item_rc.right = ITEM_WIDTH;
	item_rc.bottom = itemHeight;

	int i = itemFromPoint( 0, itemHeight * GetScrollPos( hwnd, SB_VERT ) );
	int c = count();
	while( i < c )
	{
		if( IntersectRect( &irc, &item_rc, &ps.rcPaint ) )
		{
			if( i == sel ) {
				SetTextColor( ps.hdc, GetSysColor(COLOR_HIGHLIGHTTEXT) );
				SetBkColor( ps.hdc, GetSysColor(COLOR_HIGHLIGHT) );
			}
			const WCHAR* text = data[i].first.c_str();
			ExtTextOutW( ps.hdc, 
						item_rc.left + X_MARGIN,
						item_rc.top + Y_MARGIN,
						ETO_OPAQUE|ETO_CLIPPED, 
						&item_rc, text, data[i].first.length(),
						NULL );
			if( i == sel ) {
				if( GetFocus() == hwnd ){
					DrawFocusRect( ps.hdc, &item_rc );
				}
				SetTextColor( ps.hdc, GetSysColor(COLOR_WINDOWTEXT) );
				SetBkColor( ps.hdc, GetSysColor(COLOR_WINDOW) );
			}
		}

		++i;
		if( (i % itemsPerRow) == 0 ){
			item_rc.left = X_MARGIN;
			item_rc.right = ITEM_WIDTH;
			item_rc.top += itemHeight;
			item_rc.bottom = item_rc.top + itemHeight;
		}
		else{
			OffsetRect( &item_rc, ITEM_WIDTH, 0 );
		}

		if( item_rc.top > rc.bottom )
			break;
	}

	SelectObject( ps.hdc, oldf );
	EndPaint( hwnd, &ps );
}


int CPhraseList::itemFromPoint( int x, int y )
{
	x -= X_MARGIN;
	x /= ITEM_WIDTH;
	if( x >= itemsPerRow )
		x = itemsPerRow - 1;
	y /= itemHeight;
	if( y >= lineCount )
		y = lineCount - 1;

	if( x >= 0 && y >= 0 ) {
		return ( y * itemsPerRow + x );
	}
	return -1;
}

void CPhraseList::getItemRect( int idx, RECT& rc )
{
	int x = idx % itemsPerRow;
	int y = idx / itemsPerRow;
	rc.left = x * ITEM_WIDTH;
	rc.top = y * itemHeight;
	rc.right = rc.left + ITEM_WIDTH;
	rc.bottom = rc.top + itemHeight;
}

void CPhraseList::setCurSel(int idx)
{
	int old = sel;
	if( idx >= 0 && idx < count() )
		sel = idx;
	else {
		return;
	}
	long scroll_pos = GetScrollPos( hwnd, SB_VERT ) * itemHeight;
	if( !lock ) {
		RECT rc;
		if( old >= 0 ) {
			getItemRect( old, rc );
			OffsetRect( &rc, 0, -scroll_pos );
			InvalidateRect( hwnd, &rc, TRUE );	// Redraw
		}
		getItemRect( sel, rc );
		OffsetRect( &rc, 0, -scroll_pos );
		InvalidateRect( hwnd, &rc, TRUE );	// Redraw
	}
}

int CPhraseList::getCurSel(void)
{
	return -1;
}

void CPhraseList::onLButtonDown(WPARAM wp, int x, int y)
{
	SetFocus( hwnd );
	y += GetScrollPos( hwnd, SB_VERT ) * itemHeight;
	setCurSel( itemFromPoint(x, y) );
}

int CPhraseList::count(void)
{
	return data.size();
}

void CPhraseList::insertItem(int pos, std::wstring text, void* user_data)
{
	if( pos < 0 || pos >= count() ) {
		data.insert( data.end(), 
			pair<wstring, void*>(text, user_data) );
	}
	else {
		data.insert( data.begin() + pos, 
			pair<wstring, void*>(text, user_data) );
	}
	if( !lock )
		recalcLayout();
}

std::wstring CPhraseList::getItemText( int idx )
{
	if( idx < data.size() && idx >=0 ) {
		return data[idx].first;
	}
	return wstring();
}

void* CPhraseList::getItemData( int idx )
{
	if( idx < data.size() && idx >=0 ) {
		return data[idx].second;
	}
	return NULL;
}

void CPhraseList::deleteItem( int idx )
{
	if( !lock )
		recalcLayout();
}

void CPhraseList::clear(void)
{
	data.clear();
	if( !lock )
		recalcLayout();
}


void CPhraseList::recalcLayout(void)
{
	RECT rc;
	GetClientRect( hwnd, &rc );
	onSize( 0, rc.right, rc.bottom );	// Recalc layout
	InvalidateRect( hwnd, NULL, TRUE );	// Redraw
}

void CPhraseList::lockUpdate(void)
{
	lock = true;
}

void CPhraseList::unlockUpdate(void)
{
	if( lock ) {
		lock = false;
		recalcLayout();
	}
}

void CPhraseList::onSetFont(HFONT hfont)
{
	font = hfont;
	HDC dc = GetDC( hwnd );
	HGDIOBJ oldf = SelectObject( dc, font );
	SIZE size;
	GetTextExtentPoint( dc, " ", 1, &size );
	itemHeight = size.cy + Y_MARGIN * 2;
	SelectObject( dc, oldf );
	ReleaseDC( hwnd, dc );
}

bool CPhraseList::onKeyDown(int key, LPARAM lp)
{
	int new_sel = -1;
	switch( key )
	{
	case VK_LEFT:
		new_sel = sel - 1;
		if( sel < 0 )
			new_sel = -1;
		break;
	case VK_RIGHT:
		new_sel = sel + 1;
		if( new_sel >= count() )
			new_sel = -1;
		break;
	case VK_UP:
		new_sel = sel - itemsPerRow;
		if( new_sel < 0 )
			new_sel = -1;
		break;
	case VK_DOWN:
		new_sel = sel + itemsPerRow;
		if( new_sel >= count() )
			new_sel = -1;
		break;
	case VK_PRIOR:
		onVScroll( SB_PAGEUP, 0 );
		break;
	case VK_NEXT:
		onVScroll( SB_PAGEDOWN, 0 );
		break;
	default:
		return false;
	}
	if( new_sel >= 0 ) {
		setCurSel( new_sel );
		ensureItemVisible( new_sel );
	}
	return true;
}

void CPhraseList::onMouseWheel(int delta)
{
	SCROLLINFO si;
	si.fMask = SIF_ALL;
	GetScrollInfo( hwnd, SB_VERT, &si );
	si.fMask = SIF_POS;
	int oldPos = si.nPos;

	delta /= WHEEL_DELTA;
	si.nPos += delta * (-5);

	if( si.nPos < 0 )
		si.nPos = 0;
	else if( si.nPos > (si.nMax - si.nPage) )
		si.nPos = (si.nMax - si.nPage);

	if( oldPos != si.nPos ) {
		SetScrollInfo( hwnd, SB_VERT, &si, TRUE );
		InvalidateRect( hwnd, NULL, TRUE );
	}
}

void CPhraseList::ensureItemVisible(int idx)
{
	RECT item_rc;
	RECT rc;
	::GetClientRect( hwnd, &rc );

	int scroll_pos = GetScrollPos( hwnd, SB_VERT );
	getItemRect( idx, item_rc );
	OffsetRect( &item_rc, 0, - (scroll_pos * itemHeight ) );

	int y = idx / itemsPerRow;

	if( item_rc.top < 0 ) {
		scroll_pos = y;
	}
	else if( item_rc.bottom > rc.bottom ) {
		int visible_lines = (rc.bottom / itemHeight);
		int last_visible_line = (scroll_pos + visible_lines);
		scroll_pos += ( y - last_visible_line );
	}
	else {
		return;
	}
	SetScrollPos( hwnd, SB_VERT, scroll_pos, TRUE );
	if( !lock )
		recalcLayout();
}
