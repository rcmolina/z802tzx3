// z80tzxGUIDlg.h : header file
//

#if !defined(AFX_Z80TZXGUIDLG_H__7A791C0D_D3D8_48FD_82E7_AEF37D9B13F5__INCLUDED_)
#define AFX_Z80TZXGUIDLG_H__7A791C0D_D3D8_48FD_82E7_AEF37D9B13F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CZ80tzxGUIDlg dialog

class CZ80tzxGUIDlg : public CDialog
{
// Construction
public:
	CZ80tzxGUIDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CZ80tzxGUIDlg)
	enum { IDD = IDD_Z80TZXGUI_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CZ80tzxGUIDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CZ80tzxGUIDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnRadio2();
	afx_msg void OnRadio3();
	afx_msg void OnRadio4();
	afx_msg void OnRadio5();
	afx_msg void OnRadio6();
	afx_msg void OnRadio7();
	afx_msg void OnRadio8();
	afx_msg void OnRadio9();
	afx_msg void OnRadio10();
	afx_msg void OnRadio11();
	afx_msg void OnRadio12();
	afx_msg void OnRadio13();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnIgnore();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_Z80TZXGUIDLG_H__7A791C0D_D3D8_48FD_82E7_AEF37D9B13F5__INCLUDED_)
