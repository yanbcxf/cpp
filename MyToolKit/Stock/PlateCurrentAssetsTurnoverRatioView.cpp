// PlateCurrentAssetsTurnoverRatioView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "IndividualShareDoc.h"
#include "CalculateStockInfoEx.h"
#include "PlateCurrentAssetsTurnoverRatioView.h"


// CPlateCurrentAssetsTurnoverRatioView

IMPLEMENT_DYNCREATE(CPlateCurrentAssetsTurnoverRatioView, CBaseChartCtlView)

CPlateCurrentAssetsTurnoverRatioView::CPlateCurrentAssetsTurnoverRatioView()
{

}

CPlateCurrentAssetsTurnoverRatioView::~CPlateCurrentAssetsTurnoverRatioView()
{
}

BEGIN_MESSAGE_MAP(CPlateCurrentAssetsTurnoverRatioView, CBaseChartCtlView)
END_MESSAGE_MAP()


// CPlateCurrentAssetsTurnoverRatioView 绘图

void CPlateCurrentAssetsTurnoverRatioView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CPlateCurrentAssetsTurnoverRatioView 诊断

#ifdef _DEBUG
void CPlateCurrentAssetsTurnoverRatioView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CPlateCurrentAssetsTurnoverRatioView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CPlateCurrentAssetsTurnoverRatioView 消息处理程序


void CPlateCurrentAssetsTurnoverRatioView::OnInitialUpdate()
{
	CBaseMessageView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	RedrawDmGraph();
}

void CPlateCurrentAssetsTurnoverRatioView::RedrawDmGraph()
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CCalculateStockInfoEx * cse =  new CCalculateStockInfoEx(m_hWnd, WM_USER_LOG_1);

	m_MessageCtrl.SetStockRangeForCalculateTask(pDoc, cse);

	cse->ScheduleTask(&CCalculateStockInfoEx::RecentBalanceAndIncomestatement1);
}

void CPlateCurrentAssetsTurnoverRatioView::ProcessXmlWParam(string msgType,string code,string model,\
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


void CPlateCurrentAssetsTurnoverRatioView::RedrawDmGraph(CCalculateTask * cse)
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
				double sales_of_operation = pCse->m_vec_sales_from_operations[m];
				double total_assets = pCse->m_vec_total_current_assets[m] + pCse->m_vec_total_non_current_assets[m];
				string report_date = pCse->m_vec_report_date[m];

				COleDateTime odt_in ;
				odt_in.ParseDateTime(report_date.c_str());
				int mon = odt_in.GetMonth();

				// 计算“年化营业收入”
				if(mon==3)
					sales_of_operation = 4 * sales_of_operation;
				else if(mon==6)
					sales_of_operation = 2 * sales_of_operation;
				else if(mon==9)
					sales_of_operation = 4 * sales_of_operation / 3;

				//	寻找 期初流动资产
				int j = m -1;
				double original_current = -1.0;
				for(; j>=0; j--)
				{
					string report_date1 = pCse->m_vec_report_date[j];
					COleDateTime odt_in ;
					odt_in.ParseDateTime(report_date1.c_str());
					int mon = odt_in.GetMonth();
					if(mon==12)
					{
						original_current = pCse->m_vec_total_current_assets[j];
						break;
					}
				}
				//	流动资产平均余额=（期初流动资产 + 期末流动资产）÷2
				if(original_current>0)
				{
					//	计算“年化 流动资产周转率”
					double current = (original_current + pCse->m_vec_total_current_assets[m])/2;
					vec_ratio[report_date] = (sales_of_operation/ current);
				}
				else
				{
					vec_ratio[report_date] = 0;
				}
			}
		}

		//	绘制 “年化 流动资产周转率”曲线
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
	pLeftAxis->SetMinMax(0, ymax + 1);

	m_pChartCtrl->GetLegend()->SetVisible(true);
	m_pChartCtrl->RefreshCtrl();

	m_pChartCtrl->EnableRefresh(true);

}