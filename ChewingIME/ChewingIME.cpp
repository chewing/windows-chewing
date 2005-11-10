// ChewingIME.cpp : 定義 DLL 應用程式的進入點。
//

#include "ChewingIME.h"

#include "CompStr.h"
#include "CandList.h"
#include "IMCLock.h"

#include "resource.h"

#include <winreg.h>
#include <shlobj.h>
#include <windowsx.h>

#include "CompWnd.h"
#include "CandWnd.h"
#include "StatusWnd.h"

#include "IMEUI.h"

HINSTANCE g_dllInst = NULL;
bool g_isWindowNT = false;

long g_shiftPressedTime = -1;

ChewingClient* g_chewing = NULL;

// Configuration
DWORD g_keyboardLayout = KB_DEFAULT;
DWORD g_candPerRow = 4;
DWORD g_defaultEnglish = false;
DWORD g_defaultFullSpace = false;
DWORD g_spaceAsSelection = false;
DWORD g_enableShift = true;
DWORD g_shiftCapital = false;
DWORD g_addPhraseForward = true;
DWORD g_hideStatusWnd = false;
DWORD g_fixCompWnd = false;
DWORD g_ColorCandWnd = true;
DWORD g_ColoredCompCursor = false;
DWORD g_AdvanceAfterSelection = true;
DWORD g_FontSize = DEF_FONT_SIZE;
DWORD g_selKeyType = 0;
DWORD g_selAreaLen = 50;

static const char* g_selKeys[]={
	"1234567890",
	"asdfghjkl;",
	"asdfzxcv89",
	"asdfjkl789",
	"aoeuhtn789",
    "1234qweras",
	NULL
};

#ifdef	UNICODE
	static const TCHAR* g_selKeyNameStrs[]={
		_T("1234567890"),
		_T("asdfghjkl;"),
		_T("asdfzxcv89"),
		_T("asdfjkl789"),
		_T("aoeuhtn789"),
		_T("1234qweras"),
		NULL
	};
	const TCHAR** g_selKeyNames = g_selKeyNameStrs;
#else
	const TCHAR** g_selKeyNames = g_selKeys;
#endif

