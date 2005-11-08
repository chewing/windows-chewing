// HashEdDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HashEd.h"
#include "HashEdDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHashEdDlg dialog

CHashEdDlg::CHashEdDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHashEdDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHashEdDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHashEdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHashEdDlg)
	DDX_Control(pDX, IDC_ABOUT, m_btnAbout);
	DDX_Control(pDX, IDC_BANNER, m_banner);
	DDX_Control(pDX, IDC_SAVE_AS, m_btnSaveAs);
	DDX_Control(pDX, IDC_FIND_PHRASE, m_btnFindPhrase);
	DDX_Control(pDX, IDC_PHRASE_LIST, m_listing);
	DDX_Control(pDX, IDC_ADD_PHRASE, m_btnAddPhrase);
	DDX_Control(pDX, IDC_DEL_PHRASE, m_btnDelPhrase);
	DDX_Control(pDX, IDC_IMPORT, m_Import);
	DDX_Control(pDX, IDC_NEW_PHRASE_EDIT, m_edtPhrase);
	DDX_Control(pDX, IDC_SAVE, m_btnSave);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CHashEdDlg, CDialog)
	//{{AFX_MSG_MAP(CHashEdDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_UPDATE(IDC_NEW_PHRASE_EDIT, OnUpdateNewPhraseEdit)
	ON_EN_CHANGE(IDC_NEW_PHRASE_EDIT, OnChangeNewPhraseEdit)
	ON_BN_CLICKED(IDC_ADD_PHRASE, OnAddPhrase)
	ON_EN_KILLFOCUS(IDC_NEW_PHRASE_EDIT, OnKillfocusNewPhraseEdit)
	ON_BN_CLICKED(IDC_FIND_PHRASE, OnFindPhrase)
	ON_BN_CLICKED(IDC_IMPORT, OnImport)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_DEL_PHRASE, OnDelPhrase)
	ON_BN_CLICKED(IDC_SAVE_AS, OnSaveAs)
	ON_BN_CLICKED(IDC_ABOUT, OnAbout)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHashEdDlg message handlers

BOOL CHashEdDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
    _enable_buttons(FALSE);
    SetTimer(553, 10, NULL);
    
    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CHashEdDlg::ReloadListCtrl()
{
    HASH_ITEM *pItem;
    int lop, idx, count = m_context.get_phrase_count();

    m_listing.DeleteAllItems();
    for ( lop=0; lop<count; ++lop )
    {
        pItem = m_context.get_phrase_by_id(lop);
        idx = m_listing.InsertItem(lop, pItem->data.wordSeq);
        m_listing.SetItemData(idx, (DWORD)pItem);
    }
}

void CHashEdDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CHashEdDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CHashEdDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CHashEdDlg::OnUpdateNewPhraseEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
}

void CHashEdDlg::OnChangeNewPhraseEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here

}

int CHashEdDlg::_isMatch(char *string, int id)
{
    HASH_ITEM *pItem = (HASH_ITEM*) m_listing.GetItemData(id);
    return  strcmp(string, pItem->data.wordSeq);
}

int CHashEdDlg::find(char *tok, BOOL &bExactMatch, int hi, int lo)
{
    int p, cmp;
    
    p = m_listing.GetItemCount()-1;
    if ( hi==-1 )
        hi = p;
    if ( lo==-1 )
        lo=0;

    if ( lo<0 ) lo = 0;
    if ( hi>p ) hi = p;

    bExactMatch = FALSE;
    while ( hi>=lo )
    {
        p = (hi+lo)/2;
        cmp = _isMatch(tok, p);
        
        if ( cmp<0 )
            hi = p-1;
        else if ( cmp>0 )
            lo = p+1;
        else
        {
            bExactMatch = TRUE;
            return   p;
        }
    };

    return p;
}

void CHashEdDlg::SelItem(int idx) 
{
    m_listing.SetItemState(idx, LVIS_FOCUSED|LVIS_SELECTED,
                                LVIS_FOCUSED|LVIS_SELECTED);
	m_listing.EnsureVisible(idx, FALSE);
}

