// ChewingServer.cpp: implementation of the ChewingServer class.
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <tchar.h>
#include <shlobj.h>
#include <time.h>	// For time()

#include "ChewingServer.h"
#include "Chewingpp.h"
#include "..\include\chewingserver.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static ChewingServer* g_ChewingServerInstance = NULL;

ChewingMemberFuncCI ChewingServer::chewingCmdTable[] = {
	(ChewingMemberFuncCI) &Chewing::Space ,
	(ChewingMemberFuncCI) &Chewing::Esc ,
	(ChewingMemberFuncCI) &Chewing::Enter ,
	(ChewingMemberFuncCI) &Chewing::Delete ,
	(ChewingMemberFuncCI) &Chewing::Backspace ,
	(ChewingMemberFuncCI) &Chewing::Tab ,
	(ChewingMemberFuncCI) &Chewing::ShiftLeft ,
	(ChewingMemberFuncCI) &Chewing::ShiftRight ,
	(ChewingMemberFuncCI) &Chewing::Space ,
	(ChewingMemberFuncCI) &Chewing::Right ,
	(ChewingMemberFuncCI) &Chewing::Left ,
	(ChewingMemberFuncCI) &Chewing::Up ,
	(ChewingMemberFuncCI) &Chewing::Down ,
	(ChewingMemberFuncCI) &Chewing::Home ,
	(ChewingMemberFuncCI) &Chewing::End ,
	(ChewingMemberFuncCI) &Chewing::Capslock ,
	(ChewingMemberFuncCI) &Chewing::DoubleTab ,
	(ChewingMemberFuncCI) &Chewing::CommitReady ,
	(ChewingMemberFuncCI) &Chewing::BufferLen ,
	(ChewingMemberFuncCI) &Chewing::CursorPos ,
	(ChewingMemberFuncCI) &Chewing::KeystrokeIgnore ,
	(ChewingMemberFuncCI) &Chewing::ChineseMode ,
	(ChewingMemberFuncCI) &Chewing::GetFullShape ,
	(ChewingMemberFuncCI) &Chewing::Candidate ,
	(ChewingMemberFuncCI) &Chewing::ChoicePerPage ,
	(ChewingMemberFuncCI) &Chewing::TotalChoice ,
	(ChewingMemberFuncCI) &Chewing::TotalPage ,
	(ChewingMemberFuncCI) &Chewing::CurrentPage ,
	(ChewingMemberFuncCI) &Chewing::ShowMsgLen ,
	(ChewingMemberFuncCI) &Chewing::GetAddPhraseForward ,

	// int (int) or void int
	(ChewingMemberFuncCI) &Chewing::Key ,
	(ChewingMemberFuncCI) &Chewing::CtrlNum ,
	(ChewingMemberFuncCI) &Chewing::NumPad ,
	(ChewingMemberFuncCI) (GetSelKeyFunc)&Chewing::SelKey ,

	(ChewingMemberFuncCI) &Chewing::SetFullShape ,
	(ChewingMemberFuncCI) &Chewing::SetSpaceAsSelection ,
	(ChewingMemberFuncCI) &Chewing::SetAddPhraseForward ,
	(ChewingMemberFuncCI) &Chewing::SetKeyboardLayout ,
	(ChewingMemberFuncCI) &Chewing::SetHsuSelectionKeyType ,
	(ChewingMemberFuncCI) &Chewing::SetCandPerPage ,
	(ChewingMemberFuncCI) &Chewing::SetEscCleanAllBuf ,
    (ChewingMemberFuncCI) &Chewing::SetAdvanceAfterSelection,
    (ChewingMemberFuncCI) &Chewing::SetEasySymbolInput,

	  // char* (void)
	(ChewingMemberFuncCI) (ChewingMemberFuncCV)&Chewing::ZuinStr ,
	(ChewingMemberFuncCI) (ChewingMemberFuncCV)&Chewing::CommitStr ,
	(ChewingMemberFuncCI) (ChewingMemberFuncCV)&Chewing::Buffer ,
	(ChewingMemberFuncCI) (ChewingMemberFuncCV)&Chewing::IntervalArray ,
	(ChewingMemberFuncCI) (ChewingMemberFuncCV)&Chewing::ShowMsg ,

	  // char* (int)
	(ChewingMemberFuncCI) &Chewing::Selection, 

	// void (char*)
	(ChewingMemberFuncCI) (SetSelKeyFunc)&Chewing::SelKey 

};

