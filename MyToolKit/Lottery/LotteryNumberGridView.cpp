// LotteryNumberGridView.cpp: 实现文件
//

#include "stdafx.h"
#include "Lottery.h"
#include "LotteryDoc.h"
#include "MainFrm.h"
#include "LotteryNumberGridView.h"
#include "TrendDistributionGridView.h"
#include "CalcLotteryNumberList.h"
#include "CalcLotteryNumberEnum.h"

#include "DyncItemGroupDlg.h"
#include "GridDlg.h"
#include "ChartDlg.h"


#include "BasicExcel.hpp"
using namespace YExcel;

#include "Shellapi.h"

#include<iostream>  
#include<fstream> 

//////////////////////////////////////////////////////////////////////////
// 保存为 Excel 

class SaveAsExcelRequest {
public:
	BasicExcel  * pExcel;
	CString strFilepath;
};

UINT SaveAsExcelThreadFunc(LPVOID param)
{
	SaveAsExcelRequest  * pReq = (SaveAsExcelRequest  *)param;
	string strFilepath = pReq->strFilepath.GetBuffer();

	// 生成临时保存文件名称
	char buffer[64] = { 0 };
	GUID guid;
	if (CoCreateGuid(&guid))
	{
		AfxMessageBox("create guid error");
		return -1;
	}
	sprintf_s(buffer, sizeof(buffer),
		"%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X.xls",
		guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1], guid.Data4[2],
		guid.Data4[3], guid.Data4[4], guid.Data4[5],
		guid.Data4[6], guid.Data4[7]);
	
	/*vector<string> vec;
	splitString(strFilepath, ":\\", vec);
	string strTemppath;
	for (int i = 0; i < vec.size()-1; i++)
	{
		if(vec[i].empty()) continue;
		strTemppath += vec[i];
		if (i == 0)	strTemppath += ":";
		else strTemppath += "\\";
	}*/

	// 在当前目录 保存为临时文件
	pReq->pExcel->SaveAs(string(buffer).c_str());
	delete pReq->pExcel;
	delete pReq;

	//	拷贝临时文件，并且更名
	CopyFile(buffer, strFilepath.c_str(), false);
	DeleteFile(buffer);

	// 在资源管理器中弹出并且选中文件
	CString strParam;
	strParam.Format(" /select, %s", strFilepath.c_str());
	SHELLEXECUTEINFO shex = { 0 };
	shex.cbSize = sizeof(SHELLEXECUTEINFO);
	shex.lpFile  = _T("explorer");
	shex.lpParameters = strParam.GetBuffer();
	shex.lpVerb = NULL;  // _T("open");	
	shex.nShow = SW_SHOWDEFAULT;
	shex.lpDirectory = NULL;
	ShellExecuteEx(&shex);

	//AfxMessageBox(_T("Excel 文件保存成功"));
	return 0;
}


//////////////////////////////////////////////////////////////////////////
// CLotteryNumberGridView

IMPLEMENT_DYNCREATE(CLotteryNumberGridView, CBaseGridCtlView)

CLotteryNumberGridView::CLotteryNumberGridView()
{

}

CLotteryNumberGridView::~CLotteryNumberGridView()
{
}

BEGIN_MESSAGE_MAP(CLotteryNumberGridView, CBaseGridCtlView)
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_LOTTERY_RANGE_SET, &CLotteryNumberGridView::OnLotteryRangeSet)
	ON_COMMAND(ID_VAR_ANALYSIS, &CLotteryNumberGridView::OnVarAnalysis)
	ON_COMMAND(ID_STDEV_ANALYSIS, &CLotteryNumberGridView::OnStdevAnalysis)
	ON_COMMAND(ID_DEVSQ_ANALYSIS, &CLotteryNumberGridView::OnDevsqAnalysis)
	ON_COMMAND(ID_KURT_ANALYSIS, &CLotteryNumberGridView::OnKurtAnalysis)
	ON_COMMAND(ID_SKEW_ANALYSIS, &CLotteryNumberGridView::OnSkewAnalysis)
	ON_COMMAND(ID_SUM_ANALYSIS, &CLotteryNumberGridView::OnSumAnalysis)
	ON_COMMAND(ID_ODD_EVEN_ANALYSIS, &CLotteryNumberGridView::OnOddEvenAnalysis)
	ON_COMMAND(ID_CONSECUTIVE_NUMBER_ANALYSIS, &CLotteryNumberGridView::OnConsecutiveNumberAnalysis)
	ON_COMMAND(ID_FRED_PARAM_SET, &CLotteryNumberGridView::OnFredParamSet)
	ON_COMMAND(ID_REGION_NUMBER_SET, &CLotteryNumberGridView::OnRegionNumberSet)
	ON_COMMAND(ID_SAVE_AS_EXCEL, &CLotteryNumberGridView::OnSaveAsExcel)
	ON_COMMAND(ID_ADJACENT_NUMBER_ANALYSIS, &CLotteryNumberGridView::OnAdjacentNumberAnalysis)
	ON_COMMAND(ID_REGION_ANALYSIS, &CLotteryNumberGridView::OnRegionAnalysis)
	ON_COMMAND(ID_AC_ANALYSIS, &CLotteryNumberGridView::OnAcAnalysis)
	ON_COMMAND(ID_GROUP_QTY_ANALYSIS, &CLotteryNumberGridView::OnGroupQtyAnalysis)
	ON_COMMAND(ID_FRINGE_NO_QTY_ANALYSIS, &CLotteryNumberGridView::OnFringeNoQtyAnalysis)
	ON_COMMAND(ID_FANTASTIC_ANALYSIS, &CLotteryNumberGridView::OnFantasticAnalysis)
	ON_COMMAND(ID_RUN, &CLotteryNumberGridView::OnRun)
	ON_COMMAND(ID_BASIC_CHARACTER_SETUP, &CLotteryNumberGridView::OnBasicCharacterSetup)
	ON_COMMAND(ID_REGION_CHARACTER_SETUP, &CLotteryNumberGridView::OnRegionCharacterSetup)
	ON_COMMAND(ID_STATISTICS_CHARACTER_SETUP, &CLotteryNumberGridView::OnStatisticsCharacterSetup)
	ON_COMMAND(ID_ADVANCED_CHARACTER_SETUP, &CLotteryNumberGridView::OnAdvancedCharacterSetup)
	ON_COMMAND(ID_CONSECUTIVE_CHARACTER_SETUP, &CLotteryNumberGridView::OnConsecutiveCharacterSetup)
	ON_COMMAND(ID_TAIL_CHARACTER_SETUP, &CLotteryNumberGridView::OnTailCharacterSetup)
	ON_COMMAND(ID_LOTTERY_NUMBER_LIST, &CLotteryNumberGridView::OnLotteryNumberList)
	ON_COMMAND(ID_TREND_DISTRIBUTION_VIEW, &CLotteryNumberGridView::OnTrendDistributionView)
	ON_COMMAND(ID_LEAKAGE_AWARD_QUERY, &CLotteryNumberGridView::OnLeakageAwardQuery)
	ON_COMMAND(ID_WIN_LOTTERY_QUERY, &CLotteryNumberGridView::OnWinLotteryQuery)
	ON_COMMAND(ID_SYSTEM_FILTER_SETUP, &CLotteryNumberGridView::OnSystemFilterSetup)
	ON_COMMAND(ID_DATA_SOURCE_SETUP, &CLotteryNumberGridView::OnDataSourceSetup)
	ON_UPDATE_COMMAND_UI(ID_RUN, &CLotteryNumberGridView::OnUpdateRun)
	ON_COMMAND(ID_PAI_SETUP, &CLotteryNumberGridView::OnPaiSetup)
	ON_COMMAND(ID_LN_PAI_SETUP, &CLotteryNumberGridView::OnLnPaiSetup)
	ON_COMMAND(ID_SWING_POWER_SETUP, &CLotteryNumberGridView::OnSwingPowerSetup)
	ON_COMMAND(ID_LN_SWING_POWER_SETUP, &CLotteryNumberGridView::OnLnSwingPowerSetup)
	ON_COMMAND(ID_BERNOULLI_SETUP, &CLotteryNumberGridView::OnBernoulliSetup)
	ON_COMMAND(ID_POTENTIAL_SETUP, &CLotteryNumberGridView::OnPotentialSetup)
	ON_COMMAND(ID_PAI_ANALYSIS, &CLotteryNumberGridView::OnPaiAnalysis)
	ON_COMMAND(ID_FILTER_SOURCE_SETUP, &CLotteryNumberGridView::OnFilterSourceSetup)
	ON_COMMAND(ID_LN_PAI_ANALYSIS, &CLotteryNumberGridView::OnLnPaiAnalysis)
	ON_COMMAND(ID_SWING_POWER_ANALYSIS, &CLotteryNumberGridView::OnSwingPowerAnalysis)
	ON_COMMAND(ID_LN_SWING_POWER_ANALYSIS, &CLotteryNumberGridView::OnLnSwingPowerAnalysis)
	ON_COMMAND(ID_BERNOULLI_ANALYSIS, &CLotteryNumberGridView::OnBernoulliAnalysis)
	ON_COMMAND(ID_POTENTIAL_ANALYSIS, &CLotteryNumberGridView::OnPotentialAnalysis)
END_MESSAGE_MAP()


// CLotteryNumberGridView 绘图

void CLotteryNumberGridView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO:  在此添加绘制代码
}


// CLotteryNumberGridView 诊断

#ifdef _DEBUG
void CLotteryNumberGridView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CLotteryNumberGridView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CLotteryNumberGridView 消息处理程序


void CLotteryNumberGridView::OnInitialUpdate()
{
	CBaseGridCtlView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	
	//Startup();

}


void CLotteryNumberGridView::ProcessXmlWParam(string msgType, string code, string model, \
	string param1, string param2, string param3)
{
	if (msgType == "TaskFinished")
	{
		if (model == "CCalcLotteryNumberList")
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
}


void CLotteryNumberGridView::RedrawGridCtrl(CCalculateTask * cse)
{
	CCalcLotteryNumberList * pStockInfoEx = (CCalcLotteryNumberList *)cse;
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();

	LotteryInfo  info = pDoc->m_lottery_info;

	/*CString strTitle;
	strTitle.Format(_T("开奖号码表-%s"), info.name.c_str());
	this->GetParent()->SetWindowText(strTitle);*/

	if (m_pGridCtrl)
	{
		CLotteryApp * pApp = (CLotteryApp *)AfxGetApp();

		try {
			int nNum = 0;
			if (pDoc->m_region_1_start.size() > 0)	nNum++;
			if (pDoc->m_region_2_start.size() > 0)	nNum++;
			if (pDoc->m_region_3_start.size() > 0)	nNum++;
			if (pDoc->m_region_4_start.size() > 0)	nNum++;
			if (pDoc->m_region_5_start.size() > 0)	nNum++;
			if (pDoc->m_region_6_start.size() > 0)	nNum++;

			m_pGridCtrl->SetRowCount(pStockInfoEx->m_vec_lottery_number.size() + 1);
			m_pGridCtrl->SetColumnCount(32 + nNum);
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

		set<int>	setHideCol;
		for(int i = 1; i <= 8; i++)	setHideCol.insert(i);
		for(int i = 25; i <= 31; i++)	setHideCol.insert(i);



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
					else if (col == 8)	val = "NO.S";
					else if (col == 9) val = "SUM";
					else if (col == 10) val = "ODD";

					else if (col == 11) val = "EVEN";
					else if (col == 12) val = "相同尾数";
					else if (col == 13) val = "VAR";
					else if (col == 14) val = "STDEV";
					else if (col == 15) val = "DEVSQ";
					else if (col == 16) val = "KURT";
					else if (col == 17) val = "SKEW";

					else if (col == 18) val = "AC";
					else if (col == 19) val = "GROUP QTY";
					else if (col == 20) val = "Fringe No. QTY";
					else if (col == 21) val = "FRED";

					else if (col == 22) val = "间0连号列示";
					else if (col == 23) val = "间1连号列示";
					else if (col == 24) val = "间2连号列示";

					else if (col == 25) val = "NO.1-NO.2";
					else if (col == 26) val = "NO.2-NO.3";
					else if (col == 27) val = "NO.3-NO.4";
					else if (col == 28) val = "NO.4-NO.5";
					else if (col == 29) val = "NO.5-NO.6";
					else if (col == 30) val = "NO.6-NO.7";
					else if (col == 31) val = "NO.7-NO.8";

					else if (col >= 32)
					{
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
						int idx = col - 32;
						if (idx < region.size())
						{
							CString str;
							str.Format("区间 %d-%d", region[idx], region1[idx]);
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

					string val;
					if (col == 0)		val = Int2String(pStockInfoEx->m_vec_lottery_number[row - 1].periods);
					else if (col == 1)	val = Int2String(pStockInfoEx->m_vec_lottery_number[row - 1].number[0]);
					else if (col == 2)	val = Int2String(pStockInfoEx->m_vec_lottery_number[row - 1].number[1]);
					else if (col == 3)	val = Int2String(pStockInfoEx->m_vec_lottery_number[row - 1].number[2]);
					else if (col == 4)	val = Int2String(pStockInfoEx->m_vec_lottery_number[row - 1].number[3]);
					else if (col == 5)	val = Int2String(pStockInfoEx->m_vec_lottery_number[row - 1].number[4]);
					else if (col == 6)	val = Int2String(pStockInfoEx->m_vec_lottery_number[row - 1].number[5]);
					else if (col == 7)	val = Int2String(pStockInfoEx->m_vec_lottery_number[row - 1].number[6]);

					else if (col == 8)	val = Int2String(pStockInfoEx->m_vec_lottery_number[row - 1].special_number);
					else if (col == 9)	val = Int2String(pStockInfoEx->m_vec_lottery_number[row - 1].sum);
					else if (col == 10)	val = Int2String(pStockInfoEx->m_vec_lottery_number[row - 1].odd);
					else if (col == 11)	val = Int2String(pStockInfoEx->m_vec_lottery_number[row - 1].even);

					else if (col == 12)	val = pStockInfoEx->m_vec_lottery_number[row - 1].equal_tail;
					else if (col == 13)	val = Double2String(pStockInfoEx->m_vec_lottery_number[row - 1].var);
					else if (col == 14)	val = Double2String(pStockInfoEx->m_vec_lottery_number[row - 1].stdev);
					else if (col == 15)	val = Double2String(pStockInfoEx->m_vec_lottery_number[row - 1].devsq);
					else if (col == 16)	val = Double2String(pStockInfoEx->m_vec_lottery_number[row - 1].kurt);
					else if (col == 17)	val = Double2String(pStockInfoEx->m_vec_lottery_number[row - 1].skew);

					else if (col == 18)	val = Int2String(pStockInfoEx->m_vec_lottery_number[row - 1].ac);
					else if (col == 19)	val = Int2String(pStockInfoEx->m_vec_lottery_number[row - 1].group_qty);
					else if (col == 20)	val = Int2String(pStockInfoEx->m_vec_lottery_number[row - 1].fringe_no_qty);
					else if (col == 21)	val = Double2String(pStockInfoEx->m_vec_lottery_number[row - 1].fred);

					else if (col == 22)	val = consecutive_number_to_string(pStockInfoEx->m_vec_lottery_number[row - 1].Consecutive0);
					else if (col == 23)	val = consecutive_number_to_string(pStockInfoEx->m_vec_lottery_number[row - 1].Consecutive1);
					else if (col == 24)	val = consecutive_number_to_string(pStockInfoEx->m_vec_lottery_number[row - 1].Consecutive2);

					else if (col == 25)	val = Int2String(pStockInfoEx->m_vec_lottery_number[row - 1].adjacent_interval[0]);
					else if (col == 26)	val = Int2String(pStockInfoEx->m_vec_lottery_number[row - 1].adjacent_interval[1]);
					else if (col == 27)	val = Int2String(pStockInfoEx->m_vec_lottery_number[row - 1].adjacent_interval[2]);
					else if (col == 28)	val = Int2String(pStockInfoEx->m_vec_lottery_number[row - 1].adjacent_interval[3]);
					else if (col == 29)	val = Int2String(pStockInfoEx->m_vec_lottery_number[row - 1].adjacent_interval[4]);
					else if (col == 30)	val = Int2String(pStockInfoEx->m_vec_lottery_number[row - 1].adjacent_interval[5]);
					else if (col == 31)	val = Int2String(pStockInfoEx->m_vec_lottery_number[row - 1].adjacent_interval[6]);

					else if (col >= 32)
					{
						int idx = col - 32;
						val = Int2String(pStockInfoEx->m_vec_lottery_number[row - 1].region_count[idx]);
					}

					if (col >= 1 && col <= 8 || col >= 25 && col <= 31)
					{
						if (val == "0")
						{
							val = "";
						}
						else
							setHideCol.erase(col);
					}

					if (col >= 18 && col <= 21)
					{
						Item.mask |= (GVIF_FGCLR);
						Item.crFgClr = CColor::darkred;
					}

					Item.strText.Format(_T("%s"), val.c_str());

				}
				m_pGridCtrl->SetItem(&Item);
			}
			int k = 1;
		}

		//	保证第二次查询时，上次被隐藏的列纳入本次显示
		for (int i = 1; i <= 8; i++)	m_pGridCtrl->SetColumnWidth(i,1);
		for (int i = 25; i <= 31; i++)	m_pGridCtrl->SetColumnWidth(i,1);

		// 将无数据的 列隐藏
		set<int>::iterator it = setHideCol.begin();
		for (; it != setHideCol.end(); it++)
		{
			m_pGridCtrl->SetColumnWidth(*it, 0);
		}

		m_pGridCtrl->AutoSize();

		

		//m_pGridCtrl->SetColumnWidth(2, 80);
	}
}


void CLotteryNumberGridView::OnSetFocus(CWnd* pOldWnd)
{
	CBaseGridCtlView::OnSetFocus(pOldWnd);

	// TODO: 在此处添加消息处理程序代码
	RefreshStatusBar();
}

void CLotteryNumberGridView::PostGridDblClick(int nRw)
{
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();
	LotteryInfo  info = pDoc->m_lottery_info;

	if (m_vec_lottery_number.size() < 1)
		return;

	CString str = m_pGridCtrl->GetItemText(nRw, 0);
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

	//	保存被选中的 号码
	m_selected_lottery_number = m_vec_lottery_number[nIdx];

	int nNumPerLine = 6;
	CDyncItemGroupDlg infd;
	infd.CXCHECKBOX = 20;
	infd.CYCHECKBOX = 20;
	infd.CXSPACE = 3;
	infd.CYSPACE = 3;

	int nRow, nCol;
	for (int i = 1; i <= info.totalnum; i++)
	{
		nRow = (i - 1) / nNumPerLine;
		nCol = (i - 1) % nNumPerLine;
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

	infd.m_vecFindItem[0][nRow+1][0].nType = CDlgTemplateBuilder::RADIOBUTTON;
	infd.m_vecFindItem[0][nRow + 1][0].intItem = 0;
	infd.m_vecFindItem[0][nRow + 1][0].bKey = true;
	sprintf_s(infd.m_vecFindItem[0][nRow+1][0].caption, 64, _T("漏奖查询"));

	infd.m_vecFindItem[0][nRow + 1][1].nType = CDlgTemplateBuilder::RADIOBUTTON;
	sprintf_s(infd.m_vecFindItem[0][nRow + 1][1].caption, 64, _T("中奖查询"));

	infd.Init(_T("开奖号码框"), _T("开奖号码框"));
	if(infd.DoModal()!= IDOK)
		return;

	int number[10];
	for (int k = 0; k < info.regularnum; k++)
	{
		number[k] = m_vec_lottery_number[nIdx].number[k];
	}
	if (pDoc->m_bContainSpecial)
	{
		number[info.regularnum] = m_vec_lottery_number[nIdx].special_number;
	}

	if (infd.m_vecFindItem[0][nRow + 1][0].intItem==0)
	{
		LeakageAwardProcess(number);
	}

	if (infd.m_vecFindItem[0][nRow + 1][0].intItem==1)
	{
		WinLotteryProcess(number);
	}
	
}


void CLotteryNumberGridView::OnLotteryRangeSet()
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
	infd.CXCAPTION = 35;
	infd.m_vecFindItem[0][0][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[0][0][0].caption, _T("包含特别号"), 64);
	infd.m_vecFindItem[0][0][0].intItem = pDoc->m_bContainSpecial;

	infd.m_vecFindItem[0][1][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][1][0].caption, _T("开始期数"), 64);
	infd.m_vecFindItem[0][1][0].strItem.Format("%d", nMin );

	infd.m_vecFindItem[0][2][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][2][0].caption, _T("结束期数"), 64);
	infd.m_vecFindItem[0][2][0].strItem.Format("%d", nMax);

	

/*
	CDyncItemGroupDlg infd;
	infd.m_vecFindItem[0][0][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][0][0].caption, _T("开始期数"), 64);
	infd.m_vecFindItem[0][0][0].strItem = "缺省值1234";

	infd.m_vecFindItem[0][1][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][1][0].caption, _T("结束期数"), 64);

	infd.m_vecFindItem[0][1][1].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][1][1].caption, _T("结束期数123"), 64);

	infd.m_vecFindItem[0][2][0].nType = CDlgTemplateBuilder::COMBOBOX;
	infd.m_vecFindItem[0][2][0].strData = "Yes;No;>=";
	infd.m_vecFindItem[0][2][0].strItem = "No";
	memcpy(infd.m_vecFindItem[0][2][0].caption, _T("Combobox"), 64);

	infd.m_vecFindItem[0][3][0].nType = CDlgTemplateBuilder::CHECKBOX;
	infd.m_vecFindItem[0][3][0].dwStyle = BS_PUSHLIKE;
	memcpy(infd.m_vecFindItem[0][3][0].caption, _T("结数111"), 64);


	infd.m_vecFindItem[0][4][0].nType = CDlgTemplateBuilder::RADIOBUTTON;
	infd.m_vecFindItem[0][4][0].bKey = true;
	infd.m_vecFindItem[0][4][0].intItem = 1;
	memcpy(infd.m_vecFindItem[0][4][0].caption, _T("结数"), 64);

	infd.m_vecFindItem[0][4][1].nType = CDlgTemplateBuilder::RADIOBUTTON;
	memcpy(infd.m_vecFindItem[0][4][1].caption, _T("结数Radio"), 64);

		

	infd.m_vecFindItem[1][0][0].nType = CDlgTemplateBuilder::RADIOBUTTON;
	memcpy(infd.m_vecFindItem[1][0][0].caption, _T("开始期数"), 64);
	infd.m_vecFindItem[1][0][0].bKey = true;
	infd.m_vecFindItem[1][0][0].intItem = 1;

	infd.m_vecFindItem[1][1][0].nType = CDlgTemplateBuilder::RADIOBUTTON;
	memcpy(infd.m_vecFindItem[1][1][0].caption, _T("结束期数"), 64);

	infd.m_vecFindItem[1][2][0].nType = CDlgTemplateBuilder::RADIOBUTTON;
	memcpy(infd.m_vecFindItem[1][2][0].caption, _T("是否包含特别号"), 64);

	

	infd.m_vecFindItem[2][0][0].nType = CDlgTemplateBuilder::RADIOBUTTON;
	memcpy(infd.m_vecFindItem[2][0][0].caption, _T("开始期数"), 64);
	infd.m_vecFindItem[2][0][0].bKey = true;
	infd.m_vecFindItem[2][0][0].intItem = 1;

	infd.m_vecFindItem[2][1][0].nType = CDlgTemplateBuilder::RADIOBUTTON;
	memcpy(infd.m_vecFindItem[2][1][0].caption, _T("结束期数"), 64);

	infd.m_vecFindItem[2][1][1].nType = CDlgTemplateBuilder::ICON;
	memcpy(infd.m_vecFindItem[2][1][1].caption, _T("是否包含特别号"), 64);
	infd.m_vecFindItem[2][1][1].intItem = (WORD)IDI_QUESTION;

	infd.m_vecFindItem[2][2][0].nType = CDlgTemplateBuilder::RADIOBUTTON;
	memcpy(infd.m_vecFindItem[2][2][0].caption, _T("是否包含特别号"), 64);

	
	infd.m_vecFindItem[2][3][0].nType = CDlgTemplateBuilder::SPIN_EDIT;
	memcpy(infd.m_vecFindItem[2][3][0].caption, _T("SPIN_EDIT"), 64);
	infd.m_vecFindItem[2][3][0].strItem = "55";

		

	infd.m_vecFindItem[3][0][0].nType = CDlgTemplateBuilder::RADIOBUTTON;
	memcpy(infd.m_vecFindItem[3][0][0].caption, _T("开始期数"), 64);
	infd.m_vecFindItem[3][0][0].bKey = true;
	infd.m_vecFindItem[3][0][0].intItem = 1;

	infd.m_vecFindItem[3][1][0].nType = CDlgTemplateBuilder::RADIOBUTTON;
	memcpy(infd.m_vecFindItem[3][1][0].caption, _T("结束期数"), 64);

	infd.m_vecFindItem[3][1][1].nType = CDlgTemplateBuilder::ICON;
	memcpy(infd.m_vecFindItem[3][1][1].caption, _T("是否包含特别号"), 64);
	infd.m_vecFindItem[3][1][1].intItem = (WORD)IDI_QUESTION;

	infd.m_vecFindItem[3][2][0].nType = CDlgTemplateBuilder::RADIOBUTTON;
	memcpy(infd.m_vecFindItem[3][2][0].caption, _T("是否包含特别号"), 64);


	infd.m_vecFindItem[3][3][0].nType = CDlgTemplateBuilder::SPIN_EDIT;
	memcpy(infd.m_vecFindItem[3][3][0].caption, _T("SPIN_EDIT"), 64);
	infd.m_vecFindItem[3][3][0].strItem = "55";

		

	infd.m_vecFindItem[4][0][0].nType = CDlgTemplateBuilder::RADIOBUTTON;
	memcpy(infd.m_vecFindItem[4][0][0].caption, _T("开始期数"), 64);
	infd.m_vecFindItem[4][0][0].bKey = true;
	infd.m_vecFindItem[4][0][0].intItem = 1;

	infd.m_vecFindItem[4][1][0].nType = CDlgTemplateBuilder::RADIOBUTTON;
	memcpy(infd.m_vecFindItem[4][1][0].caption, _T("结束期数"), 64);

	infd.m_vecFindItem[4][2][0].nType = CDlgTemplateBuilder::RADIOBUTTON;
	memcpy(infd.m_vecFindItem[4][2][0].caption, _T("是否包含特别号"), 64);

		

	infd.m_vecFindItem[5][0][0].nType = CDlgTemplateBuilder::RADIOBUTTON;
	memcpy(infd.m_vecFindItem[5][0][0].caption, _T("开始期数"), 64);
	infd.m_vecFindItem[5][0][0].bKey = true;
	infd.m_vecFindItem[5][0][0].intItem = 1;

	infd.m_vecFindItem[5][1][0].nType = CDlgTemplateBuilder::RADIOBUTTON;
	memcpy(infd.m_vecFindItem[5][1][0].caption, _T("结束期数"), 64);

	infd.m_vecFindItem[5][2][0].nType = CDlgTemplateBuilder::RADIOBUTTON;
	memcpy(infd.m_vecFindItem[5][2][0].caption, _T("是否包含特别号"), 64);*/

	infd.Init(_T("期数范围"), _T("期数范围"));
	if (infd.DoModal() == IDOK)
	{
		pDoc->m_bContainSpecial = infd.m_vecFindItem[0][0][0].intItem;
		m_start_number = atoi(infd.m_vecFindItem[0][1][0].strItem.GetBuffer());
		m_end_number = atoi(infd.m_vecFindItem[0][2][0].strItem.GetBuffer());;

		Startup();
	}
}

