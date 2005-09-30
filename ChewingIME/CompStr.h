#pragma once

#include <windows.h>
#include <tchar.h>
#include "imm.h"

class CompStr
{
public:
	CompStr(void);
	~CompStr(void);
protected:
	COMPOSITIONSTRING cs;
	TCHAR compStr[256];
	TCHAR resultStr[256];
public:
	bool isEmpty()	{	return !cs.dwCompStrLen;	}
	TCHAR* getCompStr()	{	return &compStr[0];	}
	TCHAR* getResultStr()	{	return &resultStr[0];	}
	DWORD getCursorPos()	{	return cs.dwCursorPos;	}
	void setCompStr(LPCTSTR compstr);
	void setResultStr(LPCTSTR resultstr);
	void appendCompStr(LPCTSTR str);
	void setCursorPos(int pos);
};
