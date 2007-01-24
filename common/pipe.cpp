
#include <windows.h>
#include "private.h"
#include "pipe.h"



#define	PIPE_MAX_BUFFER_SZ	64
#define PIPE_NAME_PREFIX	"LIBCHEWING_WIN32_PIPE_"



class PipeImpl
{
public:
	static HANDLE create(char *name);

	static HANDLE open(char *name);

	static unsigned int send_and_read(HANDLE h, char *data, unsigned int dlen, char *buffer, unsigned int blen, BOOL &isMoreData);

	static unsigned int write(HANDLE h, char *dat, unsigned int sz);

	static unsigned int read(HANDLE h, char *buf, unsigned int sz, BOOL &isMoreData);

	static void get_pipe_name(char *buf, unsigned int len);

	static void disconnect(HANDLE h);

        static BOOL fetch(HANDLE h);

	static void close(HANDLE h);
};

HANDLE PipeImpl::create(char *name)
{
	return
		CreateNamedPipe(
			name, 
			PIPE_ACCESS_DUPLEX, 
			PIPE_TYPE_MESSAGE|PIPE_READMODE_MESSAGE|PIPE_WAIT,
			PIPE_UNLIMITED_INSTANCES, 
			PIPE_MAX_BUFFER_SZ,
			PIPE_MAX_BUFFER_SZ, 
			NMPWAIT_USE_DEFAULT_WAIT, 
			NULL);
}

HANDLE PipeImpl::open(char *n)
{
	HANDLE h = INVALID_HANDLE_VALUE;
	int lop = 0;

	while ( lop<3 ) {
		h = CreateFile(n, GENERIC_READ|GENERIC_WRITE, 0, 
			NULL, OPEN_EXISTING, 0, NULL);
		if ( INVALID_HANDLE_VALUE!=h ) {
			DWORD m = PIPE_READMODE_MESSAGE|PIPE_WAIT; 
			if ( TRUE==SetNamedPipeHandleState(h, &m, NULL, NULL) ) {
				break;
			}

			CloseHandle(h);
			h = INVALID_HANDLE_VALUE;
		}
		else if ( FALSE==WaitNamedPipe(n, 5000) ) {
			break;
		}
		lop++;
	};

	return	h;
}

unsigned int PipeImpl::send_and_read(HANDLE h,
				     char *data, unsigned int dlen,
				     char *buffer, unsigned int blen,
				     BOOL &isMoreData)
{
	unsigned int szo = 0;
	isMoreData = FALSE;
	if ( 0==TransactNamedPipe(h, (LPVOID) data, dlen, 
		(LPVOID) buffer, blen, (LPDWORD) &szo, NULL) )
	{
		szo = GetLastError();
		if ( ERROR_MORE_DATA==szo ) {
			isMoreData = TRUE;
		}
		szo = 0;
	}
	return szo;
}

unsigned int PipeImpl::write(HANDLE h, char *dat, unsigned int sz)
{
	if ( NULL==dat || 0==sz ) {
		return	FALSE;
	}

	DWORD wlen = 0;
	BOOL br = TRUE;
	WriteFile(h, dat, sz, &wlen, NULL);
	FlushFileBuffers(h); 
	return	wlen;
}

unsigned int PipeImpl::read(HANDLE h, char *buf, unsigned int sz, BOOL &isMoreData)
{
	BOOL br = FALSE;
	DWORD rlen = 0;

	isMoreData = FALSE;
	br = ReadFile(h, buf, sz, &rlen, NULL);
	if ( FALSE==br || 0==rlen ) {
		rlen = 0;
	}
	return	rlen;
}

void PipeImpl::get_pipe_name(char *buf, unsigned int len)
{
	char tc[MAX_PATH], tu[MAX_PATH];
	DWORD tlen;

	tlen = sizeof(tc);
	if ( FALSE==GetComputerName(tc, &tlen) ) {
		strncpy(tc, "unknown_com", sizeof(tc));
		tc[sizeof(tc)-1] = '\0';
	}

	tlen = sizeof(tu);
	if ( FALSE==GetUserName(tu, &tlen) ) {
		strncpy(tu, "unknown_user", sizeof(tu));
		tc[sizeof(tu)-1] = '\0';
	}

	_snprintf(buf, len, "\\\\.\\pipe\\%s_%s_%s", PIPE_NAME_PREFIX, tc, tu);
	buf[len-1] = '\0';
}

void PipeImpl::disconnect(HANDLE h)
{
	if ( INVALID_HANDLE_VALUE!=h ) {
		DisconnectNamedPipe(h); 
	}
}

void PipeImpl::close(HANDLE h)
{
	if ( INVALID_HANDLE_VALUE!=h ) {
		CloseHandle(h);
	}
}

BOOL PipeImpl::fetch(HANDLE h)
{
	return	ConnectNamedPipe(h, NULL)
		? TRUE : (GetLastError()==ERROR_PIPE_CONNECTED); 
}

ChewingPipeServ::ChewingPipeServ()
{
	char name[MAX_PATH];

	PipeImpl::get_pipe_name(name, sizeof(name));
	_h = PipeImpl::create(name);
}

ChewingPipeServ::~ChewingPipeServ()
{
	PipeImpl::close(_h);
}

BOOL ChewingPipeServ::fetch()
{
	return	PipeImpl::fetch(_h);
}

BOOL ChewingPipeServ::read()
{
	BOOL bl = FALSE;
	_szd = PipeImpl::read(_h, (char*)_d, sizeof(_d), bl);
	if ( sizeof(_d)!=_szd ) {
		return	FALSE;
	}
	return	TRUE;
}

BOOL ChewingPipeServ::reply(unsigned int dat)
{
	unsigned int len = PipeImpl::write(_h, (char*)&dat, sizeof(dat));
	if ( sizeof(dat)!=len ) {
		return	FALSE;
	}
	return	TRUE;
}

BOOL ChewingPipeServ::get_param(unsigned int &session, unsigned int &cmd, unsigned int &dat)
{
	if ( sizeof(_d)!=_szd ) {
		return	FALSE;
	}

	session = _d[0];
	cmd = _d[1];
	dat = _d[2];
	return	TRUE;
}

void ChewingPipeServ::disconnect()
{
	PipeImpl::disconnect(_h);
}

unsigned int ChewingPipeClnt::SendMessageReply(unsigned int session, unsigned int cmd, unsigned int dat)
{
	if ( INVALID_HANDLE_VALUE==_h ) {
		return	0;
	}

	BOOL bl = FALSE;
	unsigned int d[3], ret=0, len=0;
	d[0] = session;
	d[1] = cmd;
	d[2] = dat;

	len = PipeImpl::send_and_read(_h, (char*)d, sizeof(d), (char*)&ret,
		sizeof(ret), bl);
	if ( sizeof(ret)!=len ) {
		return	0;
	}
	return ret;
}

ChewingPipeClnt::ChewingPipeClnt()
{
	PipeImpl::get_pipe_name(_n, sizeof(_n));
	_h = PipeImpl::open(_n);
}

ChewingPipeClnt::~ChewingPipeClnt()
{
	PipeImpl::disconnect(_h);
	PipeImpl::close(_h);
}

