// ChewingClient.cpp: implementation of the ChewingClient class.
//
//////////////////////////////////////////////////////////////////////

#include "ChewingClient.h"
#include <string.h>
#include <tchar.h>
#include ".\chewingclient.h"
#include ".\pipe.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ChewingClient::ChewingClient( int kbLayout, bool spaceAsSel, const char* selKeys, bool AdvAfterSel, bool EscCleanAllBuf )
	: serverWnd(NULL), chewingID(0), sharedMem(INVALID_HANDLE_VALUE)
	, spaceAsSelection(spaceAsSel)
	, keyLayout(kbLayout)
	, advAfterSelection(AdvAfterSel)
	, escCleanAllBuf( EscCleanAllBuf )
{
	ConnectServer();
	SelKey((char*)selKeys);
	SetAdvanceAfterSelection(advAfterSelection);
	SetEscCleanAllBuf( escCleanAllBuf );
	pSelKeys = (char*)selKeys;
}

ChewingClient::~ChewingClient()
{
	SendMessage( chewingID, ChewingServer::cmdRemoveClient, 0);
	if ( sharedMem==INVALID_HANDLE_VALUE )
	{
		CloseHandle(sharedMem);
	}
}

unsigned int ChewingClient::SendMessage(unsigned int session, unsigned int cmd,
					unsigned int dat)
{
	ChewingPipeClnt pipe;
	return	pipe.SendMessageReply(session, cmd, dat);
}

unsigned int ChewingClient::AddClient()
{
	ChewingPipeClnt pipe;
	return	pipe.SendMessageReply(0, ChewingServer::cmdAddClient, 0);
}

void ChewingClient::SetKeyboardLayout(int kb)
{
	SendMessage(chewingID, ChewingServer::cmdSetKeyboardLayout, kb);
}


void ChewingClient::SetHsuSelectionKeyType(int type)
{
	SendMessage(chewingID, ChewingServer::cmdSetHsuSelectionKeyType, type);
}


int ChewingClient::Space()
{
	return (int)SendMessage(chewingID, ChewingServer::cmdSpace, 0);
}

int ChewingClient::Esc()
{
	return (int)SendMessage(chewingID, ChewingServer::cmdEsc, 0);
}

int ChewingClient::Enter()
{
	return (int)SendMessage(chewingID, ChewingServer::cmdEnter, 0);
}

int ChewingClient::Delete()
{
	return (int)SendMessage( chewingID, ChewingServer::cmdDelete, 0);
}

int ChewingClient::Backspace()
{
	return (int)SendMessage( chewingID, ChewingServer::cmdBackspace, 0);
}

int ChewingClient::Tab()
{
	return (int)SendMessage( chewingID, ChewingServer::cmdTab, 0);
}

int ChewingClient::ShiftLeft()
{
	return (int)SendMessage( chewingID, ChewingServer::cmdShiftLeft, 0);
}

int ChewingClient::ShiftRight()
{
	return (int)SendMessage( chewingID, ChewingServer::cmdShiftRight, 0);
}

int ChewingClient::ShiftSpace()
{
	return (int)SendMessage( chewingID, ChewingServer::cmdShiftSpace, 0);
}

int ChewingClient::Right()
{
	return (int)SendMessage( chewingID, ChewingServer::cmdRight, 0);
}

int ChewingClient::Left()
{
	return (int)SendMessage( chewingID, ChewingServer::cmdLeft, 0);
}

int ChewingClient::Up()
{
	return (int)SendMessage( chewingID, ChewingServer::cmdUp, 0);
}

int ChewingClient::Down()
{
	return (int)SendMessage( chewingID, ChewingServer::cmdDown, 0);
}

int ChewingClient::Home()
{
	return (int)SendMessage( chewingID, ChewingServer::cmdHome, 0);
}

int ChewingClient::End()
{
	return (int)SendMessage( chewingID, ChewingServer::cmdEnd, 0);
}

int ChewingClient::Capslock()
{
	return (int)SendMessage( chewingID, ChewingServer::cmdCapslock, 0);
}

int ChewingClient::Key(unsigned int code)
{
	return (int)SendMessage( chewingID, ChewingServer::cmdKey, code);
}

int ChewingClient::CtrlNum(unsigned int code)
{
	return (int)SendMessage( chewingID, ChewingServer::cmdCtrlNum, code);
}

int ChewingClient::NumPad(unsigned int code)
{
	return (int)SendMessage( chewingID, ChewingServer::cmdNumPad, code);
}

