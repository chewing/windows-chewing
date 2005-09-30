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
	void setCompStr( string compstr );
	void setCursorPos(int pos);
	string getCompStr(){ return compStr; }
protected:
	static LRESULT WndProc(HWND hwnd , UINT msg, WPARAM wp , LPARAM lp);
	void OnPaint(PAINTSTRUCT& ps);
	string compStr;
	HFONT font;
	int cursorPos;
public:
	void setFont(LOGFONT* lf);
	HFONT getFont(){	return font;	}
	void getSize(int* w, int* h);
protected:
	int indexToXPos(int idx);
public:
	void showCand(void);
};
