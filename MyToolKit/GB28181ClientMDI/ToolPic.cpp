// ToolPic.cpp: implementation of the CToolPic class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ToolPic.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CToolPic::CToolPic()
{

}

CToolPic::~CToolPic()
{

}
BEGIN_MESSAGE_MAP(CToolPic, CStatic) 
     //添加OnMouseMove事件触发 
     ON_WM_MOUSEMOVE() 
END_MESSAGE_MAP() 

void CToolPic::PreSubclassWindow()     
{   
	if(m_ToolTip.GetSafeHwnd())  
	{
		return;
	}
	EnableToolTips(TRUE); 
	m_ToolTip.Create(this);   
	m_ToolTip.Activate(TRUE);
	m_ToolTip.AddTool(this, LPSTR_TEXTCALLBACK);
	m_ToolTip.SetDelayTime(100);
	CStatic::PreSubclassWindow();   
}   

void CToolPic::SetToolTipText(LPCTSTR   lpszToolTipText)   
{   
	m_ToolTip.UpdateTipText(lpszToolTipText, this);   
	strTIP = lpszToolTipText;
	m_ToolTip.Activate(TRUE);
}   

BOOL CToolPic::PreTranslateMessage(MSG* pMsg) 
{
	if ( pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST )
	{
		if (m_ToolTip.GetSafeHwnd() != NULL)
		{
			m_ToolTip.RelayEvent(pMsg);
		}		
	}

	return CStatic::PreTranslateMessage(pMsg);
}

void CToolPic::OnMouseMove(UINT nFlags, CPoint point) 
{ 
	//m_ToolTip.UpdateTipText((LPCTSTR)strTIP, this);
	::PostMessage(GetParent()->GetSafeHwnd(),UPDATE_TOOL_TIP,(WPARAM)(LPCTSTR)strTIP,0);
	CStatic::OnMouseMove(nFlags, point); 
} 

