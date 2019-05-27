// IndividualShareView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "IndividualShareView.h"
#include "IndividualShareDoc.h"

#include "StockRangeSet.h"


// CIndividualShareView

IMPLEMENT_DYNCREATE(CIndividualShareView, CView)

CIndividualShareView::CIndividualShareView()
{
	m_EndDate = -1.0;
	m_StartDate = -1.0;

	m_strViewType = "Open";

}

CIndividualShareView::~CIndividualShareView()
{
}

BEGIN_MESSAGE_MAP(CIndividualShareView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_MESSAGE(WM_USER_XML, OnUserXmlWParam)
	ON_COMMAND(ID_SET_STOCK_RANGE, &CIndividualShareView::OnSetStockRange)
END_MESSAGE_MAP()


// CIndividualShareView 绘图

void CIndividualShareView::OnDraw(CDC* pDC)
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	// TODO: 在此添加绘制代码

	//PeCurve(nStart,nEnd);
}


// CIndividualShareView 诊断

#ifdef _DEBUG
void CIndividualShareView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CIndividualShareView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CIndividualShareView 消息处理程序

int CIndividualShareView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	RECT rc;
	GetClientRect(&rc);

	CRect rect(100, 10, 200, 110);

	m_DMGraph.Create(NULL, WS_CHILD | WS_VISIBLE, rect, this, 1);

	//	设置 X 轴的时间坐标
	LPDISPATCH lpdis = m_DMGraph.get_Axis(0/*HorizontalX*/);
	if(lpdis)
	{
		CDMGraphAxis dmgraphAxis(lpdis);
		dmgraphAxis.put_Format("%Y-%m-%d");
		dmgraphAxis.put_Time(VARIANT_TRUE);
	}

	//	设置 Y 轴的价格坐标
	lpdis = m_DMGraph.get_Axis(1/*HorizontalY*/);
	if(lpdis)
	{
		CDMGraphAxis dmgraphAxis(lpdis);
		dmgraphAxis.put_Format("%.2f");
	}

	m_DMGraph.put_LabelColor(RGB(0,0,0));
	//
	return 0;
}

void CIndividualShareView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();

	CStockApp * pApp = (CStockApp *)AfxGetApp();
	//JMutexAutoLock lock(pApp->m_mutexStockinfo);
	StockInfo_Reqeust req;
	req.code = atoi(pDoc->m_nCode.c_str());
	req.hwnd = m_hWnd;
	pApp->m_listStockinfoReq.push_front(req);
}

void CIndividualShareView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	CRect rect;
	GetClientRect(&rect);

	m_DMGraph.MoveWindow(rect);
}


LRESULT CIndividualShareView::OnUserXmlWParam(WPARAM wParam, LPARAM lParam)
{
	// we expect the CMessageString in the WParam paramater
	CMessageString* pString = (CMessageString*)wParam;

	// make sure passed string is valid (this is important to prevent unexpected
	// results if an invalid string is passed)
	if (CMessageString::IsStringValid(pString))
	{
		CString curString;
		
		delete pString;

		RedrawDmGraph();
	}
	else
	{
		CString strMessage = "ERROR! Test message using wParam received, but CMessageString passed in wParam is invalid.";
		AfxMessageBox(strMessage);

		ASSERT(FALSE);
		return MESSAGESTRING_NOT_RECEIVED;
	}

	return 0;
}

void CIndividualShareView::RedrawDmGraph()
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();

	CStockApp * pApp = (CStockApp *)AfxGetApp();
	CNetEaseTradeDaily tradingDaily;
	CBussinessUtils::GetSohuTradingDaily(atoi(pDoc->m_nCode.c_str()), tradingDaily);
	if(tradingDaily.m_vec_report_date.size()>0)
	{
		int nStart = 0, nEnd = tradingDaily.m_vec_report_date.size()-1;
		if(m_StartDate> 0 && m_EndDate> m_StartDate)
		{
			//	寻找“起始”的记录
			for(int i=0 ; i<tradingDaily.m_vec_report_date.size(); i++)
			{
				COleDateTime odt ;
				odt.ParseDateTime(tradingDaily.m_vec_report_date[i].c_str());
				if(odt.m_dt >= m_StartDate)
				{
					nStart = i;
					break;
				}
			}

			//	寻找“结束”的记录
			for(int i=tradingDaily.m_vec_report_date.size() - 1; i>=0; i--)
			{
				COleDateTime odt ;
				odt.ParseDateTime(tradingDaily.m_vec_report_date[i].c_str());
				if(odt.m_dt<= m_EndDate)
				{
					nEnd = i;
					break;
				}
			}
		}

		//	数据输入到 控件
		TradingDailyCurve(nStart, nEnd, "Open");

		PeCurve(nStart, nEnd);
		m_DMGraph.AutoRange();
	}
}

