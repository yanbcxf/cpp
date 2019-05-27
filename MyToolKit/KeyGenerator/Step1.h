#if !defined(AFX_STEP1_H__9C36D055_7562_4BF3_A437_1B504411370F__INCLUDED_)
#define AFX_STEP1_H__9C36D055_7562_4BF3_A437_1B504411370F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Step1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStep1 dialog

class CStep1 : public CDialog
{
// Construction
public:
	CStep1(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CStep1)
	enum { IDD = IDD_DLG_STEP1 };
	CString	m_strSlc;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStep1)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CStep1)
	afx_msg void OnNext();
	virtual BOOL OnInitDialog();
	afx_msg void OnPrev1();
	afx_msg void OnClose();
	afx_msg void OnButBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STEP1_H__9C36D055_7562_4BF3_A437_1B504411370F__INCLUDED_)
