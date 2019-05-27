// AutoDownloadView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "IndividualShareDoc.h"
#include "AutoDownloadView.h"
#include "BaseMessageElementsParser.h"


//////////////////////////////////////////////////////////////////////////
// CAutoDownloadView

IMPLEMENT_DYNCREATE(CAutoDownloadView, CEditView)

CAutoDownloadView::CAutoDownloadView()
{
	m_bDownloading = NULL;
	m_bAutoClose = false;
}

CAutoDownloadView::~CAutoDownloadView()
{
}

BEGIN_MESSAGE_MAP(CAutoDownloadView, CEditView)
	ON_COMMAND(ID_AUTO_DOWNLOAD_LIST, &CAutoDownloadView::OnAutoDownloadList)
	ON_COMMAND(ID_DOWNLOAD_STOCKINFO_SHENZHEN_ASHARE, &CAutoDownloadView::OnDownloadStockinfoShenzhenAshare)
	ON_COMMAND(ID_DOWNLOAD_STOCKINFO_SMALLANDMID, &CAutoDownloadView::OnDownloadStockinfoSmallandmid)
	ON_COMMAND(ID_DOWNLOAD_STOCKINFO_ENTERPRENEUR, &CAutoDownloadView::OnDownloadStockinfoEnterpreneur)
	ON_COMMAND(ID_DOWNLOAD_TRADINGDAILY_ENTERPRENUER, &CAutoDownloadView::OnDownloadTradingdailyEnterprenuer)
	ON_COMMAND(ID_DOWNLOAD_FINANCE_ENTERPRENUER, &CAutoDownloadView::OnDownloadFinanceEnterprenuer)
	ON_MESSAGE(WM_USER_LOG_1, OnUserLogWParam_1)
	ON_MESSAGE(WM_USER_XML, OnUserXmlWParam)
	ON_COMMAND(ID_DOWNLOAD_STOCKINFO_SHANGHAI, &CAutoDownloadView::OnDownloadStockinfoShanghai)
	ON_COMMAND(ID_DOWNLOAD_TRADINGDAILY_SHENZHEN_ASHARE, &CAutoDownloadView::OnDownloadTradingdailyShenzhenAshare)
	ON_COMMAND(ID_DOWNLOAD_TRADINGDAILY_SMALL_AND_MID, &CAutoDownloadView::OnDownloadTradingdailySmallAndMid)
	ON_COMMAND(ID_DOWNLOAD_TRADINGDAILY_SHANGHAISTOCK, &CAutoDownloadView::OnDownloadTradingdailyShanghaistock)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOAD_TRADINGDAILY_SHANGHAISTOCK, &CAutoDownloadView::OnUpdateDownloadTradingdailyShanghaistock)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOAD_TRADINGDAILY_SHENZHEN_ASHARE, &CAutoDownloadView::OnUpdateDownloadTradingdailyShenzhenAshare)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOAD_TRADINGDAILY_SMALL_AND_MID, &CAutoDownloadView::OnUpdateDownloadTradingdailySmallAndMid)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOAD_TRADINGDAILY_ENTERPRENUER, &CAutoDownloadView::OnUpdateDownloadTradingdailyEnterprenuer)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOAD_FINANCE_ENTERPRENUER, &CAutoDownloadView::OnUpdateDownloadFinanceEnterprenuer)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOAD_FINANCE_SHANGHAI, &CAutoDownloadView::OnUpdateDownloadFinanceShanghai)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOAD_FINANCE_SMALL_AND_MID, &CAutoDownloadView::OnUpdateDownloadFinanceSmallAndMid)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOAD_FINANCE_SHENZHEN_ASHARE, &CAutoDownloadView::OnUpdateDownloadFinanceShenzhenAshare)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOAD_CAPITIALSTRUCT_SHANGHAI, &CAutoDownloadView::OnUpdateDownloadCapitialstructShanghai)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOAD_STOCKINFO_SHENZHEN_ASHARE, &CAutoDownloadView::OnUpdateDownloadStockinfoShenzhenAshare)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOAD_STOCKINFO_SMALLANDMID, &CAutoDownloadView::OnUpdateDownloadStockinfoSmallandmid)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOAD_STOCKINFO_ENTERPRENEUR, &CAutoDownloadView::OnUpdateDownloadStockinfoEnterpreneur)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOAD_STOCKINFO_SHANGHAI, &CAutoDownloadView::OnUpdateDownloadStockinfoShanghai)
	ON_COMMAND(ID_DOWNLOAD_FINANCE_SHENZHEN_ASHARE, &CAutoDownloadView::OnDownloadFinanceShenzhenAshare)
	ON_COMMAND(ID_DOWNLOAD_FINANCE_SMALL_AND_MID, &CAutoDownloadView::OnDownloadFinanceSmallAndMid)
	ON_COMMAND(ID_DOWNLOAD_FINANCE_SHANGHAI, &CAutoDownloadView::OnDownloadFinanceShanghai)
	ON_COMMAND(ID_DOWNLOAD_CAPITIALSTRUCT_SHANGHAI, &CAutoDownloadView::OnDownloadCapitialstructShanghai)
	ON_COMMAND(ID_DOWNLOAD_CAPITIALSTRUCT_SHENZHEN, &CAutoDownloadView::OnDownloadCapitialstructShenzhen)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOAD_CAPITIALSTRUCT_SHENZHEN, &CAutoDownloadView::OnUpdateDownloadCapitialstructShenzhen)
	ON_COMMAND(ID_DOWNLOAD_SOUHU_STOCK_PLATE, &CAutoDownloadView::OnDownloadSouhuStockPlate)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOAD_SOUHU_STOCK_PLATE, &CAutoDownloadView::OnUpdateDownloadSouhuStockPlate)
	ON_COMMAND(ID_DOWNLOAD_CAPITALFLOW_SHENZHEN_ASHARE, &CAutoDownloadView::OnDownloadCapitalflowShenzhenAshare)
	ON_COMMAND(ID_DOWNLOAD_CAPITALFLOW_SMALL_AND_MID, &CAutoDownloadView::OnDownloadCapitalflowSmallAndMid)
	ON_COMMAND(ID_DOWNLOAD_CAPITALFLOW_ENTERPRENUER, &CAutoDownloadView::OnDownloadCapitalflowEnterprenuer)
	ON_COMMAND(ID_DOWNLOAD_CAPITALFLOW_SHANGHAISTOCK, &CAutoDownloadView::OnDownloadCapitalflowShanghaistock)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOAD_CAPITALFLOW_SHENZHEN_ASHARE, &CAutoDownloadView::OnUpdateDownloadCapitalflowShenzhenAshare)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOAD_CAPITALFLOW_SMALL_AND_MID, &CAutoDownloadView::OnUpdateDownloadCapitalflowSmallAndMid)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOAD_CAPITALFLOW_ENTERPRENUER, &CAutoDownloadView::OnUpdateDownloadCapitalflowEnterprenuer)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOAD_CAPITALFLOW_SHANGHAISTOCK, &CAutoDownloadView::OnUpdateDownloadCapitalflowShanghaistock)
	ON_COMMAND(ID_DOWNLOAD_PERFORMANCE_FORECAST, &CAutoDownloadView::OnDownloadPerformanceForecast)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOAD_PERFORMANCE_FORECAST, &CAutoDownloadView::OnUpdateDownloadPerformanceForecast)
	ON_COMMAND(ID_DOWNLOAD_INCOME_COMPOSITION, &CAutoDownloadView::OnDownloadIncomeComposition)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOAD_INCOME_COMPOSITION, &CAutoDownloadView::OnUpdateDownloadIncomeComposition)
	ON_COMMAND(ID_DOWNLOAD_FUND_HOLD_SHENZHEN_ASHARE, &CAutoDownloadView::OnDownloadFundHoldShenzhenAshare)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOAD_FUND_HOLD_SHENZHEN_ASHARE, &CAutoDownloadView::OnUpdateDownloadFundHoldShenzhenAshare)
	ON_COMMAND(ID_DOWNLOAD_FUND_HOLD_ENTERPRENUER, &CAutoDownloadView::OnDownloadFundHoldEnterprenuer)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOAD_FUND_HOLD_ENTERPRENUER, &CAutoDownloadView::OnUpdateDownloadFundHoldEnterprenuer)
	ON_COMMAND(ID_DOWNLOAD_FUND_HOLD_SMALL_AND_MID, &CAutoDownloadView::OnDownloadFundHoldSmallAndMid)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOAD_FUND_HOLD_SMALL_AND_MID, &CAutoDownloadView::OnUpdateDownloadFundHoldSmallAndMid)
	ON_COMMAND(ID_DOWNLOAD_FUND_HOLD_SHANGHAI, &CAutoDownloadView::OnDownloadFundHoldShanghai)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOAD_FUND_HOLD_SHANGHAI, &CAutoDownloadView::OnUpdateDownloadFundHoldShanghai)
	ON_COMMAND(ID_DOWNLOAD_HEXUN_RESEARCH_PAPER, &CAutoDownloadView::OnDownloadHexunResearchPaper)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOAD_HEXUN_RESEARCH_PAPER, &CAutoDownloadView::OnUpdateDownloadHexunResearchPaper)
	ON_COMMAND(ID_DOWNLOAD_HUSHENG300_INDEX, &CAutoDownloadView::OnDownloadHusheng300Index)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOAD_HUSHENG300_INDEX, &CAutoDownloadView::OnUpdateDownloadHusheng300Index)
	ON_COMMAND(ID_DOWNLOAD_CFFEX_FUTURE_DAILY, &CAutoDownloadView::OnDownloadCffexFutureDaily)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOAD_CFFEX_FUTURE_DAILY, &CAutoDownloadView::OnUpdateDownloadCffexFutureDaily)
	ON_COMMAND(ID_DOWNLOAD_STATSGOV_HGYD, &CAutoDownloadView::OnDownloadStatsgovHgyd)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOAD_STATSGOV_HGYD, &CAutoDownloadView::OnUpdateDownloadStatsgovHgyd)
	ON_COMMAND(ID_DOWNLOAD_STATSGOV_HGJD, &CAutoDownloadView::OnDownloadStatsgovHgjd)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOAD_STATSGOV_HGJD, &CAutoDownloadView::OnUpdateDownloadStatsgovHgjd)
	ON_COMMAND(ID_DOWNLOAD_STATSGOV_HGND, &CAutoDownloadView::OnDownloadStatsgovHgnd)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOAD_STATSGOV_HGND, &CAutoDownloadView::OnUpdateDownloadStatsgovHgnd)
	ON_COMMAND(ID_DOWNLOAD_STATSGOV_FSYD, &CAutoDownloadView::OnDownloadStatsgovFsyd)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOAD_STATSGOV_FSYD, &CAutoDownloadView::OnUpdateDownloadStatsgovFsyd)
	ON_COMMAND(ID_DOWNLOAD_STATSGOV_FSJD, &CAutoDownloadView::OnDownloadStatsgovFsjd)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOAD_STATSGOV_FSJD, &CAutoDownloadView::OnUpdateDownloadStatsgovFsjd)
	ON_COMMAND(ID_DOWNLOAD_STATSGOV_FSND, &CAutoDownloadView::OnDownloadStatsgovFsnd)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOAD_STATSGOV_FSND, &CAutoDownloadView::OnUpdateDownloadStatsgovFsnd)
	ON_COMMAND(ID_DOWNLOAD_CNINFO_ANNOUNCEMENT, &CAutoDownloadView::OnDownloadCninfoAnnouncement)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOAD_CNINFO_ANNOUNCEMENT, &CAutoDownloadView::OnUpdateDownloadCninfoAnnouncement)
