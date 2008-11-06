// HashEd.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "HashEd.h"
#include "HashEdDlg.h"

#include "PhraseList.h"

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

	// Init RichEdit 2.0
	HMODULE riched20 = LoadLibraryA("RICHED20.DLL");

	// Init Phrase List Control
	CPhraseList::init();

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

	FreeLibrary( riched20 );
	return 0;
};
