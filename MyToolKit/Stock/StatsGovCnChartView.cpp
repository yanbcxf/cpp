// StatsGovCnChartView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "ChildSplitterFrm.h"
#include "StatsGovCnChartView.h"
#include "CalcStatsGovCn.h"
#include "CalcFinanceReport.h"

#include "StatsGovMenuDlg.h"
#include "StockGroupDlg.h"

// CStatsGovCnChartView

IMPLEMENT_DYNCREATE(CStatsGovCnChartView, CBaseChartCtlView)

CStatsGovCnChartView::CStatsGovCnChartView()
{

}

CStatsGovCnChartView::~CStatsGovCnChartView()
{
}

BEGIN_MESSAGE_MAP(CStatsGovCnChartView, CBaseChartCtlView)
	ON_COMMAND_RANGE(ID_STATSGOV_HGYD_MENU, ID_STATSGOV_HGYD_MENU, &CStatsGovCnChartView::OnStatsgovHgydMenu)
	ON_COMMAND_RANGE(ID_STATSGOV_HGYD_MENU, ID_STATSGOV_HGJD_MENU, &CStatsGovCnChartView::OnStatsgovHgydMenu)
	ON_COMMAND_RANGE(ID_STATSGOV_HGYD_MENU, ID_STATSGOV_HGND_MENU, &CStatsGovCnChartView::OnStatsgovHgydMenu)
	ON_COMMAND_RANGE(ID_STATSGOV_HGYD_MENU, ID_STATSGOV_FSYD_MENU, &CStatsGovCnChartView::OnStatsgovHgydMenu)
	ON_COMMAND_RANGE(ID_STATSGOV_HGYD_MENU, ID_STATSGOV_FSJD_MENU, &CStatsGovCnChartView::OnStatsgovHgydMenu)
	ON_COMMAND_RANGE(ID_STATSGOV_HGYD_MENU, ID_STATSGOV_FSND_MENU, &CStatsGovCnChartView::OnStatsgovHgydMenu)

	ON_COMMAND_RANGE(ID_FINANCE_SALES_FROM_OPERATIONS, ID_FINANCE_SALES_FROM_OPERATIONS,  &CStatsGovCnChartView::OnFinanceSalesFromOperations)
	ON_COMMAND_RANGE(ID_FINANCE_COST_OF_OPERATIONS, ID_FINANCE_COST_OF_OPERATIONS,  &CStatsGovCnChartView::OnFinanceSalesFromOperations)
END_MESSAGE_MAP()


// CStatsGovCnChartView 绘图

void CStatsGovCnChartView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CStatsGovCnChartView 诊断

#ifdef _DEBUG
void CStatsGovCnChartView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CStatsGovCnChartView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CStatsGovCnChartView 消息处理程序




void CStatsGovCnChartView::OnInitialUpdate()
{
	CBaseMessageView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类

	CStatsGovCnDoc * pDoc = (CStatsGovCnDoc *)GetDocument();

	m_LocalDoc.m_vec_csrc_gate_code = pDoc->m_vec_csrc_gate_code;
	m_LocalDoc.m_str_db_code = pDoc->m_str_db_code;
	m_LocalDoc.m_vec_zb_code = pDoc->m_vec_zb_code;
	m_LocalDoc.m_vec_reg_code = pDoc->m_vec_reg_code;
	m_LocalDoc.m_vec_csrc_big_code = pDoc->m_vec_csrc_big_code;
	m_LocalDoc.m_nOutput = pDoc->m_nOutput;
	m_LocalDoc.m_StartDate.m_dt = 0;
	m_LocalDoc.m_End_Date.m_dt = 0;

	RedrawDmGraph();
}

void CStatsGovCnChartView::RedrawDmGraph()
{
	CStatsGovCnDoc * pDoc = (CStatsGovCnDoc *)&m_LocalDoc;

	m_LocalDoc.m_StartDate = m_StartDate;
	m_LocalDoc.m_End_Date = m_EndDate;

	if(pDoc->m_str_db_code == "hgyd" || pDoc->m_str_db_code == "hgjd" || pDoc->m_str_db_code == "hgnd" || 
		pDoc->m_str_db_code == "fsyd" || pDoc->m_str_db_code == "fsjd" || pDoc->m_str_db_code == "fsnd" )
	{
		CCalcStatsGovCn * cse =  new CCalcStatsGovCn(m_hWnd, WM_USER_LOG_1);
		cse->m_str_db_code = pDoc->m_str_db_code;
		cse->m_vec_zb_code = pDoc->m_vec_zb_code;
		cse->m_vec_reg_code = pDoc->m_vec_reg_code;
		cse->m_EndDate = pDoc->m_End_Date;
		cse->m_StartDate = pDoc->m_StartDate;
		cse->m_nOutput = 1 /*pDoc->m_nOutput*/;

		cse->ScheduleTask(&CCalcStatsGovCn::Execute1);
	}
	else
	{
		CCalcFinanceReport * cse =  new CCalcFinanceReport(m_hWnd, WM_USER_LOG_1);
		cse->m_str_db_code =  pDoc->m_str_db_code;
		cse->m_vec_zb_code = pDoc->m_vec_zb_code;
		cse->m_vec_csrc_gate_code = pDoc->m_vec_csrc_gate_code;
		cse->m_vec_csrc_big_code = pDoc->m_vec_csrc_big_code;
		cse->m_vec_reg_code = pDoc->m_vec_reg_code;
		cse->m_EndDate = pDoc->m_End_Date;
		cse->m_StartDate = pDoc->m_StartDate;

		cse->ScheduleTask(&CCalcFinanceReport::Execute1);
	}

	
}


