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

	HKEY hk = NULL;
	if( ERROR_SUCCESS != RegCreateKeyEx( HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\新酷音輸入法", 0, 
			NULL, 0, KEY_ALL_ACCESS , NULL, &hk, NULL) )
		hk = NULL;

	if( strstr( lpCmdLine, "/uninstall" ) )
	{
		if( hk )
		{
			DWORD type = REG_DWORD, size = sizeof(DWORD);
			if(	ERROR_SUCCESS == RegQueryValueEx( hk, "KeyboardLayout", 0, &type, (LPBYTE)&kl, &size ) )
			{
				UnloadKeyboardLayout( kl );
				char klstr[10];
				wsprintf( klstr, "%X", kl );
				char regpath[256];
				lstrcpy( regpath, "Keyboard Layout\\Preload" );
				HKEY hk2 = NULL;
				if( ERROR_SUCCESS == RegOpenKey( HKEY_CURRENT_USER, regpath, &hk2 ) )
				{
					for( int i = 1; i <= 100; ++i )
					{
						char num[4];
						wsprintf( num, "%d", i );
						type = REG_SZ;	size = sizeof(regpath);
						if(	ERROR_SUCCESS != RegQueryValueEx( hk2, num, 0, &type, (LPBYTE)regpath, &size ) )
							continue;
						if( 0 == lstrcmp( regpath, klstr ) )
						{
							RegDeleteValue( hk2, num );
							break;
						}
					}
					RegCloseKey(hk2);
				}

				wsprintf( regpath, "SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts\\%s", klstr );
				RegDeleteKey( HKEY_LOCAL_MACHINE, regpath );
			}
		}
	}
	else
	{
		kl = ImmInstallIME( "Chewing.ime", "中文 (繁體) - 新酷音輸入法" );
		if( hk )
			RegSetValueEx( hk, "KeyboardLayout", 0, REG_DWORD, (LPBYTE)&kl, sizeof(DWORD) );
	}

	RegCloseKey( hk );

	return 0;
}