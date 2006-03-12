#include <windows.h>
#include <tchar.h>

// lpCmd: file to convert
int WINAPI WinMain(HINSTANCE hinst, HINSTANCE hprev, LPSTR lpCmd, int nShow )
{
	HANDLE hHashFile = CreateFile( lpCmd, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
	if( hHashFile == INVALID_HANDLE_VALUE )
		return -1;

	DWORD size = GetFileSize( hHashFile, NULL );
	DWORD buflen = (size * 3 / 2) + 1;
	char *buf = new char[ buflen ];
	if( buf )	{
		if( ReadFile( hHashFile, buf, size, &size, NULL ) )	{
			CloseHandle( hHashFile );

			DWORD usize = MultiByteToWideChar( 950, 0, buf, size + 1, NULL, 0 );
			wchar_t* ubuf = new wchar_t[usize];
			if( ubuf )	{
				usize = MultiByteToWideChar( 950, 0, buf, size + 1, ubuf, usize );
				size = WideCharToMultiByte( CP_UTF8, 0, ubuf, usize, NULL, 0, NULL, NULL );
				if ( buf!=NULL ) {
					delete [] buf;
					buf = new char[size+1];
				}
				size = WideCharToMultiByte( CP_UTF8, 0, ubuf, usize, buf, size, NULL, NULL );

				TCHAR* name = _tcsrchr( lpCmd, '\\' );
				if( name )
					strcpy( (name + 1), _T("uhash.dat") );

				hHashFile = CreateFile( lpCmd, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL );
				if( hHashFile != INVALID_HANDLE_VALUE )	{
					WriteFile( hHashFile, buf, size, &size, NULL );
					CloseHandle( hHashFile );
				}
				delete []ubuf;
			}
		}
		delete []buf;
	}
	return 0;
}

