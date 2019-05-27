// BaseMessageView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "BaseMessageElementsParser.h"
#include "BaseMessageView.h"

#include "FindStockByPinyin.h"

//////////////////////////////////////////////////////////////////////////
// CBaseMessageView

IMPLEMENT_DYNCREATE(CBaseMessageView, CView)

CBaseMessageView::CBaseMessageView()
{
	m_MessageCtrl.SetOwnerView(this);
}

CBaseMessageView::~CBaseMessageView()
{
}

BEGIN_MESSAGE_MAP(CBaseMessageView, CView)
	ON_MESSAGE(WM_USER_XML, OnUserXmlWParam)
	ON_MESSAGE(WM_USER_LOG_1, OnUserLogWParam_1)
	ON_MESSAGE(WM_USER_LOG_2, OnUserLogWParam_1)
	ON_MESSAGE(WM_USER_LOG_3, OnUserLogWParam_1)
	ON_WM_ACTIVATE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_CHAR()
END_MESSAGE_MAP()


// CBaseMessageView 绘图

void CBaseMessageView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CBaseMessageView 诊断

#ifdef _DEBUG
void CBaseMessageView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CBaseMessageView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CBaseMessageView 消息处理程序


LRESULT CBaseMessageView::OnUserXmlWParam(WPARAM wParam, LPARAM lParam)
{
	return m_MessageCtrl.OnUserXmlWParam(wParam,lParam);
}


LRESULT CBaseMessageView::OnUserLogWParam_1(WPARAM wParam, LPARAM lParam)
{
	return m_MessageCtrl.OnUserLogWParam_1(wParam, lParam);
}


void CBaseMessageView::OnSetFocus(CWnd* pOldWnd)
{
	CView::OnSetFocus(pOldWnd);

	// TODO: 在此处添加消息处理程序代码

	/*当获取焦点时，自动在左边的股票各个板块中寻找对应项*/

	CString strTitle;
	GetParent()->GetWindowText(strTitle);
	
	m_MessageCtrl.OnSetFocus(strTitle);

	//////////////////////////////////////////////////////////////////////////
	PostFocusChanged();
	
}

void CBaseMessageView::OnKillFocus(CWnd* pNewWnd)
{
	CView::OnKillFocus(pNewWnd);

	// TODO: 在此处添加消息处理程序代码

	
	PostFocusChanged();
}

BOOL CBaseMessageView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message==WM_CHAR )
	{
		TCHAR ch = pMsg->wParam;

		if(/*(ch >='A' && ch <='Z') ||*/ (ch >='a' && ch <='z'))
		{
			CFindStockByPinyin dlg;
			dlg.m_strPinyin = ch; 
			dlg.DoModal();

			if(dlg.m_strPinyin.GetLength()>6)
			{
				CString strCode = dlg.m_strPinyin.Mid(5).Left(6);
				m_MessageCtrl.FocusStockOnLeftPane(strCode.GetBuffer());
			}

			return TRUE;
		}
		
	}

	return CView::PreTranslateMessage(pMsg);
}
