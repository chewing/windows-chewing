#pragma once

#include <windows.h>

#include "IMEUIWnd.h"
#include "CompWnd.h"
#include "CandWnd.h"
#include "StatusWnd.h"

class IMEData
{
public:
	IMEData(void);
	~IMEData(void);

	CompWnd compWnd;
	CandWnd candWnd;
	StatusWnd statusWnd;
};
