#pragma once

#include <windows.h>

#include "CompWnd.h"
#include "CandWnd.h"
#include "StatusWnd.h"

class IMEUI
{
public:
	IMEUI(HWND hUIWnd);
	~IMEUI(void);

	CompWnd compWnd;
	CandWnd candWnd;
	StatusWnd statusWnd;
	HWND hwnd;

	static BOOL IMEUI::registerUIClasses();
protected:
	static LRESULT CALLBACK wndProc( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp );
	LRESULT onIMENotify( HIMC hIMC, WPARAM wp , LPARAM lp );
	LRESULT wndProc( UINT msg, WPARAM wp, LPARAM lp);

	void closeStatusWnd(void);
	void openStatusWnd(HIMC hIMC);
	void openCandWnd(void);
	void updateCandWnd(void);
	void closeCandWnd(void);
	void setCandWndPos(HIMC hIMC);
public:
	static bool getWorkingArea(RECT* rc);
};
