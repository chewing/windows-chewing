#include "ChewingIME.h"
#include ".\statuswnd.h"
#include "DrawUtil.h"
#include "resource.h"

#include "CompStr.h"
#include "IMCLock.h"
#include "IMEUI.h"
#include "IMEUILock.h"

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

BOOL StatusWnd::registerClass(void)
{
	WNDCLASSEX wc;
	wc.cbSize         = sizeof(WNDCLASSEX);
	wc.style          = CS_IME;
	wc.lpfnWndProc    = (WNDPROC)StatusWnd::wndProc;
	wc.cbClsExtra     = 0;
	wc.cbWndExtra     = 0;
	wc.hInstance      = g_dllInst;
	wc.hCursor        = LoadCursor( NULL, IDC_SIZEALL );
	wc.hIcon          = NULL;
	wc.lpszMenuName   = (LPTSTR)NULL;
	wc.lpszClassName  = g_statusWndClass;
	wc.hbrBackground  = NULL;
	wc.hIconSm        = NULL;

	if( !RegisterClassEx( (LPWNDCLASSEX)&wc ) )
		return FALSE;

	return TRUE;
}


LRESULT StatusWnd::wndProc( HWND hwnd, UINT msg, WPARAM wp , LPARAM lp )
{
	HIMC hIMC = getIMC(hwnd);
	IMCLock imc( hIMC );
	IMEUILock lock( GetParent(hwnd) );
	IMEUI* ui = lock.getIMEUI();

	switch (msg)
	{
		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				BeginPaint( hwnd, &ps );
				if( ui )
					ui->statusWnd.onPaint(ps);
				EndPaint(hwnd, &ps);
				break;
			}
		case WM_COMMAND:
			{
				switch(LOWORD(wp))
				{
				case ID_CHI_ENG:
					if( ui )
						ui->statusWnd.toggleChiEngMode(hIMC);
					break;
				case ID_FULL_HALF:
					if( ui )
						ui->statusWnd.toggleShapeMode(hIMC);
					break;
				case ID_CONFIG:
					{
						HWND top = ui->hwnd;
						HWND desktop = GetDesktopWindow(), parent = NULL;
						while( (parent=GetParent(top)) != desktop && parent )
							top = parent;

						ConfigureChewingIME(top);
					}
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
			if( ui )
				ui->statusWnd.onLButtonDown(wp, lp);
			break;
		case WM_MOUSEMOVE:
			if( ui )
				ui->statusWnd.onMouseMove(wp, lp);
			break;
		case WM_LBUTTONUP:
			if( ui )
				ui->statusWnd.onLButtonUp(wp, lp);
			break;
		case WM_MOUSEACTIVATE:
			return MA_NOACTIVATE;
		case WM_SHOWWINDOW:
			if( ! wp )
			{
				RECT rc;
				GetWindowRect( hwnd, &rc );
				INPUTCONTEXT* ic = imc.getIC();
				if( ic )
				{
					ic->ptStatusWndPos.x = rc.left;
					ic->ptStatusWndPos.y = rc.top;
				}
			}
			break;
		case WM_DESTROY:
			{
				RECT rc;
				GetWindowRect( hwnd, &rc );
				INPUTCONTEXT* ic = imc.getIC();
				if( ic )
				{
					ic->ptStatusWndPos.x = rc.left;
					ic->ptStatusWndPos.y = rc.top;
				}
			}
			break;
		default:
			return DefWindowProc(hwnd, msg, wp, lp);
	}
	return 0;
}

void StatusWnd::onPaint(PAINTSTRUCT& ps)
{
	RECT rc;
	GetClientRect(hwnd,&rc);

	FillSolidRect( ps.hdc, &rc, GetSysColor(COLOR_BTNFACE));

/*	HDC memdc = CreateCompatibleDC( ps.hdc );
	HGDIOBJ oldobj = SelectObject( memdc, bgbmp );
	StretchBlt( ps.hdc, 62, 0, 100, 26, memdc, 60, 0, 2, 26, SRCCOPY);
	BitBlt( ps.hdc, 0, 0, 62, 26, memdc, 0, 0, SRCCOPY);
	SelectObject( memdc, oldobj );
*/

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

	hwnd = CreateWindowEx(0, g_statusWndClass, NULL,
					WS_POPUP|WS_CLIPCHILDREN,
					0, 0, 0, 0, imeUIWnd, NULL, g_dllInst, NULL);
	if( !hwnd )
		return false;

	toolbar_btns[0].iBitmap = imc.isChinese() ? 0 : 1;
	toolbar_btns[1].iBitmap = imc.isFullShape() ? 2 : 3;

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

/*	HDC dc = GetDC(hwnd);
	bgbmp = LoadBitmap( g_dllInst, LPCTSTR(IDB_STATUSBG) );
	BYTE buf[8192];
	long len = GetBitmapBits( bgbmp, sizeof(buf), buf );
	SetBitmapBits(bgbmp, len,buf);
	ReleaseDC(hwnd, dc );
*/
	return true;
}

void StatusWnd::enableChiEng(bool enable)
{
	SendMessage( GetDlgItem( hwnd, IDC_STATUS_TB), TB_ENABLEBUTTON, ID_CHI_ENG, enable );
}

void StatusWnd::toggleChiEngMode(HIMC hIMC)
{
	ToggleChiEngMode(hIMC);
	updateIcons(hIMC);
}

void StatusWnd::toggleShapeMode(HIMC hIMC)
{
	ToggleFullShapeMode( hIMC );
	updateIcons(hIMC);
}

void StatusWnd::updateIcons(HIMC hIMC)
{
	HWND toolbar = GetDlgItem( hwnd, IDC_STATUS_TB );

	IMCLock imc(hIMC);

	SendMessage( toolbar, TB_CHANGEBITMAP, 
		ID_CHI_ENG, MAKELPARAM(imc.isChinese() ? 0 : 1, 0));

	SendMessage( toolbar, TB_CHANGEBITMAP, 
		ID_FULL_HALF, MAKELPARAM(imc.isFullShape() ? 2 : 3, 0));
}
