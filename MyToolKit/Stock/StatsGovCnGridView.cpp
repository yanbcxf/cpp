// StatsGovCnGridView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "StatsGovCnGridView.h"
#include "CalcStatsGovCn.h"

// StatsGovCnGridView

IMPLEMENT_DYNCREATE(CStatsGovCnGridView, CBaseGridCtlView)

CStatsGovCnGridView::CStatsGovCnGridView()
{

}

CStatsGovCnGridView::~CStatsGovCnGridView()
{
}

BEGIN_MESSAGE_MAP(CStatsGovCnGridView, CBaseGridCtlView)
END_MESSAGE_MAP()


// StatsGovCnGridView 绘图

void CStatsGovCnGridView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// StatsGovCnGridView 诊断

#ifdef _DEBUG
void CStatsGovCnGridView::AssertValid() const
{
	CBaseGridCtlView::AssertValid();
}

#ifndef _WIN32_WCE
void CStatsGovCnGridView::Dump(CDumpContext& dc) const
{
	CBaseGridCtlView::Dump(dc);
}
#endif
#endif //_DEBUG


// StatsGovCnGridView 消息处理程序


void CStatsGovCnGridView::OnInitialUpdate()
{
	CBaseGridCtlView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();


	Startup();	
}



void CStatsGovCnGridView::Startup()
{
	CStatsGovCnDoc * pDoc = (CStatsGovCnDoc *)GetDocument();
	CCalcStatsGovCn * cse =  new CCalcStatsGovCn(m_hWnd, WM_USER_LOG_1);
	cse->m_str_db_code = pDoc->m_str_db_code;
	cse->m_vec_zb_code = pDoc->m_vec_zb_code;
	cse->m_vec_reg_code = pDoc->m_vec_reg_code;
	cse->m_nOutput = pDoc->m_nOutput;

	cse->ScheduleTask(&CCalcStatsGovCn::Execute1);
	m_bCalculating = true;
}


void CStatsGovCnGridView::ProcessXmlWParam(string msgType,string code,string model,\
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


void CStatsGovCnGridView::RedrawGridCtrl(CCalculateTask * cse)
{

	CCalcStatsGovCn * pCse = (CCalcStatsGovCn *)cse;
	CStatsGovCnDoc * pDoc = (CStatsGovCnDoc *)GetDocument();

	//	对现在数据进行汇总，以便表格显示
	map<string , map<string, string>>	map_Cell;
	set<string >		set_Col;
	set<string >		set_Row;
	vector<string>		vec_Col;
	vector<string>		vec_Row;
	for(int i=0; i< pCse->zb_name.size(); i++)
	{
		map_Cell[pCse->zb_name[i]][pCse->sj_code[i]] = Double2String(pCse->data[i]);
		set_Col.insert(set_Col.end(), pCse->sj_code[i]);
		set_Row.insert(set_Row.end(), pCse->zb_name[i]);
	}

	set<string>::iterator it = set_Col.begin();
	for(; it!=set_Col.end(); it++)
	{
		vec_Col.push_back(*it);
	}

	it = set_Row.begin();
	for(; it!=set_Row.end(); it++)
	{
		vec_Row.push_back(*it);
	}

	if(m_pGridCtrl && map_Cell.size()>0)
	{
		CStockApp * pApp = (CStockApp *)AfxGetApp();

		try {
			m_pGridCtrl->SetRowCount(vec_Row.size() + 1);
			m_pGridCtrl->SetColumnCount(set_Col.size() + 1);
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
			for (int col = 0; col < m_pGridCtrl->GetColumnCount(); col++)
			{ 
				GV_ITEM Item;
				Item.mask = GVIF_TEXT|GVIF_FORMAT;
				Item.row = row;
				Item.col = col;
				string val;

				if (row < 1) {
					Item.nFormat = DT_LEFT|DT_WORDBREAK;
					
					if(col==0)		
						val = pCse->title_name;
					else
					{
						val = vec_Col[m_pGridCtrl->GetColumnCount() - col-1];
					}
					Item.strText.Format(_T("%s"), val.c_str());
				} 
				else 
				{
					if(col==0)
						Item.nFormat = DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;
					else
						Item.nFormat = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;

					if( col> 0 )
					{
						if (!m_pGridCtrl->SetCellType(row, col, RUNTIME_CLASS(CGridCellNumeric)))
							return;
					}

					if(col==0)
					{
						val = vec_Row[row-1];
					}
					else
					{
						string zb_name = vec_Row[row-1];
						string sj_code = vec_Col[m_pGridCtrl->GetColumnCount() - col-1];

						if(map_Cell[zb_name].count(sj_code) >0)
						{
							val = map_Cell[zb_name][sj_code];
						}
						else 
							val = "";
					}
								
					Item.strText.Format(_T("%s"), val.c_str());

				}
				m_pGridCtrl->SetItem(&Item);
			}
			int k = 1;
		}

		m_pGridCtrl->AutoSize();
		//m_pGridCtrl->SetColumnWidth(2, 80);
	}

}

