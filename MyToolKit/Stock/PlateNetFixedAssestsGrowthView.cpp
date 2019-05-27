// PlateNetFixedAssestsGrowthView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "IndividualShareDoc.h"
#include "PlateNetFixedAssestsGrowthView.h"
#include "CalcAssetsGrowth.h"


// CPlateNetFixedAssestsGrowthView

IMPLEMENT_DYNCREATE(CPlateNetFixedAssestsGrowthView, CBaseChartCtlView)

CPlateNetFixedAssestsGrowthView::CPlateNetFixedAssestsGrowthView()
{

}

CPlateNetFixedAssestsGrowthView::~CPlateNetFixedAssestsGrowthView()
{
}


// CPlateNetFixedAssestsGrowthView 消息处理程序
BEGIN_MESSAGE_MAP(CPlateNetFixedAssestsGrowthView, CBaseChartCtlView)
END_MESSAGE_MAP()

void CPlateNetFixedAssestsGrowthView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CPlateTotalAssetsGrowthView 诊断

#ifdef _DEBUG
void CPlateNetFixedAssestsGrowthView::AssertValid() const
{
	CBaseChartCtlView::AssertValid();
}

#ifndef _WIN32_WCE
void CPlateNetFixedAssestsGrowthView::Dump(CDumpContext& dc) const
{
	CBaseChartCtlView::Dump(dc);
}
#endif
#endif //_DEBUG

void CPlateNetFixedAssestsGrowthView::OnInitialUpdate()
{
	CBaseMessageView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	RedrawDmGraph();
}

void CPlateNetFixedAssestsGrowthView::RedrawDmGraph()
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CCalcAssetsGrowth * cse =  new CCalcAssetsGrowth(m_hWnd, WM_USER_LOG_1);

	m_MessageCtrl.SetStockRangeForCalculateTask(pDoc, cse);

	cse->ScheduleTask(&CCalcAssetsGrowth::Execute1);
}

void CPlateNetFixedAssestsGrowthView::ProcessXmlWParam(string msgType,string code,string model,\
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



void CPlateNetFixedAssestsGrowthView::RedrawDmGraph(CCalculateTask * cse)
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CStockApp * pApp = (CStockApp *)AfxGetApp();

	ASSERT_VALID(pDoc);	

	m_pChartCtrl->EnableRefresh(false);

	CRect rect = m_pChartCtrl->GetPlottingRect();

	CCalcAssetsGrowth * pCse =  (CCalcAssetsGrowth *)cse;
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
		string abbreviation ;
		map<string, double>	vec_ratio;
		for(int m=0; m< pCse->m_vec_code.size(); m++)
		{
			if(pCse->m_vec_code[m]==atoi(pCse->m_vec_request_code[i].c_str()))
			{
				string report_date = pCse->m_vec_report_date[m];

				/*固定资产净值 + 在建工程*/
				vec_ratio[report_date] = pCse->m_vec_net_fixed_assets[m] + pCse->m_vec_construction_in_progress[m];

				//abbreviation = pCse->m_vec_abbreviation[m];
			}
		}
		//	绘制 “总资产的增长”曲线
		CChartLineSerie* pSeries = NULL;
		if(vec_ratio.size()>0)
		{
			pSeries = m_pChartCtrl->CreateLineSerie();
			char tmp[32];
			sprintf_s(tmp, 32, "%s", pCse->m_vec_request_code[i].c_str());
			pSeries->SetName(tmp /*abbreviation.c_str()*/);
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
	pLeftAxis->SetMinMax(ymin, ymax);

	m_pChartCtrl->GetLegend()->SetVisible(true);
	m_pChartCtrl->RefreshCtrl();

	m_pChartCtrl->EnableRefresh(true);
}

