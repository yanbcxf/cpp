// ToolPic.h: interface for the CToolPic class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOOLPIC_H__E0E96E26_4A71_4651_BE1C_5719E258A0EF__INCLUDED_)
#define AFX_TOOLPIC_H__E0E96E26_4A71_4651_BE1C_5719E258A0EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define  UPDATE_TOOL_TIP WM_USER+100
class CToolPic : public CStatic  
{
public:

	CToolPic();
	virtual ~CToolPic();
	virtual void PreSubclassWindow(); 

	CToolTipCtrl   m_ToolTip; 
	CString strTIP;
	void   SetToolTipText(LPCTSTR   lpszToolTipText);    
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnMouseMove(UINT nFlags, CPoint point); 

};

#endif // !defined(AFX_TOOLPIC_H__E0E96E26_4A71_4651_BE1C_5719E258A0EF__INCLUDED_)
