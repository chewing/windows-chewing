#pragma once

#include "IMEWnd.h"

class StatusWnd : public IMEWnd
{
public:
	StatusWnd( HWND imeUIWnd );
	~StatusWnd(void);
	static BOOL RegisterClass(void);
protected:
	static LRESULT WndProc(HWND hwnd , UINT msg, WPARAM wp , LPARAM lp);
	void OnPaint(PAINTSTRUCT& ps);

};
