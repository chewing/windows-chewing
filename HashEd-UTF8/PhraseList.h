#pragma once

#include <windows.h>

#include <vector>
#include <utility>
#include <string>

// This class is used to replace list view provided by Windows

class CPhraseList
{
public:
	CPhraseList(void);
	~CPhraseList(void);
	static BOOL init(void);
	static CPhraseList* fromHandle(HWND hwnd){ return (CPhraseList*)GetWindowLongPtr(hwnd, GWL_USERDATA); }
	HWND getHwnd() { return hwnd; }
	int itemFromPoint( int x, int y );
	void setCurSel(int idx);
	int getCurSel(void);
	int count(void);
	void insertItem( int pos, std::wstring text, void* user_data );
	std::wstring getItemText( int idx );
	void* getItemData( int idx );
	void deleteItem( int idx );
	void clear(void);
	void getItemRect( int idx, RECT& rc );

	void lockUpdate(void);
	void unlockUpdate(void);

protected:
	LRESULT CPhraseList::wndProc( UINT msg, WPARAM wp, LPARAM lp );
	static LRESULT wndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	void onSize( WPARAM wp, long cx, long cy );
	void onVScroll( int code, int pos );
	void onPaint(void);
	void onLButtonDown(WPARAM wp, int x, int y);
	void recalcLayout(void);
	void onSetFont(HFONT hfont);
	void onMouseWheel(int delta);
	bool onKeyDown(int key, LPARAM lp);

protected:
	HWND hwnd;
	int sel;	// Current selected item;
	int itemsPerRow;
	int lineCount;
	std::vector< std::pair<std::wstring, void*> > data;
	bool lock;
	int itemHeight;
	HFONT font;
public:
	void ensureItemVisible(int idx);
};

