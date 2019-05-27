#if !defined(AFX_STEP2_H__AE33401C_CCAC_4699_B5AB_AD779B92D88A__INCLUDED_)
#define AFX_STEP2_H__AE33401C_CCAC_4699_B5AB_AD779B92D88A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Step2.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStep2 dialog

class CStep2 : public CDialog
{
// Construction
public:
	CStep2(CWnd* pParent = NULL);   // standard constructor

	CList<CString,CString&> listMac;	//macµÿ÷∑¡–±Ì

// Dialog Data
	//{{AFX_DATA(CStep2)
	enum { IDD = IDD_DLG_STEP2 };
	CString	m_strVersion;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStep2)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CStep2)
	afx_msg void OnNext();
	afx_msg void OnPrev();
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnButBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STEP2_H__AE33401C_CCAC_4699_B5AB_AD779B92D88A__INCLUDED_)
