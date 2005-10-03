#ifndef	_PCMANIME_H_
#define	_PCMANIME_H_

#include <windows.h>
#include <tchar.h>
#include "imm.h"
#include "ChewingPP.h"

extern HINSTANCE g_dllInst;
extern bool g_isWindowNT;

const LPCTSTR g_pcman_ime_class = _T("ChewingIME");
const LPCTSTR g_comp_wnd_class = _T("ChewingComp");
const LPCTSTR g_cand_wnd_class = _T("ChewingCand");
const LPCTSTR g_status_wnd_class = _T("ChewingStatus");

#define	WM_IME_RELOADCONFIG		(WM_APP+1)
extern DWORD g_keyboardLayout;
extern DWORD g_candPerRow;
extern bool g_isChinese;
extern POINT g_statusWndPos;

class CompWnd;
extern CompWnd g_compWnd;
class CandWnd;
extern CandWnd g_candWnd;
class StatusWnd;
extern StatusWnd g_statusWnd;

extern Chewing* g_chewing;

inline BOOL IsImeMessage(UINT msg)
{
	return ( (msg >= WM_IME_STARTCOMPOSITION && msg <= WM_IME_KEYLAST)
			|| (msg >= WM_IME_SETCONTEXT && msg <= WM_IME_KEYUP) );
}

BOOL GenerateIMEMessage(HIMC hIMC, UINT msg, WPARAM wp=0, LPARAM lp=0);

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
