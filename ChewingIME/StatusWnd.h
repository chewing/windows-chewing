#pragma once

#include "IMEWnd.h"

class StatusWnd : public IMEWnd
{
public:
	StatusWnd(void);
	~StatusWnd(void);
	static BOOL RegisterClass(void);
protected:
	HWND toolbar;
	static LRESULT WndProc(HWND hwnd , UINT msg, WPARAM wp , LPARAM lp);
	void OnPaint(PAINTSTRUCT& ps);

public:
	void getSize(int* w, int* h);
protected:
	void getToolbarSize(int* w, int* h);
public:
	bool create(HWND imeUIWnd);
};
