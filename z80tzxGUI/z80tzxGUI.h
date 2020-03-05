// z80tzxGUI.h : main header file for the Z80TZXGUI application
//

#if !defined(AFX_Z80TZXGUI_H__828F809B_C00B_4035_85AD_5C4A8C0F819E__INCLUDED_)
#define AFX_Z80TZXGUI_H__828F809B_C00B_4035_85AD_5C4A8C0F819E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CZ80tzxGUIApp:
// See z80tzxGUI.cpp for the implementation of this class
//

class CZ80tzxGUIApp : public CWinApp
{
public:
	CZ80tzxGUIApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CZ80tzxGUIApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CZ80tzxGUIApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_Z80TZXGUI_H__828F809B_C00B_4035_85AD_5C4A8C0F819E__INCLUDED_)