int ChewingClient::CtrlOption(unsigned int code)
{
	return (int)SendMessage( chewingID, ChewingServer::cmdCtrlOption, code);
}

int ChewingClient::DoubleTab()
{
	return (int)SendMessage( chewingID, ChewingServer::cmdDoubleTab, 0);
}

// Return the i-th selection key, i >= 0.
char ChewingClient::SelKey(int i)
{
	return (char)SendMessage( chewingID, ChewingServer::cmdGetSelKey, i);
}

void ChewingClient::SetAdvanceAfterSelection(bool bDo)
{
	SendMessage( chewingID, ChewingServer::cmdAdvanceAfterSelection, 
		(bDo==true)?1 :0);
}

void ChewingClient::SetEasySymbolInput(bool bSet)
{
	SendMessage( chewingID, ChewingServer::cmdEasySymbolInput, 
		(bSet==true)?1 :0);
}

void ChewingClient::SelKey(char* selkey)
{
    sharedMem = OpenFileMapping( FILE_MAP_ALL_ACCESS, FALSE, filemapName);
    if ( sharedMem==NULL )
    {
        sharedMem = INVALID_HANDLE_VALUE; 
        return;
    }

    char* pbuf = (char*)MapViewOfFile( sharedMem, FILE_MAP_WRITE, 
								0, 0, CHEWINGSERVER_BUF_SIZE );
    if ( pbuf!=NULL )
    {
	    strcpy( pbuf, selkey );
	    UnmapViewOfFile( pbuf );
    }
	CloseHandle(sharedMem);
    sharedMem = INVALID_HANDLE_VALUE;

    SendMessage( chewingID, ChewingServer::cmdSetSelKey, 0);
}

char* ChewingClient::ZuinStr()
{
	int len = (int)SendMessage( chewingID, ChewingServer::cmdZuinStr, 0);
	return (char*)GetDataFromSharedMem(len);
}

char* ChewingClient::CommitStr()
{
	int len = (int)SendMessage( chewingID, ChewingServer::cmdCommitStr, 0);
	return (char*)GetDataFromSharedMem(len);
}

int   ChewingClient::CommitReady()
{
	return (int)SendMessage( chewingID, ChewingServer::cmdCommitReady, 0);
}

char* ChewingClient::Buffer()
{
	int len = (int)SendMessage( chewingID, ChewingServer::cmdBuffer, 0);
	return (char*)GetDataFromSharedMem(len);
}

int   ChewingClient::BufferLen()
{
	return (int)SendMessage( chewingID, ChewingServer::cmdBufferLen, 0);
}

int ChewingClient::CursorPos()
{
	return (int)SendMessage( chewingID, ChewingServer::cmdCursorPos, 0);
}

int ChewingClient::PointStart()
{
	return (int)SendMessage( chewingID, ChewingServer::cmdPointStart, 0);
}

int ChewingClient::PointEnd()
{
	return (int)SendMessage( chewingID, ChewingServer::cmdPointEnd, 0);
}

int ChewingClient::KeystrokeRtn()
{
	return (int)SendMessage( chewingID, ChewingServer::cmdKeystrokeRtn, 0);
}

int ChewingClient::KeystrokeIgnore()
{
	return (int)SendMessage( chewingID, ChewingServer::cmdKeystrokeIgnore, 0);
}

int ChewingClient::ChineseMode()
{
	return (int)SendMessage( chewingID, ChewingServer::cmdChineseMode, 0);
}

// CandidateWindow-related routines
int ChewingClient::Candidate()
{
	return (int)SendMessage( chewingID, ChewingServer::cmdCandidate, 0);
}

int ChewingClient::ChoicePerPage()
{
	return (int)SendMessage( chewingID, ChewingServer::cmdChoicePerPage, 0);
}

int ChewingClient::TotalChoice()
{
	return (int)SendMessage( chewingID, ChewingServer::cmdTotalChoice, 0);
}

int ChewingClient::TotalPage()
{
	return (int)SendMessage( chewingID, ChewingServer::cmdTotalPage, 0);
}

int ChewingClient::CurrentPage()
{
	return (int)SendMessage( chewingID, ChewingServer::cmdCurrentPage, 0);
}

// Return the i-th selection wchar_t, i >= 0.
char* ChewingClient::Selection(int i)
{
	int len = (int)SendMessage( chewingID, ChewingServer::cmdSelection, i);
	return (char*)GetDataFromSharedMem(len);
}

void ChewingClient::SetFullShape(bool full)
{
	SendMessage( chewingID, ChewingServer::cmdSetFullShape, full);
}