BOOL FilterKeyByChewing( IMCLock& imc, UINT key, KeyInfo ki, const BYTE* keystate );

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
		DWORD size = sizeof(DWORD);
		DWORD type = REG_DWORD;
		RegQueryValueEx( hk, "KeyboardLayout", 0, &type, (LPBYTE)&g_keyboardLayout, &size );		
		RegQueryValueEx( hk, "CandPerRow", 0, &type, (LPBYTE)&g_candPerRow, &size );
		RegQueryValueEx( hk, "DefaultEnglish", 0, &type, (LPBYTE)&g_defaultEnglish, &size );
		RegQueryValueEx( hk, "DefaultFullSpace", 0, &type, (LPBYTE)&g_defaultFullSpace, &size );
		RegQueryValueEx( hk, "SpaceAsSelection", 0, &type, (LPBYTE)&g_spaceAsSelection, &size );
		RegQueryValueEx( hk, "EnableShift", 0, &type, (LPBYTE)&g_enableShift, &size );
		RegQueryValueEx( hk, "ShiftCapital", 0, &type, (LPBYTE)&g_shiftCapital, &size );
		RegQueryValueEx( hk, "AddPhraseForward", 0, &type, (LPBYTE)&g_addPhraseForward, &size );
		RegQueryValueEx( hk, "FixCompWnd", 0, &type, (LPBYTE)&g_fixCompWnd, &size );
		RegQueryValueEx( hk, "HideStatusWnd", 0, &type, (LPBYTE)&g_hideStatusWnd, &size );
		RegQueryValueEx( hk, "ColorCandWnd", 0, &type, (LPBYTE)&g_ColorCandWnd, &size );
		RegQueryValueEx( hk, "ColorCompCursor", 0, &type, (LPBYTE)&g_ColoredCompCursor, &size );
		RegQueryValueEx( hk, "AdvanceAfterSelection", 0, &type, (LPBYTE)&g_AdvanceAfterSelection, &size );
        RegQueryValueEx( hk, "DefFontSize", 0, &type, (LPBYTE)&g_FontSize, &size );
		RegQueryValueEx( hk, "SelKeyType", 0, &type, (LPBYTE)&g_selKeyType, &size );
		RegQueryValueEx( hk, "SelAreaLen", 0, &type, (LPBYTE)&g_selAreaLen, &size );
		RegCloseKey( hk );
	}

	if( g_selKeyType > ((sizeof(g_selKeys)/sizeof(char*))-1) )
		g_selKeyType = 0;

	if( g_selAreaLen > 55 || g_selAreaLen < 40 )
		g_selAreaLen = 40;

	if( g_chewing )
		g_chewing->SelKey( (char*)g_selKeys[g_selKeyType] );
    if ( g_chewing!=NULL )
        g_chewing->SetAdvanceAfterSelection((g_AdvanceAfterSelection!=0)?true: false);
    if ( g_FontSize>64 || g_FontSize<4 )
        g_FontSize = DEF_FONT_SIZE;
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
		RegSetValueEx( hk, _T("DefaultFullSpace"), 0, REG_DWORD, (LPBYTE)&g_defaultFullSpace, sizeof(DWORD) );
		RegSetValueEx( hk, _T("SpaceAsSelection"), 0, REG_DWORD, (LPBYTE)&g_spaceAsSelection, sizeof(DWORD) );
		RegSetValueEx( hk, _T("EnableShift"), 0, REG_DWORD, (LPBYTE)&g_enableShift, sizeof(DWORD) );
		RegSetValueEx( hk, _T("ShiftCapital"), 0, REG_DWORD, (LPBYTE)&g_shiftCapital, sizeof(DWORD) );
		RegSetValueEx( hk, _T("AddPhraseForward"), 0, REG_DWORD, (LPBYTE)&g_addPhraseForward, sizeof(DWORD) );
		RegSetValueEx( hk, _T("FixCompWnd"), 0, REG_DWORD, (LPBYTE)&g_fixCompWnd, sizeof(DWORD) );
		RegSetValueEx( hk, _T("HideStatusWnd"), 0, REG_DWORD, (LPBYTE)&g_hideStatusWnd, sizeof(DWORD) );
		RegSetValueEx( hk, _T("ColorCandWnd"), 0, REG_DWORD, (LPBYTE)&g_ColorCandWnd, sizeof(DWORD) );
		RegSetValueEx( hk, _T("ColorCompCursor"), 0, REG_DWORD, (LPBYTE)&g_ColoredCompCursor, sizeof(DWORD) );
		RegSetValueEx( hk, _T("AdvanceAfterSelection"), 0, REG_DWORD, (LPBYTE)&g_AdvanceAfterSelection, sizeof(DWORD) );
		RegSetValueEx( hk, _T("DefFontSize"), 0, REG_DWORD, (LPBYTE)&g_FontSize, sizeof(DWORD) );
		RegSetValueEx( hk, _T("SelKeyType"), 0, REG_DWORD, (LPBYTE)&g_selKeyType, sizeof(DWORD) );
		RegSetValueEx( hk, _T("SelAreaLen"), 0, REG_DWORD, (LPBYTE)&g_selAreaLen, sizeof(DWORD) );
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
			CheckDlgButton( hwnd, IDC_DEFAULT_ENG, g_defaultEnglish );
			CheckDlgButton( hwnd, IDC_DEFAULT_FS, g_defaultFullSpace );
			CheckDlgButton( hwnd, IDC_SPACESEL, g_spaceAsSelection );
			CheckDlgButton( hwnd, IDC_ENABLE_SHIFT, g_enableShift );
			CheckDlgButton( hwnd, IDC_SHIFT_CAPITAL, g_shiftCapital );
			CheckDlgButton( hwnd, IDC_ADD_PHRASE_FORWARD, g_addPhraseForward );
			CheckDlgButton( hwnd, IDC_HIDE_STATUSWND, g_hideStatusWnd );
			CheckDlgButton( hwnd, IDC_FIX_COMPWND, g_fixCompWnd );
			CheckDlgButton( hwnd, IDC_COLOR_CANDIDATE, g_ColorCandWnd );
			CheckDlgButton( hwnd, IDC_BLOCK_CURSOR, g_ColoredCompCursor );
			CheckDlgButton( hwnd, IDC_ADV_AFTER_SEL, g_AdvanceAfterSelection );

			HWND spin = GetDlgItem( hwnd, IDC_CAND_PER_ROW_SPIN );
			::SendMessage( spin, UDM_SETRANGE32, 1, 10 );
			::SendMessage( spin, UDM_SETPOS, 0, 
                           (LPARAM) MAKELONG ((short) g_candPerRow , 0));

			spin = GetDlgItem( hwnd, IDC_CAND_PER_PAGE_SPIN );
			::SendMessage( spin, UDM_SETRANGE32, 7, 10 );
			int cand_per_page = ( g_selAreaLen - 5 ) / ( 1 * 2 + 3 );
			::SendMessage( spin, UDM_SETPOS, 0, 
                           (LPARAM) MAKELONG ((short) cand_per_page , 0));

			spin = GetDlgItem( hwnd, IDC_FONT_SIZE_SPIN );
			::SendMessage( spin, UDM_SETRANGE32, 4, 64 );
			::SendMessage( spin, UDM_SETPOS, 0, 
                           (LPARAM) MAKELONG ((short) g_FontSize , 0));

			HWND combo = GetDlgItem( hwnd, IDC_SELKEYS );
			const TCHAR** pselkeys = g_selKeyNames;
			while( *pselkeys )
				ComboBox_AddString( combo, *(pselkeys++) );
			ComboBox_SetCurSel( combo, g_selKeyType );
		}
		return TRUE;
	case WM_COMMAND:
		switch( LOWORD(wp) )
		{
		case IDOK:
			{
				for( UINT id = IDC_KB1; id <= IDC_KB9; ++id )	{
					if( IsDlgButtonChecked( hwnd, id) )	{
						g_keyboardLayout = (id - IDC_KB1);
						break;
					}
				}
				HWND spin = GetDlgItem( hwnd, IDC_CAND_PER_ROW_SPIN );
				g_candPerRow = (DWORD)::SendMessage( spin, UDM_GETPOS, 0, 0 );
				if( g_candPerRow < 1 )	g_candPerRow = 1;
				if( g_candPerRow > 10 )	g_candPerRow = 10;

				spin = GetDlgItem( hwnd, IDC_CAND_PER_PAGE_SPIN );
				int cand_per_page = (int)::SendMessage( spin, UDM_GETPOS, 0, 0 );
				if( cand_per_page < 7 )	g_candPerRow = 7;
				if( cand_per_page > 10 )	g_candPerRow = 10;
				g_selAreaLen = cand_per_page * ( 1 * 2 + 3 ) + 5;

				spin = GetDlgItem( hwnd, IDC_FONT_SIZE_SPIN );
				int tFontSize = (int)::SendMessage( spin, UDM_GETPOS, 0, 0 );
				if( tFontSize < 4 )     tFontSize = 4;
				if( tFontSize > 64 )	tFontSize = 64;
				g_FontSize = tFontSize;

				g_defaultEnglish = IsDlgButtonChecked( hwnd, IDC_DEFAULT_ENG );
				g_defaultFullSpace = IsDlgButtonChecked( hwnd, IDC_DEFAULT_FS );
				g_spaceAsSelection = IsDlgButtonChecked( hwnd, IDC_SPACESEL );
				g_enableShift = IsDlgButtonChecked( hwnd, IDC_ENABLE_SHIFT );
				g_shiftCapital = IsDlgButtonChecked( hwnd, IDC_SHIFT_CAPITAL );
				g_addPhraseForward = IsDlgButtonChecked( hwnd, IDC_ADD_PHRASE_FORWARD );
				g_hideStatusWnd = IsDlgButtonChecked( hwnd, IDC_HIDE_STATUSWND );
				g_fixCompWnd = IsDlgButtonChecked( hwnd, IDC_FIX_COMPWND );
				g_ColorCandWnd = IsDlgButtonChecked( hwnd, IDC_COLOR_CANDIDATE );
				g_ColoredCompCursor = IsDlgButtonChecked( hwnd, IDC_BLOCK_CURSOR );
                g_AdvanceAfterSelection = IsDlgButtonChecked( hwnd, IDC_ADV_AFTER_SEL);

                if ( g_chewing!=NULL )
                    g_chewing->SetAdvanceAfterSelection((g_AdvanceAfterSelection!=0)?true: false);

				g_selKeyType = ComboBox_GetCurSel(GetDlgItem(hwnd, IDC_SELKEYS));
				if( g_selKeyType < 0 )
					g_selKeyType = 0;

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
			ImmUnlockIMCC(hbuf);
		}
	}
	ImmUnlockIMC(hIMC);

	if( success )
		success = ImmGenerateMessage(hIMC);

	return success;
}


