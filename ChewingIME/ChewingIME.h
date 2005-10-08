#ifndef	_PCMANIME_H_
#define	_PCMANIME_H_

#include <windows.h>
#include <tchar.h>
#include "imm.h"
#include "ChewingPP.h"

extern HINSTANCE g_dllInst;
extern bool g_isWindowNT;

const LPCTSTR g_pcmanIMEClass = _T("ChewingIME");
const LPCTSTR g_compWndClass = _T("ChewingComp");
const LPCTSTR g_candWndClass = _T("ChewingCand");
const LPCTSTR g_statusWndClass = _T("ChewingStatus");

#define	WM_IME_RELOADCONFIG		(WM_APP+1)
extern DWORD g_keyboardLayout;
extern DWORD g_candPerRow;

extern Chewing* g_chewing;

inline BOOL IsImeMessage(UINT msg)
{
	return ( (msg >= WM_IME_STARTCOMPOSITION && msg <= WM_IME_KEYLAST)
			|| (msg >= WM_IME_SETCONTEXT && msg <= WM_IME_KEYUP) );
}

void LoadConfig();
void ToggleChiEngMode(HIMC hIMC);
void ToggleFullShapeMode(HIMC hIMC);

BOOL GenerateIMEMessage(HIMC hIMC, UINT msg, WPARAM wp=0, LPARAM lp=0);

void ToggleChiEngMode(HIMC hIMC);

void ConfigureChewingIME(HWND parent);

typedef struct _tagTRANSMSG {
	UINT message;
	WPARAM wParam;
	LPARAM lParam;
} TRANSMSG, *LPTRANSMSG;

struct KeyInfo
{
	UINT repeatCount:16;
	UINT scanCode:8;
	UINT isExtended:1;
	UINT reserved:4;
	UINT contextCode:1;
	UINT prevKeyState:1;
	UINT isKeyUp:1;	// transition state
};

inline KeyInfo GetKeyInfo(LPARAM lparam)
{	return *(KeyInfo*)&lparam;	}

inline bool IsKeyDown(BYTE keystate){ return !!(keystate & 0xF0); }
inline bool IsKeyToggled(BYTE keystate){ return !!(keystate & 0x0F); }

BOOL ProcessKeyEvent( UINT key, KeyInfo ki, const BYTE* keystate );

#endif
