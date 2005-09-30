// ChewingIME.cpp : 定義 DLL 應用程式的進入點。
//

#include "ChewingIME.h"

#include "CompStr.h"
#include "CandList.h"

#include "IMEUIWnd.h"
#include "CompWnd.h"
#include "CandWnd.h"
#include "StatusWnd.h"

#include "ChewingPP.h"
#include "resource.h"

#include <winreg.h>

HINSTANCE g_dllInst = NULL;

Chewing* g_chewing = NULL;
DWORD g_keyboardLayout = KB_DEFAULT;


CompWnd* g_compWnd = NULL;
CandWnd* g_candWnd = NULL;
StatusWnd* g_statusWnd = NULL;

POINT g_oldCompWndPos = {-1, -1};


void LoadConfig()
{
/*
	#define KB_TYPE_NUM 9
	#define KB_DEFAULT 0
	#define KB_HSU 1
	#define KB_IBM 2
	#define KB_GIN_YIEH 3
	#define KB_ET 4
	#define KB_ET26 5
	#define KB_DVORAK 6
	#define KB_DVORAK_HSU 7
	#define KB_HANYU_PINYING 8
*/

	HKEY hk = NULL;
	if( ERROR_SUCCESS == RegOpenKey( HKEY_CURRENT_USER, _T("Software\\ChewingIME"), &hk) )
	{
		DWORD size = sizeof(g_keyboardLayout);
		DWORD type = REG_DWORD;
		RegQueryValueEx( hk, "KeyboardLayout", 0, &type, (LPBYTE)&g_keyboardLayout, &size );
		g_chewing->SetKeyboardLayout( int(g_keyboardLayout) );
		RegCloseKey( hk );
	}
}


void SaveConfig()
{
	HKEY hk = NULL;
	if( ERROR_SUCCESS == RegCreateKeyEx( HKEY_CURRENT_USER, "Software\\ChewingIME", 0, 
			NULL, 0, KEY_ALL_ACCESS , NULL, &hk, NULL) )
	{
		RegSetValueEx( hk, _T("KeyboardLayout"), 0, REG_DWORD, (LPBYTE)&g_keyboardLayout, sizeof(g_keyboardLayout) );
		RegCloseKey( hk );
	}
}

BOOL ConfigDlgProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch( msg )
	{
	case WM_INITDIALOG:
		{
			CheckRadioButton( hwnd, IDC_KB1, IDC_KB8, IDC_KB1 + g_keyboardLayout );
		}
		return TRUE;
	case WM_COMMAND:
		switch( LOWORD(wp) )
		{
		case IDOK:
			{
				for( UINT id = IDC_KB1; id <= IDC_KB8; ++id )
				{
					if( IsDlgButtonChecked( hwnd, id) )
					{
						g_keyboardLayout = (id - IDC_KB1);
						SaveConfig();



						break;
					}
				}
				EndDialog(hwnd, IDOK);
			}
			break;
		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			break;
		}
		return TRUE;
		break;
	}
	return FALSE;
}

BOOL GenerateIMEMessage( HIMC hIMC, UINT msg, WPARAM wp, LPARAM lp )
{
	BOOL success = FALSE;
	INPUTCONTEXT* ic = ImmLockIMC(hIMC);
	HIMCC hbuf = ImmReSizeIMCC( ic->hMsgBuf, sizeof(TRANSMSG) * (ic->dwNumMsgBuf + 1) );
	if( hbuf )
	{
		ic->hMsgBuf = hbuf;
		TRANSMSG* pbuf = (TRANSMSG*)ImmLockIMCC( hbuf );
		if( pbuf )
		{
			pbuf[ic->dwNumMsgBuf].message = msg;
			pbuf[ic->dwNumMsgBuf].wParam = wp;
			pbuf[ic->dwNumMsgBuf].lParam = lp;
			ic->dwNumMsgBuf++;
			success = TRUE;
		}
		ImmUnlockIMCC(hbuf);
	}
	ImmUnlockIMC(hIMC);
	if( success )
		success = ImmGenerateMessage(hIMC);
	return success;
}


