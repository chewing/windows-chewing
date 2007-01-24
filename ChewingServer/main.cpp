#include <windows.h>
#include <tchar.h>
#include "ChewingServer.h"



int existed()
{
	HANDLE h = CreateMutex(NULL, TRUE, "CHEWING_WIN32_SERVER");
	if ( ERROR_ALREADY_EXISTS==GetLastError() ) {
		return	1;
	}
	if ( NULL==h ) {
		return	1;
	}

	return	0;
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	if ( existed() ) {
		return	0;
	}

	ChewingServer server;

	return ! server.run();
}



