// HexunResearchPaperListView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "IndividualShareDoc.h"
#include "CninfoAnnouncePDFListView.h"
#include "CalcCninfoAnnouncePDFQuery.h"

#include "SimpleInputBox.h"
#include "SelfSelectPlateDlg.h"

// CHexunResearchPaperListView

IMPLEMENT_DYNCREATE(CCninfoAnnouncePDFListView, CBaseRichEditFormView)

CCninfoAnnouncePDFListView::CCninfoAnnouncePDFListView()
{
	/*显示分页栏*/
	m_bPagination = true;
	
}

CCninfoAnnouncePDFListView::~CCninfoAnnouncePDFListView()
{
}

BEGIN_MESSAGE_MAP(CCninfoAnnouncePDFListView, CBaseRichEditFormView)
	ON_NOTIFY(NM_DBLCLK, IDC_GRID, OnGridDblClick)
	ON_COMMAND(ID_TABLE_ADVANCED_QUERY, &CCninfoAnnouncePDFListView::OnTableAdvancedQuery)
	ON_UPDATE_COMMAND_UI(ID_TABLE_ADVANCED_QUERY, &CCninfoAnnouncePDFListView::OnUpdateTableAdvancedQuery)
	
	ON_COMMAND(ID_TABLE_ADD_TO_SELF_SELECT, &CCninfoAnnouncePDFListView::OnTableAddToSelfSelect)

	ON_NOTIFY(GVN_SELCHANGING, IDC_GRID, OnGridStartSelChange)
	ON_NOTIFY(GVN_SELCHANGED, IDC_GRID, OnGridEndSelChange)
END_MESSAGE_MAP()


// CHexunResearchPaperListView 绘图

void CCninfoAnnouncePDFListView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CHexunResearchPaperListView 诊断

#ifdef _DEBUG
void CCninfoAnnouncePDFListView::AssertValid() const
{
	CBaseRichEditFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CCninfoAnnouncePDFListView::Dump(CDumpContext& dc) const
{
	CBaseRichEditFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CHexunResearchPaperListView 消息处理程序


void CCninfoAnnouncePDFListView::OnInitialUpdate()
{
	CBaseRichEditFormView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	m_strLike = "";
	Startup();
}

void CCninfoAnnouncePDFListView::RedrawGridCtrl(CCalculateTask * cse)
{
	CCalcCninfoAnnouncePDFQuery * pStockInfoEx = (CCalcCninfoAnnouncePDFQuery *)cse;
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();

	m_bCalculating = false;
	m_nPageNo = pStockInfoEx->m_page_no;
	m_nMaxPage = pStockInfoEx->m_max_page;
	CString str1;
	str1.Format("第 %d 页  共 %d 页   关键字 : %s", m_nPageNo, m_nMaxPage, m_strLike.c_str());
	//m_pStaticShowPage->SetWindowText(str1);
	GetDlgItem(IDC_STATIC)->SetWindowText(str1);

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
		//m_pStaticTip->SetWindowText(ss.str().c_str());
	}

	m_vec_catalogContent = pStockInfoEx->m_vec_catalogContent;


	if(m_GridCtrl)
	{
		CStockApp * pApp = (CStockApp *)AfxGetApp();

		try {
			m_GridCtrl.SetRowCount(pStockInfoEx->m_vec_code.size()+1);
			m_GridCtrl.SetColumnCount(7);
			m_GridCtrl.SetFixedRowCount(1);
			m_GridCtrl.SetFixedColumnCount(3);
			m_GridCtrl.SetHeaderSort(TRUE);
		}
		catch (CMemoryException* e)
		{
			e->ReportError();
			e->Delete();
			return;
		}

		for (int row = 0; row < m_GridCtrl.GetRowCount(); row++)
		{
			for (int col = 0; col < m_GridCtrl.GetColumnCount(); col++)
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
					else if(col==3) val = "日期";
					else if(col==4) val = "URL";
					else if(col==5)	val = "公告标题";
					else if(col==6) val = "目录";
					
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
						if (!m_GridCtrl.SetCellType(row, col, RUNTIME_CLASS(CGridCellNumeric)))
							return;
					}
				

					if(col==3 )
					{
						if (!m_GridCtrl.SetCellType(row, col, RUNTIME_CLASS(CGridCellDateTime)))
							return;
					}
					
					if(col==4 )
					{
						if (!m_GridCtrl.SetCellType(row, col, RUNTIME_CLASS(CGridURLCell)))
							return;
					}

					string val ;
					if(col==0)		val = Int2String(row);
					else if(col==1)	val = Int2String(pStockInfoEx->m_vec_code[row-1], "%06d");
					else if(col==2) val = pStockInfoEx->m_vec_abbreviation[row-1];
					else if(col==3)	val =  pStockInfoEx->m_vec_report_date[row-1];

					else if(col==4)	val =  "http://www.cninfo.com.cn/"  + pStockInfoEx->m_vec_url[row-1];

					else if(col==5)	val =  pStockInfoEx->m_vec_title[row-1];
					else if(col==6)	val =  pStockInfoEx->m_vec_catalogName[row-1];

					Item.strText.Format(_T("%s"), val.c_str());

				}
				m_GridCtrl.SetItem(&Item);
			}
			int k = 1;
		}

		m_GridCtrl.AutoSize();
		m_GridCtrl.SetColumnWidth(2, 80);
		m_GridCtrl.SetColumnWidth(4, 60);
		m_GridCtrl.SetColumnWidth(5, 200);
		m_GridCtrl.SetColumnWidth(6, 400);
	}
}



