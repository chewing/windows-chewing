#include ".\imclock.h"

IMCLock::IMCLock(HIMC hIMC) : himc(hIMC), compStr(NULL), candList(NULL), data(NULL)
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
		if( data )
			ImmUnlockIMCC(ic->hPrivate);
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

IMEData* IMCLock::getData(void)
{
	if( data )
		return data;
	if( ic )
		return (IMEData*)ImmLockIMCC(ic->hPrivate);
	return NULL;
}
