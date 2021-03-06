// TrendDistributionGridView.cpp: 实现文件
//

#include "stdafx.h"
#include "Lottery.h"
#include "TrendDistributionGridView.h"

#include "LotteryDoc.h"
#include "CalcLotteryNumberList.h"

#include "ChartDlg.h"
#include "GridDlg.h"
#include "DyncItemGroupDlg.h"

// CTrendDistributionGridView

IMPLEMENT_DYNCREATE(CTrendDistributionGridView, CBaseGridCtlView)

CTrendDistributionGridView::CTrendDistributionGridView()
{
	
}

CTrendDistributionGridView::~CTrendDistributionGridView()
{
}

BEGIN_MESSAGE_MAP(CTrendDistributionGridView, CBaseGridCtlView)
	ON_COMMAND(ID_LOTTERY_RANGE_SET, &CTrendDistributionGridView::OnLotteryRangeSet)
	ON_COMMAND(ID_CUMULATIVE_NUMBER_GRAPH, &CTrendDistributionGridView::OnCumulativeNumberGraph)
	ON_COMMAND(ID_INHERIT_NUMBER_GRID, &CTrendDistributionGridView::OnInheritNumberGrid)
	ON_COMMAND(ID_THEORY_INHERIT_NUMBER_GRID, &CTrendDistributionGridView::OnTheoryInheritNumberGrid)
END_MESSAGE_MAP()


// CTrendDistributionGridView 绘图

void CTrendDistributionGridView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO:  在此添加绘制代码
}


// CTrendDistributionGridView 诊断

#ifdef _DEBUG
void CTrendDistributionGridView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CTrendDistributionGridView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CTrendDistributionGridView 消息处理程序


void CTrendDistributionGridView::OnInitialUpdate()
{
	CBaseGridCtlView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	
	//Startup();

}



void CTrendDistributionGridView::ProcessXmlWParam(string msgType, string code, string model, \
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
	}
}


void CTrendDistributionGridView::RedrawGridCtrl(CCalculateTask * cse)
{
	CCalcLotteryNumberList * pStockInfoEx = (CCalcLotteryNumberList *)cse;
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();

	LotteryInfo lotto = pDoc->m_lottery_info;

	/*CString strTitle;
	strTitle.Format(_T("走势分布图-%s"), lotto.name.c_str());
	this->GetParent()->SetWindowText(strTitle);*/

	if (m_pGridCtrl)
	{
		CLotteryApp * pApp = (CLotteryApp *)AfxGetApp();

		try {
			// 增加了 "间隔期数" "累计次数"
			m_pGridCtrl->SetRowCount(pStockInfoEx->m_vec_lottery_number.size() + 3);
			m_pGridCtrl->SetColumnCount(lotto.totalnum + 1);
			m_pGridCtrl->SetFixedRowCount(1);
			m_pGridCtrl->SetFixedColumnCount(1);
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
					else if (col <= lotto.totalnum) val = Int2String(col);
					
					Item.strText.Format(_T("%s"), val.c_str());
				}
				else if (row == m_pGridCtrl->GetRowCount() -1)
				{
					string val = "";
					if (col == 0)	val = "间隔期数";
					else if (col <= lotto.totalnum) val = Int2String(m_interval[col]);
					Item.strText.Format(_T("%s"), val.c_str());
				}
				else if (row == m_pGridCtrl->GetRowCount() - 2)
				{
					string val = "";
					if (col == 0)	val = "累计次数";
					else if (col <= lotto.totalnum) val = Int2String(m_accumulative[col]);

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
					else if (col <= lotto.totalnum)
					{
						bool bFound = false;
						for (int k = 0; k < lotto.regularnum; k++)
						{
							if (pStockInfoEx->m_vec_lottery_number[row - 1].number[k] == col)
							{
								m_accumulative[col]++;
								val = "●";
								bFound = true;
							}
						}
						if (lotto.specialnum == 1)
						{
							if (pStockInfoEx->m_vec_lottery_number[row - 1].special_number == col)
							{
								m_accumulative[col]++;
								val = "☆";
								bFound = true;
							}
						}

						if (bFound)	m_interval[col] = 0;
						else m_interval[col]++;
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


void CTrendDistributionGridView::OnLotteryRangeSet()
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
	
	infd.m_vecFindItem[0][0][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][0][0].caption, _T("开始期数"), 64);
	infd.m_vecFindItem[0][0][0].strItem.Format("%d", nMin);

	infd.m_vecFindItem[0][1][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][1][0].caption, _T("结束期数"), 64);
	infd.m_vecFindItem[0][1][0].strItem.Format("%d", nMax);

	infd.Init(_T("期数范围"), _T("期数范围"));
	if (infd.DoModal() == IDOK)
	{
		m_start_number = atoi(infd.m_vecFindItem[0][0][0].strItem.GetBuffer());
		m_end_number = atoi(infd.m_vecFindItem[0][1][0].strItem.GetBuffer());;

		Startup();
	}

}


void CTrendDistributionGridView::OnCumulativeNumberGraph()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();

	LotteryInfo lotto = pDoc->m_lottery_info;


	CChartBarDlg dlg;
	for (int i = 1; i <= lotto.totalnum; i++)
	{
		dlg.m_vecX.push_back(i);
		dlg.m_vecY.push_back(m_accumulative[i]);
	}
	dlg.DoModal();
}


