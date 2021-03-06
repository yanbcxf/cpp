// LotteryNumberGridView.cpp: 实现文件
//

#include "stdafx.h"
#include "Lottery.h"
#include "LotteryDoc.h"
#include "MainFrm.h"
#include "LotteryNumberGridBView.h"
#include "CalcLotteryNumberList.h"

#include "DyncItemGroupDlg.h"
#include "GridDlg.h"
#include "ChartDlg.h"

// CLotteryNumberGridView

IMPLEMENT_DYNCREATE(CLotteryNumberGridBView, CBaseGridCtlView)

CLotteryNumberGridBView::CLotteryNumberGridBView()
{

}

CLotteryNumberGridBView::~CLotteryNumberGridBView()
{
}

BEGIN_MESSAGE_MAP(CLotteryNumberGridBView, CBaseGridCtlView)
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_LOTTERY_RANGE_SET, &CLotteryNumberGridBView::OnLotteryRangeSet)
	ON_COMMAND(ID_REGION_NUMBER_SET, &CLotteryNumberGridBView::OnRegionNumberSet)
END_MESSAGE_MAP()


// CLotteryNumberGridView 绘图

void CLotteryNumberGridBView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO:  在此添加绘制代码
}


// CLotteryNumberGridBView 诊断

#ifdef _DEBUG
void CLotteryNumberGridBView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CLotteryNumberGridBView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CLotteryNumberGridBView 消息处理程序


void CLotteryNumberGridBView::OnInitialUpdate()
{
	CBaseGridCtlView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类

	Startup();

}

void CLotteryNumberGridBView::Startup()
{
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();
	CCalcLotteryNumberList * cse = new CCalcLotteryNumberList(m_hWnd, WM_USER_LOG_1);
	vector<LotteryInfo>	vecInfo = ((CLotteryApp *)AfxGetApp())->m_vec_lottery_info;
	cse->m_lottery_info = vecInfo[pDoc->m_lottery_code];
	cse->m_bContainSpecial = pDoc->m_bContainSpecial;
	cse->m_start_number = m_start_number;
	cse->m_end_number = m_end_number;

	vector<int> region, region1;
	if (pDoc->m_region_1_start.size() > 0) {
		region.push_back(pDoc->m_region_1_start[0]);    region1.push_back(pDoc->m_region_1_end[0]);
	}
	if (pDoc->m_region_2_start.size() > 0) {
		region.push_back(pDoc->m_region_2_start[0]);    region1.push_back(pDoc->m_region_2_end[0]);
	}
	if (pDoc->m_region_3_start.size() > 0) {
		region.push_back(pDoc->m_region_3_start[0]);    region1.push_back(pDoc->m_region_3_end[0]);
	}
	if (pDoc->m_region_4_start.size() > 0) {
		region.push_back(pDoc->m_region_4_start[0]);    region1.push_back(pDoc->m_region_4_end[0]);
	}
	if (pDoc->m_region_5_start.size() > 0) {
		region.push_back(pDoc->m_region_5_start[0]);    region1.push_back(pDoc->m_region_5_end[0]);
	}
	if (pDoc->m_region_6_start.size() > 0) {
		region.push_back(pDoc->m_region_6_start[0]);    region1.push_back(pDoc->m_region_6_end[0]);
	}
	cse->m_region_start = region;
	cse->m_region_end = region1;
	cse->ScheduleTask(&CCalcLotteryNumberList::Execute1);
}


void CLotteryNumberGridBView::ProcessXmlWParam(string msgType, string code, string model, \
	string param1, string param2, string param3)
{
	if (msgType == "TaskFinished")
	{
		stringstream ss;
		ss << "任务【" << (param1) << "】 计算完成. \r\n";

		CCalculateTask * cse = CCalculateTask::FetchTask(atol(param1.c_str()));
		CCalcLotteryNumberList * pStockInfoEx = (CCalcLotteryNumberList *)cse;
		m_vec_lottery_number = pStockInfoEx->m_vec_lottery_number;
		RedrawGridCtrl(cse);
		if (cse != NULL)
			delete cse;
		sendToOutput(ss.str().c_str(), NULL, WM_USER_LOG_1);

		RefreshStatusBar();
	}
}


