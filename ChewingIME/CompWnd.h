#pragma once

#include "IMEChildWnd.h"
#include "IMCLock.h"

#include <string>

using namespace std;

class CompWnd : public IMEChildWnd
{
public:
	CompWnd(void);
	~CompWnd(void);
	static BOOL registerClass(void);
protected:
	static LRESULT wndProc( HWND hwnd , UINT msg, WPARAM wp , LPARAM lp);
	int indexToXPos( string compStr, int idx);
	void onPaint(IMCLock& imc, PAINTSTRUCT& ps);
	HFONT font;
public:
	void setFont(LOGFONT* lf);
	HFONT getFont(){	return font;	}
	void getSize(int* w, int* h);
	string getDisplayedCompStr(IMCLock& imc);
	int getDisplayedCursorPos(IMCLock& imc);
	string getDisplayedCompStr(){	return getDisplayedCompStr(IMCLock(getIMC()));	}
	int getDisplayedCursorPos(){	return getDisplayedCursorPos(IMCLock(getIMC()));	}
public:
	bool create(HWND imeUIWnd);
	void getRelativeCandPos(IMCLock& imc, POINT* pt);
	void getCandPos(IMCLock& imc, POINT* pt);
};
