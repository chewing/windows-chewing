#pragma once

#include <windows.h>
#include "imm.h"

class IMEWnd
{
public:
	IMEWnd( HWND existing );
	IMEWnd( HWND imeUIWnd, LPCTSTR classname );
	~IMEWnd(void);
	void Show(){ ShowWindow(hwnd, SW_SHOWNA); }
	void Hide(){ ShowWindow(hwnd, SW_HIDE); }
	BOOL isWindow(){ return IsWindow(hwnd); }
	void refresh()	{	InvalidateRect( hwnd, NULL, FALSE );	}
	virtual void getSize(int* w, int* h){	*w=0; *h=0;	}
	HIMC getIMC(){	return (HIMC)GetWindowLong( GetParent(hwnd), IMMGWL_IMC );	}
	HWND getHwnd(){	return hwnd;	}
	BOOL isVisible(){ return IsWindowVisible(hwnd); }
protected:
	HWND hwnd;
	void assocWndObj(void);
public:
	static IMEWnd* getAssocWndObj(HWND hwnd);
	void Move(int x, int y)
	{
		int w, h;
		getSize(&w, &h);
		MoveWindow( hwnd, x, y, w, h, TRUE );
	}
};
