// Installer.cpp : 定義應用程式的進入點。
//
#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <sddl.h>
#include <Aclapi.h>
#include <imm.h>
#include <winreg.h>
#include "Installer.h"

// 全域變數:
HINSTANCE hInst;								// 目前執行個體



PSID name_to_PSID(char *name)
{
	SID_NAME_USE snu;
	char	*sidbuf, domainname[512];
	DWORD	sidlen, dnamelen;

	sidlen = 512;
	sidbuf = (char*)calloc(1, sidlen);
	dnamelen = sizeof(domainname);
	if ( FALSE==LookupAccountName(NULL, name, sidbuf, &sidlen,
		domainname, &dnamelen, &snu) ) {
			free(sidbuf);
			return	NULL;
		}
		return	(PSID) sidbuf;
}

PACL	generate_acl()
{
	PACL	pAcl = NULL;
	DWORD	szAcl = 0;
	PSID	sids[2];
	DWORD	AceMask;

	sids[0] = name_to_PSID("everyone");
	sids[1] = name_to_PSID("administrators");
	szAcl = 
		sizeof(ACL)+
		((sizeof(ACCESS_ALLOWED_ACE)-sizeof(DWORD))*sizeof(sids)/sizeof(PSID))+
		GetLengthSid(sids[0])+
		GetLengthSid(sids[1]);
	pAcl = (PACL) LocalAlloc(LPTR, szAcl);
	if ( 0==InitializeAcl(pAcl, szAcl, ACL_REVISION) ) {
		LocalFree(pAcl);
		return	NULL;
	}
	AceMask = (1<<30) | (1<<31);
	if ( FALSE==AddAccessAllowedAce(pAcl, ACL_REVISION, AceMask, sids[0]) ) {
		LocalFree(pAcl);
		return	NULL;
	}
	AceMask = (1<<28);
	if ( FALSE==AddAccessAllowedAce(pAcl, ACL_REVISION, AceMask, sids[1]) ) {
		LocalFree(pAcl);
		return	NULL;
	}
	return	pAcl;
};

int enable_access(char *filename)
{
	int	retval = 1;
	PACL myacl = generate_acl();

	if ( ERROR_SUCCESS!=SetNamedSecurityInfo(filename, SE_FILE_OBJECT, 
		DACL_SECURITY_INFORMATION, 
		NULL, NULL, myacl, NULL) ) {
		retval = 0;
	}

	if ( NULL!=myacl ) {
		LocalFree(myacl);
	}
	return retval;
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

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	HKL kl;

	HKEY hk = NULL;
	if( ERROR_SUCCESS != RegCreateKeyEx( HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\新酷音輸入法", 0, 
			NULL, 0, KEY_ALL_ACCESS , NULL, &hk, NULL) )
		hk = NULL;

	if( strstr( lpCmdLine, "/uninstall" ) )
	{
		char temp[1024];
		_gen_event_name(temp, sizeof(temp), "ChewingServer");
		HWND hwnd = FindWindow(temp, NULL);
		if ( hwnd ) {
			SendMessage(hwnd, WM_DESTROY, 0, 0);
		}


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

				// Windows NT only, 9x will be supported in the future
				if( (GetVersion() < 0x80000000) )
				{
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
				}

				wsprintf( regpath, "SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts\\%s", klstr );
				RegDeleteKey( HKEY_LOCAL_MACHINE, regpath );
			}
		}
	}
	else if ( strstr( lpCmdLine, "/privilege" ) )
	{
		enable_access("ch_index.dat");
		enable_access("dict.dat");
		enable_access("us_freq.dat");
		enable_access("ph_index.dat");
		enable_access("fonetree.dat");
	}
	else
	{
		char path[MAX_PATH];
		GetSystemDirectory(path, MAX_PATH);

		lstrcat( path, "\\Chewing.ime" );
		kl = ImmInstallIME( path, 
			(GetVersion() < 0x80000000) ? "中文 (繁體) - 新酷音輸入法" : "新酷音輸入法" );
		if( hk )
			RegSetValueEx( hk, "KeyboardLayout", 0, REG_DWORD, (LPBYTE)&kl, sizeof(DWORD) );
	}

	RegCloseKey( hk );

	return 0;
}