char* _gen_event_name(char *buf, int szbuf, const char *prefix)
{
	char temp[512]={'0'};
	DWORD sztemp = sizeof(temp);
	GetUserName(temp, &sztemp);

	strncpy(buf, prefix, szbuf);
	strncat(buf, "_", szbuf);
	strncat(buf, temp, szbuf);
	buf[szbuf-1] = '\0';
	return	buf;
}


ChewingServer::ChewingServer() : hwnd(NULL), sharedMem(INVALID_HANDLE_VALUE), checkTimer(0)
{
	char classname[512];
	_gen_event_name(classname, sizeof(classname), chewingServerClassName);
	g_ChewingServerInstance = this;

	WNDCLASSEX wc;
	wc.cbSize			= sizeof(WNDCLASSEX);
	wc.style			= 0;
	wc.lpfnWndProc		= (WNDPROC)ChewingServer::wndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= (HINSTANCE)GetModuleHandle(NULL);
	wc.hCursor			= NULL;
	wc.hIcon			= NULL;
	wc.lpszMenuName		= (LPTSTR)NULL;
	wc.lpszClassName	= classname;
	wc.hbrBackground	= NULL;
	wc.hIconSm			= NULL;
	if( !RegisterClassEx( (LPWNDCLASSEX)&wc ) )
		return;
	hwnd = CreateWindowEx(0, classname, NULL, 0, 
					0, 0, 0, 0, HWND_DESKTOP, NULL, wc.hInstance, NULL);
}

ChewingServer::~ChewingServer()
{
	CloseHandle(sharedMem);	
    OutputDebugString("Chewing server down.");
}

bool ChewingServer::run()
{
	if( !hwnd )
		return false;
	if( !startServer() )
		return false;

	MSG msg;
	while( GetMessage(&msg, NULL, 0, 0 ) )
		DispatchMessage( &msg );

	return true;
}

LRESULT CALLBACK ChewingServer::wndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	if( !g_ChewingServerInstance->hwnd )
		g_ChewingServerInstance->hwnd = hwnd;
	return g_ChewingServerInstance->wndProc( msg, wp, lp );
}

void GetUserDataPath( LPTSTR filename )
{
    LPITEMIDLIST pidl;
	HRESULT ret;
	ret = SHGetSpecialFolderLocation( NULL, CSIDL_APPDATA, &pidl );
	if ( ret != S_OK )
		ret = SHGetSpecialFolderLocation( NULL, CSIDL_COMMON_APPDATA, &pidl );
	if ( ret == S_OK )
	{
		SHGetPathFromIDList(pidl, filename);
		_tcscat( filename, _T("\\Chewing") );
		CreateDirectory( filename, NULL );

		IMalloc* pmalloc;
		if( NOERROR == SHGetMalloc(&pmalloc) ) {
			pmalloc->Free( pidl );
			pmalloc->Release();
		}
    }
}