//////////////////////////////////////////////////////////////////////////
void IntegerAnalysis(vector<int> vec, int interval, CString strCaption)
{
	if (vec.size() == 0)
		return;

	int nMin = 100000;
	int nMax = -100000;
	for (int i = 0; i < vec.size(); i++)
	{
		if (vec[i] > nMax)	nMax = vec[i];
		if (vec[i] < nMin)	nMin = vec[i];
	}

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 35;

	infd.m_vecFindItem[0][0][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][0][0].caption, _T("区间起始值"), 64);
	infd.m_vecFindItem[0][0][0].strItem.Format("%d", nMin);
	infd.m_vecFindItem[0][0][0].nMin = nMin - 1;
	infd.m_vecFindItem[0][0][0].nMax = nMax + 1;

	infd.m_vecFindItem[0][1][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][1][0].caption, _T("区间结束值"), 64);
	infd.m_vecFindItem[0][1][0].strItem.Format("%d", nMax);
	infd.m_vecFindItem[0][1][0].nMin = nMin - 1;
	infd.m_vecFindItem[0][1][0].nMax = nMax + 1;

	if (interval >= nMax -nMin || interval <=0)
	{
		interval = (nMax - nMin) / 3;
	}
	infd.m_vecFindItem[0][2][0].nType = CDlgTemplateBuilder::SPIN_EDIT;
	memcpy(infd.m_vecFindItem[0][2][0].caption, _T("区段间隔值"), 64);
	infd.m_vecFindItem[0][2][0].strItem.Format("%d", interval);
	infd.m_vecFindItem[0][2][0].nMin = 1;
	infd.m_vecFindItem[0][2][0].nMax = nMax;

	infd.Init(strCaption.GetBuffer(), strCaption.GetBuffer());
	if (infd.DoModal() == IDOK)
	{
		int nStart = atoi(infd.m_vecFindItem[0][0][0].strItem.GetBuffer());
		int nEnd = atoi(infd.m_vecFindItem[0][1][0].strItem.GetBuffer());
		int nSpan = atoi(infd.m_vecFindItem[0][2][0].strItem.GetBuffer());

		if (nEnd > nStart)
		{
			// 生成区段
			vector<int> vecSegmentStart;
			vector<int>	vecSegmentEnd;
			vector<int>	vecSegmentCount;

			int nCurrent = nStart;
			while (nCurrent < nEnd)
			{
				vecSegmentStart.push_back(nCurrent);
				vecSegmentEnd.push_back((nCurrent + nSpan - 1) > nEnd ? nEnd : (nCurrent + nSpan - 1));
				vecSegmentCount.push_back(0);
				nCurrent += nSpan;
			}

			//	汇总统计
			for (int i = 0; i < vec.size(); i++)
			{
				for (int k = 0; k < vecSegmentStart.size(); k++)
				{
					if (vec[i] >= vecSegmentStart[k] && vec[i] <= vecSegmentEnd[k])
					{
						vecSegmentCount[k]++;
						break;
					}
				}
			}

			//	展示
			CGridDlg gridDlg;
			gridDlg.m_vecHeader.push_back("区间");
			gridDlg.m_vecHeader.push_back("数量");
			gridDlg.m_vecHeader.push_back("比例（%）");

			for (int i = 0; i < vecSegmentStart.size(); i++)
			{
				string str = Int2String(vecSegmentStart[i]) + "-" + Int2String(vecSegmentEnd[i]);
				string str1 = Int2String(vecSegmentCount[i]);
				string str2 = Double2String((vecSegmentCount[i] * 1.0 / vec.size()) * 100.0);
				vector<string> vec;
				vec.push_back(str);
				vec.push_back(str1);
				vec.push_back(str2);
				gridDlg.m_vecData.push_back(vec);

			}

			gridDlg.DoModal();
		}

	}
}

void DoubleAnalysis(vector<double> vec, double interval, CString strCaption)
{
	if (vec.size() == 0)
		return; 

	double dbMin = 100000;
	double dbMax = -100000;
	for (int i = 0; i < vec.size(); i++)
	{
		if (vec[i] > dbMax)	dbMax = vec[i];
		if (vec[i] < dbMin)	dbMin = vec[i];
	}

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 35;
	infd.m_vecFindItem[0][0][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][0][0].caption, _T("区间起始值"), 64);
	infd.m_vecFindItem[0][0][0].strItem.Format("%.2f", dbMin);
	infd.m_vecFindItem[0][0][0].dbMin = dbMin - 0.1;
	infd.m_vecFindItem[0][0][0].dbMax = dbMax + 0.1;
	

	infd.m_vecFindItem[0][1][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][1][0].caption, _T("区间结束值"), 64);
	infd.m_vecFindItem[0][1][0].strItem.Format("%.2f", dbMax);
	infd.m_vecFindItem[0][1][0].dbMin = dbMin - 0.1;
	infd.m_vecFindItem[0][1][0].dbMax = dbMax + 0.1;
	

	infd.m_vecFindItem[0][2][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][2][0].caption, _T("区段间隔值"), 64);
	infd.m_vecFindItem[0][2][0].strItem.Format("%.2f", interval);
	infd.m_vecFindItem[0][2][0].dbMin = 0.01;
	infd.m_vecFindItem[0][2][0].dbMax = dbMax + 0.1;
	
	infd.Init(strCaption.GetBuffer(), strCaption.GetBuffer());
	if (infd.DoModal() == IDOK)
	{
		double dbStart, dbEnd, dbSpan;
		dbStart = String2Double(infd.m_vecFindItem[0][0][0].strItem.GetBuffer());
		dbEnd = String2Double(infd.m_vecFindItem[0][1][0].strItem.GetBuffer());
		dbSpan = String2Double(infd.m_vecFindItem[0][2][0].strItem.GetBuffer());
		

		if (dbEnd > dbStart)
		{
			// 生成区段
			vector<double> vecSegmentStart;
			vector<double>	vecSegmentEnd;
			vector<int>	vecSegmentCount;

			double dbCurrent = dbStart;
			while (dbCurrent < dbEnd)
			{
				vecSegmentStart.push_back(dbCurrent);
				vecSegmentEnd.push_back((dbCurrent + dbSpan) > dbEnd ? dbEnd : (dbCurrent + dbSpan));
				vecSegmentCount.push_back(0);
				dbCurrent += dbSpan;
			}

			//	汇总统计
			for (int i = 0; i < vec.size(); i++)
			{
				for (int k = 0; k < vecSegmentStart.size(); k++)
				{
					if (vec[i] >= vecSegmentStart[k] && vec[i] < vecSegmentEnd[k])
					{
						vecSegmentCount[k]++;
						break;
					}
				}
			}

			//	展示
			CGridDlg gridDlg;
			gridDlg.m_vecHeader.push_back("区段");
			gridDlg.m_vecHeader.push_back("数量");
			gridDlg.m_vecHeader.push_back("比例（%）");

			for (int i = 0; i < vecSegmentStart.size(); i++)
			{
				string str = Double2String(vecSegmentStart[i]) + "-" + Double2String(vecSegmentEnd[i]);
				string str1 = Int2String(vecSegmentCount[i]);
				string str2 = Double2String((vecSegmentCount[i] * 1.0 / vec.size()) * 100.0, "%.2f");
				vector<string> vec;
				vec.push_back(str);
				vec.push_back(str1);
				vec.push_back(str2);
				gridDlg.m_vecData.push_back(vec);

			}

			gridDlg.DoModal();
		}
	}
}


//////////////////////////////////////////////////////////////////////////
//	基本特征分析

void CLotteryNumberGridView::OnSumAnalysis()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();
	if (m_vec_lottery_number.size() < 1)
		return;

	vector<int> vec;
	for (int i = 0; i < m_vec_lottery_number.size(); i++)
		vec.push_back(m_vec_lottery_number[i].sum);

	IntegerAnalysis(vec, 10, _T("和值分析"));
}


void CLotteryNumberGridView::OnOddEvenAnalysis()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();

	CGridDlg gridDlg;
	gridDlg.m_vecHeader.push_back("Odd/Even");
	gridDlg.m_vecHeader.push_back("数量");
	gridDlg.m_vecHeader.push_back("比例（%）");

	map<string, int>  mapOddEven;
	for (int i = 0; i < m_vec_lottery_number.size(); i++)
	{
		string str = Int2String(m_vec_lottery_number[i].odd) + ":" + Int2String(m_vec_lottery_number[i].even);
		if (mapOddEven.count(str) > 0)
			mapOddEven[str]++;
		else
			mapOddEven[str] = 1;
	}

	map<string, int>::iterator it =  mapOddEven.begin();
	for (; it!=mapOddEven.end(); it++)
	{
		string str = it->first;
		string str1 = Int2String(it->second);
		string str2 = Double2String((it->second * 1.0 / m_vec_lottery_number.size()) * 100.0);
		vector<string> vec;
		vec.push_back(str);
		vec.push_back(str1);
		vec.push_back(str2);
		gridDlg.m_vecData.push_back(vec);
	}

	gridDlg.DoModal();

}


void CLotteryNumberGridView::OnConsecutiveNumberAnalysis()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();

	CGridDlg gridDlg;
	gridDlg.m_vecHeader.push_back("类型");
	gridDlg.m_vecHeader.push_back("数量");
	gridDlg.m_vecHeader.push_back("比例（%）");

	map<string, int>  mapConsecutive;
	for (int i = 0; i < m_vec_lottery_number.size(); i++)
	{
		for (int k = 0; k < 5; k++)
		{
			if (m_vec_lottery_number[i].Consecutive0[k][0] == 0) break;
			int j = 0;
			for (; j < 10 && m_vec_lottery_number[i].Consecutive0[k][j]; j++);

			string str = "间0连" + Int2String(j);
			if (mapConsecutive.count(str) > 0)
				mapConsecutive[str]++;
			else
				mapConsecutive[str] = 1;
		}
		
		for (int k = 0; k < 5; k++)
		{
			if (m_vec_lottery_number[i].Consecutive1[k][0] == 0) break;
			int j = 0;
			for (; j < 10 && m_vec_lottery_number[i].Consecutive1[k][j]; j++);

			string str = "间1连" + Int2String(j);
			if (mapConsecutive.count(str) > 0)
				mapConsecutive[str]++;
			else
				mapConsecutive[str] = 1;
		}

		for (int k = 0; k < 5; k++)
		{
			if (m_vec_lottery_number[i].Consecutive2[k][0] == 0) break;
			int j = 0;
			for (; j < 10 && m_vec_lottery_number[i].Consecutive2[k][j]; j++);

			string str = "间2连" + Int2String(j);
			if (mapConsecutive.count(str) > 0)
				mapConsecutive[str]++;
			else
				mapConsecutive[str] = 1;
		}
	}

	map<string, int>::iterator it = mapConsecutive.begin();
	for (; it != mapConsecutive.end(); it++)
	{
		string str = it->first;
		string str1 = Int2String(it->second);
		string str2 = Double2String((it->second * 1.0 / m_vec_lottery_number.size()) * 100.0);
		vector<string> vec;
		vec.push_back(str);
		vec.push_back(str1);
		vec.push_back(str2);
		gridDlg.m_vecData.push_back(vec);
	}

	gridDlg.DoModal();
}


void CLotteryNumberGridView::OnSaveAsExcel()
{
	// TODO: 在此添加命令处理程序代码

	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();

	vector<LotteryInfo>	vecInfo = ((CLotteryApp *)AfxGetApp())->m_vec_lottery_info;
	LotteryInfo info = pDoc->m_lottery_info;

	if (m_vec_lottery_number.size() < 1)
		return;

	// 选择需保存的列
	CDyncItemGroupDlg infd;
	infd.GROUP_NUM_PER_LINE = 4;
	//////////////////////////////////////////////////////////////////////////
	infd.m_vecFindItem[0][0][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[0][0][0].caption, _T("基本特征分析"), 64);
	infd.m_vecFindItem[0][0][0].dwStyle = BS_PUSHLIKE;
	infd.m_vecFindItem[0][0][0].intItem = 1;

	infd.m_vecFindItem[0][1][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[0][1][0].caption, _T("开奖数据"), 64);
	infd.m_vecFindItem[0][1][0].intItem = 1;

	infd.m_vecFindItem[0][2][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[0][2][0].caption, _T("和数值分析"), 64);
	infd.m_vecFindItem[0][2][0].intItem = 1;

	infd.m_vecFindItem[0][3][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[0][3][0].caption, _T("奇偶比例分析"), 64);
	infd.m_vecFindItem[0][3][0].intItem = 1;

	infd.m_vecFindItem[0][4][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[0][4][0].caption, _T("连号分析"), 64);
	infd.m_vecFindItem[0][4][0].intItem = 1;

	//////////////////////////////////////////////////////////////////////////
	infd.m_vecFindItem[1][0][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[1][0][0].caption, _T("区间特征分析"), 64);
	infd.m_vecFindItem[1][0][0].dwStyle = BS_PUSHLIKE;
	infd.m_vecFindItem[1][0][0].intItem = 1;

	infd.m_vecFindItem[1][1][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[1][1][0].caption, _T("相邻号码间隔分析"), 64);
	infd.m_vecFindItem[1][1][0].intItem = 1;

	infd.m_vecFindItem[1][2][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[1][2][0].caption, _T("各段设定区间号码数分析"), 64);
	infd.m_vecFindItem[1][2][0].intItem = 1;

	//////////////////////////////////////////////////////////////////////////
	infd.m_vecFindItem[2][0][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[2][0][0].caption, _T("统计特征分析"), 64);
	infd.m_vecFindItem[2][0][0].dwStyle = BS_PUSHLIKE;
	infd.m_vecFindItem[2][0][0].intItem = 1;

	infd.m_vecFindItem[2][1][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[2][1][0].caption, _T("样本方差分析"), 64);
	infd.m_vecFindItem[2][1][0].intItem = 1;

	infd.m_vecFindItem[2][2][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[2][2][0].caption, _T("标准偏差分析"), 64);
	infd.m_vecFindItem[2][2][0].intItem = 1;

	infd.m_vecFindItem[2][3][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[2][3][0].caption, _T("偏差平方和分析"), 64);
	infd.m_vecFindItem[2][3][0].intItem = 1;

	infd.m_vecFindItem[2][4][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[2][4][0].caption, _T("峰值分析"), 64);
	infd.m_vecFindItem[2][4][0].intItem = 1;

	infd.m_vecFindItem[2][5][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[2][5][0].caption, _T("斜峰度分析"), 64);
	infd.m_vecFindItem[2][5][0].intItem = 1;

	//////////////////////////////////////////////////////////////////////////
	infd.m_vecFindItem[3][0][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[3][0][0].caption, _T("高级特征分析"), 64);
	infd.m_vecFindItem[3][0][0].dwStyle = BS_PUSHLIKE;
	infd.m_vecFindItem[3][0][0].intItem = 1;

	infd.m_vecFindItem[3][1][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[3][1][0].caption, _T("AC 值分析"), 64);
	infd.m_vecFindItem[3][1][0].intItem = 1;

	infd.m_vecFindItem[3][2][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[3][2][0].caption, _T("集团数分析"), 64);
	infd.m_vecFindItem[3][2][0].intItem = 1;

	infd.m_vecFindItem[3][3][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[3][3][0].caption, _T("边缘号码数分析"), 64);
	infd.m_vecFindItem[3][3][0].intItem = 1;

	infd.m_vecFindItem[3][4][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[3][4][0].caption, _T("奇异选号公式分析"), 64);
	infd.m_vecFindItem[3][4][0].intItem = 1;

	//////////////////////////////////////////////////////////////////////////

	infd.Init(_T("点选生成Excel报告"), _T("点选生成Excel报告"));
	if(infd.DoModal()!=IDOK)
		return;

	CString  filter = _T("EXCEL文件|*.xls||");
	CFileDialog cfd(FALSE, _T("xls"), pDoc->m_lottery_info.name.c_str(),
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter);
	if (cfd.DoModal() == IDOK)
	{
		CString filepath = cfd.GetPathName();

		BasicExcel * e = new BasicExcel;
		e->New(1);		//	with 1 sheet

		e->RenameWorksheet("Sheet1", "Test1");
		BasicExcelWorksheet* sheet = e->GetWorksheet("Test1");
		BasicExcelCell* cell;
		if (sheet)
		{

			for (int row = 0; row < m_pGridCtrl->GetRowCount(); row++)
			{
				int nOutCol = 0;
				for (int col = 0; col < m_pGridCtrl->GetColumnCount(); col++)
				{
					bool bOuput = false;
					if (col == 0)
						bOuput = true;
					// 开奖数据
					if (col >= 1 && col <= 8 && infd.m_vecFindItem[0][0][0].intItem && infd.m_vecFindItem[0][1][0].intItem)
						bOuput = true;
					//	和值
					if (col == 9 && infd.m_vecFindItem[0][0][0].intItem && infd.m_vecFindItem[0][2][0].intItem)
						bOuput = true;
					//	奇偶分析
					if (col >= 10 && col <=11 && infd.m_vecFindItem[0][0][0].intItem && infd.m_vecFindItem[0][3][0].intItem)
						bOuput = true;
					//	连号分析
					if (col >= 22  && col <=24 && infd.m_vecFindItem[0][0][0].intItem && infd.m_vecFindItem[0][4][0].intItem)
						bOuput = true;

					//	相邻号码间隔分析
					if (col >= 25 && col <= 31 && infd.m_vecFindItem[1][0][0].intItem && infd.m_vecFindItem[1][1][0].intItem)
						bOuput = true;
					//	各段设定区间号码数分析
					if (col >= 32 && infd.m_vecFindItem[1][0][0].intItem && infd.m_vecFindItem[1][2][0].intItem)
						bOuput = true;

					// 样本方差分析
					if (col == 13 && infd.m_vecFindItem[2][0][0].intItem && infd.m_vecFindItem[2][1][0].intItem)
						bOuput = true;
					//	标准偏差分析
					if (col == 14 && infd.m_vecFindItem[2][0][0].intItem && infd.m_vecFindItem[2][2][0].intItem)
						bOuput = true;
					//	偏差平方和分析
					if (col == 15 && infd.m_vecFindItem[2][0][0].intItem && infd.m_vecFindItem[2][3][0].intItem)
						bOuput = true;
					//	峰值分析
					if (col == 16 && infd.m_vecFindItem[2][0][0].intItem && infd.m_vecFindItem[2][4][0].intItem)
						bOuput = true;
					//	斜峰度分析
					if (col == 17 && infd.m_vecFindItem[2][0][0].intItem && infd.m_vecFindItem[2][5][0].intItem)
						bOuput = true;


					// AC 值分析
					if (col == 18 && infd.m_vecFindItem[3][0][0].intItem && infd.m_vecFindItem[3][1][0].intItem)
						bOuput = true;
					//	集团数分析
					if (col == 19 && infd.m_vecFindItem[3][0][0].intItem && infd.m_vecFindItem[3][2][0].intItem)
						bOuput = true;
					//	边缘号码数分析
					if (col == 20 && infd.m_vecFindItem[3][0][0].intItem && infd.m_vecFindItem[3][3][0].intItem)
						bOuput = true;
					//	奇异选号公式分析
					if (col == 21 && infd.m_vecFindItem[3][0][0].intItem && infd.m_vecFindItem[3][4][0].intItem)
						bOuput = true;

					if (bOuput)
					{
						CString strItem = m_pGridCtrl->GetItemText(row, col);
						//sheet->Cell(row, col)->SetString(strItem.GetBuffer());
						string strUtf8 = Gbk2Utf8(strItem.GetBuffer());

						WCHAR tmp[512];
						if (MbcsToUnicode(strUtf8.c_str(), tmp, 512) > 0)
							sheet->Cell(row, nOutCol)->SetWString(tmp);

						nOutCol++;
					}
				}
			}

		}

		WCHAR tmp[512];
		string strFilepath = Gbk2Utf8(filepath.GetBuffer());
		//MbcsToUnicode(strFilepath.c_str(), tmp, 512)
		SaveAsExcelRequest * pReq = new SaveAsExcelRequest;
		pReq->pExcel = e;
		pReq->strFilepath = filepath;
		
		((CLotteryApp *)AfxGetApp())->StartWorkerThread(SaveAsExcelThreadFunc, pReq);
		
	}
}

//////////////////////////////////////////////////////////////////////////
// 统计特征分析


void CLotteryNumberGridView::OnVarAnalysis()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();
	if (m_vec_lottery_number.size() < 1)
		return;

	vector<double> vec;
	for (int i = 0; i < m_vec_lottery_number.size(); i++)
		vec.push_back(m_vec_lottery_number[i].var);

	DoubleAnalysis(vec, 0.1, _T("VAR 方差分析"));
}


void CLotteryNumberGridView::OnStdevAnalysis()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();
	if (m_vec_lottery_number.size() < 1)
		return;

	vector<double> vec;
	for (int i = 0; i < m_vec_lottery_number.size(); i++)
		vec.push_back(m_vec_lottery_number[i].stdev);

	DoubleAnalysis(vec, 0.1, _T("STDEV 分析"));
}


void CLotteryNumberGridView::OnDevsqAnalysis()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();
	if (m_vec_lottery_number.size() < 1)
		return;

	vector<double> vec;
	for (int i = 0; i < m_vec_lottery_number.size(); i++)
		vec.push_back(m_vec_lottery_number[i].devsq);

	DoubleAnalysis(vec, 0.1, _T("DEVSQ 分析"));
}


void CLotteryNumberGridView::OnKurtAnalysis()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();
	if (m_vec_lottery_number.size() < 1)
		return;

	vector<double> vec;
	for (int i = 0; i < m_vec_lottery_number.size(); i++)
		vec.push_back(m_vec_lottery_number[i].kurt);

	DoubleAnalysis(vec, 0.1, _T("Kurt 分析"));
}


void CLotteryNumberGridView::OnSkewAnalysis()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();
	if (m_vec_lottery_number.size() < 1)
		return;

	vector<double> vec;
	for (int i = 0; i < m_vec_lottery_number.size(); i++)
		vec.push_back(m_vec_lottery_number[i].skew);

	DoubleAnalysis(vec, 0.1, _T("Skew 分析"));
}


//////////////////////////////////////////////////////////////////////////
// 区间特征分析
void CLotteryNumberGridView::OnAdjacentNumberAnalysis()
{
	// TODO: 在此添加命令处理程序代码

	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();

	vector<LotteryInfo>	vecInfo = ((CLotteryApp *)AfxGetApp())->m_vec_lottery_info;
	LotteryInfo info = pDoc->m_lottery_info;

	if (m_vec_lottery_number.size() < 1)
		return;

	//	计算可能存在的相邻号码段数目
	int nNum = info.regularnum - 1;
	if (pDoc->m_bContainSpecial && info.specialnum)
		nNum++;
		
	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 35;
	infd.m_vecFindItem[0][0][0].nType = CDlgTemplateBuilder::RADIOBUTTON;
	memcpy(infd.m_vecFindItem[0][0][0].caption, "整体区段", 64);
	infd.m_vecFindItem[0][0][0].intItem = 1;
	infd.m_vecFindItem[0][0][0].bKey = true;

	for (int i = 1; i <= nNum; i++)
	{
		infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::RADIOBUTTON;
		sprintf_s(infd.m_vecFindItem[0][i][0].caption, 64, "No.%d-No.%d", i, i + 1);
		infd.m_vecFindItem[0][i][0].intItem = 0;
	}

	infd.m_vecFindItem[1][0][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[1][0][0].caption, _T("区间起始值"), 64);
	infd.m_vecFindItem[1][0][0].strItem.Format("%d", 1);
	infd.m_vecFindItem[1][0][0].nMin = 1;
	infd.m_vecFindItem[1][0][0].nMax = info.totalnum;

	infd.m_vecFindItem[1][1][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[1][1][0].caption, _T("区间结束值"), 64);
	infd.m_vecFindItem[1][1][0].strItem.Format("%d", info.totalnum);
	infd.m_vecFindItem[1][1][0].nMin = 1;
	infd.m_vecFindItem[1][1][0].nMax = info.totalnum;

	infd.m_vecFindItem[1][2][0].nType = CDlgTemplateBuilder::SPIN_EDIT;
	memcpy(infd.m_vecFindItem[1][2][0].caption, _T("区段间隔值"), 64);
	infd.m_vecFindItem[1][2][0].strItem = "1";
	infd.m_vecFindItem[1][2][0].nMin = 1;
	infd.m_vecFindItem[1][2][0].nMax = info.totalnum;

	infd.Init(_T("区间段选择"), _T("区间段选择"));
	if (infd.DoModal() == IDOK)
	{
		int nStart = atoi(infd.m_vecFindItem[1][0][0].strItem.GetBuffer());
		int nEnd = atoi(infd.m_vecFindItem[1][1][0].strItem.GetBuffer());
		int nSpan = atoi(infd.m_vecFindItem[1][2][0].strItem.GetBuffer());

		if (nEnd > nStart)
		{
			// 生成区段
			vector<int> vecSegmentStart;
			vector<int>	vecSegmentEnd;
			vector<vector<int>>	vecSegmentCount;
			for (int i = 0; i < nNum; i++)
			{
				vector<int>  vec;
				vecSegmentCount.push_back(vec);
			}

			int nCurrent = nStart;
			while (nCurrent < nEnd)
			{
				vecSegmentStart.push_back(nCurrent);
				vecSegmentEnd.push_back((nCurrent + nSpan - 1) > nEnd ? nEnd : (nCurrent + nSpan - 1));
				nCurrent += nSpan;
			}

			for (int i = 0; i < nNum; i++)
			{
				for(int k=0; k< vecSegmentStart.size(); k++)
					vecSegmentCount[i].push_back(0);
			}

			//	汇总统计
			for (int i = 0; i < m_vec_lottery_number.size(); i++)
			{
				for (int m = 0; m < nNum; m++)
				{
					for (int k = 0; k < vecSegmentStart.size(); k++)
					{
						if (m_vec_lottery_number[i].adjacent_interval[m] >= vecSegmentStart[k] &&
							m_vec_lottery_number[i].adjacent_interval[m] <= vecSegmentEnd[k])
						{
							vecSegmentCount[m][k]++;
							break;
						}
					}
				}
				
			}

			//	展示
			if (infd.m_vecFindItem[0][0][0].intItem == 0)
			{
				CGridDlg gridDlg;
				gridDlg.m_vecHeader.push_back("区间");
				for (int i = 0; i < nNum; i++)
				{
					CString str;
					str.Format("No.%d-No.%d",i + 1,i + 2);
					gridDlg.m_vecHeader.push_back(str.GetBuffer());
				}

				for (int i = 0; i < vecSegmentStart.size(); i++)
				{
					vector<string> vec;
					string str = Int2String(vecSegmentStart[i]) + "-" + Int2String(vecSegmentEnd[i]);
					vec.push_back(str);
					for (int m = 0; m < nNum; m++)
					{
						string str1 = Int2String(vecSegmentCount[m][i]);
						vec.push_back(str1);
					}
					gridDlg.m_vecData.push_back(vec);

				}
				gridDlg.DoModal();
			}
			else 
			{
				int m = infd.m_vecFindItem[0][0][0].intItem-1;
				CGridDlg gridDlg;
				gridDlg.m_vecHeader.push_back("区间");
				gridDlg.m_vecHeader.push_back("数量");
				gridDlg.m_vecHeader.push_back("比例（%）");

				for (int i = 0; i < vecSegmentStart.size(); i++)
				{
					string str = Int2String(vecSegmentStart[i]) + "-" + Int2String(vecSegmentEnd[i]);
					string str1 = Int2String(vecSegmentCount[m][i]);
					string str2 = Double2String((vecSegmentCount[m][i] * 1.0 / m_vec_lottery_number.size()) * 100.0);
					vector<string> vec;
					vec.push_back(str);
					vec.push_back(str1);
					vec.push_back(str2);
					gridDlg.m_vecData.push_back(vec);

				}

				gridDlg.DoModal();
			}
		}
	
	}

}


