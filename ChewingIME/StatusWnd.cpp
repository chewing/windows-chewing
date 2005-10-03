#include "ChewingIME.h"
#include ".\statuswnd.h"
#include "DrawUtil.h"
#include "resource.h"

#include "CompStr.h"
#include "IMCLock.h"
#include "IMEData.h"

#include <commctrl.h>


//	SendMessage( toolbar, TB_GETMAXSIZE, 0, LPARAM(&sz));
//  This standard toolbar message provided by Windows has some known bugs.
//  So I implemented a new function myself to prevent the problem.
static void GetToolbarSize(HWND toolbar, SIZE *sz)
{
	sz->cx = sz->cy = 0;
	int c = SendMessage( toolbar, TB_BUTTONCOUNT, 0, 0);
	for( int i = 0; i < c ; ++i )
	{
		RECT itemrc;
		SendMessage( toolbar, TB_GETITEMRECT, i, LPARAM(&itemrc));
		sz->cx += (itemrc.right - itemrc.left);
		itemrc.bottom -= itemrc.top;
		if( itemrc.bottom > sz->cy )
			sz->cy = itemrc.bottom;
	}
}

static TBBUTTON toolbar_btns[]={
	{0, ID_CHI_ENG, TBSTATE_ENABLED ,TBSTYLE_BUTTON, {0}, 0, 0 },
	{3, ID_FULL_HALF, TBSTATE_ENABLED ,TBSTYLE_BUTTON, {0}, 0, 0 },
	{4, ID_CONFIG, TBSTATE_ENABLED ,TBSTYLE_BUTTON, {0}, 0, 0 },
};

StatusWnd::StatusWnd(void)
{
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
	wc.hCursor        = LoadCursor( NULL, IDC_SIZEALL );
	wc.hIcon          = NULL;
	wc.lpszMenuName   = (LPTSTR)NULL;
	wc.lpszClassName  = g_status_wnd_class;
	wc.hbrBackground  = NULL;
	wc.hIconSm        = NULL;

	if( !RegisterClassEx( (LPWNDCLASSEX)&wc ) )
		return FALSE;

	return TRUE;
}


LRESULT StatusWnd::WndProc( HWND hwnd, UINT msg, WPARAM wp , LPARAM lp )
{
	HIMC hIMC = getIMC(hwnd);
	IMCLock imc( hIMC );
	switch (msg)
	{
		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				BeginPaint( hwnd, &ps );
				g_statusWnd.OnPaint(ps);
				EndPaint(hwnd, &ps);
				break;
			}
		case WM_COMMAND:
			{
				DWORD conv, sentence;
				switch(LOWORD(wp))
				{
				case ID_CHI_ENG:
					{
						ImmGetConversionStatus( hIMC, &conv, &sentence);
						g_isChinese = !!(conv & IME_CMODE_NATIVE);
						if( g_isChinese )
							conv &= ~IME_CMODE_NATIVE;
						else
							conv |= IME_CMODE_NATIVE;
						ImmSetConversionStatus( hIMC, conv, sentence);

						g_isChinese = !g_isChinese;
						HWND toolbar = GetDlgItem( hwnd, IDC_STATUS_TB );
						SendMessage( toolbar, TB_CHANGEBITMAP, 
							ID_CHI_ENG, MAKELPARAM(g_isChinese ? 0 : 1, 0));

						if( ! LOBYTE(GetKeyState(VK_CAPITAL)) )
							g_chewing->Capslock();

						break;
					}
				case ID_FULL_HALF:
					{
						ImmGetConversionStatus( hIMC, &conv, &sentence);
						DWORD isFull = conv & IME_CMODE_FULLSHAPE;
						if( isFull )
							conv &= ~IME_CMODE_FULLSHAPE;
						else
							conv |= IME_CMODE_FULLSHAPE;
						ImmSetConversionStatus( hIMC, conv, sentence);
						isFull = !isFull;

						HWND toolbar = GetDlgItem( hwnd, IDC_STATUS_TB );
						SendMessage( toolbar, TB_CHANGEBITMAP, 
							ID_FULL_HALF, MAKELPARAM(isFull ? 2 : 3, 0));
						break;
					}
				case ID_CONFIG:
//					ConfigureChewingIME(HWND_DESKTOP);
					::ShellExecute( HWND_DESKTOP, "open", "rundll32.exe", "chewing.ime,ConfigureChewingIME", NULL, SW_SHOW );
					break;
				}
				break;
			}
		case WM_NOTIFY:
			{
				switch( ((NMHDR*)lp)->code ) 
				{
				case TTN_GETDISPINFO:
					{ 
						LPTOOLTIPTEXT pttt;
						pttt = (LPTOOLTIPTEXT)lp;
						pttt->hinst = g_dllInst;
						pttt->lpszText = MAKEINTRESOURCE(pttt->hdr.idFrom);
						break;
					}
				}
			}
			break;
		case WM_LBUTTONDOWN:
			g_statusWnd.OnLButtonDown(wp, lp);
			break;
		case WM_MOUSEMOVE:
			g_statusWnd.OnMouseMove(wp, lp);
			break;
		case WM_LBUTTONUP:
			g_statusWnd.OnLButtonUp(wp, lp);
			break;
		case WM_MOUSEACTIVATE:
			return MA_NOACTIVATE;
		case WM_DESTROY:
			{
				RECT rc;
				GetWindowRect( hwnd, &rc );
				g_statusWndPos.x = rc.left;
				g_statusWndPos.y = rc.top;
			}
			break;
		default:
			return DefWindowProc(hwnd, msg, wp, lp);
	}
	return 0;
}

