// StockHomePageView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "StockHomePageView.h"


// CStockHomePageView

IMPLEMENT_DYNCREATE(CStockHomePageView, CBaseGridCtlView)

CStockHomePageView::CStockHomePageView()
{

}

CStockHomePageView::~CStockHomePageView()
{
}

BEGIN_MESSAGE_MAP(CStockHomePageView, CBaseGridCtlView)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CStockHomePageView 绘图

void CStockHomePageView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CStockHomePageView 诊断

#ifdef _DEBUG
void CStockHomePageView::AssertValid() const
{
	CBaseGridCtlView::AssertValid();
}

#ifndef _WIN32_WCE
void CStockHomePageView::Dump(CDumpContext& dc) const
{
	CBaseGridCtlView::Dump(dc);
}
#endif
#endif //_DEBUG


// CStockHomePageView 消息处理程序


void CStockHomePageView::OnInitialUpdate()
{
	CBaseGridCtlView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类

	RedrawGridCtrl();
}




void CStockHomePageView::RedrawGridCtrl()
{

	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();

	if(m_pGridCtrl)
	{
		CStockApp * pApp = (CStockApp *)AfxGetApp();

		//////////////////////////////////////////////////////////////////////////
		//	

		map<string, string>  mapUrl;
		
		mapUrl["同花顺"] = "http://www.10jqka.com.cn/";
		mapUrl["和讯网"] = "http://www.hexun.com/";
		mapUrl["中财网"] = "http://data.cfi.cn/cfidata.aspx";
		mapUrl["东方财富网"] = "http://data.eastmoney.com/cjsj/hbgyl.html";

		mapUrl["中国指数有限公司"] = "http://www.csindex.com.cn/sseportal/csiportal/hy_syl/syl.jsp";
		mapUrl["中国证券投资者保护基金"] = "http://www.sipf.com.cn/zjjk/tjsj/";
		mapUrl["中国证券登记结算有限责任公司"] = "http://www.chinaclear.cn/";

		mapUrl["上海证券交易所"] = "http://www.sse.com.cn/";
		mapUrl["深圳证券交易所"] = "http://www.szse.cn/";
		mapUrl["中国金融期货交易所"] = "http://www.cffex.com.cn/";
		mapUrl["中国国家统计局"] = "http://www.stats.gov.cn/tjsj/tjbz/201301/t20130114_8675.html";

		mapUrl["中央国债登记结算有限责任公司"] = "http://www.chinabond.com.cn/d2s/index.html";
		mapUrl["巨潮资讯"] = "http://www.cninfo.com.cn/cninfo-new/index";


		m_pGridCtrl->SetRowCount(mapUrl.size()+1 + 20);
		m_pGridCtrl->SetColumnCount(3);
		m_pGridCtrl->SetFixedRowCount(1);
		m_pGridCtrl->SetFixedColumnCount(1);
		m_pGridCtrl->SetHeaderSort(TRUE);

		SetGridCell(0, 0, true, "序号");
		SetGridCell(0, 1, true, "公司");
		SetGridCell(0, 2, true, "链接（URL）");

		map<string, string>::iterator it = mapUrl.begin();
		for(int i=1; it!= mapUrl.end(); i++, it++)
		{
			char tmp[5] = {0};
			sprintf_s(tmp, 5, "%d", i);
			SetGridCell(i,0, true, tmp);

			SetGridCell(i,1, false, it->first.c_str());
			
			if (!m_pGridCtrl->SetCellType(i, 2, RUNTIME_CLASS(CGridURLCell)))
				return;
			SetGridCell(i, 2, false, it->second.c_str());
		}
				
		m_pGridCtrl->AutoSize();
		m_pGridCtrl->SetColumnWidth(1, 230);
		m_pGridCtrl->SetColumnWidth(2, 560);
	}

}

void CStockHomePageView::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	int kkk = 1;
	//CBaseGridCtlView::OnClose();
}
