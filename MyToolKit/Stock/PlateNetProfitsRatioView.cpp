// PlateNetProfitsRatioView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "IndividualShareDoc.h"
#include "CalculateStockInfoEx.h"
#include "PlateNetProfitsRatioView.h"


// CPlateNetProfitsRatioView

IMPLEMENT_DYNCREATE(CPlateNetProfitsRatioView, CBaseChartCtlView)

CPlateNetProfitsRatioView::CPlateNetProfitsRatioView()
{

}

CPlateNetProfitsRatioView::~CPlateNetProfitsRatioView()
{
}

BEGIN_MESSAGE_MAP(CPlateNetProfitsRatioView, CBaseChartCtlView)
END_MESSAGE_MAP()


// CPlateNetProfitsRatioView 绘图

void CPlateNetProfitsRatioView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CPlateNetProfitsRatioView 诊断

#ifdef _DEBUG
void CPlateNetProfitsRatioView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CPlateNetProfitsRatioView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CPlateNetProfitsRatioView 消息处理程序



void CPlateNetProfitsRatioView::OnInitialUpdate()
{
	CBaseMessageView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	RedrawDmGraph();
}

void CPlateNetProfitsRatioView::RedrawDmGraph()
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CCalculateStockInfoEx * cse =  new CCalculateStockInfoEx(m_hWnd, WM_USER_LOG_1);

	m_MessageCtrl.SetStockRangeForCalculateTask(pDoc, cse);

	cse->ScheduleTask(&CCalculateStockInfoEx::RecentNetProfitsRatio1);
}

void CPlateNetProfitsRatioView::ProcessXmlWParam(string msgType,string code,string model,\
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


void CPlateNetProfitsRatioView::RedrawDmGraph(CCalculateTask * cse)
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
		string abbreviation;
		map<string, double>	vec_net_profits_ratio;
		for(int m=0; m< pCse->m_vec_code.size(); m++)
		{
			if(pCse->m_vec_code[m]==atoi(pCse->m_vec_request_code[i].c_str()))
			{
				vec_net_profits_ratio[pCse->m_vec_report_date[m]] = pCse->m_vec_net_profits_ratio[m];

				if(abbreviation.empty())
				{
					abbreviation = pCse->m_vec_abbreviation[m];
				}
			}
		}

		//	绘制 “净利润比使用资金净额”曲线
		CChartLineSerie* pSeries = NULL;
		if(vec_net_profits_ratio.size()>0)
		{
			pSeries = m_pChartCtrl->CreateLineSerie();
			char tmp[32];
			sprintf_s(tmp, 32, "%s(%s)",abbreviation.c_str(), pCse->m_vec_request_code[i].c_str());
			pSeries->SetName(tmp);
			pSeries->SetWidth(1);

			pSeries->EnableMouseNotifications(true,false);
			pSeries->RegisterMouseListener(new CBaseChartCtlViewMouseListener(this));
		}
		map<string, double>::iterator it = vec_net_profits_ratio.begin();
		for(; it!=vec_net_profits_ratio.end(); it++)
		{
			string report_date = it->first;
			COleDateTime odt ;
			odt.ParseDateTime(report_date.c_str());
			if(m_StartDate> 0 && m_EndDate> m_StartDate)
			{
				if(odt.m_dt < m_StartDate || odt.m_dt > m_EndDate)
					continue;
			}

			double net_profits_ratio = it->second;
			if(odt.m_dt> xmax) xmax = odt.m_dt;
			if(odt.m_dt< xmin) xmin = odt.m_dt;

			if(net_profits_ratio > ymax) ymax = net_profits_ratio;
			if(net_profits_ratio < ymin) ymin = net_profits_ratio;

			pSeries->AddPoint(odt.m_dt, net_profits_ratio);
		}
	}
	if(xmin < xmax)
		pBottomAxis->SetMinMax(xmin, xmax);
	if(ymin < ymax)
		pLeftAxis->SetMinMax(ymin, ymax);

	m_pChartCtrl->GetLegend()->SetVisible(true);
	m_pChartCtrl->RefreshCtrl();

	m_pChartCtrl->EnableRefresh(true);
}
