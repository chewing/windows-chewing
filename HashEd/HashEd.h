// HashEd.h : main header file for the HASHED application
//

#if !defined(AFX_HASHED_H__69C59549_6152_4802_A835_892AD84C996D__INCLUDED_)
#define AFX_HASHED_H__69C59549_6152_4802_A835_892AD84C996D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CHashEdApp:
// See HashEd.cpp for the implementation of this class
//

class CHashEdApp : public CWinApp
{
public:
	CHashEdApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHashEdApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CHashEdApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HASHED_H__69C59549_6152_4802_A835_892AD84C996D__INCLUDED_)