void CLotteryNumberGridView::OnRegionAnalysis()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();

	vector<LotteryInfo>	vecInfo = ((CLotteryApp *)AfxGetApp())->m_vec_lottery_info;
	LotteryInfo info = pDoc->m_lottery_info;

	if (m_vec_lottery_number.size() < 1)
		return;

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

	if (region.size() == 0)
	{
		AfxMessageBox(_T("未设置号码区段"));
		return;
	}

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 35;
	infd.m_vecFindItem[0][0][0].nType = CDlgTemplateBuilder::RADIOBUTTON;
	memcpy(infd.m_vecFindItem[0][0][0].caption, "整体区间", 64);
	infd.m_vecFindItem[0][0][0].intItem = 1;
	infd.m_vecFindItem[0][0][0].bKey = true;

	for (int i = 1; i <= region.size(); i++)
	{
		infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::RADIOBUTTON;
		sprintf_s(infd.m_vecFindItem[0][i][0].caption, 64, "区间 %d-%d", region[i-1],  region1[i-1]);
		infd.m_vecFindItem[0][i][0].intItem = 0;
	}


	infd.Init(_T("区间段选择"), _T("区间段选择"));
	if (infd.DoModal() == IDOK)
	{
		//	最多可能的球数
		int nNum = info.regularnum;
		if (pDoc->m_bContainSpecial && info.specialnum)
			nNum++;
		
		vector<vector<int>>	vecSegmentCount;
		for (int i = 0; i < region.size(); i++)
		{
			vector<int>  vec;
			vecSegmentCount.push_back(vec);
		}

		for (int i = 0; i <  region.size(); i++)
		{
			for (int k = 0; k <= nNum; k++)
				vecSegmentCount[i].push_back(0);
		}

		

		//	汇总统计
		for (int i = 0; i < m_vec_lottery_number.size(); i++)
		{
			for (int m = 0; m < region.size(); m++)
			{
				for (int k = 0; k <= nNum; k++) 
				{
					if (m_vec_lottery_number[i].region_count[m] == k)
					{
						vecSegmentCount[m][k]++;
						break;
					}
				}
			}

		}

		//	展示
		if (infd.m_vecFindItem[0][0][0].intItem == 0)
		{
			CGridDlg gridDlg;
			gridDlg.m_vecHeader.push_back("个数值");
			for (int i = 0; i <  region.size(); i++)
			{
				CString str;
				str.Format("区间 %d-%d", region[i],  region1[i]);
				gridDlg.m_vecHeader.push_back(str.GetBuffer());
			}

			for (int i = 0; i <=  nNum; i++)
			{
				vector<string> vec;
				string str = "value=" + Int2String(i);
				vec.push_back(str);
				for (int m = 0; m < region.size(); m++)
				{
					string str1 = Int2String(vecSegmentCount[m][i]);
					vec.push_back(str1);
				}
				gridDlg.m_vecData.push_back(vec);

			}
			gridDlg.DoModal();
		}
		else
		{
			int m = infd.m_vecFindItem[0][0][0].intItem - 1;
			CGridDlg gridDlg;
			gridDlg.m_vecHeader.push_back("个数值");
			gridDlg.m_vecHeader.push_back("数量");
			gridDlg.m_vecHeader.push_back("比例（%）");

			for (int i = 0; i <= nNum; i++)
			{
				string str = "value=" + Int2String(i);
				string str1 = Int2String(vecSegmentCount[m][i]);
				string str2 = Double2String((vecSegmentCount[m][i] * 1.0 / m_vec_lottery_number.size()) * 100.0);
				vector<string> vec;
				vec.push_back(str);
				vec.push_back(str1);
				vec.push_back(str2);
				gridDlg.m_vecData.push_back(vec);

			}

			gridDlg.DoModal();
		}

	}
}

//////////////////////////////////////////////////////////////////////////
//高级特征分析
void CLotteryNumberGridView::OnAcAnalysis()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();
	if (m_vec_lottery_number.size() < 1)
		return;

	vector<int> vec;
	for (int i = 0; i < m_vec_lottery_number.size(); i++)
		vec.push_back(m_vec_lottery_number[i].ac);

	IntegerAnalysis(vec, 10, _T("AC 值分析"));
}


void CLotteryNumberGridView::OnGroupQtyAnalysis()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();
	if (m_vec_lottery_number.size() < 1)
		return;

	vector<int> vec;
	for (int i = 0; i < m_vec_lottery_number.size(); i++)
		vec.push_back(m_vec_lottery_number[i].group_qty);

	IntegerAnalysis(vec, 10, _T("集团数分析"));
}


void CLotteryNumberGridView::OnFringeNoQtyAnalysis()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();
	if (m_vec_lottery_number.size() < 1)
		return;

	vector<int> vec;
	for (int i = 0; i < m_vec_lottery_number.size(); i++)
		vec.push_back(m_vec_lottery_number[i].fringe_no_qty);

	IntegerAnalysis(vec, 10, _T("边缘数分析"));
}


void CLotteryNumberGridView::OnFantasticAnalysis()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();
	if (m_vec_lottery_number.size() < 1)
		return;

	vector<double> vec;
	for (int i = 0; i < m_vec_lottery_number.size(); i++)
		vec.push_back(m_vec_lottery_number[i].fred);

	DoubleAnalysis(vec, 0.1, _T("Fred 分析"));
}


void CLotteryNumberGridView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO: 在此添加专用代码和/或调用基类
	Startup();
}


void CLotteryNumberGridView::OnRun()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();
		
	bool bSpecial = false;
	if (pDoc->m_bContainSpecial && pDoc->m_lottery_info.specialnum > 0)
		bSpecial = true;

	stringstream ss;
	ss.str("");
		
	if (pDoc->WhereSum().empty() == false)
		ss << pDoc->WhereSum();

	if (pDoc->WhereOdd().empty() == false)
		ss << pDoc->WhereOdd();
		
	if (pDoc->WhereAc().empty() == false)
		ss << pDoc->WhereAc();

	if (pDoc->WhereGrpQty().empty() == false)
		ss << pDoc->WhereGrpQty();

	if (pDoc->WhereFnQty().empty() == false)
		ss << pDoc->WhereFnQty();

	if (pDoc->WhereVar().empty() == false)
		ss << pDoc->WhereVar();

	if (pDoc->WhereDevsq().empty() == false)
		ss << pDoc->WhereDevsq();

	if (pDoc->WhereStdev().empty() == false)
		ss << pDoc->WhereStdev();

	if (pDoc->WhereKurt().empty() == false)
		ss << pDoc->WhereKurt();

	if (pDoc->WhereSkew().empty() == false)
		ss << pDoc->WhereSkew();

	if (pDoc->WhereAdjacentSum().empty() == false)
		ss << pDoc->WhereAdjacentSum();

	if (pDoc->WhereAdjacentTotality().empty() == false)
		ss << pDoc->WhereAdjacentTotality();

	if (pDoc->WhereAdjacentInterval().empty() == false)
		ss << pDoc->WhereAdjacentInterval();
	
	if (pDoc->WhereRegion().empty() == false)
		ss << pDoc->WhereRegion();
	
	CCalcLotteryNumberEnum * cse = new CCalcLotteryNumberEnum(m_hWnd, WM_USER_LOG_1);
	cse->m_lottery_sql = ss.str();

	cse->m_lottery_doc.m_bContainSpecial = pDoc->m_bContainSpecial;
	cse->m_lottery_doc.m_lottery_info = pDoc->m_lottery_info;
	cse->m_lottery_doc.m_strDataSource = pDoc->m_strDataSource;
	cse->m_lottery_doc.m_strFilterSource = pDoc->m_strFilterSource;

	cse->m_lottery_doc.m_consecutive_1 = pDoc->m_consecutive_1;
	cse->m_lottery_doc.m_consecutive_2 = pDoc->m_consecutive_2;
	cse->m_lottery_doc.m_consecutive_3 = pDoc->m_consecutive_3;
	cse->m_lottery_doc.m_consecutive_4 = pDoc->m_consecutive_4;
	cse->m_lottery_doc.m_consecutive_5 = pDoc->m_consecutive_5;
	cse->m_lottery_doc.m_consecutive_6 = pDoc->m_consecutive_6;

	cse->m_lottery_doc.m_tail_1 = pDoc->m_tail_1;
	cse->m_lottery_doc.m_tail_2 = pDoc->m_tail_2;
	cse->m_lottery_doc.m_tail_3 = pDoc->m_tail_3;
	cse->m_lottery_doc.m_tail_4 = pDoc->m_tail_4;
	cse->m_lottery_doc.m_tail_5 = pDoc->m_tail_5;
	cse->m_lottery_doc.m_tail_6 = pDoc->m_tail_6;

	cse->m_lottery_doc.m_fred_alpha = pDoc->m_fred_alpha;
	cse->m_lottery_doc.m_fred_beta = pDoc->m_fred_beta;
	cse->m_lottery_doc.m_fred_upsilon = pDoc->m_fred_upsilon;
	cse->m_lottery_doc.m_fred_theta = pDoc->m_fred_theta;
	cse->m_lottery_doc.m_fred_eta = pDoc->m_fred_eta;

	cse->m_lottery_doc.m_fred_start = pDoc->m_fred_start;
	cse->m_lottery_doc.m_fred_end = pDoc->m_fred_end;

	cse->m_lottery_doc.m_filter_equal_number = pDoc->m_filter_equal_number;
	cse->m_lottery_doc.m_filter_start = pDoc->m_filter_start;
	cse->m_lottery_doc.m_filter_end = pDoc->m_filter_end;

	cse->m_lottery_doc.m_pai_value = pDoc->m_pai_value;
	cse->m_lottery_doc.m_pai_start = pDoc->m_pai_start;
	cse->m_lottery_doc.m_pai_end = pDoc->m_pai_end;
	cse->m_lottery_doc.m_pai_analysis_periods = pDoc->m_pai_analysis_periods;
	cse->m_lottery_doc.m_pai_period_difference = pDoc->m_pai_period_difference;
	cse->m_lottery_doc.m_pai_mul_adjunstment = pDoc->m_pai_mul_adjunstment;
	cse->m_lottery_doc.m_pai_add_start = pDoc->m_pai_add_start;
	cse->m_lottery_doc.m_pai_add_end = pDoc->m_pai_add_end;
	cse->m_lottery_doc.m_pai_mul_start = pDoc->m_pai_mul_start;
	cse->m_lottery_doc.m_pai_mul_end = pDoc->m_pai_mul_end;

	cse->m_lottery_doc.m_ln_pai_value = pDoc->m_ln_pai_value;
	cse->m_lottery_doc.m_ln_pai_start = pDoc->m_ln_pai_start;
	cse->m_lottery_doc.m_ln_pai_end = pDoc->m_ln_pai_end;
	cse->m_lottery_doc.m_ln_pai_analysis_periods = pDoc->m_ln_pai_analysis_periods;
	cse->m_lottery_doc.m_ln_pai_period_difference = pDoc->m_ln_pai_period_difference;
	cse->m_lottery_doc.m_ln_pai_mul_adjunstment = pDoc->m_ln_pai_mul_adjunstment;
	cse->m_lottery_doc.m_ln_pai_add_start = pDoc->m_ln_pai_add_start;
	cse->m_lottery_doc.m_ln_pai_add_end = pDoc->m_ln_pai_add_end;
	cse->m_lottery_doc.m_ln_pai_mul_start = pDoc->m_ln_pai_mul_start;
	cse->m_lottery_doc.m_ln_pai_mul_end = pDoc->m_ln_pai_mul_end;

	cse->m_lottery_doc.m_swing_start = pDoc->m_swing_start;
	cse->m_lottery_doc.m_swing_end = pDoc->m_swing_end;
	cse->m_lottery_doc.m_swing_analysis_periods = pDoc->m_swing_analysis_periods;
	cse->m_lottery_doc.m_swing_mul_adjunstment = pDoc->m_swing_mul_adjunstment;
	cse->m_lottery_doc.m_swing_add_start = pDoc->m_swing_add_start;
	cse->m_lottery_doc.m_swing_add_end = pDoc->m_swing_add_end;
	cse->m_lottery_doc.m_swing_mul_start = pDoc->m_swing_mul_start;
	cse->m_lottery_doc.m_swing_mul_end = pDoc->m_swing_mul_end;

	cse->m_lottery_doc.m_ln_swing_start = pDoc->m_ln_swing_start;
	cse->m_lottery_doc.m_ln_swing_end = pDoc->m_ln_swing_end;
	cse->m_lottery_doc.m_ln_swing_analysis_periods = pDoc->m_ln_swing_analysis_periods;
	cse->m_lottery_doc.m_ln_swing_mul_adjunstment = pDoc->m_ln_swing_mul_adjunstment;
	cse->m_lottery_doc.m_ln_swing_add_start = pDoc->m_ln_swing_add_start;
	cse->m_lottery_doc.m_ln_swing_add_end = pDoc->m_ln_swing_add_end;
	cse->m_lottery_doc.m_ln_swing_mul_start = pDoc->m_ln_swing_mul_start;
	cse->m_lottery_doc.m_ln_swing_mul_end = pDoc->m_ln_swing_mul_end;

	cse->m_lottery_doc.m_bernoulli_start = pDoc->m_bernoulli_start;
	cse->m_lottery_doc.m_bernoulli_end = pDoc->m_bernoulli_end;
	cse->m_lottery_doc.m_bernoulli_analysis_periods = pDoc->m_bernoulli_analysis_periods;
	cse->m_lottery_doc.m_bernoulli_period_difference = pDoc->m_bernoulli_period_difference;
	cse->m_lottery_doc.m_bernoulli_mul_adjunstment = pDoc->m_bernoulli_mul_adjunstment;
	cse->m_lottery_doc.m_bernoulli_add_start = pDoc->m_bernoulli_add_start;
	cse->m_lottery_doc.m_bernoulli_add_end = pDoc->m_bernoulli_add_end;
	cse->m_lottery_doc.m_bernoulli_mul_start = pDoc->m_bernoulli_mul_start;
	cse->m_lottery_doc.m_bernoulli_mul_end = pDoc->m_bernoulli_mul_end;

	cse->m_lottery_doc.m_potential_start = pDoc->m_potential_start;
	cse->m_lottery_doc.m_potential_end = pDoc->m_potential_end;
	cse->m_lottery_doc.m_potential_analysis_periods = pDoc->m_potential_analysis_periods;
	cse->m_lottery_doc.m_potential_period_difference = pDoc->m_potential_period_difference;
	cse->m_lottery_doc.m_potential_mul_adjunstment = pDoc->m_potential_mul_adjunstment;
	cse->m_lottery_doc.m_potential_add_start = pDoc->m_potential_add_start;
	cse->m_lottery_doc.m_potential_add_end = pDoc->m_potential_add_end;
	cse->m_lottery_doc.m_potential_mul_start = pDoc->m_potential_mul_start;
	cse->m_lottery_doc.m_potential_mul_end = pDoc->m_potential_mul_end;
	cse->m_lottery_doc.m_potential_period_absolute = pDoc->m_potential_period_absolute;


	// 打开 “保存文件”对话框
	CString  filter = _T("csv文件|*.csv||");
	CFileDialog cfd(FALSE, _T("csv"), pDoc->m_lottery_info.name.c_str(),
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter);
	if (cfd.DoModal() == IDOK)
	{
		theApp.m_bRunning = TRUE;
		CString filepath = cfd.GetPathName();
		cse->m_file_path = filepath.GetBuffer();
		cse->ScheduleTask(&CCalcLotteryNumberEnum::Execute1);
	}
}

//////////////////////////////////////////////////////////////////////////
//	设置各种特征值


void CLotteryNumberGridView::OnFredParamSet()
{
	// TODO: 在此添加命令处理程序代码

	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 20;
	for (int i = 0; i < 4; i++)
	{
		infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::RADIOBUTTON;
		if (i == 0)	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("FRED1 参数"), 64);
		if (i == 1)	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("FRED2 参数"), 64);
		if (i == 2)	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("FRED3 参数"), 64);
		if (i == 3)	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("FRED4 参数"), 64);
		infd.m_vecFindItem[0][i][0].intItem = 0;

		infd.m_vecFindItem[0][i][1].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[0][i][1].caption, _T("α"), 64);
		infd.m_vecFindItem[0][i][1].strItem = "3.4113";
		infd.m_vecFindItem[0][i][1].dbMin = -10;
		infd.m_vecFindItem[0][i][1].dbMax = 10;

		infd.m_vecFindItem[0][i][2].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[0][i][2].caption, _T("β"), 64);
		infd.m_vecFindItem[0][i][2].strItem = "0.0238";
		infd.m_vecFindItem[0][i][2].dbMin = -10;
		infd.m_vecFindItem[0][i][2].dbMax = 10;

		infd.m_vecFindItem[0][i][3].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[0][i][3].caption, _T("γ"), 64);
		infd.m_vecFindItem[0][i][3].strItem = "0.7255";
		infd.m_vecFindItem[0][i][3].dbMin = -10;
		infd.m_vecFindItem[0][i][3].dbMax = 10;

		infd.m_vecFindItem[0][i][4].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[0][i][4].caption, _T("θ"), 64);
		infd.m_vecFindItem[0][i][4].strItem = "0.0771";
		infd.m_vecFindItem[0][i][4].dbMin = -10;
		infd.m_vecFindItem[0][i][4].dbMax = 10;

		infd.m_vecFindItem[0][i][5].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[0][i][5].caption, _T("η"), 64);
		infd.m_vecFindItem[0][i][5].strItem = "-0.0403";
		infd.m_vecFindItem[0][i][5].dbMin = -10;
		infd.m_vecFindItem[0][i][5].dbMax = 10;
	}

	for (int i = 0; i < pDoc->m_fred_alpha.size() && i < pDoc->m_fred_beta.size() && i < 4; i++)
	{
		infd.m_vecFindItem[0][i][1].strItem.Format("%.4f", pDoc->m_fred_alpha[i]);
		infd.m_vecFindItem[0][i][2].strItem.Format("%.4f", pDoc->m_fred_beta[i]);
		infd.m_vecFindItem[0][i][3].strItem.Format("%.4f", pDoc->m_fred_upsilon[i]);
		infd.m_vecFindItem[0][i][4].strItem.Format("%.4f", pDoc->m_fred_theta[i]);
		infd.m_vecFindItem[0][i][5].strItem.Format("%.4f", pDoc->m_fred_eta[i]);
	}

	//	第一个 Radio 为主
	infd.m_vecFindItem[0][0][0].bKey = true;
	if (pDoc->m_fred_select >= 0 && pDoc->m_fred_select < 4)
		infd.m_vecFindItem[0][0][0].intItem = pDoc->m_fred_select;

	infd.Init(_T("Fred 参数设置"), _T("Fred 参数设置"));
	if (infd.DoModal() == IDOK)
	{
		pDoc->m_fred_alpha.clear();
		pDoc->m_fred_beta.clear();
		pDoc->m_fred_upsilon.clear();
		pDoc->m_fred_theta.clear();
		pDoc->m_fred_eta.clear();

		for (int i = 0; i < 4; i++)
		{
			double dbAlpha = String2Double(infd.m_vecFindItem[0][i][1].strItem.GetBuffer());
			double dbBeta = String2Double(infd.m_vecFindItem[0][i][2].strItem.GetBuffer());
			double dbUpsilon = String2Double(infd.m_vecFindItem[0][i][3].strItem.GetBuffer());
			double dbTheta = String2Double(infd.m_vecFindItem[0][i][4].strItem.GetBuffer());
			double dbEta = String2Double(infd.m_vecFindItem[0][i][5].strItem.GetBuffer());

			pDoc->m_fred_alpha.push_back(dbAlpha);
			pDoc->m_fred_beta.push_back(dbBeta);
			pDoc->m_fred_upsilon.push_back(dbUpsilon);
			pDoc->m_fred_theta.push_back(dbTheta);
			pDoc->m_fred_eta.push_back(dbEta);
		}

		pDoc->m_fred_select = infd.m_vecFindItem[0][0][0].intItem;
		//pDoc->SaveDocumentIntoDB();
		pDoc->UpdateAllViews(this);

		Startup();
	}
}

void CLotteryNumberGridView::OnRegionNumberSet()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();
	LotteryInfo info = pDoc->m_lottery_info;

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 20;
	infd.CXEDIT = 20;
	infd.CXCHECKBOX = 50;
	infd.GROUP_NUM_PER_LINE = 3;

	for (int k = 0; k < 6; k++)
	{
		vector<int>	vec, vec1;
		if (k == 0) { vec = pDoc->m_region_1_start;   vec1 = pDoc->m_region_1_end; }
		if (k == 1) { vec = pDoc->m_region_2_start;   vec1 = pDoc->m_region_2_end; }
		if (k == 2) { vec = pDoc->m_region_3_start;   vec1 = pDoc->m_region_3_end; }
		if (k == 3) { vec = pDoc->m_region_4_start;   vec1 = pDoc->m_region_4_end; }
		if (k == 4) { vec = pDoc->m_region_5_start;   vec1 = pDoc->m_region_5_end; }
		if (k == 5) { vec = pDoc->m_region_6_start;   vec1 = pDoc->m_region_6_end; }

		infd.m_vecFindItem[k][0][0].nType = CDlgTemplateBuilder::CHECKBOX;
		memcpy(infd.m_vecFindItem[k][0][0].caption, _T("区间设置"), 64);
		infd.m_vecFindItem[k][0][0].dwStyle = BS_PUSHLIKE;
		if (vec.size() > 0)	infd.m_vecFindItem[k][0][0].intItem = 1;
		else 		infd.m_vecFindItem[k][0][0].intItem = 0;

		infd.m_vecFindItem[k][0][1].nType = CDlgTemplateBuilder::SPIN_EDIT;
		memcpy(infd.m_vecFindItem[k][0][1].caption, _T("范围从"), 64);
		infd.m_vecFindItem[k][0][1].strItem = "2";
		infd.m_vecFindItem[k][0][1].nMin = 1;
		infd.m_vecFindItem[k][0][1].nMax = info.totalnum;

		infd.m_vecFindItem[k][0][2].nType = CDlgTemplateBuilder::SPIN_EDIT;
		memcpy(infd.m_vecFindItem[k][0][2].caption, _T("到"), 64);
		infd.m_vecFindItem[k][0][2].strItem = "2";
		infd.m_vecFindItem[k][0][2].nMin = 1;
		infd.m_vecFindItem[k][0][2].nMax = info.totalnum;

		if (vec.size()>0)
		{
			infd.m_vecFindItem[k][0][1].strItem.Format("%d", vec[0]);
			infd.m_vecFindItem[k][0][2].strItem.Format("%d", vec1[0]);
		}

		for (int i = 1; i <= 3; i++)
		{
			infd.m_vecFindItem[k][i][0].nType = CDlgTemplateBuilder::CHECKBOX;
			if (i == 1)	memcpy(infd.m_vecFindItem[k][i][0].caption, _T("个数设定"), 64);
			if (i == 2)	memcpy(infd.m_vecFindItem[k][i][0].caption, _T("个数设定"), 64);
			if (i == 3)	memcpy(infd.m_vecFindItem[k][i][0].caption, _T("个数设定"), 64);
			infd.m_vecFindItem[k][i][0].intItem = 0;

			infd.m_vecFindItem[k][i][1].nType = CDlgTemplateBuilder::SPIN_EDIT;
			memcpy(infd.m_vecFindItem[k][i][1].caption, _T("从"), 64);
			infd.m_vecFindItem[k][i][1].strItem = "2";
			infd.m_vecFindItem[k][i][1].nMin = 0;
			infd.m_vecFindItem[k][i][1].nMax = 8;

			infd.m_vecFindItem[k][i][2].nType = CDlgTemplateBuilder::SPIN_EDIT;
			memcpy(infd.m_vecFindItem[k][i][2].caption, _T("到"), 64);
			infd.m_vecFindItem[k][i][2].strItem = "2";
			infd.m_vecFindItem[k][i][2].nMin = 0;
			infd.m_vecFindItem[k][i][2].nMax = 8;

			if (i < vec.size())
			{
				infd.m_vecFindItem[k][i][0].intItem = 1;
				infd.m_vecFindItem[k][i][1].strItem.Format("%d", vec[i]);
				infd.m_vecFindItem[k][i][2].strItem.Format("%d", vec1[i]);
			}
		}
	}

	infd.Init(_T("区段号码设置"), _T("区段号码设置"));
	if (infd.DoModal() != IDOK)
		return;

	for (int k = 0; k < 6; k++)
	{
		vector<int> vec, vec1;
		if (infd.m_vecFindItem[k][0][0].intItem)
		{
			vec.push_back(atoi(infd.m_vecFindItem[k][0][1].strItem.GetBuffer()));
			vec1.push_back(atoi(infd.m_vecFindItem[k][0][2].strItem.GetBuffer()));
			for (int i = 1; i <= 3; i++)
			{
				if (infd.m_vecFindItem[k][i][0].intItem)
				{
					vec.push_back(atoi(infd.m_vecFindItem[k][i][1].strItem.GetBuffer()));
					vec1.push_back(atoi(infd.m_vecFindItem[k][i][2].strItem.GetBuffer()));
				}
			}
		}

		if (k == 0) { pDoc->m_region_1_start = vec;  pDoc->m_region_1_end = vec1; }
		if (k == 1) { pDoc->m_region_2_start = vec;  pDoc->m_region_2_end = vec1; }
		if (k == 2) { pDoc->m_region_3_start = vec;  pDoc->m_region_3_end = vec1; }
		if (k == 3) { pDoc->m_region_4_start = vec;  pDoc->m_region_4_end = vec1; }
		if (k == 4) { pDoc->m_region_5_start = vec;  pDoc->m_region_5_end = vec1; }
		if (k == 5) { pDoc->m_region_6_start = vec;  pDoc->m_region_6_end = vec1; }

	}
	pDoc->SetModifiedFlag();

	pDoc->UpdateAllViews(this);
	Startup();
}

