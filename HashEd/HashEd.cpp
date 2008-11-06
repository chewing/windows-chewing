// HashEd.cpp : Defines the class behaviors for the application.
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
// CHashEdApp

/////////////////////////////////////////////////////////////////////////////
// CHashEdApp initialization

int WINAPI WinMain( HINSTANCE hinst, HINSTANCE hprev, LPSTR lpcmd, int nShow )
{
	InitCommonControls();

	CHashEdDlg dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	return 0;
};
