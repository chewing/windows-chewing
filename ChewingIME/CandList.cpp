#include ".\candlist.h"

CandList::CandList(void)
{
	ci.dwSize = sizeof(CandList);
	ci.dwCount = 1;
	ci.dwOffset[0] = (size_t(&cl) - size_t(this));
	ci.dwPrivateSize = 0;

	for( int i = 0; i < (sizeof(offset)/sizeof(DWORD)); ++i )
		offset[i] = ((DWORD)candStr[i] - (DWORD)&cl);

	cl.dwSize = sizeof(cl)+sizeof(offset)+sizeof(candStr);
	cl.dwStyle = IME_CAND_READ;
	cl.dwSelection = 0;
}

CandList::~CandList(void)
{
}

void CandList::setCand( int i, LPCTSTR cand )
{
	_tcscpy( candStr[i], cand );
}

