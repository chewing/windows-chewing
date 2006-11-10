#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include <tchar.h>
#include <wininet.h>
#include <shellapi.h>

#define COMPILE_MULTIMON_STUBS
#include <multimon.h>	// For MonitorFromWindow

#include "resource.h"

TCHAR url[100] = _T("http://chewing.csie.net/download/win32/");
TCHAR* url_file_name = url + 39; // dir path is 39 bytes long
char CSig[] = "ISTA";

TCHAR notifier_class[] = _T("ChewingUpdate");
const char* version = NULL;
const char* released_time = NULL;
const char* change_log = NULL;
const char* news_url = NULL;
HINSTANCE hinst = NULL;

bool has_new_version = false;

LRESULT CALLBACK notifier_proc( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
{
	switch( msg ) {
	case WM_ERASEBKGND:
		{
			HDC dc = HDC(wp);
			RECT rc;
			GetClientRect( hwnd, &rc );
			SetBkColor( dc, GetSysColor(COLOR_INFOBK) );
			ExtTextOut( dc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL );
			FrameRect( dc, &rc, GetSysColorBrush(COLOR_WINDOWFRAME ));
			return TRUE;
		}
	case WM_CTLCOLORSTATIC:
		{
			HDC dc = HDC(wp);
			HWND item = (HWND)lp;
			LONG id = GetWindowLong( item, GWL_ID );
			if ( id == IDC_TITLE ) {
				SetBkColor( dc, GetSysColor(COLOR_ACTIVECAPTION) );
				SetTextColor( dc, GetSysColor(COLOR_CAPTIONTEXT) );
				return (LRESULT)GetSysColorBrush(COLOR_ACTIVECAPTION);
			}
			else if ( id == IDC_CHANGELOG ) {
				SetBkColor( dc, GetSysColor(COLOR_WINDOW) );
				return (LRESULT)GetSysColorBrush(COLOR_WINDOW);			
			}
			else {
				SetBkColor( dc, GetSysColor(COLOR_INFOBK) );
				return (LRESULT)GetSysColorBrush(COLOR_INFOBK);
			}
		}
	case WM_COMMAND:
		switch( LOWORD(wp) )
		{
		case IDOK:
			_tcscpy( url_file_name, _T("win32-chewing-") );
			_tcscat( url_file_name, version );
			_tcscat( url_file_name, _T(".exe") );
			ShellExecute( NULL, "open", url, NULL, NULL, SW_SHOWNORMAL );
			DestroyWindow( hwnd );
			return TRUE;
		case IDCANCEL:
			DestroyWindow( hwnd );
			return TRUE;
		case IDC_WEBPAGE:
			if( news_url && *news_url ) {
				ShellExecute( NULL, "open", news_url, NULL, NULL, SW_SHOWNORMAL );
			}
		}
		break;
	case WM_INITDIALOG:
		SetDlgItemText( hwnd, IDC_VERSION, version );
		SetDlgItemText( hwnd, IDC_RELEASEDTIME, released_time );
		SetDlgItemText( hwnd, IDC_CHANGELOG, change_log );
		SendMessage( GetDlgItem(hwnd, IDC_INFOICON), 
					 STM_SETICON, (WPARAM)LoadIcon(NULL, IDI_INFORMATION), 0 );
		if( ! has_new_version )	{
			SetDlgItemText( hwnd, IDC_TITLE, "您的新酷音已經是最新版本" );
			EnableWindow( GetDlgItem( hwnd, IDOK), FALSE );
		}
		return TRUE;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return FALSE;
}


bool get_working_area(RECT* rc, HWND app_wnd)
{
	HMONITOR mon = MonitorFromWindow( app_wnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi;
	mi.cbSize = sizeof(mi);
	if( GetMonitorInfo(mon, &mi) )
		*rc = mi.rcWork;
	return true;
}

void show_notify()
{
	HWND hwnd = CreateDialog( hinst, LPCTSTR(IDD_POPUP), HWND_DESKTOP, 
							  (DLGPROC)notifier_proc );
	SetWindowLong( hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) | WS_BORDER );

	RECT wa_rc, dlg_rc;
	GetWindowRect( hwnd, &dlg_rc );
	get_working_area( &wa_rc, hwnd );
	wa_rc.right -= (dlg_rc.right - dlg_rc.left + 10);
	wa_rc.bottom -= (dlg_rc.bottom - dlg_rc.top);
	SetWindowPos( hwnd, HWND_TOPMOST, wa_rc.right, wa_rc.bottom, 0, 0, SWP_NOSIZE );
	ShowWindow( hwnd, SW_SHOWNA );

	MSG msg;
	while( GetMessage( &msg, NULL, 0, 0 ) ){
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
}

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPTSTR    lpCmdLine,
                   int       nCmdShow)
{
	HINTERNET inet = InternetOpen( "ChewingUpdate", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if( !inet )
		return -1;
	_tcscpy( url_file_name, "version-info" );
	HINTERNET ifile = InternetOpenUrl( inet, url, NULL, 0, INTERNET_FLAG_NO_CACHE_WRITE, 0 );
	if( ifile )
	{
		char buf[4096];
		memset( buf, 0, sizeof(buf) );

		char* pbuf = buf;
		char* end = buf + sizeof(buf);
		DWORD len = 0;

		while( InternetReadFile( ifile, pbuf, int(end - pbuf), &len) && len )
		{
			pbuf += len;
		}
		InternetCloseHandle(ifile);

		// version-info should in following simple format:
		// 1st line: Version string (required)
		// 2nd line: Release date (required)
		// 3rd line: URL of relating web page (required)
		// 4th line to end of file: Change log (optional)

		if ( strncmp(buf, CSig, strlen(CSig))!=0 ) {
			return	-1;
		}
		version = strtok(buf, "\r\n ");
		if( ! version )
			return -1;

		version += strlen(CSig);

		// Check if we are using the latest version
		HKEY hk = NULL;
		char cur_ver[32] = "";
		if( ERROR_SUCCESS == RegOpenKey( HKEY_LOCAL_MACHINE, 
			_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\新酷音輸入法"), &hk) )
		{
			DWORD size = sizeof(cur_ver);
			DWORD type = REG_SZ;
			RegQueryValueEx( hk, "DisplayVersion", 0, &type, (LPBYTE)cur_ver, &size );
			RegCloseKey( hk );
		}

		has_new_version = strcmp( cur_ver, version ) ? true : false;

		// No new version and in silent mode
		if( ! has_new_version && strstr(lpCmdLine, "/silent") )
			return 0;
		released_time = strtok( NULL, "\r\n" );
		news_url = strtok( NULL, "\r\n" );
		change_log = strtok( NULL, "" );

		hinst = hInstance;
		show_notify();
		return 0;
	}
	InternetCloseHandle(inet);
	return (ifile ? 0 : -1);
}

