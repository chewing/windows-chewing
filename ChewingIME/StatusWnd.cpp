#include "statuswnd.h"
#include "ChewingIME.h"

StatusWnd::StatusWnd( HWND imeUIWnd ) : IMEWnd(imeUIWnd, g_status_wnd_class)
{
	EnableWindow(hwnd, TRUE);
}

StatusWnd::~StatusWnd(void)
{
}

BOOL StatusWnd::RegisterClass(void)
{
	WNDCLASSEX wc;
	wc.cbSize         = sizeof(WNDCLASSEX);
	wc.style          = CS_IME;
	wc.lpfnWndProc    = (WNDPROC)StatusWnd::WndProc;
	wc.cbClsExtra     = 0;
	wc.cbWndExtra     = 0;
	wc.hInstance      = g_dllInst;
	wc.hCursor        = LoadCursor( NULL, IDC_ARROW );
	wc.hIcon          = NULL;
	wc.lpszMenuName   = (LPTSTR)NULL;
	wc.lpszClassName  = g_status_wnd_class;
	wc.hbrBackground  = NULL;
	wc.hIconSm        = NULL;

	if( !RegisterClassEx( (LPWNDCLASSEX)&wc ) )
		return FALSE;

	return TRUE;
}

LRESULT StatusWnd::WndProc(HWND hwnd , UINT msg, WPARAM wp , LPARAM lp)
{
	switch (msg)
	{
		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				BeginPaint( hwnd, &ps );
//				g_thisCompWnd->OnPaint(ps);
				EndPaint(hwnd, &ps);
				break;
			}
		case WM_ERASEBKGND:
			return TRUE;
			break;
		default:
			if (!IsImeMessage(msg))
				return DefWindowProc(hwnd, msg, wp, lp);
	}
	return 0;
}

void StatusWnd::OnPaint(PAINTSTRUCT& ps)
{

}
