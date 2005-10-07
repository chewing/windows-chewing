#pragma once

#include <windows.h>
#include "imm.h"

class IMEChildWnd
{
public:
	IMEChildWnd();
	virtual ~IMEChildWnd(void);
	void show()
	{
		if( hwnd )
			ShowWindow(hwnd, SW_SHOWNA);
	}
	void hide(){ ShowWindow(hwnd, SW_HIDE); }
	BOOL isWindow(){ return IsWindow(hwnd); }
	void refresh()	{	InvalidateRect( hwnd, NULL, FALSE );	}
	virtual void getSize(int* w, int* h){	*w=0; *h=0;	}
	HIMC getIMC(){	return (HIMC)GetWindowLong( GetParent(hwnd), IMMGWL_IMC );	}
	static HIMC getIMC(HWND hwnd){	return (HIMC)GetWindowLong( GetParent(hwnd), IMMGWL_IMC );	}
	HWND getHwnd(){	return hwnd;	}
	BOOL isVisible(){ return IsWindowVisible(hwnd); }
	void move(int x, int y);
//	static IMEChildWnd* getAssocWndObj(HWND hwnd);
protected:
	HWND hwnd;
	POINTS oldPos;
//	void assocWndObj(void);
public:
protected:
	void onLButtonDown(WPARAM wp, LPARAM lp);
	void onLButtonUp(WPARAM wp, LPARAM lp);
	void onMouseMove(WPARAM wp, LPARAM lp);
public:
	virtual bool create(HWND imeUIWnd) = 0;
	void destroy(void);
};