void CLotteryNumberGridView::OnBasicCharacterSetup()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();

	LotteryInfo info = pDoc->m_lottery_info;

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 35;
	infd.GROUP_NUM_PER_LINE = 2;
	//////////////////////////////////////////////////////////////////////////
	infd.m_vecFindItem[0][0][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[0][0][0].caption, _T("和数设置"), 64);
	infd.m_vecFindItem[0][0][0].dwStyle = BS_PUSHLIKE;
	if (pDoc->m_sum_start.size() > 0)	infd.m_vecFindItem[0][0][0].intItem = 1;
	else 		infd.m_vecFindItem[0][0][0].intItem = 0;

	for (int i = 1; i <= 4; i++)
	{
		infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::CHECKBOX;
		if (i == 1)	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("SUM="), 64);
		if (i == 2)	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("或 SUM="), 64);
		if (i == 3)	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("或 SUM="), 64);
		if (i == 4)	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("或 SUM="), 64);
		infd.m_vecFindItem[0][i][0].intItem = 0;

		infd.m_vecFindItem[0][i][1].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[0][i][1].caption, _T("从"), 64);
		infd.m_vecFindItem[0][i][1].strItem = "";

		infd.m_vecFindItem[0][i][2].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[0][i][2].caption, _T("到"), 64);
		infd.m_vecFindItem[0][i][2].strItem = "";

		int num = pDoc->m_sum_start.size();
		if (i <= num)
		{
			infd.m_vecFindItem[0][i][0].intItem = 1;
			infd.m_vecFindItem[0][i][1].strItem.Format("%d", pDoc->m_sum_start[i - 1]);
			infd.m_vecFindItem[0][i][2].strItem.Format("%d", pDoc->m_sum_end[i - 1]);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	int nOdd = info.regularnum;
	if (pDoc->m_bContainSpecial && info.specialnum)
		nOdd++;

	infd.m_vecFindItem[1][0][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[1][0][0].caption, _T("奇偶比例设置"), 64);
	infd.m_vecFindItem[1][0][0].dwStyle = BS_PUSHLIKE;
	if (pDoc->m_odd.size() > 0)	infd.m_vecFindItem[1][0][0].intItem = 1;
	else 		infd.m_vecFindItem[1][0][0].intItem = 0;

	for (int i = 1; i <= nOdd + 1; i++)
	{
		infd.m_vecFindItem[1][i][0].nType = CDlgTemplateBuilder::CHECKBOX;
		sprintf_s(infd.m_vecFindItem[1][i][0].caption, 64, "%d 奇 %d 偶", i-1, nOdd + 1 -i );
		infd.m_vecFindItem[1][i][0].intItem = 0;

		for (int k = 0; k < pDoc->m_odd.size(); k++)
		{
			if (pDoc->m_odd[k] == i - 1)
			{
				infd.m_vecFindItem[1][i][0].intItem = 1;
				break;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////

	infd.Init(_T("基本特征设置"), _T("基本特征设置"));
	if (infd.DoModal() != IDOK)
		return;

	pDoc->m_sum_start.clear();
	pDoc->m_sum_end.clear();
	if (infd.m_vecFindItem[0][0][0].intItem)
	{
		for (int i = 1; i <= 4; i++)
		{
			if (infd.m_vecFindItem[0][i][0].intItem)
			{
				pDoc->m_sum_start.push_back(atoi(infd.m_vecFindItem[0][i][1].strItem.GetBuffer()));
				pDoc->m_sum_end.push_back(atoi(infd.m_vecFindItem[0][i][2].strItem.GetBuffer()));
			}
		}
	}

	pDoc->m_odd.clear();
	if (infd.m_vecFindItem[1][0][0].intItem)
	{
		for (int i = 1; i <= nOdd; i++)
		{
			if (infd.m_vecFindItem[1][i][0].intItem)
			{
				pDoc->m_odd.push_back(i -1);
			}
		}
	}
	pDoc->SetModifiedFlag();
}


void CLotteryNumberGridView::OnConsecutiveCharacterSetup()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();

	LotteryInfo info = pDoc->m_lottery_info;

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 35;
	infd.GROUP_NUM_PER_LINE = 3;

	for (int k = 0; k < 6; k++)
	{
		map<int, int>	vec;
		if (k == 0)	vec = pDoc->m_consecutive_1;
		if (k == 1)	vec = pDoc->m_consecutive_2;
		if (k == 2)	vec = pDoc->m_consecutive_3;
		if (k == 3)	vec = pDoc->m_consecutive_4;
		if (k == 4)	vec = pDoc->m_consecutive_5;
		if (k == 5)	vec = pDoc->m_consecutive_6;

		infd.m_vecFindItem[k][0][0].nType = CDlgTemplateBuilder::CHECKBOX;
		memcpy(infd.m_vecFindItem[k][0][0].caption, _T("连号设置"), 64);
		infd.m_vecFindItem[k][0][0].dwStyle = BS_PUSHLIKE;
		if (vec.size() > 0)	infd.m_vecFindItem[k][0][0].intItem = 1;
		else 		infd.m_vecFindItem[k][0][0].intItem = 0;

		for (int i = 1; i <= 8; i++)
		{
			infd.m_vecFindItem[k][i][0].nType = CDlgTemplateBuilder::CHECKBOX;
			if (i == 1)	memcpy(infd.m_vecFindItem[k][i][0].caption, _T("间0 连2"), 64);
			if (i == 2)	memcpy(infd.m_vecFindItem[k][i][0].caption, _T("且 间0 连3"), 64);
			if (i == 3)	memcpy(infd.m_vecFindItem[k][i][0].caption, _T("且 间0 连4"), 64);
			if (i == 4)	memcpy(infd.m_vecFindItem[k][i][0].caption, _T("且 间0 连5"), 64);
			if (i == 5)	memcpy(infd.m_vecFindItem[k][i][0].caption, _T("且 间1 连2"), 64);
			if (i == 6)	memcpy(infd.m_vecFindItem[k][i][0].caption, _T("且 间1 连3"), 64);
			if (i == 7)	memcpy(infd.m_vecFindItem[k][i][0].caption, _T("且 间2 连2"), 64);
			if (i == 8)	memcpy(infd.m_vecFindItem[k][i][0].caption, _T("且 间2 连3"), 64);
			infd.m_vecFindItem[k][i][0].intItem = 0;

			infd.m_vecFindItem[k][i][1].nType = CDlgTemplateBuilder::SPIN_EDIT;
			memcpy(infd.m_vecFindItem[k][i][1].caption, _T(""), 64);
			infd.m_vecFindItem[k][i][1].strItem = "0";
			infd.m_vecFindItem[k][i][1].nMin = 0;
			infd.m_vecFindItem[k][i][1].nMax = 3;

			if (vec.count(i) > 0)
			{
				infd.m_vecFindItem[k][i][0].intItem = 1;
				infd.m_vecFindItem[k][i][1].strItem.Format("%d", vec[i]);
			}
		}
	}

	infd.Init(_T("连号设置"), _T("连号设置"));
	if (infd.DoModal() != IDOK)
		return;

	for (int k = 0; k < 6; k++)
	{
		map<int, int > vec;
		if (infd.m_vecFindItem[k][0][0].intItem)
		{
			for (int i = 1; i <= 8; i++)
			{
				if (infd.m_vecFindItem[k][i][0].intItem)
				{
					vec[i] = atoi(infd.m_vecFindItem[k][i][1].strItem.GetBuffer());
				}
			}
		}

		if (k == 0)	pDoc->m_consecutive_1 = vec;
		if (k == 1)	pDoc->m_consecutive_2 = vec;
		if (k == 2)	pDoc->m_consecutive_3 = vec;
		if (k == 3)	pDoc->m_consecutive_4 = vec;
		if (k == 4)	pDoc->m_consecutive_5 = vec;
		if (k == 5)	pDoc->m_consecutive_6 = vec;

	}
	pDoc->SetModifiedFlag();
}


void CLotteryNumberGridView::OnTailCharacterSetup()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();

	LotteryInfo info = pDoc->m_lottery_info;

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 35;
	infd.GROUP_NUM_PER_LINE = 3;
	infd.CXCHECKBOX = 70;

	for (int k = 0; k < 6; k++)
	{
		map<int, int>	vec;
		if (k == 0)	vec = pDoc->m_tail_1;
		if (k == 1)	vec = pDoc->m_tail_2;
		if (k == 2)	vec = pDoc->m_tail_3;
		if (k == 3)	vec = pDoc->m_tail_4;
		if (k == 4)	vec = pDoc->m_tail_5;
		if (k == 5)	vec = pDoc->m_tail_6;
		
		infd.m_vecFindItem[k][0][0].nType = CDlgTemplateBuilder::CHECKBOX;
		memcpy(infd.m_vecFindItem[k][0][0].caption, _T("尾数设置"), 64);
		infd.m_vecFindItem[k][0][0].dwStyle = BS_PUSHLIKE;
		if (vec.size() > 0)	infd.m_vecFindItem[k][0][0].intItem = 1;
		else 		infd.m_vecFindItem[k][0][0].intItem = 0;

		for (int i = 1; i <= 3; i++)
		{
			infd.m_vecFindItem[k][i][0].nType = CDlgTemplateBuilder::CHECKBOX;
			if (i == 1)	memcpy(infd.m_vecFindItem[k][i][0].caption, _T("一类同尾数"), 64);
			if (i == 2)	memcpy(infd.m_vecFindItem[k][i][0].caption, _T("且 二类同尾数"), 64);
			if (i == 3)	memcpy(infd.m_vecFindItem[k][i][0].caption, _T("且 三类同尾数"), 64);
			infd.m_vecFindItem[k][i][0].intItem = 0;

			infd.m_vecFindItem[k][i][1].nType = CDlgTemplateBuilder::SPIN_EDIT;
			memcpy(infd.m_vecFindItem[k][i][1].caption, _T(""), 64);
			infd.m_vecFindItem[k][i][1].strItem = "2";
			infd.m_vecFindItem[k][i][1].nMin = 2;
			infd.m_vecFindItem[k][i][1].nMax = 4;

			if (vec.count(i) > 0)
			{
				infd.m_vecFindItem[k][i][0].intItem = 1;
				infd.m_vecFindItem[k][i][1].strItem.Format("%d", vec[i]);
			}
		}
	}

	infd.Init(_T("尾数设置"), _T("尾数设置"));
	if (infd.DoModal() != IDOK)
		return;

	for (int k = 0; k < 6; k++)
	{
		map<int, int > vec;
		if (infd.m_vecFindItem[k][0][0].intItem)
		{
			for (int i = 1; i <= 3; i++)
			{
				if (infd.m_vecFindItem[k][i][0].intItem)
				{
					vec[i] = atoi(infd.m_vecFindItem[k][i][1].strItem.GetBuffer());
				}
			}
		}

		if (k == 0)	pDoc->m_tail_1 = vec;
		if (k == 1)	pDoc->m_tail_2 = vec;
		if (k == 2)	pDoc->m_tail_3 = vec;
		if (k == 3)	pDoc->m_tail_4 = vec;
		if (k == 4)	pDoc->m_tail_5 = vec;
		if (k == 5)	pDoc->m_tail_6 = vec;
	}
	pDoc->SetModifiedFlag();
}


void CLotteryNumberGridView::OnRegionCharacterSetup()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();

	LotteryInfo info = pDoc->m_lottery_info;

	// 选择需保存的列
	CDyncItemGroupDlg infd;
	infd.CXCHECKBOX = 50;
	infd.CXCOMBOX = 30;
	infd.CXEDIT = 30;
	infd.GROUP_NUM_PER_LINE = 3;
	//////////////////////////////////////////////////////////////////////////
	infd.m_vecFindItem[0][0][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[0][0][0].caption, _T("相邻号码- 总体设置"), 64);
	infd.m_vecFindItem[0][0][0].dwStyle = BS_PUSHLIKE;
	if(pDoc->m_adjacent_totality_1.size()>0)		infd.m_vecFindItem[0][0][0].intItem = 1;
	else infd.m_vecFindItem[0][0][0].intItem = 0;

	for (int k = 1; k <= 3; k++)
	{
		infd.m_vecFindItem[0][k][0].nType = CDlgTemplateBuilder::COMBOBOX;
		infd.m_vecFindItem[0][k][0].strData = "无;=;>;>=;<;<=";
		infd.m_vecFindItem[0][k][0].strItem = "无";
		sprintf_s(infd.m_vecFindItem[0][k][0].caption, 64, k == 1 ? _T("") : _T("或"), k, k + 1);

		infd.m_vecFindItem[0][k][1].nType = CDlgTemplateBuilder::SPIN_EDIT;
		memcpy(infd.m_vecFindItem[0][k][1].caption, _T(""), 64);
		infd.m_vecFindItem[0][k][1].strItem = "";

		infd.m_vecFindItem[0][k][2].nType = CDlgTemplateBuilder::COMBOBOX;
		infd.m_vecFindItem[0][k][2].strData = "=;>;>=;<;<=";
		infd.m_vecFindItem[0][k][2].strItem = "=";
		sprintf_s(infd.m_vecFindItem[0][k][2].caption, 64, "个", k, k + 1);

		infd.m_vecFindItem[0][k][3].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[0][k][3].caption, _T(""), 64);
		infd.m_vecFindItem[0][k][3].strItem = "";

		if (pDoc->m_adjacent_totality_1.size() > 0)
		{
			infd.m_vecFindItem[0][k][0].strItem.Format("%s",  pDoc->m_adjacent_totality_1[k - 1].c_str());
			infd.m_vecFindItem[0][k][1].strItem.Format("%d", pDoc->m_adjacent_totality_2[k - 1]);
			infd.m_vecFindItem[0][k][2].strItem.Format("%s",  pDoc->m_adjacent_totality_3[k - 1].c_str());
			infd.m_vecFindItem[0][k][3].strItem.Format("%d", pDoc->m_adjacent_totality_4[k - 1]);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	infd.m_vecFindItem[1][0][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[1][0][0].caption, _T("相邻号码- 分区设置"), 64);
	infd.m_vecFindItem[1][0][0].dwStyle = BS_PUSHLIKE;
	if (pDoc->m_adjacent_zone_1.size() > 0)		infd.m_vecFindItem[1][0][0].intItem = 1;
	else infd.m_vecFindItem[1][0][0].intItem = 0;

	for (int k = 1; k <= 7; k++)
	{
		infd.m_vecFindItem[1][k][0].nType = CDlgTemplateBuilder::STATIC;
		sprintf_s(infd.m_vecFindItem[1][k][0].caption, 64, "NO.%d ~ NO.%d", k, k + 1);

		infd.m_vecFindItem[1][k][1].nType = CDlgTemplateBuilder::COMBOBOX;
		infd.m_vecFindItem[1][k][1].strData = "无;=;>;>=;<;<=";
		infd.m_vecFindItem[1][k][1].strItem = "无";
		sprintf_s(infd.m_vecFindItem[1][k][1].caption,  64, "",  k, k+1);

		infd.m_vecFindItem[1][k][2].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[1][k][2].caption, _T(""), 64);
		infd.m_vecFindItem[1][k][2].strItem = "";

		infd.m_vecFindItem[1][k][3].nType = CDlgTemplateBuilder::COMBOBOX;
		infd.m_vecFindItem[1][k][3].strData = "无;=;>;>=;<;<=";
		infd.m_vecFindItem[1][k][3].strItem = "无";
		sprintf_s(infd.m_vecFindItem[1][k][3].caption, 64, "且", k, k + 1);

		infd.m_vecFindItem[1][k][4].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[1][k][4].caption, _T(""), 64);
		infd.m_vecFindItem[1][k][4].strItem = "";

		if (pDoc->m_adjacent_zone_1.size() > 0)
		{
			infd.m_vecFindItem[1][k][1].strItem.Format("%s", pDoc->m_adjacent_zone_1[k - 1].c_str());
			infd.m_vecFindItem[1][k][2].strItem.Format("%d", pDoc->m_adjacent_zone_2[k - 1]);
			infd.m_vecFindItem[1][k][3].strItem.Format("%s", pDoc->m_adjacent_zone_3[k - 1].c_str());
			infd.m_vecFindItem[1][k][4].strItem.Format("%d", pDoc->m_adjacent_zone_4[k - 1]);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	infd.m_vecFindItem[2][0][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[2][0][0].caption, _T("相邻号码- 间隔数和设置"), 64);
	infd.m_vecFindItem[2][0][0].dwStyle = BS_PUSHLIKE;
	if (pDoc->m_adjacent_sum_1.size() > 0)		infd.m_vecFindItem[2][0][0].intItem = 1;
	else infd.m_vecFindItem[2][0][0].intItem = 0;

	for (int k = 1; k <= 3; k++)
	{
		infd.m_vecFindItem[2][k][0].nType = CDlgTemplateBuilder::COMBOBOX;
		infd.m_vecFindItem[2][k][0].strData = "无;=;>;>=;<;<=";
		infd.m_vecFindItem[2][k][0].strItem = "无";
		memcpy(infd.m_vecFindItem[2][k][0].caption, k==1?  _T("") : _T("或"), 64);

		infd.m_vecFindItem[2][k][1].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[2][k][1].caption, _T(""), 64);
		infd.m_vecFindItem[2][k][1].strItem = "";

		if (pDoc->m_adjacent_sum_1.size() > 0)
		{
			infd.m_vecFindItem[2][k][0].strItem.Format("%s", pDoc->m_adjacent_sum_1[k - 1].c_str());
			infd.m_vecFindItem[2][k][1].strItem.Format("%d", pDoc->m_adjacent_sum_2[k - 1]);
		}
	}

	//////////////////////////////////////////////////////////////////////////

	infd.Init(_T("区间特征设置"), _T("区间特征设置"));
	if (infd.DoModal() != IDOK)
		return;

	pDoc->m_adjacent_totality_1.clear();
	pDoc->m_adjacent_totality_2.clear();
	pDoc->m_adjacent_totality_3.clear();
	pDoc->m_adjacent_totality_4.clear();
	if (infd.m_vecFindItem[0][0][0].intItem)
	{
		for (int i = 1; i <= 3; i++)
		{
			pDoc->m_adjacent_totality_1.push_back(infd.m_vecFindItem[0][i][0].strItem.GetBuffer());
			pDoc->m_adjacent_totality_2.push_back(atoi(infd.m_vecFindItem[0][i][1].strItem.GetBuffer()));
			pDoc->m_adjacent_totality_3.push_back(infd.m_vecFindItem[0][i][2].strItem.GetBuffer());
			pDoc->m_adjacent_totality_4.push_back(atoi(infd.m_vecFindItem[0][i][3].strItem.GetBuffer()));
		}
	}

	pDoc->m_adjacent_zone_1.clear();
	pDoc->m_adjacent_zone_2.clear();
	pDoc->m_adjacent_zone_3.clear();
	pDoc->m_adjacent_zone_4.clear();
	if (infd.m_vecFindItem[1][0][0].intItem)
	{
		for (int i = 1; i <= 7; i++)
		{
			pDoc->m_adjacent_zone_1.push_back(infd.m_vecFindItem[1][i][1].strItem.GetBuffer());
			pDoc->m_adjacent_zone_2.push_back(atoi(infd.m_vecFindItem[1][i][2].strItem.GetBuffer()));
			pDoc->m_adjacent_zone_3.push_back(infd.m_vecFindItem[1][i][3].strItem.GetBuffer());
			pDoc->m_adjacent_zone_4.push_back(atoi(infd.m_vecFindItem[1][i][4].strItem.GetBuffer()));
		}
	}

	pDoc->m_adjacent_sum_1.clear();
	pDoc->m_adjacent_sum_2.clear();
	if (infd.m_vecFindItem[2][0][0].intItem)
	{
		for (int i = 1; i <= 3; i++)
		{
			pDoc->m_adjacent_sum_1.push_back(infd.m_vecFindItem[2][i][0].strItem.GetBuffer());
			pDoc->m_adjacent_sum_2.push_back(atoi(infd.m_vecFindItem[2][i][1].strItem.GetBuffer()));
		}
	}
	pDoc->SetModifiedFlag();
}


