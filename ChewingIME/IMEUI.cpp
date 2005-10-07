#include ".\imeui.h"
#include "ChewingIME.h"
#include "IMCLock.h"
#include "IMEUILock.h"

#include "CompStr.h"

#include "CompWnd.h"
#include "CandWnd.h"
#include "StatusWnd.h"

#include <windows.h>

IMEUI::IMEUI(HWND hUIWnd) : hwnd(hUIWnd)
{
}

IMEUI::~IMEUI(void)
{
}

LRESULT IMEUI::onIMENotify( HIMC hIMC, WPARAM wp , LPARAM lp )
{
	switch(wp)
	{
	case IMN_CLOSESTATUSWINDOW:
		closeStatusWnd();
		break;
	case IMN_OPENSTATUSWINDOW:
		openStatusWnd(hIMC);
		break;
	case IMN_OPENCANDIDATE:
		openCandWnd();
		break;
	case IMN_CHANGECANDIDATE:
		updateCandWnd();
		break;
	case IMN_CLOSECANDIDATE:
		closeCandWnd();
		break;
	case IMN_SETCANDIDATEPOS:
		setCandWndPos(hIMC);
		break;
	case IMN_SETCONVERSIONMODE:
		statusWnd.updateIcons(hIMC);
		break;
	case IMN_SETSENTENCEMODE:
		break;
	case IMN_SETOPENSTATUS:
		break;
	case IMN_SETCOMPOSITIONFONT:
		{
			IMCLock imc(hIMC);
			if(imc.getIC())
				compWnd.setFont( (LOGFONT*)&imc.getIC()->lfFont );
		}
		break;
	case IMN_SETCOMPOSITIONWINDOW:
		{
	// The IMN_SETCOMPOSITIONWINDOW message is sent when the composition form of 
	// the Input Context is updated. When the UI window receives this message, 
	// the cfCompForm of the Input Context can be referenced to obtain the new 
	// conversion mode.

		}
		break;
	case IMN_GUIDELINE:
	// The IMN_GUIDELINE message is sent when an IME is about to show an error or 
	// information. When the application or UI window receives this message, either 
	// one can call ImmGetGuideLine to obtain information about the guideline.
		break;
	case IMN_SOFTKBDDESTROYED:
		break;
	}
	return 0;
}


