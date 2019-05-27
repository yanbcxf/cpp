// Indi10jqkaBonusView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "Indi10jqkaBonusView.h"
#include "Calc10jqkaBonusViewData.h"

// CIndi10jqkaBonusView

IMPLEMENT_DYNCREATE(CIndi10jqkaBonusView, CBaseGridCtlView)

CIndi10jqkaBonusView::CIndi10jqkaBonusView()
{

}

CIndi10jqkaBonusView::~CIndi10jqkaBonusView()
{
}

BEGIN_MESSAGE_MAP(CIndi10jqkaBonusView, CBaseGridCtlView)
END_MESSAGE_MAP()


// CIndi10jqkaBonusView 绘图

void CIndi10jqkaBonusView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CIndi10jqkaBonusView 诊断

#ifdef _DEBUG
void CIndi10jqkaBonusView::AssertValid() const
{
	CBaseGridCtlView::AssertValid();
}

#ifndef _WIN32_WCE
void CIndi10jqkaBonusView::Dump(CDumpContext& dc) const
{
	CBaseGridCtlView::Dump(dc);
}
#endif
#endif //_DEBUG


// CIndi10jqkaBonusView 消息处理程序


void CIndi10jqkaBonusView::OnInitialUpdate()
{
	CBaseGridCtlView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CCalc10jqkaBonusViewData * cse =  new CCalc10jqkaBonusViewData(m_hWnd, WM_USER_LOG_1);

	cse->m_nCode = atoi(pDoc->m_nCode.c_str());	
	cse->ScheduleTask(&CCalc10jqkaBonusViewData::Execute1);

}


void CIndi10jqkaBonusView::ProcessXmlWParam(string msgType, string code,string model, \
													string param1,string param2,string param3)
{
	if(msgType=="TaskFinished")
	{
		stringstream ss;
		ss << "任务【" << (param1) << "】 计算完成. ";

		CCalculateTask * cse = CCalculateTask::FetchTask(atol(param1.c_str()));
		RedrawGridCtrl(cse);
		if(cse!=NULL)
			delete cse;
		sendToOutput(ss.str().c_str(), NULL, WM_USER_LOG_1);
	}
}


void CIndi10jqkaBonusView::RedrawGridCtrl(CCalculateTask * cse)
{

	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CCalc10jqkaBonusViewData * pData = (CCalc10jqkaBonusViewData *)cse;

	if(m_pGridCtrl)
	{
		CStockApp * pApp = (CStockApp *)AfxGetApp();

		if(pData->m_vec_right_record_date.size()>0)
		{
			m_pGridCtrl->SetRowCount(pData->m_vec_right_record_date.size() + 1);
			m_pGridCtrl->SetColumnCount(5);
			m_pGridCtrl->SetFixedRowCount(1);
			m_pGridCtrl->SetFixedColumnCount(0);

			SetGridCell(0,0, true, "股权登记日");
			SetGridCell(0,1, true, "除权除息日");
			SetGridCell(0,2, true, "现金分红（元/每10股）");
			SetGridCell(0,3, true, "送股（股/每10股）");
			SetGridCell(0,4, true, "转增（股/每10股）");

			for(int k=0;  k <pData->m_vec_right_record_date.size(); k++)
			{
				SetGridCell(k + 1, 0, false, pData->m_vec_right_record_date[k]);
				SetGridCell(k + 1, 1, false, pData->m_vec_exit_right_date[k]);
				if(pData->m_vec_bonus_cash[k]>0)
					SetGridCell(k + 1, 2, false, Double2String(pData->m_vec_bonus_cash[k]), RGB(255, 32,32 ));
				else
					SetGridCell(k + 1, 2, false, Double2String(pData->m_vec_bonus_cash[k]));

				if(pData->m_vec_bonus_share[k]>0)
					SetGridCell(k + 1, 3, false, Double2String(pData->m_vec_bonus_share[k]), RGB(255, 32,32 ));
				else
					SetGridCell(k + 1, 3, false, Double2String(pData->m_vec_bonus_share[k]));

				if(pData->m_vec_reserve_to_common_share[k]>0)
					SetGridCell(k + 1, 4, false, Double2String(pData->m_vec_reserve_to_common_share[k]), RGB(255, 32,32 ));
				else
					SetGridCell(k + 1, 4, false, Double2String(pData->m_vec_reserve_to_common_share[k]));
					
			}

		}

		m_pGridCtrl->AutoSize();
	}

}