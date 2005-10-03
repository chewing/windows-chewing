#pragma once

#include "IMEWnd.h"

class IMCLock;
class CandWnd : public IMEWnd
{
public:
	CandWnd(void);
	~CandWnd(void);

	static BOOL RegisterClass(void);
	void setFont( HFONT f){	font = f;	}
protected:
	static LRESULT WndProc(HWND hwnd , UINT msg, WPARAM wp , LPARAM lp);
	void OnPaint(HIMC hIMC, PAINTSTRUCT& ps);
public:
	void getSize(int* w, int* h);
	void updateSize(void);
	HFONT font;
	bool create(HWND imeUIWnd);
	void show(void);
};