END_MESSAGE_MAP()


// CAutoDownloadView 诊断

#ifdef _DEBUG
void CAutoDownloadView::AssertValid() const
{
	CEditView::AssertValid();
}

#ifndef _WIN32_WCE
void CAutoDownloadView::Dump(CDumpContext& dc) const
{
	CEditView::Dump(dc);
}
#endif
#endif //_DEBUG

LRESULT CAutoDownloadView::OnUserLogWParam_1(WPARAM wParam, LPARAM lParam)
{
	// we expect the CMessageString in the WParam paramater
	CMessageString* pString = (CMessageString*)wParam;
	
	// make sure passed string is valid (this is important to prevent unexpected
	// results if an invalid string is passed)
	if (CMessageString::IsStringValid(pString))
	{
		CString curString;
		GetEditCtrl().GetWindowText(curString);
		if(curString.GetLength()>2 * 1024)
		{
			curString = curString.Right(2 * 1024);
		}

		curString += *pString;
		GetEditCtrl().SetWindowText(curString);
		GetEditCtrl().LineScroll(GetEditCtrl().GetLineCount()-1, 0 );
		delete pString;
	}
	else
	{
		CString strMessage = "ERROR! Test message using wParam received, but CMessageString passed in wParam is invalid.";
		AfxMessageBox(strMessage);

		ASSERT(FALSE);
		return MESSAGESTRING_NOT_RECEIVED;
	}

	return 0;
}


