// stdafx.h : �i�b�����Y�ɤ��]�t�зǪ��t�� Include �ɡA
// �άO�g�`�ϥΫo�ܤ��ܧ󪺱M�ױM�� Include �ɮ�
//

//#pragma once

#include <iostream>
#include <tchar.h>
#include <windows.h>

// TODO: �b���Ѧұz���{���һݭn����L���Y
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
	uint16_t phoneserial;
	int bits;
}chewing;