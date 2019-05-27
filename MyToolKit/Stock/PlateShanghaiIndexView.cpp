// ShanghaiIndexView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"

#include "PlateShanghaiIndexView.h"

#include "MatlabShanghaiSohuPlateIndex.h"

#include "StockRangeSet.h"

//#include "StCloseWindow.h"
#include "StockMatlab.h"

// CShanghaiIndexView

IMPLEMENT_DYNCREATE(CPlateShanghaiIndexView, CBaseMatlabView)

CPlateShanghaiIndexView::CPlateShanghaiIndexView()
{

}

CPlateShanghaiIndexView::~CPlateShanghaiIndexView()
{
}

BEGIN_MESSAGE_MAP(CPlateShanghaiIndexView, CBaseMatlabView)
	ON_COMMAND(ID_SET_STOCK_RANGE, &CPlateShanghaiIndexView::OnSetStockRange)
	ON_UPDATE_COMMAND_UI(ID_SHANGHAI_INDEX, &CPlateShanghaiIndexView::OnUpdateShanghaiIndex)
END_MESSAGE_MAP()


// CShanghaiIndexView 绘图

void CPlateShanghaiIndexView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CShanghaiIndexView 诊断

#ifdef _DEBUG
void CPlateShanghaiIndexView::AssertValid() const
{
	CBaseMatlabView::AssertValid();
}

#ifndef _WIN32_WCE
void CPlateShanghaiIndexView::Dump(CDumpContext& dc) const
{
	CBaseMatlabView::Dump(dc);
}
#endif
#endif //_DEBUG


// CShanghaiIndexView 消息处理程序


void CPlateShanghaiIndexView::Startup()
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();

	CMatlabShanghaiSohuPlateIndex * cse =  new CMatlabShanghaiSohuPlateIndex(m_hWnd, WM_USER_LOG_1);
	cse->m_windowInMatlab = m_windowInMatlab;
	cse->m_vec_request_code = pDoc->m_vec_request_code;
	cse->m_strGraphOrTable = pDoc->m_strInput;
	cse->m_windowInMatlab = m_windowInMatlab;

	//	传入本次 板块/个股 检索标志
	cse->m_vec_request_is_stock.push_back(pDoc->m_nIs_Stock);	
	cse->ScheduleTask(&CMatlabShanghaiSohuPlateIndex::Execute1);
	m_bCalculating = true;
}

void CPlateShanghaiIndexView::OnSetStockRange()
{
	// TODO: 在此添加命令处理程序代码

	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();

	/*mwArray mwOut(1, 1, mxINT32_CLASS);*/

	CStockRangeSet aboutDlg;
	aboutDlg.m_StartDate = pDoc->m_StartDate;
	aboutDlg.m_End_Date = pDoc->m_EndDate;
	
	if(aboutDlg.DoModal()==IDOK)
	{
		CMatlabShanghaiSohuPlateIndex * cse =  new CMatlabShanghaiSohuPlateIndex(m_hWnd, WM_USER_LOG_1);
		//	传入先前打开的窗口，以便进行关闭
		
		pDoc->m_StartDate = aboutDlg.m_StartDate;
		pDoc->m_EndDate = aboutDlg.m_End_Date;

		cse->m_windowInMatlab = m_windowInMatlab;

		cse->m_StartDate = aboutDlg.m_StartDate;
		cse->m_EndDate = aboutDlg.m_End_Date;
		cse->m_vec_request_code = pDoc->m_vec_request_code;
		cse->m_strGraphOrTable = pDoc->m_strInput;

		//	传入本次 板块/个股 检索标志
		cse->m_vec_request_is_stock.push_back(pDoc->m_nIs_Stock);	
		cse->ScheduleTask(&CMatlabShanghaiSohuPlateIndex::Execute1);

		m_bCalculating = true;
	}
}

void CPlateShanghaiIndexView::OnUpdateShanghaiIndex(CCmdUI *pCmdUI)
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
