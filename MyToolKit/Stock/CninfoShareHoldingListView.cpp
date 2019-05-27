// HexunResearchPaperListView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "IndividualShareDoc.h"
#include "CninfoShareHoldingListView.h"
#include "CalcCninfoShareHoldingQuery.h"

#include "SimpleInputBox.h"
#include "SelfSelectPlateDlg.h"

#include "PDFStructureDlg.h"

#include "DyncItemEditDlg.h"

// CHexunResearchPaperListView

IMPLEMENT_DYNCREATE(CCninfoShareHoldingListView, CBaseGridCtlView)

CCninfoShareHoldingListView::CCninfoShareHoldingListView()
{
	/*显示分页栏*/
	m_bPagination = true;
	
}

CCninfoShareHoldingListView::~CCninfoShareHoldingListView()
{
}

BEGIN_MESSAGE_MAP(CCninfoShareHoldingListView, CBaseGridCtlView)
	ON_COMMAND(ID_TABLE_ADVANCED_QUERY, &CCninfoShareHoldingListView::OnTableAdvancedQuery)
	ON_UPDATE_COMMAND_UI(ID_TABLE_ADVANCED_QUERY, &CCninfoShareHoldingListView::OnUpdateTableAdvancedQuery)
	
	ON_COMMAND(ID_TABLE_ADD_TO_SELF_SELECT, &CCninfoShareHoldingListView::OnTableAddToSelfSelect)
END_MESSAGE_MAP()


// CCninfoShareHoldingListView 绘图

void CCninfoShareHoldingListView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CCninfoShareHoldingListView 诊断

#ifdef _DEBUG
void CCninfoShareHoldingListView::AssertValid() const
{
	CBaseGridCtlView::AssertValid();
}

#ifndef _WIN32_WCE
void CCninfoShareHoldingListView::Dump(CDumpContext& dc) const
{
	CBaseGridCtlView::Dump(dc);
}
#endif
#endif //_DEBUG


// CCninfoShareHoldingListView 消息处理程序



void CCninfoShareHoldingListView::OnInitialUpdate()
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


void CCninfoShareHoldingListView::OnTableAdvancedQuery()
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

