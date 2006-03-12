#pragma once

#include <windows.h>

#include <vector>
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
	int insertItem( int pos, std::wstring text );
	std::wstring getItem( int idx );
	void deleteItem( int idx );
	void clear(void);
	void scrollToItem(int idx);
	void getItemRect( int idx, RECT& rc );

	void sortItems(bool ascending);
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

protected:
	HWND hwnd;
	int sel;	// Current selected item;
	int itemsPerRow;
	int lineCount;
	std::vector< std::wstring > data;
	bool lock;
	int itemHeight;
	HFONT font;
};