BOOL    APIENTRY ImeInquire(LPIMEINFO lpIMEInfo, LPTSTR lpszUIClass, LPCTSTR lpszOptions)
{
	_tcscpy( lpszUIClass, _T(g_pcman_ime_class) );
	lpIMEInfo->fdwConversionCaps = IME_CMODE_FULLSHAPE | IME_CMODE_NATIVE;
	lpIMEInfo->fdwSentenceCaps = IME_SMODE_NONE;
	lpIMEInfo->fdwUICaps = UI_CAP_2700;
	lpIMEInfo->fdwSCSCaps = 0;
	lpIMEInfo->fdwSelectCaps = SELECT_CAP_CONVERSION;

	return TRUE;
}

BOOL    APIENTRY ImeConfigure(HKL hkl, HWND hWnd, DWORD dwMode, LPVOID pRegisterWord)
{
	DialogBox( g_dllInst, LPCTSTR(IDD_CONFIG), hWnd, (DLGPROC)ConfigDlgProc );
	return TRUE;
}

DWORD   APIENTRY ImeConversionList(HIMC, LPCTSTR, LPCANDIDATELIST, DWORD dwBufLen, UINT uFlag)
{

	return 0;
}

BOOL    APIENTRY ImeDestroy(UINT)
{

	return TRUE;
}

LRESULT APIENTRY ImeEscape(HIMC, UINT, LPVOID)
{

	return FALSE;
}



BOOL    APIENTRY ImeProcessKey(HIMC hIMC, UINT uVirKey, LPARAM lParam, LPCBYTE lpbKeyState
)
{
	INPUTCONTEXT* ic = ImmLockIMC( hIMC );

	if( GetKeyInfo(lParam).isKeyUp )	// Key up
		return FALSE;

	if( IsKeyDown( lpbKeyState[VK_CONTROL]) && LOWORD(uVirKey) == VK_SPACE )
		return TRUE;

	int old_cursor_pos = g_chewing->CursorPos();

	BOOL ret = ProcessKeyEvent( uVirKey, GetKeyInfo(lParam), lpbKeyState );
	if( !ret )
		return FALSE;

	// Candidate list opened
	int pageCount = 0;
	if( pageCount = g_chewing->Candidate() )
	{
		CandList* candList = (CandList*)ImmLockIMCC( ic->hCandInfo );
		candList->setPageStart( g_chewing->CurrentPage() * g_chewing->ChoicePerPage() );
		candList->setPageSize( g_chewing->ChoicePerPage() );

		candList->setTotalCount( g_chewing->TotalChoice() );
		if(candList->getPageStart() == 0)
		{
			for( int i = 0; i < g_chewing->TotalChoice(); ++i )
			{
				char* cand =  g_chewing->Selection( i );
				candList->setCand( i , cand );
				free(cand);
			}
		}
		g_candWnd->updateSize();
		g_candWnd->refresh();
		if( ! g_candWnd->isVisible() )
			g_compWnd->showCand();
		ImmUnlockIMCC( ic->hCandInfo );
		ImmUnlockIMC( hIMC );
		return TRUE;
	}
	else if( g_candWnd->isVisible() )
		g_candWnd->Hide();

	CompStr* cs = (CompStr*)ImmLockIMCC( ic->hCompStr);
	if( cs->isEmpty() && ret )	// No composition string
		GenerateIMEMessage( hIMC, WM_IME_STARTCOMPOSITION );

	if( g_chewing->CommitReady() )
	{
		char* cstr = g_chewing->CommitStr(0);
		cs->setResultStr(cstr);
		cs->setCompStr("");
		GenerateIMEMessage( hIMC, WM_IME_COMPOSITION, 0, GCS_RESULTSTR );
	}

	string comp_str;
	if( g_chewing->BufferLen() )
	{
		char* chibuf = g_chewing->Buffer();
		comp_str = chibuf;
		free(chibuf);
	}

	const TCHAR* pcompstr = cs->getCompStr();
	for( int i = 0; i < old_cursor_pos && *pcompstr; ++i )
		pcompstr = _tcsinc(pcompstr);
	old_cursor_pos = int(pcompstr - cs->getCompStr());

	char* zuin = g_chewing->ZuinStr();
	if( *zuin )
	{
		comp_str.insert( old_cursor_pos, zuin );
		free(zuin);
	}

	if( comp_str != cs->getCompStr() )
	{
		cs->setCompStr( comp_str.c_str() );
		GenerateIMEMessage( hIMC, WM_IME_COMPOSITION, 0, GCS_COMPSTR );
	}

	int cursorpos = g_chewing->CursorPos();
	pcompstr = cs->getCompStr();
	for( int i = 0; i < cursorpos && *pcompstr; ++i )
		pcompstr = _tcsinc(pcompstr);
	cursorpos = int(pcompstr - cs->getCompStr());

	if( cursorpos != old_cursor_pos )
	{
		cs->setCursorPos( cursorpos );
		GenerateIMEMessage( hIMC, WM_IME_COMPOSITION, 0, GCS_CURSORPOS );
	}

	if( cs->isEmpty() )
		GenerateIMEMessage( hIMC, WM_IME_ENDCOMPOSITION );

	ImmUnlockIMCC( ic->hCompStr );
	ImmUnlockIMC( hIMC );

	return TRUE;
}

