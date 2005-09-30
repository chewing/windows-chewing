#pragma once

#include "IMEWnd.h"

class CompWnd;
class CandWnd;
class StatusWnd;

class IMEUIWnd : public IMEWnd
{
public:
	IMEUIWnd( HWND existing );
	~IMEUIWnd(void);
	LRESULT OnImeNotify(WPARAM wp, LPARAM lp);
protected:
	HWND hwnd;

	CompWnd* compWnd;
	CandWnd* candWnd;
	StatusWnd* statusWnd;
};
