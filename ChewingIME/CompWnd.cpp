#include "compwnd.h"
#include "ChewingIME.h"
#include "DrawUtil.h"
#include "CandWnd.h"

static CompWnd* g_thisCompWnd = NULL;

CompWnd::CompWnd( HWND imeUIWnd ) : IMEWnd(imeUIWnd, g_comp_wnd_class)
{
	g_thisCompWnd = this;
	font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	LOGFONT lf;
	GetObject( font, sizeof(lf), &lf);
	lf.lfHeight = 16;
	font = CreateFontIndirect( &lf );
}

CompWnd::~CompWnd(void)
{
	DeleteObject( font );
}

BOOL CompWnd::RegisterClass(void)
{
	WNDCLASSEX wc;
	wc.cbSize			= sizeof(WNDCLASSEX);
	wc.style			= CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS| CS_IME;
	wc.lpfnWndProc		= (WNDPROC)CompWnd::WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= g_dllInst;
	wc.hCursor			= LoadCursor( NULL, IDC_ARROW );
	wc.hIcon			= NULL;
	wc.lpszMenuName		= (LPTSTR)NULL;
	wc.lpszClassName	= g_comp_wnd_class;
	wc.hbrBackground	= NULL;
	wc.hIconSm			= NULL;
	if( !RegisterClassEx( (LPWNDCLASSEX)&wc ) )
		return FALSE;

	return TRUE;
}

LRESULT CompWnd::WndProc(HWND hwnd , UINT msg, WPARAM wp , LPARAM lp)
{
	switch (msg)
	{
		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				BeginPaint( hwnd, &ps );
				g_thisCompWnd->OnPaint(ps);
				EndPaint(hwnd, &ps);
				break;
			}
		case WM_ERASEBKGND:
			return TRUE;
			break;
/*		case WM_SETCURSOR:
		case WM_MOUSEMOVE:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
			DragUI(hWnd, NULL, msg, wp, lp, TRUE);
			if ((msg == WM_SETCURSOR) &&
					(HIWORD(lp) != WM_LBUTTONDOWN) &&
					(HIWORD(lp) != WM_RBUTTONDOWN))
				return DefWindowProc(hWnd, msg, wp, lp);
			if ((msg == WM_LBUTTONUP) || (msg == WM_RBUTTONUP))
				SetWindowLong(hWnd, FIGWL_MOUSE, 0L);
			break;
*/		default:
			if (!IsImeMessage(msg))
				return DefWindowProc(hwnd, msg, wp, lp);
	}
	return 0;
}


void CompWnd::setCompStr( string compstr )
{
	compStr = compstr;
	int w, h;
	getSize(&w, &h);
	SetWindowPos( hwnd, NULL, 0, 0, w, h, SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER );
	InvalidateRect( hwnd, NULL, FALSE );
}


void CompWnd::OnPaint(PAINTSTRUCT& ps)
{
	HFONT oldFont;
	RECT rc;
	HBRUSH hBrush = (HBRUSH)NULL;
	HBRUSH hOldBrush = (HBRUSH)NULL;
	HPEN hPen = (HPEN)NULL;
	HPEN hOldPen = (HPEN)NULL;

	GetClientRect( hwnd, &rc );
	HDC memdc = CreateCompatibleDC( ps.hdc );
	HBITMAP membmp = CreateCompatibleBitmap( ps.hdc, rc.right, rc.bottom );
	HGDIOBJ oldbmp = SelectObject( memdc, membmp );

	InflateRect( &rc, -1, -1 );

	oldFont = (HFONT)SelectObject(memdc, font);
	if( !compStr.empty() )
	{
		SetTextColor( memdc, GetSysColor( COLOR_WINDOWTEXT ) );
		SetBkColor( memdc, GetSysColor( COLOR_WINDOW ) );
		ExtTextOut( memdc, 1, 1, ETO_OPAQUE, &rc, compStr.c_str(), 
			compStr.length(), NULL);
//		int selstart = indexToXPos( CompSelStart );
//		int selend = indexToXPos( CompSelEnd );
		int cursor = indexToXPos( cursorPos );
//		BitBlt( memdc, selstart, 0, selend-selstart, rc.bottom, memdc, selstart, 0, NOTSRCCOPY );
		BitBlt( memdc, cursor, 0, 1, rc.bottom, memdc, cursor, 0, SRCINVERT );
	}

	InflateRect( &rc, 1, 1 );
	Draw3DBorder( memdc, &rc, GetSysColor(COLOR_3DFACE), 0/*GetSysColor(COLOR_3DDKSHADOW)*/);

	BitBlt( ps.hdc, ps.rcPaint.left, ps.rcPaint.top, 
		(ps.rcPaint.right-ps.rcPaint.left), 
		(ps.rcPaint.bottom-ps.rcPaint.top), 
		memdc, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);

	SelectObject( memdc, oldbmp );
	DeleteObject( membmp );
	DeleteDC( memdc );

	SelectObject(memdc, oldFont);
}

void CompWnd::setFont(LOGFONT* lf)
{
	LOGFONT lf2;
	memcpy( &lf2, lf, sizeof( lf2) );

	if( abs(lf2.lfHeight) < 12 )
		lf2.lfHeight = abs(lf2.lfHeight) + 2;

	if( font )
		DeleteObject( font );

	font = CreateFontIndirect( &lf2 );
}

void CompWnd::getSize(int* w, int* h)
{
	HDC dc = GetDC( hwnd );
	HGDIOBJ oldfont = SelectObject( dc, font );
	SIZE size;
	GetTextExtentPoint( dc, compStr.c_str(), compStr.length(), &size );
	SelectObject( dc, oldfont );
	ReleaseDC( hwnd, dc );
	*w = size.cx + 4;
	*h = size.cy + 4;
}

void CompWnd::setCursorPos(int pos)
{
	int x = indexToXPos( cursorPos );
	RECT rc;
	GetClientRect( hwnd, &rc );
	HDC dc = GetDC( hwnd );
	BitBlt( dc, x, 0, 1, rc.bottom, dc, x, 0, SRCINVERT );
	cursorPos = pos;
	x = indexToXPos( pos );
	BitBlt( dc, x, 0, 1, rc.bottom, dc, x, 0, SRCINVERT );
}

int CompWnd::indexToXPos(int idx)
{
	if( compStr.empty() || idx <=0 )
		return 2;
	HDC dc = GetDC( hwnd );
	HGDIOBJ oldfont = SelectObject( dc, font );
	SIZE size;
	GetTextExtentPoint( dc, compStr.c_str(), idx, &size );
	SelectObject( dc, oldfont );
	ReleaseDC( hwnd, dc );
	return size.cx + 2;
}

void CompWnd::showCand(void)
{
	HIMC hIMC = getIMC();
	INPUTCONTEXT* ic = ImmLockIMC(hIMC);

	if( isVisible() )
	{
		POINT pt;
		RECT rc;
		GetWindowRect( hwnd, &rc );
		ic->cfCandForm->ptCurrentPos = ic->cfCompForm.ptCurrentPos;
		ic->cfCandForm->ptCurrentPos.y += (rc.bottom - rc.top);
		pt = ic->cfCandForm->ptCurrentPos;

		ClientToScreen( ic->hWnd, &pt );
		pt.x += indexToXPos(cursorPos);
		g_candWnd->Move( pt.x, pt.y );
	}
	ImmUnlockIMC(hIMC);

	g_candWnd->setFont( font );
	g_candWnd->updateSize();
	g_candWnd->Show();

}
