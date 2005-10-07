#pragma once

#include "IMEChildWnd.h"

class StatusWnd : public IMEChildWnd
{
public:
	StatusWnd(void);
	~StatusWnd(void);
	static BOOL registerClass(void);
protected:
	HWND toolbar;
	HBITMAP bgbmp;
	static LRESULT wndProc( HWND hwnd , UINT msg, WPARAM wp , LPARAM lp);
	void onPaint(PAINTSTRUCT& ps);

public:
	void getSize(int* w, int* h);
protected:
	void getToolbarSize(int* w, int* h);
public:
	bool create(HWND imeUIWnd);
	void enableChiEng(bool enable=true);
	void disableChiEng(){	enableChiEng(false);	}
	void toggleChiEngMode(HIMC hIMC);
	void toggleShapeMode(HIMC hIMC);
	void updateIcons(HIMC hIMC);
};