LRESULT ChewingServer::wndProc(UINT msg, WPARAM wp, LPARAM lp)
{
	if( msg >= cmdFirst && msg <= cmdLast ) {
		if ( chewingClients.end()==chewingClients.find((unsigned int)lp) ) {
			return	0;
		}
		return parseChewingCmd(msg, (int)wp, (Chewing*)lp);
	}

	switch( msg )
	{
	case cmdAddClient:
		{
			Chewing* client = new Chewing();
			chewingClients[(unsigned int)client] = client;
			return (LRESULT)client;
			break;
		}
	case cmdRemoveClient:
		{
			if ( chewingClients.erase((unsigned int)lp)>0 )
            {   
                delete (Chewing*) lp;
            }
			break;
		}
    case cmdEcho:
        {
            map<unsigned int, Chewing*>::iterator iterT;
            
            iterT = chewingClients.find((unsigned int)lp);
            if ( iterT!=chewingClients.end() )
            {
                return  ~((LRESULT)(Chewing*)(iterT->second));
            }
        }
        return  -1; /* return -1 here, so clinet side can identify 
                       its response from server or API failed */
    case cmdLastPhoneSeq:
        {
            int lop;
			uint16_t *sbuf = Chewing::GetLastPhoneSeq();
		    uint16_t *obuf = (uint16_t*)MapViewOfFile( sharedMem, FILE_MAP_WRITE, 
									    0, 0, CHEWINGSERVER_BUF_SIZE );
            for ( lop=0; lop<MAX_PHONE_SEQ_LEN; ++lop )
            {
                if ( sbuf[lop]==0 )
                {
                    break;
                }
                obuf[lop] = sbuf[lop];
            }
            obuf[lop] = 0;
		    UnmapViewOfFile( obuf );
            return  lop;
        }
	case cmdReloadSymbolTable:
		TCHAR datadir[MAX_PATH];
		TCHAR hashdir[MAX_PATH];
		GetSystemDirectory( datadir, MAX_PATH );
		_tcscat( datadir, _T("\\IME\\Chewing") );

		GetUserDataPath( hashdir );
		Chewing::ReloadSymbolTable(datadir, hashdir);
		break;
	case WM_TIMER:
		checkNewVersion();
		KillTimer( hwnd, checkTimer );
		checkTimer = SetTimer( hwnd, 1, 6*60*60*1000, NULL );
		break;
    case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		{
			map<unsigned int, Chewing*>::iterator it = chewingClients.begin();
			for( ; it != chewingClients.end(); ++it )
				delete it->second;
			chewingClients.clear();
			KillTimer( hwnd, checkTimer );
			PostQuitMessage(0);
			break;
		}
	}

	return DefWindowProc( hwnd, msg, wp, lp );
}

bool ChewingServer::startServer()
{
	HANDLE hprocess = GetCurrentProcess();

	char evt_name[512];
	_gen_event_name(evt_name, sizeof(evt_name), "Local\\ChewingServerEvent");
	LPCTSTR evtname = evt_name;

	char prc_name[512];
	_gen_event_name(prc_name, sizeof(prc_name), "Local\\ChewingServer");
	LPCTSTR name = prc_name;
	LPCTSTR winname = "Local\\ChewingServer";
	DWORD osVersion = GetVersion();
 	DWORD major = (DWORD)(LOBYTE(LOWORD(osVersion)));
	DWORD minor =  (DWORD)(HIBYTE(LOWORD(osVersion)));
	if( osVersion >= 0x80000000 || major <= 4 )	// Windows 9x or Windows NT 4
	{
		name += 6;	// remove prfix "Local\\"
		evtname += 6;	// remove prfix "Local\\"
		winname += 6;
	}
	HANDLE evt = OpenEvent( EVENT_ALL_ACCESS, FALSE, evtname );
	SetWindowText( hwnd, winname );

	sharedMem = CreateFileMapping( INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,
						0, CHEWINGSERVER_BUF_SIZE, name );

	TCHAR datadir[MAX_PATH];
	TCHAR hashdir[MAX_PATH];
	LPCTSTR phashdir = datadir;
	GetSystemDirectory( datadir, MAX_PATH );
	_tcscat( datadir, _T("\\IME\\Chewing") );

	GetUserDataPath( hashdir );
	Chewing::LoadDataFiles( datadir, hashdir );

	if( evt != INVALID_HANDLE_VALUE )
	{
		SetEvent(evt);
		CloseHandle(evt);
	}

	// Set up a timer to regularly check if there is a new version.
	checkTimer = SetTimer( hwnd, 1, 5*60*1000, NULL );
	return true;
}

