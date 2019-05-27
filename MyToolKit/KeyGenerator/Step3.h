#if !defined(AFX_STEP3_H__3D756321_8EC9_42EA_B069_B85143D7EAC3__INCLUDED_)
#define AFX_STEP3_H__3D756321_8EC9_42EA_B069_B85143D7EAC3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Step3.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStep3 dialog

class CStep3 : public CDialog
{
// Construction
public:
	CStep3(CWnd* pParent = NULL);   // standard constructor

	CList<CString,CString&> listDVID;	//macµÿ÷∑¡–±Ì

// Dialog Data
	//{{AFX_DATA(CStep3)
	enum { IDD = IDD_DLG_STEP3 };
	CString	m_strDid;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStep3)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CStep3)
	afx_msg void OnPrev();
	afx_msg void OnEnyg();
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnButBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STEP3_H__3D756321_8EC9_42EA_B069_B85143D7EAC3__INCLUDED_)