BOOL    APIENTRY ImeSelect(HIMC hIMC, BOOL fSelect)
{
	ImmSetOpenStatus( hIMC, fSelect );

	INPUTCONTEXT* ic = ImmLockIMC( hIMC );
	if(fSelect)
	{
//		if( ic->cfCompForm.ptCurrentPos.x == 0 && ic->cfCompForm.ptCurrentPos.y == 0)
//			ic->cfCompForm.ptCurrentPos = g_oldCompWndPos;

		ImmReSizeIMCC( ic->hCompStr, sizeof(CompStr) );
		CompStr* cs = (CompStr*)ImmLockIMCC( ic->hCompStr );
		cs = new (cs) CompStr;	// placement new
		ImmUnlockIMCC( ic->hCompStr );

		ImmReSizeIMCC( ic->hCandInfo, sizeof(CandList) );
		CandList* cl = (CandList*)ImmLockIMCC( ic->hCandInfo );
		cl = new (cl) CandList;	// placement new
		ImmUnlockIMCC( ic->hCandInfo );
	}
	else
	{
		CompStr* cs = (CompStr*)ImmLockIMCC( ic->hCompStr);
		if( ! cs->isEmpty() )	// Has composition string
		{
			// Ugly hack!
			g_chewing->Enter();
			char* cstr = g_chewing->CommitStr(0);
//			cs->setCompStr("");
//			GenerateIMEMessage( hIMC, WM_IME_COMPOSITION, 0, GCS_COMPSTR );
			cs->setResultStr(cstr);
			GenerateIMEMessage( hIMC, WM_IME_COMPOSITION, 0, GCS_RESULTSTR );
			GenerateIMEMessage( hIMC, WM_IME_ENDCOMPOSITION );
		}
		cs->~CompStr();	// delete cs;
		ImmUnlockIMCC( ic->hCompStr );
		CandList* cl = (CandList*)ImmLockIMCC( ic->hCandInfo );
		cl->~CandList();	// delete cl;
		ImmUnlockIMCC( ic->hCandInfo );

	}
	ImmUnlockIMC( hIMC );
	return TRUE;
}

//  Activates or deactivates an input context and notifies the IME of the newly active input context. 
//  The IME can use the notification for initialization.
BOOL    APIENTRY ImeSetActiveContext(HIMC hIMC, BOOL fFlag)
{
	if( fFlag )
	{
	}
	return TRUE;
}

UINT    APIENTRY ImeToAsciiEx(UINT uVirtKey, UINT uScaCode, CONST LPBYTE lpbKeyState, LPDWORD lpdwTransBuf, UINT fuState, HIMC)
{

	return TRUE;
}

BOOL    APIENTRY NotifyIME(HIMC, DWORD, DWORD, DWORD)
{
	return TRUE;
}

BOOL    APIENTRY ImeRegisterWord(LPCTSTR, DWORD, LPCTSTR)
{
	return 0;
}

BOOL    APIENTRY ImeUnregisterWord(LPCTSTR, DWORD, LPCTSTR)
{
	return 0;
}

UINT    APIENTRY ImeGetRegisterWordStyle(UINT nItem, LPSTYLEBUF)
{
	return 0;
}

DWORD WINAPI  ImeGetImeMenuItems(  HIMC  hIMC,  DWORD  dwFlags,  DWORD  dwType,
    LPIMEMENUITEMINFO  lpImeParentMenu, LPIMEMENUITEMINFO  lpImeMenu, DWORD  dwSize )
{
	return 0;
}

UINT    APIENTRY ImeEnumRegisterWord(REGISTERWORDENUMPROC, LPCTSTR, DWORD, LPCTSTR, LPVOID)
{
	return 0;
}

