// XPToolbar.cpp: implementation of the XPToolbar class.
//
//////////////////////////////////////////////////////////////////////

#include ".\xptoolbar.h"
#include <tchar.h>
#include "DrawUtil.h"
#include "Tooltip.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static LPCTSTR xpToolbarClass = _T("XPToolbar");

XPToolbar::XPToolbar(int gripperw, int btnw, int btnh)
	: themeBmp(NULL), gripperW(gripperw), btnW(btnw), btnH(btnh), 
	  prevHilightBtn(-1), curPressedBtn(-1), cmdTarget(NULL)
	  , tooltip(NULL)
{
}

XPToolbar::~XPToolbar()
{
}

//	NOT USED IN CHEWING IME
BOOL XPToolbar::registerClass()
{
	WNDCLASSEX wc;
	wc.cbSize         = sizeof(WNDCLASSEX);
	wc.style          = CS_HREDRAW|CS_VREDRAW;
	wc.lpfnWndProc    = (WNDPROC)XPToolbar::wndProc;
	wc.cbClsExtra     = 0;
	wc.cbWndExtra     = sizeof(LONG) * (sizeof(void*) > sizeof(LONG) ? 2 : 1);
	wc.hInstance      = (HINSTANCE)GetModuleHandle(NULL);
	wc.hCursor        = LoadCursor( NULL, IDC_ARROW );
	wc.hIcon          = NULL;
	wc.lpszMenuName   = (LPTSTR)NULL;
	wc.lpszClassName  = xpToolbarClass;
	wc.hbrBackground  = NULL;
	wc.hIconSm        = NULL;

	if( !RegisterClassEx( (LPWNDCLASSEX)&wc ) )
		return FALSE;

	return TRUE;
}

LRESULT CALLBACK XPToolbar::wndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	XPToolbar* pthis = NULL;
	pthis = (XPToolbar*)GetWindowLongPtr( hwnd, GWL_USERDATA );
	if( pthis )
		return pthis->wndProc(msg, wp, lp);
	return DefWindowProc(hwnd, msg, wp, lp );
}

LRESULT XPToolbar::wndProc(UINT msg, WPARAM wp, LPARAM lp)
{
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			BeginPaint( hwnd, &ps );
			onPaint(ps);
			EndPaint(hwnd, &ps);
		}
	case WM_NCDESTROY:
		hwnd = NULL;
		break;
	case WM_CAPTURECHANGED:
		onCaptureChanged(lp);
		break;
	}
	return DefWindowProc(hwnd, msg, wp, lp );
}

//	NOT USED IN CHEWING IME
void XPToolbar::unregisterClass()
{
	UnregisterClass(xpToolbarClass, (HINSTANCE)GetModuleHandle(NULL));
}

void XPToolbar::onPaint(PAINTSTRUCT &ps)
{
    if ( IsWindow(hwnd)==FALSE ) return;
	RECT rc;
	GetClientRect( hwnd, &rc );

	HDC memdc = CreateCompatibleDC( ps.hdc );
	HBITMAP membmp = CreateCompatibleBitmap( ps.hdc, rc.right, rc.bottom );
	HGDIOBJ oldobj = SelectObject( memdc, membmp );
	::DrawBitmap( memdc, themeBmp, 0, 0, gripperW, rc.bottom, 0, 0 );
	for( int i = 0; i < 3; ++i )
		drawBtn( memdc, i );
	::DrawBitmap( memdc, themeBmp, rc.right-4, 0, 4, rc.bottom, gripperW + btnW * 3, 0 );
	BitBlt( ps.hdc, 0, 0, rc.right, rc.bottom, memdc, 0, 0, SRCCOPY );
	SelectObject(memdc, oldobj);
	DeleteDC( memdc );

}

//	NOT USED IN CHEWING IME
bool XPToolbar::create(HWND parent, UINT id, LONG style, int x, int y, int w, int h)
{
	hwnd = CreateWindowEx( 0, xpToolbarClass, NULL, WS_CHILD|WS_VISIBLE, x, y, w, h, 
		parent, NULL, HINSTANCE(GetModuleHandle(NULL)), NULL);
	SetWindowLongPtr( hwnd, GWL_USERDATA, LONG_PTR(this));
	return !!hwnd;
}

bool XPToolbar::onLButtonDown(WPARAM wp, LPARAM lp)
{
    if ( IsWindow(hwnd)==FALSE ) return false;
	POINTS pt = MAKEPOINTS(lp);
	curPressedBtn = hitTest(pt.x, pt.y);
	if(curPressedBtn < 0)
		return false;

	SetCapture(hwnd);
	buttons[curPressedBtn].state = -1;
	HDC dc = GetDC(hwnd);
	drawBtn( dc, curPressedBtn );
	ReleaseDC(hwnd, dc);
	return true;
}

bool XPToolbar::onLButtonUp(WPARAM wp, LPARAM lp)
{
    if ( IsWindow(hwnd)==FALSE ) return false;
	if( curPressedBtn < 0 )
		return false;
	POINTS pt = MAKEPOINTS(lp);
	int idx = hitTest(pt.x, pt.y);
	if( idx >= 0 )
	{
		if(idx == prevHilightBtn )
		{
			HDC dc = GetDC(hwnd);
			buttons[prevHilightBtn].state = 1;
			drawBtn( dc, prevHilightBtn );
			ReleaseDC(hwnd, dc);
		}
		if( idx == curPressedBtn )
			PostMessage( cmdTarget, WM_COMMAND, buttons[idx].id, 0);
	}
	ReleaseCapture();
	prevHilightBtn = curPressedBtn = -1;
	return true;
}