void CIndividualShareView::SetGraphData(VARIANT* pvarrX, VARIANT* pvarrY, LPCTSTR szName)
{
	ATLASSERT(pvarrX);
	ATLASSERT(pvarrY);
	ATLASSERT(szName);

	long i, nElementCount = 0;
	BOOL bReplace = FALSE;

	LPDISPATCH lpdis = m_DMGraph.get_Elements();
	if(lpdis==NULL)
		return;

	//	当有多条曲线时， nElementCount >1 
	CDMGraphCollection dmgraphCollection(lpdis);
	nElementCount = dmgraphCollection.get_Count();

	for(i=0; i<nElementCount; i++)
	{
		lpdis = dmgraphCollection.get_Item(i);
		if(lpdis==NULL)
			continue;

		CDMGraphElement dmgraphElement(lpdis);
		CString bsElemName = dmgraphElement.get_Name();

		if(_tcsicmp(bsElemName.GetBuffer(), szName) == 0)
		{
			bReplace = TRUE;

			dmgraphCollection.Delete(i);

			//	存在同名的曲线，则替换
			//dmgraphElement.put_PointSymbol(1 /*Dots*/);
			//dmgraphElement.put_PointSize(1);
			//dmgraphElement.Plot(*pvarrX, *pvarrY);
			break;
		}

	}
	//if(bReplace == FALSE )
	{

		lpdis = dmgraphCollection.Add();
		if(lpdis==NULL)
			return;
		CDMGraphElement dmgraphElement(lpdis);
		dmgraphElement.put_Name(szName);
		dmgraphElement.put_PointSymbol(1 /*Dots*/);
		dmgraphElement.put_PointSize(1);
		dmgraphElement.Plot(*pvarrX, *pvarrY);
	}

}

void CIndividualShareView::TradingDailyCurve(int nStart, int nEnd, string strViewType)
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CStockApp * pApp = (CStockApp *)AfxGetApp();

	ASSERT_VALID(pDoc);

	CComVariant varrX, varrY;
	varrX.parray = SafeArrayCreateVector(VT_R8, 0, nEnd - nStart + 1);
	if(varrX.parray == NULL)
		return ;
	varrX.vt = VT_ARRAY|VT_R8;

	varrY.parray = SafeArrayCreateVector(VT_R8, 0, nEnd - nStart + 1);
	if(varrY.parray == NULL)
		return ;
	varrY.vt = VT_ARRAY|VT_R8;

	HRESULT hr;
	CNetEaseTradeDaily tradingDaily;
	CBussinessUtils::GetSohuTradingDaily(atoi(pDoc->m_nCode.c_str()), tradingDaily);
	if(tradingDaily.m_vec_report_date.size()>0)
	{
		for(long i=0; i< nEnd - nStart + 1; i++)
		{
			COleDateTime odt ;
			odt.ParseDateTime(tradingDaily.m_vec_report_date[nStart + i].c_str());
			hr = SafeArrayPutElement(varrX.parray, &i, &odt.m_dt);
			hr = SafeArrayPutElement(varrY.parray, &i, &tradingDaily.m_vec_open[nStart + i]);

		}
		SetGraphData(&varrX, &varrY, strViewType.c_str());
	}
	
}