BOOL    APIENTRY ImeSetCompositionString(HIMC, DWORD dwIndex, LPCVOID lpComp, DWORD, LPCVOID lpRead, DWORD)
{
	return FALSE;
}


LRESULT OnImeNotify( INPUTCONTEXT *ic, HWND hwnd, WPARAM wp , LPARAM lp )
{
	switch(wp)
	{
	case IMN_CLOSESTATUSWINDOW:
		delete g_statusWnd;
		g_statusWnd = NULL;
		break;
	case IMN_OPENSTATUSWINDOW:
		g_statusWnd = new StatusWnd( hwnd );
		g_statusWnd->Show();
		break;
	case IMN_OPENCANDIDATE:
		g_candWnd = new CandWnd(hwnd);
//		ImmGetCandidateCount();
//		ImmGetCandidateList();
		//Display candidate list
		break;
	case IMN_CHANGECANDIDATE:
	// The IMN_CHANGECANDIDATE message is sent when an IME is about to change the 
	// content of a candidate window. An application then processes this message to 
	// display the candidate window itself. */
	// lParam = lCandidateList;

		break;
	case IMN_CLOSECANDIDATE:
		delete g_candWnd;
		g_candWnd = NULL;
		break;
	case IMN_SETCONVERSIONMODE:
		break;
	case IMN_SETOPENSTATUS:
		break;
	case IMN_SETCANDIDATEPOS:
		{
			POINT pt = ic->cfCandForm[0].ptCurrentPos;
			pt.y = ic->cfCandForm[0].rcArea.bottom;
			ClientToScreen(ic->hWnd, &pt);
			g_candWnd->Move(pt.x, pt.y);
			// lParam = lCandidateList;
			break;
		}
	case IMN_SETCOMPOSITIONFONT:
		{
			g_compWnd->setFont( (LOGFONT*)&ic->lfFont );
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


LRESULT CALLBACK UIWndProc( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
{
	HIMC hIMC = (HIMC)GetWindowLong(hwnd, IMMGWL_IMC);
	INPUTCONTEXT* ic = ImmLockIMC( hIMC );
	switch(msg)
	{
	case WM_IME_NOTIFY:
		OnImeNotify( ic, hwnd, wp, lp );
		break;
	case WM_IME_STARTCOMPOSITION:
		break;
	case WM_IME_COMPOSITION:
		{
			CompStr* cs = (CompStr*)ImmLockIMCC( ic->hCompStr );

			if( lp & GCS_COMPSTR )
				g_compWnd->setCompStr( cs->getCompStr() );

			if( lp & GCS_CURSORPOS )
			{
				g_compWnd->setCursorPos( cs->getCursorPos() );
				g_compWnd->refresh();
			}

//			switch( ic->cfCompForm.dwStyle )
//			{
//			case CFS_DEFAULT:
//			case CFS_FORCE_POSITION:
//			case CFS_POINT:
//			case CFS_RECT:
//				break;
//			}

			POINT pt = ic->cfCompForm.ptCurrentPos;
			if( 0 == ic->cfCompForm.dwStyle )
			{
				RECT rc;
				if( GetCaretPos( &pt ) )
				{
//					GetWindowRect( g_compWnd->getHwnd(), &rc );
//					pt.y -= (rc.bottom - rc.top);
				}
				else
				{
					SystemParametersInfo(SPI_GETWORKAREA, 0, (PVOID)&rc, 0 );
					pt.x = rc.left + 10;
					pt.y = rc.bottom -= 50;
				}
			}

			ClientToScreen( ic->hWnd, &pt );
			g_compWnd->Move( pt.x, pt.y );

			ImmUnlockIMCC( ic->hCompStr );

			if( !g_compWnd->isVisible() )
				g_compWnd->Show();

			break;
		}
	case WM_IME_ENDCOMPOSITION:
//		if( ic->cfCompForm.ptCurrentPos.x || ic->cfCompForm.ptCurrentPos.y )
//			g_oldCompWndPos = ic->cfCompForm.ptCurrentPos;
		g_compWnd->Hide();
		break;
	case WM_IME_SETCONTEXT:

		break;
	case WM_CREATE:
		{
			g_compWnd = new CompWnd(hwnd);
			g_candWnd = new CandWnd(hwnd);
			break;
		}
	case WM_DESTROY:
		if( g_compWnd )
			delete g_compWnd;
		if( g_compWnd )
			delete g_candWnd;
		if( g_statusWnd )
			delete g_statusWnd;
		return 0;
		break;
	}
	ImmUnlockIMC( hIMC );

	if( IsImeMessage(msg) )
		return 0;

	return DefWindowProc(hwnd, msg, wp, lp);
}

BOOL RegisterUIClasses()
{
	WNDCLASSEX wc;
	wc.cbSize			= sizeof(WNDCLASSEX);
	wc.style			= CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS| CS_IME;
	wc.lpfnWndProc		= UIWndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 2 * sizeof(LONG);
	wc.hInstance		= g_dllInst;
	wc.hCursor			= LoadCursor( NULL, IDC_ARROW );
	wc.hIcon			= NULL;
	wc.lpszMenuName		= (LPTSTR)NULL;
	wc.lpszClassName	= g_pcman_ime_class;
	wc.hbrBackground	= NULL;
	wc.hIconSm			= NULL;
	if( !RegisterClassEx( (LPWNDCLASSEX)&wc ) )
		return FALSE;
	if( !CompWnd::RegisterClass() )
		return FALSE;
	if( !CandWnd::RegisterClass() )
		return FALSE;
	if( !StatusWnd::RegisterClass() )
		return FALSE;
	return TRUE;
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  dwReason, 
                       LPVOID lpReserved
					 )
{
	switch(dwReason)
	{
	case DLL_PROCESS_ATTACH:
		{
			DisableThreadLibraryCalls((HMODULE)hModule);
			g_dllInst = (HINSTANCE)hModule;

			if( !RegisterUIClasses() )
				return FALSE;

			TCHAR chewingdir[MAX_PATH];
			GetSystemDirectory( chewingdir, MAX_PATH );
			_tcscat( chewingdir, _T("\\IME\\Chewing") );
			g_chewing = new Chewing( chewingdir, chewingdir );

			LoadConfig();

			break;
		}

	case DLL_PROCESS_DETACH:
		UnregisterClass(g_pcman_ime_class, (HINSTANCE)hModule);
		UnregisterClass(g_cand_wnd_class, (HINSTANCE)hModule);
		UnregisterClass(g_comp_wnd_class, (HINSTANCE)hModule);
		UnregisterClass(g_status_wnd_class, (HINSTANCE)hModule);
		if( g_chewing )
			delete g_chewing;

		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
    return TRUE;
}

BOOL ProcessKeyEvent( UINT key, KeyInfo ki, const BYTE* keystate )
{
	switch( key )
	{
	case VK_LEFT:
		if( IsKeyDown( keystate[VK_SHIFT] ) )
			g_chewing->ShiftLeft();
		else
			g_chewing->Left();
		break;
	case VK_RIGHT:
		if( IsKeyDown( keystate[VK_SHIFT] ) )
			g_chewing->ShiftRight();
		else
			g_chewing->Right();
		break;
	case VK_UP:
		g_chewing->Up();
		break;
	case VK_DOWN:
		g_chewing->Down();
		break;
	case VK_PRIOR:
		break;
	case VK_NEXT:
		break;
	case VK_HOME:
		g_chewing->Home();
		break;
	case VK_END:
		g_chewing->End();
		break;
	case VK_BACK:
		g_chewing->Backspace();
		break;
	case VK_RETURN:
		g_chewing->Enter();
		break;
	case VK_ESCAPE:
		g_chewing->Esc();
		break;
	case VK_DELETE:
		g_chewing->Delete();
		break;
	case VK_TAB:
		g_chewing->Tab();
		break;
	case VK_CAPITAL:
		g_chewing->Capslock();
		break;
	default:
		{
			if( key == VK_SPACE && IsKeyDown( keystate[VK_SHIFT] ) )
			{
				g_chewing->ShiftSpace();
				break;
			}

			if( key >= '0' && key <= '9' &&  IsKeyDown( keystate[VK_CONTROL] ) )
				g_chewing->CtrlNum( key );
			else
			{
				char ascii[2];
				int ret = ToAscii( key, ki.scanCode, keystate, (LPWORD)ascii, 0);
				// Ctrl + Space will only be treated as 'Space', which is used to send buffer.
				// So switching back to English mode can commit the string.
				if( ret )
					key = ascii[0];

				if( key == VK_SPACE )
					g_chewing->Space();
				else
					g_chewing->Key( key );
			}
		}
	}
	return ! g_chewing->KeystrokeIgnore();
}

