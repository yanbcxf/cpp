// AllRecentBonusCashStockListView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "MainFrm.h"
#include "AllRecentBonusCashStockListView.h"
#include "CalcRecentBonusCashSummary.h"

// CAllRecentBonusCashStockListView

IMPLEMENT_DYNCREATE(CAllRecentBonusCashStockListView, CBaseGridCtlView)

CAllRecentBonusCashStockListView::CAllRecentBonusCashStockListView()
{

}

CAllRecentBonusCashStockListView::~CAllRecentBonusCashStockListView()
{
}

BEGIN_MESSAGE_MAP(CAllRecentBonusCashStockListView, CBaseGridCtlView)
	ON_NOTIFY(NM_DBLCLK, IDC_GRID, OnGridDblClick)
	ON_COMMAND(ID_TABLE_ADD_TO_SELF_SELECT, &CAllRecentBonusCashStockListView::OnTableAddToSelfSelect)
END_MESSAGE_MAP()


// CAllRecentBonusCashStockListView 绘图

void CAllRecentBonusCashStockListView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CAllRecentBonusCashStockListView 诊断

#ifdef _DEBUG
void CAllRecentBonusCashStockListView::AssertValid() const
{
	CBaseGridCtlView::AssertValid();
}

#ifndef _WIN32_WCE
void CAllRecentBonusCashStockListView::Dump(CDumpContext& dc) const
{
	CBaseGridCtlView::Dump(dc);
}
#endif
#endif //_DEBUG


// CAllRecentBonusCashStockListView 消息处理程序




void CAllRecentBonusCashStockListView::OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	//Trace(_T("Double Clicked on row %d, col %d\n"), pItem->iRow, pItem->iColumn);

	CString str;
	str.Format(_T("Context menu called on row %d, col %d\n"), pItem->iRow, pItem->iColumn);
	//AfxMessageBox(str);
	//////////////////////////////////////////////////////////////////////////
	//	股票停靠栏显示当前选择的 股票

	if(pItem->iRow>0)
	{
		CString str = m_pGridCtrl->GetItemText(pItem->iRow, 1);
		m_MessageCtrl.FocusStockOnLeftPane(str.GetBuffer());
	}
}
void CAllRecentBonusCashStockListView::OnInitialUpdate()
{
	CBaseGridCtlView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();

	Startup();	
}



void CAllRecentBonusCashStockListView::Startup()
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CCalcRecentBonusCashSummary * cse =  new CCalcRecentBonusCashSummary(m_hWnd, WM_USER_LOG_1);

	m_MessageCtrl.SetStockRangeForCalculateTask(pDoc, cse);

	cse->ScheduleTask(&CCalcRecentBonusCashSummary::Execute1);
	m_bCalculating = true;
}


void CAllRecentBonusCashStockListView::ProcessXmlWParam(string msgType,string code,string model,\
															 string param1,string param2,string param3)
{
	if(msgType=="TaskFinished")
	{
		stringstream ss;
		ss << "任务【" << (param1) << "】 计算完成. ";

		CCalculateTask * cse = CCalculateTask::FetchTask(atol(param1.c_str()));
		RedrawGridCtrl(cse);
		if(cse!=NULL)
			delete cse;
		sendToOutput(ss.str().c_str(), NULL, WM_USER_LOG_1);
		m_bCalculating = false;
	}
	//RedrawGridCtrl();
}


void CAllRecentBonusCashStockListView::RedrawGridCtrl(CCalculateTask * cse)
{

	CCalcRecentBonusCashSummary * pStockInfoEx = (CCalcRecentBonusCashSummary *)cse;
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();

	if(m_pGridCtrl)
	{
		CStockApp * pApp = (CStockApp *)AfxGetApp();

		try {
			m_pGridCtrl->SetRowCount(pStockInfoEx->m_vec_code.size()+1);
			m_pGridCtrl->SetColumnCount(10 + 1);
			m_pGridCtrl->SetFixedRowCount(1);
			m_pGridCtrl->SetFixedColumnCount(3);
			m_pGridCtrl->SetHeaderSort(TRUE);
		}
		catch (CMemoryException* e)
		{
			e->ReportError();
			e->Delete();
			return;
		}

		for (int row = 0; row < m_pGridCtrl->GetRowCount(); row++)
		{
			for (int col = 0; col < m_pGridCtrl->GetColumnCount(); col++)
			{ 
				GV_ITEM Item;
				Item.mask = GVIF_TEXT|GVIF_FORMAT;
				Item.row = row;
				Item.col = col;
				if (row < 1) {
					Item.nFormat = DT_LEFT|DT_WORDBREAK;
					string val;
					if(col==0)		val = "序号";
					else if(col==1)	val = "代码";
					else if(col==2)	val = "名称";
					else if(col==3) val = "分红年度";
					else if(col==4) val = "送股(股)";
					else if(col==5) val = "转增股（股）";
					else if(col==6) val = "现金分红（元）";
					else if(col==7) val = "最近股价（元）";
					else if(col==8) val = "收益比例(%)";
					else if(col==9) val = "股权登记日";
					else if(col==10) val = "除权除息日";
					
					Item.strText.Format(_T("%s"), val.c_str());
				} 
				else 
				{
					if(col==0)
						Item.nFormat = DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;
					else
						Item.nFormat = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;

					if( col==0 )
					{
						if (!m_pGridCtrl->SetCellType(row, col, RUNTIME_CLASS(CGridCellNumeric)))
							return;
					}

					string val ;
					if(col==0)		val = Int2String(row);
					else if(col==1)	val = Int2String(pStockInfoEx->m_vec_code[row-1], "%06d");
					else if(col==2)	val = pStockInfoEx->m_vec_abbreviation[row-1];
					else if(col==3) val = pStockInfoEx->m_vec_bonus_year[row-1];
					else if(col==4) val = Double2String(pStockInfoEx->m_vec_bonus_share[row-1]);
					else if(col==5) val = Double2String(pStockInfoEx->m_vec_reserve_to_common_share[row-1]);
					else if(col==6) val = Double2String(pStockInfoEx->m_vec_bonus_cash[row-1]);
					else if(col==7) val = Double2String(pStockInfoEx->m_vec_recent_close[row-1]);
					else if(col==8)
					{
						val = Double2String(pStockInfoEx->m_vec_earning_percent[row-1]*100);
						Item.mask    |= (GVIF_FGCLR);
						Item.crFgClr = RGB(255, 64, 64);
					}
					else if(col==9) val = pStockInfoEx->m_vec_right_record_date[row-1];
					else if(col==10) val = pStockInfoEx->m_vec_exit_right_date[row-1];

					
					Item.strText.Format(_T("%s"), val.c_str());

				}
				m_pGridCtrl->SetItem(&Item);
			}
			int k = 1;
		}

		m_pGridCtrl->AutoSize();
		m_pGridCtrl->SetColumnWidth(2, 80);
	}

}



void CAllRecentBonusCashStockListView::OnTableAddToSelfSelect()
{
	// TODO: 在此添加命令处理程序代码
	TableAddToSelfSelect();
}