bool ChewingClient::GetFullShape(void)
{
	return !!SendMessage( chewingID, ChewingServer::cmdGetFullShape, 0);
}

void ChewingClient::SetSpaceAsSelection(bool spaceAsSelection)
{
	SendMessage( chewingID, ChewingServer::cmdSetSpaceAsSelection, spaceAsSelection);
}

void ChewingClient::SetEscCleanAllBuf( bool escCleanAllBuf )
{
	SendMessage( chewingID, ChewingServer::cmdSetEscCleanAllBuf, escCleanAllBuf);
}

unsigned char* ChewingClient::GetDataFromSharedMem(int len)
{
	unsigned char* data = NULL;

	if ( len<=0 ){
		return	NULL;
	}

	sharedMem = OpenFileMapping( FILE_MAP_ALL_ACCESS, FALSE, filemapName);
	if ( sharedMem==NULL )
	{
		sharedMem = INVALID_HANDLE_VALUE; 
		return  NULL;
	}

	char* buf = (char*)MapViewOfFile( sharedMem, FILE_MAP_READ, 0, 0, CHEWINGSERVER_BUF_SIZE );
	if( buf )
	{
		data = (unsigned char*)calloc( len, sizeof(unsigned char) );
		memcpy( data, buf, len );
		UnmapViewOfFile(buf);
	}
	CloseHandle(sharedMem);
	sharedMem = INVALID_HANDLE_VALUE;
	return data;
}

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

void ChewingClient::ConnectServer(void)
{
	char tempname[512];

	_gen_event_name(tempname, sizeof(tempname), chewingServerClassName);
	if ( 0==ChewingClient::EchoFromServer() )
	{
		char evt_name[512];
		_gen_event_name(evt_name, sizeof(evt_name), "Local\\ChewingServerEvent");
		LPCTSTR evtname = evt_name;

		DWORD osVersion = GetVersion();
 		DWORD major = (DWORD)(LOBYTE(LOWORD(osVersion)));
		DWORD minor =  (DWORD)(HIBYTE(LOWORD(osVersion)));
		if( osVersion >= 0x80000000 || major <= 4 )	// Windows 9x or Windows NT 4
			evtname += 6;	// remove prfix "Local\\"

		HANDLE evt = CreateEvent( NULL, TRUE, FALSE, evtname );
		TCHAR server_path[MAX_PATH];
		GetSystemDirectory( server_path, MAX_PATH );
		_tcscat( server_path, _T("\\IME\\Chewing\\ChewingServer.exe") );
		ShellExecute( NULL, "open", server_path, NULL, NULL, SW_HIDE );
		WaitForSingleObject( evt, 15000 );
		CloseHandle(evt);
	}

	chewingID = AddClient();

	_gen_event_name(filemapName, sizeof(filemapName), "Local\\ChewingServer");

	SetSpaceAsSelection(spaceAsSelection);
	SetKeyboardLayout(keyLayout);
}

int ChewingClient::ShowMsgLen(void)
{
	return (int)SendMessage( chewingID, ChewingServer::cmdShowMsgLen, 0);
}

char* ChewingClient::ShowMsg(void)
{
	int len = (int)SendMessage( chewingID, ChewingServer::cmdShowMsg, 0);
	return (char*)GetDataFromSharedMem(len);
}

void ChewingClient::SetAddPhraseForward(bool add_forward)
{
	SendMessage( chewingID, ChewingServer::cmdSetAddPhraseForward, add_forward);
}

int ChewingClient::GetAddPhraseForward(void)
{
	return (int)SendMessage( chewingID, ChewingServer::cmdGetAddPhraseForward, 0);
}

unsigned int ChewingClient::EchoFromServer()
{
	return (unsigned int)SendMessage( chewingID, ChewingServer::cmdEcho, 0);
}

bool ChewingClient::CheckServer()
{
	if ( ChewingClient::EchoFromServer()==(~chewingID) )
	{
		return  true;
	}
	ConnectServer();
	SelKey((char*)pSelKeys);
	return  false;
}

void ChewingClient::SetCandPerPage(int len)
{
	SendMessage( chewingID, ChewingServer::cmdSetCandPerPage, len);
}

unsigned char* ChewingClient::GetIntervalArray(int& len)
{
	len = (int)SendMessage( chewingID, ChewingServer::cmdIntervalArray, 0);
	return GetDataFromSharedMem( len );
}

void ChewingClient::ReloadSymbolTable(void)
{
	SendMessage( chewingID, ChewingServer::cmdReloadSymbolTable, 0);
}

