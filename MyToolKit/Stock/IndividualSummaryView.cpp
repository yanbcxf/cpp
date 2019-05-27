// IndividualSummaryView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "IndividualSummaryView.h"
#include "IndividualShareDoc.h"


// CIndividualSummaryView

IMPLEMENT_DYNCREATE(CIndividualSummaryView, CBaseGridCtlView)

CIndividualSummaryView::CIndividualSummaryView()
{
}

CIndividualSummaryView::~CIndividualSummaryView()
{
}

BEGIN_MESSAGE_MAP(CIndividualSummaryView, CBaseGridCtlView)
END_MESSAGE_MAP()


// CIndividualSummaryView 绘图

void CIndividualSummaryView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CIndividualSummaryView 诊断

#ifdef _DEBUG
void CIndividualSummaryView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CIndividualSummaryView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CIndividualSummaryView 消息处理程序

void CIndividualSummaryView::OnInitialUpdate()
{
	CBaseGridCtlView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();

	CStockApp * pApp = (CStockApp *)AfxGetApp();

	// 删除内存中原先数据，确保重新读入
	{
		AcquireWriteLock(&g_RWlock);

		if(pApp->m_mapCashFlows.count(atoi(pDoc->m_nCode.c_str()))>0)
			pApp->m_mapCashFlows.erase(atoi(pDoc->m_nCode.c_str()));
		if(pApp->m_mapBalance.count(atoi(pDoc->m_nCode.c_str()))>0)
			pApp->m_mapBalance.erase(atoi(pDoc->m_nCode.c_str()));
		if(pApp->m_mapIncomeStatement.count(atoi(pDoc->m_nCode.c_str()))>0)
			pApp->m_mapIncomeStatement.erase(atoi(pDoc->m_nCode.c_str()));

		if(pApp->m_mapTradingDaily.count(atoi(pDoc->m_nCode.c_str()))>0)
			pApp->m_mapTradingDaily.erase(atoi(pDoc->m_nCode.c_str()));
		if(pApp->m_mapCapitalStruct.count(atoi(pDoc->m_nCode.c_str()))>0)
			pApp->m_mapCapitalStruct.erase(atoi(pDoc->m_nCode.c_str()));

		if(pApp->m_mapCapitalFlows.count(atoi(pDoc->m_nCode.c_str()))>0)
			pApp->m_mapCapitalFlows.erase(atoi(pDoc->m_nCode.c_str()));
		if(pApp->m_CSohuPerformanceForecast.count(atoi(pDoc->m_nCode.c_str()))>0)
			pApp->m_CSohuPerformanceForecast.erase(atoi(pDoc->m_nCode.c_str()));

		ReleaseWriteLock(&g_RWlock);
	}

	//JMutexAutoLock lock(pApp->m_mutexStockinfo);
	StockInfo_Reqeust req;
	req.code = atoi(pDoc->m_nCode.c_str());
	req.hwnd = m_hWnd;
	pApp->m_listStockinfoReq.push_front(req);

}


int CIndividualSummaryView::SearchStockInfo(CStockInfo * pStockinfo, int code)
{
	int nRow = -1;
	map<int, int>::iterator it = pStockinfo->m_vec_code.begin();
	map<int, int>::iterator it_end = pStockinfo->m_vec_code.end();
	for(; it!=it_end; it++)
	{
		if(it->second==code)
		{
			nRow = it->first;
			break;
		}
	}
	return nRow;
}


