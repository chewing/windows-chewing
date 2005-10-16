#include "ChewingIME.h"
#include ".\statuswnd.h"
#include "DrawUtil.h"
#include "resource.h"

#include "CompStr.h"
#include "IMCLock.h"
#include "IMEUI.h"
#include "IMEUILock.h"

#include "XPToolbar.h"

#include <commctrl.h>

BOOL AboutDlgProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	if( msg == WM_COMMAND )
	{
		EndDialog( hwnd, LOWORD(wp) );
		return TRUE;
	}
	return FALSE;
}

StatusWnd::StatusWnd() : XPToolbar(9, 20, 26), dragging(false)
{
}

StatusWnd::~StatusWnd(void)
{
	DestroyIcon( iconChi );
	DestroyIcon( iconEng );
	DestroyIcon( iconFull );
	DestroyIcon( iconHalf );
	DestroyIcon( iconConfig );
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
	IMEUILock lock( GetParent(hwnd) );
	IMEUI* ui = lock.getIMEUI();
	if( ui && ui->statusWnd.getHwnd() )
		return ui->statusWnd.wndProc( msg, wp, lp );
	return DefWindowProc(hwnd, msg, wp, lp);
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

	TCHAR bmppath[MAX_PATH];
	GetSystemDirectory( bmppath, MAX_PATH );
	_tcscat( bmppath, _T("\\IME\\Chewing\\statuswnd.bmp") );
	HBITMAP tbbmp = (HBITMAP)LoadImage( NULL, bmppath, IMAGE_BITMAP, 
		0, 0, LR_DEFAULTCOLOR|LR_LOADFROMFILE);

	this->setTheme(tbbmp);
	iconChi = (HICON)LoadImage( g_dllInst, LPCTSTR (IDI_CHI), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR );
	iconEng = (HICON)LoadImage( g_dllInst, LPCTSTR (IDI_ENG), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR  );
	iconFull = (HICON)LoadImage( g_dllInst, LPCTSTR (IDI_FULL), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR  );
	iconHalf = (HICON)LoadImage( g_dllInst, LPCTSTR (IDI_HALF), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR  );
	iconConfig = (HICON)LoadImage( g_dllInst, LPCTSTR (IDI_CONFIG), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR  );

	TCHAR tip[128];
	LoadString(g_dllInst, ID_CHI_ENG, tip, sizeof(tip) );
	addBtn( ID_CHI_ENG, imc.isChinese() ? iconChi : iconEng, tip );
	LoadString(g_dllInst, ID_FULL_HALF, tip, sizeof(tip) );
	addBtn( ID_FULL_HALF, imc.isFullShape() ? iconFull : iconHalf, tip );
	LoadString(g_dllInst, ID_MENU, tip, sizeof(tip) );
	addBtn( ID_MENU, iconConfig, tip );

	IMEUILock ui(GetParent(hwnd));
	setTooltip( &ui.getIMEUI()->tooltip );
	setCmdTarget(hwnd);

	int w, h;
	XPToolbar::getSize(&w, &h);
	SetWindowPos( hwnd, NULL, 0, 0, w, h, SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER );

	return true;
}

void StatusWnd::enableChiEng(bool enable)
{
	SendMessage( GetDlgItem( hwnd, IDC_STATUS_TB), TB_ENABLEBUTTON, ID_CHI_ENG, enable );
}

void StatusWnd::toggleChiEngMode(HIMC hIMC)
{
	ToggleChiEngMode(hIMC);
/// FIXME: icons should indicate whether capslock is toggled or not.
	updateIcons(hIMC);
}

void StatusWnd::toggleShapeMode(HIMC hIMC)
{
	ToggleFullShapeMode( hIMC );
	updateIcons(hIMC);
}

void StatusWnd::updateIcons(HIMC hIMC)
{
	IMCLock imc(hIMC);
	bool isChinese;
//	if( g_enableShift )
		isChinese = imc.isChinese();
//	else
//		isChinese = !LOBYTE(GetKeyState(VK_CAPITAL));
	this->setBtnImage( 0, isChinese ? iconChi : iconEng );
	this->setBtnImage( 1, imc.isFullShape() ? iconFull : iconHalf );
}

LRESULT StatusWnd::wndProc(UINT msg, WPARAM wp, LPARAM lp)
{
	HIMC hIMC = getIMC(hwnd);
	IMCLock imc( hIMC );

	switch (msg)
	{
		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				BeginPaint( hwnd, &ps );
				XPToolbar::onPaint(ps);
				EndPaint(hwnd, &ps);
				break;
			}
		case WM_COMMAND:
			onCommand( LOWORD(wp), hIMC );
			break;
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
			if( !XPToolbar::onLButtonDown(wp, lp) )
			{
				dragging = true;
				IMEChildWnd::onLButtonDown(wp, lp);
			}
			break;
		case WM_MOUSEMOVE:
			if( dragging || !XPToolbar::onMouseMove(wp, lp) )
				IMEChildWnd::onMouseMove(wp, lp);
			break;
		case WM_LBUTTONUP:
			if( dragging || !XPToolbar::onLButtonUp(wp, lp) )
			{
				dragging = false;
				IMEChildWnd::onLButtonUp(wp, lp);
			}
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
//				DeleteObject(tbbmp);
			}
			break;
		case WM_CAPTURECHANGED:
			onCaptureChanged(lp);
			break;

		default:
			return DefWindowProc(hwnd, msg, wp, lp);
	}
	return 0;
}

void StatusWnd::getSize(int *w, int *h)
{
	XPToolbar::getSize(w, h);
}

void StatusWnd::onCommand(UINT id, HIMC hIMC)
{
	switch( id )
	{
	case ID_CHI_ENG:
		toggleChiEngMode(hIMC);
		break;
	case ID_FULL_HALF:
		toggleShapeMode(hIMC);
		break;
	case ID_MENU:
		{
			HMENU menu = LoadMenu( g_dllInst, LPCTSTR(IDR_POPUP));
			HMENU popup = GetSubMenu( menu, 0 );
			RECT rc;
			GetWindowRect(hwnd, &rc);
			rc.left += 9;
			UINT flag;
			long y;
			if((rc.top * 2) > GetSystemMetrics(SM_CYSCREEN) )
			{
				y = rc.top;
				flag = TPM_BOTTOMALIGN|TPM_LEFTALIGN;
			}
			else
			{
				y = rc.bottom;
				flag = TPM_TOPALIGN|TPM_LEFTALIGN;
			}
			TrackPopupMenu( popup, flag, rc.left, y, 0, hwnd, NULL);
			DestroyMenu(menu);
			break;
		}
	case ID_CONFIG:
	case ID_ABOUT:
		{
			HWND top = hwnd;
			HWND desktop = GetDesktopWindow(), parent = NULL;
			while( (parent=GetParent(top)) != desktop && parent )
				top = parent;
			EnableWindow( hwnd, FALSE );
			if( id == ID_CONFIG )
				ConfigureChewingIME(top);
			else
				DialogBox(g_dllInst, LPCTSTR(IDD_ABOUT), top, (DLGPROC)AboutDlgProc);
			EnableWindow( hwnd, TRUE );
		}
		break;
	case ID_WEBSITE:
	case ID_BUGREPORT:
		{
			LPCTSTR url = _T("http://chewing.csie.net/");
			if( id == ID_BUGREPORT )
				url = _T("http://rt.openfoundry.org/Foundry/Project/Tracker/?Queue=271");
			ShellExecute( NULL, "open", url, NULL, NULL, SW_SHOWNORMAL );
			break;
		}
	}
}
