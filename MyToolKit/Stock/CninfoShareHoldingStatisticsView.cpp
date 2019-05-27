// HexunResearchPaperListView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "IndividualShareDoc.h"
#include "CninfoShareHoldingStatisticsView.h"
#include "CalcCninfoShareHoldingStatistics.h"

#include "SimpleInputBox.h"
#include "SelfSelectPlateDlg.h"

#include "PDFStructureDlg.h"


// CHexunResearchPaperListView

IMPLEMENT_DYNCREATE(CCninfoShareHoldingStatisticsView, CBaseGridCtlView)

CCninfoShareHoldingStatisticsView::CCninfoShareHoldingStatisticsView()
{
	/*显示分页栏*/
	m_bPagination = true;
	
}

CCninfoShareHoldingStatisticsView::~CCninfoShareHoldingStatisticsView()
{
}

BEGIN_MESSAGE_MAP(CCninfoShareHoldingStatisticsView, CBaseGridCtlView)
	ON_COMMAND(ID_TABLE_ADVANCED_QUERY, &CCninfoShareHoldingStatisticsView::OnTableAdvancedQuery)
	ON_UPDATE_COMMAND_UI(ID_TABLE_ADVANCED_QUERY, &CCninfoShareHoldingStatisticsView::OnUpdateTableAdvancedQuery)
END_MESSAGE_MAP()


// CCninfoShareHoldingListView 绘图

void CCninfoShareHoldingStatisticsView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CCninfoShareHoldingListView 诊断

#ifdef _DEBUG
void CCninfoShareHoldingStatisticsView::AssertValid() const
{
	CBaseGridCtlView::AssertValid();
}

#ifndef _WIN32_WCE
void CCninfoShareHoldingStatisticsView::Dump(CDumpContext& dc) const
{
	CBaseGridCtlView::Dump(dc);
}
#endif
#endif //_DEBUG


// CCninfoShareHoldingListView 消息处理程序



void CCninfoShareHoldingStatisticsView::OnInitialUpdate()
{
	CBaseGridCtlView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类

	{
		item_info it =  { CDlgTemplateBuilder::EDIT, _T("季度(如 2016年4季度)"), 
			WS_VISIBLE|WS_CHILD|WS_BORDER|ES_AUTOHSCROLL|WS_TABSTOP, false , _T("") };
		m_vecFindItem.insert(m_vecFindItem.end(), it);
	}

	{
		item_info it =  { CDlgTemplateBuilder::EDIT, _T("股东名称"), 
			WS_VISIBLE|WS_CHILD|WS_BORDER|ES_AUTOHSCROLL|WS_TABSTOP, false , _T("") };
		m_vecFindItem.insert(m_vecFindItem.end(), it);
	}

	{
		item_info it =  { CDlgTemplateBuilder::EDIT, _T("股东性质"), 
			WS_VISIBLE|WS_CHILD|WS_BORDER|ES_AUTOHSCROLL|WS_TABSTOP, false , _T("") };
		m_vecFindItem.insert(m_vecFindItem.end(), it);
	}


	m_strLike = "";
	Startup();
}


void CCninfoShareHoldingStatisticsView::OnTableAdvancedQuery()
{
	// TODO: 在此添加命令处理程序代码

	CDyncItemFindDlg infd(m_vecFindItem);

	infd.Init( _T("查询条件"), _T("查询条件"));
	if(infd.DoModal()==IDOK)
	{
		m_strLike = "";
		/*m_strLike = dlg.m_strInput.GetBuffer();*/
		string strValue = string(m_vecFindItem[0].strValue);
		if(strValue.empty()==false)
		{
			m_strLike += " and year_quarter like \'";
			m_strLike += strValue + "\' ";
		}
		
		strValue = string(m_vecFindItem[1].strValue);
		if(strValue.empty()==false)
		{
			m_strLike += " and share_holder like \'";
			m_strLike += strValue + "\' ";
		}
		
		strValue = string(m_vecFindItem[2].strValue);
		if(strValue.empty()==false)
		{
			m_strLike += " and holder_character like \'";
			m_strLike += strValue + "\' ";
		}
		m_nPageNo = 1;
		Startup();
	}
}

void CCninfoShareHoldingStatisticsView::ProcessXmlWParam(string msgType,string code,string model,\
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


void CCninfoShareHoldingStatisticsView::RedrawGridCtrl(CCalculateTask * cse)
{
	CCalcCninfoShareHoldingStatistics * pStockInfoEx = (CCalcCninfoShareHoldingStatistics *)cse;
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
			m_pGridCtrl->SetRowCount(pStockInfoEx->m_vec_year_quarter.size()+1);
			m_pGridCtrl->SetColumnCount(5);
			m_pGridCtrl->SetFixedRowCount(1);
			m_pGridCtrl->SetFixedColumnCount(1);
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
					else if(col==1)	val = "季度";
					else if(col==2)	val = "股东名称";
					else if(col==3)	val = "股东性质";
					else if(col==4)	val = "公司数量";
					
					Item.strText.Format(_T("%s"), val.c_str());
				} 
				else 
				{
										
					if(col==0)
						Item.nFormat = DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;
					else
						Item.nFormat = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;

					if( col==4 )
					{
						if (!m_pGridCtrl->SetCellType(row, col, RUNTIME_CLASS(CGridCellNumeric)))
							return;
					}
				

					string val ;
					
					if(col==0)		val = Int2String(row);
					else if(col==1)	val = pStockInfoEx->m_vec_year_quarter[row-1];
					else if(col==2) val = pStockInfoEx->m_vec_share_holder[row-1];
					
					else if(col==3)	val = pStockInfoEx->m_vec_holder_character[row-1];
					else if(col==4)	val = Int2String(pStockInfoEx->m_vec_company_number[row-1]);
					

					Item.strText.Format(_T("%s"), val.c_str());

				}
				m_pGridCtrl->SetItem(&Item);
			}
			int k = 1;
		}

		m_pGridCtrl->EnableTitleTips(TRUE);		//	关闭每个单元格的文本超长的 气球提示
		m_pGridCtrl->AutoSize();
		m_pGridCtrl->SetColumnWidth(2, 300);


		//	确定行的高度
		for (int row = 1; row < m_pGridCtrl->GetRowCount(); row++)
		{
			CString strrr = m_pGridCtrl->GetItemText(row,2);
			CGridCellBase* pCell = m_pGridCtrl->GetCell(row,2);
			if(pCell)
			{
				CSize si = pCell->GetTextExtentByWidth(strrr, 300);
				m_pGridCtrl->SetRowHeight(row, si.cy);
			}

		}
	}
}




void CCninfoShareHoldingStatisticsView::OnPageUpdate()
{
	Startup();
}

void CCninfoShareHoldingStatisticsView::Startup()
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();

	CCalcCninfoShareHoldingStatistics * cse =  new CCalcCninfoShareHoldingStatistics(m_hWnd, WM_USER_LOG_1);
	cse->m_request_like = m_strLike;
	cse->m_page_no = m_nPageNo;

	m_MessageCtrl.SetStockRangeForCalculateTask(pDoc, cse);

	cse->ScheduleTask(&CCalcCninfoShareHoldingStatistics::Execute1);
	m_bCalculating = true;
}

void CCninfoShareHoldingStatisticsView::OnUpdateTableAdvancedQuery(CCmdUI *pCmdUI)
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


void CCninfoShareHoldingStatisticsView::PostGridDblClick(int nRow)
{
	// 弹出 “修改 或者 删除” 对话框

	
}