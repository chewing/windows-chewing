// XPToolbar.h: interface for the XPToolbar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XPTOOLBAR_H__B78EEDE7_6867_422E_B87B_5291E8EEFFF6__INCLUDED_)
#define AFX_XPTOOLBAR_H__B78EEDE7_6867_422E_B87B_5291E8EEFFF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "window.h"
#include <tchar.h>
#include <vector>
#include <string>

using namespace std;

class XPToolbarBtn
{
friend class XPToolbar;
protected:
	XPToolbarBtn(UINT cmdid, int image, LPCTSTR tooltip_text=_T(""), int btnstate=0)
		: iImage(image), id(cmdid), tooltip(tooltip_text), state(btnstate){}
	union{
		int iImage;
		HICON hIcon;
	};
	UINT id;
	int state;
	string tooltip;
};

class Tooltip;

class XPToolbar : virtual public Window
{
public:
	int prevHilightBtn;
	void setBtnImage(int idx, HICON icon);
	void setCmdTarget(HWND target){	cmdTarget = target;	}
	int hitTest( int x, int y );
	void getSize(int* w, int* h);
	int addBtn(UINT id, int iImage, LPCTSTR tooltip=_T(""));
	int addBtn(UINT id, HICON hIcon, LPCTSTR tooltip=_T("") ){	return addBtn(id, (int)hIcon, tooltip);	}
	void drawBtn(HDC dc, int idx);
	void setTheme(HBITMAP bmp);
	bool create(HWND parent, UINT id, LONG style, int x, int y, int w, int h);
	static void unregisterClass();
	static BOOL registerClass();
	XPToolbar(int gripperw = 10, int btnw = 20, int btnh = 21);
	~XPToolbar();

protected:
	void onCaptureChanged(LPARAM lp);
	void onPaint( PAINTSTRUCT& ps );
	LRESULT wndProc(UINT msg, WPARAM wp, LPARAM lp);
	static LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	bool onLButtonDown(WPARAM wp, LPARAM lp);
	bool onLButtonUp(WPARAM wp, LPARAM lp);
	bool onMouseMove(WPARAM wp, LPARAM lp);

	HWND cmdTarget;
	HBITMAP themeBmp;
	int gripperW;
	int btnW;
	int btnH;
	int curPressedBtn;
	vector<XPToolbarBtn> buttons;
public:
	void setTooltip(Tooltip* tip);
	bool getBtnRect(int idx, RECT& rc);
protected:
	Tooltip* tooltip;
};

#endif // !defined(AFX_XPTOOLBAR_H__B78EEDE7_6867_422E_B87B_5291E8EEFFF6__INCLUDED_)
