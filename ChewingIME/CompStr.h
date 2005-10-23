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
	TCHAR readStr[256];
	BYTE readAttr[ 256/sizeof(TCHAR) ];
	DWORD readClause[2];

	TCHAR compStr[256];
	BYTE compAttr[ 256/sizeof(TCHAR) ];
	DWORD compClause[2];

	TCHAR resultReadStr[256];
	DWORD resultReadClause[2];

	TCHAR resultStr[256];
	DWORD resultClause[2];

// private data;
	TCHAR showMsg[50];

public:
	TCHAR* getCompStr()	{	return &compStr[0];	}
	TCHAR* getResultStr()	{	return &resultStr[0];	}
	TCHAR* getShowMsg()	{	return &showMsg[0];	}
	DWORD getCursorPos()	{	return cs.dwCursorPos;	}
	void setCompStr(LPCTSTR compstr);
	void setResultStr(LPCTSTR resultstr);
	void setShowMsg(LPCTSTR showmsg);
	void setCursorPos(int pos);
	void setZuin(const char* zuin);
	void beforeGenerateMsg(void);
};
