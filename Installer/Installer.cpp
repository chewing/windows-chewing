// Installer.cpp : 定義應用程式的進入點。
//
#include <windows.h>
#include <imm.h>
#include "Installer.h"
#include <winreg.h>

// 全域變數:
HINSTANCE hInst;								// 目前執行個體

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	char path[MAX_PATH];
	GetSystemDirectory(path, MAX_PATH);
	SetCurrentDirectory(path);

	HKL kl;

	if( strstr( lpCmdLine, "/uninstall" ) )
	{
/*		HKEY hk = NULL;
		if( ERROR_SUCCESS != RegOpenKey( HKEY_LOCAL_MACHINE, "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\ChewingIME", &hk ) )
			return -1;
		RegCloseKey(hkey);
*/		
	}
	else
	{
		kl = ImmInstallIME( "Chewing.ime", "中文 (繁體) - 新酷音輸入法" );
		char regpath[256];
		wsprintf( regpath, "SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts\\%X", kl );
	}


	return 0;
}