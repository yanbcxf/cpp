// IndividualNetProfitsView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "IndividualShareDoc.h"
#include "IndividualNetProfitsView.h"


// CIndividualNetProfitsView

IMPLEMENT_DYNCREATE(CIndividualNetProfitsView, CIndividualDmGraphView)

CIndividualNetProfitsView::CIndividualNetProfitsView()
{

}

CIndividualNetProfitsView::~CIndividualNetProfitsView()
{
}

BEGIN_MESSAGE_MAP(CIndividualNetProfitsView, CIndividualDmGraphView)
END_MESSAGE_MAP()


// CIndividualNetProfitsView 绘图

void CIndividualNetProfitsView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CIndividualNetProfitsView 诊断

#ifdef _DEBUG
void CIndividualNetProfitsView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CIndividualNetProfitsView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CIndividualNetProfitsView 消息处理程序



void CIndividualNetProfitsView::RedrawDmGraph()
{
	// 来自“Yahoo 股价曲线” 和 通过结合股本结构计算的动态市盈率

	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CStockApp * pApp = (CStockApp *)AfxGetApp();

	ASSERT_VALID(pDoc);	

	HRESULT hr;
	vector<string>   vec_annotion;
	vector<COLORREF> vec_clr;
	vector<double>	 vec_percent;
	map<int, double> map_summary_in;
	map<int, double> map_summary_out;
	bool isHaveData;

	CCninfoIncomeStatement balance;
	CBussinessUtils::GetIncomestatement(atoi(pDoc->m_nCode.c_str()), balance);
	if(balance.m_vec_row.size()>0)
	{
		int nStart = 0, nEnd = balance.m_vec_row.size()-1;
		if(m_StartDate> 0 && m_EndDate> m_StartDate)
		{
			//	寻找“起始”的记录
			for(int i=0 ; i<balance.m_vec_row.size(); i++)
			{
				COleDateTime odt ;
				odt.ParseDateTime(balance.m_vec_row[i][_T("截止日期")].c_str());
				if(odt.m_dt >= m_StartDate)
				{
					nStart = i;
					break;
				}
			}

			//	寻找“结束”的记录
			for(int i=balance.m_vec_row.size() - 1; i>=0; i--)
			{
				COleDateTime odt ;
				odt.ParseDateTime(balance.m_vec_row[i][_T("截止日期")].c_str());
				if(odt.m_dt<= m_EndDate)
				{
					nEnd = i;
					break;
				}
			}
		}

		CComVariant varrX, varrY;
		varrX.parray = SafeArrayCreateVector(VT_R8, 0, nEnd - nStart +1);
		if(varrX.parray == NULL)
			return ;
		varrX.vt = VT_ARRAY|VT_R8;

		varrY.parray = SafeArrayCreateVector(VT_R8, 0,  nEnd - nStart +1);
		if(varrY.parray == NULL)
			return ;
		varrY.vt = VT_ARRAY|VT_R8;

		for(long i=0; i<  nEnd - nStart +1; i++)
		{
			COleDateTime odt ;
			double yy = 0.0;
			odt.ParseDateTime(balance.m_vec_row[nStart + i][_T("截止日期")].c_str());
			hr = SafeArrayPutElement(varrX.parray, &i, &odt.m_dt);
			hr = SafeArrayPutElement(varrY.parray, &i, &yy);
		}
		SetGraphData(&varrX, &varrY, "零位线", RGB(0,0,0));

		//////////////////////////////////////////////////////////////////////////
		// 营业总收入
		
		CURRENT_ASSET_CURVE(m_vec_cash,"营业总收入", RGB(255, 128, 128), 2, 1)
		
		//////////////////////////////////////////////////////////////////////////
		// +投资收益
		
		CURRENT_ASSET_CURVE(m_vec_cash,"投资收益", RGB(255, 128, 128), 1, 1)

		//////////////////////////////////////////////////////////////////////////
		// +营业外收入
		
		CURRENT_ASSET_CURVE(m_vec_cash,"营业外收入", RGB(255, 128, 128), 0, 1)

		//////////////////////////////////////////////////////////////////////////
		// 营业总成本
		
		CURRENT_ASSET_CURVE(m_vec_cash,"营业总成本", RGB(128, 255, 128), 2, 1)


		//////////////////////////////////////////////////////////////////////////
		// +营业外支出
		

		CURRENT_ASSET_CURVE(m_vec_cash,"营业外支出", RGB(128, 255, 128), 1, 1)


		//////////////////////////////////////////////////////////////////////////
		// 所得税费用
		
		CURRENT_ASSET_CURVE(m_vec_cash,"所得税费用", RGB(128, 255, 128), 0, 1)

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