void CLotteryNumberGridView::OnStatisticsCharacterSetup()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();

	LotteryInfo info = pDoc->m_lottery_info;

	// 选择需保存的列
	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 10;
	infd.CXEDIT = 40;
	infd.GROUP_NUM_PER_LINE = 3;
	//////////////////////////////////////////////////////////////////////////
	infd.m_vecFindItem[0][0][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[0][0][0].caption, _T("样本方差设置"), 64);
	infd.m_vecFindItem[0][0][0].dwStyle = BS_PUSHLIKE;
	if (pDoc->m_var_start.size() > 0)	infd.m_vecFindItem[0][0][0].intItem = 1;
	else 		infd.m_vecFindItem[0][0][0].intItem = 0;

	for (int i = 1; i <= 4; i++)
	{
		infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::CHECKBOX;
		if (i == 1)	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("VAR="), 64);
		if (i == 2)	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("或 VAR="), 64);
		if (i == 3)	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("或 VAR="), 64);
		if (i == 4)	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("或 VAR="), 64);
		infd.m_vecFindItem[0][i][0].intItem = 0;

		infd.m_vecFindItem[0][i][1].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[0][i][1].caption, _T("从"), 64);
		infd.m_vecFindItem[0][i][1].strItem = "";

		infd.m_vecFindItem[0][i][2].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[0][i][2].caption, _T("到"), 64);
		infd.m_vecFindItem[0][i][2].strItem = "";

		int num = pDoc->m_var_start.size();
		if (i <= num)
		{
			infd.m_vecFindItem[0][i][0].intItem = 1;
			infd.m_vecFindItem[0][i][1].strItem.Format("%.2f", pDoc->m_var_start[i - 1]);
			infd.m_vecFindItem[0][i][2].strItem.Format("%.2f", pDoc->m_var_end[i - 1]);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	infd.m_vecFindItem[1][0][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[1][0][0].caption, _T("偏差平方和设置"), 64);
	infd.m_vecFindItem[1][0][0].dwStyle = BS_PUSHLIKE;
	if (pDoc->m_devsq_start.size() > 0)	infd.m_vecFindItem[1][0][0].intItem = 1;
	else 		infd.m_vecFindItem[1][0][0].intItem = 0;

	for (int i = 1; i <= 4; i++)
	{
		infd.m_vecFindItem[1][i][0].nType = CDlgTemplateBuilder::CHECKBOX;
		if (i == 1)	memcpy(infd.m_vecFindItem[1][i][0].caption, _T("DEVSQ="), 64);
		if (i == 2)	memcpy(infd.m_vecFindItem[1][i][0].caption, _T("或 DEVSQ="), 64);
		if (i == 3)	memcpy(infd.m_vecFindItem[1][i][0].caption, _T("或 DEVSQ="), 64);
		if (i == 4)	memcpy(infd.m_vecFindItem[1][i][0].caption, _T("或 DEVSQ="), 64);
		infd.m_vecFindItem[1][i][0].intItem = 0;

		infd.m_vecFindItem[1][i][1].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[1][i][1].caption, _T("从"), 64);
		infd.m_vecFindItem[1][i][1].strItem = "";

		infd.m_vecFindItem[1][i][2].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[1][i][2].caption, _T("到"), 64);
		infd.m_vecFindItem[1][i][2].strItem = "";

		int num = pDoc->m_devsq_start.size();
		if (i <= num)
		{
			infd.m_vecFindItem[1][i][0].intItem = 1;
			infd.m_vecFindItem[1][i][1].strItem.Format("%.2f", pDoc->m_devsq_start[i - 1]);
			infd.m_vecFindItem[1][i][2].strItem.Format("%.2f", pDoc->m_devsq_end[i - 1]);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	infd.m_vecFindItem[2][0][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[2][0][0].caption, _T("标准偏差设置"), 64);
	infd.m_vecFindItem[2][0][0].dwStyle = BS_PUSHLIKE;
	if (pDoc->m_stdev_start.size() > 0)	infd.m_vecFindItem[2][0][0].intItem = 1;
	else 		infd.m_vecFindItem[2][0][0].intItem = 0;

	for (int i = 1; i <= 4; i++)
	{
		infd.m_vecFindItem[2][i][0].nType = CDlgTemplateBuilder::CHECKBOX;
		if (i == 1)	memcpy(infd.m_vecFindItem[2][i][0].caption, _T("STDEV="), 64);
		if (i == 2)	memcpy(infd.m_vecFindItem[2][i][0].caption, _T("或 STDEV="), 64);
		if (i == 3)	memcpy(infd.m_vecFindItem[2][i][0].caption, _T("或 STDEV="), 64);
		if (i == 4)	memcpy(infd.m_vecFindItem[2][i][0].caption, _T("或 STDEV="), 64);
		infd.m_vecFindItem[2][i][0].intItem = 0;

		infd.m_vecFindItem[2][i][1].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[2][i][1].caption, _T("从"), 64);
		infd.m_vecFindItem[2][i][1].strItem = "";

		infd.m_vecFindItem[2][i][2].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[2][i][2].caption, _T("到"), 64);
		infd.m_vecFindItem[2][i][2].strItem = "";

		int num = pDoc->m_stdev_start.size();
		if (i <= num)
		{
			infd.m_vecFindItem[2][i][0].intItem = 1;
			infd.m_vecFindItem[2][i][1].strItem.Format("%.2f", pDoc->m_stdev_start[i - 1]);
			infd.m_vecFindItem[2][i][2].strItem.Format("%.2f", pDoc->m_stdev_end[i - 1]);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	infd.m_vecFindItem[3][0][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[3][0][0].caption, _T("峰值设置"), 64);
	infd.m_vecFindItem[3][0][0].dwStyle = BS_PUSHLIKE;
	if (pDoc->m_kurt_start.size() > 0)	infd.m_vecFindItem[3][0][0].intItem = 1;
	else 		infd.m_vecFindItem[3][0][0].intItem = 0;

	for (int i = 1; i <= 4; i++)
	{
		infd.m_vecFindItem[3][i][0].nType = CDlgTemplateBuilder::CHECKBOX;
		if (i == 1)	memcpy(infd.m_vecFindItem[3][i][0].caption, _T("KURT="), 64);
		if (i == 2)	memcpy(infd.m_vecFindItem[3][i][0].caption, _T("或 KURT="), 64);
		if (i == 3)	memcpy(infd.m_vecFindItem[3][i][0].caption, _T("或 KURT="), 64);
		if (i == 4)	memcpy(infd.m_vecFindItem[3][i][0].caption, _T("或 KURT="), 64);
		infd.m_vecFindItem[3][i][0].intItem = 0;

		infd.m_vecFindItem[3][i][1].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[3][i][1].caption, _T("从"), 64);
		infd.m_vecFindItem[3][i][1].strItem = "";

		infd.m_vecFindItem[3][i][2].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[3][i][2].caption, _T("到"), 64);
		infd.m_vecFindItem[3][i][2].strItem = "";

		int num = pDoc->m_kurt_start.size();
		if (i <= num)
		{
			infd.m_vecFindItem[3][i][0].intItem = 1;
			infd.m_vecFindItem[3][i][1].strItem.Format("%.2f", pDoc->m_kurt_start[i - 1]);
			infd.m_vecFindItem[3][i][2].strItem.Format("%.2f", pDoc->m_kurt_end[i - 1]);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	infd.m_vecFindItem[4][0][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[4][0][0].caption, _T("斜峰度设置"), 64);
	infd.m_vecFindItem[4][0][0].dwStyle = BS_PUSHLIKE;
	if (pDoc->m_skew_start.size() > 0)	infd.m_vecFindItem[4][0][0].intItem = 1;
	else 		infd.m_vecFindItem[4][0][0].intItem = 0;

	for (int i = 1; i <= 4; i++)
	{
		infd.m_vecFindItem[4][i][0].nType = CDlgTemplateBuilder::CHECKBOX;
		if (i == 1)	memcpy(infd.m_vecFindItem[4][i][0].caption, _T("SKEW="), 64);
		if (i == 2)	memcpy(infd.m_vecFindItem[4][i][0].caption, _T("或 SKEW="), 64);
		if (i == 3)	memcpy(infd.m_vecFindItem[4][i][0].caption, _T("或 SKEW="), 64);
		if (i == 4)	memcpy(infd.m_vecFindItem[4][i][0].caption, _T("或 SKEW="), 64);
		infd.m_vecFindItem[4][i][0].intItem = 0;

		infd.m_vecFindItem[4][i][1].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[4][i][1].caption, _T("从"), 64);
		infd.m_vecFindItem[4][i][1].strItem = "";

		infd.m_vecFindItem[4][i][2].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[4][i][2].caption, _T("到"), 64);
		infd.m_vecFindItem[4][i][2].strItem = "";

		int num = pDoc->m_skew_start.size();
		if (i <= num)
		{
			infd.m_vecFindItem[4][i][0].intItem = 1;
			infd.m_vecFindItem[4][i][1].strItem.Format("%.2f", pDoc->m_skew_start[i - 1]);
			infd.m_vecFindItem[4][i][2].strItem.Format("%.2f", pDoc->m_skew_end[i - 1]);
		}
	}

	//////////////////////////////////////////////////////////////////////////

	infd.Init(_T("统计特征设置"), _T("统计特征设置"));
	if (infd.DoModal() != IDOK)
		return;

	pDoc->m_var_start.clear();
	pDoc->m_var_end.clear();
	if (infd.m_vecFindItem[0][0][0].intItem)
	{
		for (int i = 1; i <= 4; i++)
		{
			if (infd.m_vecFindItem[0][i][0].intItem)
			{
				pDoc->m_var_start.push_back(String2Double(infd.m_vecFindItem[0][i][1].strItem.GetBuffer()));
				pDoc->m_var_end.push_back(String2Double(infd.m_vecFindItem[0][i][2].strItem.GetBuffer()));
			}
		}
	}

	pDoc->m_devsq_start.clear();
	pDoc->m_devsq_end.clear();
	if (infd.m_vecFindItem[1][0][0].intItem)
	{
		for (int i = 1; i <= 4; i++)
		{
			if (infd.m_vecFindItem[1][i][0].intItem)
			{
				pDoc->m_devsq_start.push_back(String2Double(infd.m_vecFindItem[1][i][1].strItem.GetBuffer()));
				pDoc->m_devsq_end.push_back(String2Double(infd.m_vecFindItem[1][i][2].strItem.GetBuffer()));
			}
		}
	}

	pDoc->m_stdev_start.clear();
	pDoc->m_stdev_end.clear();
	if (infd.m_vecFindItem[2][0][0].intItem)
	{
		for (int i = 1; i <= 4; i++)
		{
			if (infd.m_vecFindItem[2][i][0].intItem)
			{
				pDoc->m_stdev_start.push_back(String2Double(infd.m_vecFindItem[2][i][1].strItem.GetBuffer()));
				pDoc->m_stdev_end.push_back(String2Double(infd.m_vecFindItem[2][i][2].strItem.GetBuffer()));
			}
		}
	}

	pDoc->m_kurt_start.clear();
	pDoc->m_kurt_end.clear();
	if (infd.m_vecFindItem[3][0][0].intItem)
	{
		for (int i = 1; i <= 4; i++)
		{
			if (infd.m_vecFindItem[3][i][0].intItem)
			{
				pDoc->m_kurt_start.push_back(String2Double(infd.m_vecFindItem[3][i][1].strItem.GetBuffer()));
				pDoc->m_kurt_end.push_back(String2Double(infd.m_vecFindItem[3][i][2].strItem.GetBuffer()));
			}
		}
	}

	pDoc->m_skew_start.clear();
	pDoc->m_skew_end.clear();
	if (infd.m_vecFindItem[4][0][0].intItem)
	{
		for (int i = 1; i <= 4; i++)
		{
			if (infd.m_vecFindItem[4][i][0].intItem)
			{
				pDoc->m_skew_start.push_back(String2Double(infd.m_vecFindItem[4][i][1].strItem.GetBuffer()));
				pDoc->m_skew_end.push_back(String2Double(infd.m_vecFindItem[4][i][2].strItem.GetBuffer()));
			}
		}
	}
	pDoc->SetModifiedFlag();
}


void CLotteryNumberGridView::OnAdvancedCharacterSetup()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();

	LotteryInfo info = pDoc->m_lottery_info;

	if (m_vec_lottery_number.size() < 1)
		return;

	// 选择需保存的列
	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 10;
	infd.GROUP_NUM_PER_LINE = 2;
	//////////////////////////////////////////////////////////////////////////
	infd.m_vecFindItem[0][0][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[0][0][0].caption, _T("AC 值设置"), 64);
	infd.m_vecFindItem[0][0][0].dwStyle = BS_PUSHLIKE;
	if (pDoc->m_ac_start.size() > 0)	infd.m_vecFindItem[0][0][0].intItem = 1;
	else 		infd.m_vecFindItem[0][0][0].intItem = 0;

	for (int i = 1; i <= 4; i++)
	{
		infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::CHECKBOX;
		if (i == 1)	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("AC="), 64);
		if (i == 2)	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("或 AC="), 64);
		if (i == 3)	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("或 AC="), 64);
		if (i == 4)	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("或 AC="), 64);
		infd.m_vecFindItem[0][i][0].intItem = 0;

		infd.m_vecFindItem[0][i][1].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[0][i][1].caption, _T("从"), 64);
		infd.m_vecFindItem[0][i][1].strItem = "";

		infd.m_vecFindItem[0][i][2].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[0][i][2].caption, _T("到"), 64);
		infd.m_vecFindItem[0][i][2].strItem = "";

		int num = pDoc->m_ac_start.size();
		if (i <= num)
		{
			infd.m_vecFindItem[0][i][0].intItem = 1;
			infd.m_vecFindItem[0][i][1].strItem.Format("%d", pDoc->m_ac_start[i - 1]);
			infd.m_vecFindItem[0][i][2].strItem.Format("%d", pDoc->m_ac_end[i - 1]);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	infd.m_vecFindItem[1][0][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[1][0][0].caption, _T("集团数设置"), 64);
	infd.m_vecFindItem[1][0][0].dwStyle = BS_PUSHLIKE;
	if (pDoc->m_grp_qty_start.size() > 0)	infd.m_vecFindItem[1][0][0].intItem = 1;
	else 		infd.m_vecFindItem[1][0][0].intItem = 0;

	for (int i = 1; i <= 4; i++)
	{
		infd.m_vecFindItem[1][i][0].nType = CDlgTemplateBuilder::CHECKBOX;
		if (i == 1)	memcpy(infd.m_vecFindItem[1][i][0].caption, _T("G QTY="), 64);
		if (i == 2)	memcpy(infd.m_vecFindItem[1][i][0].caption, _T("或 G QTY="), 64);
		if (i == 3)	memcpy(infd.m_vecFindItem[1][i][0].caption, _T("或 G QTY="), 64);
		if (i == 4)	memcpy(infd.m_vecFindItem[1][i][0].caption, _T("或 G QTY="), 64);
		infd.m_vecFindItem[1][i][0].intItem = 0;

		infd.m_vecFindItem[1][i][1].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[1][i][1].caption, _T("从"), 64);
		infd.m_vecFindItem[1][i][1].strItem = "";

		infd.m_vecFindItem[1][i][2].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[1][i][2].caption, _T("到"), 64);
		infd.m_vecFindItem[1][i][2].strItem = "";

		int num = pDoc->m_grp_qty_start.size();
		if (i <= num)
		{
			infd.m_vecFindItem[1][i][0].intItem = 1;
			infd.m_vecFindItem[1][i][1].strItem.Format("%d", pDoc->m_grp_qty_start[i - 1]);
			infd.m_vecFindItem[1][i][2].strItem.Format("%d", pDoc->m_grp_qty_end[i - 1]);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	infd.m_vecFindItem[2][0][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[2][0][0].caption, _T("边缘号码设置"), 64);
	infd.m_vecFindItem[2][0][0].dwStyle = BS_PUSHLIKE;
	if (pDoc->m_fn_qty_start.size() > 0)	infd.m_vecFindItem[2][0][0].intItem = 1;
	else 		infd.m_vecFindItem[2][0][0].intItem = 0;

	for (int i = 1; i <= 4; i++)
	{
		infd.m_vecFindItem[2][i][0].nType = CDlgTemplateBuilder::CHECKBOX;
		if (i == 1)	memcpy(infd.m_vecFindItem[2][i][0].caption, _T("FN QTY="), 64);
		if (i == 2)	memcpy(infd.m_vecFindItem[2][i][0].caption, _T("或 FN QTY="), 64);
		if (i == 3)	memcpy(infd.m_vecFindItem[2][i][0].caption, _T("或 FN QTY="), 64);
		if (i == 4)	memcpy(infd.m_vecFindItem[2][i][0].caption, _T("或 FN QTY="), 64);
		infd.m_vecFindItem[2][i][0].intItem = 0;

		infd.m_vecFindItem[2][i][1].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[2][i][1].caption, _T("从"), 64);
		infd.m_vecFindItem[2][i][1].strItem = "";

		infd.m_vecFindItem[2][i][2].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[2][i][2].caption, _T("到"), 64);
		infd.m_vecFindItem[2][i][2].strItem = "";

		int num = pDoc->m_fn_qty_start.size();
		if (i <= num)
		{
			infd.m_vecFindItem[2][i][0].intItem = 1;
			infd.m_vecFindItem[2][i][1].strItem.Format("%d", pDoc->m_fn_qty_start[i - 1]);
			infd.m_vecFindItem[2][i][2].strItem.Format("%d", pDoc->m_fn_qty_end[i - 1]);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	infd.m_vecFindItem[3][0][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[3][0][0].caption, _T("奇异选号公式"), 64);
	infd.m_vecFindItem[3][0][0].dwStyle = BS_PUSHLIKE;
	if (pDoc->m_fred_start.size() > 0)	infd.m_vecFindItem[3][0][0].intItem = 1;
	else 		infd.m_vecFindItem[3][0][0].intItem = 0;

	for (int i = 1; i <= 4; i++)
	{
		infd.m_vecFindItem[3][i][0].nType = CDlgTemplateBuilder::CHECKBOX;
		if (i == 1)	memcpy(infd.m_vecFindItem[3][i][0].caption, _T("FRED1="), 64);
		if (i == 2)	memcpy(infd.m_vecFindItem[3][i][0].caption, _T("或 FRED2="), 64);
		if (i == 3)	memcpy(infd.m_vecFindItem[3][i][0].caption, _T("或 FRED3="), 64);
		if (i == 4)	memcpy(infd.m_vecFindItem[3][i][0].caption, _T("或 FRED4="), 64);
		infd.m_vecFindItem[3][i][0].intItem = 0;

		infd.m_vecFindItem[3][i][1].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[3][i][1].caption, _T("从"), 64);
		infd.m_vecFindItem[3][i][1].strItem = "";

		infd.m_vecFindItem[3][i][2].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[3][i][2].caption, _T("到"), 64);
		infd.m_vecFindItem[3][i][2].strItem = "";

		if (pDoc->m_fred_start.count(i)>0)
		{
			infd.m_vecFindItem[3][i][0].intItem = 1;
			infd.m_vecFindItem[3][i][1].strItem.Format("%.2f", pDoc->m_fred_start[i]);
			infd.m_vecFindItem[3][i][2].strItem.Format("%.2f", pDoc->m_fred_end[i]);
		}
	}

	//////////////////////////////////////////////////////////////////////////

	infd.Init(_T("高级特征设置"), _T("高级特征设置"));
	if (infd.DoModal() != IDOK)
		return;

	pDoc->m_ac_start.clear();
	pDoc->m_ac_end.clear();
	if (infd.m_vecFindItem[0][0][0].intItem)
	{
		for (int i = 1; i <= 4; i++)
		{
			if (infd.m_vecFindItem[0][i][0].intItem)
			{
				pDoc->m_ac_start.push_back(atoi(infd.m_vecFindItem[0][i][1].strItem.GetBuffer()));
				pDoc->m_ac_end.push_back(atoi(infd.m_vecFindItem[0][i][2].strItem.GetBuffer()));
			}
		}
	}

	pDoc->m_grp_qty_start.clear();
	pDoc->m_grp_qty_end.clear();
	if (infd.m_vecFindItem[1][0][0].intItem)
	{
		for (int i = 1; i <= 4; i++)
		{
			if (infd.m_vecFindItem[1][i][0].intItem)
			{
				pDoc->m_grp_qty_start.push_back(atoi(infd.m_vecFindItem[1][i][1].strItem.GetBuffer()));
				pDoc->m_grp_qty_end.push_back(atoi(infd.m_vecFindItem[1][i][2].strItem.GetBuffer()));
			}
		}
	}

	pDoc->m_fn_qty_start.clear();
	pDoc->m_fn_qty_end.clear();
	if (infd.m_vecFindItem[2][0][0].intItem)
	{
		for (int i = 1; i <= 4; i++)
		{
			if (infd.m_vecFindItem[2][i][0].intItem)
			{
				pDoc->m_fn_qty_start.push_back(atoi(infd.m_vecFindItem[2][i][1].strItem.GetBuffer()));
				pDoc->m_fn_qty_end.push_back(atoi(infd.m_vecFindItem[2][i][2].strItem.GetBuffer()));
			}
		}
	}

	pDoc->m_fred_start.clear();
	pDoc->m_fred_end.clear();
	if (infd.m_vecFindItem[3][0][0].intItem)
	{
		for (int i = 1; i <= 4; i++)
		{
			if (infd.m_vecFindItem[3][i][0].intItem)
			{
				pDoc->m_fred_start[i]= String2Double(infd.m_vecFindItem[3][i][1].strItem.GetBuffer());
				pDoc->m_fred_end[i]= String2Double(infd.m_vecFindItem[3][i][2].strItem.GetBuffer());
			}
		}
	}
	pDoc->SetModifiedFlag();
}

//////////////////////////////////////////////////////////////////////////
//打开新视图

void CLotteryNumberGridView::OnLotteryNumberList()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();

	CDocTemplate * pDocTemplate = ((CLotteryApp *)AfxGetApp())->GetCurrDocTemplate(this);
	CFrameWnd * wnd = pDocTemplate->CreateNewFrame(pDoc, NULL);//create view
	wnd->ShowWindow(SW_SHOWMAXIMIZED);
	pDocTemplate->InitialUpdateFrame(wnd, pDoc, TRUE);
}


void CLotteryNumberGridView::OnTrendDistributionView()
{
	// TODO: 在此添加命令处理程序代码

	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();

	CTrendDistributionGridView  	MonitorListView;
	CDocTemplate * pDocTemplate = ((CLotteryApp *)AfxGetApp())->GetCurrDocTemplate(&MonitorListView);
	CFrameWnd * wnd = pDocTemplate->CreateNewFrame(pDoc, NULL);//create view
	wnd->ShowWindow(SW_SHOWMAXIMIZED);
	pDocTemplate->InitialUpdateFrame(wnd, pDoc, TRUE);
}


void LeakageAwardTest(string sqlSelect, string sqlWhere, string strTitle, vector<vector<string>> & vecData)
{
	if (sqlWhere.empty())
	{
		vector<string> vec;
		vec.push_back(strTitle);
		vec.push_back(_T("no"));
		vec.push_back(_T(" "));
		vecData.push_back(vec);
	}
	else
	{
		session sql(g_MysqlPool);
		row r;
		statement st = (sql.prepare << (sqlSelect + sqlWhere), into(r));
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{
			cnt++;
		}
		if (cnt > 0)
		{
			vector<string> vec;
			vec.push_back(strTitle);
			vec.push_back(_T("yes"));
			vec.push_back(_T("success"));
			vecData.push_back(vec);
		}
		else
		{
			vector<string> vec;
			vec.push_back(strTitle);
			vec.push_back(_T("yes"));
			vec.push_back(_T("failure"));
			vecData.push_back(vec);
		}
	}
}


void LeakageAwardTest_1(int nRet, string strTitle, vector<vector<string>> & vecData)
{
	if (nRet==0)
	{
		vector<string> vec;
		vec.push_back(strTitle);
		vec.push_back(_T("no"));
		vec.push_back(_T(" "));
		vecData.push_back(vec);
	}
	else
	{
		if (nRet > 0)
		{
			vector<string> vec;
			vec.push_back(strTitle);
			vec.push_back(_T("yes"));
			vec.push_back(_T("success"));
			vecData.push_back(vec);
		}
		else
		{
			vector<string> vec;
			vec.push_back(strTitle);
			vec.push_back(_T("yes"));
			vec.push_back(_T("failure"));
			vecData.push_back(vec);
		}
	}
}

void CLotteryNumberGridView::AssignmentFilterLog(map<int, Assignment> mapPai, string type, LotteryNumber ln)
{
	//	起止期数 选择不对
	if (mapPai[1].total_periods == 0)
		return ;

	stringstream ss;
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();
	for (int i = 0; i < pDoc->m_lottery_info.regularnum; i++)
	{
		int m = ln.number[i];
		ss.str("");
		ss << _T("【漏奖查询】");
		ss << type << " >> " << Int2String(m, "%#2d");
		ss << _T(", 累计球数 ") << Int2String(mapPai[m].goal_count ,"%#2d");
		ss << _T(", 期差 ") << mapPai[m].periods_different;
		ss << _T(", 最近出现的期数序号 ") << mapPai[m].periods_recent_number;
		ss << _T(", 最大的间隔期数 ") << Int2String(mapPai[m].periods_max_internal, "%#2d");
		ss << _T(", 参与分析的总期数 ") << mapPai[m].total_periods;
		sendToOutput(ss.str().c_str(), NULL, WM_USER_LOG_1);
	}
}