void CTrendDistributionGridView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO: 在此添加专用代码和/或调用基类
	Startup();
}


void CTrendDistributionGridView::OnInheritNumberGrid()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();

	LotteryInfo lotto = pDoc->m_lottery_info;

	int Inherit[10];
	for (int i = 0; i < 10; i++)	Inherit[i] = 0;
	for (int i = 1; i < m_vec_lottery_number.size(); i++)
	{
		int nInherit = 0;
		for (int j = 0; j < lotto.regularnum; j++)
		{
			for (int m = 0; m < lotto.regularnum; m++)
			{
				if (m_vec_lottery_number[i].number[j] == m_vec_lottery_number[i - 1].number[m])
				{
					nInherit++;
					break;
				}
			}
		}
		Inherit[nInherit] ++;
	}

	//	展示
	CGridDlg gridDlg;
	gridDlg.m_vecHeader.push_back(_T("从上期继承号码数"));
	gridDlg.m_vecHeader.push_back("数量");
	gridDlg.m_vecHeader.push_back("比例（%）");

	for (int i = 0; i <=  lotto.regularnum; i++)
	{
		string str = Int2String(i);
		string str1 = Int2String(Inherit[i]);
		string str2 = Double2String((Inherit[i] * 1.0 / (m_vec_lottery_number.size()-1)) * 100.0);
		vector<string> vec;
		vec.push_back(str);
		vec.push_back(str1);
		vec.push_back(str2);
		gridDlg.m_vecData.push_back(vec);

	}

	gridDlg.DoModal();
}


void CTrendDistributionGridView::OnTheoryInheritNumberGrid()
{
	// TODO: 在此添加命令处理程序代码
	CLotteryDoc * pDoc = (CLotteryDoc *)GetDocument();

	LotteryInfo lotto = pDoc->m_lottery_info;

	long long Total;
	long long Inherit[10];
	for (int i = 0; i < 10; i++)	Inherit[i] = 0;

	{
		//  总组合数
		long long n = 1;
		int m = 1;
		for (int i = 0; i < lotto.regularnum; i++)
		{
			n *= (lotto.totalnum  - i);
			m *= (lotto.regularnum - i);
		}
		Total = n / m;
	}

	{
		//  0 个继承, 在剩余的 lotto.totalnum- lotto.regularnum 个号码中选 lotto.regularnum 的组合数
		long long n = 1;
		int m = 1;
		for (int i = 0; i < lotto.regularnum; i++)
		{
			n *= (lotto.totalnum - lotto.regularnum - i);
			m *= (lotto.regularnum - i);
		}
		Inherit[0] = n / m;
	}
	
	{
		//  1 个继承, 在剩余的 lotto.totalnum- lotto.regularnum 个号码中选 lotto.regularnum - 1 的组合数， 再乘 lotto.regularnum
		long long n = 1;
		int m = 1;
		for (int i = 0; i < lotto.regularnum - 1; i++)
		{
			n *= (lotto.totalnum - lotto.regularnum - i);
			m *= (lotto.regularnum - 1 - i);
		}
		Inherit[1] = lotto.regularnum * n / m ;
	}

	{
		//  2 个继承, 在剩余的 lotto.totalnum- lotto.regularnum 个号码中选 lotto.regularnum - 2 的组合数， 再乘 lotto.regularnum 取 2 的组合数
		long long n = 1;
		int m = 1;
		for (int i = 0; i < lotto.regularnum - 2; i++)
		{
			n *= (lotto.totalnum - lotto.regularnum - i);
			m *= (lotto.regularnum -2 - i);
		}
		Inherit[2] = n / m;

		n = 1;
		for (int i = 0; i < 2; i++)
		{
			n *= ( lotto.regularnum - i);
		}
		n = n / 2;
		Inherit[2] = Inherit[2] * n;
	}

	{
		//  3个继承, 在剩余的 lotto.totalnum- lotto.regularnum 个号码中选 lotto.regularnum - 3 的组合数， 再乘 lotto.regularnum 取 3 的组合数
		long long n = 1;
		int m = 1;
		for (int i = 0; i < lotto.regularnum - 3; i++)
		{
			n *= (lotto.totalnum - lotto.regularnum - i);
			m *= (lotto.regularnum - 3 - i);
		}
		Inherit[3] = n / m;

		n = 1;
		for (int i = 0; i < 3; i++)
		{
			n *= (lotto.regularnum - i);
		}
		n = n /  (3 * 2);
		Inherit[3] = Inherit[3] * n;
	}

	//	展示
	CGridDlg gridDlg;
	gridDlg.m_vecHeader.push_back(_T("从上期继承号码数"));
	gridDlg.m_vecHeader.push_back("注数");
	gridDlg.m_vecHeader.push_back("比例（%）");

	for (int i = 0; i <= 3; i++)
	{
		string str = Int2String(i);
		string str1 = Int2String(Inherit[i]);
		string str2 = Double2String((Inherit[i] * 1.0 / Total) * 100.0);
		vector<string> vec;
		vec.push_back(str);
		vec.push_back(str1);
		vec.push_back(str2);
		gridDlg.m_vecData.push_back(vec);

	}

	vector<string> vec;
	vec.push_back(_T("总注数"));
	vec.push_back(Int2String(Total));
	vec.push_back("100.00");
	gridDlg.m_vecData.push_back(vec);

	gridDlg.DoModal();
}
