#include ".\tooltip.h"
#include <tchar.h>
#include "DrawUtil.h"

static LPCTSTR tooltipClass = _T("Tooltip");

Tooltip::Tooltip(void)
: timerID(0)
{
}

Tooltip::~Tooltip(void)
{
	if( timerID )
		KillTimer( hwnd, timerID );
}

BOOL Tooltip::registerClass(void)
{
	WNDCLASSEX wc;
	wc.cbSize         = sizeof(WNDCLASSEX);
	wc.style          = CS_HREDRAW|CS_VREDRAW;
	wc.lpfnWndProc    = (WNDPROC)Tooltip::wndProc;
	wc.cbClsExtra     = 0;
	wc.cbWndExtra     = sizeof(LONG) * (sizeof(void*) > sizeof(LONG) ? 2 : 1);
	wc.hInstance      = (HINSTANCE)GetModuleHandle(NULL);
	wc.hCursor        = LoadCursor( NULL, IDC_ARROW );
	wc.hIcon          = NULL;
	wc.lpszMenuName   = (LPTSTR)NULL;
	wc.lpszClassName  = tooltipClass;
	wc.hbrBackground  = HBRUSH(COLOR_INFOBK+1);
	wc.hIconSm        = NULL;

	if( !RegisterClassEx( (LPWNDCLASSEX)&wc ) )
		return FALSE;

	return TRUE;
}

void Tooltip::unregisterClass()
{
	UnregisterClass(tooltipClass, (HINSTANCE)GetModuleHandle(NULL));
}

LRESULT CALLBACK Tooltip::wndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	Tooltip* pthis = NULL;
	pthis = (Tooltip*)GetWindowLongPtr( hwnd, GWL_USERDATA );
	if( pthis )
		return pthis->wndProc(msg, wp, lp);
	return DefWindowProc(hwnd, msg, wp, lp );
}

LRESULT Tooltip::wndProc(UINT msg, WPARAM wp, LPARAM lp)
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
		break;
	case WM_TIMER:
		hideTip();
		break;
	case WM_MOUSEACTIVATE:
		return MA_NOACTIVATE;
	default:
		return DefWindowProc(hwnd, msg, wp, lp);
	}
	return 0;
}

void Tooltip::onPaint(PAINTSTRUCT& ps)
{
	int len = GetWindowTextLength( hwnd ) + 1;
	TCHAR* text = new TCHAR[len];
	len = GetWindowText( hwnd, text, len );
	RECT rc, textrc = {0};
	GetClientRect( hwnd, &rc );

	Draw3DBorder( ps.hdc, &rc, GetSysColor(COLOR_BTNFACE), GetSysColor(COLOR_3DDKSHADOW), 1);

	SetBkMode( ps.hdc, TRANSPARENT );
	SetTextColor(ps.hdc, GetSysColor(COLOR_INFOTEXT) );
	HGDIOBJ old_font = SelectObject( ps.hdc, GetStockObject(DEFAULT_GUI_FONT));

	DrawText( ps.hdc, text, len, &textrc, DT_CALCRECT|DT_NOPREFIX|DT_NOCLIP );
	rc.top += (rc.bottom - textrc.bottom)/2;
	rc.left += (rc.right - textrc.right)/2;
	DrawText( ps.hdc, text, len, &rc, DT_NOPREFIX|DT_NOCLIP );

	SelectObject( ps.hdc, old_font );
	delete []text;
}

void Tooltip::showTip(int x, int y, LPCTSTR text, DWORD duration)
{
	if( !isWindow() )
		create();

	SetWindowText(hwnd, text);
	RECT rc = {0};
	HDC dc = GetDC(hwnd);
	HGDIOBJ old_font = SelectObject( dc, GetStockObject(DEFAULT_GUI_FONT));
	DrawText( dc, text, _tcslen(text), &rc, DT_NOCLIP|DT_NOPREFIX|DT_CALCRECT );
	SelectObject( dc, old_font );
	ReleaseDC(hwnd, dc);

	SetWindowPos( hwnd, HWND_TOPMOST, x, y, rc.right + 4, rc.bottom + 4, SWP_NOACTIVATE );
	ShowWindow( hwnd, SW_SHOWNA );
	if( duration > 0 )
	{
		if(timerID)
			KillTimer( hwnd, timerID );
		timerID = SetTimer( hwnd, 1, duration, NULL );
	}
}

BOOL Tooltip::create(void)
{
	hwnd = CreateWindowEx( WS_EX_TOOLWINDOW, tooltipClass, NULL, WS_POPUP, 0, 0, 0, 0,
		HWND_DESKTOP, NULL, HINSTANCE(GetModuleHandle(NULL)), NULL);
	if( !hwnd )
		return FALSE;
	SetWindowLongPtr( hwnd, GWL_USERDATA, LONG_PTR(this));
	return TRUE;
}

void Tooltip::hideTip(void)
{
	if( timerID )
	{
		KillTimer( hwnd, timerID );
		timerID = 0;
	}
	hide();
}