void CCninfoAnnouncePDFListView::OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
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
		CString str = m_GridCtrl.GetItemText(pItem->iRow, 1);
		m_MessageCtrl.FocusStockOnLeftPane(str.GetBuffer());
	}
}

void CCninfoAnnouncePDFListView::OnTableAdvancedQuery()
{
	// TODO: 在此添加命令处理程序代码

	CSimpleInputBox dlg;

	dlg.m_ComboString.insert(dlg.m_ComboString.end(), "解除限售");
	dlg.m_ComboString.insert(dlg.m_ComboString.end(), "公司控制权");
	dlg.m_ComboString.insert(dlg.m_ComboString.end(), "授予价格");
	dlg.m_ComboString.insert(dlg.m_ComboString.end(), "解锁条件");
	

	dlg.m_strCaption = _T("输入查询关键字符");
	if(IDOK==dlg.DoModal())
	{
		m_strLike = dlg.m_strInput.GetBuffer();
		m_nPageNo = 1;
		Startup();
	}
}

void CCninfoAnnouncePDFListView::OnPageUpdate()
{
	Startup();
}

void CCninfoAnnouncePDFListView::Startup()
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();

	CCalcCninfoAnnouncePDFQuery * cse =  new CCalcCninfoAnnouncePDFQuery(m_hWnd, WM_USER_LOG_1);
	cse->m_request_like = m_strLike;
	cse->m_page_no = m_nPageNo;

	m_MessageCtrl.SetStockRangeForCalculateTask(pDoc, cse);

	cse->ScheduleTask(&CCalcCninfoAnnouncePDFQuery::Execute1);
	m_bCalculating = true;
}

void CCninfoAnnouncePDFListView::OnUpdateTableAdvancedQuery(CCmdUI *pCmdUI)
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

void CCninfoAnnouncePDFListView::OnTableAddToSelfSelect()
{
	// TODO: 在此添加命令处理程序代码
	//TableAddToSelfSelect();
}


// GVN_SELCHANGING
void CCninfoAnnouncePDFListView::OnGridStartSelChange(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	/*Trace(_T("Start Selection Change on row %d, col %d (%d Selected)\n"), 
		pItem->iRow, pItem->iColumn, m_Grid.GetSelectedCount());*/
}

// GVN_SELCHANGED
void CCninfoAnnouncePDFListView::OnGridEndSelChange(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	/*Trace(_T("End Selection Change on row %d, col %d (%d Selected)\n"), 
		pItem->iRow, pItem->iColumn, m_Grid.GetSelectedCount());*/

	if(pItem->iRow >0 && pItem->iRow <= m_vec_catalogContent.size())
	{
		
		m_RichEdit.SetWindowText(m_vec_catalogContent[pItem->iRow - 1].c_str());
		m_RichEdit.HideSelection(false, true);	//	选择的区域被 永久显示

		vector<string>	vecTarget;
		vecTarget.insert(vecTarget.end(), "业绩考核目标");
		vecTarget.insert(vecTarget.end(), "将导致公司");
		vecTarget.insert(vecTarget.end(), "不会导致公司");
		vecTarget.insert(vecTarget.end(), "不会导致本公司");
		vecTarget.insert(vecTarget.end(), "未导致公司");
		vecTarget.insert(vecTarget.end(), "不会导致发行人");
		vecTarget.insert(vecTarget.end(), "仍为公司");
		vecTarget.insert(vecTarget.end(), "业绩考核要求");
		vecTarget.insert(vecTarget.end(), "业绩考核的指标");
		vecTarget.insert(vecTarget.end(), "解锁业绩条件");

		for(int i=0; i<vecTarget.size(); i++)
		{

			FINDTEXTEX ft;
			ft.chrg.cpMin = 0;
			ft.chrg.cpMax = m_RichEdit.GetWindowTextLength();
			ft.lpstrText = vecTarget[i].c_str();

			long lPos = m_RichEdit.FindText(FR_DOWN , &ft);
			if(lPos!=-1)
			{
				m_RichEdit.SetSel(lPos, lPos + strlen(ft.lpstrText));
			}
		}
		
	}
}