void CLotteryNumberGridView::LeakageAwardProcess(int number[])
{
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();
	pDoc->m_nOperType = 1;		//	为漏奖查询 打印日志

	bool bSpecial = false;
	if (pDoc->m_bContainSpecial && pDoc->m_lottery_info.specialnum > 0)
		bSpecial = true;

	CGridDlg gridDlg;
	gridDlg.m_vecHeader.push_back("计算条件");
	gridDlg.m_vecHeader.push_back("是否设置");
	gridDlg.m_vecHeader.push_back("是否通过");

	stringstream ss;
	ss << pDoc->SelectClause();
	for (int i = 0; i < pDoc->m_lottery_info.regularnum; i++)
	{
		ss << " and number_" << (i + 1) << "=" << number[i] << " ";
	}
	if (bSpecial)
	{
		ss << " and special_number =" << number[pDoc->m_lottery_info.regularnum] << " ";
	}
	string strSelect = ss.str();

	// 获取 现有已开奖的号码
	vector<LotteryNumber> vecln = pDoc->FetchAwardRecord();

	try {
		session sql(g_MysqlPool);
		row r;
		statement st = (sql.prepare << strSelect, into(r));
		st.execute();
		if (st.fetch())
		{
			ss.str("");
			ss << _T("【漏奖查询】查询号码");
			for (int i = 0; i < pDoc->m_lottery_info.regularnum; i++)
				ss << "," <<  number[i];
			if (bSpecial)
				ss << " (" << number[pDoc->m_lottery_info.regularnum] << ")";
			sendToOutput(ss.str().c_str(), NULL, WM_USER_LOG_1);

			ss.str("");
			ss << _T("【漏奖查询】基本特征");
			ss << ", sum " << r.get<int>("sum_number", 0);
			ss << ", odd " << r.get<int>("odd", 0);
			ss << ", even " << r.get<int>("even", 0);
			ss << _T(", 相同尾数 ") << r.get<string>("equal_tail", "");
			ss << _T(", 间0连号 ") << r.get<string>("consecutive0", "");
			ss << _T(", 间1连号 ") << r.get<string>("consecutive1", "");
			ss << _T(", 间2连号 ") << r.get<string>("consecutive2", "");
			sendToOutput(ss.str().c_str(), NULL, WM_USER_LOG_1);

			ss.str("");
			ss << _T("【漏奖查询】统计特征");
			ss << ", var " << r.get<double>("var", 0);
			ss << ", stdev " << r.get<double>("stdev", 0);
			ss << ", devsq " << r.get<double>("devsq", 0);
			ss << ", kurt " << r.get<double>("kurt", 0);
			ss << ", skew " << r.get<double>("skew", 0);
			sendToOutput(ss.str().c_str(), NULL, WM_USER_LOG_1);

			ss.str("");
			ss << _T("【漏奖查询】高级特征");
			ss << ", ac " << r.get<int>("ac", 0);
			ss << ", group_qty " << r.get<int>("group_qty", 0);
			ss << ", fringe_no_qty " << r.get<int>("fringe_no_qty", 0);
			sendToOutput(ss.str().c_str(), NULL, WM_USER_LOG_1);

			int nRet = 0;
			// 连号过滤
			nRet = pDoc->ConsecutiveFilter(r.get<string>("consecutive0", ""),
				r.get<string>("consecutive1", ""), r.get<string>("consecutive2", ""));
			LeakageAwardTest_1(nRet, _T("基本特征-连号"), gridDlg.m_vecData);

			//	相同尾数过滤
			nRet = pDoc->TailFilter(r.get<string>("equal_tail", ""));
			LeakageAwardTest_1(nRet, _T("基本特征-相同尾数"), gridDlg.m_vecData);

			//  fred 过滤
			nRet = pDoc->FredFilter(r.get<int>("sum_number", 0),
					r.get<int>("ac", 0.0), r.get<int>("fringe_no_qty", 0), r.get<int>("group_qty", 0));
			LeakageAwardTest_1(nRet, _T("高级特征-奇异选号（fred）"), gridDlg.m_vecData);

			// 根据已开奖号码，进行相同个数的过滤
			nRet = pDoc->EqualNumberFilter(vecln, number);
			LeakageAwardTest_1(nRet, _T("开奖号码-相同个数"), gridDlg.m_vecData);

			// 根据已开奖号码，进行算术修正的过滤
			nRet = pDoc->ArithmeticCorrectFilter(vecln, number);
			LeakageAwardTest_1(nRet, _T("开奖号码-算术修正"), gridDlg.m_vecData);
		}
		else
		{
			AfxMessageBox("先‘运行’生成数据，再尝试查询");
			return;
		}
	}
	catch (...)
	{
		AfxMessageBox("‘运行'错误，再尝试查询");
		return;
	}

	LeakageAwardTest(strSelect, pDoc->WhereSum(), _T("基本特征-和值"), gridDlg.m_vecData);
	LeakageAwardTest(strSelect, pDoc->WhereOdd(), _T("基本特征-奇偶性"), gridDlg.m_vecData);
	LeakageAwardTest(strSelect, pDoc->WhereAc(), _T("高级特征-AC"), gridDlg.m_vecData);
	LeakageAwardTest(strSelect, pDoc->WhereGrpQty(), _T("高级特征-集团数"), gridDlg.m_vecData);
	LeakageAwardTest(strSelect, pDoc->WhereFnQty(), _T("高级特征-边缘数"), gridDlg.m_vecData);
	LeakageAwardTest(strSelect, pDoc->WhereVar(), _T("统计特征-var"), gridDlg.m_vecData);
	LeakageAwardTest(strSelect, pDoc->WhereDevsq(), _T("统计特征-devsq"), gridDlg.m_vecData);
	LeakageAwardTest(strSelect, pDoc->WhereStdev(), _T("统计特征-stdev"), gridDlg.m_vecData);
	LeakageAwardTest(strSelect, pDoc->WhereKurt(), _T("统计特征-kurt"), gridDlg.m_vecData);
	LeakageAwardTest(strSelect, pDoc->WhereSkew(), _T("统计特征-skew"), gridDlg.m_vecData);
	LeakageAwardTest(strSelect, pDoc->WhereAdjacentSum(), _T("相邻间隔数-总和"), gridDlg.m_vecData);
	LeakageAwardTest(strSelect, pDoc->WhereAdjacentTotality(), _T("相邻间隔数-总体"), gridDlg.m_vecData);
	LeakageAwardTest(strSelect, pDoc->WhereAdjacentInterval(), _T("相邻间隔数-分区"), gridDlg.m_vecData);
	LeakageAwardTest(strSelect, pDoc->WhereRegion(), _T("区段号码个数"), gridDlg.m_vecData);
	//LeakageAwardTest(strSelect, pDoc->WhereArithmeticCorrect(), _T("开奖号码-算术修正"), gridDlg.m_vecData);

	int nRet = 0;
	LotteryNumber ln;
	int i;
	for (i = 0; i < pDoc->m_lottery_info.regularnum; i++)
	{
		ln.number[i] = number[i];
	}
	if (pDoc->m_lottery_info.specialnum > 0)
	{
		ln.special_number = number[i];
	}
	//	π赋值
	map<int, Assignment> mapPai = pDoc->AssignmentFilterPre(vecln, pDoc->m_pai_start, pDoc->m_pai_end, pDoc->m_pai_analysis_periods);
	nRet = pDoc->PaiFilter(mapPai, ln, true);
	LeakageAwardTest_1(nRet, _T("π赋值"), gridDlg.m_vecData);
	
	//	lnπ赋值
	map<int, Assignment> mapLnPai = pDoc->AssignmentFilterPre(vecln, pDoc->m_ln_pai_start, pDoc->m_ln_pai_end, pDoc->m_ln_pai_analysis_periods);
	nRet = pDoc->LnPaiFilter(mapLnPai, ln, true);
	LeakageAwardTest_1(nRet, _T("Lnπ赋值"), gridDlg.m_vecData);

	//	涨跌动力赋值
	map<int, Assignment> mapSwingPower = pDoc->AssignmentFilterPre(vecln, pDoc->m_swing_start, pDoc->m_swing_end, pDoc->m_swing_analysis_periods);
	nRet = pDoc->SwingPowerFilter(mapSwingPower, ln, true);
	LeakageAwardTest_1(nRet, _T("涨跌动力赋值"), gridDlg.m_vecData);

	//	Ln 涨跌动力赋值
	map<int, Assignment> mapLnSwing = pDoc->AssignmentFilterPre(vecln, pDoc->m_ln_swing_start, pDoc->m_ln_swing_end, pDoc->m_ln_swing_analysis_periods);
	nRet = pDoc->LnSwingPowerFilter(mapLnSwing, ln, true);
	LeakageAwardTest_1(nRet, _T("Ln 涨跌动力赋值"), gridDlg.m_vecData);

	//	Bernoulli 赋值
	map<int, Assignment> mapBernoulli = pDoc->AssignmentFilterPre(vecln, pDoc->m_bernoulli_start, pDoc->m_bernoulli_end, pDoc->m_bernoulli_analysis_periods);
	nRet = pDoc->BernoulliFilter(mapBernoulli, ln, true);
	LeakageAwardTest_1(nRet, _T("Bernoulli 赋值"), gridDlg.m_vecData);

	//	Potential 赋值
	map<int, Assignment> mapPotential = pDoc->AssignmentFilterPre(vecln, pDoc->m_potential_start, pDoc->m_potential_end, pDoc->m_potential_analysis_periods);
	nRet = pDoc->PotentialFilter(mapPotential, ln, true);
	LeakageAwardTest_1(nRet, _T("Potential 赋值"), gridDlg.m_vecData);

	//  组合过滤源
	nRet = pDoc->RegularExpressFilter(ln);
	LeakageAwardTest_1(nRet, _T("组合过滤源"), gridDlg.m_vecData);
	
	gridDlg.DoModal();
}

void CLotteryNumberGridView::OnLeakageAwardQuery()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();
	pDoc->m_nOperType = 1;		//	为漏奖查询 打印日志

	bool bSpecial = false;
	if (pDoc->m_bContainSpecial && pDoc->m_lottery_info.specialnum > 0)
		bSpecial = true;


	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 30;
	infd.GROUP_NUM_PER_LINE = 3;
	for (int i = 0; i < pDoc->m_lottery_info.regularnum; i++)
	{
		infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
		sprintf_s(infd.m_vecFindItem[0][i][0].caption, 64, _T("号码%d"),i+1);
		infd.m_vecFindItem[0][i][0].strItem = "";
		if (m_selected_lottery_number.number[i] != 0)
			infd.m_vecFindItem[0][i][0].strItem.Format("%d", m_selected_lottery_number.number[i]);
		infd.m_vecFindItem[0][i][0].nMin = 1;
		infd.m_vecFindItem[0][i][0].nMax = pDoc->m_lottery_info.totalnum;

	}
	if (bSpecial)
	{
		infd.m_vecFindItem[0][pDoc->m_lottery_info.regularnum][0].nType = CDlgTemplateBuilder::EDIT;
		sprintf_s(infd.m_vecFindItem[0][pDoc->m_lottery_info.regularnum][0].caption, 64, _T("特别号"));
		infd.m_vecFindItem[0][pDoc->m_lottery_info.regularnum][0].strItem = "";
		if (m_selected_lottery_number.special_number != 0)
			infd.m_vecFindItem[0][pDoc->m_lottery_info.regularnum][0].strItem.Format("%d", m_selected_lottery_number.special_number);
		infd.m_vecFindItem[0][pDoc->m_lottery_info.regularnum][0].nMin = 1;
		infd.m_vecFindItem[0][pDoc->m_lottery_info.regularnum][0].nMax = pDoc->m_lottery_info.totalnum;
	}

	infd.Init(_T("漏奖查询"), _T("漏奖查询"));
	if (infd.DoModal() != IDOK)
		return;

	int number[10];
	for (int i = 0; i < pDoc->m_lottery_info.regularnum + (bSpecial ? 1: 0); i++)
	{
		number[i] = atoi(infd.m_vecFindItem[0][i][0].strItem.GetBuffer());
	}
	for (int i = 0; i < pDoc->m_lottery_info.regularnum - 1; i++)
	{
		if (number[i] >= number[i + 1])
		{
			AfxMessageBox(_T("开奖号码未按照大小顺序输入"));
			return;
		}
	}

	LeakageAwardProcess(number);
}


void CLotteryNumberGridView::WinLotteryProcess(int number[])
{
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();

	CString  filter = _T("csv文件|*.csv||");
	CFileDialog cfd(TRUE, _T("csv"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter);
	if (cfd.DoModal() != IDOK)
		return;

	CString filepath = cfd.GetPathName();

	ifstream infile;
	infile.open(cfd.GetPathName().GetBuffer());

	if (!infile)
	{
		AfxMessageBox(_T("查询文件打开失败"));
		return;
	}

	// 统计排序中奖情况
	map<int, vector<string>> mapWinLottery;
	string buff;
	while (std::getline(infile, buff))
	{
		vector<string>	vecStr;
		splitString(buff, ",", vecStr);

		if (vecStr.size() < pDoc->m_lottery_info.regularnum )
			continue;

		int query[10];
		if (vecStr.size() < pDoc->m_lottery_info.regularnum + pDoc->m_lottery_info.specialnum)
		{
			// 有特别号，但是待查询数据集中未有 特别号
			for (int i = 0; i < pDoc->m_lottery_info.regularnum; i++)
			{
				query[i] = atoi(vecStr[i].c_str());
			}
			query[pDoc->m_lottery_info.regularnum] = 0;
		}
		else 
		{
			for (int i = 0; i < pDoc->m_lottery_info.regularnum + pDoc->m_lottery_info.specialnum; i++)
			{
				query[i] = atoi(vecStr[i].c_str());
			}
		}
		
		string mark = "";
		int nLevel = pDoc->WinLotteryQuery(query, number, mark);
		if (mapWinLottery.count(nLevel) > 0)
		{
			mapWinLottery[nLevel].push_back(buff + "@" + mark);
		}
		else
		{
			vector<string> vec;
			vec.push_back(buff + "@" + mark);
			mapWinLottery[nLevel] = vec;
		}
	}
	infile.close();

	CGridDlg gridDlg;
	gridDlg.m_vecHeader.push_back("奖项等级");
	gridDlg.m_vecHeader.push_back("号码");
	gridDlg.m_vecHeader.push_back("中奖号");

	for (map<int, vector<string>>::iterator it = mapWinLottery.begin();
		it != mapWinLottery.end(); it++)
	{
		if (it->first==0)
			continue; // 未获奖

		{
			vector<string>	vec;
			vec.push_back(pDoc->LotteryLevel(it->first) + " 数量");
			vec.push_back(Int2String(mapWinLottery[it->first].size()));
			vec.push_back("");
			gridDlg.m_vecData.push_back(vec);
		}

		for (int i = 0; i < it->second.size(); i++)
		{
			vector<string>	vec;
			vec.push_back(pDoc->LotteryLevel(it->first));

			vector<string>	vecStr;
			splitString(it->second[i], "@", vecStr);
			vec.push_back(vecStr[0].c_str());
			if (vecStr.size() > 1) {
				vec.push_back(vecStr[1].c_str());
			}
			else {
				vec.push_back("");
			}

			gridDlg.m_vecData.push_back(vec);
		}
	}

	gridDlg.DoModal();
}

void CLotteryNumberGridView::OnWinLotteryQuery()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();

	bool bSpecial = false;
	if (pDoc->m_bContainSpecial && pDoc->m_lottery_info.specialnum > 0)
		bSpecial = true;

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 30;
	infd.GROUP_NUM_PER_LINE = 3;
	for (int i = 0; i < pDoc->m_lottery_info.regularnum; i++)
	{
		infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
		sprintf_s(infd.m_vecFindItem[0][i][0].caption, 64, _T("号码%d"), i + 1);
		infd.m_vecFindItem[0][i][0].strItem = "";
		if (m_selected_lottery_number.number[i] != 0)
			infd.m_vecFindItem[0][i][0].strItem.Format("%d", m_selected_lottery_number.number[i]);
		infd.m_vecFindItem[0][i][0].nMin = 1;
		infd.m_vecFindItem[0][i][0].nMax = pDoc->m_lottery_info.totalnum;

	}
	if (pDoc->m_lottery_info.specialnum > 0)
	{
		infd.m_vecFindItem[0][pDoc->m_lottery_info.regularnum][0].nType = CDlgTemplateBuilder::EDIT;
		sprintf_s(infd.m_vecFindItem[0][pDoc->m_lottery_info.regularnum][0].caption, 64, _T("特别号"));
		infd.m_vecFindItem[0][pDoc->m_lottery_info.regularnum][0].strItem = "";
		if (m_selected_lottery_number.special_number != 0)
			infd.m_vecFindItem[0][pDoc->m_lottery_info.regularnum][0].strItem.Format("%d", m_selected_lottery_number.special_number);
		infd.m_vecFindItem[0][pDoc->m_lottery_info.regularnum][0].nMin = 1;
		infd.m_vecFindItem[0][pDoc->m_lottery_info.regularnum][0].nMax = pDoc->m_lottery_info.totalnum;
	}

	infd.Init(_T("输入开奖号码"), _T("输入开奖号码"));
	if (infd.DoModal() != IDOK)
		return;

	int number[10];
	for (int i = 0; i < pDoc->m_lottery_info.regularnum + (pDoc->m_lottery_info.specialnum > 0 ? 1 : 0); i++)
	{
		number[i] = atoi(infd.m_vecFindItem[0][i][0].strItem.GetBuffer());
	}
	for (int i = 0; i < pDoc->m_lottery_info.regularnum - 1; i++)
	{
		if (number[i] >= number[i + 1])
		{
			AfxMessageBox(_T("开奖号码未按照大小顺序输入"));
			return;
		}
	}

	WinLotteryProcess(number);
}


void CLotteryNumberGridView::OnSystemFilterSetup()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 20;
	infd.GROUP_NUM_PER_LINE = 3;
		
	//////////////////////////
	infd.m_vecFindItem[0][0][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[0][0][0].caption, _T("参与运算期数"), 64);
	infd.m_vecFindItem[0][0][0].dwStyle = BS_PUSHLIKE;
	if (pDoc->m_filter_start >= 0 && pDoc->m_filter_start <= pDoc->m_filter_end )	
		infd.m_vecFindItem[0][0][0].intItem = 1;
	else
	{
		infd.m_vecFindItem[0][0][0].intItem = 0;
	}
	infd.m_vecFindItem[0][1][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][1][0].caption, _T("从"), 64);
	infd.m_vecFindItem[0][1][0].strItem.Format("%d",pDoc->m_filter_start);
	infd.m_vecFindItem[0][1][0].nMin = 0;
	infd.m_vecFindItem[0][1][0].nMax = 100000000;

	infd.m_vecFindItem[0][2][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][2][0].caption, _T("到"), 64);
	infd.m_vecFindItem[0][2][0].strItem.Format("%d", pDoc->m_filter_end);
	infd.m_vecFindItem[0][2][0].nMin = 0;
	infd.m_vecFindItem[0][2][0].nMax = 100000000;

	//////////////////////////
	map<int, int>	vec = pDoc->m_filter_arithmetic_correct;
	int k = 1;
	infd.m_vecFindItem[k][0][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[k][0][0].caption, _T("算术修改参数设置"), 64);
	infd.m_vecFindItem[k][0][0].dwStyle = BS_PUSHLIKE;
	if (vec.size() > 0)	infd.m_vecFindItem[k][0][0].intItem = 1;
	else 		infd.m_vecFindItem[k][0][0].intItem = 0;

	for (int i = 1; i <= 7; i++)
	{
		infd.m_vecFindItem[k][i][0].nType = CDlgTemplateBuilder::CHECKBOX;
		if (i == 1)	memcpy(infd.m_vecFindItem[k][i][0].caption, _T("排除+/-0"), 64);
		if (i == 2)	memcpy(infd.m_vecFindItem[k][i][0].caption, _T("且 排除+/-1"), 64);
		if (i == 3)	memcpy(infd.m_vecFindItem[k][i][0].caption, _T("且 排除+/-2"), 64);
		if (i == 4)	memcpy(infd.m_vecFindItem[k][i][0].caption, _T("且 排除+/-3"), 64);
		if (i == 5)	memcpy(infd.m_vecFindItem[k][i][0].caption, _T("且 排除+/-4"), 64);
		if (i == 6)	memcpy(infd.m_vecFindItem[k][i][0].caption, _T("且 排除+/-5"), 64);
		if (i == 7)	memcpy(infd.m_vecFindItem[k][i][0].caption, _T("且 排除+/-6"), 64);
		infd.m_vecFindItem[k][i][0].intItem = 0;
				
		if (vec.count(i) > 0)
		{
			infd.m_vecFindItem[k][i][0].intItem = 1;
			//infd.m_vecFindItem[k][i][1].strItem.Format("%d", vec[i]);
		}
	}

	/////////////////////////////////
	k = 2;
	vec = pDoc->m_filter_equal_number;
	infd.m_vecFindItem[k][0][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[k][0][0].caption, _T("相同个数设置"), 64);
	infd.m_vecFindItem[k][0][0].dwStyle = BS_PUSHLIKE;
	if (vec.size() > 0)	infd.m_vecFindItem[k][0][0].intItem = 1;
	else 		infd.m_vecFindItem[k][0][0].intItem = 0;

	for (int i = 1; i <= 6; i++)
	{
		infd.m_vecFindItem[k][i][0].nType = CDlgTemplateBuilder::CHECKBOX;
		if (i == 1)	memcpy(infd.m_vecFindItem[k][i][0].caption, _T("排除3个相同"), 64);
		if (i == 2)	memcpy(infd.m_vecFindItem[k][i][0].caption, _T("且排除4个相同"), 64);
		if (i == 3)	memcpy(infd.m_vecFindItem[k][i][0].caption, _T("且排除5个相同"), 64);
		if (i == 4)	memcpy(infd.m_vecFindItem[k][i][0].caption, _T("且排除6个相同"), 64);
		if (i == 5)	memcpy(infd.m_vecFindItem[k][i][0].caption, _T("且排除7个相同"), 64);
		if (i == 6)	memcpy(infd.m_vecFindItem[k][i][0].caption, _T("且排除8个相同"), 64);
		infd.m_vecFindItem[k][i][0].intItem = 0;

		if (vec.count(i) > 0)
		{
			infd.m_vecFindItem[k][i][0].intItem = 1;
			//infd.m_vecFindItem[k][i][1].strItem.Format("%d", vec[i]);
		}
	}

	infd.Init(_T("开奖号码过滤"), _T("开奖号码过滤"));
	if (infd.DoModal() != IDOK)
		return;

	if (infd.m_vecFindItem[0][0][0].intItem)
	{
		pDoc->m_filter_start = atoi(infd.m_vecFindItem[0][1][0].strItem.GetBuffer());
		pDoc->m_filter_end = atoi(infd.m_vecFindItem[0][2][0].strItem.GetBuffer());
	}
	else
	{
		pDoc->m_filter_start = pDoc->m_filter_end = 0;
	}

	k = 1;
	if (infd.m_vecFindItem[k][0][0].intItem)
	{
		vec.clear();
		for (int i = 1; i <= 7; i++)
		{
			if (infd.m_vecFindItem[k][i][0].intItem)
			{
				vec[i] = i;
			}
		}
		pDoc->m_filter_arithmetic_correct = vec;
	}
	else {
		pDoc->m_filter_arithmetic_correct.clear();
	}

	k = 2;
	if (infd.m_vecFindItem[k][0][0].intItem)
	{
		vec.clear();
		for (int i = 1; i <= 6; i++)
		{
			if (infd.m_vecFindItem[k][i][0].intItem)
			{
				vec[i] = i;
			}
		}
		pDoc->m_filter_equal_number = vec;
	}
	else {
		pDoc->m_filter_equal_number.clear();
	}
	pDoc->SetModifiedFlag();
}


void CLotteryNumberGridView::OnDataSourceSetup()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();

	CString  filter = _T("csv文件|*.csv||");
	CFileDialog cfd(TRUE, _T("csv"), NULL/*pDoc->m_lottery_info.code.c_str()*/,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter);
	if (cfd.DoModal() != IDOK)
	{
		pDoc->m_strDataSource.Empty();
		return;
	}

	pDoc->m_strDataSource = cfd.GetPathName();
}


void CLotteryNumberGridView::OnFilterSourceSetup()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();

	CString  filter = _T("flt文件|*.flt||");
	CFileDialog cfd(TRUE, _T("flt"), NULL/*pDoc->m_lottery_info.code.c_str()*/,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter);
	if (cfd.DoModal() != IDOK)
	{
		pDoc->m_strFilterSource.Empty();
		pDoc->m_vecFilterArrayData.clear();
		return;
	}

	pDoc->m_strFilterSource = cfd.GetPathName();
	pDoc->m_vecFilterArrayData.clear();
}

void CLotteryNumberGridView::OnUpdateRun(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	if(theApp.m_bRunning == FALSE)
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}


void CLotteryNumberGridView::OnPaiSetup()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 35;
	infd.GROUP_NUM_PER_LINE = 3;

	//////////////////////////
	infd.m_vecFindItem[0][0][0].nType = CDlgTemplateBuilder::STATIC;
	memcpy(infd.m_vecFindItem[0][0][0].caption, _T("参与运算期数"), 64);
	/*infd.m_vecFindItem[0][0][0].dwStyle = BS_PUSHLIKE;
	if (pDoc->m_pai_start >= 0 && pDoc->m_pai_start <= pDoc->m_pai_end)
		infd.m_vecFindItem[0][0][0].intItem = 1;
	else
	{
		infd.m_vecFindItem[0][0][0].intItem = 0;
	}*/
	
	infd.m_vecFindItem[0][1][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][1][0].caption, _T("从"), 64);
	infd.m_vecFindItem[0][1][0].strItem.Format("%d", pDoc->m_pai_start);
	infd.m_vecFindItem[0][1][0].nMin = 0;
	infd.m_vecFindItem[0][1][0].nMax = 100000000;

	infd.m_vecFindItem[0][2][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][2][0].caption, _T("到"), 64);
	infd.m_vecFindItem[0][2][0].strItem.Format("%d", pDoc->m_pai_end);
	infd.m_vecFindItem[0][2][0].nMin = 0;
	infd.m_vecFindItem[0][2][0].nMax = 100000000;

	infd.m_vecFindItem[0][3][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[0][3][0].caption, _T("考虑期差"), 64);
	infd.m_vecFindItem[0][3][0].dwStyle = BS_PUSHLIKE;
	if (pDoc->m_pai_period_difference != 0)
		infd.m_vecFindItem[0][3][0].intItem = 1;
	else
		infd.m_vecFindItem[0][3][0].intItem = 0;

	infd.m_vecFindItem[0][4][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][4][0].caption, _T("π"), 64);
	infd.m_vecFindItem[0][4][0].strItem = "";
	infd.m_vecFindItem[0][4][0].strItem.Format("%.8f", pDoc->m_pai_value);

	infd.m_vecFindItem[0][5][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][5][0].caption, _T("累计球控制期数"), 64);
	infd.m_vecFindItem[0][5][0].strItem = "";
	infd.m_vecFindItem[0][5][0].strItem.Format("%d", pDoc->m_pai_analysis_periods);
	infd.m_vecFindItem[0][5][0].nMin = 0;
	infd.m_vecFindItem[0][5][0].nMax = 1000;

	//////////////////////////////////////////////////////////////////////////
	infd.m_vecFindItem[1][0][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[1][0][0].caption, _T("+Sum"), 64);
	infd.m_vecFindItem[1][0][0].dwStyle = BS_PUSHLIKE;
	if (pDoc->m_pai_add_start.size() > 0)	infd.m_vecFindItem[1][0][0].intItem = 1;
	else 		infd.m_vecFindItem[1][0][0].intItem = 0;

	for (int i = 1; i <= 4; i++)
	{
		infd.m_vecFindItem[1][i][0].nType = CDlgTemplateBuilder::CHECKBOX;
		if (i == 1)	memcpy(infd.m_vecFindItem[1][i][0].caption, _T("+Sum="), 64);
		if (i == 2)	memcpy(infd.m_vecFindItem[1][i][0].caption, _T("或 +Sum="), 64);
		if (i == 3)	memcpy(infd.m_vecFindItem[1][i][0].caption, _T("或 +Sum="), 64);
		if (i == 4)	memcpy(infd.m_vecFindItem[1][i][0].caption, _T("或 +Sum="), 64);
		infd.m_vecFindItem[1][i][0].intItem = 0;

		infd.m_vecFindItem[1][i][1].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[1][i][1].caption, _T("从"), 64);
		infd.m_vecFindItem[1][i][1].strItem = "";

		infd.m_vecFindItem[1][i][2].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[1][i][2].caption, _T("到"), 64);
		infd.m_vecFindItem[1][i][2].strItem = "";

		int num = pDoc->m_pai_add_start.size();
		if (i <= num)
		{
			infd.m_vecFindItem[1][i][0].intItem = 1;
			infd.m_vecFindItem[1][i][1].strItem.Format("%.2f", pDoc->m_pai_add_start[i - 1]);
			infd.m_vecFindItem[1][i][2].strItem.Format("%.2f", pDoc->m_pai_add_end[i - 1]);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	infd.m_vecFindItem[2][0][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[2][0][0].caption, _T("*Sum"), 64);
	infd.m_vecFindItem[2][0][0].dwStyle = BS_PUSHLIKE;
	if (pDoc->m_pai_mul_start.size() > 0)	infd.m_vecFindItem[2][0][0].intItem = 1;
	else 		infd.m_vecFindItem[2][0][0].intItem = 0;

	for (int i = 1; i <= 4; i++)
	{
		infd.m_vecFindItem[2][i][0].nType = CDlgTemplateBuilder::CHECKBOX;
		if (i == 1)	memcpy(infd.m_vecFindItem[2][i][0].caption, _T("*Sum="), 64);
		if (i == 2)	memcpy(infd.m_vecFindItem[2][i][0].caption, _T("或 *Sum="), 64);
		if (i == 3)	memcpy(infd.m_vecFindItem[2][i][0].caption, _T("或 *Sum="), 64);
		if (i == 4)	memcpy(infd.m_vecFindItem[2][i][0].caption, _T("或 *Sum="), 64);
		infd.m_vecFindItem[2][i][0].intItem = 0;

		infd.m_vecFindItem[2][i][1].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[2][i][1].caption, _T("从"), 64);
		infd.m_vecFindItem[2][i][1].strItem = "";

		infd.m_vecFindItem[2][i][2].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[2][i][2].caption, _T("到"), 64);
		infd.m_vecFindItem[2][i][2].strItem = "";

		int num = pDoc->m_pai_mul_start.size();
		if (i <= num)
		{
			infd.m_vecFindItem[2][i][0].intItem = 1;
			infd.m_vecFindItem[2][i][1].strItem.Format("%.2f", pDoc->m_pai_mul_start[i - 1]);
			infd.m_vecFindItem[2][i][2].strItem.Format("%.2f", pDoc->m_pai_mul_end[i - 1]);
		}
	}

	infd.m_vecFindItem[2][5][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[2][5][0].caption, _T("调节参数"), 64);
	if (pDoc->m_pai_mul_adjunstment != 0)
		infd.m_vecFindItem[2][5][0].strItem.Format("%.2f", pDoc->m_pai_mul_adjunstment);
	else
		infd.m_vecFindItem[2][5][0].strItem = " ";

	/////////////////////////////////////////////
	infd.Init(_T("π赋值设置"), _T("π赋值设置"));
	if (infd.DoModal() != IDOK)
		return;

	if (1)
	{
		pDoc->m_pai_start = atoi(infd.m_vecFindItem[0][1][0].strItem.GetBuffer());
		pDoc->m_pai_end = atoi(infd.m_vecFindItem[0][2][0].strItem.GetBuffer());
	}
	else
	{
		pDoc->m_pai_start = pDoc->m_pai_end = 0;
	}

	pDoc->m_pai_period_difference = infd.m_vecFindItem[0][3][0].intItem;
	pDoc->m_pai_value = String2Double(infd.m_vecFindItem[0][4][0].strItem.GetBuffer());
	pDoc->m_pai_analysis_periods = atoi(infd.m_vecFindItem[0][5][0].strItem.GetBuffer());

	pDoc->m_pai_add_start.clear();
	pDoc->m_pai_add_end.clear();
	if (infd.m_vecFindItem[1][0][0].intItem)
	{
		for (int i = 1; i <= 4; i++)
		{
			if (infd.m_vecFindItem[1][i][0].intItem)
			{
				pDoc->m_pai_add_start.push_back(String2Double(infd.m_vecFindItem[1][i][1].strItem.GetBuffer()));
				pDoc->m_pai_add_end.push_back(String2Double(infd.m_vecFindItem[1][i][2].strItem.GetBuffer()));
			}
		}
	}

	pDoc->m_pai_mul_start.clear();
	pDoc->m_pai_mul_end.clear();
	pDoc->m_pai_mul_adjunstment = 0;
	if (infd.m_vecFindItem[2][0][0].intItem)
	{
		for (int i = 1; i <= 4; i++)
		{
			if (infd.m_vecFindItem[2][i][0].intItem)
			{
				pDoc->m_pai_mul_start.push_back(String2Double(infd.m_vecFindItem[2][i][1].strItem.GetBuffer()));
				pDoc->m_pai_mul_end.push_back(String2Double(infd.m_vecFindItem[2][i][2].strItem.GetBuffer()));
			}
		}

		pDoc->m_pai_mul_adjunstment = String2Double(infd.m_vecFindItem[2][5][0].strItem.GetBuffer());
	}

	pDoc->SetModifiedFlag();
}


