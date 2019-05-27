// PlateDealDetailDailyView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "PlateDealDetailDailyView.h"

#include "CalcDealDetailDaily.h"
#include "SelfSelectPlateDlg.h"


// CPlateDealDetailDailyView

IMPLEMENT_DYNCREATE(CPlateDealDetailDailyView, CBaseChartCtlView)

CPlateDealDetailDailyView::CPlateDealDetailDailyView()
{

}

CPlateDealDetailDailyView::~CPlateDealDetailDailyView()
{
}

BEGIN_MESSAGE_MAP(CPlateDealDetailDailyView, CBaseChartCtlView)
	ON_COMMAND(ID_TABLE_ADD_TO_SELF_SELECT, &CPlateDealDetailDailyView::OnTableAddToSelfSelect)
END_MESSAGE_MAP()


// CPlateDealDetailDailyView 绘图

void CPlateDealDetailDailyView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CPlateDealDetailDailyView 诊断

#ifdef _DEBUG
void CPlateDealDetailDailyView::AssertValid() const
{
	CBaseChartCtlView::AssertValid();
}

#ifndef _WIN32_WCE
void CPlateDealDetailDailyView::Dump(CDumpContext& dc) const
{
	CBaseChartCtlView::Dump(dc);
}
#endif
#endif //_DEBUG


// CPlateDealDetailDailyView 消息处理程序



void CPlateDealDetailDailyView::OnInitialUpdate()
{
	CBaseMessageView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	RedrawDmGraph();
}

void CPlateDealDetailDailyView::RedrawDmGraph()
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CCalcDealDetailDaily * cse =  new CCalcDealDetailDaily(m_hWnd, WM_USER_LOG_1);
	cse->m_nRequestDealDate = atoi(pDoc->m_strInput.c_str());

	m_MessageCtrl.SetStockRangeForCalculateTask(pDoc, cse, CBaseMessageControl::Search_Stock);
	cse->ScheduleTask(&CCalcDealDetailDaily::Execute1);
}

void CPlateDealDetailDailyView::ProcessXmlWParam(string msgType,string code,string model,\
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


void CPlateDealDetailDailyView::RedrawDmGraph(CCalculateTask * cse)
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CStockApp * pApp = (CStockApp *)AfxGetApp();

	ASSERT_VALID(pDoc);	

	m_pChartCtrl->EnableRefresh(false);

	CRect rect = m_pChartCtrl->GetPlottingRect();

	CCalcDealDetailDaily * pCse =  (CCalcDealDetailDaily *)cse;
	m_pChartCtrl->RemoveAllSeries();

	CChartDateTimeAxis* pBottomAxis = m_pChartCtrl->CreateDateTimeAxis(CChartCtrl::BottomAxis);
	pBottomAxis->SetTickLabelFormat(false, "%H:%M:%S");
	CChartStandardAxis* pLeftAxis = m_pChartCtrl->CreateStandardAxis(CChartCtrl::LeftAxis);

	double xmin, xmax, ymin, ymax;
	xmax = 0; xmin = 99999.0;
	ymin = 99999.0; ymax = 0.0;

	//	绘制 “成交明细”曲线
	int nLastCode = -1;
	double open_price = -1;
	CChartLineSerie* pSeries = NULL;
	for(int m=0; m< pCse->m_vec_code.size(); m++)
	{	
		if(nLastCode!=pCse->m_vec_code[m])
		{
			open_price = pCse->m_vec_deal_price[m];
			nLastCode = pCse->m_vec_code[m];

			pSeries = m_pChartCtrl->CreateLineSerie();
			pSeries->SetWidth(1);
			char tmp[32];
			sprintf_s(tmp, 32, "%06d", pCse->m_vec_code[m]);

			pSeries->SetName( tmp /* abbreviation.c_str()*/);

			pSeries->EnableMouseNotifications(true,false);
			pSeries->RegisterMouseListener(new CBaseChartCtlViewMouseListener(this));
		}

		string report_date = pCse->m_vec_deal_time[m];
		COleDateTime odt ;
		odt.ParseDateTime(report_date.c_str());
		if(m_StartDate> 0 && m_EndDate> m_StartDate)
		{
			if(odt.m_dt < m_StartDate || odt.m_dt > m_EndDate)
				continue;
		}

		// 以开盘价作为“基准”
		double ratio = pCse->m_vec_deal_price[m] / open_price * 100;
		if(odt.m_dt> xmax) xmax = odt.m_dt;
		if(odt.m_dt< xmin) xmin = odt.m_dt;

		if(ratio > ymax) ymax = ratio;
		if(ratio < ymin) ymin = ratio;

		pSeries->AddPoint(odt.m_dt, ratio);
	}

	if(ymax>0 && xmax > xmin)
	{
		pBottomAxis->SetMinMax(xmin, xmax);
		pLeftAxis->SetMinMax(ymin, ymax);

		m_pChartCtrl->GetLegend()->SetVisible(true);
		m_pChartCtrl->RefreshCtrl();
	}

	m_pChartCtrl->EnableRefresh(true);
	
}

void CPlateDealDetailDailyView::OnTableAddToSelfSelect()
{
	// TODO: 在此添加命令处理程序代码

	vector<string> vecSeries = GetVisibleSeries();

	map<int ,string >  codes;
	for(int i=0; i< vecSeries.size(); i++)
	{
		CStockInfoData stock = CBussinessUtils::GetStockInfo(atoi(vecSeries[i].c_str()));	
		codes[stock.code] = stock.abbreviation;
	}

	if(vecSeries.size()>0)
	{
		CSelfSelectPlateDlg dlg;
		dlg.DoModal();

		if(dlg.m_nPlateCode> 0)
		{
			JMutexAutoLock lock(((CStockApp *)AfxGetApp())->m_pCSelfSelectStockPlates->m_mutex);
			map<int ,string >::iterator it = codes.begin();
			for(; it!=codes.end(); it++)
			{
				int code = it->first;

				CStockPlateData treeNode;
				treeNode.code = Int2String(code, "%06d");
				treeNode.name = it->second;
				treeNode.company_number = 1;
				treeNode.is_stock = 1;

				CStockPlateData treeNodeParent;
				treeNodeParent.code = Int2String(dlg.m_nPlateCode, "%08d");
				treeNodeParent.is_stock = 0;


				CStockPlateTree * pPlates = ((CStockApp *)AfxGetApp())->m_pCSelfSelectStockPlates->m_pTree;
				CStockPlateTree * pNode  = pPlates->SearchSpecialNode(treeNodeParent);
				if(pNode)
				{
					pNode->insert((const CStockPlateData&)treeNode);
				}

			}
			((CStockApp *)AfxGetApp())->m_pCSelfSelectStockPlates->ImportToDatabase();

			StockInfo_Reqeust req;
			req.code = -ID_READ_STOCKPLATE_SELF_SELECT;
			req.hwnd = NULL;
			((CStockApp *)AfxGetApp())->m_listStockinfoReq.push_front(req);
		}
	}

}
