// IndividualVolumeView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "IndividualShareDoc.h"
#include "IndividualVolumeView.h"


// CIndividualVolumeView

IMPLEMENT_DYNCREATE(CIndividualVolumeView, CIndividualDmGraphView)

CIndividualVolumeView::CIndividualVolumeView()
{

}

CIndividualVolumeView::~CIndividualVolumeView()
{
}

BEGIN_MESSAGE_MAP(CIndividualVolumeView, CIndividualDmGraphView)
END_MESSAGE_MAP()


// CIndividualVolumeView 绘图

void CIndividualVolumeView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CIndividualVolumeView 诊断

#ifdef _DEBUG
void CIndividualVolumeView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CIndividualVolumeView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CIndividualVolumeView 消息处理程序


void CIndividualVolumeView::RedrawDmGraph()
{
	// 来自“深圳证券交易所的个股每日市盈率和股价曲线”

	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CStockApp * pApp = (CStockApp *)AfxGetApp();

	ASSERT_VALID(pDoc);	

	HRESULT hr;
	vector<string>   vec_annotion;
	vector<COLORREF> vec_clr;

	map<int, CShenzhenMarketQuotation>::iterator it = pApp->m_mapShenzhenQuotation.find(atoi(pDoc->m_nCode.c_str()));
	if( 0 /*it!=pApp->m_mapShenzhenQuotation.end()*/)
	{
		//	以下代码不用
		CShenzhenMarketQuotation quotation = it->second;

		CComVariant varrX, varrY;
		varrX.parray = SafeArrayCreateVector(VT_R8, 0, quotation.m_vec_report_date.size());
		if(varrX.parray == NULL)
			return ;
		varrX.vt = VT_ARRAY|VT_R8;

		varrY.parray = SafeArrayCreateVector(VT_R8, 0, quotation.m_vec_report_date.size());
		if(varrY.parray == NULL)
			return ;
		varrY.vt = VT_ARRAY|VT_R8;

		for(long i=0; i< quotation.m_vec_report_date.size(); i++)
		{
			COleDateTime odt ;
			double yy = 0.0;
			odt.ParseDateTime(quotation.m_vec_report_date[i].c_str());
			hr = SafeArrayPutElement(varrX.parray, &i, &odt.m_dt);
			hr = SafeArrayPutElement(varrY.parray, &i, &yy);
		}
		SetGraphData(&varrX, &varrY, "零位线", RGB(0,0,0));
		
		//////////////////////////////////////////////////////////////////////////
		// 今日成交量（来自Szse）

		for(long i=0; i< quotation.m_vec_report_date.size(); i++)
		{
			if(quotation.m_vec_report_date[i]=="2015-7-7")
			{
				int www = 1;
			}
			double amount = quotation.m_vec_turnover_amount[i]/10000.0;
			hr = SafeArrayPutElement(varrY.parray, &i, &amount);
		}
		//COLORREF clr = RGB(rand()%128 + 128, rand()%128 + 128, rand()%128 + 128);
		COLORREF clr = RGB(255, 128, 128);
		SetGraphData(&varrX, &varrY, "今日成交金额（万元，来自Szse）", clr);
		vec_clr.push_back( clr);
		vec_annotion.push_back( "今日成交金额（万元，来自Szse）");

	}

	CNetEaseTradeDaily trading;
	CBussinessUtils::GetSohuTradingDaily(atoi(pDoc->m_nCode.c_str()), trading);
	if(trading.m_vec_report_date.size()>0)
	{
		CComVariant varrX, varrY;
		varrX.parray = SafeArrayCreateVector(VT_R8, 0, trading.m_vec_report_date.size());
		if(varrX.parray == NULL)
			return ;
		varrX.vt = VT_ARRAY|VT_R8;

		varrY.parray = SafeArrayCreateVector(VT_R8, 0, trading.m_vec_report_date.size());
		if(varrY.parray == NULL)
			return ;
		varrY.vt = VT_ARRAY|VT_R8;

		for(long i=0; i< trading.m_vec_report_date.size(); i++)
		{
			COleDateTime odt ;
			double yy = 0.0;
			odt.ParseDateTime(trading.m_vec_report_date[i].c_str());
			hr = SafeArrayPutElement(varrX.parray, &i, &odt.m_dt);
			hr = SafeArrayPutElement(varrY.parray, &i, &yy);
		}
		SetGraphData(&varrX, &varrY, "零位线", RGB(0,0,0));

		//////////////////////////////////////////////////////////////////////////
		// 今日成交量（来自 Souhu）

		for(long i=0; i< trading.m_vec_report_date.size(); i++)
		{
			double amount = trading.m_vec_volume[i];
			hr = SafeArrayPutElement(varrY.parray, &i, &amount);
		}
		//COLORREF clr = RGB(rand()%128 + 128, rand()%128 + 128, rand()%128 + 128);
		COLORREF clr = RGB(128, 255, 128);
		SetGraphData(&varrX, &varrY, "今日成交股数（万手，来自搜狐）", clr);
		vec_clr.push_back( clr);
		vec_annotion.push_back( "今日成交量股数（万手，来自搜狐）");

		//////////////////////////////////////////////////////////////////////////
		// 今日成交量（来自 Souhu）

		for(long i=0; i< trading.m_vec_report_date.size(); i++)
		{
			if(trading.m_vec_report_date[i]=="2015-7-7")
			{
				int www = 1;
			}
			double amount = trading.m_vec_turnover[i];
			hr = SafeArrayPutElement(varrY.parray, &i, &amount);
		}
		//COLORREF clr = RGB(rand()%128 + 128, rand()%128 + 128, rand()%128 + 128);
		clr = RGB(255, 128, 128);
		SetGraphData(&varrX, &varrY, "今日成交金额（万元，来自搜狐）", clr);
		vec_clr.push_back( clr);
		vec_annotion.push_back( "今日成交金额（万元，来自搜狐）");

	}

	//////////////////////////////////////////////////////////////////////////
	// 打上曲线的注释

	m_DMGraph.AutoRange();
	double xmin,xmax,ymin,ymax;
	m_DMGraph.GetAutoRange(&xmin, &xmax, &ymin, &ymax);
	double ystep = (ymax - ymin) /25;
	for(int k = 0; k<vec_annotion.size(); k++)
	{
		SetGraphAnnotation(xmin, ymax - ystep*(vec_annotion.size() - k), vec_annotion[k], vec_clr[k]);
	}
}