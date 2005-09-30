#pragma once

#include "IMEWnd.h"

class CandWnd : public IMEWnd
{
public:
	CandWnd( HWND imeUIWnd );
	~CandWnd(void);

	static BOOL RegisterClass(void);
	void setFont( HFONT f){	font = f;	}
protected:
	static LRESULT WndProc(HWND hwnd , UINT msg, WPARAM wp , LPARAM lp);
	void OnPaint(PAINTSTRUCT& ps);
public:
	void getSize(int* w, int* h);
	void updateSize(void);
	HFONT font;
};
