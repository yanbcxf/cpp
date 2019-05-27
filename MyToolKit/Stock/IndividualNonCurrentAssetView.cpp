// IndividualNonCurrentAssetView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "IndividualShareDoc.h"
#include "IndividualNonCurrentAssetView.h"


// CIndividualNonCurrentAssetView

IMPLEMENT_DYNCREATE(CIndividualNonCurrentAssetView, CIndividualDmGraphView)

CIndividualNonCurrentAssetView::CIndividualNonCurrentAssetView()
{

}

CIndividualNonCurrentAssetView::~CIndividualNonCurrentAssetView()
{
}

BEGIN_MESSAGE_MAP(CIndividualNonCurrentAssetView, CIndividualDmGraphView)
END_MESSAGE_MAP()


// CIndividualNonCurrentAssetView 绘图

void CIndividualNonCurrentAssetView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CIndividualNonCurrentAssetView 诊断

#ifdef _DEBUG
void CIndividualNonCurrentAssetView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CIndividualNonCurrentAssetView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CIndividualNonCurrentAssetView 消息处理程序


void CIndividualNonCurrentAssetView::RedrawDmGraph()
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CStockApp * pApp = (CStockApp *)AfxGetApp();

	ASSERT_VALID(pDoc);	

	HRESULT hr;
	vector<string>   vec_annotion;
	vector<COLORREF> vec_clr;
	vector<double>	 vec_percent;
	map<int, double> map_summary_in;
	bool isHaveData;

	CCninfoBalance balance ;
	CBussinessUtils::GetBalance(atoi(pDoc->m_nCode.c_str()), balance);
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
		// 
		
		CURRENT_ASSET_CURVE(m_vec_loans_and_payments_on_behalf,"发放贷款及垫款", RGB(255, 128, 128), 2, 1)
		CURRENT_ASSET_CURVE(m_vec_available_for_sale_financial_assets,"可供出售金融资产", RGB(255, 128, 128), 2, 1)
		CURRENT_ASSET_CURVE(m_vec_held_to_maturity_investments,"持有至到期投资", RGB(255, 128, 128), 2, 1)
		CURRENT_ASSET_CURVE(m_vec_long_term_receivables,"长期应收款", RGB(255, 128, 128), 2, 1)
		CURRENT_ASSET_CURVE(m_vec_long_term_equity_investments,"长期股权投资", RGB(255, 128, 128), 2, 1)
		CURRENT_ASSET_CURVE(m_vec_other_long_term_investments,"其他长期投资", RGB(255, 128, 128), 2, 1)
		CURRENT_ASSET_CURVE(m_vec_investment_real_estates,"投资性房地产", RGB(255, 128, 128), 2, 1)
		//NON_CURRENT_ASSET_CURVE(m_vec_original_value_of_fixed_assets,"固定资产原值", RGB(255, 128, 128), 2, 1)
		//NON_CURRENT_ASSET_CURVE(m_vec_accumulated_depreciation,"累计折旧", RGB(255, 128, 128), 2, 1)
		//NON_CURRENT_ASSET_CURVE(m_vec_fixed_assets_net_value,"固定资产净值", RGB(255, 128, 128), 2, 1)
		//NON_CURRENT_ASSET_CURVE(m_vec_fixed_assets_impairment_provision,"固定资产减值准备", RGB(255, 128, 128), 2, 1)
		CURRENT_ASSET_CURVE(m_vec_fixed_assets_net_book_value,"固定资产净额", RGB(255, 128, 128), 2, 1)
		CURRENT_ASSET_CURVE(m_vec_construction_in_progress,"在建工程", RGB(255, 128, 128), 2, 1)
		CURRENT_ASSET_CURVE(m_vec_construction_supplies,"工程物资", RGB(255, 128, 128), 2, 1)
		CURRENT_ASSET_CURVE(m_vec_fixed_assets_pending_disposal,"固定资产清理", RGB(255, 128, 128), 2, 1)
		CURRENT_ASSET_CURVE(m_vec_bearer_biological_assets,"生产性生物资产", RGB(255, 128, 128), 2, 1)
		CURRENT_ASSET_CURVE(m_vec_public_welfare_biological_assets,"公益性生物资产", RGB(255, 128, 128), 2, 1)
		CURRENT_ASSET_CURVE(m_vec_oil_and_natural_gas_assets,"油气资产", RGB(255, 128, 128), 2, 1)
		CURRENT_ASSET_CURVE(m_vec_intangibel_assets,"无形资产", RGB(255, 128, 128), 2, 1)
		CURRENT_ASSET_CURVE(m_vec_research_and_development_costs,"开发支出", RGB(255, 128, 128), 2, 1)
		CURRENT_ASSET_CURVE(m_vec_goodwill,"商誉", RGB(255, 128, 128), 2, 1)
		CURRENT_ASSET_CURVE(m_vec_long_term_deferred_expenses,"长期待摊费用", RGB(255, 128, 128), 2, 1)
		CURRENT_ASSET_CURVE(m_vec_circulation_right_for_equity_separation,"股权分置流通权", RGB(255, 128, 128), 2, 1)
		CURRENT_ASSET_CURVE(m_vec_deferred_tax_assets,"递延所得税资产", RGB(255, 128, 128), 2, 1)
		CURRENT_ASSET_CURVE(m_vec_other_non_current_assets,"其他非流动资产", RGB(255, 128, 128), 2, 1)




		//////////////////////////////////////////////////////////////////////////
		// 打上曲线的注释

		m_DMGraph.AutoRange();
		double xmin,xmax,ymin,ymax;
		m_DMGraph.GetAutoRange(&xmin, &xmax, &ymin, &ymax);
		double ystep = (ymax - ymin) /25;
		for(int k = 0; k<vec_annotion.size(); k++)
		{
			double sum = map_summary_in[nEnd];
			double percent = (vec_percent[k]/map_summary_in[nEnd]) * 100;
			char tmp[256];
			sprintf_s(tmp, 256, "  %.2f％", percent);
			string annotation = vec_annotion[k] + string(tmp); 
			SetGraphAnnotation(xmin, ymax - ystep*(vec_annotion.size() - k), annotation, vec_clr[k]);
		}
		m_DMGraph.SetRange(xmin, xmax , ymin, ymax + (ymax - ymin) * 0.02);
	}
}