#pragma once

#include "IMEWnd.h"
#include "IMCLock.h"

#include <string>

using namespace std;

class CompWnd : public IMEWnd
{
public:
	CompWnd(void);
	~CompWnd(void);
	static BOOL RegisterClass(void);
protected:
	static LRESULT WndProc(HWND hwnd , UINT msg, WPARAM wp , LPARAM lp);
	void OnPaint(IMCLock& imc, PAINTSTRUCT& ps);
	HFONT font;
public:
	void setFont(LOGFONT* lf);
	HFONT getFont(){	return font;	}
	void getSize(int* w, int* h);
	string getDisplayedCompStr(IMCLock& imc);
	int getDisplayedCursorPos(IMCLock& imc);
	string getDisplayedCompStr(){	return getDisplayedCompStr(IMCLock(getIMC()));	}
	int getDisplayedCursorPos(){	return getDisplayedCursorPos(IMCLock(getIMC()));	}
	int indexToXPos( string compStr, int idx);
public:
	bool create(HWND imeUIWnd);
};
