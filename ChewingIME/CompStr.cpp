#include "Compstr.h"
#include ".\compstr.h"

CompStr::CompStr(void)
{
	cs.dwSize = sizeof(CompStr);

	cs.dwCompStrOffset = DWORD( &compStr[0] - (TCHAR*)this);
	cs.dwCompStrLen = 0;
	memset( compStr, 0, sizeof(compStr) );

	cs.dwCompAttrOffset = DWORD( &compAttr[0] ) - DWORD(this);
	cs.dwCompAttrLen = 0;
	memset( compAttr, 0, sizeof(compAttr) );

	cs.dwResultStrOffset = DWORD( &resultStr[0] - (TCHAR*)this);
	cs.dwResultStrLen = 0;
	memset( resultStr, 0, sizeof(resultStr) );

	cs.dwCompReadStrOffset = DWORD( &readStr[0] - (TCHAR*)this);
	cs.dwCompReadStrLen = 0;
	memset( readStr, 0, sizeof(readStr) );

	cs.dwCompReadAttrOffset = DWORD( &readAttr[0] ) - DWORD(this);
	cs.dwCompReadAttrLen = 0;
	memset( readAttr, 0, sizeof(readAttr) );

	cs.dwDeltaStart = 0;
}

CompStr::~CompStr(void)
{
}

void CompStr::setCompStr(LPCTSTR compstr)
{
	_tcscpy( compStr, compstr );
	cs.dwCompStrLen = _tcslen( compStr );
	cs.dwCompAttrLen = cs.dwCompStrLen;
	memset( (char*)compAttr, ATTR_CONVERTED, cs.dwCompAttrLen );
}

void CompStr::setResultStr(LPCTSTR resultstr)
{
	_tcscpy( resultStr, resultstr );
	cs.dwResultStrLen = _tcslen( resultStr );
}


void CompStr::setCursorPos(int pos)
{
	TCHAR* next = _tcsinc( &compStr[cs.dwCursorPos] );
	memset( &compAttr[cs.dwCursorPos], ATTR_CONVERTED, (next - &compStr[cs.dwCursorPos]) );

	cs.dwCursorPos = pos;

	next = _tcsinc( (TCHAR*)&compStr[pos] );
	memset( &compAttr[pos], ATTR_TARGET_CONVERTED, (next - &compStr[pos]) );
}

void CompStr::setZuin(const char* zuin)
{
	_tcscpy( readStr, zuin );
	cs.dwCompReadStrLen = _tcslen(readStr);

	cs.dwCompReadAttrLen = cs.dwCompReadStrLen;
	if(cs.dwCompReadStrLen)
		memset( (char*)readAttr, ATTR_TARGET_CONVERTED, cs.dwCompReadStrLen );
}

void CompStr::beforeGenerateMsg(void)
{
	TCHAR* sinsert = compStr + cs.dwCursorPos;

	memmove( sinsert + cs.dwCompReadStrLen, 
		sinsert, cs.dwCompStrLen - cs.dwCursorPos );
	_tcsncpy( sinsert, readStr, cs.dwCompReadStrLen );
	cs.dwCompStrLen += cs.dwCompReadStrLen;
	compStr[cs.dwCompStrLen] = '\0';

	BYTE* ainsert = compAttr + cs.dwCursorPos;
	memmove( ainsert + cs.dwCompReadAttrLen, 
		ainsert, cs.dwCompAttrLen - cs.dwCursorPos);
	memcpy( ainsert, readAttr, cs.dwCompReadAttrLen );
	cs.dwCompAttrLen += cs.dwCompReadAttrLen;
}
