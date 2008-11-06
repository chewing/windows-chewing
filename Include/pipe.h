#ifndef _LIBCHEWING_WIN32_PIPE_IMPLEMENT_
#define _LIBCHEWING_WIN32_PIPE_IMPLEMENT_


#include <windows.h>



class ChewingPipeServ
{
public:
        BOOL fetch();

	BOOL read();

	BOOL reply(unsigned int dat);

	BOOL get_param(unsigned int &session, unsigned int &cmd, unsigned int &dat);

	void disconnect();

	ChewingPipeServ();
	~ChewingPipeServ();

private:
	unsigned int _d[3];
	unsigned int _szd;
	HANDLE _h;
};



class ChewingPipeClnt
{
public:
	unsigned int SendMessageReply(unsigned int session, unsigned int cmd, unsigned int dat);

	ChewingPipeClnt();
	~ChewingPipeClnt();

private:
	char _n[MAX_PATH];
	HANDLE _h;
};



#endif
