// HexunResearchPaperListView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "IndividualShareDoc.h"
#include "SimulatedAccountListView.h"
#include "CalcSimulatedAccountQuery.h"

#include "SimpleInputBox.h"
#include "SelfSelectPlateDlg.h"

#include "PDFStructureDlg.h"

#include "DyncItemEditDlg.h"

// CHexunResearchPaperListView

IMPLEMENT_DYNCREATE(CSimulatedAccountListView, CBaseGridCtlView)

CSimulatedAccountListView::CSimulatedAccountListView()
{
	/*显示分页栏*/
	m_bPagination = true;
	
}

CSimulatedAccountListView::~CSimulatedAccountListView()
{
}

BEGIN_MESSAGE_MAP(CSimulatedAccountListView, CBaseGridCtlView)
	ON_COMMAND(ID_TABLE_ADVANCED_QUERY, &CSimulatedAccountListView::OnTableAdvancedQuery)
	ON_UPDATE_COMMAND_UI(ID_TABLE_ADVANCED_QUERY, &CSimulatedAccountListView::OnUpdateTableAdvancedQuery)
	
	ON_COMMAND(ID_TABLE_ADD_TO_SELF_SELECT, &CSimulatedAccountListView::OnTableAddToSelfSelect)
END_MESSAGE_MAP()


// CCninfoGetLandListView 绘图

void CSimulatedAccountListView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CCninfoGetLandListView 诊断

#ifdef _DEBUG
void CSimulatedAccountListView::AssertValid() const
{
	CBaseGridCtlView::AssertValid();
}

#ifndef _WIN32_WCE
void CSimulatedAccountListView::Dump(CDumpContext& dc) const
{
	CBaseGridCtlView::Dump(dc);
}
#endif
#endif //_DEBUG


// CSimulatedAccountListView 消息处理程序



void CSimulatedAccountListView::OnInitialUpdate()
{
	CBaseGridCtlView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	m_strLike = "";
	Startup();
}



void CSimulatedAccountListView::ProcessXmlWParam(string msgType,string code,string model,\
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
}


