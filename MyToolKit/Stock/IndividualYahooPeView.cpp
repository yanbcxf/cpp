// IndividualYahooPeView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "IndividualShareDoc.h"
#include "IndividualYahooPeView.h"


// CIndividualYahooPeView

IMPLEMENT_DYNCREATE(CIndividualYahooPeView, CBaseChartCtlView)

CIndividualYahooPeView::CIndividualYahooPeView()
{

}

CIndividualYahooPeView::~CIndividualYahooPeView()
{
}

BEGIN_MESSAGE_MAP(CIndividualYahooPeView, CBaseChartCtlView)
END_MESSAGE_MAP()


// CIndividualYahooPeView 绘图

void CIndividualYahooPeView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CIndividualYahooPeView 诊断

#ifdef _DEBUG
void CIndividualYahooPeView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CIndividualYahooPeView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CIndividualYahooPeView 消息处理程序


void CIndividualYahooPeView::RedrawDmGraph()
{
	// 来自“Yahoo 股价曲线” 和 通过结合股本结构计算的动态市盈率

	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CStockApp * pApp = (CStockApp *)AfxGetApp();

	ASSERT_VALID(pDoc);	

	HRESULT hr;
	vector<string>   vec_annotion;
	vector<COLORREF> vec_clr;

	CRect rect;
	m_pChartCtrl->GetClientRect(&rect);
	m_pChartCtrl->RemoveAllSeries();

	CNetEaseTradeDaily trading;
	CBussinessUtils::GetSohuTradingDaily(atoi(pDoc->m_nCode.c_str()), trading);
	if(trading.m_vec_report_date.size()>0)
	{
		int nStart = 0, nEnd = trading.m_vec_report_date.size()-1;
		if(m_StartDate> 0 && m_EndDate> m_StartDate)
		{
			//	寻找“起始”的记录
			for(int i=0 ; i<trading.m_vec_report_date.size(); i++)
			{
				COleDateTime odt ;
				odt.ParseDateTime(trading.m_vec_report_date[i].c_str());
				if(odt.m_dt >= m_StartDate)
				{
					nStart = i;
					break;
				}
			}

			//	寻找“结束”的记录
			for(int i=trading.m_vec_report_date.size() - 1; i>=0; i--)
			{
				COleDateTime odt ;
				odt.ParseDateTime(trading.m_vec_report_date[i].c_str());
				if(odt.m_dt<= m_EndDate)
				{
					nEnd = i;
					break;
				}
			}
		}

		//////////////////////////////////////////////////////////////////////////
		// 绘制价格曲线

		double xmin, xmax, ymin, ymax;
		ymin = 99999.0; ymax = 0.0;

		for(long i=0; i<  nEnd - nStart +1; i++)
		{
			COleDateTime odt ;
			double yy = 0.0;
			odt.ParseDateTime(trading.m_vec_report_date[nStart + i].c_str());
			if(i==0)	xmin = odt.m_dt;
			if(i== nEnd - nStart) xmax = odt.m_dt;
			if(trading.m_vec_reorganize.count(trading.m_vec_report_date[nStart + i])>0)
			{
				CNetEaseTradeDailyData data = trading.m_vec_reorganize[trading.m_vec_report_date[nStart + i]];
				if(data.high> ymax)  ymax = data.high;
				if(data.low<ymin)	ymin = data.low;
			}
		}

		CChartDateTimeAxis* pBottomAxis = m_pChartCtrl->CreateDateTimeAxis(CChartCtrl::BottomAxis);
		pBottomAxis->SetTickLabelFormat(false, "%Y%m%d");
		pBottomAxis->SetMinMax(xmin, xmax);

		CChartStandardAxis* pLeftAxis = m_pChartCtrl->CreateStandardAxis(CChartCtrl::LeftAxis);
		//pLeftAxis->SetMinMax(ymax- (ymax-ymin)*2, ymax);	//	占上半部 ，共 1/2
		pLeftAxis->SetMinMax(ymin, ymax);

		bool bSecondHorizAxis = false;
		bool bSecondVertAxis = false;
		//CChartCandlestickSerie* pSeries = m_pChartCtrl->CreateCandlestickSerie(bSecondHorizAxis, bSecondVertAxis);
		CChartLineSerie* pSeries = m_pChartCtrl->CreateLineSerie();
		pSeries->SetName("收盘价");
		int nWidth = (rect.right - rect.left)/(xmax - xmin);
		//if(nWidth <5) nWidth = 5;
		//pSeries->SetWidth(nWidth);
		
		for(long i=0; i<  nEnd - nStart +1; i++)
		{
			COleDateTime odt ;
			double yy = 0.0;
			odt.ParseDateTime(trading.m_vec_report_date[nStart + i].c_str());
			if(trading.m_vec_reorganize.count(trading.m_vec_report_date[nStart + i])>0)
			{
				CNetEaseTradeDailyData data = trading.m_vec_reorganize[trading.m_vec_report_date[nStart + i]];
				//pSeries->AddPoint(odt.m_dt, data.low, data.high, data.open, data.close);
				pSeries->AddPoint(odt.m_dt, data.close);
			}
		}


		//////////////////////////////////////////////////////////////////////////
		//	计算市盈率

		CChartStandardAxis* pRightAxis = m_pChartCtrl->CreateStandardAxis(CChartCtrl::RightAxis);

		CChartLineSerie* pSeries1 = m_pChartCtrl->CreateLineSerie(false, true);
		CChartLineSerie* pSeries2 = m_pChartCtrl->CreateLineSerie(false, true);
		pSeries1->SetName("市盈率");
		pSeries2->SetName("市盈率(加预告)");

		ymin = 0.0; ymax = 0.0;
		for(int i=0; i< nEnd - nStart + 1; i++)
		{
			//	生成“时间轴”
			COleDateTime odt ;
			odt.ParseDateTime(trading.m_vec_report_date[nStart + i].c_str());

			double nShare = CBussinessUtils::CurrentATotalShares(atoi(pDoc->m_nCode.c_str()), trading.m_vec_report_date[nStart + i]);
			nShare = nShare * 10000.0;

			// 根据利润表， 计算动态市盈率
			COleDateTime odt_in ;
			string baseDate;
			bool isLast;
			double nProfits1 = CBussinessUtils::NetProfitBelongingToParentCompanyFromIncome(atoi(pDoc->m_nCode.c_str()), \
				trading.m_vec_report_date[nStart + i], baseDate, isLast);

			odt_in.ParseDateTime(baseDate.c_str());
			int mon = odt_in.GetMonth();
			
			if(mon==3)
				nProfits1 = 4 * nProfits1;
			else if(mon==6)
				nProfits1 = 2 * nProfits1;
			else if(mon==9)
				nProfits1 = 4 * nProfits1 / 3;

			double peClose1;
			if(nProfits1> 0 && nShare >0)
			{
				double profitPerShare = nProfits1/nShare;
				peClose1 = trading.m_vec_close[nStart + i]/profitPerShare;
			}
			else if(nProfits1<=0)
			{
				peClose1 = -1;
			}

			pSeries1->AddPoint(odt.m_dt, peClose1);
			if(isLast==false)
				pSeries2->AddPoint(odt.m_dt, peClose1);

			if(peClose1> ymax)  ymax = peClose1;
			if(peClose1<ymin)	ymin = peClose1;

			//	计算动态市盈率（末尾附加 业绩预告）， isLast == true ，说明是根据最晚的利润表计算得到的
			if(isLast)
			{
				double nProfits2 = CBussinessUtils::NetProfitBelongingToParentCompanyFromForecast(atoi(pDoc->m_nCode.c_str()), \
					trading.m_vec_report_date[nStart + i], baseDate);
				if(nProfits2>0)
				{
					odt_in.ParseDateTime(baseDate.c_str());
					int mon = odt_in.GetMonth();

					if(mon==3)
						nProfits2 = 4 * nProfits2;
					else if(mon==6)
						nProfits2 = 2 * nProfits2;
					else if(mon==9)
						nProfits2 = 4 * nProfits2 / 3;

					if(nProfits2> 0 && nShare >0)
					{
						double profitPerShare = nProfits2/nShare;
						double peClose = trading.m_vec_close[nStart + i]/profitPerShare;

						pSeries2->AddPoint(odt.m_dt, peClose);

						if(peClose> ymax)  ymax = peClose;
						if(peClose<ymin)	ymin = peClose;
					}
					else if(nProfits2<=0)
					{
						pSeries2->AddPoint(odt.m_dt, -1.0);
					}
				}
				else
				{
					pSeries2->AddPoint(odt.m_dt, peClose1);
				}
			}		
		}
		pRightAxis->SetMinMax(ymin, ymax);

	}

	m_pChartCtrl->GetLegend()->SetVisible(true);
	m_pChartCtrl->RefreshCtrl();

	//////////////////////////////////////////////////////////////////////////
	// 打上曲线的注释

}



void CIndividualYahooPeView::OnInitialUpdate()
{
	CBaseMessageView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();

	CStockApp * pApp = (CStockApp *)AfxGetApp();
	StockInfo_Reqeust req;
	req.code = atoi(pDoc->m_nCode.c_str());
	req.hwnd = m_hWnd;
	pApp->m_listStockinfoReq.push_front(req);

}