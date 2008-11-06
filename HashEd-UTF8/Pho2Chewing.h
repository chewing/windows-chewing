// stdafx.h : 可在此標頭檔中包含標準的系統 Include 檔，
// 或是經常使用卻很少變更的專案專用 Include 檔案
//

//#pragma once

#include <iostream>
#include <tchar.h>
#include <windows.h>

// TODO: 在此參考您的程式所需要的其他標頭
#include "chewing-utf8-util.h"
#include "global.h"

typedef struct
{
	char zuin[8];
	char keysel[5];
}ZuinKeyCompareTable;

typedef struct
{
	int lastfreq;
	int lasttime;
	int maxfreq;
	int systemfreq;
	int phonenum;
	uint16 phoneserial;
	int bits;
}chewing;