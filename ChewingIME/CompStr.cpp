#include "Compstr.h"
#include ".\compstr.h"

CompStr::CompStr(void)
{
	cs.dwCompStrOffset = DWORD( &compStr[0] - (TCHAR*)this);
	cs.dwCompStrLen = 0;
	memset( compStr, 0, sizeof(compStr) );
	cs.dwResultStrOffset = DWORD( &resultStr[0] - (TCHAR*)this);
	cs.dwResultStrLen;
	memset( resultStr, 0, sizeof(resultStr) );
}

CompStr::~CompStr(void)
{
}

void CompStr::setCompStr(LPCTSTR compstr)
{
	_tcscpy( compStr, compstr );
	cs.dwCompStrLen = _tcslen( compStr );
}

void CompStr::setResultStr(LPCTSTR resultstr)
{
	_tcscpy( resultStr, resultstr );
	cs.dwResultStrLen = _tcslen( resultStr );
}

void CompStr::appendCompStr(LPCTSTR str)
{
	_tcscat( compStr, str );
	cs.dwCompStrLen += _tcslen( str );
}

void CompStr::setCursorPos(int pos)
{
	cs.dwCursorPos = pos;
}
