#include ".\imclock.h"

IMCLock::IMCLock(HIMC hIMC)
 : himc(hIMC), ic(NULL), compStr(NULL), candList(NULL)
{
	lock();
}

IMCLock::~IMCLock(void)
{
	unlock();
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


bool IMCLock::lock(void)
{
	if( !ic )
		ic = himc ? ImmLockIMC(himc) : NULL;
	return !!ic;
}

void IMCLock::unlock(void)
{
	if( ic )
	{
		if( compStr )
		{
			ImmUnlockIMCC(ic->hCompStr);
			compStr = NULL;
		}
		if( candList )
		{
			ImmUnlockIMCC(ic->hCandInfo);
			candList = NULL;
		}
		ImmUnlockIMC(himc);
		ic = NULL;
	}
}
