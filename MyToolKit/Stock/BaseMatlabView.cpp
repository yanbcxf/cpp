// BaseMatlabView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "BaseMatlabView.h"

#include "CalcCninfoAnnouncePDFQuery.h"
//#include "stocktest.h"
//#include "StOpenWindow.h"
//#include "StCloseWindow.h"

#include "StockMatlab.h"

// CBaseMatlabView

int CBaseMatlabView::g_windowInMatlab = 1;

IMPLEMENT_DYNCREATE(CBaseMatlabView, CBaseMessageView)

CBaseMatlabView::CBaseMatlabView()
{
	//bool res = stocktestInitialize();
	m_plotH = NULL;
	m_windowInMatlab = -1;
	m_bCalculating = false;

}

CBaseMatlabView::~CBaseMatlabView()
{
	if(m_windowInMatlab >=0 )
	{
		try{
			// yangbin matlab
			mwArray mwOut(1, 1, mxINT32_CLASS);

			char wndName[32] = { 0 };
			sprintf_s(wndName, 32, "%d", m_windowInMatlab);
			StCloseWindow(1, mwOut, wndName);
		}
		catch(CException & e)
		{
			stringstream ss;
			;
			string ttt = ss.str();
			int k = 1;
		}
		
	}
	
}

BEGIN_MESSAGE_MAP(CBaseMatlabView, CBaseMessageView)
	ON_WM_SIZE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CBaseMatlabView 绘图

void CBaseMatlabView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CBaseMatlabView 诊断

#ifdef _DEBUG
void CBaseMatlabView::AssertValid() const
{
	CBaseMessageView::AssertValid();
}

#ifndef _WIN32_WCE
void CBaseMatlabView::Dump(CDumpContext& dc) const
{
	CBaseMessageView::Dump(dc);
}
#endif
#endif //_DEBUG


// CBaseMatlabView 消息处理程序

void CBaseMatlabView::OnInitialUpdate()
{
	CBaseMessageView::OnInitialUpdate();

	g_windowInMatlab++;
	m_windowInMatlab = g_windowInMatlab;

	char wndName[32] = { 0 };
	sprintf_s(wndName, 32, "%d", m_windowInMatlab);

	// yangbin matlab
	StOpenWindow(wndName);

	do{
		char wndName[32] = { 0 };
		sprintf_s(wndName, 32, "Stock%d", m_windowInMatlab);

		m_plotH = ::FindWindow(NULL, wndName);
		if(::IsWindow(m_plotH))
		{
			//	将本 View 设置为 Matlab 窗口的父窗口
			::SetParent(m_plotH, this->GetSafeHwnd());
			CWnd * pWnd = NULL;
			pWnd = FromHandle(m_plotH);

			CRect rect;
			GetClientRect(&rect);
			pWnd->MoveWindow(&rect, false);
			pWnd->ModifyStyle(WS_CAPTION|WS_BORDER, WS_MAXIMIZE, 0);
			Invalidate(TRUE);
			break;
		}
		else
			Sleep(100);
	}while(1);
	

	// TODO: 在此添加专用代码和/或调用基类
	Startup();
}



void CBaseMatlabView::ProcessXmlWParam(string msgType,string code,string model,\
										string param1,string param2,string param3)
{
	if(msgType=="TaskFinished")
	{
		stringstream ss;
		ss << "任务【" << (param1) << "】 计算完成. ";

		CCalculateTask * cse = CCalculateTask::FetchTask(atol(param1.c_str()));

		// 记住本视图在 Matlab 中打开的窗口
		if(m_windowInMatlab == cse->m_windowInMatlab)
			ss << "窗口标识 OK";
		else
			ss << "窗口标识 Fail";

		RedrawGridCtrl(cse);
		if(cse!=NULL)
			delete cse;
		sendToOutput(ss.str().c_str(), NULL, WM_USER_LOG_1);
		m_bCalculating = false;
	}
	//RedrawGridCtrl();
}

/*
void CBaseMatlabView::Startup()
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();

	CCalcCninfoAnnouncePDFQuery * cse =  new CCalcCninfoAnnouncePDFQuery(m_hWnd, WM_USER_LOG_1);
	cse->m_request_like = "";
	cse->m_page_no = 0;

	m_MessageCtrl.SetStockRangeForCalculateTask(pDoc, cse);

	cse->ScheduleTask(&CCalcCninfoAnnouncePDFQuery::Execute1);
}
*/


void CBaseMatlabView::RedrawGridCtrl(CCalculateTask * cse)
{

	CCalculateTask * pCse = (CCalculateTask *)cse;
}


void CBaseMatlabView::OnSize(UINT nType, int cx, int cy)
{
	CBaseMessageView::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码

	if(::IsWindow(m_plotH))
	{
		::MoveWindow(m_plotH, 0, 0, cx, cy, true);
	}
}


void CBaseMatlabView::PostFocusChanged()
{
	if(::IsWindow(m_plotH))
	{
		//	将本 View 设置为 Matlab 窗口的父窗口
		CWnd * pWnd = NULL;
		pWnd = FromHandle(m_plotH);

		CRect rect;
		GetClientRect(&rect);
		pWnd->MoveWindow(&rect, false);
	}
}
void CBaseMatlabView::OnDestroy()
{
	CBaseMessageView::OnDestroy();

	// TODO: 在此处添加消息处理程序代码

	if(m_windowInMatlab >=0 && ::IsWindow(m_plotH))
	{
		// yangbin matlab
		try{
			mwArray mwOut(1, 1, mxINT32_CLASS);

			char wndName[32] = { 0 };
			sprintf_s(wndName, 32, "%d", m_windowInMatlab);
			StCloseWindow(1, mwOut, wndName);
		}
		catch(mwException & e)
		{
			stringstream ss;
			ss << e.what();
			string ttt = ss.str();
			int k = 1;
		}
		m_windowInMatlab = -1;
	}
}
