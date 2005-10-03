#include ".\imclock.h"

IMCLock::IMCLock(HIMC hIMC) : himc(hIMC), compStr(NULL), candList(NULL)
{
	ic = himc ? ImmLockIMC(himc) : NULL;
}

IMCLock::~IMCLock(void)
{
	if( ic )
	{
		if( compStr )
			ImmUnlockIMCC(ic->hCompStr);
		if( candList )
			ImmUnlockIMCC(ic->hCandInfo);
		ImmUnlockIMC(himc);
	}
}

CompStr* IMCLock::getCompStr(void)
{
	if( compStr )
		return compStr;
	if( ic )
		return (CompStr*)ImmLockIMCC(ic->hCompStr);
	return NULL;
}

CandList* IMCLock::getCandList(void)
{
	if( candList )
		return candList;
	if( ic )
		return (CandList*)ImmLockIMCC(ic->hCandInfo);
	return NULL;
}

