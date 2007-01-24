// ChewingServer.cpp: implementation of the ChewingServer class.
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <tchar.h>
#include <shlobj.h>
#include <time.h>	// For time()

#include "ChewingServer.h"
#include "Chewingpp.h"
#include "..\include\chewingserver.h"
#include "private.h"
#include "pipe.h"

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
	g_ChewingServerInstance = this;
}

ChewingServer::~ChewingServer()
{
	CloseHandle(sharedMem);
	TerminateChewing();
}

bool ChewingServer::run()
{
	if( !startServer() ) {
		return false;
	}
	if ( !processor() ) {
		return false;
	}
	return true;
}

void GetUserDataPath( LPTSTR filename )
{
	LPITEMIDLIST pidl;
	if( S_OK == SHGetSpecialFolderLocation( NULL, CSIDL_APPDATA, &pidl ) )
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

unsigned int ChewingServer::NewChewingClient(ChewingPipeServ *cs)
{
	Chewing* client = new Chewing();
	clients[(unsigned int)client] = client;
	return	(unsigned int) client;
}

unsigned int ChewingServer::RemoveChewingClient(unsigned int client)
{
	if ( clients.erase(client)>0 ) {
		delete (Chewing*) client;
	}
	return	0;
}

unsigned int ChewingServer::Echo(unsigned int client)
{
	map<unsigned int, Chewing*>::iterator iterT;
	unsigned int ret = -1;

	iterT = clients.find(client);
	if ( iterT!=clients.end() ) {
		ret = ~(unsigned int)(iterT->second);
	}

	return	ret;
}

unsigned int ChewingServer::LastPhoneSeq()
{
	uint16 *sbuf = GetLastPhoneSeq();
	uint16 *obuf = (uint16*)MapViewOfFile( sharedMem, FILE_MAP_WRITE, 
		0, 0, CHEWINGSERVER_BUF_SIZE );
	for ( int lop=0; lop<MAX_PHONE_SEQ_LEN; ++lop ) {
		if ( sbuf[lop]==0 ) {
			break;
		}
		obuf[lop] = sbuf[lop];
	}
	obuf[lop] = 0;
	UnmapViewOfFile( obuf );

	return	(unsigned int)lop;
}

unsigned int ChewingServer::ReloadSymbolTables()
{
	char userdir[MAX_PATH];
	GetUserDataPath( userdir );
	Chewing::ReloadSymbolTable(userdir);
	return	0;
}

unsigned int ChewingServer::ShutdownServer()
{
	map<unsigned int, Chewing*>::iterator it = clients.begin();
	for( ; it != clients.end(); ++it )
		delete it->second;
	clients.clear();
	return	0;
}

bool ChewingServer::processor()
{
	ChewingPipeServ cserv;
	time_t	btick, ttick = 60*3;
	unsigned int session, cmd, dat, ret;

	fireReadyEvent();
	
	btick = time(NULL);
	while ( 1 ) {
		if ( ! cserv.fetch() ) {
			continue;
		}

		if ( ! cserv.read() ) {
			continue;
		}

		if ( ! cserv.get_param(session, cmd, dat) ) {
			continue;
		}

		ret = 0;
{
	char aaa[50];
	sprintf(aaa, "session: %08x, cmd: %x, dat=%08x\n", session, cmd, dat);
}

		if( cmd >= cmdFirst && cmd <= cmdLast ) {
			if ( clients.end()==clients.find(session) ) {
				continue;
			}
			ret = parseChewingCmd(cmd, (int)dat, (Chewing*)session);
		}
		else {
			switch ( cmd ) {
			case cmdAddClient:
				ret = NewChewingClient(&cserv);
				break;
			case cmdRemoveClient:
				ret = RemoveChewingClient(session);
				break;
			case cmdEcho:
				ret = Echo(session);
				break;
			case cmdLastPhoneSeq:
				ret = LastPhoneSeq();
				break;
			case cmdReloadSymbolTable:
				ret = ReloadSymbolTables();
				break;
			};
		}
		
		cserv.reply(ret);
		cserv.disconnect();

		if ( cmd==cmdShutdownServer ) {
			break;
		}

		if ( time(NULL)-btick>=ttick ) {
			checkNewVersion();
			btick = time(NULL);
			ttick = 86400*3;
		}
	}

	return 1;
}

bool ChewingServer::fireReadyEvent()
{
	char evt_name[512];
	LPCTSTR evtname = evt_name;
	DWORD osVersion = GetVersion();
 	DWORD major = (DWORD)(LOBYTE(LOWORD(osVersion)));
	DWORD minor =  (DWORD)(HIBYTE(LOWORD(osVersion)));

	_gen_event_name(evt_name, sizeof(evt_name), "Local\\ChewingServerEvent");
	if( osVersion >= 0x80000000 || major <= 4 )	{ // Windows 9x or Windows NT 4
		evtname += 6;	// remove prfix "Local\\"
	}

	HANDLE evt = OpenEvent( EVENT_ALL_ACCESS, FALSE, evtname );
	if ( evt != INVALID_HANDLE_VALUE ) {
		SetEvent(evt);
		CloseHandle(evt);
		return	true;
	}
	return	false;
}

bool ChewingServer::startServer()
{
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
		winname += 6;
	}
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

	return true;
}

void ChewingServer::write_to_client(char *str, unsigned int len)
{
	char* pbuf = (char*)MapViewOfFile(sharedMem, FILE_MAP_WRITE, 
		0, 0, CHEWINGSERVER_BUF_SIZE );
	memcpy(pbuf, str, len);
	UnmapViewOfFile(pbuf);
}

unsigned int ChewingServer::parseChewingCmd(UINT cmd, int param, Chewing *chewing)
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
