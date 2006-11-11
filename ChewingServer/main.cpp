#include <windows.h>
#include <tchar.h>
#include "ChewingServer.h"


int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	char classname[512];
	_gen_event_name(classname, sizeof(classname), chewingServerClassName);
	if( FindWindow( classname, NULL) )
		return -1;
	ChewingServer server;
	return ! server.run();
}