void CIndividualSummaryView::RedrawGridCtrl()
{

	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();

	if(m_pGridCtrl)
	{
		CStockApp * pApp = (CStockApp *)AfxGetApp();

		CStockInfo * pStockInfo = NULL;
		int nRow = -1;

		AcquireReadLock(&g_RWlock);
		do{
			nRow = SearchStockInfo(&pApp->m_CEntrepreneurship, atoi(pDoc->m_nCode.c_str()));
			if(nRow>=0)
			{
				pStockInfo = &pApp->m_CEntrepreneurship;
				break;
			}
			nRow = SearchStockInfo(&pApp->m_CSmallMidEnterprise, atoi(pDoc->m_nCode.c_str()));
			if(nRow>=0)
			{
				pStockInfo = &pApp->m_CSmallMidEnterprise;
				break;
			}
			nRow = SearchStockInfo(&pApp->m_CShenzhenAshares, atoi(pDoc->m_nCode.c_str()));
			if(nRow>=0)
			{
				pStockInfo = &pApp->m_CShenzhenAshares;
				break;
			}
			nRow = SearchStockInfo(&pApp->m_CShanghaiStock, atoi(pDoc->m_nCode.c_str()));
			if(nRow>=0)
			{
				pStockInfo = &pApp->m_CShanghaiStock;
				break;
			}

		}while(0);
		ReleaseReadLock(&g_RWlock);


		if(pStockInfo==NULL)
			return;

		SetGridCell(1,1, true, "公司简称");
		SetGridCell(1,2, false,pStockInfo->m_vec_abbreviation[nRow]);
		SetGridCell(1,3, true, "公司全称");
		SetGridCell(1,4, false,pStockInfo->m_vec_name[nRow]);
		SetGridCell(1,5, true, "所属行业");
		SetGridCell(1,6, false, pStockInfo->m_vec_belong_to_industry[nRow]);

		SetGridCell(2,1, true, "A股上市时间");
		SetGridCell(2,2, false, pStockInfo->m_vec_a_time_to_market[nRow]);
		SetGridCell(2,3, true, "公司网址");
		if (!m_pGridCtrl->SetCellType(2, 4, RUNTIME_CLASS(CGridURLCell)))
			return;
		SetGridCell(2,4, false, pStockInfo->m_vec_web_addr[nRow]);
		SetGridCell(2,5, true, "城 市");
		SetGridCell(2,6, false, pStockInfo->m_vec_province[nRow] + "-" + pStockInfo->m_vec_city[nRow]);


		SetGridCell(3,1, true, "财报名称");
		SetGridCell(3,2, false,"现金流量表");
		SetGridCell(3,3, true, "最早财报日期");
		SetGridCell(3,4, false, "无");
		SetGridCell(3,5, true, "最晚财报日期");
		SetGridCell(3,6, false, "无");
		CCninfoCashFlows cash;
		CBussinessUtils::GetCashFlow(atoi(pDoc->m_nCode.c_str()), cash);
		if(cash.m_vec_row.size()>0)
		{
			SetGridCell(3,3, true, "最早财报日期");
			SetGridCell(3,4, false, cash.m_vec_row[0][_T("截止日期")]);
			SetGridCell(3,5, true, "最晚财报日期");
			SetGridCell(3,6, false, cash.m_vec_row[cash.m_vec_row.size()-1][_T("截止日期")]);
		}

		SetGridCell(4,1, true, "财报名称");
		SetGridCell(4,2, false,"资产负债表");
		SetGridCell(4,3, true, "最早财报日期");
		SetGridCell(4,4, false, "无");
		SetGridCell(4,5, true, "最晚财报日期");
		SetGridCell(4,6, false, "无");
		CCninfoBalance balance;
		CBussinessUtils::GetBalance(atoi(pDoc->m_nCode.c_str()), balance);
		if(balance.m_vec_row.size()>0)
		{
			SetGridCell(4,3, true, "最早财报日期");
			SetGridCell(4,4, false,balance.m_vec_row[0][_T("截止日期")]);
			SetGridCell(4,5, true, "最晚财报日期");
			SetGridCell(4,6, false,balance.m_vec_row[balance.m_vec_row.size()-1][_T("截止日期")]);
		}

		SetGridCell(5,1, true, "财报名称");
		SetGridCell(5,2, false,"利润表");
		SetGridCell(5,3, true, "最早财报日期");
		SetGridCell(5,4, false, "无");
		SetGridCell(5,5, true, "最晚财报日期");
		SetGridCell(5,6, false, "无");
		CCninfoIncomeStatement income;
		CBussinessUtils::GetIncomestatement(atoi(pDoc->m_nCode.c_str()), income);
		if(income.m_vec_row.size()>0)
		{
			SetGridCell(5,3, true, "最早财报日期");
			SetGridCell(5,4, false, income.m_vec_row[0][_T("截止日期")]);
			SetGridCell(5,5, true, "最晚财报日期");
			SetGridCell(5,6, false, income.m_vec_row[income.m_vec_row.size()-1][_T("截止日期")]);
		}

		SetGridCell(6,1, true, "财报名称");
		SetGridCell(6,2, false,"每日交易数据表");
		SetGridCell(6,3, true, "最早交易数据日期");
		SetGridCell(6,4, false, "无");
		SetGridCell(5,5, true, "最晚交易数据日期");
		SetGridCell(6,6, false, "无");
		CNetEaseTradeDaily trading;
		CBussinessUtils::GetSohuTradingDaily(atoi(pDoc->m_nCode.c_str()), trading);
		if(trading.m_vec_report_date.size()>0)
		{
			int nNum = trading.m_vec_report_date.size();
			/*map<int, string>::iterator it_first = trading.m_vec_report_date.begin();
			map<int, string>::reverse_iterator it_last = trading.m_vec_report_date.rbegin();*/
			SetGridCell(6,3, true, "最早交易数据日期");
			SetGridCell(6,4, false, trading.m_vec_report_date[0]);
			SetGridCell(6,5, true, "最晚交易数据日期");
			SetGridCell(6,6, false, trading.m_vec_report_date[nNum-1]);
		}

		SetGridCell(7,1, true, "财报名称");
		SetGridCell(7,2, false,"业绩预告");
		SetGridCell(7,3, true, "最早业绩预告日期");
		SetGridCell(7,4, false, "无");
		SetGridCell(7,5, true, "最晚业绩预告日期");
		SetGridCell(7,6, false, "无");
		CSohuPerformanceForecast forecast;
		CBussinessUtils::GetSohuPerformanceForecast(atoi(pDoc->m_nCode.c_str()), forecast);
		if(forecast.m_vec_report_date.size()>0)
		{
			map<int, string>::iterator it_first = forecast.m_vec_report_date.begin();
			map<int, string>::reverse_iterator it_last = forecast.m_vec_report_date.rbegin();
			SetGridCell(7,3, true, "最早业绩预告日期");
			SetGridCell(7,4, false, it_first->second);
			SetGridCell(7,5, true, "最晚业绩预告日期");
			SetGridCell(7,6, false, it_last->second);
		}

		SetGridCell(8,1, true, "财报名称");
		SetGridCell(8,2, false,"每日资金流向");
		SetGridCell(8,3, true, "最早每日资金流向日期");
		SetGridCell(8,4, false, "无");
		SetGridCell(8,5, true, "最晚每日资金流向日期");
		SetGridCell(8,6, false, "无");
		CCapitalFlows163 capitalflows;
		CBussinessUtils::GetCapitalFlow(atoi(pDoc->m_nCode.c_str()), capitalflows);
		if(capitalflows.m_vec_report_date.size()>0)
		{
			map<int, string>::iterator it_first = capitalflows.m_vec_report_date.begin();
			map<int, string>::reverse_iterator it_last = capitalflows.m_vec_report_date.rbegin();
			SetGridCell(8,3, true, "最早每日资金流向日期");
			SetGridCell(8,4, false, it_first->second);
			SetGridCell(8,5, true, "最晚每日资金流向日期");
			SetGridCell(8,6, false, it_last->second);
		}

		//////////////////////////////////////////////////////////////////////////
		//	
		CString urlSohu;
	
		urlSohu.Format("http://q.stock.sohu.com/cn/%06d/index.shtml" ,pStockInfo->m_vec_code[nRow]);
		SetGridCell(12,1, true, "搜狐股票");
		if (!m_pGridCtrl->SetCellType(12, 2, RUNTIME_CLASS(CGridURLCell)))
			return;
		SetGridCell(12,2, false, urlSohu.GetBuffer());

		urlSohu.Format("http://stockpage.10jqka.com.cn/%06d//holder/#astockchange" ,pStockInfo->m_vec_code[nRow]);
		SetGridCell(13,1, true, "同花顺股本结构");
		if (!m_pGridCtrl->SetCellType(13, 2, RUNTIME_CLASS(CGridURLCell)))
			return;
		SetGridCell(13,2, false, urlSohu.GetBuffer());

		urlSohu.Format("http://yanbao.stock.hexun.com/yb_%06d.shtml" ,pStockInfo->m_vec_code[nRow]);
		SetGridCell(14,1, true, "和讯网研报");
		if (!m_pGridCtrl->SetCellType(14, 2, RUNTIME_CLASS(CGridURLCell)))
			return;
		SetGridCell(14,2, false, urlSohu.GetBuffer());

		urlSohu.Format("http://www.goomj.com/xmp/xmp866.htm" ,pStockInfo->m_vec_code[nRow]);
		SetGridCell(15,1, true, "公司成长性分析");
		if (!m_pGridCtrl->SetCellType(15, 2, RUNTIME_CLASS(CGridURLCell)))
			return;
		SetGridCell(15,2, false, urlSohu.GetBuffer());

		urlSohu.Format("http://www.cninfo.com.cn/information/companyinfo_n.html?fulltext?szmb%06d" ,pStockInfo->m_vec_code[nRow]);
		SetGridCell(16,1, true, "巨潮网");
		if (!m_pGridCtrl->SetCellType(16, 2, RUNTIME_CLASS(CGridURLCell)))
			return;
		SetGridCell(16,2, false, urlSohu.GetBuffer());

		//m_pGridCtrl->ExpandColumnsToFit(TRUE);
		//m_pGridCtrl->ExpandToFit(false);
		m_pGridCtrl->AutoSize();
		m_pGridCtrl->SetColumnWidth(2, 160);
		m_pGridCtrl->SetColumnWidth(4, 160);
	}

}


void CIndividualSummaryView::ProcessXmlWParam(string msgType,string code,string model,\
											  string param1,string param2,string param3)
{
	RedrawGridCtrl();
}