LRESULT CALLBACK IMEUI::wndProc( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
{
	IMEUILock uilock(hwnd);
	IMEUI* ui = uilock.getIMEUI();
	if( ! ui )
		ui = uilock.createIMEUI();
	if( ui )
		return ui->wndProc( msg, wp, lp );
	return 0;
}

BOOL IMEUI::registerUIClasses()
{
	WNDCLASSEX wc;
	wc.cbSize			= sizeof(WNDCLASSEX);
	wc.style			= CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS| CS_IME;
	wc.lpfnWndProc		= (WNDPROC)IMEUI::wndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 2 * sizeof(LONG);
	wc.hInstance		= g_dllInst;
	wc.hCursor			= LoadCursor( NULL, IDC_ARROW );
	wc.hIcon			= NULL;
	wc.lpszMenuName		= (LPTSTR)NULL;
	wc.lpszClassName	= g_pcmanIMEClass;
	wc.hbrBackground	= NULL;
	wc.hIconSm			= NULL;
	if( !RegisterClassEx( (LPWNDCLASSEX)&wc ) )
		return FALSE;
	if( !CompWnd::registerClass() )
		return FALSE;
	if( !CandWnd::registerClass() )
		return FALSE;
	if( !StatusWnd::registerClass() )
		return FALSE;
	return TRUE;
}

LRESULT IMEUI::wndProc( UINT msg, WPARAM wp, LPARAM lp)
{
	HIMC hIMC = (HIMC)GetWindowLong(hwnd, IMMGWL_IMC);

	switch(msg)
	{
	case WM_IME_NOTIFY:
		onIMENotify( hIMC,wp, lp );
		break;
	case WM_IME_STARTCOMPOSITION:
		break;
	case WM_IME_COMPOSITION:
		{
			IMCLock imc(hIMC);
			if( !imc.getIC() )
				break;

			CompStr* cs = imc.getCompStr();

			if( !compWnd.isWindow() )
				compWnd.create(hwnd);

			if( lp & GCS_COMPSTR )
			{
				compWnd.refresh();

				POINT pt = imc.getIC()->cfCompForm.ptCurrentPos;
				if( 0 == imc.getIC()->cfCompForm.dwStyle )
				{
					RECT rc;
					if( GetCaretPos( &pt ) )
					{
					}
					else
					{
						SystemParametersInfo(SPI_GETWORKAREA, 0, (PVOID)&rc, 0 );
						pt.x = rc.left + 10;
						pt.y = rc.bottom -= 50;
					}
				}
				imc.getIC()->cfCompForm.ptCurrentPos = pt;
				ClientToScreen( imc.getIC()->hWnd, &pt );
				compWnd.move( pt.x, pt.y );

				if( ! cs->isEmpty() )
				{
					if( !compWnd.isVisible() )
						compWnd.show();
				}
				else
					compWnd.hide();
			}
			if( (lp & GCS_COMPSTR) || (lp & GCS_CURSORPOS) )
				if( compWnd.isVisible() )
					compWnd.refresh();
			break;
		}
	case WM_IME_ENDCOMPOSITION:
		{
			compWnd.hide();
			break;
		}
	case WM_IME_SETCONTEXT:
		{
			if( wp )
			{
				if( hIMC )
				{
					if( (lp & ISC_SHOWUICOMPOSITIONWINDOW)
					&& ! compWnd.getDisplayedCompStr().empty() )
						compWnd.show();
					statusWnd.show();
				}
				else
				{
					compWnd.hide();
					statusWnd.hide();
				}
			}
			else
			{
				compWnd.hide();
				statusWnd.hide();
			}
			break;
		}
	case WM_IME_RELOADCONFIG:
		LoadConfig();
		break;
	case WM_CREATE:
		{
			break;
		}
	case WM_NCDESTROY:
		{
			HGLOBAL hIMEUI = (HGLOBAL)GetWindowLong(hwnd, IMMGWL_PRIVATE);
			IMEUI* ui = (IMEUI*)GlobalLock(hIMEUI);
			if(ui)
				ui->~IMEUI();	// delete ui
			GlobalUnlock(hIMEUI);
			GlobalFree(hIMEUI);
		}
		break;
	default:
		if( !IsImeMessage(msg) )
			return DefWindowProc(hwnd, msg, wp, lp);
	}
	return 0;
}


void IMEUI::closeStatusWnd(void)
{
//	char log[100];
//	sprintf(log, "CLOSE: hwnd=%x, himc=%x", hwnd, hIMC );
	if( statusWnd.isWindow() )
		statusWnd.hide();
}

void IMEUI::openStatusWnd(HIMC hIMC)
{
	if( !statusWnd.isWindow() )
		statusWnd.create(hwnd);

	IMCLock imc(hIMC);
	INPUTCONTEXT* ic = imc.getIC();

	if( ic )
	{
		if( ic->ptStatusWndPos.x == -1 && ic->ptStatusWndPos.y == -1 )
		{
			RECT rc;
			SystemParametersInfo(SPI_GETWORKAREA, 0, (PVOID)&rc, 0 );
			int w, h;
			statusWnd.getSize(&w, &h);
			ic->ptStatusWndPos.x = rc.right - w;
			ic->ptStatusWndPos.y = rc.bottom - h - 32;
		}
		statusWnd.move( ic->ptStatusWndPos.x, ic->ptStatusWndPos.y );
	}
	statusWnd.show();
}

void IMEUI::openCandWnd(void)
{
	if( !candWnd.isWindow() )
		candWnd.create(hwnd);

	if( candWnd.isVisible() )
		candWnd.refresh();
	else
		candWnd.show();
}

void IMEUI::updateCandWnd(void)
{
	candWnd.refresh();
//	candWnd.updateSize();
	candWnd.show();
}

void IMEUI::closeCandWnd(void)
{
	candWnd.destroy();
}

void IMEUI::setCandWndPos(HIMC hIMC)
{
	IMCLock imc(hIMC);

	INPUTCONTEXT* ic = imc.getIC();
	if( ic )
		return;
	POINT pt = ic->cfCandForm[0].ptCurrentPos;
	switch( ic->cfCandForm[0].dwStyle )
	{
	case CFS_CANDIDATEPOS :
//		pt = ic->cfCandForm[0].ptCurrentPos;
		break;
	case CFS_DEFAULT:
		break;
	case CFS_EXCLUDE:
		{
			RECT &rc = ic->cfCandForm[0].rcArea;
			if( pt.x >= rc.left && pt.x <= rc.right )
				pt.x = rc.right;

			if( pt.y >= rc.top && pt.y <= rc.bottom )
				pt.x = rc.bottom;
			break;
		}
	}
	ClientToScreen(ic->hWnd, &pt);
}

bool IMEUI::getWorkingArea(RECT* rc)
{
	SystemParametersInfo(SPI_GETWORKAREA, 0, (PVOID)rc, 0 );

// Prepared to support mutiple monitor in the future.
/*	MONITORINFO mi;
	mi.cbSize = sizeof(mi);
	GetMonitorInfo(, &mi);
*/
	return true;
}
