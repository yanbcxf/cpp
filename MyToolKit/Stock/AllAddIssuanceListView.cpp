// AllAddIssuanceListView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "AllAddIssuanceListView.h"
#include "CalcAddIssuanceSummary.h"


// CAllAddIssuanceListView

IMPLEMENT_DYNCREATE(CAllAddIssuanceListView, CBaseGridCtlView)

CAllAddIssuanceListView::CAllAddIssuanceListView()
{

}

CAllAddIssuanceListView::~CAllAddIssuanceListView()
{
}

BEGIN_MESSAGE_MAP(CAllAddIssuanceListView, CBaseGridCtlView)
	ON_NOTIFY(NM_DBLCLK, IDC_GRID, OnGridDblClick)
	ON_COMMAND(ID_TABLE_ADD_TO_SELF_SELECT, &CAllAddIssuanceListView::OnTableAddToSelfSelect)
END_MESSAGE_MAP()


// CAllAddIssuanceListView 绘图

void CAllAddIssuanceListView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CAllAddIssuanceListView 诊断

#ifdef _DEBUG
void CAllAddIssuanceListView::AssertValid() const
{
	CBaseGridCtlView::AssertValid();
}

#ifndef _WIN32_WCE
void CAllAddIssuanceListView::Dump(CDumpContext& dc) const
{
	CBaseGridCtlView::Dump(dc);
}
#endif
#endif //_DEBUG


// CAllAddIssuanceListView 消息处理程序


void CAllAddIssuanceListView::OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
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
void CAllAddIssuanceListView::OnInitialUpdate()
{
	CBaseGridCtlView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();

	Startup();	
}



void CAllAddIssuanceListView::Startup()
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CCalcAddIssuanceSummary * cse =  new CCalcAddIssuanceSummary(m_hWnd, WM_USER_LOG_1);

	m_MessageCtrl.SetStockRangeForCalculateTask(pDoc, cse);

	cse->ScheduleTask(&CCalcAddIssuanceSummary::Execute1);
	m_bCalculating = true;
}


void CAllAddIssuanceListView::ProcessXmlWParam(string msgType,string code,string model,\
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


void CAllAddIssuanceListView::RedrawGridCtrl(CCalculateTask * cse)
{

	CCalcAddIssuanceSummary * pStockInfoEx = (CCalcAddIssuanceSummary *)cse;
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();

	if(m_pGridCtrl)
	{
		CStockApp * pApp = (CStockApp *)AfxGetApp();

		try {
			m_pGridCtrl->SetRowCount(pStockInfoEx->m_vec_code.size()+1);
			m_pGridCtrl->SetColumnCount(8);
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
					else if(col==2) val = "股票简称";
					else if(col==3) val = "增发上市日";
					else if(col==4) val = "增发价（元）";
					else if(col==5) val = "上市日收盘价（元）";
					else if(col==6) val = "历次分红后增发价（元）";
					else if(col==7) val = "近期收盘价（元）";
					else if(col==8) val = "涨跌幅（%）";

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

					if( col>=4 )
					{
						if (!m_pGridCtrl->SetCellType(row, col, RUNTIME_CLASS(CGridCellNumeric)))
							return;
					}



					string val ;
					if(col==0)		val = Int2String(row);
					else if(col==1)	val = Int2String(pStockInfoEx->m_vec_code[row-1], "%06d");
					else if(col==2) val = pStockInfoEx->m_vec_abbreviation[row-1];
					else if(col==3)	val =  pStockInfoEx->m_vec_market_date[row-1];
					else if(col==4)	val =  Double2String(pStockInfoEx->m_vec_placement_price[row-1]);
					else if(col==5)	val =  Double2String(pStockInfoEx->m_vec_market_close[row-1]);

					else if(col==6)
					{
						Item.mask    |= (GVIF_FGCLR);
						val = Double2String(pStockInfoEx->m_vec_add_issuance_price[row-1]);

						if(pStockInfoEx->m_vec_add_issuance_price[row-1] >0)
							Item.crFgClr = RGB(255, 64, 64);
						else
							Item.crFgClr = RGB(0, 163, 0);
					}

					else if(col==7) val =  Double2String(pStockInfoEx->m_vec_recent_close[row-1]);
					else if(col==8)
					{
						Item.mask    |= (GVIF_FGCLR);
						val = Double2String(pStockInfoEx->m_vec_change_rate[row-1]);

						if(pStockInfoEx->m_vec_change_rate[row-1] >0)
							Item.crFgClr = RGB(255, 64, 64);
						else
							Item.crFgClr = RGB(0, 163, 0);
					}

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



void CAllAddIssuanceListView::OnTableAddToSelfSelect()
{
	// TODO: 在此添加命令处理程序代码
	TableAddToSelfSelect();
}