void CHashEdDlg::OnAddPhrase() 
{
	// TODO: Add your control notification handler code here
    HASH_ITEM *pItem;
	int	tt;
    BOOL bMatch;

    if ( strlen(m_string)==0 )    return;
	if ( _isDbcsString(m_string)==FALSE )
	{
		AfxMessageBox("請輸入中文字串。");
		return;
	}

	if ( (int)strlen(m_string)/2!=m_NumPhoneSeq )
	{
		AfxMessageBox(
            "請依照以下方法輸入:\n\n"
            "1. 使用新酷音輸入法輸入中文。\n"
            "2. 一次完成整個句子，才按下 enter 鍵。\n");
		return;
	}

    tt = find(m_string, bMatch);
    if ( bMatch==TRUE )
	{
        CString strtemp;
        SelItem(tt);
        strtemp.Format("「%s」已經存在。", m_string);
		AfxMessageBox(strtemp);
		return;
	}

	pItem = m_context.append_phrase(m_string, m_PhoneSeq);

    tt = m_listing.InsertItem(m_listing.GetItemCount(), m_string);
    m_listing.SetItemState(tt, LVIS_FOCUSED|LVIS_SELECTED,
                               LVIS_FOCUSED|LVIS_SELECTED);
	tt = m_listing.EnsureVisible(tt, FALSE);
    m_listing.SetItemData(tt, (DWORD)pItem);

    strcpy(m_string, "");
    m_NumPhoneSeq = 0;
    m_PhoneSeq[0] = 0;
    m_edtPhrase.SetWindowText("");
    UpdateBanner();
}

BOOL CHashEdDlg::_isDbcsString(char *str)
{
	char *pNextChar, *pCurChar;

	pCurChar = str;
	while ( *pCurChar!=NULL )
	{
		pNextChar = CharNext(pCurChar);
		if ( (int)(pNextChar-pCurChar)!=2 )
		{
			return	FALSE;
		}
		pCurChar = pNextChar;
	};

	return	TRUE;
}

void CHashEdDlg::OnKillfocusNewPhraseEdit() 
{
	// TODO: Add your control notification handler code here
    m_edtPhrase.GetWindowText(m_string, sizeof(m_string));
    m_string[sizeof(m_string)-1] = '\0';
	m_NumPhoneSeq = m_context._get_phone_seq_from_server(m_PhoneSeq);
}



void CHashEdDlg::OnFindPhrase() 
{
	// TODO: Add your control notification handler code here
    char    tstring[(MAX_PHONE_SEQ_LEN+1)*2];
    int     idx, beep;
    BOOL    bMatch;

    m_edtPhrase.GetWindowText(tstring, sizeof(tstring));
    tstring[sizeof(tstring)-1] = '\0';
    if ( strlen(tstring)==0 )   return;
	if ( _isDbcsString(tstring)==FALSE )
	{
		AfxMessageBox("請輸入中文字串。");
		return;
	}
    
    idx = find(tstring, bMatch);

    beep = MB_ICONQUESTION;
    if ( bMatch==TRUE )
    {
        beep = MB_OK;
    }
    MessageBeep(beep);

    SelItem(idx);
}

void CHashEdDlg::OnImport() 
{
	// TODO: Add your control notification handler code here
    CFileDialog FileDlg(TRUE, NULL/*"dat\0\0"*/, "hash",
                        OFN_ENABLESIZING|OFN_HIDEREADONLY|
                        OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST,
                        "Chewing hash data file(*.dat)|*.dat|All files(*.*)|*.*||");
    if ( FileDlg.DoModal()==IDCANCEL ) 
    {
        return;
    }

    _enable_buttons(FALSE);
    
    Reload((LPSTR)(LPCTSTR)FileDlg.GetPathName(), false);

    _enable_buttons(TRUE);
    UpdateBanner();
}

void CHashEdDlg::Reload(char* hashfile, bool bClearContext)
{
    CString strtemp;

    strtemp.Format("正在載入 %s", hashfile);
    UpdateBanner((LPCTSTR)strtemp);

    m_context.load_hash(hashfile, bClearContext);
    m_context.sort_phrase();
    m_context.arrange_phrase();
    ReloadListCtrl();

    UpdateBanner();
}

void CHashEdDlg::GetHashLocation()
{
    char    hashdir[MAX_PATH];
    LPITEMIDLIST pidl;
    m_strHashFolder = "c:\\";
	if( S_OK == SHGetSpecialFolderLocation( NULL, CSIDL_APPDATA, &pidl ) )
	{
		SHGetPathFromIDList(pidl, hashdir);
		_tcscat(hashdir, _T("\\Chewing") );
        m_strHashFolder = hashdir;
    }
}

void CHashEdDlg::OnSave() 
{
	// TODO: Add your control notification handler code here
    CString strTemp, strpath;

    if ( AfxMessageBox("確定要以目前正在編輯的內容取代本地詞庫檔案嗎？\n"
                       "此過程將會包含重新載入詞庫，可能需要一點時間。\n", 
                       MB_YESNO)!=IDYES )
    {
        return;
    }
    //  ui...
    _enable_buttons(FALSE);
    m_btnSave.GetWindowText(strTemp);
    m_btnSave.SetWindowText("存檔中...");

    //
    strpath = m_strHashFolder+"\\hash.dat";
    _save((LPCTSTR)strpath, FALSE);
    
    strpath = m_strHashFolder;
    strpath += "\\hash.dat";
    Reload((LPSTR)(LPCTSTR)strpath, true);

    //  ui...
    _enable_buttons(TRUE);
    m_btnSave.SetWindowText(strTemp);
}

