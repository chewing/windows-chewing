#pragma once

#include "IMEWnd.h"
#include <string>

using namespace std;

class CompWnd : public IMEWnd
{
public:
	CompWnd(HWND imeUIWnd);
	~CompWnd(void);
	static BOOL RegisterClass(void);
protected:
	static LRESULT WndProc(HWND hwnd , UINT msg, WPARAM wp , LPARAM lp);
	void OnPaint(PAINTSTRUCT& ps);
	HFONT font;
public:
	void setFont(LOGFONT* lf);
	HFONT getFont(){	return font;	}
	void getSize(int* w, int* h);
	string getDisplayedCompStr(void);
	int getDisplayedCursorPos(void);
	void showCand(void);
protected:
	int indexToXPos( string compStr, int idx);
};
