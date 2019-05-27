// PlateInventoryTurnoverRatioView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "IndividualShareDoc.h"
#include "CalculateStockInfoEx.h"
#include "PlateInventoryTurnoverRatioView.h"


// CPlateInventoryTurnoverRatioView

IMPLEMENT_DYNCREATE(CPlateInventoryTurnoverRatioView, CBaseChartCtlView)

CPlateInventoryTurnoverRatioView::CPlateInventoryTurnoverRatioView()
{

}

CPlateInventoryTurnoverRatioView::~CPlateInventoryTurnoverRatioView()
{
}

BEGIN_MESSAGE_MAP(CPlateInventoryTurnoverRatioView, CBaseChartCtlView)
END_MESSAGE_MAP()


// CPlateInventoryTurnoverRatioView 绘图

void CPlateInventoryTurnoverRatioView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CPlateInventoryTurnoverRatioView 诊断

#ifdef _DEBUG
void CPlateInventoryTurnoverRatioView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CPlateInventoryTurnoverRatioView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CPlateInventoryTurnoverRatioView 消息处理程序


void CPlateInventoryTurnoverRatioView::OnInitialUpdate()
{
	CBaseMessageView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	RedrawDmGraph();
}

void CPlateInventoryTurnoverRatioView::RedrawDmGraph()
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CCalculateStockInfoEx * cse =  new CCalculateStockInfoEx(m_hWnd, WM_USER_LOG_1);

	m_MessageCtrl.SetStockRangeForCalculateTask(pDoc, cse);

	cse->ScheduleTask(&CCalculateStockInfoEx::RecentBalanceAndIncomestatement1);
}

void CPlateInventoryTurnoverRatioView::ProcessXmlWParam(string msgType,string code,string model,\
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



void CPlateInventoryTurnoverRatioView::RedrawDmGraph(CCalculateTask * cse)
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
				//	营业成本
				double cost_of_operations = pCse->m_vec_cost_of_operations[m];
				string report_date = pCse->m_vec_report_date[m];

				COleDateTime odt_in ;
				odt_in.ParseDateTime(report_date.c_str());
				int mon = odt_in.GetMonth();

				// 计算“年化营业成本”
				if(mon==3)
					cost_of_operations = 4 * cost_of_operations;
				else if(mon==6)
					cost_of_operations = 2 * cost_of_operations;
				else if(mon==9)
					cost_of_operations = 4 * cost_of_operations / 3;

				//	寻找 期初存货
				int j = m -1;
				double original_Inventory = -1.0;
				for(; j>=0; j--)
				{
					string report_date1 = pCse->m_vec_report_date[j];
					COleDateTime odt_in ;
					odt_in.ParseDateTime(report_date1.c_str());
					int mon = odt_in.GetMonth();
					if(mon==12)
					{
						original_Inventory = pCse->m_vec_inventories[j];
						break;
					}
				}
				//	存货平均余额=（期初存货+期末存货）÷2
				if(original_Inventory>0)
				{
					//	计算“存货周转率”
					double inventory = (original_Inventory + pCse->m_vec_inventories[m])/2;
					vec_ratio[report_date] = (cost_of_operations/ inventory);
				}
				else
				{
					vec_ratio[report_date] = 0;
				}

				if(strSerieName.empty())
				{
					char tmp[32];
					sprintf_s(tmp, 32, "%s(%s)", pCse->m_vec_abbreviation[m].c_str(), pCse->m_vec_request_code[i].c_str());
					strSerieName = string(tmp);
				}			

			}
		}

		//	绘制 “存货周转率”曲线
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
	pBottomAxis->SetMinMax(xmin, xmax);
	pLeftAxis->SetMinMax(0, ymax + 3);

	m_pChartCtrl->GetLegend()->SetVisible(true);
	m_pChartCtrl->RefreshCtrl();

	m_pChartCtrl->EnableRefresh(true);
}