void CLotteryNumberGridBView::RedrawGridCtrl(CCalculateTask * cse)
{
	CCalcLotteryNumberList * pStockInfoEx = (CCalcLotteryNumberList *)cse;
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();
	

	if (m_pGridCtrl)
	{
		CLotteryApp * pApp = (CLotteryApp *)AfxGetApp();

		try {
			m_pGridCtrl->SetRowCount(pStockInfoEx->m_vec_lottery_number.size() + 1);
			m_pGridCtrl->SetColumnCount(16 + pDoc->m_region_start.size());
			m_pGridCtrl->SetFixedRowCount(1);
			m_pGridCtrl->SetFixedColumnCount(9);
			m_pGridCtrl->SetHeaderSort(FALSE);
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
				Item.mask = GVIF_TEXT | GVIF_FORMAT;
				Item.row = row;
				Item.col = col;
				if (row < 1) {
					Item.nFormat = DT_LEFT | DT_WORDBREAK;
					string val;
					if (col == 0)	val = "期号";
					else if (col == 1) val = "NO.1";
					else if (col == 2) val = "NO.2";
					else if (col == 3) val = "NO.3";
					else if (col == 4) val = "NO.4";
					else if (col == 5) val = "NO.5";
					else if (col == 6) val = "NO.6";
					else if (col == 7) val = "NO.7";
					else if (col == 8) val = "NO.S";
					
					else if (col == 9) val = "NO.1-NO.2";
					else if (col == 10) val = "NO.2-NO.3";
					else if (col == 11) val = "NO.3-NO.4";
					else if (col == 12) val = "NO.4-NO.5";
					else if (col == 13) val = "NO.5-NO.6";
					else if (col == 14) val = "NO.6-NO.7";
					else if (col == 15) val = "NO.7-NO.8";

					else if (col >= 16)
					{
						int idx = col - 16;
						if (idx < pDoc->m_region_start.size())
						{
							CString str;
							str.Format("区间 %d-%d", pDoc->m_region_start[idx], pDoc->m_region_end[idx]);
							val = str.GetBuffer();
						}
					}
					
					Item.strText.Format(_T("%s"), val.c_str());
				}
				else
				{
					if (col == 0)
						Item.nFormat = DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;
					else
						Item.nFormat = DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;

					if (col == 0)
					{
						if (!m_pGridCtrl->SetCellType(row, col, RUNTIME_CLASS(CGridCellNumeric)))
							return;
					}

					if (col >0)
					{
						if (!m_pGridCtrl->SetCellType(row, col, RUNTIME_CLASS(CGridCellNumeric)))
							return;
					}

					/*if (col == 3 || col == 4)
					{
						if (!m_pGridCtrl->SetCellType(row, col, RUNTIME_CLASS(CGridCellDateTime)))
							return;
					}*/

					string val = "";
					if (col == 0)		val = Int2String(pStockInfoEx->m_vec_lottery_number[row - 1].periods);
					else if (col == 1)	val = Int2String(pStockInfoEx->m_vec_lottery_number[row - 1].number[0]);
					else if (col == 2)	val = Int2String(pStockInfoEx->m_vec_lottery_number[row - 1].number[1]);
					else if (col == 3)	val = Int2String(pStockInfoEx->m_vec_lottery_number[row - 1].number[2]);
					else if (col == 4)	val = Int2String(pStockInfoEx->m_vec_lottery_number[row - 1].number[3]);
					else if (col == 5)	val = Int2String(pStockInfoEx->m_vec_lottery_number[row - 1].number[4]);
					else if (col == 6)	val = Int2String(pStockInfoEx->m_vec_lottery_number[row - 1].number[5]);
					else if (col == 7)	val = Int2String(pStockInfoEx->m_vec_lottery_number[row - 1].number[6]);
					else if (col == 8)	val = Int2String(pStockInfoEx->m_vec_lottery_number[row - 1].special_number);
									

					else if (col == 9)	val = Int2String(pStockInfoEx->m_vec_lottery_number[row - 1].adjacent_interval[0]);
					else if (col == 10)	val = Int2String(pStockInfoEx->m_vec_lottery_number[row - 1].adjacent_interval[1]);
					else if (col == 11)	val = Int2String(pStockInfoEx->m_vec_lottery_number[row - 1].adjacent_interval[2]);
					else if (col == 12)	val = Int2String(pStockInfoEx->m_vec_lottery_number[row - 1].adjacent_interval[3]);
					else if (col == 13)	val = Int2String(pStockInfoEx->m_vec_lottery_number[row - 1].adjacent_interval[4]);
					else if (col == 14)	val = Int2String(pStockInfoEx->m_vec_lottery_number[row - 1].adjacent_interval[5]);
					else if (col == 15)	val = Int2String(pStockInfoEx->m_vec_lottery_number[row - 1].adjacent_interval[6]);

					else if (col >= 16)
					{
						int idx = col - 16;
						val = Int2String(pStockInfoEx->m_vec_lottery_number[row - 1].region_count[idx]);
					}

					if (col >= 1 && col <= 8 || col >= 9 && col<= 15)
					{
						if (val == "0")	val = "";
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


void CLotteryNumberGridBView::OnSetFocus(CWnd* pOldWnd)
{
	CBaseGridCtlView::OnSetFocus(pOldWnd);

	// TODO: 在此处添加消息处理程序代码
	RefreshStatusBar();
}

void CLotteryNumberGridBView::PostGridDblClick(int nRow)
{
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();
	vector<LotteryInfo>	vecInfo = ((CLotteryApp *)AfxGetApp())->m_vec_lottery_info;
	LotteryInfo  info = vecInfo[pDoc->m_lottery_code];

	if (m_vec_lottery_number.size() < 1)
		return;

	CString str = m_pGridCtrl->GetItemText(nRow, 0);
	int nPeriods = atoi(str.GetBuffer());
	int nIdx = 0;
	for (int i = 0; i < m_vec_lottery_number.size(); i++)
	{
		if (m_vec_lottery_number[i].periods == nPeriods)
		{
			nIdx = i;
			break;
		}
	}

	int nNumPerLine = 6;
	CDyncItemGroupDlg infd;
	infd.CXCHECKBOX = 20;
	infd.CYCHECKBOX = 20;
	infd.CXSPACE = 3;
	infd.CYSPACE = 3;

	for (int i = 1; i <= info.totalnum; i++)
	{
		int nRow = (i - 1) / nNumPerLine;
		int nCol = (i - 1) % nNumPerLine;
		infd.m_vecFindItem[0][nRow][nCol].nType = CDlgTemplateBuilder::CHECKBOX;
		infd.m_vecFindItem[0][nRow][nCol].dwStyle = BS_PUSHLIKE;
		sprintf_s(infd.m_vecFindItem[0][nRow][nCol].caption, 64, "%d", i);
		
		//
		if(pDoc->m_bContainSpecial&& i == m_vec_lottery_number[nIdx].special_number)
			infd.m_vecFindItem[0][nRow][nCol].intItem = 1;
		
		// 比较正选号
		for (int k = 0; k < info.regularnum; k++)
		{
			if (m_vec_lottery_number[nIdx].number[k] == i)
			{
				infd.m_vecFindItem[0][nRow][nCol].intItem = 1;
				break;
			}
		}
			
	}

	infd.Init(_T("集团数分析"), _T("集团数分析"));
	infd.DoModal();
}


void CLotteryNumberGridBView::OnLotteryRangeSet()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();
	if (m_vec_lottery_number.size() < 1)
		return;
	int nMin = 100000000;
	int nMax = 0;
	for (int i = 0; i < m_vec_lottery_number.size(); i++)
	{
		if (m_vec_lottery_number[i].periods > nMax)
			nMax = m_vec_lottery_number[i].periods;
		if (m_vec_lottery_number[i].periods < nMin)
			nMin = m_vec_lottery_number[i].periods;
	}

	CDyncItemGroupDlg infd;
	infd.m_vecFindItem[0][0][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[0][0][0].caption, _T("包含特别号"), 64);
	infd.m_vecFindItem[0][0][0].intItem = pDoc->m_bContainSpecial;

	infd.m_vecFindItem[0][1][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][1][0].caption, _T("开始期数"), 64);
	infd.m_vecFindItem[0][1][0].strItem.Format("%d", nMin );

	infd.m_vecFindItem[0][2][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][2][0].caption, _T("结束期数"), 64);
	infd.m_vecFindItem[0][2][0].strItem.Format("%d", nMax);

	

	infd.Init(_T("期数范围"), _T("期数范围"));
	if (infd.DoModal() == IDOK)
	{
		pDoc->m_bContainSpecial = infd.m_vecFindItem[0][0][0].intItem;
		m_start_number = atoi(infd.m_vecFindItem[0][1][0].strItem.GetBuffer());
		m_end_number = atoi(infd.m_vecFindItem[0][2][0].strItem.GetBuffer());;

		Startup();
	}
}



void CLotteryNumberGridBView::OnRegionNumberSet()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();
	
	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 20;
	for (int i = 0; i < 8; i++)
	{
		infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::CHECKBOX;
		if(i==0)	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("一区间"), 64);
		if (i == 1)	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("二区间"), 64);
		if (i == 2)	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("三区间"), 64);
		if (i == 3)	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("四区间"), 64);
		if (i == 4)	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("五区间"), 64);
		if (i == 5)	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("六区间"), 64);
		if (i == 6)	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("七区间"), 64);
		if (i == 7)	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("八区间"), 64);
		infd.m_vecFindItem[0][i][0].intItem = 0;

		infd.m_vecFindItem[0][i][1].nType = CDlgTemplateBuilder::SPIN_EDIT;
		memcpy(infd.m_vecFindItem[0][i][1].caption, _T("从"), 64);
		infd.m_vecFindItem[0][i][1].strItem = "";

		infd.m_vecFindItem[0][i][2].nType = CDlgTemplateBuilder::SPIN_EDIT;
		memcpy(infd.m_vecFindItem[0][i][2].caption, _T("到"), 64);
		infd.m_vecFindItem[0][i][2].strItem = "";
	}

	for (int i = 0; i < pDoc->m_region_start.size() && i < pDoc->m_region_start.size(); i++)
	{
		if (pDoc->m_region_end[i] > pDoc->m_region_start[i])
		{
			infd.m_vecFindItem[0][i][0].intItem = 1;
			infd.m_vecFindItem[0][i][1].strItem.Format("%d", pDoc->m_region_start[i]);
			infd.m_vecFindItem[0][i][2].strItem.Format("%d", pDoc->m_region_end[i]);
		}
	}
	
	infd.Init(_T("区间号码设置"), _T("区间号码设置"));
	if (infd.DoModal() == IDOK)
	{
		pDoc->m_region_start.clear();
		pDoc->m_region_end.clear();
		for (int i = 0; i < 8; i++)
		{
			if (infd.m_vecFindItem[0][i][0].intItem > 0)
			{
				int nStart = atoi(infd.m_vecFindItem[0][i][1].strItem.GetBuffer());
				int nEnd = atoi(infd.m_vecFindItem[0][i][2].strItem.GetBuffer());
				if (nStart > 0 && nEnd > nStart)
				{
					pDoc->m_region_start.push_back(nStart);
					pDoc->m_region_end.push_back(nEnd);
				}
			}
		}

		//pDoc->SaveDocumentIntoDB();
		pDoc->UpdateAllViews(this);
		Startup();
	}

}
