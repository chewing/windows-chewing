// ChewingServer.h: interface for the ChewingServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHEWINGSERVER_H__BA0A82BD_DE77_4E22_9B6C_443D7E3D22EB__INCLUDED_)
#define AFX_CHEWINGSERVER_H__BA0A82BD_DE77_4E22_9B6C_443D7E3D22EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#pragma warning(disable : 4786) 

#include <windows.h>
//#include <list>
#include <map>
#include "chewingpp.h"

using namespace std;

typedef	int (Chewing::*ChewingMemberFuncII)(int);
typedef	int (Chewing::*ChewingMemberFuncIV)(void);
typedef	char* (Chewing::*ChewingMemberFuncCI)(int);
typedef	char* (Chewing::*ChewingMemberFuncCV)(void);
typedef	char (Chewing::*GetSelKeyFunc)(int);
typedef	void (Chewing::*SetSelKeyFunc)(char*);

#define		chewingServerClassName		_T("ChewingServer")
#define		CHEWINGSERVER_BUF_SIZE		4096

class ChewingServer  
{
public:
	enum ChewingCmd
	{
		cmdFirst = (WM_APP + 1),
		// int (void), void (void)
		cmdSpace = cmdFirst,
		cmdEsc,
		cmdEnter,
		cmdDelete,
		cmdBackspace,
		cmdTab,
		cmdShiftLeft,
		cmdShiftRight,
		cmdShiftSpace,
		cmdRight,
		cmdLeft,
		cmdUp,
		cmdDown,
		cmdHome,
		cmdEnd,
		cmdCapslock,
		cmdDoubleTab,
		cmdCommitReady,
		cmdBufferLen,
		cmdCursorPos,
		cmdPointStart,
		cmdPointEnd,
		cmdKeystrokeRtn,
		cmdKeystrokeIgnore,
		cmdChineseMode,
		cmdGetFullShape,
		cmdCandidate,
		cmdChoicePerPage,
		cmdTotalChoice,
		cmdTotalPage,
		cmdCurrentPage,
		cmdShowMsgLen,
		cmdGetAddPhraseForward,

		// int (int) or void (int)
		cmdKey,
		cmdCtrlNum,
		cmdNumPad,
		cmdCtrlOption,
		cmdGetSelKey,

		cmdSetFullShape,
		cmdSetSpaceAsSelection,
		cmdSetAddPhraseForward,
		cmdSetKeyboardLayout,
		cmdSetHsuSelectionKeyType,
		cmdSetCandPerPage,
		cmdSetEscCleanAllBuf,
        cmdAdvanceAfterSelection,
        cmdEasySymbolInput,

		// char* (void) or unsigned char* (void)
		cmdZuinStr,
		cmdCommitStr,
		cmdBuffer,
		cmdIntervalArray,
		cmdShowMsg,

		// char* (int)
		cmdSelection,

		// void (char*)
		cmdSetSelKey,

		cmdLast, 

		cmdAddClient,
		cmdRemoveClient,
        cmdEcho,
        cmdLastPhoneSeq,
		cmdReloadSymbolTable
	};

public:
	bool run();
	ChewingServer();
	virtual ~ChewingServer();
protected:
	LRESULT parseChewingCmd( UINT cmd, int param, Chewing* chewing );
	bool startServer();
	HWND hwnd;
	LRESULT wndProc( UINT msg, WPARAM wp, LPARAM lp);
	static LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	map<unsigned int, Chewing*> chewingClients;

	static ChewingMemberFuncCI chewingCmdTable[];
	HANDLE sharedMem;
	UINT checkTimer;
	void checkNewVersion(void);
};


//  Generate class, event name according to the user account.
char* _gen_event_name(char *buf, int szbuf, const char *prefix);

#endif // !defined(AFX_CHEWINGSERVER_H__BA0A82BD_DE77_4E22_9B6C_443D7E3D22EB__INCLUDED_)