bool XPToolbar::onMouseMove(WPARAM wp, LPARAM lp)
{
    if ( IsWindow(hwnd)==FALSE ) return false;
	POINTS pt = MAKEPOINTS(lp);
	int idx = hitTest(pt.x, pt.y);

	if( idx < 0 )
	{
		if( prevHilightBtn >=0 && prevHilightBtn != idx )
		{
			HDC dc = GetDC(hwnd);
			buttons[prevHilightBtn].state = 0;
			drawBtn( dc, prevHilightBtn );
			ReleaseDC(hwnd, dc);
			if( tooltip )
				tooltip->hideTip();
		}
		prevHilightBtn = -1;

		if( curPressedBtn < 0 )
		{
			ReleaseCapture();
			return false;
		}
		return true;
	}

	SetCursor( LoadCursor( NULL, IDC_ARROW ) );
	SetCapture(hwnd);

	if( idx == prevHilightBtn )
		return true;

	if( curPressedBtn == idx )
		buttons[idx].state = -1;
	else
		buttons[idx].state = ( curPressedBtn < 0 ? 1 : 0 );

	HDC dc = GetDC(hwnd);
	if( prevHilightBtn >=0 && prevHilightBtn != idx )
	{
		if( prevHilightBtn != idx )
		{
			buttons[prevHilightBtn].state = 0;
			drawBtn( dc, prevHilightBtn );
		}
	}

	if(tooltip && !buttons[idx].tooltip.empty() )
	{
		RECT rc;
		getBtnRect( idx, rc );
		POINT pt;	pt.x = rc.left;	pt.y = rc.bottom;
		ClientToScreen( hwnd, &pt );
		tooltip->showTip( pt.x, pt.y, buttons[idx].tooltip.c_str(), 6000 );
	}

	drawBtn( dc, idx );
	prevHilightBtn = idx;
	ReleaseDC(hwnd, dc);
	return true;
}

void XPToolbar::setTheme(HBITMAP bmp)
{
    if ( IsWindow(hwnd)==FALSE ) return;
	themeBmp = bmp;
}

void XPToolbar::drawBtn(HDC dc, int idx)
{
    if ( IsWindow(hwnd)==FALSE ) return;
	int left = gripperW + btnW * idx;
	int iconleft = left + (btnW-16) / 2;
	int icontop = (btnH-16)/2;
	int state = buttons[idx].state;
	int srcx;
	if( state == 0 )
		srcx = gripperW;
	else if( state < 0 )
	{
		srcx = gripperW + btnW * 2;
		++iconleft;
		++icontop;
	}
	else
		srcx = gripperW + btnW;

	::DrawBitmap( dc, themeBmp, left, 0, btnW, btnH, srcx, 0 );

	DrawIconEx( dc, iconleft, icontop, buttons[idx].hIcon
				, 16, 16, 0, NULL, DI_NORMAL );
}

int XPToolbar::addBtn(UINT id, int iImage, LPCTSTR tooltip)
{
	buttons.push_back( XPToolbarBtn(id, iImage, tooltip, 0) );
	return buttons.size();
}

void XPToolbar::getSize(int *w, int *h)
{
    if ( IsWindow(hwnd)==FALSE ) return;
	*w = gripperW + btnW * buttons.size() + 4;
	*h = btnH;
}

int XPToolbar::hitTest(int x, int y)
{
    if ( IsWindow(hwnd)==FALSE ) return 0;
	RECT rc;
	GetClientRect(hwnd, &rc);
	int ymargin = (rc.bottom - 21)/2;
	if( y < (ymargin+1) || y > (rc.bottom - ymargin))
		return -1;
	if( x < gripperW || x >= (rc.right - 4) )
		return -1;
	return ((x-gripperW) / btnW);
}

void XPToolbar::setBtnImage(int idx, HICON icon)
{
    if ( IsWindow(hwnd)==FALSE ) return;
	if( buttons[idx].hIcon == icon )
		return;
	buttons[idx].hIcon = icon;
	HDC dc = GetDC(hwnd);
	drawBtn(dc, idx);
	ReleaseDC(hwnd, dc);
}

void XPToolbar::onCaptureChanged( LPARAM lp )
{
    if ( IsWindow(hwnd)==FALSE ) return;
	if( HWND(lp) != hwnd && prevHilightBtn >= 0 )
	{
		HDC dc = GetDC(hwnd);
		buttons[prevHilightBtn].state = 0;
		drawBtn( dc, prevHilightBtn );
		ReleaseDC(hwnd, dc);
		if( tooltip )
			tooltip->hideTip();
	}
}

void XPToolbar::setTooltip(Tooltip* tip)
{
	tooltip = tip;
}

bool XPToolbar::getBtnRect(int idx, RECT& rc)
{
	if( idx >= buttons.size() )
		return false;
	GetClientRect( hwnd, &rc );
	int ymargin = (rc.bottom - 21)/2;

	rc.left = gripperW + btnW * idx;
	rc.right = rc.left + btnW;
	rc.top = ymargin;
	rc.bottom = rc.top + btnH;
	return true;
}