BOOL    APIENTRY ImeInquire(LPIMEINFO lpIMEInfo, LPTSTR lpszUIClass, LPCTSTR lpszOptions)
{
	_tcscpy( lpszUIClass, _T(g_pcmanIMEClass) );
	lpIMEInfo->fdwConversionCaps = IME_CMODE_NOCONVERSION | IME_CMODE_FULLSHAPE | IME_CMODE_CHINESE;
	lpIMEInfo->fdwSentenceCaps = IME_SMODE_NONE;
	lpIMEInfo->fdwUICaps = UI_CAP_2700;
	lpIMEInfo->fdwSCSCaps = 0;
	lpIMEInfo->fdwSelectCaps = SELECT_CAP_CONVERSION;
	lpIMEInfo->fdwProperty = /*IME_PROP_IGNORE_UPKEYS|*/IME_PROP_AT_CARET|IME_PROP_KBD_CHAR_FIRST|
						#ifdef	UNICODE
							 IME_PROP_UNICODE|
						#endif
							 IME_PROP_CANDLIST_START_FROM_1|IME_PROP_COMPLETE_ON_UNSELECT
							 |IME_PROP_END_UNLOAD;
/*
	if(g_isWindowsNT && (DWORD(lpszOptions) & IME_SYSTEMINFO_WINLOGON ))
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

void ConfigureChewingIME(HWND parent)
{
//	TCHAR title[32];
//	LoadString( g_dllInst, IDS_CONFIGTIELE, title, sizeof(title) );
//	HWND dlg;
//	if( dlg = FindWindow( NULL, title) )
//		SetForegroundWindow( dlg );
//	else
	{
		if( IDOK == DialogBox( g_dllInst, LPCTSTR(IDD_CONFIG), parent, (DLGPROC)ConfigDlgProc ) )
		{
			SaveConfig();
			// Force all Chewing instances to reload
			EnumChildWindows( GetDesktopWindow(), ReloadAllChewingInst, NULL);
		}
	}
}


BOOL    APIENTRY ImeConfigure(HKL hkl, HWND hWnd, DWORD dwMode, LPVOID pRegisterWord)
{
	ConfigureChewingIME(hWnd);
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

void ToggleChiEngMode(HIMC hIMC)
{
	bool isChinese;
//	if( g_enableShift )
//	{
		DWORD conv, sentence;
		ImmGetConversionStatus( hIMC, &conv, &sentence);
		isChinese = !!(conv & IME_CMODE_CHINESE);
		if( isChinese )
			conv &= ~IME_CMODE_CHINESE;
		else
			conv |= IME_CMODE_CHINESE;
		ImmSetConversionStatus( hIMC, conv, sentence);
//	}
//	else
//	{
//		isChinese = !LOBYTE(GetKeyState(VK_CAPITAL));
//		BYTE scan = MapVirtualKey(VK_CAPITAL, 0);
//		keybd_event( VK_CAPITAL, MapVirtualKey(VK_CAPITAL, 0), 0, 0  );	// Capslock on/off
//		keybd_event( VK_CAPITAL, MapVirtualKey(VK_CAPITAL, 0), KEYEVENTF_KEYUP, 0 );	// Capslock on/off
//	}

	isChinese = !isChinese;

	if( g_chewing )
	{
		if( isChinese )	// We need Chinese mode
		{
			if( !g_chewing->ChineseMode() )	// Chewing is in English mode
				if( ! LOBYTE(GetKeyState(VK_CAPITAL)) )	// no CapsLock
					g_chewing->Capslock();	// Switch Chewing to Chinese mode
		}
		else	// We need English mode
		{
			if(  g_chewing->ChineseMode() )	// Chewing is in Chinese mode
				g_chewing->Capslock();	// Switch Chewing to English mode
		}
	}
}

void ToggleFullShapeMode(HIMC hIMC)
{
	DWORD conv, sentence;
	ImmGetConversionStatus( hIMC, &conv, &sentence);
	bool isFullShape = !!(conv & IME_CMODE_FULLSHAPE);
	if( isFullShape )
		conv &= ~IME_CMODE_FULLSHAPE;
	else
		conv |= IME_CMODE_FULLSHAPE;
	ImmSetConversionStatus( hIMC, conv, sentence);
}

BOOL ProcessCandidateList( HIMC hIMC, HIMCC hCandInfo )
{
	// Candidate list opened
	int pageCount = 0;
	CandList* candList = (CandList*)ImmLockIMCC(hCandInfo);
	if( !candList )
		return FALSE;
	if( pageCount = g_chewing->Candidate() )
	{
		candList->setPageStart( g_chewing->CurrentPage() * g_chewing->ChoicePerPage() );
		candList->setPageSize( g_chewing->ChoicePerPage() );

		int old_total_count = candList->getTotalCount();
		candList->setTotalCount( g_chewing->TotalChoice() );

		if(candList->getPageStart() == 0)
		{
			for( int i = 0; i < g_chewing->TotalChoice(); ++i )
			{
				char* cand =  g_chewing->Selection( i );
				if( cand )
				{
					candList->setCand( i , cand );
					free(cand);
				}
				else
					candList->setCand( i , "" );
			}
		}
		ImmUnlockIMCC( hCandInfo );

		if( 0 == old_total_count )
			GenerateIMEMessage( hIMC, WM_IME_NOTIFY, IMN_OPENCANDIDATE, 1 );
		GenerateIMEMessage( hIMC, WM_IME_NOTIFY, IMN_CHANGECANDIDATE, 1 );
		return TRUE;
	}
	else if( candList->getTotalCount() > 0 )
	{
		ImmUnlockIMCC( hCandInfo );
		GenerateIMEMessage( hIMC, WM_IME_NOTIFY, IMN_CLOSECANDIDATE, 1 );
		candList->setTotalCount(0);
	}

	return FALSE;
}


BOOL    APIENTRY ImeProcessKey(HIMC hIMC, UINT uVirKey, LPARAM lParam, CONST BYTE *lpbKeyState )
{
	if( !hIMC )
		return FALSE;

	if( GetKeyInfo(lParam).isKeyUp )	{	// Key up
		if( g_enableShift )		{
			if( g_shiftPressedTime > 0 )	{
				if( uVirKey == VK_SHIFT && (GetTickCount() - g_shiftPressedTime) <= 300 )
				{
					// Toggle Chinese/English mode.
					ToggleChiEngMode(hIMC);
				}
				g_shiftPressedTime = -1;
			}
		}
		return FALSE;
	}

	if( g_enableShift )	{
		if( uVirKey == VK_SHIFT  )	{
			if( ! IsKeyDown( lpbKeyState[VK_CONTROL] ) && g_shiftPressedTime < 0 )
				g_shiftPressedTime = GetTickCount();
		}
		else if( g_shiftPressedTime > 0 )
			g_shiftPressedTime = -1;
	}

    //  Is server alive? Or the server could be different one.
    if ( g_chewing != NULL )
        g_chewing->CheckServer();

	// IME Toggle key : Ctrl + Space & Shift + Space
	if( LOWORD(uVirKey) == VK_SPACE && 
		(IsKeyDown( lpbKeyState[VK_CONTROL]) || IsKeyDown( lpbKeyState[VK_SHIFT])) )
		return TRUE;	// Eat the message

	BOOL ret = FilterKeyByChewing( IMCLock(hIMC), uVirKey, GetKeyInfo(lParam), lpbKeyState );
	if( !ret )
		return FALSE;

	INPUTCONTEXT* ic = (INPUTCONTEXT*)ImmLockIMC(hIMC);
	if( ProcessCandidateList( hIMC, ic->hCandInfo ) )	{
		ImmUnlockIMC( hIMC );
		return TRUE;
	}

	CompStr* cs = (CompStr*)ImmLockIMCC(ic->hCompStr);
	bool composition_started = !!*cs->getCompStr();

	if( g_chewing->CommitReady() )	{
		char* cstr = g_chewing->CommitStr();
		if( cstr )	{
			cs->setResultStr(cstr);
			free(cstr);
		}
		else
			cs->setResultStr("");
	}
	else
		cs->setResultStr("");

	if( g_chewing->BufferLen() )	{
		char* chibuf = g_chewing->Buffer();
		if(chibuf)	{
			cs->setCompStr(chibuf);
			free(chibuf);
		}
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
	if( zuin )	{
		cs->setZuin(zuin);
		free(zuin);
	}
	else
		cs->setZuin("");

	cs->beforeGenerateMsg();

	WORD word = *(WORD*)cs->getCompStr();
	bool is_empty = !*cs->getCompStr();
	bool has_result = !!*cs->getResultStr();

	if( ! composition_started && *cs->getCompStr() )
	{
		GenerateIMEMessage( hIMC, WM_IME_STARTCOMPOSITION );
		composition_started = true;
	}

	ImmUnlockIMCC(ic->hCompStr);

	GenerateIMEMessage( hIMC, WM_IME_COMPOSITION, 
				(composition_started ? word : 0),
				(GCS_COMPSTR|GCS_COMPATTR|GCS_COMPREADSTR|GCS_COMPREADATTR|
				GCS_COMPCLAUSE|GCS_COMPREADCLAUSE|
				GCS_COMPREADATTR|GCS_CURSORPOS|
				(composition_started ? GCS_DELTASTART : 0 )|
				(has_result ? (GCS_RESULTCLAUSE|GCS_RESULTSTR|GCS_RESULTREADSTR|GCS_RESULTREADCLAUSE):0 ) ) );

	if( is_empty && composition_started )
		GenerateIMEMessage( hIMC, WM_IME_ENDCOMPOSITION );

	ImmUnlockIMC(hIMC);
	return TRUE;
}

ChewingClient* LoadChewingEngine()
{
	return new ChewingClient( int(g_keyboardLayout), 
		g_spaceAsSelection, g_selKeys[g_selKeyType], g_AdvanceAfterSelection);
}

BOOL    APIENTRY ImeSelect(HIMC hIMC, BOOL fSelect)
{
	IMCLock imc( hIMC );
	INPUTCONTEXT* ic = imc.getIC();
	if( !ic )
		return FALSE;

	if(fSelect)
	{
		ic->fOpen = TRUE;

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

		if( ! (ic->fdwInit & INIT_CONVERSION) )		// Initialize
		{
			ic->fdwConversion = g_defaultEnglish ? IME_CMODE_CHARCODE : IME_CMODE_CHINESE;

			if( g_defaultFullSpace )
				ic->fdwConversion |=  IME_CMODE_FULLSHAPE;
			else
				ic->fdwConversion &=  ~IME_CMODE_FULLSHAPE;
			ic->fdwInit |= INIT_CONVERSION;
		}
		if( ! (ic->fdwInit & INIT_STATUSWNDPOS) )
		{
			RECT rc;
			IMEUI::getWorkingArea( &rc, ic->hWnd );
			ic->ptStatusWndPos.x = rc.right - (9+20*3+4) - 150;
			ic->ptStatusWndPos.y = rc.bottom - 26;
			ic->fdwInit |= INIT_STATUSWNDPOS;
		}
		if( ! (ic->fdwInit & INIT_LOGFONT) )
		{
			// TODO: initialize font here
			ic->lfFont;
		}

		// Set Chinese or English mode
		if( imc.isChinese() )	//	Chinese mode
		{
			if( g_chewing )
			{
				if( g_chewing->ChineseMode() )
				{
					if(  LOBYTE(GetKeyState(VK_CAPITAL)) )
						g_chewing->Capslock();
				}
				else if( ! LOBYTE(GetKeyState(VK_CAPITAL)) )
					g_chewing->Capslock();
			}
		}
		else
		{
			ic->fdwConversion &= ~IME_CMODE_CHINESE;
		}
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

	if( *cs->getCompStr() )
	{
		// FIX #15284.
        //  If candidate wnd is open, send ESC key to lib, 
        //  then submit composition string, close cand wnd
    	if( g_chewing->Candidate() > 0 )
        {
            g_chewing->Esc();
            g_chewing->Enter();
            GenerateIMEMessage( imc.getHIMC(), WM_IME_NOTIFY, IMN_CLOSECANDIDATE, 1 );
            CandList* candList = (CandList*)ImmLockIMCC(imc.getIC()->hCandInfo);
            candList->setTotalCount(0);
        }

        if( g_chewing )
		{
			g_chewing->Enter();	// Commit
			char* cstr = NULL;
			if( g_chewing->CommitReady() && 
				(cstr = g_chewing->CommitStr()) )	{
				free(cstr);
			}
		}
		cs->setResultStr( cs->getCompStr() );
		cs->setZuin("");
		cs->setCompStr("");
		cs->setCursorPos(0);
		cs->beforeGenerateMsg();

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
				break;
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

			if( !IMEUI::registerUIClasses() )
				return FALSE;

			LoadConfig();

			break;
		}

	case DLL_PROCESS_DETACH:
		IMEUI::unregisterUIClasses();

		if( g_chewing )
			delete g_chewing;

		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
    return TRUE;
}

static int _InvertCase(int key)
{
	if( key >= 'A' && key <= 'Z' )
		key = tolower(key);
	else if( key >= 'a' && key <= 'z' )
		key = toupper( key );
    return  key;
}

BOOL FilterKeyByChewing( IMCLock& imc, UINT key, KeyInfo ki, const BYTE* keystate )
{
	bool isChinese = imc.isChinese();
	bool isFullShape = imc.isFullShape();

	if( !g_chewing )
	{
		if( ! isChinese && !isFullShape )
			return FALSE;	//delay the loading of Chewing engine
		// Enable numpad even in Chinese mode
		if( key >= VK_NUMPAD0 && key <= VK_DIVIDE )
			return FALSE;
		if( ! (g_chewing = LoadChewingEngine()) )
			return FALSE;
		if( IsKeyToggled( keystate[VK_CAPITAL]) || !isChinese )
			g_chewing->Capslock();	// switch to English mode
	}
	else
	{
		// Correct Chinese/English mode
		if( g_chewing->ChineseMode() )
		{
			if( IsKeyToggled( keystate[VK_CAPITAL]) || !isChinese )
				g_chewing->Capslock();
		}
		else if( !IsKeyToggled( keystate[VK_CAPITAL]) && isChinese )
			g_chewing->Capslock();

		CompStr* cs = imc.getCompStr();
		// In English mode, Bypass chewing if there is no composition string
		if( !cs || !*cs->getCompStr() )
		{
			if( isChinese )
			{
				// Enable numpad even in Chinese mode
				if( key >= VK_NUMPAD0 && key <= VK_DIVIDE )
					return FALSE;
				if( ( key == VK_SPACE && !isFullShape ) || key == VK_RETURN )
					return FALSE;
				char ascii[2];
				int ret = ToAscii( key, ki.scanCode, (BYTE*)keystate, (LPWORD)ascii, 0);
				if( ret )
					if( ascii[0] >= 'A' && ascii[0] <= 'Z' &&
					!IsKeyToggled( keystate[VK_CAPITAL]) && g_shiftCapital )
						return FALSE;
			}
			else if( !isFullShape )	// Chewing has been loaded but in English mode
				return FALSE;
		}


	}

	g_chewing->SetFullShape(isFullShape);
	g_chewing->SetSpaceAsSelection( !!g_spaceAsSelection );
	g_chewing->SetKeyboardLayout( (int)g_keyboardLayout );
	g_chewing->SetAddPhraseForward( !!g_addPhraseForward );
	g_chewing->SetSelAreaLen( (int)g_selAreaLen );

	if( IsKeyDown( keystate[VK_CONTROL] ) )
	{
		if(  key >= '0' && key <= '9' )
		{
			g_chewing->CtrlNum( key );
			CompStr* cs = imc.getCompStr();
			if( g_chewing->ShowMsgLen() > 0 )
			{
				char* msg = g_chewing->ShowMsg();
				if( msg )
				{
					cs->setShowMsg(msg);
					free(msg);
					GenerateIMEMessage( imc.getHIMC(), WM_IME_NOTIFY, IMN_PRIVATE, 0 );
				}
			}
			else if( ! g_chewing->Candidate() )
				return FALSE;
			return ! g_chewing->KeystrokeIgnore();
		}
		else
			return FALSE;
	}

	if( g_chewing->Candidate() > 0 )
	{
		switch( key )
		{
		case VK_NEXT:
        case VK_SPACE:// allow space to PgDn 
			key = VK_RIGHT;
			break;
		case VK_PRIOR:
			key = VK_LEFT;
            break;
        // Fix #15218, allow backspace/enter to close cand win
        case    VK_BACK: 
        case    VK_RETURN:
            key = VK_ESCAPE;
            break;
		}
	}
	else if( imc.isVerticalComp() && ! g_fixCompWnd )
	{
		//Vertical composition mode
		switch( key )
		{
		case VK_LEFT:
		case VK_RIGHT:
			key = VK_DOWN;
			break;
		case VK_UP:
			key = VK_LEFT;
			break;
		case VK_DOWN:
			key = VK_RIGHT;
			break;
		}
	}

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
		if( isChinese )
			g_chewing->Capslock();
			return FALSE;
		break;
	default:
		{
			char ascii[2];
			int ret = ToAscii( key, ki.scanCode, (BYTE*)keystate, (LPWORD)ascii, 0);
			if( ret )
			{
				if( key >= VK_NUMPAD0 && key <= VK_DIVIDE )
				{
					g_chewing->NumPad( ascii[0] );
					return TRUE;
				}
				key = ascii[0];
			}
			else
				return FALSE;

			if( isChinese )
			{
				if( IsKeyToggled( keystate[VK_CAPITAL] ) )
				{
                    key = _InvertCase(key);
                    if ( IsKeyDown(keystate[VK_SHIFT]) )
                    {
                        g_chewing->Key( key );
                        return TRUE;
                    }
				}
				if( g_shiftCapital && key >= 'A' && key <= 'Z' )
				{
					/// FIXME: Temporary hack to enable typing
					//  capital English characters in Chinese mode
					//  with shift key. There should be a better way.
					//  We temporary switch to English mode here, and
					//  that's ok since this wrong state will be corrected 
					//  when FilterKeyByChewing() is called next time.
					//  We always examine whether English/Chinese mode
					//  is properly set and correct it in the beginning
					//  of this function.
					g_chewing->Capslock();
					g_chewing->Key( key );
					return TRUE;
				}
			}

			if( key == VK_SPACE )
				g_chewing->Space();
			else
				g_chewing->Key( key );
		}
	}
	return ! g_chewing->KeystrokeIgnore();
}


