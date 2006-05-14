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
	(ChewingMemberFuncCI) &Chewing::PointStart ,
	(ChewingMemberFuncCI) &Chewing::PointEnd ,
	(ChewingMemberFuncCI) &Chewing::KeystrokeRtn ,
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
	(ChewingMemberFuncCI) &Chewing::CtrlOption ,
	(ChewingMemberFuncCI) (GetSelKeyFunc)&Chewing::SelKey ,

	(ChewingMemberFuncCI) &Chewing::SetFullShape ,
	(ChewingMemberFuncCI) &Chewing::SetSpaceAsSelection ,
	(ChewingMemberFuncCI) &Chewing::SetAddPhraseForward ,
	(ChewingMemberFuncCI) &Chewing::SetKeyboardLayout ,
	(ChewingMemberFuncCI) &Chewing::SetHsuSelectionKeyType ,
	(ChewingMemberFuncCI) &Chewing::SetSelAreaLen ,
	(ChewingMemberFuncCI) &Chewing::SetEscCleanAllBuf ,
    (ChewingMemberFuncCI) &Chewing::SetAdvanceAfterSelection,

	  // char* (void)
	(ChewingMemberFuncCI) (ChewingMemberFuncCV)&Chewing::ZuinStr ,
	(ChewingMemberFuncCI) (ChewingMemberFuncCV)&Chewing::CommitStr ,
	(ChewingMemberFuncCI) (ChewingMemberFuncCV)&Chewing::Buffer ,
	(ChewingMemberFuncCI) (ChewingMemberFuncCV)&Chewing::IntervalStr ,
	(ChewingMemberFuncCI) (ChewingMemberFuncCV)&Chewing::ShowMsg ,

	  // char* (int)
	(ChewingMemberFuncCI) &Chewing::Selection, 

	// void (char*)
	(ChewingMemberFuncCI) (SetSelKeyFunc)&Chewing::SelKey 

};

ChewingServer::ChewingServer() : hwnd(NULL), sharedMem(INVALID_HANDLE_VALUE), checkTimer(0)
{
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
	wc.lpszClassName	= chewingServerClassName;
	wc.hbrBackground	= NULL;
	wc.hIconSm			= NULL;
	if( !RegisterClassEx( (LPWNDCLASSEX)&wc ) )
		return;
	hwnd = CreateWindowEx(0, chewingServerClassName, NULL, 0, 
					0, 0, 0, 0, HWND_DESKTOP, NULL, wc.hInstance, NULL);
}

ChewingServer::~ChewingServer()
{
	CloseHandle(sharedMem);
	TerminateChewing();
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

LRESULT ChewingServer::wndProc(UINT msg, WPARAM wp, LPARAM lp)
{
	if( msg >= cmdFirst && msg <= cmdLast )
		return parseChewingCmd(msg, (int)wp, (Chewing*)lp);

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
            uint16 *sbuf = GetLastPhoneSeq();
		    uint16 *obuf = (uint16*)MapViewOfFile( sharedMem, FILE_MAP_WRITE, 
									    0, 0, CHEWINGSERVER_BUF_SIZE );
            for ( int lop=0; lop<MAX_PHONE_SEQ_LEN; ++lop )
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
	case WM_TIMER:
		checkNewVersion();
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

	LPCTSTR name = _T("Local\\ChewingServer");
	LPCTSTR evtname = _T("Local\\ChewingServerEvent");
	DWORD osVersion = GetVersion();
 	DWORD major = (DWORD)(LOBYTE(LOWORD(osVersion)));
	DWORD minor =  (DWORD)(HIBYTE(LOWORD(osVersion)));
	if( osVersion >= 0x80000000 || major <= 4 )	// Windows 9x or Windows NT 4
	{
		name += 6;	// remove prfix "Local\\"
		evtname += 6;	// remove prfix "Local\\"
	}
	HANDLE evt = OpenEvent( EVENT_ALL_ACCESS, FALSE, evtname );
	SetWindowText( hwnd, name );

	sharedMem = CreateFileMapping( INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,
						0, CHEWINGSERVER_BUF_SIZE, name );

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
	Chewing::LoadDataFiles( datadir, hashdir );

	if( evt != INVALID_HANDLE_VALUE )
	{
		SetEvent(evt);
		CloseHandle(evt);
	}
    OutputDebugString("Chewing server up.");

	// Set up a timer to regularly check if there is a new version.
	checkTimer = SetTimer( hwnd, 1, 3600 * 1000, NULL );
	checkNewVersion();	// Check now!
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
		if( cmd < (cmdSelection - cmdFirst) )	// char* (void)
		{
			ChewingMemberFuncCV func = (ChewingMemberFuncCV)chewingCmdTable[cmd];
			str = (chewing->*func)();
		}
		else	// char* (int)
		{
			ChewingMemberFuncCI func = (ChewingMemberFuncCI)chewingCmdTable[cmd];
			str = (chewing->*func)(param);
		}
		if( str )
		{
			int len = strlen(str);
			char* pbuf = (char*)MapViewOfFile( sharedMem, FILE_MAP_WRITE, 
										0, 0, CHEWINGSERVER_BUF_SIZE );
			memcpy( pbuf, str, len + 1 );
			UnmapViewOfFile( pbuf );
			free(str);
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

		RegQueryValueEx( hk, key_name, 0, &type, (LPBYTE)&last_check, &size );
		if( cur_time - last_check > 86400 ) {
			// Update time of last check
			RegSetValueEx( hk, key_name, 0, REG_DWORD, (BYTE*)&cur_time, sizeof(DWORD) );

			// Launch update checker
			TCHAR path[MAX_PATH];
			GetSystemDirectory( path, MAX_PATH );
			_tcscat( path, _T("\\IME\\Chewing\\Update.exe") );
			ShellExecute( NULL, "open", path, NULL, NULL, SW_HIDE );
		}
		RegCloseKey( hk );
	}
}