void CLotteryNumberGridView::OnLnPaiSetup()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 25;
	infd.GROUP_NUM_PER_LINE = 3;

	//////////////////////////
	infd.m_vecFindItem[0][0][0].nType = CDlgTemplateBuilder::STATIC;
	memcpy(infd.m_vecFindItem[0][0][0].caption, _T("参与运算期数"), 64);
	/*infd.m_vecFindItem[0][0][0].dwStyle = BS_PUSHLIKE;
	if (pDoc->m_ln_pai_start >= 0 && pDoc->m_ln_pai_start <= pDoc->m_ln_pai_end)
		infd.m_vecFindItem[0][0][0].intItem = 1;
	else
	{
		infd.m_vecFindItem[0][0][0].intItem = 0;
	}*/

	infd.m_vecFindItem[0][1][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][1][0].caption, _T("从"), 64);
	infd.m_vecFindItem[0][1][0].strItem.Format("%d", pDoc->m_ln_pai_start);
	infd.m_vecFindItem[0][1][0].nMin = 0;
	infd.m_vecFindItem[0][1][0].nMax = 100000000;

	infd.m_vecFindItem[0][2][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][2][0].caption, _T("到"), 64);
	infd.m_vecFindItem[0][2][0].strItem.Format("%d", pDoc->m_ln_pai_end);
	infd.m_vecFindItem[0][2][0].nMin = 0;
	infd.m_vecFindItem[0][2][0].nMax = 100000000;

	infd.m_vecFindItem[0][3][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[0][3][0].caption, _T("考虑期差"), 64);
	infd.m_vecFindItem[0][3][0].dwStyle = BS_PUSHLIKE;
	if (pDoc->m_ln_pai_period_difference != 0)
		infd.m_vecFindItem[0][3][0].intItem = 1;
	else
		infd.m_vecFindItem[0][3][0].intItem = 0;

	infd.m_vecFindItem[0][4][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][4][0].caption, _T("π"), 64);
	infd.m_vecFindItem[0][4][0].strItem = "";
	infd.m_vecFindItem[0][4][0].strItem.Format("%.8f", pDoc->m_ln_pai_value);

	infd.m_vecFindItem[0][5][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][5][0].caption, _T("累计球控制期数"), 64);
	infd.m_vecFindItem[0][5][0].strItem = "";
	infd.m_vecFindItem[0][5][0].strItem.Format("%d", pDoc->m_ln_pai_analysis_periods);
	infd.m_vecFindItem[0][5][0].nMin = 0;
	infd.m_vecFindItem[0][5][0].nMax = 1000;

	//////////////////////////////////////////////////////////////////////////
	infd.m_vecFindItem[1][0][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[1][0][0].caption, _T("+Sum"), 64);
	infd.m_vecFindItem[1][0][0].dwStyle = BS_PUSHLIKE;
	if (pDoc->m_ln_pai_add_start.size() > 0)	infd.m_vecFindItem[1][0][0].intItem = 1;
	else 		infd.m_vecFindItem[1][0][0].intItem = 0;

	for (int i = 1; i <= 4; i++)
	{
		infd.m_vecFindItem[1][i][0].nType = CDlgTemplateBuilder::CHECKBOX;
		if (i == 1)	memcpy(infd.m_vecFindItem[1][i][0].caption, _T("+Sum="), 64);
		if (i == 2)	memcpy(infd.m_vecFindItem[1][i][0].caption, _T("或 +Sum="), 64);
		if (i == 3)	memcpy(infd.m_vecFindItem[1][i][0].caption, _T("或 +Sum="), 64);
		if (i == 4)	memcpy(infd.m_vecFindItem[1][i][0].caption, _T("或 +Sum="), 64);
		infd.m_vecFindItem[1][i][0].intItem = 0;

		infd.m_vecFindItem[1][i][1].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[1][i][1].caption, _T("从"), 64);
		infd.m_vecFindItem[1][i][1].strItem = "";

		infd.m_vecFindItem[1][i][2].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[1][i][2].caption, _T("到"), 64);
		infd.m_vecFindItem[1][i][2].strItem = "";

		int num = pDoc->m_ln_pai_add_start.size();
		if (i <= num)
		{
			infd.m_vecFindItem[1][i][0].intItem = 1;
			infd.m_vecFindItem[1][i][1].strItem.Format("%.2f", pDoc->m_ln_pai_add_start[i - 1]);
			infd.m_vecFindItem[1][i][2].strItem.Format("%.2f", pDoc->m_ln_pai_add_end[i - 1]);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	infd.m_vecFindItem[2][0][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[2][0][0].caption, _T("*Sum"), 64);
	infd.m_vecFindItem[2][0][0].dwStyle = BS_PUSHLIKE;
	if (pDoc->m_ln_pai_mul_start.size() > 0)	infd.m_vecFindItem[2][0][0].intItem = 1;
	else 		infd.m_vecFindItem[2][0][0].intItem = 0;

	for (int i = 1; i <= 4; i++)
	{
		infd.m_vecFindItem[2][i][0].nType = CDlgTemplateBuilder::CHECKBOX;
		if (i == 1)	memcpy(infd.m_vecFindItem[2][i][0].caption, _T("*Sum="), 64);
		if (i == 2)	memcpy(infd.m_vecFindItem[2][i][0].caption, _T("或 *Sum="), 64);
		if (i == 3)	memcpy(infd.m_vecFindItem[2][i][0].caption, _T("或 *Sum="), 64);
		if (i == 4)	memcpy(infd.m_vecFindItem[2][i][0].caption, _T("或 *Sum="), 64);
		infd.m_vecFindItem[2][i][0].intItem = 0;

		infd.m_vecFindItem[2][i][1].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[2][i][1].caption, _T("从"), 64);
		infd.m_vecFindItem[2][i][1].strItem = "";

		infd.m_vecFindItem[2][i][2].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[2][i][2].caption, _T("到"), 64);
		infd.m_vecFindItem[2][i][2].strItem = "";

		int num = pDoc->m_ln_pai_mul_start.size();
		if (i <= num)
		{
			infd.m_vecFindItem[2][i][0].intItem = 1;
			infd.m_vecFindItem[2][i][1].strItem.Format("%.2f", pDoc->m_ln_pai_mul_start[i - 1]);
			infd.m_vecFindItem[2][i][2].strItem.Format("%.2f", pDoc->m_ln_pai_mul_end[i - 1]);
		}
	}

	infd.m_vecFindItem[2][5][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[2][5][0].caption, _T("调节参数"), 64);
	if (pDoc->m_ln_pai_mul_adjunstment != 0)
		infd.m_vecFindItem[2][5][0].strItem.Format("%.2f", pDoc->m_ln_pai_mul_adjunstment);
	else
		infd.m_vecFindItem[2][5][0].strItem = " ";

	/////////////////////////////////////////////
	infd.Init(_T("lnπ赋值设置"), _T("lnπ赋值设置"));
	if (infd.DoModal() != IDOK)
		return;

	if (1)
	{
		pDoc->m_ln_pai_start = atoi(infd.m_vecFindItem[0][1][0].strItem.GetBuffer());
		pDoc->m_ln_pai_end = atoi(infd.m_vecFindItem[0][2][0].strItem.GetBuffer());
	}
	else
	{
		pDoc->m_ln_pai_start = pDoc->m_ln_pai_end = 0;
	}

	pDoc->m_ln_pai_period_difference = infd.m_vecFindItem[0][3][0].intItem;
	pDoc->m_ln_pai_value = String2Double(infd.m_vecFindItem[0][4][0].strItem.GetBuffer());
	pDoc->m_ln_pai_analysis_periods = atoi(infd.m_vecFindItem[0][5][0].strItem.GetBuffer());

	pDoc->m_ln_pai_add_start.clear();
	pDoc->m_ln_pai_add_end.clear();
	if (infd.m_vecFindItem[1][0][0].intItem)
	{
		for (int i = 1; i <= 4; i++)
		{
			if (infd.m_vecFindItem[1][i][0].intItem)
			{
				pDoc->m_ln_pai_add_start.push_back(String2Double(infd.m_vecFindItem[1][i][1].strItem.GetBuffer()));
				pDoc->m_ln_pai_add_end.push_back(String2Double(infd.m_vecFindItem[1][i][2].strItem.GetBuffer()));
			}
		}
	}

	pDoc->m_ln_pai_mul_start.clear();
	pDoc->m_ln_pai_mul_end.clear();
	pDoc->m_ln_pai_mul_adjunstment = 0;
	if (infd.m_vecFindItem[2][0][0].intItem)
	{
		for (int i = 1; i <= 4; i++)
		{
			if (infd.m_vecFindItem[2][i][0].intItem)
			{
				pDoc->m_ln_pai_mul_start.push_back(String2Double(infd.m_vecFindItem[2][i][1].strItem.GetBuffer()));
				pDoc->m_ln_pai_mul_end.push_back(String2Double(infd.m_vecFindItem[2][i][2].strItem.GetBuffer()));
			}
		}

		pDoc->m_ln_pai_mul_adjunstment = String2Double(infd.m_vecFindItem[2][5][0].strItem.GetBuffer());
	}

	pDoc->SetModifiedFlag();
}


void CLotteryNumberGridView::OnSwingPowerSetup()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 25;
	infd.GROUP_NUM_PER_LINE = 3;

	//////////////////////////
	infd.m_vecFindItem[0][0][0].nType = CDlgTemplateBuilder::STATIC;
	memcpy(infd.m_vecFindItem[0][0][0].caption, _T("参与运算期数"), 64);
	/*infd.m_vecFindItem[0][0][0].dwStyle = BS_PUSHLIKE;
	if (pDoc->m_swing_start >= 0 && pDoc->m_swing_start <= pDoc->m_swing_end)
		infd.m_vecFindItem[0][0][0].intItem = 1;
	else
	{
		infd.m_vecFindItem[0][0][0].intItem = 0;
	}*/

	infd.m_vecFindItem[0][1][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][1][0].caption, _T("从"), 64);
	infd.m_vecFindItem[0][1][0].strItem.Format("%d", pDoc->m_swing_start);
	infd.m_vecFindItem[0][1][0].nMin = 0;
	infd.m_vecFindItem[0][1][0].nMax = 100000000;

	infd.m_vecFindItem[0][2][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][2][0].caption, _T("到"), 64);
	infd.m_vecFindItem[0][2][0].strItem.Format("%d", pDoc->m_swing_end);
	infd.m_vecFindItem[0][2][0].nMin = 0;
	infd.m_vecFindItem[0][2][0].nMax = 100000000;

	infd.m_vecFindItem[0][3][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][3][0].caption, _T("累计球控制期数"), 64);
	infd.m_vecFindItem[0][3][0].strItem = "";
	infd.m_vecFindItem[0][3][0].strItem.Format("%d", pDoc->m_swing_analysis_periods);
	infd.m_vecFindItem[0][3][0].nMin = 0;
	infd.m_vecFindItem[0][3][0].nMax = 1000;


	//////////////////////////////////////////////////////////////////////////
	infd.m_vecFindItem[1][0][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[1][0][0].caption, _T("+Sum"), 64);
	infd.m_vecFindItem[1][0][0].dwStyle = BS_PUSHLIKE;
	if (pDoc->m_swing_add_start.size() > 0)	infd.m_vecFindItem[1][0][0].intItem = 1;
	else 		infd.m_vecFindItem[1][0][0].intItem = 0;

	for (int i = 1; i <= 4; i++)
	{
		infd.m_vecFindItem[1][i][0].nType = CDlgTemplateBuilder::CHECKBOX;
		if (i == 1)	memcpy(infd.m_vecFindItem[1][i][0].caption, _T("+Sum="), 64);
		if (i == 2)	memcpy(infd.m_vecFindItem[1][i][0].caption, _T("或 +Sum="), 64);
		if (i == 3)	memcpy(infd.m_vecFindItem[1][i][0].caption, _T("或 +Sum="), 64);
		if (i == 4)	memcpy(infd.m_vecFindItem[1][i][0].caption, _T("或 +Sum="), 64);
		infd.m_vecFindItem[1][i][0].intItem = 0;

		infd.m_vecFindItem[1][i][1].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[1][i][1].caption, _T("从"), 64);
		infd.m_vecFindItem[1][i][1].strItem = "";

		infd.m_vecFindItem[1][i][2].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[1][i][2].caption, _T("到"), 64);
		infd.m_vecFindItem[1][i][2].strItem = "";

		int num = pDoc->m_swing_add_start.size();
		if (i <= num)
		{
			infd.m_vecFindItem[1][i][0].intItem = 1;
			infd.m_vecFindItem[1][i][1].strItem.Format("%.2f", pDoc->m_swing_add_start[i - 1]);
			infd.m_vecFindItem[1][i][2].strItem.Format("%.2f", pDoc->m_swing_add_end[i - 1]);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	infd.m_vecFindItem[2][0][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[2][0][0].caption, _T("*Sum"), 64);
	infd.m_vecFindItem[2][0][0].dwStyle = BS_PUSHLIKE;
	if (pDoc->m_swing_mul_start.size() > 0)	infd.m_vecFindItem[2][0][0].intItem = 1;
	else 		infd.m_vecFindItem[2][0][0].intItem = 0;

	for (int i = 1; i <= 4; i++)
	{
		infd.m_vecFindItem[2][i][0].nType = CDlgTemplateBuilder::CHECKBOX;
		if (i == 1)	memcpy(infd.m_vecFindItem[2][i][0].caption, _T("*Sum="), 64);
		if (i == 2)	memcpy(infd.m_vecFindItem[2][i][0].caption, _T("或 *Sum="), 64);
		if (i == 3)	memcpy(infd.m_vecFindItem[2][i][0].caption, _T("或 *Sum="), 64);
		if (i == 4)	memcpy(infd.m_vecFindItem[2][i][0].caption, _T("或 *Sum="), 64);
		infd.m_vecFindItem[2][i][0].intItem = 0;

		infd.m_vecFindItem[2][i][1].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[2][i][1].caption, _T("从"), 64);
		infd.m_vecFindItem[2][i][1].strItem = "";

		infd.m_vecFindItem[2][i][2].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[2][i][2].caption, _T("到"), 64);
		infd.m_vecFindItem[2][i][2].strItem = "";

		int num = pDoc->m_swing_mul_start.size();
		if (i <= num)
		{
			infd.m_vecFindItem[2][i][0].intItem = 1;
			infd.m_vecFindItem[2][i][1].strItem.Format("%.2f", pDoc->m_swing_mul_start[i - 1]);
			infd.m_vecFindItem[2][i][2].strItem.Format("%.2f", pDoc->m_swing_mul_end[i - 1]);
		}
	}

	infd.m_vecFindItem[2][5][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[2][5][0].caption, _T("调节参数"), 64);
	if (pDoc->m_swing_mul_adjunstment != 0)
		infd.m_vecFindItem[2][5][0].strItem.Format("%.2f", pDoc->m_swing_mul_adjunstment);
	else
		infd.m_vecFindItem[2][5][0].strItem = " ";

	/////////////////////////////////////////////
	infd.Init(_T("涨跌动力设置"), _T("涨跌动力设置"));
	if (infd.DoModal() != IDOK)
		return;

	if (1)
	{
		pDoc->m_swing_start = atoi(infd.m_vecFindItem[0][1][0].strItem.GetBuffer());
		pDoc->m_swing_end = atoi(infd.m_vecFindItem[0][2][0].strItem.GetBuffer());
	}
	else
	{
		pDoc->m_swing_start = pDoc->m_swing_end = 0;
	}
	pDoc->m_swing_analysis_periods = atoi(infd.m_vecFindItem[0][3][0].strItem.GetBuffer());

	pDoc->m_swing_add_start.clear();
	pDoc->m_swing_add_end.clear();
	if (infd.m_vecFindItem[1][0][0].intItem)
	{
		for (int i = 1; i <= 4; i++)
		{
			if (infd.m_vecFindItem[1][i][0].intItem)
			{
				pDoc->m_swing_add_start.push_back(String2Double(infd.m_vecFindItem[1][i][1].strItem.GetBuffer()));
				pDoc->m_swing_add_end.push_back(String2Double(infd.m_vecFindItem[1][i][2].strItem.GetBuffer()));
			}
		}
	}

	pDoc->m_swing_mul_start.clear();
	pDoc->m_swing_mul_end.clear();
	pDoc->m_swing_mul_adjunstment = 0;
	if (infd.m_vecFindItem[2][0][0].intItem)
	{
		for (int i = 1; i <= 4; i++)
		{
			if (infd.m_vecFindItem[2][i][0].intItem)
			{
				pDoc->m_swing_mul_start.push_back(String2Double(infd.m_vecFindItem[2][i][1].strItem.GetBuffer()));
				pDoc->m_swing_mul_end.push_back(String2Double(infd.m_vecFindItem[2][i][2].strItem.GetBuffer()));
			}
		}

		pDoc->m_swing_mul_adjunstment = String2Double(infd.m_vecFindItem[2][5][0].strItem.GetBuffer());
	}

	pDoc->SetModifiedFlag();
}


void CLotteryNumberGridView::OnLnSwingPowerSetup()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 25;
	infd.GROUP_NUM_PER_LINE = 3;

	//////////////////////////
	infd.m_vecFindItem[0][0][0].nType = CDlgTemplateBuilder::STATIC;
	memcpy(infd.m_vecFindItem[0][0][0].caption, _T("参与运算期数"), 64);
	/*infd.m_vecFindItem[0][0][0].dwStyle = BS_PUSHLIKE;
	if (pDoc->m_ln_swing_start >= 0 && pDoc->m_ln_swing_start <= pDoc->m_ln_swing_end)
		infd.m_vecFindItem[0][0][0].intItem = 1;
	else
	{
		infd.m_vecFindItem[0][0][0].intItem = 0;
	}*/

	infd.m_vecFindItem[0][1][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][1][0].caption, _T("从"), 64);
	infd.m_vecFindItem[0][1][0].strItem.Format("%d", pDoc->m_ln_swing_start);
	infd.m_vecFindItem[0][1][0].nMin = 0;
	infd.m_vecFindItem[0][1][0].nMax = 100000000;

	infd.m_vecFindItem[0][2][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][2][0].caption, _T("到"), 64);
	infd.m_vecFindItem[0][2][0].strItem.Format("%d", pDoc->m_ln_swing_end);
	infd.m_vecFindItem[0][2][0].nMin = 0;
	infd.m_vecFindItem[0][2][0].nMax = 100000000;

	infd.m_vecFindItem[0][3][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][3][0].caption, _T("累计球控制期数"), 64);
	infd.m_vecFindItem[0][3][0].strItem = "";
	infd.m_vecFindItem[0][3][0].strItem.Format("%d", pDoc->m_ln_swing_analysis_periods);
	infd.m_vecFindItem[0][3][0].nMin = 0;
	infd.m_vecFindItem[0][3][0].nMax = 1000;

	
	//////////////////////////////////////////////////////////////////////////
	infd.m_vecFindItem[1][0][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[1][0][0].caption, _T("+Sum"), 64);
	infd.m_vecFindItem[1][0][0].dwStyle = BS_PUSHLIKE;
	if (pDoc->m_ln_swing_add_start.size() > 0)	infd.m_vecFindItem[1][0][0].intItem = 1;
	else 		infd.m_vecFindItem[1][0][0].intItem = 0;

	for (int i = 1; i <= 4; i++)
	{
		infd.m_vecFindItem[1][i][0].nType = CDlgTemplateBuilder::CHECKBOX;
		if (i == 1)	memcpy(infd.m_vecFindItem[1][i][0].caption, _T("+Sum="), 64);
		if (i == 2)	memcpy(infd.m_vecFindItem[1][i][0].caption, _T("或 +Sum="), 64);
		if (i == 3)	memcpy(infd.m_vecFindItem[1][i][0].caption, _T("或 +Sum="), 64);
		if (i == 4)	memcpy(infd.m_vecFindItem[1][i][0].caption, _T("或 +Sum="), 64);
		infd.m_vecFindItem[1][i][0].intItem = 0;

		infd.m_vecFindItem[1][i][1].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[1][i][1].caption, _T("从"), 64);
		infd.m_vecFindItem[1][i][1].strItem = "";

		infd.m_vecFindItem[1][i][2].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[1][i][2].caption, _T("到"), 64);
		infd.m_vecFindItem[1][i][2].strItem = "";

		int num = pDoc->m_ln_swing_add_start.size();
		if (i <= num)
		{
			infd.m_vecFindItem[1][i][0].intItem = 1;
			infd.m_vecFindItem[1][i][1].strItem.Format("%.2f", pDoc->m_ln_swing_add_start[i - 1]);
			infd.m_vecFindItem[1][i][2].strItem.Format("%.2f", pDoc->m_ln_swing_add_end[i - 1]);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	infd.m_vecFindItem[2][0][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[2][0][0].caption, _T("*Sum"), 64);
	infd.m_vecFindItem[2][0][0].dwStyle = BS_PUSHLIKE;
	if (pDoc->m_ln_swing_mul_start.size() > 0)	infd.m_vecFindItem[2][0][0].intItem = 1;
	else 		infd.m_vecFindItem[2][0][0].intItem = 0;

	for (int i = 1; i <= 4; i++)
	{
		infd.m_vecFindItem[2][i][0].nType = CDlgTemplateBuilder::CHECKBOX;
		if (i == 1)	memcpy(infd.m_vecFindItem[2][i][0].caption, _T("*Sum="), 64);
		if (i == 2)	memcpy(infd.m_vecFindItem[2][i][0].caption, _T("或 *Sum="), 64);
		if (i == 3)	memcpy(infd.m_vecFindItem[2][i][0].caption, _T("或 *Sum="), 64);
		if (i == 4)	memcpy(infd.m_vecFindItem[2][i][0].caption, _T("或 *Sum="), 64);
		infd.m_vecFindItem[2][i][0].intItem = 0;

		infd.m_vecFindItem[2][i][1].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[2][i][1].caption, _T("从"), 64);
		infd.m_vecFindItem[2][i][1].strItem = "";

		infd.m_vecFindItem[2][i][2].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[2][i][2].caption, _T("到"), 64);
		infd.m_vecFindItem[2][i][2].strItem = "";

		int num = pDoc->m_ln_swing_mul_start.size();
		if (i <= num)
		{
			infd.m_vecFindItem[2][i][0].intItem = 1;
			infd.m_vecFindItem[2][i][1].strItem.Format("%.2f", pDoc->m_ln_swing_mul_start[i - 1]);
			infd.m_vecFindItem[2][i][2].strItem.Format("%.2f", pDoc->m_ln_swing_mul_end[i - 1]);
		}
	}

	infd.m_vecFindItem[2][5][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[2][5][0].caption, _T("调节参数"), 64);
	if (pDoc->m_ln_swing_mul_adjunstment != 0)
		infd.m_vecFindItem[2][5][0].strItem.Format("%.2f", pDoc->m_ln_swing_mul_adjunstment);
	else
		infd.m_vecFindItem[2][5][0].strItem = " ";

	/////////////////////////////////////////////
	infd.Init(_T("ln涨跌动力设置"), _T("ln涨跌动力设置"));
	if (infd.DoModal() != IDOK)
		return;

	if (1)
	{
		pDoc->m_ln_swing_start = atoi(infd.m_vecFindItem[0][1][0].strItem.GetBuffer());
		pDoc->m_ln_swing_end = atoi(infd.m_vecFindItem[0][2][0].strItem.GetBuffer());
	}
	else
	{
		pDoc->m_ln_swing_start = pDoc->m_ln_swing_end = 0;
	}
	pDoc->m_ln_swing_analysis_periods = atoi(infd.m_vecFindItem[0][3][0].strItem.GetBuffer());


	pDoc->m_ln_swing_add_start.clear();
	pDoc->m_ln_swing_add_end.clear();
	if (infd.m_vecFindItem[1][0][0].intItem)
	{
		for (int i = 1; i <= 4; i++)
		{
			if (infd.m_vecFindItem[1][i][0].intItem)
			{
				pDoc->m_ln_swing_add_start.push_back(String2Double(infd.m_vecFindItem[1][i][1].strItem.GetBuffer()));
				pDoc->m_ln_swing_add_end.push_back(String2Double(infd.m_vecFindItem[1][i][2].strItem.GetBuffer()));
			}
		}
	}

	pDoc->m_ln_swing_mul_start.clear();
	pDoc->m_ln_swing_mul_end.clear();
	pDoc->m_ln_swing_mul_adjunstment = 0;
	if (infd.m_vecFindItem[2][0][0].intItem)
	{
		for (int i = 1; i <= 4; i++)
		{
			if (infd.m_vecFindItem[2][i][0].intItem)
			{
				pDoc->m_ln_swing_mul_start.push_back(String2Double(infd.m_vecFindItem[2][i][1].strItem.GetBuffer()));
				pDoc->m_ln_swing_mul_end.push_back(String2Double(infd.m_vecFindItem[2][i][2].strItem.GetBuffer()));
			}
		}

		pDoc->m_ln_swing_mul_adjunstment = String2Double(infd.m_vecFindItem[2][5][0].strItem.GetBuffer());
	}

	pDoc->SetModifiedFlag();
}


