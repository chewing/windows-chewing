#include <windows.h>
#include <tchar.h>
#include "ChewingServer.h"

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	if( FindWindow( chewingServerClassName, NULL) )
		return -1;
	ChewingServer server;
	return ! server.run();
}



