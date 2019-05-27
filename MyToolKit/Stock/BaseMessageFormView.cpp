// BaseMessageFormView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "BaseMessageFormView.h"

#include "BaseChartCtlView.h"

#include "FindStockByPinyin.h"

// CBaseMessageFormView

IMPLEMENT_DYNCREATE(CBaseMessageFormView, CFormView)

CBaseMessageFormView::CBaseMessageFormView()
	: CFormView(CBaseMessageFormView::IDD)
{
	m_MessageCtrl.SetOwnerView(this);
}

CBaseMessageFormView::~CBaseMessageFormView()
{
}

void CBaseMessageFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_BUTTON1, m_btn1);
	DDX_Control(pDX, IDC_GRID, m_Grid);
	DDX_Control(pDX, IDC_CHART, m_ChartCtrl);
}

BEGIN_MESSAGE_MAP(CBaseMessageFormView, CFormView)
	ON_WM_SIZE()
	ON_MESSAGE(WM_USER_XML, OnUserXmlWParam)
	ON_MESSAGE(WM_USER_LOG_1, OnUserLogWParam_1)
	ON_MESSAGE(WM_USER_LOG_2, OnUserLogWParam_1)
	ON_MESSAGE(WM_USER_LOG_3, OnUserLogWParam_1)
	ON_WM_ACTIVATE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()


// CBaseMessageFormView 诊断

#ifdef _DEBUG
void CBaseMessageFormView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CBaseMessageFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CBaseMessageFormView 消息处理程序

void CBaseMessageFormView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类

	ReLayout();
}

void CBaseMessageFormView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	ReLayout();
}

void CBaseMessageFormView::ReLayout()
{
	if (m_Grid.GetSafeHwnd())
	{
		CRect rect;
		GetClientRect(&rect);
		rect.bottom = rect.top + (rect.bottom - rect.top) * 2 / 3 - 2;
		m_Grid.MoveWindow(rect);

		GetClientRect(&rect);
		rect.top = rect.bottom - (rect.bottom - rect.top) * 1 / 3 + 2;
		m_ChartCtrl.MoveWindow(rect);
	}
}



LRESULT CBaseMessageFormView::OnUserXmlWParam(WPARAM wParam, LPARAM lParam)
{
	return m_MessageCtrl.OnUserXmlWParam(wParam,lParam);
}


LRESULT CBaseMessageFormView::OnUserLogWParam_1(WPARAM wParam, LPARAM lParam)
{
	return m_MessageCtrl.OnUserLogWParam_1(wParam, lParam);
}


void CBaseMessageFormView::OnSetFocus(CWnd* pOldWnd)
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

void CBaseMessageFormView::OnKillFocus(CWnd* pNewWnd)
{
	CView::OnKillFocus(pNewWnd);

	// TODO: 在此处添加消息处理程序代码


	PostFocusChanged();
}




void CBaseMessageFormView::ProcessXmlWParam(string msgType,string code,string model,\
										 string param1,string param2,string param3)
{
	if(msgType=="TaskFinished")
	{
		stringstream ss;
		ss << "任务【" << (param1) << "】 计算完成. ";

		CCalculateTask * cse = CCalculateTask::FetchTask(atol(param1.c_str()));
		RedrawChart(cse);
		if(cse!=NULL)
			delete cse;
		sendToOutput(ss.str().c_str(), NULL, WM_USER_LOG_1);
	}
	else if(msgType=="ReadFinished")
		RedrawDmGraph();
}


BOOL CBaseMessageFormView::PreTranslateMessage(MSG* pMsg)
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

	return CFormView::PreTranslateMessage(pMsg);
}
