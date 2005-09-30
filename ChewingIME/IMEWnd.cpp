#include "imewnd.h"
#include "ChewingIME.h"
#include ".\imewnd.h"

IMEWnd::IMEWnd( HWND imeUIWnd, LPCTSTR classname )
{
	hwnd = CreateWindowEx(0, classname, NULL,
					WS_DISABLED | WS_POPUP,
					0, 0, 0, 0, imeUIWnd, NULL, g_dllInst, NULL);
	this->assocWndObj();
}

IMEWnd::IMEWnd( HWND existing )
{
	hwnd = existing;
	this->assocWndObj();
}

IMEWnd::~IMEWnd(void)
{
}

void IMEWnd::assocWndObj(void)
{
	if( sizeof(IMEWnd*) > sizeof(LONG) )	// pointer size > 32 bit (64 bit environment)
	{
		void* ptr = this;
		LONG ptrparts[] = {0, 0};
		memcpy( ptrparts, &ptr, sizeof(IMEWnd*) );
		SetWindowLong( hwnd, GWL_USERDATA, ptrparts[0] );
		SetWindowLong( hwnd, IMMGWL_PRIVATE, ptrparts[1] );		
	}
	else
		SetWindowLong( hwnd, GWL_USERDATA, (LONG)this );
}

IMEWnd* IMEWnd::getAssocWndObj(HWND hwnd)
{
	if( sizeof(void*) > sizeof(LONG) )	// pointer size > 32 bit (64 bit environment)
	{
		IMEWnd* ptr = NULL;
		LONG ptrparts[] = {0, 0};
		ptrparts[0] = GetWindowLong( hwnd, GWL_USERDATA  );
		ptrparts[1] = GetWindowLong( hwnd, IMMGWL_PRIVATE );		
		memcpy( &ptr, ptrparts, sizeof(IMEWnd*) );
		return ptr;
	}
	else
		return (IMEWnd*)GetWindowLong( hwnd, GWL_USERDATA );

	return NULL;
}

