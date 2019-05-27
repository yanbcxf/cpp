// IndividualSzsePeView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "IndividualShareDoc.h"
#include "IndividualSzsePeView.h"


// CIndividualSzsePeView

IMPLEMENT_DYNCREATE(CIndividualSzsePeView, CIndividualDmGraphView)

CIndividualSzsePeView::CIndividualSzsePeView()
{

}

CIndividualSzsePeView::~CIndividualSzsePeView()
{
}

BEGIN_MESSAGE_MAP(CIndividualSzsePeView, CIndividualDmGraphView)
END_MESSAGE_MAP()


// CIndividualSzsePeView 绘图

void CIndividualSzsePeView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CIndividualSzsePeView 诊断

#ifdef _DEBUG
void CIndividualSzsePeView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CIndividualSzsePeView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CIndividualSzsePeView 消息处理程序

void CIndividualSzsePeView::RedrawDmGraph()
{
	// 来自“深圳证券交易所的个股每日市盈率和股价曲线”

	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CStockApp * pApp = (CStockApp *)AfxGetApp();

	ASSERT_VALID(pDoc);	

	HRESULT hr;
	map<int, CShenzhenMarketQuotation>::iterator it = pApp->m_mapShenzhenQuotation.find(atoi(pDoc->m_nCode.c_str()));
	if(it!=pApp->m_mapShenzhenQuotation.end())
	{
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

		/*for(long i=0; i< income.m_vec_report_date.size(); i++)
		{
			COleDateTime odt ;
			odt.ParseDateTime(income.m_vec_report_date[i].c_str());
			SetGraphAnnotation(odt.m_dt, 10000000, income.m_vec_report_date[i]);
		}*/

		vector<COLORREF> vec_clr;
		vector<string>   vec_annotion;
		map<int, double> map_summary;
		bool isHaveData;

		for(long i=0; i< quotation.m_vec_report_date.size(); i++)
		{
			map_summary[i] = 0.0;

			COleDateTime odt ;
			double yy = 0.0;
			odt.ParseDateTime(quotation.m_vec_report_date[i].c_str());
			hr = SafeArrayPutElement(varrX.parray, &i, &odt.m_dt);
			hr = SafeArrayPutElement(varrY.parray, &i, &yy);
		}
		SetGraphData(&varrX, &varrY, "零位线", RGB(0,0,0));
		
		//////////////////////////////////////////////////////////////////////////
		// 收盘价

		for(long i=0; i< quotation.m_vec_report_date.size(); i++)
		{
			hr = SafeArrayPutElement(varrY.parray, &i, &quotation.m_vec_today_close[i]);
		}
		//COLORREF clr = RGB(rand()%128 + 128, rand()%128 + 128, rand()%128 + 128);
		COLORREF clr = RGB(255, 128, 128);
		SetGraphData(&varrX, &varrY, "今日收盘价", clr);
		vec_clr.push_back( clr);
		vec_annotion.push_back( "今日收盘价");

		//////////////////////////////////////////////////////////////////////////
		//	市盈率

		for(long i=0; i< quotation.m_vec_report_date.size(); i++)
		{
			hr = SafeArrayPutElement(varrY.parray, &i, &quotation.m_vec_price_earnings_ratio[i]);
		}
		//COLORREF clr = RGB(rand()%128 + 128, rand()%128 + 128, rand()%128 + 128);
		clr = RGB(128, 128, 255);
		SetGraphData(&varrX, &varrY, "市盈率", clr);
		vec_clr.push_back( clr);
		vec_annotion.push_back( "市盈率");

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
}