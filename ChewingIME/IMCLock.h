#pragma once

#include <windows.h>
#include "imm.h"

class CompStr;
class CandList;

class IMCLock
{
public:
	IMCLock(HIMC hIMC);
	~IMCLock(void);
protected:
	HIMC himc;
	INPUTCONTEXT* ic;
	CompStr* compStr;
	CandList* candList;
public:
	CompStr* getCompStr(void);
	CandList* getCandList(void);
	INPUTCONTEXT* getIC(){	return ic;	}
};
