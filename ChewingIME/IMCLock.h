#pragma once

#include <windows.h>
#include "imm.h"

class CompStr;
class CandList;
class IMEData;

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
	IMEData* data;
public:
	CompStr* getCompStr(void);
	CandList* getCandList(void);
	IMEData* getData(void);
	INPUTCONTEXT* getIC(){	return ic;	}
};
