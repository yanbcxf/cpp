// PlateDebtToCapitalRatioView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "IndividualShareDoc.h"
#include "CalculateStockInfoEx.h"
#include "PlateDebtToCapitalRatioView.h"


// CPlateDebtToCapitalRatioView

IMPLEMENT_DYNCREATE(CPlateDebtToCapitalRatioView, CBaseChartCtlView)

CPlateDebtToCapitalRatioView::CPlateDebtToCapitalRatioView()
{

}

CPlateDebtToCapitalRatioView::~CPlateDebtToCapitalRatioView()
{
}

BEGIN_MESSAGE_MAP(CPlateDebtToCapitalRatioView, CBaseChartCtlView)
END_MESSAGE_MAP()


// CPlateDebtToCapitalRatioView 绘图

void CPlateDebtToCapitalRatioView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CPlateDebtToCapitalRatioView 诊断

#ifdef _DEBUG
void CPlateDebtToCapitalRatioView::AssertValid() const
{
	CBaseChartCtlView::AssertValid();
}

#ifndef _WIN32_WCE
void CPlateDebtToCapitalRatioView::Dump(CDumpContext& dc) const
{
	CBaseChartCtlView::Dump(dc);
}
#endif
#endif //_DEBUG


// CPlateDebtToCapitalRatioView 消息处理程序


void CPlateDebtToCapitalRatioView::OnInitialUpdate()
{
	CBaseMessageView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	RedrawDmGraph();
}

void CPlateDebtToCapitalRatioView::RedrawDmGraph()
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CCalculateStockInfoEx * cse =  new CCalculateStockInfoEx(m_hWnd, WM_USER_LOG_1);

	m_MessageCtrl.SetStockRangeForCalculateTask(pDoc, cse);

	cse->ScheduleTask(&CCalculateStockInfoEx::RecentBalanceAndIncomestatement1);
}

void CPlateDebtToCapitalRatioView::ProcessXmlWParam(string msgType,string code,string model,\
												   string param1,string param2,string param3)
{
	if(msgType=="TaskFinished")
	{
		stringstream ss;
		ss << "任务【" << (param1) << "】 计算完成. ";

		CCalculateTask * cse = CCalculateTask::FetchTask(atol(param1.c_str()));
		RedrawDmGraph(cse);
		if(cse!=NULL)
			delete cse;
		sendToOutput(ss.str().c_str(), NULL, WM_USER_LOG_1);
	}
}


void CPlateDebtToCapitalRatioView::RedrawDmGraph(CCalculateTask * cse)
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CStockApp * pApp = (CStockApp *)AfxGetApp();

	ASSERT_VALID(pDoc);	

	m_pChartCtrl->EnableRefresh(false);

	CRect rect = m_pChartCtrl->GetPlottingRect();

	CCalculateStockInfoEx * pCse =  (CCalculateStockInfoEx *)cse;
	m_pChartCtrl->RemoveAllSeries();

	CChartDateTimeAxis* pBottomAxis = m_pChartCtrl->CreateDateTimeAxis(CChartCtrl::BottomAxis);
	pBottomAxis->SetTickLabelFormat(false, "%Y%m%d");
	CChartStandardAxis* pLeftAxis = m_pChartCtrl->CreateStandardAxis(CChartCtrl::LeftAxis);

	double xmin, xmax, ymin, ymax;
	xmax = 0; xmin = 99999.0;
	ymin = 99999.0; ymax = 0.0;

	for(int i =0; i<pCse->m_vec_request_code.size(); i++)
	{
		//	逐个股票数据进行过滤
		map<string, double>	vec_ratio;
		for(int m=0; m< pCse->m_vec_code.size(); m++)
		{
			if(pCse->m_vec_code[m]==atoi(pCse->m_vec_request_code[i].c_str()))
			{
				double total_liabilities = pCse->m_vec_total_current_liabilities[m] + pCse->m_vec_total_non_current_liabilities[m];
				double total_assets = pCse->m_vec_total_current_assets[m] + pCse->m_vec_total_non_current_assets[m];
				string report_date = pCse->m_vec_report_date[m];

				COleDateTime odt_in ;
				odt_in.ParseDateTime(report_date.c_str());
				int mon = odt_in.GetMonth();


				//	计算“资本负债率”, 资本 = 所有者权益
				vec_ratio[report_date] = (total_liabilities/(total_assets - total_liabilities)) * 100.0;
			}
		}

		//	绘制 “资本负债率”曲线
		CChartLineSerie* pSeries = NULL;
		if(vec_ratio.size()>0)
		{
			pSeries = m_pChartCtrl->CreateLineSerie();
			char tmp[32];
			sprintf_s(tmp, 32, "%s", pCse->m_vec_request_code[i].c_str());
			pSeries->SetName(tmp);
			pSeries->SetWidth(1);

			pSeries->EnableMouseNotifications(true,false);
			pSeries->RegisterMouseListener(new CBaseChartCtlViewMouseListener(this));
		}
		map<string, double>::iterator it = vec_ratio.begin();
		for(; it!=vec_ratio.end(); it++)
		{
			string report_date = it->first;
			COleDateTime odt ;
			odt.ParseDateTime(report_date.c_str());
			if(m_StartDate> 0 && m_EndDate> m_StartDate)
			{
				if(odt.m_dt < m_StartDate || odt.m_dt > m_EndDate)
					continue;
			}

			double ratio = it->second;
			if(odt.m_dt> xmax) xmax = odt.m_dt;
			if(odt.m_dt< xmin) xmin = odt.m_dt;

			if(ratio > ymax) ymax = ratio;
			if(ratio < ymin) ymin = ratio;

			pSeries->AddPoint(odt.m_dt, ratio);
		}
	}
	pBottomAxis->SetMinMax(xmin, xmax);
	pLeftAxis->SetMinMax(0, ymax + 10);

	m_pChartCtrl->GetLegend()->SetVisible(true);
	m_pChartCtrl->RefreshCtrl();

	m_pChartCtrl->EnableRefresh(true);
}