void CHashEdDlg::OnDelPhrase() 
{
	// TODO: Add your control notification handler code here
    HASH_ITEM *pItem;
    int selItem;

    selItem = m_listing.GetSelectionMark();
    if ( selItem==-1 )  return;

    pItem = (HASH_ITEM*) m_listing.GetItemData(selItem);
    m_listing.DeleteItem(selItem);
    m_context.del_phrase_by_id(selItem);//pItem->item_index);

    m_listing.UpdateWindow();
    UpdateBanner();
}

void CHashEdDlg::_save(const char *pathfile, BOOL bSaveNoSwap)
{
    SYSTEMTIME syst;
    CString strHashOld, strTempfile, strHashFile, strTemp;

    strTemp = m_strHashFolder+"\\hash.dat";
    GetLocalTime(&syst);
    //  locate hash.dat
    strHashFile = pathfile;
    strTempfile = pathfile;
    strTempfile += ".new";
    strHashOld.Format("%s.%04d%02d%02d.%02d%02d%02d.bak",
        pathfile, syst.wYear, syst.wMonth, syst.wDay,
        syst.wHour, syst.wMinute, syst.wSecond);
    DeleteFile(strTempfile);

    //  prepare Hash_new.dat file
    m_context.sort_phrase();
    m_context.arrange_phrase();

    if ( bSaveNoSwap==TRUE &&
         strTemp.CompareNoCase(strHashFile)!=0 )
    {
        m_context.save_hash((LPSTR)(LPCTSTR)strHashFile);
        return;
    }

    m_context.save_hash((LPSTR)(LPCTSTR)strTempfile);
    
    //  shutdown server
    m_context.shutdown_server();
    
    //  swap new & current file.
    MoveFile((LPCTSTR)strHashFile, (LPCTSTR)strHashOld);
    MoveFile((LPCTSTR)strTempfile, (LPCTSTR)strHashFile);
    
    //  launch server
    m_context._connect_server();
}

void CHashEdDlg::UpdateBanner(const char *message) 
{
    if ( message==NULL )
    {
        char banner[64];

        sprintf(banner, 
            "編輯區內共有 %d 個詞彙。", 
            m_context.get_phrase_count());

        m_banner.SetWindowText(banner);
    }
    else
        m_banner.SetWindowText(message);
}

void CHashEdDlg::OnSaveAs() 
{
	// TODO: Add your control notification handler code here
    CString strTemp;
    CFileDialog FileDlg(FALSE, NULL/*"dat\0\0"*/, "hash",
                        OFN_ENABLESIZING|OFN_HIDEREADONLY|
                        OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST,
                        "All files(*.*)|*.*||");
    if ( FileDlg.DoModal()==IDCANCEL ) 
    {
        return;
    }

    _enable_buttons(FALSE);
    m_btnSaveAs.GetWindowText(strTemp);
    m_btnSaveAs.SetWindowText("Saving...");

    //
    _save((LPCTSTR)FileDlg.GetPathName(), TRUE);
    
    //  ui...
    _enable_buttons(TRUE);
    m_btnSaveAs.SetWindowText(strTemp);
    
}

void CHashEdDlg::_enable_buttons(BOOL bEnable)
{
	m_btnSaveAs.EnableWindow(bEnable);
	m_btnFindPhrase.EnableWindow(bEnable);
	m_btnAddPhrase.EnableWindow(bEnable);
	m_btnDelPhrase.EnableWindow(bEnable);
	m_Import.EnableWindow(bEnable);
	m_btnSave.EnableWindow(bEnable);
    m_btnAbout.EnableWindow(bEnable);
}

void CHashEdDlg::OnAbout() 
{
	// TODO: Add your control notification handler code here
    CAboutDlg dlgAbout;

    dlgAbout.DoModal();
}

void CHashEdDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
    if ( nIDEvent==553 )
    {
        CString strFile;

        GetHashLocation();
    
        strFile = m_strHashFolder;
        strFile += "\\hash.dat";

        m_context._connect_server();
        Reload((LPSTR)(LPCTSTR)strFile, true);

        KillTimer(553);
        _enable_buttons(TRUE);
    }
	
	CDialog::OnTimer(nIDEvent);
}
