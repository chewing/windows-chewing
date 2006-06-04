// ChewingClient.h: interface for the ChewingClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHEWINGCLIENT_H__E54E6740_17FB_4E1D_B912_751FF053FFD1__INCLUDED_)
#define AFX_CHEWINGCLIENT_H__E54E6740_17FB_4E1D_B912_751FF053FFD1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include "ChewingServer.h"

class ChewingClient  
{
public:
	ChewingClient( int kbLayout, bool spaceAsSel = true, const char* selKeys="1234567890", bool AdvAfterSel=true, bool EscCleanAllBuf = false );
	~ChewingClient();

public:
	void SetKeyboardLayout(int kb);
	void SetHsuSelectionKeyType(int type);

	int Space();
	int Esc();
	int Enter();
	int Delete();
	int Backspace();
	int Tab();
	int ShiftLeft();
	int ShiftRight();
	int ShiftSpace();
	int Right();
	int Left();
	int Up();
	int Down();
	int Home();
	int End();
	int Capslock();
	int Key(unsigned int code);
	int CtrlNum(unsigned int code);
	int NumPad(unsigned int code);
	int CtrlOption(unsigned int code);
	int DoubleTab();

	// Return the i-th selection key, i >= 0.
	char SelKey(int i);
	void SelKey(char* selkey);
    void SetAdvanceAfterSelection(bool bDo);

	char* ZuinStr();
	char* CommitStr();
	int   CommitReady();

	char* Buffer();
	int   BufferLen();

	int CursorPos();
	int PointStart();
	int PointEnd();
	int KeystrokeRtn();
	int KeystrokeIgnore();

	int ChineseMode();

	// CandidateWindow-related routines
	int Candidate();
	int ChoicePerPage();
	int TotalChoice();
	int TotalPage();
	int CurrentPage();

	// Return the i-th selection wchar_t, i >= 0.
	char* Selection(int i);
	void SetFullShape(bool full);
	bool GetFullShape(void);

    unsigned int  EchoFromServer();
    bool CheckServer();

	unsigned char* GetIntervalArray(int& len);

protected:
	unsigned char* GetDataFromSharedMem(int len);
	HANDLE sharedMem;
	LPARAM chewingID;
	HWND serverWnd;
    TCHAR filemapName[MAX_PATH];
    char *pSelKeys;

public:
	void SetSpaceAsSelection(bool spaceAsSelection);
	void SetEscCleanAllBuf( bool escCleanAllBuf );
	void ConnectServer(void);
protected:
	bool spaceAsSelection;
    bool advAfterSelection;
	bool escCleanAllBuf;
	int keyLayout;
public:
	int ShowMsgLen(void);
	char* ShowMsg(void);
	void SetAddPhraseForward(bool add_forward);
	int GetAddPhraseForward(void);
	void SetCandPerPage(int len);
	void ReloadSymbolTable(void);
};

#endif // !defined(AFX_CHEWINGCLIENT_H__E54E6740_17FB_4E1D_B912_751FF053FFD1__INCLUDED_)
