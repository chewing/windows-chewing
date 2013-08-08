// HashEdDlg.h : header file
//

#if !defined(AFX_HASHEDDLG_H__35818C20_FC7D_4643_ABE9_61774E0F5CBC__INCLUDED_)
#define AFX_HASHEDDLG_H__35818C20_FC7D_4643_ABE9_61774E0F5CBC__INCLUDED_

#include "chashcontext.h"
#include "Chewingpp.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CHashEdDlg dialog

using namespace std;

class CPhraseList;

class CHashEdDlg
{
// Construction
public:
	CHashEdDlg();	// standard constructor
	int DoModal(){
		return DialogBoxParam( (HINSTANCE)GetModuleHandle(NULL), LPCTSTR(IDD), 
								HWND_DESKTOP, (DLGPROC)wndProc, LPARAM(this) );
	}

// Dialog Data
	enum { IDD = IDD_HASHED_DIALOG };
	HWND	m_btnAbout;
	HWND	m_banner;
	HWND	m_btnSaveAs;
	HWND	m_btnFindPhrase;
	HWND	m_btnAddPhrase;
	HWND	m_btnDelPhrase;
	HWND	m_Import;
	HWND	m_edtPhrase;
	HWND	m_btnSave;
	CPhraseList* phraseList;	// Phrase List control

    CHashContext    m_context;
    uint16_t  m_PhoneSeq[MAX_PHONE_SEQ_LEN+1];
    int     m_NumPhoneSeq;
    char    m_string[(MAX_PHONE_SEQ_LEN+1)*2];
    char    m_msgStr[256];
    TCHAR   m_strHashFolder[ MAX_PATH + 1 ];

// Implementation
protected:
	HWND m_hWnd;

	HICON m_hIcon;

	static BOOL wndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	LRESULT wndProc(UINT msg, WPARAM wp, LPARAM lp);
	// Generated message map functions
	virtual BOOL OnInitDialog();
	void OnUpdateNewPhraseEdit();
	void OnChangeNewPhraseEdit();
	void OnAddPhrase();
	void OnKillfocusNewPhraseEdit();
	void OnFindPhrase();
	void OnImport();
	void OnSave();
	void OnDelPhrase();
	void OnSaveAs();
	void OnAbout();
	void OnTimer(UINT nIDEvent);

    int _isMatch(const char *string, int id);
    int find(const char *tok, BOOL &bExactMatch, int hi=-1, int lo=-1);
    void GetHashLocation();
    void _save(const char *pathfile, BOOL bSaveNoSwap);
    void _enable_buttons(BOOL bEnable);
    void ReloadListCtrl();
    void UpdateBanner(const char *message=NULL);
    void SelItem(int idx);
    void Reload(char* hashfile, bool bClearContext);
    char *GetStringFromTab(int id);

public:
	void onCommand(UINT cmd);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HASHEDDLG_H__35818C20_FC7D_4643_ABE9_61774E0F5CBC__INCLUDED_)
