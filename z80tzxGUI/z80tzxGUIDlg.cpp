// z80tzxGUIDlg.cpp : implementation file
//

#include "stdafx.h"
#include "z80tzxGUI.h"
#include "z80tzxGUIDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int speed;
CString StrFile;  

/////////////////////////////////////////////////////////////////////////////
// CZ80tzxGUIDlg dialog

CZ80tzxGUIDlg::CZ80tzxGUIDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CZ80tzxGUIDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CZ80tzxGUIDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CZ80tzxGUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CZ80tzxGUIDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CZ80tzxGUIDlg, CDialog)
	//{{AFX_MSG_MAP(CZ80tzxGUIDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_BN_CLICKED(IDC_RADIO3, OnRadio3)
	ON_BN_CLICKED(IDC_RADIO4, OnRadio4)
	ON_BN_CLICKED(IDC_RADIO5, OnRadio5)
	ON_BN_CLICKED(IDC_RADIO6, OnRadio6)
	ON_BN_CLICKED(IDC_RADIO7, OnRadio7)
	ON_BN_CLICKED(IDC_RADIO8, OnRadio8)
	ON_BN_CLICKED(IDC_RADIO9, OnRadio9)
	ON_BN_CLICKED(IDC_RADIO10, OnRadio10)
	ON_BN_CLICKED(IDC_RADIO11, OnRadio11)
	ON_BN_CLICKED(IDC_RADIO12, OnRadio12)
	ON_BN_CLICKED(IDIGNORE, OnIgnore)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CZ80tzxGUIDlg message handlers

BOOL CZ80tzxGUIDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	
	// TODO: Add extra initialization here
	CheckRadioButton(IDC_RADIO2, IDC_RADIO12, IDC_RADIO9); 
	speed=7;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CZ80tzxGUIDlg::OnPaint() 
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
HCURSOR CZ80tzxGUIDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CZ80tzxGUIDlg::OnRadio2() 
{
	speed = 0;	/* 1364 */	
}

void CZ80tzxGUIDlg::OnRadio3() 
{
	speed = 1;	/* 2250 */	
}

void CZ80tzxGUIDlg::OnRadio4() 
{
	speed = 2;	/* 3000 */	
}

void CZ80tzxGUIDlg::OnRadio5() 
{
	speed = 3;	/* 3230 */	
}

void CZ80tzxGUIDlg::OnRadio6() 
{
	speed = 4;	/* 3500 */	
}

void CZ80tzxGUIDlg::OnRadio7() 
{
	speed = 6;	/* 4500 */	
}

void CZ80tzxGUIDlg::OnRadio8() 
{
	speed = 6;	/* 5000 */	
}

void CZ80tzxGUIDlg::OnRadio9() 
{
	speed = 7;	/* 5100 */	
}

void CZ80tzxGUIDlg::OnRadio10() 
{
	speed = 8;	/* 5500 */	
}

void CZ80tzxGUIDlg::OnRadio11() 
{
	speed = 9;	/* 5800 */
	
}

void CZ80tzxGUIDlg::OnRadio12() 
{
	speed = 10;	/* 6000 */	
}

void CZ80tzxGUIDlg::OnCancel()
{
	CDialog::OnCancel();
}

void CZ80tzxGUIDlg::OnIgnore()
{
// Create an instance First
//CFileDialog fOpenDlg(TRUE, "txt", "vicon_cams_data", OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, "Camera Data Files (*.txt)|*.txt|*.dat||", this);


	//CString StrFile;      
	CString defExe("");
	CString defFileName("");
	//CString defFilter("(*.z80)|*.z80|(*.sna)|*.sna|");
	CString defFilter("zx Snapshot files(*.sna;*.z80)|*.sna;*.z80|All files(*.*)|*.*|");
	CFileDialog dlg(true,defExe,defFileName,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,defFilter);
	dlg.m_ofn.lpstrInitialDir= _T(".\\"); 
	if(dlg.DoModal()==IDOK)
	{
	   StrFile = dlg.GetPathName();
	   // do something 
	}


}

void CZ80tzxGUIDlg::OnOK() 
{
	char cmdargs[256];
	char speedstr[4];
	HINSTANCE rc;

	itoa(speed,speedstr,10); 

	strcpy (cmdargs, StrFile);	
	strcat (cmdargs, " -s "); 
	strcat (cmdargs, speedstr);

	//rc = ShellExecute (NULL, "open", "http://123.456.789.001", NULL, NULL, SW_SHOWMAXIMIZED);
	rc = ShellExecute (NULL, "open", "z802tzx3.exe", cmdargs, NULL, SW_SHOWMAXIMIZED);		
	//CDialog::OnOK();
}


