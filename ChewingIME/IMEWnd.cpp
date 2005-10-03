#include "imewnd.h"
#include "ChewingIME.h"
#include ".\imewnd.h"

IMEWnd::IMEWnd()
{
}



IMEWnd::~IMEWnd(void)
{
	DestroyWindow(hwnd);
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


void IMEWnd::OnLButtonDown(WPARAM wp, LPARAM lp)
{
	oldPos = MAKEPOINTS(lp);
	SetCapture(hwnd);
}

void IMEWnd::OnLButtonUp(WPARAM wp, LPARAM lp)
{
	ReleaseCapture();
}

void IMEWnd::OnMouseMove(WPARAM wp, LPARAM lp)
{
	if( GetCapture() != hwnd )
		return;
	POINTS pt = MAKEPOINTS(lp);
	RECT rc;
	GetWindowRect( hwnd, &rc );
	OffsetRect( &rc, (pt.x - oldPos.x), (pt.y - oldPos.y) );
//	MoveWindow( hwnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE );
	Move(rc.left, rc.top);
}

void IMEWnd::Move(int x, int y)
{
	int w, h;
	getSize(&w, &h);

	RECT rc;
	SystemParametersInfo(SPI_GETWORKAREA, 0, (PVOID)&rc, 0 );
	if( x < rc.left )
		x = rc.left;
	else if( (x + w) > rc.right )
		x = rc.right - w;

	if( y < rc.top )
		y = rc.top;
	else if( (y + h) > rc.bottom )
		y = rc.bottom - h;

	MoveWindow( hwnd, x, y, w, h, TRUE );
}