void CCninfoShareHoldingListView::ProcessXmlWParam(string msgType,string code,string model,\
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


void CCninfoShareHoldingListView::RedrawGridCtrl(CCalculateTask * cse)
{
	CCalcCninfoShareHoldingQuery * pStockInfoEx = (CCalcCninfoShareHoldingQuery *)cse;
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
			m_pGridCtrl->SetColumnCount(14);
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

		// 来自同一个公告的记录显示相同的 背景色
		int		bgColor = 0;
		string	last_report_date = "";

		for (int row = 0; row < m_pGridCtrl->GetRowCount(); row++)
		{
			if(row >=1)
			{
				if(pStockInfoEx->m_vec_announcementId[row-1]!=last_report_date)
				{
					last_report_date = pStockInfoEx->m_vec_announcementId[row-1];
					if(bgColor==0) bgColor = 1;
					else bgColor = 0;
				}
			}
			
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

					else if(col==3) val = "公告标识";
					else if(col==4) val = "股东序号";
					else if(col==5) val = "日期";
					else if(col==6) val = "URL";
					else if(col==7)	val = "股东名称";
					else if(col==8) val = "持股数量";
					else if(col==9) val = "持股比例(%)";
					else if(col==10)	val = "限售股数量";
					else if(col==11)	val = "股东性质";
					else if(col==12)	val = "季度";
					else if(col==13)	val = "变动比例(%)";
					
					Item.strText.Format(_T("%s"), val.c_str());
				} 
				else 
				{
					// 来自同一个公告的记录显示相同的 背景色
					if(col >=3)
					{
						Item.mask = GVIF_TEXT|GVIF_FORMAT | GVIF_BKCLR;
						if(bgColor==0)
							Item.crBkClr = RGB(196, 236, 196);
						else 
							Item.crBkClr = RGB(255, 255, 255);
					}
					

					if(col==0)
						Item.nFormat = DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;
					else
						Item.nFormat = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;

					if( col==0 || col ==8 || col==9 || col ==10 || col ==13 )
					{
						if (!m_pGridCtrl->SetCellType(row, col, RUNTIME_CLASS(CGridCellNumeric)))
							return;
					}
				

					if(col==5 )
					{
						if (!m_pGridCtrl->SetCellType(row, col, RUNTIME_CLASS(CGridCellDateTime)))
							return;
					}
					
					if(col==6 )
					{
						if (!m_pGridCtrl->SetCellType(row, col, RUNTIME_CLASS(CGridURLCell)))
							return;
					}

					if(col==7)
					{
						Item.nFormat = DT_RIGHT|DT_WORDBREAK|DT_EDITCONTROL|DT_NOPREFIX|DT_END_ELLIPSIS;
					}

					string val ;
					
					if(col==0)		val = Int2String(row);
					else if(col==1)	val = Int2String(pStockInfoEx->m_vec_code[row-1], "%06d");
					else if(col==2) val = pStockInfoEx->m_vec_abbreviation[row-1];
					
					else if(col==3)	val = pStockInfoEx->m_vec_announcementId[row-1];
					else if(col==4)	val = pStockInfoEx->m_vec_holder_num[row-1];
					else if(col==5)	val = pStockInfoEx->m_vec_report_date[row-1];

					else if(col==6)	val =  /*"http://www.cninfo.com.cn/" +*/ pStockInfoEx->m_vec_url[row-1];

					else if(col==7)	val =  pStockInfoEx->m_vec_share_holder[row-1];
					else if(col==8)	
					{
						if(pStockInfoEx->m_vec_number_of_holding[row-1] >=0)
							val =  Int2String(pStockInfoEx->m_vec_number_of_holding[row-1]);
						else 
							val = "";
					}
					else if(col==9)
					{
						if(pStockInfoEx->m_vec_share_percent[row-1] >=0)
							val =  Double2String( pStockInfoEx->m_vec_share_percent[row-1]);
						else 
							val = "";
						
					}
					else if(col==10)	
					{
						if(pStockInfoEx->m_vec_number_of_restrict[row-1] >=0)
							val = Int2String( pStockInfoEx->m_vec_number_of_restrict[row-1]);
						else 
							val = "";
					}
					else if(col==11)	
					{
						val = pStockInfoEx->m_vec_holder_character[row-1];
					}
					else if(col==12)	
					{
						val = pStockInfoEx->m_vec_year_quarter[row-1];
					}

					else if(col==13)	
					{
						val = pStockInfoEx->m_vec_change_percent[row-1];

						Item.mask    |= (GVIF_FGCLR);
						if(String2Double(val)>0)
							Item.crFgClr = RGB(255, 64, 64);
						else if(String2Double(val)<0)
							Item.crFgClr = RGB(0, 163, 0);
						else
							Item.crFgClr = RGB(0,0,0);
					}

					Item.strText.Format(_T("%s"), val.c_str());

				}
				m_pGridCtrl->SetItem(&Item);
			}
			int k = 1;
		}

		m_pGridCtrl->EnableTitleTips(TRUE);		//	关闭每个单元格的文本超长的 气球提示
		m_pGridCtrl->AutoSize();
		m_pGridCtrl->SetColumnWidth(2, 80);
		m_pGridCtrl->SetColumnWidth(6, 40);
		m_pGridCtrl->SetColumnWidth(7, 200);

		m_pGridCtrl->SetColumnWidth(8, 80);		//	持股数量
		m_pGridCtrl->SetColumnWidth(9, 60);
		m_pGridCtrl->SetColumnWidth(10, 80);
		m_pGridCtrl->SetColumnWidth(11, 120);

		m_pGridCtrl->SetColumnWidth(12, 80);

		//	确定行的高度
		for (int row = 1; row < m_pGridCtrl->GetRowCount(); row++)
		{
			CString strrr = m_pGridCtrl->GetItemText(row,7);
			CGridCellBase* pCell = m_pGridCtrl->GetCell(row,7);
			if(pCell)
			{
				CSize si = pCell->GetTextExtentByWidth(strrr, 200);
				m_pGridCtrl->SetRowHeight(row, si.cy);
			}

		}
	}
}