void CStatsGovCnChartView::PostFocusChanged()
{
	((CChildSplitterFrame*)GetParentFrame())->m_wndSplitter.RefreshSplitBars();
}

void CStatsGovCnChartView::ProccessChartMenu()
{
	//	弹出菜单
	/*CMenu menu;
	menu.LoadMenu(IDR_IndividualSummary);		
	CMenu *pMenuPopup = menu.GetSubMenu(4);
	ASSERT(pMenuPopup);

	CPoint point;
	GetCursorPos(&point);
	pMenuPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON,point.x,point.y,this,NULL);
	menu.DestroyMenu();*/

	
}




void CStatsGovCnChartView::OnStatsgovHgydMenu(UINT nID)
{
	// TODO: 在此添加命令处理程序代码

	CStatsGovCnDoc * pDoc = (CStatsGovCnDoc *)&m_LocalDoc;
	
	CStatsGovMenuDlg dlg;
	if(nID==ID_STATSGOV_HGYD_MENU)		dlg.m_str_db_code = "hgyd";
	if(nID==ID_STATSGOV_HGJD_MENU)		dlg.m_str_db_code = "hgjd";
	if(nID==ID_STATSGOV_HGND_MENU)		dlg.m_str_db_code = "hgnd";
	if(nID==ID_STATSGOV_FSYD_MENU)		dlg.m_str_db_code = "fsyd";
	if(nID==ID_STATSGOV_FSJD_MENU)		dlg.m_str_db_code = "fsjd";
	if(nID==ID_STATSGOV_FSND_MENU)		dlg.m_str_db_code = "fsnd";

	if(IDOK==dlg.DoModal() && dlg.m_vec_zb_code.size() > 0)
	{
		((CStatsGovCnDoc *)pDoc)->m_str_db_code = dlg.m_str_db_code;
		((CStatsGovCnDoc *)pDoc)->m_vec_zb_code = dlg.m_vec_zb_code;
		((CStatsGovCnDoc *)pDoc)->m_vec_reg_code = dlg.m_vec_reg_code;

		RedrawDmGraph();
	}
}


void CStatsGovCnChartView::OnFinanceSalesFromOperations(UINT nID)
{
	// TODO: 在此添加命令处理程序代码

	CStatsGovCnDoc * pDoc = (CStatsGovCnDoc *)&m_LocalDoc;
	/*CStatsGovMenuDlg dlg;
	dlg.m_str_db_code = "hgyd";*/

	CStockGroupDlg dlg;
	//dlg.DoModal();

	if(IDOK==dlg.DoModal() )
	{
		pDoc->m_str_db_code =  "incomestatement";

		pDoc->m_vec_zb_code.clear();
		if(nID==ID_FINANCE_SALES_FROM_OPERATIONS)
			pDoc->m_vec_zb_code.push_back("colume_2");
		if(nID==ID_FINANCE_COST_OF_OPERATIONS)
			pDoc->m_vec_zb_code.push_back("colume_9");

		pDoc->m_vec_csrc_gate_code.clear();
		for(int i = 0 ; i<dlg.m_vecSelectedItemCsrcGate.size(); i++)
		{
			vector<string> firstMatch;
			string strPattern = string("\\([A-Za-z\\d]+\\)");
			if(Pcre2Grep(strPattern.c_str(), dlg.m_vecSelectedItemCsrcGate[i].c_str(), firstMatch )>0)
			{
				string::size_type pos1 = firstMatch[0].find(")");
				string codeStr = firstMatch[0].substr(1, pos1-1);
				pDoc->m_vec_csrc_gate_code.push_back(codeStr);
			}
		}

		pDoc->m_vec_csrc_big_code.clear();
		for(int i = 0 ; i<dlg.m_vecSelectedItemCsrcBig.size(); i++)
		{
			vector<string> firstMatch;
			string strPattern = string("\\([A-Za-z\\d]+\\)");
			if(Pcre2Grep(strPattern.c_str(), dlg.m_vecSelectedItemCsrcBig[i].c_str(), firstMatch )>0)
			{
				string::size_type pos1 = firstMatch[0].find(")");
				string codeStr = firstMatch[0].substr(1, pos1-1);
				pDoc->m_vec_csrc_big_code.push_back(codeStr);
			}
		}


		pDoc->m_vec_reg_code.clear();
		for(int i = 0 ; i<dlg.m_vecSelectedItemReg.size(); i++)
		{
			vector<string> firstMatch;
			string strPattern = string("\\([A-Za-z\\d]+\\)");
			if(Pcre2Grep(strPattern.c_str(), dlg.m_vecSelectedItemReg[i].c_str(), firstMatch )>0)
			{
				string::size_type pos1 = firstMatch[0].find(")");
				string codeStr = firstMatch[0].substr(1, pos1-1);
				pDoc->m_vec_reg_code.push_back(codeStr);
			}
		}
		RedrawDmGraph();
	}
}
