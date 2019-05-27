// MacdStockListView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "IndividualShareDoc.h"
#include "MacdStockListView.h"
#include "CalcMacdListData.h"

#include "SelfSelectPlateDlg.h"


// CMacdStockListView

IMPLEMENT_DYNCREATE(CMacdStockListView, CBaseGridCtlView)

CMacdStockListView::CMacdStockListView()
{

}

CMacdStockListView::~CMacdStockListView()
{
}

BEGIN_MESSAGE_MAP(CMacdStockListView, CBaseGridCtlView)
	ON_NOTIFY(NM_DBLCLK, IDC_GRID, OnGridDblClick)
	ON_COMMAND(ID_TABLE_ADD_TO_SELF_SELECT, &CMacdStockListView::OnTableAddToSelfSelect)
END_MESSAGE_MAP()


// CMacdStockListView 绘图

void CMacdStockListView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CMacdStockListView 诊断

#ifdef _DEBUG
void CMacdStockListView::AssertValid() const
{
	CBaseGridCtlView::AssertValid();
}

#ifndef _WIN32_WCE
void CMacdStockListView::Dump(CDumpContext& dc) const
{
	CBaseGridCtlView::Dump(dc);
}
#endif
#endif //_DEBUG


// CMacdStockListView 消息处理程序



void CMacdStockListView::OnInitialUpdate()
{
	CBaseGridCtlView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CCalcMacdListData * cse =  new CCalcMacdListData(m_hWnd, WM_USER_LOG_1);

	CStockApp * pApp = (CStockApp *)AfxGetApp();


	cse->ScheduleTask(&CCalcMacdListData::Execute1);
}



