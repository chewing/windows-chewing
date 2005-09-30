#ifndef _DRAW_UTIL_H_
#define _DRAW_UTIL_H_

#include <windows.h>

void FillSolidRect( HDC dc, LPRECT rc, COLORREF color );
void FillSolidRect( HDC dc, int l, int t, int w, int h, COLORREF color );
void Draw3DBorder(HDC hdc, LPRECT rc, COLORREF light, COLORREF dark, int width = 1);

#endif