LRESULT ChewingServer::parseChewingCmd(UINT cmd, int param, Chewing *chewing)
{
	cmd -= cmdFirst;
	if( cmd < (cmdKey - cmdFirst) )	 //	int (void)
	{
		ChewingMemberFuncIV func = (ChewingMemberFuncIV)chewingCmdTable[cmd];
		return (chewing->*func)();
	}
	if( cmd < (cmdZuinStr - cmdFirst) )	 //	int (int)
	{
		ChewingMemberFuncII func = (ChewingMemberFuncII)chewingCmdTable[cmd];
		return (chewing->*func)(param);
	}
	if( cmd <= (cmdSelection - cmdFirst) )	//  char* (void) && char* (int)
	{
		char* str = NULL;
		int len = 0;
		if( cmd == (cmdIntervalArray - cmdFirst) ) // unsigned char* (void)
		{
			// Special case for interval array
			str = (char*)chewing->IntervalArray();
			len = chewing->IntervalLen() * sizeof(unsigned char);
		}
		else if( cmd < (cmdSelection - cmdFirst) )	// char* (void)
		{
			ChewingMemberFuncCV func = (ChewingMemberFuncCV)chewingCmdTable[cmd];
			str = (chewing->*func)();
			len = strlen(str) + 1;
		}
		else	// char* (int)
		{
			ChewingMemberFuncCI func = (ChewingMemberFuncCI)chewingCmdTable[cmd];
			str = (chewing->*func)(param);
			len = strlen(str) + 1;
		}
		if( str )
		{
			char* pbuf = (char*)MapViewOfFile( sharedMem, FILE_MAP_WRITE, 
										0, 0, CHEWINGSERVER_BUF_SIZE );
			memcpy( pbuf, str, len );
			UnmapViewOfFile( pbuf );
			if( cmd == (cmdIntervalArray - cmdFirst) )
				free(str);
			else
				chewing_free(str);
			return len;
		}
	}
	if( cmd == (cmdSetSelKey - cmdFirst) )
	{
		SetSelKeyFunc func = (SetSelKeyFunc)chewingCmdTable[cmd];
		char* pbuf = (char*)MapViewOfFile( sharedMem, FILE_MAP_READ, 
									0, 0, CHEWINGSERVER_BUF_SIZE );
		(chewing->*func)(pbuf);
		UnmapViewOfFile( pbuf );
	}
	return 0;
}


void ChewingServer::checkNewVersion(void)
{
	// Check if there is a new version on the website
	time_t cur_time = 0, last_check = 0;
	time(&cur_time);

	HKEY hk = NULL;
	if( ERROR_SUCCESS == RegCreateKeyEx( HKEY_CURRENT_USER, _T("Software\\ChewingIME"), 0, 
			NULL, 0, KEY_ALL_ACCESS , NULL, &hk, NULL) )
	{
		DWORD size = sizeof(DWORD);
		DWORD type = REG_DWORD;
		const TCHAR key_name[] = _T("LastVersionCheck");

		DWORD check_new_version = true;
		RegQueryValueEx( hk, _T("CheckNewVersion"), 0, &type, (LPBYTE)&check_new_version, &size );
		if( check_new_version ) {
			RegQueryValueEx( hk, key_name, 0, &type, (LPBYTE)&last_check, &size );
			if( cur_time - last_check > 86400 ) {
				// Update time of last check
				RegSetValueEx( hk, key_name, 0, REG_DWORD, (BYTE*)&cur_time, sizeof(DWORD) );

				// Launch update checker
				TCHAR path[MAX_PATH];
				GetSystemDirectory( path, MAX_PATH );
				_tcscat( path, _T("\\IME\\Chewing\\Update.exe") );
				ShellExecute( NULL, "open", path, "/silent", NULL, SW_HIDE );
			}
		}
		RegCloseKey( hk );
	}
}
