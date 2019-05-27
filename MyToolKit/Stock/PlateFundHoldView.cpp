// PlateFundHoldView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "IndividualShareDoc.h"
#include "CalculateStockInfoEx.h"
#include "PlateFundHoldView.h"


// CPlateFundHoldView

IMPLEMENT_DYNCREATE(CPlateFundHoldView, CBaseChartCtlView)

CPlateFundHoldView::CPlateFundHoldView()
{

}

CPlateFundHoldView::~CPlateFundHoldView()
{
}

BEGIN_MESSAGE_MAP(CPlateFundHoldView, CBaseChartCtlView)
END_MESSAGE_MAP()


// CPlateFundHoldView 绘图

void CPlateFundHoldView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CPlateFundHoldView 诊断

#ifdef _DEBUG
void CPlateFundHoldView::AssertValid() const
{
	CBaseChartCtlView::AssertValid();
}

#ifndef _WIN32_WCE
void CPlateFundHoldView::Dump(CDumpContext& dc) const
{
	CBaseChartCtlView::Dump(dc);
}
#endif
#endif //_DEBUG


// CPlateFundHoldView 消息处理程序



void CPlateFundHoldView::OnInitialUpdate()
{
	CBaseMessageView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	RedrawDmGraph();
}

void CPlateFundHoldView::RedrawDmGraph()
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CCalculateStockInfoEx * cse =  new CCalculateStockInfoEx(m_hWnd, WM_USER_LOG_1);

	m_MessageCtrl.SetStockRangeForCalculateTask(pDoc, cse);

	cse->ScheduleTask(&CCalculateStockInfoEx::RecentFundHold1);
}

void CPlateFundHoldView::ProcessXmlWParam(string msgType,string code,string model,\
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



void CPlateFundHoldView::RedrawDmGraph(CCalculateTask * cse)
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
		string strSerieName;
		map<string, double>	vec_ratio;
		for(int m=0; m< pCse->m_vec_code.size(); m++)
		{
			if(pCse->m_vec_code[m]==atoi(pCse->m_vec_request_code[i].c_str()))
			{
				
				double a_total = pCse->m_vec_number_A_total_shares[m];
				double a_shares = pCse->m_vec_number_AShares[m];
				double a_fund_hold = pCse->m_vec_number_fund_hold[m];
				string report_date = pCse->m_vec_report_date[m];

				if(a_shares>1)
				{
					//	当为 “发行前股本”时，流通股数可能为 0.00
					vec_ratio[report_date] = (a_fund_hold/ a_shares) * 100;
				}

				if(strSerieName.empty())
				{
					char tmp[32];
					sprintf_s(tmp, 32, "%s(%s)", pCse->m_vec_abbreviation[m].c_str(), pCse->m_vec_request_code[i].c_str());
					strSerieName = string(tmp);
				}	

			}
		}

		//	绘制 “基金持有率”曲线
		CChartLineSerie* pSeries = NULL;
		if(vec_ratio.size()>0)
		{
			pSeries = m_pChartCtrl->CreateLineSerie();
			
			pSeries->SetName(strSerieName);
			pSeries->SetWidth(1);
			// 
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
	if(xmax > 0)
		pBottomAxis->SetMinMax(xmin, xmax);
	else
		pBottomAxis->SetMinMax(0,1);
	pLeftAxis->SetMinMax(0, ymax + 3);

	m_pChartCtrl->GetLegend()->SetVisible(true);
	m_pChartCtrl->RefreshCtrl();

	m_pChartCtrl->EnableRefresh(true);
}
