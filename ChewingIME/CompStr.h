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
	TCHAR readStr[16];
	BYTE compAttr[ 256*sizeof(TCHAR) ];
	BYTE readAttr[ 16*sizeof(TCHAR) ];
public:
	bool isEmpty()
	{
		return !cs.dwCompStrLen 
			&& !cs.dwCompReadStrLen 
			&& !cs.dwResultStrLen;
	}
	TCHAR* getCompStr()	{	return &compStr[0];	}
	TCHAR* getResultStr()	{	return &resultStr[0];	}
	DWORD getCursorPos()	{	return cs.dwCursorPos;	}
	void setCompStr(LPCTSTR compstr);
	void setResultStr(LPCTSTR resultstr);
	void setCursorPos(int pos);
	void setZuin(const char* zuin);
	void beforeGenerateMsg(void);
};
