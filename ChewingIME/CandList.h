#pragma once

#include <windows.h>
#include <tchar.h>
#include "imm.h"

class CandList
{
public:
	CandList(void);
	~CandList(void);
	void setTotalCount(int c){	cl.dwCount = c;	}
	int getTotalCount(){ return cl.dwCount; }
	void setPageSize(int s){ cl.dwPageSize = s; }
	int getPageSize(){ return cl.dwPageSize; }
	void setPageStart(int i){ cl.dwPageStart = i;
							cl.dwSelection = i;}
	int getPageStart(){ return cl.dwPageStart; }
	void setCand(int i, LPCWSTR cand);
	LPCWSTR getCand(int i){ return candStr[i]; }
protected:
    CANDIDATEINFO  ci;
    CANDIDATELIST  cl;
    DWORD offset[240-1];
	wchar_t candStr[240][8];	// unicode string
};
