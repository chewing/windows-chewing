// HashEdDlg.h : header file
//

#if !defined(AFX_HASHEDDLG_H__35818C20_FC7D_4643_ABE9_61774E0F5CBC__INCLUDED_)
#define AFX_HASHEDDLG_H__35818C20_FC7D_4643_ABE9_61774E0F5CBC__INCLUDED_

#include "chashcontext.h"


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CHashEdDlg dialog

class CHashEdDlg : public CDialog
{
// Construction
public:
	CHashEdDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CHashEdDlg)
	enum { IDD = IDD_HASHED_DIALOG };
	CButton	m_btnAbout;
	CButton	m_banner;
	CButton	m_btnSaveAs;
	CButton	m_btnFindPhrase;
	CListCtrl	m_listing;
	CButton	m_btnAddPhrase;
	CButton	m_btnDelPhrase;
	CButton	m_Import;
	CEdit	m_edtPhrase;
	CButton	m_btnSave;
	//}}AFX_DATA

    CHashContext    m_context;
    uint16  m_PhoneSeq[MAX_PHONE_SEQ_LEN+1];
    int     m_NumPhoneSeq;
    char    m_string[(MAX_PHONE_SEQ_LEN+1)*2];
    CString m_strHashFolder;



	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHashEdDlg)
	protected:          
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CHashEdDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnUpdateNewPhraseEdit();
	afx_msg void OnChangeNewPhraseEdit();
	afx_msg void OnAddPhrase();
	afx_msg void OnKillfocusNewPhraseEdit();
	afx_msg void OnFindPhrase();
	afx_msg void OnImport();
	afx_msg void OnSave();
	afx_msg void OnDelPhrase();
	afx_msg void OnSaveAs();
	afx_msg void OnAbout();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    BOOL _isDbcsString(char *str);
    int _isMatch(char *string, int id);
    int find(char *tok, BOOL &bExactMatch, int hi=-1, int lo=-1);
    void GetHashLocation();
    void _save(const char *pathfile, BOOL bSaveNoSwap);
    void _enable_buttons(BOOL bEnable);
    void ReloadListCtrl();
    void UpdateBanner(const char *message=NULL);
    void SelItem(int idx);
    void Reload(char* hashfile, bool bClearContext);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HASHEDDLG_H__35818C20_FC7D_4643_ABE9_61774E0F5CBC__INCLUDED_)
