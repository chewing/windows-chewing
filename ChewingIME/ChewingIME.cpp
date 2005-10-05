// ChewingIME.cpp : 定義 DLL 應用程式的進入點。
//

#include "ChewingIME.h"

#include "CompStr.h"
#include "CandList.h"
#include "IMCLock.h"

#include "resource.h"

#include <commctrl.h>
#include <winreg.h>
#include <shlobj.h>

#include "CompWnd.h"
#include "CandWnd.h"
#include "StatusWnd.h"

HINSTANCE g_dllInst = NULL;
bool g_isWindowNT = false;

bool g_isChinese = true;
bool g_isFullShape = false;
long g_shiftPressedTime = -1;

POINT g_statusWndPos = { -1, -1 };

Chewing* g_chewing = NULL;
DWORD g_keyboardLayout = KB_DEFAULT;
DWORD g_candPerRow = 4;
DWORD g_defaultEnglish = false;

CompWnd g_compWnd;
CandWnd g_candWnd;
StatusWnd g_statusWnd;

BOOL FilterKeyByChewing( UINT key, KeyInfo ki, const BYTE* keystate );

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
		RegQueryValueEx( hk, "CandPerRow", 0, &type, (LPBYTE)&g_candPerRow, &size );
		RegQueryValueEx( hk, "DefaultEnglish", 0, &type, (LPBYTE)&g_defaultEnglish, &size );
		RegCloseKey( hk );
	}
}


void SaveConfig()
{
	HKEY hk = NULL;
	if( ERROR_SUCCESS == RegCreateKeyEx( HKEY_CURRENT_USER, "Software\\ChewingIME", 0, 
			NULL, 0, KEY_ALL_ACCESS , NULL, &hk, NULL) )
	{
		RegSetValueEx( hk, _T("KeyboardLayout"), 0, REG_DWORD, (LPBYTE)&g_keyboardLayout, sizeof(DWORD) );
		RegSetValueEx( hk, _T("CandPerRow"), 0, REG_DWORD, (LPBYTE)&g_candPerRow, sizeof(DWORD) );
		RegSetValueEx( hk, _T("DefaultEnglish"), 0, REG_DWORD, (LPBYTE)&g_defaultEnglish, sizeof(DWORD) );
		RegCloseKey( hk );
	}
}