void CMacdStockListView::ProcessXmlWParam(string msgType,string code,string model,\
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


void CMacdStockListView::RedrawGridCtrl(CCalculateTask * cse)
{
	CCalcMacdListData * pStockInfoEx = (CCalcMacdListData *)cse;
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();

	if(m_pGridCtrl)
	{
		CStockApp * pApp = (CStockApp *)AfxGetApp();

		try {
			m_pGridCtrl->SetRowCount(pStockInfoEx->m_vec_code.size()+1);
			m_pGridCtrl->SetColumnCount(11);
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
					else if(col==3) val = "最近交易日";
					else if(col==4) val = "前一交易日";
					else if(col==5) val = "最近交易日DIF";
					else if(col==6) val = "前一交易日DIF";
					else if(col==7) val = "最近交易日DEA";
					else if(col==8) val = "前一交易日DEA";
					else if(col==9)	val = "水上或水下";
					else if(col==10) val = "买卖信号";
					
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

					if( col==6 || col==7 || col ==9)
					{
						if (!m_pGridCtrl->SetCellType(row, col, RUNTIME_CLASS(CGridCellNumeric)))
							return;
					}

					if(col==3 || col ==4 )
					{
						if (!m_pGridCtrl->SetCellType(row, col, RUNTIME_CLASS(CGridCellDateTime)))
							return;
					}

					string val ;
					if(col==0)		val = Int2String(row);
					else if(col==1)	val = Int2String(pStockInfoEx->m_vec_code[row-1], "%06d");
					else if(col==2) val = pStockInfoEx->m_vec_abbreviation[row-1];
					else if(col==3)	val =  pStockInfoEx->m_vec_date1[row-1];
					else if(col==4)	val =  pStockInfoEx->m_vec_date2[row-1];
					else if(col==5)
					{
						Item.mask    |= (GVIF_FGCLR);
						val = Double2String(pStockInfoEx->m_vec_dif1[row-1]);

						if(pStockInfoEx->m_vec_dif1[row-1] >0)
							Item.crFgClr = RGB(255, 64, 64);
						else
							Item.crFgClr = RGB(0, 163, 0);
					}
					else if(col==6)
					{
						Item.mask    |= (GVIF_FGCLR);
						val = Double2String(pStockInfoEx->m_vec_dif2[row-1]);

						if(pStockInfoEx->m_vec_dif2[row-1] >0)
							Item.crFgClr = RGB(255, 64, 64);
						else
							Item.crFgClr = RGB(0, 163, 0);
					}
					else if(col==7)
					{
						Item.mask    |= (GVIF_FGCLR);
						val = Double2String(pStockInfoEx->m_vec_dea1[row-1]);

						if(pStockInfoEx->m_vec_dea1[row-1] >0)
							Item.crFgClr = RGB(255, 64, 64);
						else
							Item.crFgClr = RGB(0, 163, 0);
					}
					else if(col==8)
					{
						Item.mask    |= (GVIF_FGCLR);
						val = Double2String(pStockInfoEx->m_vec_dea2[row-1]);

						if(pStockInfoEx->m_vec_dea2[row-1] >0)
							Item.crFgClr = RGB(255, 64, 64);
						else
							Item.crFgClr = RGB(0, 163, 0);
					}
					else if(col==9)
					{
						Item.mask    |= (GVIF_FGCLR);
						if(pStockInfoEx->m_vec_dif1[row-1]> 0 && pStockInfoEx->m_vec_dea1[row-1] >0)
						{
							Item.crFgClr = RGB(255, 64, 64);
							val = "水上";
						}
						else if(pStockInfoEx->m_vec_dif1[row-1]< 0 && pStockInfoEx->m_vec_dea1[row-1] <0)
						{
							Item.crFgClr = RGB(0, 163, 0);
							val = "水下";
						}
						else
						{
							Item.crFgClr = RGB(163, 163, 163);
							val = "待定";
						}
					}
					else if(col==10)
					{
						Item.mask    |= (GVIF_FGCLR);
						if(pStockInfoEx->m_vec_dif1[row-1]> pStockInfoEx->m_vec_dea1[row-1] &&
							pStockInfoEx->m_vec_dif2[row-1] < pStockInfoEx->m_vec_dea2[row-1]  )
						{
							/*macd 金叉，买入*/
							Item.crFgClr = RGB(255, 64, 64);
							val = "1.MACD 金叉，买入";

						}
						else if(pStockInfoEx->m_vec_dif1[row-1] < pStockInfoEx->m_vec_dea1[row-1] &&
							pStockInfoEx->m_vec_dif2[row-1] > pStockInfoEx->m_vec_dea2[row-1]  )
						{
							/*macd 死叉，卖出*/
							Item.crFgClr = RGB(0, 163, 0);
							val = "2.MACD 死叉，卖出";
						}
						else
						{
							Item.crFgClr = RGB(163, 163, 163);
							val = "3.待定";
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



void CMacdStockListView::OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
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

void CMacdStockListView::OnTableAddToSelfSelect()
{
	// TODO: 在此添加命令处理程序代码
	int selected = m_pGridCtrl->GetSelectedCount();
	if(selected>0)
	{
		map<int ,string >  codes;
		for (int row = 0; row < m_pGridCtrl->GetRowCount(); row++)
		{
			for (int col = 0; col < m_pGridCtrl->GetColumnCount(); col++)
			{
				if(m_pGridCtrl->IsCellSelected(row, col))
				{
					CString str = m_pGridCtrl->GetItemText(row, 1);
					int code = atoi(str.GetBuffer());
					str = m_pGridCtrl->GetItemText(row, 2);
					codes[code] = str.GetBuffer();
				}
			}
		}

		CSelfSelectPlateDlg dlg;
		dlg.DoModal();

		if(dlg.m_nPlateCode> 0)
		{
			JMutexAutoLock lock(((CStockApp *)AfxGetApp())->m_pCSelfSelectStockPlates->m_mutex);
			map<int ,string >::iterator it = codes.begin();
			for(; it!=codes.end(); it++)
			{
				int code = it->first;

				CStockPlateData treeNode;
				treeNode.code = Int2String(code, "%06d");
				treeNode.name = it->second;
				treeNode.company_number = 1;
				treeNode.is_stock = 1;

				CStockPlateData treeNodeParent;
				treeNodeParent.code = Int2String(dlg.m_nPlateCode, "%08d");
				treeNodeParent.is_stock = 0;


				CStockPlateTree * pPlates = ((CStockApp *)AfxGetApp())->m_pCSelfSelectStockPlates->m_pTree;
				CStockPlateTree * pNode = pPlates->SearchSpecialNode(treeNodeParent);
				if(pNode)
				{
					pNode->insert(treeNode);
				}

			}
			((CStockApp *)AfxGetApp())->m_pCSelfSelectStockPlates->ImportToDatabase();

			StockInfo_Reqeust req;
			req.code = -ID_READ_STOCKPLATE_SELF_SELECT;
			req.hwnd = NULL;
			((CStockApp *)AfxGetApp())->m_listStockinfoReq.push_front(req);
		}
	}
}
