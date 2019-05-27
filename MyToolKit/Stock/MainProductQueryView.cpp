// MainProductQueryView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "IndividualShareDoc.h"
#include "MainProductQueryView.h"

#include "CalculateStockInfoEx.h"

#include "SelfSelectPlateDlg.h"

// CMainProductQueryView

IMPLEMENT_DYNCREATE(CMainProductQueryView, CBaseGridCtlView)

CMainProductQueryView::CMainProductQueryView()
{

}

CMainProductQueryView::~CMainProductQueryView()
{
}

BEGIN_MESSAGE_MAP(CMainProductQueryView, CBaseGridCtlView)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY(NM_DBLCLK, IDC_GRID, OnGridDblClick)
	ON_COMMAND(ID_TABLE_ADD_TO_SELF_SELECT, &CMainProductQueryView::OnTableAddToSelfSelect)
END_MESSAGE_MAP()


// CMainProductQueryView 绘图

void CMainProductQueryView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CMainProductQueryView 诊断

#ifdef _DEBUG
void CMainProductQueryView::AssertValid() const
{
	CBaseGridCtlView::AssertValid();
}

#ifndef _WIN32_WCE
void CMainProductQueryView::Dump(CDumpContext& dc) const
{
	CBaseGridCtlView::Dump(dc);
}
#endif
#endif //_DEBUG


// CMainProductQueryView 消息处理程序

void CMainProductQueryView::OnInitialUpdate()
{
	CBaseGridCtlView::OnInitialUpdate();
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();

	// TODO: 在此添加专用代码和/或调用基类
	
	if(pDoc->m_strInput.empty()==false)
	{
		CCalculateStockInfoEx * cse =  new CCalculateStockInfoEx(m_hWnd, WM_USER_LOG_1);
		cse->m_main_product_query = pDoc->m_strInput;
		cse->ScheduleTask(&CCalculateStockInfoEx::MainProductQueryList1);
	}
}



void CMainProductQueryView::ProcessXmlWParam(string msgType,string code,string model,\
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


void CMainProductQueryView::RedrawGridCtrl(CCalculateTask * cse)
{
	CCalculateStockInfoEx * pStockInfoEx = (CCalculateStockInfoEx *)cse;
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();

	if(m_pGridCtrl)
	{
		CStockApp * pApp = (CStockApp *)AfxGetApp();


		try {
			m_pGridCtrl->SetRowCount(pStockInfoEx->m_vec_code.size()+1);
			m_pGridCtrl->SetColumnCount(6);
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
					else if(col==3) val = "报告日期";
					else if(col==4) val = "主营产品";
					else if(col==5) val = "产品收入（元）";
					Item.strText.Format(_T("%s"), val.c_str());
				} 
				else 
				{
					if(col==0)
						Item.nFormat = DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;
					else
						Item.nFormat = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;

					/*if(col==3)
					{
						m_pGridCtrl->SetModified(TRUE, row, col);
						if (!m_pGridCtrl->SetCellType(row, col, RUNTIME_CLASS(CGridCellCheck)))
							return;
					}*/

					if( col==5 )
					{
						if (!m_pGridCtrl->SetCellType(row, col, RUNTIME_CLASS(CGridCellNumeric)))
							return;
					}

					string val ;
					if(col==0)		val = Int2String(row);
					else if(col==1)	val = Int2String(pStockInfoEx->m_vec_code[row-1], "%06d");
					else if(col==2)	val = pStockInfoEx->m_vec_abbreviation[row-1];
					else if(col==3) val = pStockInfoEx->m_vec_report_date[row-1];
					else if(col==4) val = pStockInfoEx->m_vec_composition_name[row-1];
					else if(col==5) val = String2Currency(Double2String(pStockInfoEx->m_vec_income[row-1]));
					Item.strText.Format(_T("%s"), val.c_str());

					if(col==9 )
					{
						Item.mask    |= (GVIF_FGCLR);
						if(pStockInfoEx->m_vec_StockSummary[row-1].change_rate>0)
							Item.crFgClr = RGB(255, 64, 64);
						else
							Item.crFgClr = RGB(0, 163, 0);
					}


				}
				m_pGridCtrl->SetItem(&Item);
			}
			int k = 1;
		}

		m_pGridCtrl->AutoSize();
		m_pGridCtrl->SetColumnWidth(2, 70);
	}
}


void CMainProductQueryView::OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
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

void CMainProductQueryView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: 在此处添加消息处理程序代码
	if (pWnd->GetSafeHwnd() == m_pGridCtrl->GetSafeHwnd())
	{
		m_pGridCtrl->ScreenToClient(&point);
		CCellID cell = m_pGridCtrl->GetCellFromPt(point);
		CString str;
		str.Format(_T("Context menu called on row %d, col %d\n"), cell.row, cell.col);
		//AfxMessageBox(str);

	}
}
void CMainProductQueryView::OnTableAddToSelfSelect()
{
	// TODO: 在此添加命令处理程序代码
	TableAddToSelfSelect();
}
