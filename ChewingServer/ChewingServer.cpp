// ChewingServer.cpp: implementation of the ChewingServer class.
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <tchar.h>
#include <shlobj.h>

#include "ChewingServer.h"
#include "Chewingpp.h"

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
	(ChewingMemberFuncCI) &Chewing::CtrlOption ,
	(ChewingMemberFuncCI) (GetSelKeyFunc)&Chewing::SelKey ,

	(ChewingMemberFuncCI) &Chewing::SetFullShape ,
	(ChewingMemberFuncCI) &Chewing::SetSpaceAsSelection ,
	(ChewingMemberFuncCI) &Chewing::SetAddPhraseForward ,
	(ChewingMemberFuncCI) &Chewing::SetKeyboardLayout ,
	(ChewingMemberFuncCI) &Chewing::SetHsuSelectionKeyType ,

	  // char* (void)
	(ChewingMemberFuncCI) (ChewingMemberFuncCV)&Chewing::ZuinStr ,
	(ChewingMemberFuncCI) (ChewingMemberFuncCV)&Chewing::CommitStr ,
	(ChewingMemberFuncCI) (ChewingMemberFuncCV)&Chewing::Buffer ,
	(ChewingMemberFuncCI) (ChewingMemberFuncCV)&Chewing::ShowMsg ,

	  // char* (int)
	(ChewingMemberFuncCI) &Chewing::Selection, 

	// void (char*)
	(ChewingMemberFuncCI) (SetSelKeyFunc)&Chewing::SelKey 

};

ChewingServer::ChewingServer() : hwnd(NULL), sharedMem(INVALID_HANDLE_VALUE)
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
//			SetPriorityClass( GetCurrentProcess(), NORMAL_PRIORITY_CLASS );
			Chewing* client = new Chewing();
			chewingClients.push_back(client);
			return (LRESULT)client;
			break;
		}
	case cmdRemoveClient:
		{
			Chewing* client = (Chewing*)lp;
			chewingClients.remove( client );
			delete client;
//			if( chewingClients.empty() )
//				SetPriorityClass( GetCurrentProcess(), IDLE_PRIORITY_CLASS );
//			PostQuitMessage(0);
			break;
		}
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		{
			list<Chewing*>::iterator it = chewingClients.begin();
			for( ; it != chewingClients.end(); ++it )
				delete *it;
			chewingClients.clear();
			PostQuitMessage(0);
			break;
		}
	}

	return DefWindowProc( hwnd, msg, wp, lp );
}

bool ChewingServer::startServer()
{
	HANDLE hprocess = GetCurrentProcess();
//	SetPriorityClass( hprocess, HIGH_PRIORITY_CLASS );

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

//	SetPriorityClass( hprocess, NORMAL_PRIORITY_CLASS );
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
