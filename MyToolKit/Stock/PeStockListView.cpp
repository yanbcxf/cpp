// PeStockListView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"

#include "PeStockListView.h"
#include "CalcPeStockList.h"
#include "CalcPeGrossStockList.h"

// CPeStockListView

IMPLEMENT_DYNCREATE(CPeStockListView, CBaseGridCtlView)

CPeStockListView::CPeStockListView()
{

}

CPeStockListView::~CPeStockListView()
{
}

BEGIN_MESSAGE_MAP(CPeStockListView, CBaseGridCtlView)
	ON_NOTIFY(NM_DBLCLK, IDC_GRID, OnGridDblClick)
	ON_COMMAND(ID_TABLE_ADD_TO_SELF_SELECT, &CPeStockListView::OnTableAddToSelfSelect)
END_MESSAGE_MAP()


// CPeStockListView 绘图

void CPeStockListView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CPeStockListView 诊断

#ifdef _DEBUG
void CPeStockListView::AssertValid() const
{
	CBaseGridCtlView::AssertValid();
}

#ifndef _WIN32_WCE
void CPeStockListView::Dump(CDumpContext& dc) const
{
	CBaseGridCtlView::Dump(dc);
}
#endif
#endif //_DEBUG


// CPeStockListView 消息处理程序


void CPeStockListView::OnInitialUpdate()
{
	CBaseGridCtlView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	if(pDoc->m_strInput=="pe_gross")
	{
		CCalcPeGrossStockList * cse =  new CCalcPeGrossStockList(m_hWnd, WM_USER_LOG_1);

		CStockApp * pApp = (CStockApp *)AfxGetApp();
		m_MessageCtrl.SetStockRangeForCalculateTask(pDoc, cse);
		cse->ScheduleTask(&CCalcPeGrossStockList::Execute1);
	}
	else
	{
		CCalcPeStockList * cse =  new CCalcPeStockList(m_hWnd, WM_USER_LOG_1);

		CStockApp * pApp = (CStockApp *)AfxGetApp();
		m_MessageCtrl.SetStockRangeForCalculateTask(pDoc, cse);
		cse->ScheduleTask(&CCalcPeStockList::Execute1);
	}
	
}



void CPeStockListView::ProcessXmlWParam(string msgType,string code,string model,\
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
	}
}


void CPeStockListView::RedrawGridCtrl(CCalculateTask * cse)
{
	CCalcPeStockList * pStockInfoEx = (CCalcPeStockList *)cse;
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
					else if(col==3) val = "最低 PE 日期";
					else if(col==4)	val = "最低 PE";
					else if(col==5) val = "最近 PE 日期";
					else if(col==6) val = "最近 PE";
					else if(col==7) val = "PE 差值";
					
					Item.strText.Format(_T("%s"), val.c_str());
				} 
				else 
				{
					if(col==0)
						Item.nFormat = DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;
					else
						Item.nFormat = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;

					if( col==4 ||col==6 || col==7 )
					{
						if (!m_pGridCtrl->SetCellType(row, col, RUNTIME_CLASS(CGridCellNumeric)))
							return;
					}

					if(col==3 || col ==5 )
					{
						if (!m_pGridCtrl->SetCellType(row, col, RUNTIME_CLASS(CGridCellDateTime)))
							return;
					}

					string val ;
					if(col==0)		val = Int2String(row);
					else if(col==1)	val = Int2String(pStockInfoEx->m_vec_code[row-1], "%06d");
					else if(col==2) val = pStockInfoEx->m_vec_abbreviation[row-1];
					else if(col==3)	val =  pStockInfoEx->m_vec_min_date[row-1];

					else if(col==4)	val =  Double2String(pStockInfoEx->m_vec_min_pe[row-1]);
					else if(col==5)	val =  pStockInfoEx->m_vec_recent_date[row-1];
					else if(col==6)	val =  Double2String(pStockInfoEx->m_vec_recent_pe[row-1]);
					else if(col==7)	val =  Double2String(pStockInfoEx->m_vec_defferent[row-1]);

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



void CPeStockListView::OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
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

void CPeStockListView::OnTableAddToSelfSelect()
{
	// TODO: 在此添加命令处理程序代码
	TableAddToSelfSelect();
}