void CAutoDownloadView::StartupDownloadThread(int downloadType)
{
	if(m_bDownloading==NULL)
	{
		CDownloadRequest * req = new CDownloadRequest;
		req->m_nDownloadType = downloadType;
		req->m_hwnd = m_hWnd;
		req->m_nLogType = WM_USER_LOG_1;
		CWinThread * pThread = ((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req);
		m_bDownloading =pThread->m_hThread;
	}
}

void CAutoDownloadView::OnUpdateCmdUIDownload(CCmdUI *pCmdUI)
{
	if(m_bDownloading==NULL)
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

// CAutoDownloadView 消息处理程序

void CAutoDownloadView::OnAutoDownloadList()
{
	// TODO: 在此添加命令处理程序代码

	// CWinThread * pThread = AfxBeginThread(AutoDownloadThreadFunc, this);
	
}

void CAutoDownloadView::OnDownloadStockinfoShenzhenAshare()
{
	// TODO: 在此添加命令处理程序代码
	StartupDownloadThread(ID_DOWNLOAD_STOCKINFO_SHENZHEN_ASHARE);
}

void CAutoDownloadView::OnDownloadStockinfoSmallandmid()
{
	// TODO: 在此添加命令处理程序代码
	StartupDownloadThread(ID_DOWNLOAD_STOCKINFO_SMALLANDMID);

}

void CAutoDownloadView::OnDownloadStockinfoEnterpreneur()
{
	// TODO: 在此添加命令处理程序代码
	StartupDownloadThread(ID_DOWNLOAD_STOCKINFO_ENTERPRENEUR);

}

void CAutoDownloadView::OnDownloadTradingdailyEnterprenuer()
{
	// TODO: 在此添加命令处理程序代码
	StartupDownloadThread(ID_DOWNLOAD_TRADINGDAILY_ENTERPRENUER);

}

void CAutoDownloadView::OnDownloadFinanceEnterprenuer()
{
	// TODO: 在此添加命令处理程序代码
	StartupDownloadThread(ID_DOWNLOAD_FINANCE_ENTERPRENUER);
}


void CAutoDownloadView::OnDownloadFinanceShenzhenAshare()
{
	// TODO: 在此添加命令处理程序代码
	StartupDownloadThread(ID_DOWNLOAD_FINANCE_SHENZHEN_ASHARE);
}

void CAutoDownloadView::OnDownloadFinanceSmallAndMid()
{
	// TODO: 在此添加命令处理程序代码
	StartupDownloadThread(ID_DOWNLOAD_FINANCE_SMALL_AND_MID);
}

void CAutoDownloadView::OnDownloadFinanceShanghai()
{
	// TODO: 在此添加命令处理程序代码
	StartupDownloadThread(ID_DOWNLOAD_FINANCE_SHANGHAI);

}


void CAutoDownloadView::OnDownloadStockinfoShanghai()
{
	// TODO: 在此添加命令处理程序代码
	StartupDownloadThread(ID_DOWNLOAD_STOCKINFO_SHANGHAI);

}

void CAutoDownloadView::OnDownloadTradingdailyShenzhenAshare()
{
	// TODO: 在此添加命令处理程序代码
	StartupDownloadThread(ID_DOWNLOAD_TRADINGDAILY_SHENZHEN_ASHARE);

}

void CAutoDownloadView::OnDownloadTradingdailySmallAndMid()
{
	// TODO: 在此添加命令处理程序代码
	StartupDownloadThread(ID_DOWNLOAD_TRADINGDAILY_SMALL_AND_MID);

}

void CAutoDownloadView::OnDownloadTradingdailyShanghaistock()
{
	// TODO: 在此添加命令处理程序代码

	StartupDownloadThread(ID_DOWNLOAD_TRADINGDAILY_SHANGHAISTOCK);
	
}


void CAutoDownloadView::OnDownloadCapitialstructShanghai()
{
	// TODO: 在此添加命令处理程序代码
	StartupDownloadThread(ID_DOWNLOAD_CAPITIALSTRUCT_SHANGHAI);
}

void CAutoDownloadView::OnDownloadCapitialstructShenzhen()
{
	// TODO: 在此添加命令处理程序代码
	/*CCapitalStructOf10jqka * pCapital = new CCapitalStructOf10jqka;
	pCapital->m_nCode = 300067;

	pCapital->SaveAsCsv();
	delete pCapital;*/
	StartupDownloadThread(ID_DOWNLOAD_CAPITIALSTRUCT_SHENZHEN);

}

//////////////////////////////////////////////////////////////////////////
//	菜单的状态更新

void CAutoDownloadView::OnUpdateDownloadTradingdailyShanghaistock(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	OnUpdateCmdUIDownload(pCmdUI);
}



void CAutoDownloadView::OnUpdateDownloadTradingdailyShenzhenAshare(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	OnUpdateCmdUIDownload(pCmdUI);

}

void CAutoDownloadView::OnUpdateDownloadTradingdailySmallAndMid(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	OnUpdateCmdUIDownload(pCmdUI);

}

void CAutoDownloadView::OnUpdateDownloadTradingdailyEnterprenuer(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	OnUpdateCmdUIDownload(pCmdUI);

}

void CAutoDownloadView::OnUpdateDownloadFinanceEnterprenuer(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	OnUpdateCmdUIDownload(pCmdUI);
}

void CAutoDownloadView::OnUpdateDownloadFinanceShanghai(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	OnUpdateCmdUIDownload(pCmdUI);
}

void CAutoDownloadView::OnUpdateDownloadFinanceSmallAndMid(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	OnUpdateCmdUIDownload(pCmdUI);
}

void CAutoDownloadView::OnUpdateDownloadFinanceShenzhenAshare(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	OnUpdateCmdUIDownload(pCmdUI);
}

void CAutoDownloadView::OnUpdateDownloadCapitialstructShanghai(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	OnUpdateCmdUIDownload(pCmdUI);
}

void CAutoDownloadView::OnUpdateDownloadStockinfoShenzhenAshare(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	OnUpdateCmdUIDownload(pCmdUI);
}

void CAutoDownloadView::OnUpdateDownloadStockinfoSmallandmid(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	OnUpdateCmdUIDownload(pCmdUI);
}

void CAutoDownloadView::OnUpdateDownloadStockinfoEnterpreneur(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	OnUpdateCmdUIDownload(pCmdUI);
}

void CAutoDownloadView::OnUpdateDownloadStockinfoShanghai(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	OnUpdateCmdUIDownload(pCmdUI);
}

void CAutoDownloadView::OnUpdateDownloadCapitialstructShenzhen(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	OnUpdateCmdUIDownload(pCmdUI);
}

void CAutoDownloadView::OnInitialUpdate()
{
	CEditView::OnInitialUpdate();

	
	// TODO: 在此添加专用代码和/或调用基类

	//CFont* ptf=GetEditCtrl().GetFont();	// 得到原来的字体
	//LOGFONT lf; 
	//ptf->GetLogFont(&lf); 
	//lf.lfHeight = 20; // 改变字体高度 
	//strcpy (lf.lfFaceName, "隶书");			// 改变字体名称 
	//m_editFont.CreateFontIndirect(&lf); 

	m_editFont.CreatePointFont(100, "楷体_GB2312");   

	GetEditCtrl().SetFont(&m_editFont);	// 设置新字体


	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();

	m_bAutoClose = true;
	if(m_bDownloading==NULL && pDoc->m_vecCommandCode.size()>0)
	{
		/* 按照菜单集合中的命令进行下载  */

		vector<int>::iterator it =  pDoc->m_vecCommandCode.begin();
		int nCommand = *it;
		pDoc->m_vecCommandCode.erase(it);

		StartupDownloadThread(nCommand);
	}
	else if(m_bDownloading==NULL && pDoc->m_nCommandCode>0)
	{
		/* 按照菜单栏上的命令进行下载  */

		StartupDownloadThread(pDoc->m_nCommandCode);
	}
	else if(m_bDownloading==NULL && atoi(pDoc->m_nCode.c_str())>=0)
	{
		/* 个股数据下载 */
		CDownloadRequest * req = new CDownloadRequest;
		 req->m_nDownloadType = ID_DOWNLOAD_INDIVIDUAL;
		 req->m_hwnd = m_hWnd;
		 req->m_vecCode.push_back(atoi(pDoc->m_nCode.c_str()));
		 req->m_nLogType = WM_USER_LOG_1;

		CWinThread * pThread = ((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req);
		
		m_bDownloading = pThread->m_hThread;
	}
	else
	{
		m_bAutoClose = false;
	}

	if( 1 /*m_bAutoClose*/)
	{
		//使父框架窗口的“Close”系统菜单命令失效
		CMenu* pMenu=GetParentFrame()->GetSystemMenu(false);
		// pMenu->EnableMenuItem(SC_CLOSE,MF_BYCOMMAND|MF_GRAYED);
		pMenu->RemoveMenu(SC_CLOSE, MF_BYCOMMAND);

	}
}



LRESULT CAutoDownloadView::OnUserXmlWParam(WPARAM wParam, LPARAM lParam)
{
	// we expect the CMessageString in the WParam paramater
	CMessageString* pString = (CMessageString*)wParam;

	// make sure passed string is valid (this is important to prevent unexpected
	// results if an invalid string is passed)
	if (CMessageString::IsStringValid(pString))
	{
		CBaseMessageElementsParser parser(this);
		parser.Parse(pString->GetBuffer(), pString->GetLength());
		delete pString;
	
		if(parser.msg_type=="DownloadFinished")
		{
			m_bDownloading = NULL;

			/*从菜单命令集合中取出下一条执行*/
			CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
			if(pDoc->m_vecCommandCode.size()>0)
			{
				vector<int>::iterator it =  pDoc->m_vecCommandCode.begin();
				int nCommand = *it;
				pDoc->m_vecCommandCode.erase(it);

				StartupDownloadThread(nCommand);
			}
			else if(m_bAutoClose)
			{
				/* 下载完成，关闭本视图 */
				this->GetParentFrame()->PostMessage(WM_CLOSE);
			}
		}
	}
	else
	{
		CString strMessage = "ERROR! Test message using wParam received, but CMessageString passed in wParam is invalid.";
		AfxMessageBox(strMessage);

		ASSERT(FALSE);
		return MESSAGESTRING_NOT_RECEIVED;
	}

	return 0;
}
void CAutoDownloadView::OnDownloadSouhuStockPlate()
{
	// TODO: 在此添加命令处理程序代码

	StartupDownloadThread(ID_DOWNLOAD_SOUHU_STOCK_PLATE);
}

void CAutoDownloadView::OnUpdateDownloadSouhuStockPlate(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码

	OnUpdateCmdUIDownload(pCmdUI);
}

void CAutoDownloadView::OnDownloadCapitalflowShenzhenAshare()
{
	// TODO: 在此添加命令处理程序代码
	StartupDownloadThread(ID_DOWNLOAD_CAPITALFLOW_SHENZHEN_ASHARE);
}

void CAutoDownloadView::OnDownloadCapitalflowSmallAndMid()
{
	// TODO: 在此添加命令处理程序代码
	StartupDownloadThread(ID_DOWNLOAD_CAPITALFLOW_SMALL_AND_MID);
}

void CAutoDownloadView::OnDownloadCapitalflowEnterprenuer()
{
	// TODO: 在此添加命令处理程序代码
	StartupDownloadThread(ID_DOWNLOAD_CAPITALFLOW_ENTERPRENUER);
}

void CAutoDownloadView::OnDownloadCapitalflowShanghaistock()
{
	// TODO: 在此添加命令处理程序代码
	StartupDownloadThread(ID_DOWNLOAD_CAPITALFLOW_SHANGHAISTOCK);
}

void CAutoDownloadView::OnUpdateDownloadCapitalflowShenzhenAshare(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	OnUpdateCmdUIDownload(pCmdUI);
}

void CAutoDownloadView::OnUpdateDownloadCapitalflowSmallAndMid(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	OnUpdateCmdUIDownload(pCmdUI);
}

void CAutoDownloadView::OnUpdateDownloadCapitalflowEnterprenuer(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	OnUpdateCmdUIDownload(pCmdUI);
}

void CAutoDownloadView::OnUpdateDownloadCapitalflowShanghaistock(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	OnUpdateCmdUIDownload(pCmdUI);
}

void CAutoDownloadView::OnDownloadPerformanceForecast()
{
	// TODO: 在此添加命令处理程序代码
	StartupDownloadThread(ID_DOWNLOAD_PERFORMANCE_FORECAST);
}

void CAutoDownloadView::OnUpdateDownloadPerformanceForecast(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	OnUpdateCmdUIDownload(pCmdUI);
}

void CAutoDownloadView::OnDownloadIncomeComposition()
{
	// TODO: 在此添加命令处理程序代码
	StartupDownloadThread(ID_DOWNLOAD_INCOME_COMPOSITION);
}

void CAutoDownloadView::OnUpdateDownloadIncomeComposition(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	OnUpdateCmdUIDownload(pCmdUI);
}

void CAutoDownloadView::OnDownloadFundHoldShenzhenAshare()
{
	// TODO: 在此添加命令处理程序代码
	StartupDownloadThread(ID_DOWNLOAD_FUND_HOLD_SHENZHEN_ASHARE);

}

void CAutoDownloadView::OnUpdateDownloadFundHoldShenzhenAshare(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	OnUpdateCmdUIDownload(pCmdUI);
}

void CAutoDownloadView::OnDownloadFundHoldEnterprenuer()
{
	// TODO: 在此添加命令处理程序代码
	StartupDownloadThread(ID_DOWNLOAD_FUND_HOLD_ENTERPRENUER);

}

void CAutoDownloadView::OnUpdateDownloadFundHoldEnterprenuer(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	OnUpdateCmdUIDownload(pCmdUI);
}

void CAutoDownloadView::OnDownloadFundHoldSmallAndMid()
{
	// TODO: 在此添加命令处理程序代码
	StartupDownloadThread(ID_DOWNLOAD_FUND_HOLD_SMALL_AND_MID);

}

void CAutoDownloadView::OnUpdateDownloadFundHoldSmallAndMid(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	OnUpdateCmdUIDownload(pCmdUI);
}

void CAutoDownloadView::OnDownloadFundHoldShanghai()
{
	// TODO: 在此添加命令处理程序代码
	StartupDownloadThread(ID_DOWNLOAD_FUND_HOLD_SHANGHAI);

}

void CAutoDownloadView::OnUpdateDownloadFundHoldShanghai(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	OnUpdateCmdUIDownload(pCmdUI);
}

void CAutoDownloadView::OnDownloadHexunResearchPaper()
{
	// TODO: 在此添加命令处理程序代码
	StartupDownloadThread(ID_DOWNLOAD_HEXUN_RESEARCH_PAPER);
}

void CAutoDownloadView::OnUpdateDownloadHexunResearchPaper(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	OnUpdateCmdUIDownload(pCmdUI);
}

void CAutoDownloadView::OnDownloadHusheng300Index()
{
	// TODO: 在此添加命令处理程序代码
	StartupDownloadThread(ID_DOWNLOAD_HUSHENG300_INDEX);
}


void CAutoDownloadView::OnUpdateDownloadHusheng300Index(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	OnUpdateCmdUIDownload(pCmdUI);
}

void CAutoDownloadView::OnDownloadCffexFutureDaily()
{
	// TODO: 在此添加命令处理程序代码
	StartupDownloadThread(ID_DOWNLOAD_CFFEX_FUTURE_DAILY);
}

void CAutoDownloadView::OnUpdateDownloadCffexFutureDaily(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	OnUpdateCmdUIDownload(pCmdUI);
}

void CAutoDownloadView::OnDownloadStatsgovHgyd()
{
	// TODO: 在此添加命令处理程序代码
	StartupDownloadThread(ID_DOWNLOAD_STATSGOV_HGYD);
}

void CAutoDownloadView::OnUpdateDownloadStatsgovHgyd(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	OnUpdateCmdUIDownload(pCmdUI);
}

void CAutoDownloadView::OnDownloadStatsgovHgjd()
{
	// TODO: 在此添加命令处理程序代码
	StartupDownloadThread(ID_DOWNLOAD_STATSGOV_HGJD);
}

void CAutoDownloadView::OnUpdateDownloadStatsgovHgjd(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	OnUpdateCmdUIDownload(pCmdUI);
}

void CAutoDownloadView::OnDownloadStatsgovHgnd()
{
	// TODO: 在此添加命令处理程序代码
	StartupDownloadThread(ID_DOWNLOAD_STATSGOV_HGND);
}

void CAutoDownloadView::OnUpdateDownloadStatsgovHgnd(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	OnUpdateCmdUIDownload(pCmdUI);
}

void CAutoDownloadView::OnDownloadStatsgovFsyd()
{
	// TODO: 在此添加命令处理程序代码
	StartupDownloadThread(ID_DOWNLOAD_STATSGOV_FSYD);
}

void CAutoDownloadView::OnUpdateDownloadStatsgovFsyd(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	OnUpdateCmdUIDownload(pCmdUI);
}

void CAutoDownloadView::OnDownloadStatsgovFsjd()
{
	// TODO: 在此添加命令处理程序代码
	StartupDownloadThread(ID_DOWNLOAD_STATSGOV_FSJD);
}

void CAutoDownloadView::OnUpdateDownloadStatsgovFsjd(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	OnUpdateCmdUIDownload(pCmdUI);
}

void CAutoDownloadView::OnDownloadStatsgovFsnd()
{
	// TODO: 在此添加命令处理程序代码
	StartupDownloadThread(ID_DOWNLOAD_STATSGOV_FSND);
}

void CAutoDownloadView::OnUpdateDownloadStatsgovFsnd(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	OnUpdateCmdUIDownload(pCmdUI);
}

void CAutoDownloadView::OnDownloadCninfoAnnouncement()
{
	// TODO: 在此添加命令处理程序代码
	StartupDownloadThread(ID_DOWNLOAD_CNINFO_ANNOUNCEMENT);
}

void CAutoDownloadView::OnUpdateDownloadCninfoAnnouncement(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	OnUpdateCmdUIDownload(pCmdUI);
}