void CLotteryNumberGridView::OnBernoulliSetup()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 25;
	infd.GROUP_NUM_PER_LINE = 3;

	//////////////////////////
	infd.m_vecFindItem[0][0][0].nType = CDlgTemplateBuilder::STATIC;
	memcpy(infd.m_vecFindItem[0][0][0].caption, _T("参与运算期数"), 64);
	/*infd.m_vecFindItem[0][0][0].dwStyle = BS_PUSHLIKE;
	if (pDoc->m_bernoulli_start >= 0 && pDoc->m_bernoulli_start <= pDoc->m_bernoulli_end)
		infd.m_vecFindItem[0][0][0].intItem = 1;
	else
	{
		infd.m_vecFindItem[0][0][0].intItem = 0;
	}*/

	infd.m_vecFindItem[0][1][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][1][0].caption, _T("从"), 64);
	infd.m_vecFindItem[0][1][0].strItem.Format("%d", pDoc->m_bernoulli_start);
	infd.m_vecFindItem[0][1][0].nMin = 0;
	infd.m_vecFindItem[0][1][0].nMax = 100000000;

	infd.m_vecFindItem[0][2][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][2][0].caption, _T("到"), 64);
	infd.m_vecFindItem[0][2][0].strItem.Format("%d", pDoc->m_bernoulli_end);
	infd.m_vecFindItem[0][2][0].nMin = 0;
	infd.m_vecFindItem[0][2][0].nMax = 100000000;

	infd.m_vecFindItem[0][3][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[0][3][0].caption, _T("考虑期差"), 64);
	infd.m_vecFindItem[0][3][0].dwStyle = BS_PUSHLIKE;
	if (pDoc->m_bernoulli_period_difference != 0)
		infd.m_vecFindItem[0][3][0].intItem = 1;
	else
		infd.m_vecFindItem[0][3][0].intItem = 0;

	infd.m_vecFindItem[0][4][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][4][0].caption, _T("累计球控制期数"), 64);
	infd.m_vecFindItem[0][4][0].strItem = "";
	infd.m_vecFindItem[0][4][0].strItem.Format("%d", pDoc->m_bernoulli_analysis_periods);
	infd.m_vecFindItem[0][4][0].nMin = 0;
	infd.m_vecFindItem[0][4][0].nMax = 1000;


	//////////////////////////////////////////////////////////////////////////
	infd.m_vecFindItem[1][0][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[1][0][0].caption, _T("+Sum"), 64);
	infd.m_vecFindItem[1][0][0].dwStyle = BS_PUSHLIKE;
	if (pDoc->m_bernoulli_add_start.size() > 0)	infd.m_vecFindItem[1][0][0].intItem = 1;
	else 		infd.m_vecFindItem[1][0][0].intItem = 0;

	for (int i = 1; i <= 4; i++)
	{
		infd.m_vecFindItem[1][i][0].nType = CDlgTemplateBuilder::CHECKBOX;
		if (i == 1)	memcpy(infd.m_vecFindItem[1][i][0].caption, _T("+Sum="), 64);
		if (i == 2)	memcpy(infd.m_vecFindItem[1][i][0].caption, _T("或 +Sum="), 64);
		if (i == 3)	memcpy(infd.m_vecFindItem[1][i][0].caption, _T("或 +Sum="), 64);
		if (i == 4)	memcpy(infd.m_vecFindItem[1][i][0].caption, _T("或 +Sum="), 64);
		infd.m_vecFindItem[1][i][0].intItem = 0;

		infd.m_vecFindItem[1][i][1].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[1][i][1].caption, _T("从"), 64);
		infd.m_vecFindItem[1][i][1].strItem = "";

		infd.m_vecFindItem[1][i][2].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[1][i][2].caption, _T("到"), 64);
		infd.m_vecFindItem[1][i][2].strItem = "";

		int num = pDoc->m_bernoulli_add_start.size();
		if (i <= num)
		{
			infd.m_vecFindItem[1][i][0].intItem = 1;
			infd.m_vecFindItem[1][i][1].strItem.Format("%.2f", pDoc->m_bernoulli_add_start[i - 1]);
			infd.m_vecFindItem[1][i][2].strItem.Format("%.2f", pDoc->m_bernoulli_add_end[i - 1]);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	infd.m_vecFindItem[2][0][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[2][0][0].caption, _T("*Sum"), 64);
	infd.m_vecFindItem[2][0][0].dwStyle = BS_PUSHLIKE;
	if (pDoc->m_bernoulli_mul_start.size() > 0)	infd.m_vecFindItem[2][0][0].intItem = 1;
	else 		infd.m_vecFindItem[2][0][0].intItem = 0;

	for (int i = 1; i <= 4; i++)
	{
		infd.m_vecFindItem[2][i][0].nType = CDlgTemplateBuilder::CHECKBOX;
		if (i == 1)	memcpy(infd.m_vecFindItem[2][i][0].caption, _T("*Sum="), 64);
		if (i == 2)	memcpy(infd.m_vecFindItem[2][i][0].caption, _T("或 *Sum="), 64);
		if (i == 3)	memcpy(infd.m_vecFindItem[2][i][0].caption, _T("或 *Sum="), 64);
		if (i == 4)	memcpy(infd.m_vecFindItem[2][i][0].caption, _T("或 *Sum="), 64);
		infd.m_vecFindItem[2][i][0].intItem = 0;

		infd.m_vecFindItem[2][i][1].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[2][i][1].caption, _T("从"), 64);
		infd.m_vecFindItem[2][i][1].strItem = "";

		infd.m_vecFindItem[2][i][2].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[2][i][2].caption, _T("到"), 64);
		infd.m_vecFindItem[2][i][2].strItem = "";

		int num = pDoc->m_bernoulli_mul_start.size();
		if (i <= num)
		{
			infd.m_vecFindItem[2][i][0].intItem = 1;
			infd.m_vecFindItem[2][i][1].strItem.Format("%.2f", pDoc->m_bernoulli_mul_start[i - 1]);
			infd.m_vecFindItem[2][i][2].strItem.Format("%.2f", pDoc->m_bernoulli_mul_end[i - 1]);
		}
	}

	infd.m_vecFindItem[2][5][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[2][5][0].caption, _T("调节参数"), 64);
	if (pDoc->m_bernoulli_mul_adjunstment != 0)
		infd.m_vecFindItem[2][5][0].strItem.Format("%.2f", pDoc->m_bernoulli_mul_adjunstment);
	else
		infd.m_vecFindItem[2][5][0].strItem = " ";

	/////////////////////////////////////////////
	infd.Init(_T("伯努利赋值设置"), _T("伯努利赋值设置"));
	if (infd.DoModal() != IDOK)
		return;

	if (1)
	{
		pDoc->m_bernoulli_start = atoi(infd.m_vecFindItem[0][1][0].strItem.GetBuffer());
		pDoc->m_bernoulli_end = atoi(infd.m_vecFindItem[0][2][0].strItem.GetBuffer());
	}
	else
	{
		pDoc->m_bernoulli_start = pDoc->m_bernoulli_end = 0;
	}

	pDoc->m_bernoulli_period_difference = infd.m_vecFindItem[0][3][0].intItem;
	pDoc->m_bernoulli_analysis_periods = atoi(infd.m_vecFindItem[0][4][0].strItem.GetBuffer());


	pDoc->m_bernoulli_add_start.clear();
	pDoc->m_bernoulli_add_end.clear();
	if (infd.m_vecFindItem[1][0][0].intItem)
	{
		for (int i = 1; i <= 4; i++)
		{
			if (infd.m_vecFindItem[1][i][0].intItem)
			{
				pDoc->m_bernoulli_add_start.push_back(String2Double(infd.m_vecFindItem[1][i][1].strItem.GetBuffer()));
				pDoc->m_bernoulli_add_end.push_back(String2Double(infd.m_vecFindItem[1][i][2].strItem.GetBuffer()));
			}
		}
	}

	pDoc->m_bernoulli_mul_start.clear();
	pDoc->m_bernoulli_mul_end.clear();
	pDoc->m_bernoulli_mul_adjunstment = 0;
	if (infd.m_vecFindItem[2][0][0].intItem)
	{
		for (int i = 1; i <= 4; i++)
		{
			if (infd.m_vecFindItem[2][i][0].intItem)
			{
				pDoc->m_bernoulli_mul_start.push_back(String2Double(infd.m_vecFindItem[2][i][1].strItem.GetBuffer()));
				pDoc->m_bernoulli_mul_end.push_back(String2Double(infd.m_vecFindItem[2][i][2].strItem.GetBuffer()));
			}
		}

		pDoc->m_bernoulli_mul_adjunstment = String2Double(infd.m_vecFindItem[2][5][0].strItem.GetBuffer());
	}

	pDoc->SetModifiedFlag();
}


void CLotteryNumberGridView::OnPotentialSetup()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 25;
	infd.GROUP_NUM_PER_LINE = 3;

	//////////////////////////
	infd.m_vecFindItem[0][0][0].nType = CDlgTemplateBuilder::STATIC;
	memcpy(infd.m_vecFindItem[0][0][0].caption, _T("参与运算期数"), 64);
	/*infd.m_vecFindItem[0][0][0].dwStyle = BS_PUSHLIKE;
	if (pDoc->m_potential_start >= 0 && pDoc->m_potential_start <= pDoc->m_potential_end)
		infd.m_vecFindItem[0][0][0].intItem = 1;
	else
	{
		infd.m_vecFindItem[0][0][0].intItem = 0;
	}*/

	infd.m_vecFindItem[0][1][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][1][0].caption, _T("从"), 64);
	infd.m_vecFindItem[0][1][0].strItem.Format("%d", pDoc->m_potential_start);
	infd.m_vecFindItem[0][1][0].nMin = 0;
	infd.m_vecFindItem[0][1][0].nMax = 100000000;

	infd.m_vecFindItem[0][2][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][2][0].caption, _T("到"), 64);
	infd.m_vecFindItem[0][2][0].strItem.Format("%d", pDoc->m_potential_end);
	infd.m_vecFindItem[0][2][0].nMin = 0;
	infd.m_vecFindItem[0][2][0].nMax = 100000000;

	infd.m_vecFindItem[0][3][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[0][3][0].caption, _T("考虑期差"), 64);
	infd.m_vecFindItem[0][3][0].dwStyle = BS_PUSHLIKE;
	if (pDoc->m_potential_period_difference != 0)
		infd.m_vecFindItem[0][3][0].intItem = 1;
	else
		infd.m_vecFindItem[0][3][0].intItem = 0;

	infd.m_vecFindItem[0][4][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][4][0].caption, _T("累计球控制期数"), 64);
	infd.m_vecFindItem[0][4][0].strItem = "";
	infd.m_vecFindItem[0][4][0].strItem.Format("%d", pDoc->m_potential_analysis_periods);
	infd.m_vecFindItem[0][4][0].nMin = 0;
	infd.m_vecFindItem[0][4][0].nMax = 1000;

	infd.m_vecFindItem[0][5][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[0][5][0].caption, _T("启用绝对值"), 64);
	infd.m_vecFindItem[0][5][0].dwStyle = BS_PUSHLIKE;
	if (pDoc->m_potential_period_absolute != 0)
		infd.m_vecFindItem[0][5][0].intItem = 1;
	else
		infd.m_vecFindItem[0][5][0].intItem = 0;

	//////////////////////////////////////////////////////////////////////////
	infd.m_vecFindItem[1][0][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[1][0][0].caption, _T("+Sum"), 64);
	infd.m_vecFindItem[1][0][0].dwStyle = BS_PUSHLIKE;
	if (pDoc->m_potential_add_start.size() > 0)	infd.m_vecFindItem[1][0][0].intItem = 1;
	else 		infd.m_vecFindItem[1][0][0].intItem = 0;

	for (int i = 1; i <= 4; i++)
	{
		infd.m_vecFindItem[1][i][0].nType = CDlgTemplateBuilder::CHECKBOX;
		if (i == 1)	memcpy(infd.m_vecFindItem[1][i][0].caption, _T("+Sum="), 64);
		if (i == 2)	memcpy(infd.m_vecFindItem[1][i][0].caption, _T("或 +Sum="), 64);
		if (i == 3)	memcpy(infd.m_vecFindItem[1][i][0].caption, _T("或 +Sum="), 64);
		if (i == 4)	memcpy(infd.m_vecFindItem[1][i][0].caption, _T("或 +Sum="), 64);
		infd.m_vecFindItem[1][i][0].intItem = 0;

		infd.m_vecFindItem[1][i][1].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[1][i][1].caption, _T("从"), 64);
		infd.m_vecFindItem[1][i][1].strItem = "";

		infd.m_vecFindItem[1][i][2].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[1][i][2].caption, _T("到"), 64);
		infd.m_vecFindItem[1][i][2].strItem = "";

		int num = pDoc->m_potential_add_start.size();
		if (i <= num)
		{
			infd.m_vecFindItem[1][i][0].intItem = 1;
			infd.m_vecFindItem[1][i][1].strItem.Format("%.2f", pDoc->m_potential_add_start[i - 1]);
			infd.m_vecFindItem[1][i][2].strItem.Format("%.2f", pDoc->m_potential_add_end[i - 1]);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	infd.m_vecFindItem[2][0][0].nType = CDlgTemplateBuilder::CHECKBOX;
	memcpy(infd.m_vecFindItem[2][0][0].caption, _T("*Sum"), 64);
	infd.m_vecFindItem[2][0][0].dwStyle = BS_PUSHLIKE;
	if (pDoc->m_potential_mul_start.size() > 0)	infd.m_vecFindItem[2][0][0].intItem = 1;
	else 		infd.m_vecFindItem[2][0][0].intItem = 0;

	for (int i = 1; i <= 4; i++)
	{
		infd.m_vecFindItem[2][i][0].nType = CDlgTemplateBuilder::CHECKBOX;
		if (i == 1)	memcpy(infd.m_vecFindItem[2][i][0].caption, _T("*Sum="), 64);
		if (i == 2)	memcpy(infd.m_vecFindItem[2][i][0].caption, _T("或 *Sum="), 64);
		if (i == 3)	memcpy(infd.m_vecFindItem[2][i][0].caption, _T("或 *Sum="), 64);
		if (i == 4)	memcpy(infd.m_vecFindItem[2][i][0].caption, _T("或 *Sum="), 64);
		infd.m_vecFindItem[2][i][0].intItem = 0;

		infd.m_vecFindItem[2][i][1].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[2][i][1].caption, _T("从"), 64);
		infd.m_vecFindItem[2][i][1].strItem = "";

		infd.m_vecFindItem[2][i][2].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[2][i][2].caption, _T("到"), 64);
		infd.m_vecFindItem[2][i][2].strItem = "";

		int num = pDoc->m_potential_mul_start.size();
		if (i <= num)
		{
			infd.m_vecFindItem[2][i][0].intItem = 1;
			infd.m_vecFindItem[2][i][1].strItem.Format("%.2f", pDoc->m_potential_mul_start[i - 1]);
			infd.m_vecFindItem[2][i][2].strItem.Format("%.2f", pDoc->m_potential_mul_end[i - 1]);
		}
	}

	infd.m_vecFindItem[2][5][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[2][5][0].caption, _T("调节参数"), 64);
	if (pDoc->m_potential_mul_adjunstment != 0)
		infd.m_vecFindItem[2][5][0].strItem.Format("%.2f", pDoc->m_potential_mul_adjunstment);
	else
		infd.m_vecFindItem[2][5][0].strItem = " ";

	/////////////////////////////////////////////
	infd.Init(_T("势能赋值设置"), _T("势能赋值设置"));
	if (infd.DoModal() != IDOK)
		return;

	if (1)
	{
		pDoc->m_potential_start = atoi(infd.m_vecFindItem[0][1][0].strItem.GetBuffer());
		pDoc->m_potential_end = atoi(infd.m_vecFindItem[0][2][0].strItem.GetBuffer());
	}
	else
	{
		pDoc->m_potential_start = pDoc->m_potential_end = 0;
	}

	pDoc->m_potential_period_difference = infd.m_vecFindItem[0][3][0].intItem;
	pDoc->m_potential_analysis_periods = atoi(infd.m_vecFindItem[0][4][0].strItem.GetBuffer());
	pDoc->m_potential_period_absolute = infd.m_vecFindItem[0][5][0].intItem;


	pDoc->m_potential_add_start.clear();
	pDoc->m_potential_add_end.clear();
	if (infd.m_vecFindItem[1][0][0].intItem)
	{
		for (int i = 1; i <= 4; i++)
		{
			if (infd.m_vecFindItem[1][i][0].intItem)
			{
				pDoc->m_potential_add_start.push_back(String2Double(infd.m_vecFindItem[1][i][1].strItem.GetBuffer()));
				pDoc->m_potential_add_end.push_back(String2Double(infd.m_vecFindItem[1][i][2].strItem.GetBuffer()));
			}
		}
	}

	pDoc->m_potential_mul_start.clear();
	pDoc->m_potential_mul_end.clear();
	pDoc->m_potential_mul_adjunstment = 0;
	if (infd.m_vecFindItem[2][0][0].intItem)
	{
		for (int i = 1; i <= 4; i++)
		{
			if (infd.m_vecFindItem[2][i][0].intItem)
			{
				pDoc->m_potential_mul_start.push_back(String2Double(infd.m_vecFindItem[2][i][1].strItem.GetBuffer()));
				pDoc->m_potential_mul_end.push_back(String2Double(infd.m_vecFindItem[2][i][2].strItem.GetBuffer()));
			}
		}

		pDoc->m_potential_mul_adjunstment = String2Double(infd.m_vecFindItem[2][5][0].strItem.GetBuffer());
	}

	pDoc->SetModifiedFlag();
}


int CLotteryNumberGridView::GoForwardByOffset(vector<LotteryNumber> & vecln, int first_periods, int offset, int & index)
{
	if (offset < 0)  return 0;

	for (int i = 0 ; i< vecln.size(); i++)
	{
		if (vecln[i].periods >= first_periods)
		{
			if (offset == 0)
			{
				index = i;
				return vecln[i].periods;
			}
			offset--;
		}
	}
	return 0;
}

void CLotteryNumberGridView::AssignmentAnalysis(string aa, int analysis_periods)
{
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();

	bool bSpecial = false;
	if (pDoc->m_bContainSpecial && pDoc->m_lottery_info.specialnum > 0)
		bSpecial = true;

	int nExpected = pDoc->m_lottery_info.regularnum + (bSpecial ? 1 : 0);

	if (analysis_periods <= 0)
	{
		AfxMessageBox(_T("请先设置累计球控制期数！"));
		return;
	}

	// 获取 现有已开奖的号码
	vector<LotteryNumber> vecln = pDoc->FetchAwardRecord();

	int start;
	int end;
	int pos;
	int cnt = 0;
	LotteryNumber ln;
	stringstream ss;

	// 先设置参与运算期数
	if (aa == "_π赋值") {
		if (pDoc->m_pai_start <= 0 || pDoc->m_pai_end <= 0 || pDoc->m_pai_start >= pDoc->m_pai_end) {
			AfxMessageBox(_T("请先设置参与运算期数！"));
			return;
		}
		else {
			start = pDoc->m_pai_start;
			end = pDoc->m_pai_end;
		}
	}
	else if (aa == "_Lnπ赋值") {
		if (pDoc->m_ln_pai_start <= 0 || pDoc->m_ln_pai_end <= 0 || pDoc->m_ln_pai_start >= pDoc->m_ln_pai_end) {
			AfxMessageBox(_T("请先设置参与运算期数！"));
			return;
		}
		else {
			start = pDoc->m_ln_pai_start;
			end = pDoc->m_ln_pai_end;
		}
	}
	else if (aa == "_涨落动力赋值") {
		if (pDoc->m_swing_start <= 0 || pDoc->m_swing_end <= 0 || pDoc->m_swing_start >= pDoc->m_swing_end) {
			AfxMessageBox(_T("请先设置参与运算期数！"));
			return;
		}
		else {
			start = pDoc->m_swing_start;
			end = pDoc->m_swing_end;
		}
	}
	else if (aa == "_Ln 涨落动力赋值") {

		if (pDoc->m_ln_swing_start <= 0 || pDoc->m_ln_swing_end <= 0 || pDoc->m_ln_swing_start >= pDoc->m_ln_swing_end) {
			AfxMessageBox(_T("请先设置参与运算期数！"));
			return;
		}
		else {
			start = pDoc->m_ln_swing_start;
			end = pDoc->m_ln_swing_end;
		}
	}
	else if (aa == "_伯努利赋值") {
		if (pDoc->m_bernoulli_start <= 0 || pDoc->m_bernoulli_end <= 0 || pDoc->m_bernoulli_start >= pDoc->m_bernoulli_end) {
			AfxMessageBox(_T("请先设置参与运算期数！"));
			return;
		}
		else {
			start = pDoc->m_bernoulli_start;
			end = pDoc->m_bernoulli_end;
		}
	}
	else // if (aa == "_势能赋值")
	{
		if (pDoc->m_potential_start <= 0 || pDoc->m_potential_end <= 0 || pDoc->m_potential_start >= pDoc->m_potential_end) {
			AfxMessageBox(_T("请先设置参与运算期数！"));
			return;
		}
		else {
			start = pDoc->m_potential_start;
			end = pDoc->m_potential_end;
		}
	}

	if (vecln.size() == 0 || vecln.size() <= analysis_periods)
	{
		AfxMessageBox(_T("已开奖的可供分析的记录太少！"));
		return;
	}

	// 打开 “保存文件”对话框
	CString  filter = _T("csv文件|*.csv||");
	string filename = pDoc->m_lottery_info.name + aa;
	CFileDialog cfd(FALSE, _T("csv"), filename.c_str(),
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter);
	if (cfd.DoModal() != IDOK)
		return;

	filename = cfd.GetPathName().GetBuffer();
	ofstream outfile(filename.c_str(), ofstream::out);


	int start_pos, end_pos;
	start_pos = 0;
	end_pos = vecln.size() - 1;
	if (m_start_number > 0) {
		GoForwardByOffset(vecln, m_start_number, 0, start_pos);
	}
	if (m_end_number > 0) {
		GoForwardByOffset(vecln, m_end_number, 0, end_pos);
	}
	map<int, Assignment> mapPai = pDoc->AssignmentFilterPre(vecln, start, end, analysis_periods);

	do {
	
		ln = vecln[start_pos];
		if (aa == "_π赋值")
			pDoc->PaiFilter(mapPai, ln);
		else if (aa == "_Lnπ赋值")
			pDoc->LnPaiFilter(mapPai, ln);
		else if (aa == "_涨落动力赋值")
			pDoc->SwingPowerFilter(mapPai, ln);
		else if (aa == "_Ln 涨落动力赋值")
			pDoc->LnSwingPowerFilter(mapPai, ln);
		else if (aa == "_伯努利赋值")
			pDoc->BernoulliFilter(mapPai, ln);
		else // if (aa == "_势能赋值")
			pDoc->PotentialFilter(mapPai, ln);

		ss.str("");
		ss << Int2String(ln.periods, "%6d");
		for (int i = 0; i < nExpected; i++)
		{
			int m;
			if (i < pDoc->m_lottery_info.regularnum)
				m = ln.number[i];
			else
				m = ln.special_number;
			ss << "," << Int2String(m, "%02d");
			ss << "," << Double2String(mapPai[m].dResult, "%.6f");
		}
		ss << endl;
		outfile << ss.str();
		cnt++;

		start_pos++;
	} while (start_pos <= end_pos);

	outfile.close();

	if (cnt == 0)
	{
		DeleteFile(filename.c_str());
	}
	else
	{
		// 在资源管理器中弹出并且选中文件
		CString strParam;
		strParam.Format(" /select, %s", filename.c_str());
		SHELLEXECUTEINFO shex = { 0 };
		shex.cbSize = sizeof(SHELLEXECUTEINFO);
		shex.lpFile = _T("explorer");
		shex.lpParameters = strParam.GetBuffer();
		shex.lpVerb = NULL;  // _T("open");	
		shex.nShow = SW_SHOWDEFAULT;
		shex.lpDirectory = NULL;
		ShellExecuteEx(&shex);
	}
}

void CLotteryNumberGridView::OnPaiAnalysis()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();

	AssignmentAnalysis("_π赋值", pDoc->m_pai_analysis_periods);
}

void CLotteryNumberGridView::OnLnPaiAnalysis()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();

	AssignmentAnalysis("_Lnπ赋值", pDoc->m_ln_pai_analysis_periods);
}


void CLotteryNumberGridView::OnSwingPowerAnalysis()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();
	AssignmentAnalysis("_涨落动力赋值", pDoc->m_swing_analysis_periods);
}


void CLotteryNumberGridView::OnLnSwingPowerAnalysis()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();
	AssignmentAnalysis("_Ln 涨落动力赋值", pDoc->m_ln_swing_analysis_periods);
}


void CLotteryNumberGridView::OnBernoulliAnalysis()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();
	AssignmentAnalysis("_伯努利赋值", pDoc->m_bernoulli_analysis_periods);
}


void CLotteryNumberGridView::OnPotentialAnalysis()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();
	AssignmentAnalysis("_势能赋值", pDoc->m_potential_analysis_periods);
}
