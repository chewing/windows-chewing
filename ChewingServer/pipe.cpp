
#include <windows.h>
#include "private.h"
#include "pipe.h"



static void _release_packet(ChewingMsgPacket *pkt)
{
	if ( NULL==pkt ) {
		return;
	}
	if ( NULL!=pkt->message ) {
		free(pkt->message);
	}
	if ( NULL!=pkt->response ) {
		free(pkt->response);
	}
	free(pkt);
}

static ChewingMsgPacket* _generate_packet(HANDLE hPipe)
{
	ChewingMsgPacket *pkt = NULL;
	BOOL bRes = FALSE;
	DWORD ReadLen = 0;

	pkt = ALC(ChewingMsgPacket, 1);
	if ( NULL==pkt ) {
		return	NULL;
	}
	pkt->message = ALC(char, PIPE_MAX_BUFFER_SZ);
	if ( NULL==pkt->message ) {
		_release_packet(pkt);
		return	NULL;
	}

	ReadLen =0 ;
	bRes = ReadFile(hPipe, pkt->message, PIPE_MAX_BUFFER_SZ, &ReadLen, NULL);
	if ( FALSE==bRes || 0==ReadLen ) {
		_release_packet(pkt);
		return	NULL;
	}
	return	pkt;
}

ChewingMsgPacket* FetchClientRequest(char *pipename)
{
	ChewingMsgPacket *pkt = NULL;
	HANDLE hPipe = INVALID_HANDLE_VALUE;
	BOOL fConnected = FALSE;

	hPipe = CreateNamedPipe( 
		pipename,
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE |		// message type pipe 
		PIPE_READMODE_MESSAGE |		// message-read mode 
		PIPE_WAIT,			// blocking mode 
		PIPE_UNLIMITED_INSTANCES,	// max. instances  
		PIPE_MAX_BUFFER_SZ,		// output buffer size 
		PIPE_MAX_BUFFER_SZ,		// input buffer size 
		NMPWAIT_USE_DEFAULT_WAIT,	// client time-out 
		NULL);				// default security attribute 

	if ( hPipe==INVALID_HANDLE_VALUE ) {
		return pkt;
	}

	fConnected = ConnectNamedPipe(hPipe, NULL)
		? TRUE : (GetLastError()==ERROR_PIPE_CONNECTED); 
	if ( fConnected ) { 
		pkt = _generate_packet(hPipe);
	} 
	CloseHandle(hPipe); 
	return	pkt;
}

int ReplyClientRequest(ChewingMsgPacket *pkt)
{
	return	0;
}

int DestroyPacket(ChewingMsgPacket *pkt)
{
	return	0;
}