void CSimulatedAccountListView::RedrawGridCtrl(CCalculateTask * cse)
{
	CCalcSimulatedAccountQuery * pStockInfoEx = (CCalcSimulatedAccountQuery *)cse;
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();

	m_nPageNo = pStockInfoEx->m_page_no;
	m_nMaxPage = pStockInfoEx->m_max_page;
	CString str1;
	str1.Format("第 %d 页  共 %d 页", m_nPageNo, m_nMaxPage);
	m_pStaticShowPage->SetWindowText(str1);

	stringstream ss;
	if(m_strLike.empty()==false)
	{
		if(ss.str().empty()==false)
			ss << ",";
		ss << "关键字：" << m_strLike;
	}
	if(m_sPlateName.empty()==false)
	{
		if(ss.str().empty()==false)
			ss << ",";
		ss << "自选板块：" << m_sPlateName;
	}

	if(ss.str().empty()==false)
	{
		m_pStaticTip->SetWindowText(ss.str().c_str());
	}

	if(m_pGridCtrl)
	{
		CStockApp * pApp = (CStockApp *)AfxGetApp();

		try {
			m_pGridCtrl->SetRowCount(pStockInfoEx->m_vec_code.size()+1);
			m_pGridCtrl->SetColumnCount(13);
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
			
			string str_area_of_structure = "";

			for (int col = 0; col < m_pGridCtrl->GetColumnCount(); col++)
			{ 
				GV_ITEM Item;
				Item.mask = GVIF_TEXT|GVIF_FORMAT ;
				Item.row = row;
				Item.col = col;
				if (row < 1) {
					Item.nFormat = DT_LEFT|DT_WORDBREAK;
					string val;
					if(col==0)		val = "序号";
					else if(col==1)	val = "代码";
					else if(col==2) val = "股票简称";
					else if(col==3) val = "日期";

					else if(col==4)	val = "可以资金";
					else if(col==5) val = "持有股票";
					else if(col==6)	val = "购买成本";

					else if(col==7)	val = "损益率";
					else if(col==8)	val = "Action";
					else if(col==9)	val = "买卖建议";
					else if(col==10)	val = "MACD";
					else if(col==11)	val = "KDJ";
					else if(col==12)	val = "K线图";

					
					Item.strText.Format(_T("%s"), val.c_str());
				} 
				else 
				{

					if(col==0)
						Item.nFormat = DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;
					else
						Item.nFormat = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;

					if( col==0 || col == 4 || col ==5 || col==6 || col ==7)
					{
						if (!m_pGridCtrl->SetCellType(row, col, RUNTIME_CLASS(CGridCellNumeric)))
							return;
					}
				

					if(col==3 )
					{
						if (!m_pGridCtrl->SetCellType(row, col, RUNTIME_CLASS(CGridCellDateTime)))
							return;
					}

					if(col==8 || col ==9 || col ==10)
					{
						Item.nFormat = DT_RIGHT|DT_WORDBREAK|DT_EDITCONTROL|DT_NOPREFIX|DT_END_ELLIPSIS;
					}
					

					string val ;
					
					if(col==0)		val = Int2String(row);
					else if(col==1)	val = Int2String(pStockInfoEx->m_vec_code[row-1], "%06d");
					else if(col==2) val = pStockInfoEx->m_vec_abbreviation[row-1];
					else if(col==3)	val = pStockInfoEx->m_vec_report_date[row-1];

					else if(col==4)	val =  Double2String(pStockInfoEx->m_vec_capital[row-1]);
					else if(col==5)	val =  Double2String(pStockInfoEx->m_vec_shares[row-1]);
					else if(col==6) val =  Double2String(pStockInfoEx->m_vec_cost[row-1]);
					else if(col==7) val =  Double2String(pStockInfoEx->m_vec_loss_earning[row-1]);

					else if(col==8)	val = pStockInfoEx->m_vec_action[row-1];
					else if(col==9)	val = pStockInfoEx->m_vec_remark[row-1];
					else if(col==10)	val = pStockInfoEx->m_vec_desc_macd[row-1];
					else if(col==11)	val = pStockInfoEx->m_vec_desc_kdj[row-1];
					else if(col==12)	val = pStockInfoEx->m_vec_desc_k[row-1];


					Item.strText.Format(_T("%s"), val.c_str());
				}
				m_pGridCtrl->SetItem(&Item);
			}
			int k = 1;
		}

		m_pGridCtrl->EnableTitleTips(FALSE);		//	关闭每个单元格的文本超长的 气球提示
		m_pGridCtrl->AutoSize();
		m_pGridCtrl->SetColumnWidth(8, 150);
		m_pGridCtrl->SetColumnWidth(9, 150);
		m_pGridCtrl->SetColumnWidth(10, 80);

		//	确定行的高度
		for (int row = 1; row < m_pGridCtrl->GetRowCount(); row++)
		{
			int cy = 0;
			CString strrr = m_pGridCtrl->GetItemText(row,8);
			CGridCellBase* pCell = m_pGridCtrl->GetCell(row,8);
			if(pCell)
			{
				CSize si = pCell->GetTextExtentByWidth(strrr, 150);
				if(si.cy>cy)
					cy = si.cy;
			}

			strrr = m_pGridCtrl->GetItemText(row,9);
			pCell = m_pGridCtrl->GetCell(row,9);
			if(pCell)
			{
				CSize si = pCell->GetTextExtentByWidth(strrr, 150);
				if(si.cy>cy)
					cy = si.cy;
			}

			strrr = m_pGridCtrl->GetItemText(row,10);
			pCell = m_pGridCtrl->GetCell(row,10);
			if(pCell)
			{
				CSize si = pCell->GetTextExtentByWidth(strrr, 80);
				if(si.cy>cy)
					cy = si.cy;
			}

			if(cy>0)
				m_pGridCtrl->SetRowHeight(row, cy);
		}
	}
}



void CSimulatedAccountListView::OnTableAdvancedQuery()
{
	// TODO: 在此添加命令处理程序代码

	CSimpleInputBox dlg;
	dlg.m_ComboString.insert(dlg.m_ComboString.end(), "退出");
	dlg.m_ComboString.insert(dlg.m_ComboString.end(), "进入");


	dlg.m_strCaption = _T("输入查询关键字符");
	if(IDOK==dlg.DoModal())
	{
		m_strLike = dlg.m_strInput.GetBuffer();
		m_nPageNo = 1;
		Startup();
	}
}

void CSimulatedAccountListView::OnPageUpdate()
{
	Startup();
}

void CSimulatedAccountListView::Startup()
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();

	CCalcSimulatedAccountQuery * cse =  new CCalcSimulatedAccountQuery(m_hWnd, WM_USER_LOG_1);
	cse->m_request_like = m_strLike;
	cse->m_page_no = m_nPageNo;

	m_MessageCtrl.SetStockRangeForCalculateTask(pDoc, cse);

	cse->ScheduleTask(&CCalcSimulatedAccountQuery::Execute1);
	m_bCalculating = true;
}

void CSimulatedAccountListView::OnUpdateTableAdvancedQuery(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	if(m_bCalculating)
	{
		pCmdUI->Enable(false);
	}
	else
	{
		pCmdUI->Enable(true);
	}
}

void CSimulatedAccountListView::OnTableAddToSelfSelect()
{
	// TODO: 在此添加命令处理程序代码
	TableAddToSelfSelect();
}


void CSimulatedAccountListView::PostGridDblClick(int nRow)
{
	// 弹出 “修改 或者 删除” 对话框

}