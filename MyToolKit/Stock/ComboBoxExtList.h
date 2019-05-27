#if !defined(AFX_COMBOBOXEXTLIST_H__2AC610D5_4FCB_490A_9D15_1B47894B943D__INCLUDED_)
#define AFX_COMBOBOXEXTLIST_H__2AC610D5_4FCB_490A_9D15_1B47894B943D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ComboBoxExtList.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CComboBoxExtList window

class CComboBoxExt;

class CComboBoxExtList : public CListBox
{
// Construction
public:
	CComboBoxExtList();

// Attributes
public:
	BOOL m_bShowTooltip;
	BOOL m_bTooltipOnInfo;
	BOOL m_bShowListTooltipOverItem;	// this has meaning only when m_bTooltipOnInfo == TRUE
	CComboBoxExt* m_pComboBox;

// Operations
public:
	TOOLINFO* GetToolInfo(){return &m_ToolInfo;}
	HWND* GetToolTipHwnd(){return &m_hWndToolTip;}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComboBoxExtList)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CComboBoxExtList();

protected:
	int m_nPrevItem;
	BOOL m_bToolActive;
	HWND m_hWndToolTip;
	TOOLINFO m_ToolInfo;

	// Generated message map functions
protected:
	//{{AFX_MSG(CComboBoxExtList)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(/*UINT*/UINT_PTR nIDEvent);
	//}}AFX_MSG
	afx_msg LRESULT OnLbFindString(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMBOBOXEXTLIST_H__2AC610D5_4FCB_490A_9D15_1B47894B943D__INCLUDED_)