void CCninfoShareHoldingListView::OnPageUpdate()
{
	Startup();
}

void CCninfoShareHoldingListView::Startup()
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();

	CCalcCninfoShareHoldingQuery * cse =  new CCalcCninfoShareHoldingQuery(m_hWnd, WM_USER_LOG_1);
	cse->m_request_like = m_strLike;
	cse->m_page_no = m_nPageNo;

	m_MessageCtrl.SetStockRangeForCalculateTask(pDoc, cse);

	cse->ScheduleTask(&CCalcCninfoShareHoldingQuery::Execute1);
	m_bCalculating = true;
}

void CCninfoShareHoldingListView::OnUpdateTableAdvancedQuery(CCmdUI *pCmdUI)
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

void CCninfoShareHoldingListView::OnTableAddToSelfSelect()
{
	// TODO: 在此添加命令处理程序代码
	TableAddToSelfSelect();
}


void CCninfoShareHoldingListView::PostGridDblClick(int nRow)
{
	// 弹出 “修改 或者 删除” 对话框

	CDyncItemEditDlg infd;
	vector<item_info>  vec;
	
	{
		item_info it =  { CDlgTemplateBuilder::EDIT, _T("股票代码"), 
			WS_VISIBLE|WS_CHILD|WS_BORDER|ES_AUTOHSCROLL|WS_TABSTOP | ES_READONLY, false , _T("") };
		vec.insert(vec.end(), it);
		infd.m_strItem[vec.size()-1]  = m_pGridCtrl->GetItemText(nRow, 1);
	}

	{
		item_info it =  { CDlgTemplateBuilder::EDIT, _T("股票简称"), 
			WS_VISIBLE|WS_CHILD|WS_BORDER|ES_AUTOHSCROLL|WS_TABSTOP | ES_READONLY, false , _T("")};
		vec.insert(vec.end(), it);
		infd.m_strItem[vec.size()-1]  = m_pGridCtrl->GetItemText(nRow, 2);
	}

	{
		item_info it =  { CDlgTemplateBuilder::EDIT, _T("公告ID"), 
			WS_VISIBLE|WS_CHILD|WS_BORDER|ES_AUTOHSCROLL|WS_TABSTOP | ES_READONLY, true , _T("")};
		vec.insert(vec.end(), it);
		infd.m_strItem[vec.size()-1]  = m_pGridCtrl->GetItemText(nRow, 3);
	}

	{
		item_info it =  { CDlgTemplateBuilder::EDIT, _T("条目序号"), 
			WS_VISIBLE|WS_CHILD|WS_BORDER|ES_AUTOHSCROLL|WS_TABSTOP | ES_READONLY, true , _T("")};
		vec.insert(vec.end(), it);
		infd.m_strItem[vec.size()-1]  = m_pGridCtrl->GetItemText(nRow, 4);
	}

	{
		item_info it =  { CDlgTemplateBuilder::EDIT, _T("公告日期"), 
			WS_VISIBLE|WS_CHILD|WS_BORDER|ES_AUTOHSCROLL|WS_TABSTOP | ES_READONLY, false , _T("")};
		vec.insert(vec.end(), it);
		infd.m_strItem[vec.size()-1]  = m_pGridCtrl->GetItemText(nRow, 5);
	}

	{	// 5
		item_info it =  { CDlgTemplateBuilder::EDIT, _T("交易金额"), 
			WS_VISIBLE|WS_CHILD|WS_BORDER|ES_AUTOHSCROLL|WS_TABSTOP , false , _T("")};
		vec.insert(vec.end(), it);
		infd.m_strItem[vec.size()-1]  = m_pGridCtrl->GetItemText(nRow, 8);
	}

	{	// 6
		item_info it =  { CDlgTemplateBuilder::EDIT, _T("容积率"), 
			WS_VISIBLE|WS_CHILD|WS_BORDER|ES_AUTOHSCROLL|WS_TABSTOP, false , _T("") };
		vec.insert(vec.end(), it);
		infd.m_strItem[vec.size()-1]  = m_pGridCtrl->GetItemText(nRow, 9);
	}

	{
		item_info it =  { CDlgTemplateBuilder::EDIT, _T("土地面积"), 
			WS_VISIBLE|WS_CHILD|WS_BORDER|ES_AUTOHSCROLL|WS_TABSTOP, false , _T("") };
		vec.insert(vec.end(), it);
		infd.m_strItem[vec.size()-1]  = m_pGridCtrl->GetItemText(nRow, 10);
	}

	{
		item_info it =  { CDlgTemplateBuilder::EDIT, _T("土地用途"), 
			WS_VISIBLE|WS_CHILD|WS_BORDER|ES_AUTOHSCROLL|WS_TABSTOP , false , _T("")};
		vec.insert(vec.end(), it);
		infd.m_strItem[vec.size()-1]  = m_pGridCtrl->GetItemText(nRow, 11);
	}

	{
		item_info it =  { CDlgTemplateBuilder::EDIT, _T("建筑面积"), 
			WS_VISIBLE|WS_CHILD|WS_BORDER|ES_AUTOHSCROLL|WS_TABSTOP, false , _T("") };
		vec.insert(vec.end(), it);
		infd.m_strItem[vec.size()-1]  = m_pGridCtrl->GetItemText(nRow, 12);
	}

	infd.Init("修改、删除竞得土地条目","竞得土地信息:" , vec);
	if(infd.DoModal()==IDOK)
	{
		try{
			stringstream ss;
			string strOper = infd.m_strComb.GetBuffer();
			if(strOper.find("Delete")!=string::npos)
			{
				ss << "delete from cninfo_get_land " 
					<< " where announcementId=\'" << infd.m_strItem[2] << "\' " 
					<< " and land_num=" << infd.m_strItem[3] ;

				string sqlStr = ss.str();

				session sql(g_MysqlPool);
				sql << sqlStr; 
			}
			else if(strOper.find("Update")!=string::npos)
			{
				ss << "update cninfo_get_land set rate_of_capacity = \'" << infd.m_strItem[6] << "\', "
					<< " land_area = \'" << infd.m_strItem[7] << "\', "
					<< " deal_money = \'" << infd.m_strItem[5] << "\', "
					<< " area_of_structure = \'" << infd.m_strItem[9] << "\', "
					<< " land_purpose = \'" << infd.m_strItem[8] << "\' "
					<< " where announcementId=\'" << infd.m_strItem[2] << "\' " 
					<< " and land_num=" << infd.m_strItem[3] ;

				string sqlStr = ss.str();

				session sql(g_MysqlPool);
				sql << sqlStr; 
			}

			else if(strOper.find("Insert")!=string::npos)
			{
				ss << "insert into cninfo_get_land() set rate_of_capacity = \'" << infd.m_strItem[6] << "\', "
					<< " land_area = \'" << infd.m_strItem[7] << "\', "
					<< " deal_money = \'" << infd.m_strItem[5] << "\', "
					<< " land_purpose = \'" << infd.m_strItem[8] << "\' "
					<< " where announcementId=\'" << infd.m_strItem[2] << "\' " 
					<< " and land_num=" << infd.m_strItem[3] ;

				string sqlStr = ss.str();

				session sql(g_MysqlPool);
				sql << sqlStr; 
			}

			//	修改完成后，重新读入
			m_nPageNo = 1;
			Startup();
		}
		catch(...)
		{
			AfxMessageBox("操作失败，请检查数据");
		}
		
	}
}