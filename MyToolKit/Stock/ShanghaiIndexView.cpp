// ShanghaiIndexView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
//#include "StShanghaiIndex.h"
#include "StockMatlab.h"
#include "ShanghaiIndexView.h"

#include "MatlabShanghaiIndex.h"

#include "StockRangeSet.h"


// CShanghaiIndexView

IMPLEMENT_DYNCREATE(CShanghaiIndexView, CBaseMatlabView)

CShanghaiIndexView::CShanghaiIndexView()
{

}

CShanghaiIndexView::~CShanghaiIndexView()
{
}

BEGIN_MESSAGE_MAP(CShanghaiIndexView, CBaseMatlabView)
	ON_COMMAND(ID_SET_STOCK_RANGE, &CShanghaiIndexView::OnSetStockRange)
	ON_UPDATE_COMMAND_UI(ID_SHANGHAI_INDEX, &CShanghaiIndexView::OnUpdateShanghaiIndex)
END_MESSAGE_MAP()


// CShanghaiIndexView 绘图

void CShanghaiIndexView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CShanghaiIndexView 诊断

#ifdef _DEBUG
void CShanghaiIndexView::AssertValid() const
{
	CBaseMatlabView::AssertValid();
}

#ifndef _WIN32_WCE
void CShanghaiIndexView::Dump(CDumpContext& dc) const
{
	CBaseMatlabView::Dump(dc);
}
#endif
#endif //_DEBUG


// CShanghaiIndexView 消息处理程序


void CShanghaiIndexView::Startup()
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();

	CMatlabShanghaiIndex * cse =  new CMatlabShanghaiIndex(m_hWnd, WM_USER_LOG_1);
	cse->m_windowInMatlab = m_windowInMatlab;
	cse->m_strIndexCode = pDoc->m_nCode;

	cse->ScheduleTask(&CMatlabShanghaiIndex::Execute1);
	m_bCalculating = true;
}

void CShanghaiIndexView::OnSetStockRange()
{
	// TODO: 在此添加命令处理程序代码

	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CStockRangeSet aboutDlg;
	
	if(aboutDlg.DoModal()==IDOK)
	{
		CMatlabShanghaiIndex * cse =  new CMatlabShanghaiIndex(m_hWnd, WM_USER_LOG_1);
		//	传入先前打开的窗口，以便进行关闭
		cse->m_windowInMatlab = m_windowInMatlab;
		cse->m_strIndexCode = pDoc->m_nCode;

		cse->m_StartDate = aboutDlg.m_StartDate;
		cse->m_EndDate = aboutDlg.m_End_Date;
		cse->ScheduleTask(&CMatlabShanghaiIndex::Execute1);

		m_bCalculating = true;
	}
}

void CShanghaiIndexView::OnUpdateShanghaiIndex(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	if(m_bCalculating==FALSE)
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
