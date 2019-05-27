// AllMainProfitYearGrowthStockListView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "MainFrm.h"
#include "AllMainProfitYearGrowthStockListView.h"

#include "CalcMainProfitYearGrowth.h"
#include "SelfSelectPlateDlg.h"

// CAllMainProfitYearGrowthStockListView

IMPLEMENT_DYNCREATE(CAllMainProfitYearGrowthStockListView, CBaseGridCtlView)

CAllMainProfitYearGrowthStockListView::CAllMainProfitYearGrowthStockListView()
{

}

CAllMainProfitYearGrowthStockListView::~CAllMainProfitYearGrowthStockListView()
{
}

BEGIN_MESSAGE_MAP(CAllMainProfitYearGrowthStockListView, CBaseGridCtlView)
	ON_NOTIFY(NM_DBLCLK, IDC_GRID, OnGridDblClick)
	ON_COMMAND(ID_TABLE_ADD_TO_SELF_SELECT, &CAllMainProfitYearGrowthStockListView::OnTableAddToSelfSelect)
END_MESSAGE_MAP()


// CAllMainProfitYearGrowthStockListView 绘图

void CAllMainProfitYearGrowthStockListView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CAllMainProfitYearGrowthStockListView 诊断

#ifdef _DEBUG
void CAllMainProfitYearGrowthStockListView::AssertValid() const
{
	CBaseGridCtlView::AssertValid();
}

#ifndef _WIN32_WCE
void CAllMainProfitYearGrowthStockListView::Dump(CDumpContext& dc) const
{
	CBaseGridCtlView::Dump(dc);
}
#endif
#endif //_DEBUG


// CAllMainProfitYearGrowthStockListView 消息处理程序




void CAllMainProfitYearGrowthStockListView::OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
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
void CAllMainProfitYearGrowthStockListView::OnInitialUpdate()
{
	CBaseGridCtlView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();

	Startup();	
}



void CAllMainProfitYearGrowthStockListView::Startup()
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CCalcMainProfitYearGrowth * cse =  new CCalcMainProfitYearGrowth(m_hWnd, WM_USER_LOG_1);

	m_MessageCtrl.SetStockRangeForCalculateTask(pDoc, cse);

	cse->ScheduleTask(&CCalcMainProfitYearGrowth::Execute1);
	m_bCalculating = true;
}


void CAllMainProfitYearGrowthStockListView::ProcessXmlWParam(string msgType,string code,string model,\
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


void CAllMainProfitYearGrowthStockListView::RedrawGridCtrl(CCalculateTask * cse)
{

	CCalcMainProfitYearGrowth * pStockInfoEx = (CCalcMainProfitYearGrowth *)cse;
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();

	if(m_pGridCtrl)
	{
		CStockApp * pApp = (CStockApp *)AfxGetApp();

		try {
			m_pGridCtrl->SetRowCount(pStockInfoEx->m_vec_year_growth_data.size()+1);
			m_pGridCtrl->SetColumnCount(3 + pStockInfoEx->m_max_year_index + 1);
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

					for(int n = 0; n<= pStockInfoEx->m_max_year_index; n++)
					{
						if(col==n+3) 
						{
							int nYear = 2012 + n;
							stringstream ss;
							ss << nYear << "年度增长率(%)";
							val = ss.str();
						}
					}
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
					else if(col==1)	val = Int2String(pStockInfoEx->m_vec_year_growth_data[row-1].m_code, "%06d");
					else if(col==2)	val = pStockInfoEx->m_vec_year_growth_data[row-1].m_abbreviation;

					for(int n = 0; n<= pStockInfoEx->m_max_year_index; n++)
					{
						if(col==n+3) 
						{
							if (!m_pGridCtrl->SetCellType(row, col, RUNTIME_CLASS(CGridCellNumeric)))
								return;

							val = Double2String(pStockInfoEx->m_vec_year_growth_data[row -1].m_growth_percent_from_2012[n] *100);


							Item.mask    |= (GVIF_FGCLR);
							if(pStockInfoEx->m_vec_year_growth_data[row -1].m_growth_percent_from_2012[n]>0)
								Item.crFgClr = RGB(255, 64, 64);
							else
								Item.crFgClr = RGB(0, 163, 0);
						}
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



void CAllMainProfitYearGrowthStockListView::OnTableAddToSelfSelect()
{
	// TODO: 在此添加命令处理程序代码
	TableAddToSelfSelect();
}