void StatusWnd::OnPaint(PAINTSTRUCT& ps)
{
	RECT rc;
	GetClientRect(hwnd,&rc);

	FillSolidRect( ps.hdc, &rc, GetSysColor(COLOR_BTNFACE));

	Draw3DBorder( ps.hdc, &rc, GetSysColor(COLOR_BTNHILIGHT), 0/*GetSysColor(COLOR_BTNSHADOW)*/);

	InflateRect( &rc, -3, -3 );
	rc.left++;
	rc.right = rc.left + 3;
	Draw3DBorder( ps.hdc, &rc, GetSysColor(COLOR_BTNHILIGHT), GetSysColor(COLOR_BTNSHADOW));
	OffsetRect( &rc, 4, 0);
	Draw3DBorder( ps.hdc, &rc, GetSysColor(COLOR_BTNHILIGHT), GetSysColor(COLOR_BTNSHADOW));
}

void StatusWnd::getSize(int* w, int* h)
{
	int tw, th;
	getToolbarSize(&tw, &th);
	*w = tw + 12 + 4;
	*h = th + 6;
}

void StatusWnd::getToolbarSize(int* w, int* h)
{
	static SIZE sz = {-1, -1};
	if( sz.cx == -1 && sz.cy == -1 )
		::GetToolbarSize( toolbar, &sz );
	*w = sz.cx;
	*h = sz.cy;
}

bool StatusWnd::create(HWND imeUIWnd)
{
	HIMC hIMC = (HIMC)GetWindowLong( imeUIWnd, IMMGWL_IMC );
	IMCLock imc(hIMC);

	hwnd = CreateWindowEx(0, g_status_wnd_class, NULL,
					WS_POPUP|WS_CLIPCHILDREN,
					0, 0, 0, 0, imeUIWnd, NULL, g_dllInst, NULL);
	if( !hwnd )
		return false;

	toolbar_btns[0].iBitmap = g_isChinese ? 0 : 1;
	toolbar_btns[1].fsState = 0;	// Temporarily disable Fullshape
	toolbar_btns[0].fsState = 0;	// Temporarily disable ChiEng switch

	toolbar = CreateWindowEx( 0, TOOLBARCLASSNAME, NULL, 
		TBSTYLE_FLAT|TBSTYLE_TOOLTIPS/*|TBSTYLE_LIST*/|CCS_NODIVIDER|CCS_NORESIZE|
		WS_CHILD|WS_VISIBLE|CCS_NOPARENTALIGN, 
		0, 0, 0, 0, hwnd, NULL, g_dllInst, NULL);

	if( !toolbar )
		return false;

	SetWindowLong( toolbar, GWL_ID, IDC_STATUS_TB );

/*	HIMAGELIST himl = ImageList_Create( 16, 16, ILC_COLOR24|ILC_MASK, 5, 0);
	HBITMAP htbbmp = LoadBitmap( g_dllInst, LPCTSTR(IDB_STATUS_TB) );
	ImageList_AddMasked( himl, htbbmp, RGB(192, 192, 192) );
	ImageList_Add( himl, htbbmp, NULL );
	DeleteObject(htbbmp);
	himl = (HIMAGELIST)SendMessage( toolbar, TB_SETIMAGELIST, 0, LPARAM(himl));
	if( himl )
		ImageList_Destroy( himl );
*/
	SendMessage(toolbar, TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof(TBBUTTON), 0); 
	SendMessage( toolbar, TB_ADDBUTTONS, sizeof(toolbar_btns)/sizeof(TBBUTTON), LPARAM(toolbar_btns));
	TBADDBITMAP abm;
	abm.hInst = g_dllInst;
	abm.nID = IDB_STATUS_TB;
	SendMessage( toolbar, TB_ADDBITMAP, 5,LPARAM(&abm) );

	int w, h;
	getToolbarSize(&w, &h);
	SetWindowPos( toolbar, NULL, 12, 3, w, h, SWP_NOACTIVATE|SWP_NOZORDER );

	getSize(&w, &h);
	SetWindowPos( hwnd, NULL, 0, 0, w, h, SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER );

	return true;
}
