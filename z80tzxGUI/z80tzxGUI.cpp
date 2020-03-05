// z80tzxGUI.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "z80tzxGUI.h"
#include "z80tzxGUIDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CZ80tzxGUIApp

BEGIN_MESSAGE_MAP(CZ80tzxGUIApp, CWinApp)
	//{{AFX_MSG_MAP(CZ80tzxGUIApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CZ80tzxGUIApp construction

CZ80tzxGUIApp::CZ80tzxGUIApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CZ80tzxGUIApp object

CZ80tzxGUIApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CZ80tzxGUIApp initialization

BOOL CZ80tzxGUIApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	CZ80tzxGUIDlg dlg;
	m_pMainWnd = &dlg;
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

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