static BOOL ConfigDlgProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch( msg )
	{
	case WM_INITDIALOG:
		{
			HICON icon = LoadIcon(g_dllInst, LPCTSTR(IDI_ICON));
			SendMessage( hwnd, WM_SETICON, ICON_BIG, LPARAM(icon) );
			SendMessage( hwnd, WM_SETICON, ICON_SMALL, LPARAM(icon) );
			CheckRadioButton( hwnd, IDC_KB1, IDC_KB9, IDC_KB1 + g_keyboardLayout );

			HWND spin = GetDlgItem( hwnd, IDC_CAND_PER_ROW_SPIN );
			::SendMessage( spin, UDM_SETRANGE32, 1, 7 );
			::SendMessage( spin, UDM_SETPOS32, 0, g_candPerRow );
		}
		return TRUE;
	case WM_COMMAND:
		switch( LOWORD(wp) )
		{
		case IDOK:
			{
				for( UINT id = IDC_KB1; id <= IDC_KB9; ++id )
				{
					if( IsDlgButtonChecked( hwnd, id) )
					{
						g_keyboardLayout = (id - IDC_KB1);
						HWND spin = GetDlgItem( hwnd, IDC_CAND_PER_ROW_SPIN );
						g_candPerRow = ::SendMessage( spin, UDM_GETPOS32, 0, 0 );

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
	if(!hIMC)
		return FALSE;
	BOOL success = FALSE;
	INPUTCONTEXT* ic = ImmLockIMC(hIMC);
	if(!ic)
		return FALSE;
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
	_tcscpy( lpszUIClass, _T(g_pcmanIMEClass) );
	lpIMEInfo->fdwConversionCaps = IME_CMODE_ROMAN | IME_CMODE_FULLSHAPE | IME_CMODE_NATIVE;
	lpIMEInfo->fdwSentenceCaps = IME_SMODE_NONE;
	lpIMEInfo->fdwUICaps = UI_CAP_2700;
	lpIMEInfo->fdwSCSCaps = 0;
	lpIMEInfo->fdwSelectCaps = SELECT_CAP_CONVERSION;
	lpIMEInfo->fdwProperty = /*IME_PROP_IGNORE_UPKEYS|*/IME_PROP_AT_CARET|IME_PROP_KBD_CHAR_FIRST|
						#ifdef	UNICODE
							 IME_PROP_UNICODE|
						#endif
							 IME_PROP_CANDLIST_START_FROM_1|IME_PROP_COMPLETE_ON_UNSELECT
							 /*|IME_PROP_END_UNLOAD*/;
/*	if(g_isWindowsNT && (DWORD(lpszOptions) & IME_SYSTEMINFO_WINLOGON ))
	{
		// Configuration should be disabled.
	}
*/
	return TRUE;
}


static BOOL CALLBACK ReloadAllChewingInst(HWND hwnd, LPARAM lp)
{
	TCHAR tmp[12];
	GetClassName( hwnd, tmp, 11 );
	if( 0 == _tcscmp( tmp, g_pcmanIMEClass ) )
		SendMessage( hwnd, WM_IME_RELOADCONFIG, 0, 0 );
	return TRUE;
}

void DoConfigureChewingIME(HWND parent)
{
	TCHAR title[32];
	LoadString( g_dllInst, IDS_CONFIGTIELE, title, sizeof(title) );
	HWND dlg;
	if( dlg = FindWindow( NULL, title) )
		SetForegroundWindow( dlg );
	else
	{
		if( IDOK == DialogBox( g_dllInst, LPCTSTR(IDD_CONFIG), parent, (DLGPROC)ConfigDlgProc ) )
		{
			// Force all Chewing instances to reload
			EnumChildWindows( GetDesktopWindow(), ReloadAllChewingInst, NULL);
		}
	}
}

void WINAPI ConfigureChewingIME()
{
	DoConfigureChewingIME(HWND_DESKTOP);
}


BOOL    APIENTRY ImeConfigure(HKL hkl, HWND hWnd, DWORD dwMode, LPVOID pRegisterWord)
{
	DoConfigureChewingIME(hWnd);
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




BOOL    APIENTRY ImeProcessKey(HIMC hIMC, UINT uVirKey, LPARAM lParam, LPCBYTE lpbKeyState )
{
	if( !hIMC )
		return FALSE;
	IMCLock imc(hIMC);
	INPUTCONTEXT* ic = imc.getIC();
	if( !ic )
		return FALSE;

	if( GetKeyInfo(lParam).isKeyUp )	// Key up
	{
		if( g_shiftPressedTime > 0 )
		{
			if( (GetTickCount() - g_shiftPressedTime) <= 400 )
			{
				// Toggle Chinese/English mode.
				g_statusWnd.toggleChiEngMode(hIMC);
			}
			g_shiftPressedTime = -1;
		}
		return FALSE;
	}

	if( uVirKey == VK_SHIFT  )
	{
		if( ! IsKeyDown( lpbKeyState[VK_CONTROL] ) && g_shiftPressedTime < 0 )
		{
			g_shiftPressedTime = GetTickCount();
			return TRUE;
		}
	}
	else if( g_shiftPressedTime > 0 )
		g_shiftPressedTime = -1;

	// IME Toggle key : Ctrl + Space & Shift + Space
	if( LOWORD(uVirKey) == VK_SPACE && 
		(IsKeyDown( lpbKeyState[VK_CONTROL]) || IsKeyDown( lpbKeyState[VK_SHIFT])) )
		return TRUE;	// Eat the message

	BOOL ret = FilterKeyByChewing( uVirKey, GetKeyInfo(lParam), lpbKeyState );
	if( !ret )
		return FALSE;

	// Candidate list opened
	int pageCount = 0;
	if( pageCount = g_chewing->Candidate() )
	{
		CandList* candList = imc.getCandList();
//		DWORD old_page_start = candList->getPageStart();
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
		if( !g_candWnd.isWindow() || !g_candWnd.isVisible() )
			GenerateIMEMessage( hIMC, WM_IME_NOTIFY, IMN_OPENCANDIDATE, 0 );
		else /* if( old_page_start != candList->getPageStart() ) */
			GenerateIMEMessage( hIMC, WM_IME_NOTIFY, IMN_CHANGECANDIDATE, 0 );

		return TRUE;
	}
	else if( g_candWnd.isVisible() )
		GenerateIMEMessage( hIMC, WM_IME_NOTIFY, IMN_CLOSECANDIDATE, 0 );

	CompStr* cs = imc.getCompStr();
	if( cs->isEmpty() && ret )	// No composition string
		GenerateIMEMessage( hIMC, WM_IME_STARTCOMPOSITION );

	if( g_chewing->CommitReady() )
	{
		char* cstr = g_chewing->CommitStr();
		cs->setResultStr(cstr);
		free(cstr);
		cs->setCompStr("");
		cs->setZuin("");
		GenerateIMEMessage( hIMC, WM_IME_COMPOSITION, 0, GCS_RESULTSTR );
		cs->setResultStr("");
	}

	if( g_chewing->BufferLen() )
	{
		char* chibuf = g_chewing->Buffer();
		cs->setCompStr(chibuf);
		free(chibuf);
	}
	else
		cs->setCompStr("");

	int cursorpos = g_chewing->CursorPos();
	TCHAR* pcompstr = cs->getCompStr();
	for( int i = 0; i < cursorpos && *pcompstr; ++i )
		pcompstr = _tcsinc(pcompstr);
	cursorpos = int(pcompstr - cs->getCompStr());

	cs->setCursorPos( cursorpos );

	char* zuin = g_chewing->ZuinStr();
	if( *zuin )
	{
		cs->setZuin(zuin);
		free(zuin);
	}
	else
		cs->setZuin("");

	cs->beforeGenerateMsg();

	GenerateIMEMessage( hIMC, WM_IME_COMPOSITION, 
				*(WORD*)cs->getCompStr(),
				(GCS_COMPSTR|GCS_COMPATTR|GCS_COMPREADSTR|
				GCS_COMPREADATTR|GCS_CURSORPOS|GCS_DELTASTART) );

	if( cs->isEmpty() )
		GenerateIMEMessage( hIMC, WM_IME_ENDCOMPOSITION );

	return TRUE;
}

Chewing* LoadChewingEngine()
{
	TCHAR datadir[MAX_PATH];
	TCHAR hashdir[MAX_PATH];
	LPCTSTR phashdir = datadir;
	GetSystemDirectory( datadir, MAX_PATH );
	_tcscat( datadir, _T("\\IME\\Chewing") );

	LPITEMIDLIST pidl;
	if( S_OK == SHGetSpecialFolderLocation( NULL, CSIDL_APPDATA, &pidl ) )
	{
		SHGetPathFromIDList(pidl, hashdir);
		_tcscat( hashdir, _T("\\Chewing") );
		CreateDirectory( hashdir, NULL );
		phashdir = hashdir;
		IMalloc* palloc = NULL;
		if( NOERROR == SHGetMalloc(&palloc) )
			palloc->Free(pidl);
	}
	return new Chewing( datadir, hashdir, int(g_keyboardLayout) );
}

BOOL    APIENTRY ImeSelect(HIMC hIMC, BOOL fSelect)
{
	IMCLock imc( hIMC );
	if( !imc.getIC() )
		return FALSE;

	if(fSelect)
	{
		// FIXME: I don't know why this is needed, but without this
		//        action, the IME cannot work normally under Win 2000/XP.
		//        It seems that Windows 98/ME don't have this problem.		if( g_isWindowNT )
		imc.getIC()->fOpen = TRUE;

		ImmReSizeIMCC( imc.getIC()->hCompStr, sizeof(CompStr) );
		CompStr* cs = imc.getCompStr();
		if(!cs)
			return FALSE;
		cs = new (cs) CompStr;	// placement new

		ImmReSizeIMCC( imc.getIC()->hCandInfo, sizeof(CandList) );
		CandList* cl = imc.getCandList();
		if(!cl)
			return FALSE;
		cl = new (cl) CandList;	// placement new

	// Set Chinese or English mode
		DWORD conv, sentence;
		ImmGetConversionStatus( hIMC, &conv, &sentence);
//	BEGIN UGLY HACK
		if( g_isChinese )
		{
/*			if( g_chewing->ChineseMode() )
			{
				if(  LOBYTE(GetKeyState(VK_CAPITAL)) )
					g_chewing->Capslock();
			}
			else if( ! LOBYTE(GetKeyState(VK_CAPITAL)) )
				g_chewing->Capslock();
*/
			conv |= IME_CMODE_NATIVE;
		}
		else
		{
			conv &= ~IME_CMODE_NATIVE;
		}

//	END UGLY HACK
		ImmSetConversionStatus( hIMC, conv, sentence);
	}
	else
	{
		CompStr* cs = imc.getCompStr();
		cs->~CompStr();	// delete cs;
		CandList* cl = imc.getCandList();
		cl->~CandList();	// delete cl;
	}
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
	return FALSE;
}

BOOL CommitBuffer( IMCLock& imc )
{
	CompStr* cs = imc.getCompStr();
	if( !cs )
		return FALSE;

	if( !cs->isEmpty() )
	{
		// FIXME: If candidate window is opened, this
		//        will cause problems.
		g_chewing->Enter();	// Commit
		if( g_chewing->CommitReady() )
		{
			char* cstr = g_chewing->CommitStr();
			cs->setResultStr(cstr);
			free(cstr);
		}
		else
			cs->setResultStr( cs->getCompStr() );
		cs->setCompStr("");
		cs->setCursorPos(0);
		cs->setZuin("");

		GenerateIMEMessage( imc.getHIMC(), WM_IME_COMPOSITION, 
			0,
			(GCS_RESULTSTR|GCS_COMPSTR|GCS_COMPATTR|GCS_COMPREADSTR|
			GCS_COMPREADATTR|GCS_CURSORPOS|GCS_DELTASTART) );

		GenerateIMEMessage( imc.getHIMC(), WM_IME_ENDCOMPOSITION );
	}
	return TRUE;
}

BOOL    APIENTRY NotifyIME(HIMC hIMC, DWORD dwAction, DWORD dwIndex, DWORD dwValue )
{
	if( !hIMC )
		return FALSE;

	switch( dwAction )
	{
	case NI_OPENCANDIDATE:
		break;
	case NI_CLOSECANDIDATE:
		break;
	case NI_SELECTCANDIDATESTR:
		break;
	case NI_CHANGECANDIDATELIST:
		break;
	case NI_SETCANDIDATE_PAGESTART:
		break;
	case NI_SETCANDIDATE_PAGESIZE:
		break;
	case NI_CONTEXTUPDATED:
		{
			switch(dwValue)
			{
			case IMC_SETCANDIDATEPOS:
				break;
			case IMC_SETCOMPOSITIONFONT:
				break;
			case IMC_SETCOMPOSITIONWINDOW:
				break;
			case IMC_SETCONVERSIONMODE:
				{
					DWORD conv, sentence;
					ImmGetConversionStatus( hIMC, &conv, &sentence);
					g_isFullShape = !!(conv & IME_CMODE_FULLSHAPE);
					g_statusWnd.updateIcons();
					break;
				}
			case IMC_SETSENTENCEMODE:
				break;
			case IMC_SETOPENSTATUS :
				break;
			}
			break;
		}
	case NI_COMPOSITIONSTR:
		{
			IMCLock imc( hIMC );
			CompStr* cs = imc.getCompStr();
			if( !cs )
				return FALSE;
			switch( dwIndex )
			{
			case CPS_COMPLETE:
				return CommitBuffer( imc );
				break;
			case CPS_CONVERT:
				break;
			case CPS_CANCEL:
				cs->setCompStr("");
				cs->setZuin("");
				break;
			}
		}
		break;
	}
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


LRESULT OnImeNotify( HIMC hIMC, HWND hwnd, WPARAM wp , LPARAM lp )
{
	switch(wp)
	{
	case IMN_CLOSESTATUSWINDOW:
	 	{
			if( g_statusWnd.isWindow() )
				g_statusWnd.destroy();
			break;
		}
	case IMN_OPENSTATUSWINDOW:
		if( !g_statusWnd.isWindow() )
			g_statusWnd.create(hwnd);

		if( g_statusWndPos.x != 0xffffffff && g_statusWndPos.y != 0xffffffff )
			g_statusWnd.Move( g_statusWndPos.x, g_statusWndPos.y );
		else
		{
			RECT rc;
			SystemParametersInfo(SPI_GETWORKAREA, 0, (PVOID)&rc, 0 );
			int w, h;
			g_statusWnd.getSize(&w, &h);
			g_statusWnd.Move( rc.right - w, rc.bottom - h - 32 );
		}
		g_statusWnd.Show();
		break;
	case IMN_OPENCANDIDATE:
		if( !g_candWnd.isWindow() )
			g_candWnd.create(hwnd);

		g_candWnd.updateSize();

		if( g_candWnd.isVisible() )
			g_candWnd.refresh();
		else
			g_candWnd.show();
		break;
	case IMN_CHANGECANDIDATE:
//		g_candWnd.updateSize();
		g_candWnd.refresh();
		g_candWnd.show();
		// The IMN_CHANGECANDIDATE message is sent when an IME is about to change the 
		// content of a candidate window. An application then processes this message to 
		// display the candidate window itself. */
		// lParam = lCandidateList;
		break;
	case IMN_CLOSECANDIDATE:
		g_candWnd.destroy();
		break;
	case IMN_SETCANDIDATEPOS:
		{
			IMCLock imc(hIMC);

			INPUTCONTEXT* ic = imc.getIC();
			if( !ic )
				break;

			POINT pt = ic->cfCandForm[0].ptCurrentPos;
			switch( ic->cfCandForm[0].dwStyle )
			{
			case CFS_CANDIDATEPOS :
//				 pt = ic->cfCandForm[0].ptCurrentPos;
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
			break;
		}
	case IMN_SETCONVERSIONMODE:
			break;
	case IMN_SETSENTENCEMODE:
		break;
	case IMN_SETOPENSTATUS:
		break;
	case IMN_SETCOMPOSITIONFONT:
		{
			IMCLock imc(hIMC);
			if(imc.getIC())
				g_compWnd.setFont( (LOGFONT*)&imc.getIC()->lfFont );
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

	switch(msg)
	{
	case WM_IME_NOTIFY:
		OnImeNotify( hIMC, hwnd, wp, lp );
		break;
	case WM_IME_STARTCOMPOSITION:
		break;
	case WM_IME_COMPOSITION:
		{
			IMCLock imc(hIMC);
			if( !imc.getIC() )
				break;

			CompStr* cs = imc.getCompStr();

			if( !g_compWnd.isWindow() )
				g_compWnd.create(hwnd);

			if( lp & GCS_COMPSTR )
			{
				g_compWnd.refresh();

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
				g_compWnd.Move( pt.x, pt.y );

				if( ! cs->isEmpty() )
				{
					if( !g_compWnd.isVisible() )
						g_compWnd.Show();
				}
				else
					g_compWnd.Hide();
			}
			if( (lp & GCS_COMPSTR) || (lp & GCS_CURSORPOS) )
				if( g_compWnd.isVisible() )
					g_compWnd.refresh();
			break;
		}
	case WM_IME_ENDCOMPOSITION:
		{
			g_compWnd.Hide();
			break;
		}
	case WM_IME_SETCONTEXT:
		{
			if( wp )
			{
				if( hIMC )
				{
					if( (lp & ISC_SHOWUICOMPOSITIONWINDOW)
					&& ! g_compWnd.getDisplayedCompStr().empty() )
						g_compWnd.Show();
					g_statusWnd.Show();
				}
				else
				{
					g_compWnd.Hide();
					g_statusWnd.Hide();
				}
			}
			else
			{
				g_compWnd.Hide();
				g_statusWnd.Hide();
			}
			break;
		}
	case WM_IME_RELOADCONFIG:
		LoadConfig();
		break;
	case WM_CREATE:
		{
//			This line is only used to debug.
//			PostMessage( hwnd, WM_IME_NOTIFY, IMN_OPENSTATUSWINDOW, 0 );
			break;
		}
	case WM_DESTROY:
		break;
	default:
		if( !IsImeMessage(msg) )
			return DefWindowProc(hwnd, msg, wp, lp);
	}
	return 0;
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
	wc.lpszClassName	= g_pcmanIMEClass;
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

			g_isWindowNT = (GetVersion() < 0x80000000);

			INITCOMMONCONTROLSEX iccex;
			iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
			iccex.dwICC = ICC_BAR_CLASSES;
			InitCommonControlsEx(&iccex);

			if( !RegisterUIClasses() )
				return FALSE;

			LoadConfig();
			g_isChinese = ! g_defaultEnglish;

			break;
		}

	case DLL_PROCESS_DETACH:
		UnregisterClass(g_pcmanIMEClass, (HINSTANCE)hModule);
		UnregisterClass(g_cnadWndClass, (HINSTANCE)hModule);
		UnregisterClass(g_compWndClass, (HINSTANCE)hModule);
		UnregisterClass(g_statusWndClass, (HINSTANCE)hModule);

		if( g_chewing )
			delete g_chewing;

		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
    return TRUE;
}

BOOL FilterKeyByChewing( UINT key, KeyInfo ki, const BYTE* keystate )
{
	if( !g_chewing )
	{
		if( ! (g_chewing = LoadChewingEngine()) )
			return FALSE;
		if( ! g_isChinese )
			g_chewing->Capslock();	// switch to English mode
	}
	g_chewing->SetFullShape(g_isFullShape);

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
	case VK_SPACE:
		g_chewing->Space();
		break;
	case VK_CAPITAL:
//		if( g_isChinese )
//			g_chewing->Capslock();
//		else
			return FALSE;
		break;
//	case VK_NUMLOCK:
//		break;
	default:
		{
//			if( key == VK_SPACE && IsKeyDown( keystate[VK_SHIFT] ) )
//			{
//				g_chewing->ShiftSpace();
//				break;
//			}

			if( IsKeyDown( keystate[VK_CONTROL] ) )
			{
				if(  key >= '0' && key <= '9' )
					g_chewing->CtrlNum( key );
				else
					return FALSE;
			}
			else
			{
				char ascii[2];
				int ret = ToAscii( key, ki.scanCode, keystate, (LPWORD)ascii, 0);
				if( ret )
					key = ascii[0];
				else
					return FALSE;

				if( g_isChinese && IsKeyToggled( keystate[VK_CAPITAL] ) )
				{
					if( key >= 'A' && key <= 'Z' )
						key = tolower(key);
					else if( key >= 'a' && key <= 'z' )
						key = toupper( key );
				}

				if( key == VK_SPACE )
					g_chewing->Space();
				else
					g_chewing->Key( key );
			}
		}
	}
	return ! g_chewing->KeystrokeIgnore();
}