void CIndividualShareView::OnSetStockRange()
{
	// TODO: 在此添加命令处理程序代码
	CStockRangeSet aboutDlg;
	if(m_StartDate>0 && m_EndDate> m_StartDate)
	{
		aboutDlg.m_StartDate.m_dt = m_StartDate;
		aboutDlg.m_End_Date.m_dt = m_EndDate;
	}
	aboutDlg.DoModal();

	m_StartDate = aboutDlg.m_StartDate;
	m_EndDate = aboutDlg.m_End_Date;
	m_strViewType = aboutDlg.m_strViewType;

	RedrawDmGraph();
}


//	计算“动态市盈率”曲线
void CIndividualShareView::PeCurve(int nStart, int nEnd)
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CStockApp * pApp = (CStockApp *)AfxGetApp();
	ASSERT_VALID(pDoc);

	CComVariant varrX, varrY;
	varrX.parray = SafeArrayCreateVector(VT_R8, 0, nEnd - nStart + 1);
	if(varrX.parray == NULL)
		return ;
	varrX.vt = VT_ARRAY|VT_R8;

	varrY.parray = SafeArrayCreateVector(VT_R8, 0, nEnd - nStart + 1);
	if(varrY.parray == NULL)
		return ;
	varrY.vt = VT_ARRAY|VT_R8;


	CNetEaseTradeDaily tradingDaily;
	CCapitalStructOf10jqka capital;
	CCninfoIncomeStatement income;
	CBussinessUtils::GetIncomestatement(atoi(pDoc->m_nCode.c_str()), income);
	CBussinessUtils::GetCapitalStruct(atoi(pDoc->m_nCode.c_str()), capital);
	CBussinessUtils::GetSohuTradingDaily(atoi(pDoc->m_nCode.c_str()), tradingDaily);

	if(tradingDaily.m_vec_report_date.size()>0 && capital.m_vec_real_date.size()>0 && \
		income.m_vec_row.size()>0)
	{
		HRESULT hr;
		for(long i=0; i< nEnd - nStart + 1; i++)
		{
			//	生成“时间轴”
			COleDateTime odt ;
			odt.ParseDateTime(tradingDaily.m_vec_report_date[nStart + i].c_str());
			hr = SafeArrayPutElement(varrX.parray, &i, &odt.m_dt);

			//	生成
			if(income.m_vec_row.size()>0 && capital.m_vec_real_date.size() > 0)
			{
				// 寻找最近的 股本
				double nShare = -1;
				for(int k = capital.m_vec_real_date.size() - 1; k>=0; k--)
				{
					COleDateTime odt_cap ;
					odt_cap.ParseDateTime( capital.m_vec_real_date[k].c_str());
					if(odt_cap.m_dt<= odt.m_dt)
					{
						nShare = capital.m_vec_a_total_shares[k] * 10000;
						break;
					}
				}

				// 寻找最近的 净利润, 估计全年的利润, 
				double nProfit = -1;
				for(int k = income.m_vec_row.size() - 1; k>=0; k--)
				{
					COleDateTime odt_in ;
					odt_in.ParseDateTime( income.m_vec_row[k][_T("截止日期")].c_str());
					if(odt_in.m_dt <= odt.m_dt)
					{
						int mon = odt_in.GetMonth();
						int day = odt_in.GetDay();

						nProfit = String2Double(income.m_vec_row[k][_T("净利润")]);
						if(mon==3)
							nProfit = 4 * nProfit;
						else if(mon==6)
							nProfit = 2 * nProfit;
						else if(mon==9)
							nProfit = 4 * nProfit / 3;
						break;
					}
				}

				// 计算动态市盈率
				if(nProfit> 0 && nShare >0)
				{

					double nPe = nProfit/nShare;
					nPe = tradingDaily.m_vec_open[nStart + i]/nPe;
					hr = SafeArrayPutElement(varrY.parray, &i, &nPe);
				}
				else 
				{
					double d = 5.0;
					hr = SafeArrayPutElement(varrY.parray, &i, &d);
				}
			}
			else
			{
				double d = 5.0;
				hr = SafeArrayPutElement(varrY.parray, &i, &d);
			}
		}

		SetGraphData(&varrX, &varrY, "PE");
	}

}
