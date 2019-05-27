
#include "stdafx.h"
#include "Stock.h"

//	技术分析


#include "AbsoluteBreadthIndex.h"
#include "AccumulationDistributionLine.h"
#include "RelativeStrengthIndex.h"
#include "CandlesticksJapan.h"
#include "BollingerBands.h"
#include "MAConvergenceDivergence.h"
#include "DirectionalMovement.h"
#include "StochasticsOscillatorKDJ.h"
#include "Volume.h"
#include "MovingAverages.h"
#include "OnBalanceVolume.h"
#include "HushenStockIndex.h"
#include "TechAnalysisEvent.h"

#include "SimulatedAccount.h"
#include "SimulatedAccountAnalysis.h"


void TechnicalAnalysis(int nDownloadType)
{
	// return ;
	if(nDownloadType == ID_DOWNLOAD_TRADINGDAILY_SHANGHAISTOCK )
	{
		//new CAbsoluteBreadthIndex(true);
		new CAccumulationDistributionLine(true);

		new CBollingerBands(true);
		new CMovingAverages(true);
		new CRelativeStrengthIndex(true);
		new CCandlesticksJapan(true);
		new CMAConvergenceDivergence(true);
		new CDirectionalMovement(true);
		new CStochasticsOscillatorKDJ(true);
		new CVolume(true);
		new COnBalanceVolume(true);
		new CHushenStockIndex(true);

		//	计算完成后，会删除以上对象
		CTechnicalAnalysis::getInstance().CommandHandler();

		CSimulatedAccountAnalysis::getInstance().CommandHandler();
	}
 }

void AnalyseShanghaiStock(int nDownloadType)
{
	//	该函数不再调用，指数计算 不再有存储过程进行，改由 本程序负责， CHushenStockIndex

	return;

	static bool bSohuPlate = false;
	static bool bSohuTradingDaily = false;
	static int  nCapitalStruct = 5;

	bool bAnalyse = true;
	switch(nDownloadType)
	{
	case ID_DOWNLOAD_SOUHU_STOCK_PLATE:
		bSohuPlate = true; 
		break;
	case ID_DOWNLOAD_TRADINGDAILY_SHANGHAISTOCK:
		bSohuTradingDaily = true; 
		break;
	case ID_DOWNLOAD_CAPITIALSTRUCT_SELECTED:
		nCapitalStruct--; 
		break;
	default:
		bAnalyse = false;
		break;
	}
		
	if(bAnalyse && bSohuTradingDaily && bSohuPlate && nCapitalStruct==0)
	{
		try{
			sendToOutput(_T("股市指数分析 Start !"), NULL, WM_USER_LOG_1);
			session sql(g_MysqlPool);
			
			//	下载完成后，计算 上证指数 的分析过程
			sql << "call HuShen_StockIndexTotally()";

			sendToOutput(_T("上证指数 分析 Start !"), NULL, WM_USER_LOG_1);
			sql << "call HuShen_PlateIndexByCodeTotally(1, '上证指数', 'HuShen', 2169.39, '2012-1-4');";

			sendToOutput(_T("中小板综合指数 分析 Start !"), NULL, WM_USER_LOG_1);
			sql << "call HuShen_PlateIndexByCodeTotally(399101, '中小板综指', 'HuShen', 4788.00, '2012-1-4');";

			sendToOutput(_T("创业板综合指数 分析 Start !"), NULL, WM_USER_LOG_1);
			sql << "call HuShen_PlateIndexByCodeTotally(399102, '创业板综指', 'HuShen', 702.49, '2012-1-4');";

			sendToOutput(_T("Sohu 行业指数分析 Start !"), NULL, WM_USER_LOG_1);
			sql << "call HuShen_SohuPlateIndex(1631, 1000, '2012-1-4');";

			sendToOutput(_T("Sohu 地域指数分析 Start !"), NULL, WM_USER_LOG_1);
			sql << "call HuShen_SohuPlateIndex(1632, 1000, '2012-1-4');";

			stringstream ss;
			ss.str();
			ss << "delete from hushen_plate_index where code in  "
				<< "(select code from souhustockplates where Parent_Code = 1630 and Is_Stock =0)";
			sql << ss.str();

			COleDateTime dt = COleDateTime::GetCurrentTime();
			dt.m_dt -= 365;
			string startdate = dt.Format("%Y-%m-%d").GetBuffer();

			sendToOutput(_T("Sohu 概念指数分析 Start !"), NULL, WM_USER_LOG_1);
			sql << "call HuShen_SohuPlateIndex(1630, 1000, \'" << startdate.c_str() <<"\');";

			sendToOutput(_T("股市指数分析 Success !"), NULL, WM_USER_LOG_1);
		}
		catch(...)
		{
			int kkk = 1;
			sendToOutput(_T("股市指数分析 Exception !"), NULL, WM_USER_LOG_1);
		}
	}

}

UINT AutoDownloadThreadFunc(LPVOID param)
{
	CDownloadRequest * pStock  = (CDownloadRequest *)param; 
	pStock->m_strResult = "NoMoreData";

	//	生成要下载的报表对象
	vector<CStockDataModel *> vecStockModel;

	//AcquireReadLock(&g_RWlock);
	{
		//	基本股票基础资料
		if(pStock->m_nDownloadType==ID_DOWNLOAD_STOCKINFO_ENTERPRENEUR)
		{
			CEntrepreneurship * tradingDaily = new CEntrepreneurship;
			tradingDaily->m_hWnd = pStock->m_hwnd;
			tradingDaily->m_nLogType = pStock->m_nLogType;
			vecStockModel.push_back(tradingDaily);
		}
		else if(pStock->m_nDownloadType==ID_DOWNLOAD_STOCKINFO_SMALLANDMID)
		{
			CSmallMidEnterprise * tradingDaily = new CSmallMidEnterprise;
			tradingDaily->m_hWnd = pStock->m_hwnd;
			tradingDaily->m_nLogType = pStock->m_nLogType;
			vecStockModel.push_back(tradingDaily);
		}
		else if(pStock->m_nDownloadType==ID_DOWNLOAD_STOCKINFO_SHENZHEN_ASHARE)
		{
			CShenzhenAshares * tradingDaily = new CShenzhenAshares;
			tradingDaily->m_hWnd = pStock->m_hwnd;
			vecStockModel.push_back(tradingDaily);
		}
		else if(pStock->m_nDownloadType==ID_DOWNLOAD_STOCKINFO_SHANGHAI)
		{
			CShanghaiStockInfo * tradingDaily = new CShanghaiStockInfo;
			tradingDaily->m_hWnd = pStock->m_hwnd;
			tradingDaily->m_nLogType = pStock->m_nLogType;
			vecStockModel.push_back(tradingDaily);
		}
		else if(pStock->m_nDownloadType==ID_DOWNLOAD_CNINFO_STOCKINFO)
		{
			{
				/* 巨潮网股票基础资料 */
				CCninfoStockInfo * tradingDaily = new CCninfoStockInfo;
				tradingDaily->m_hWnd = pStock->m_hwnd;
				tradingDaily->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(tradingDaily);
			}

			{
				/* 巨潮网终止上市公告 */
				CCninfoDelistingList * tradingDaily = new CCninfoDelistingList;
				tradingDaily->m_hWnd = pStock->m_hwnd;
				tradingDaily->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(tradingDaily);
			}

			{
				/* 巨潮网暂停上市公告 */
				CCninfoSuspendList * tradingDaily = new CCninfoSuspendList;
				tradingDaily->m_hWnd = pStock->m_hwnd;
				tradingDaily->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(tradingDaily);
			}
			
		}
		//  下载巨潮网的上市公司公告
		else if(pStock->m_nDownloadType == ID_DOWNLOAD_CNINFO_ANNOUNCEMENT)
		{
			CStockApp * pApp = (CStockApp *)AfxGetApp();
			CEntrepreneurship Entrepreneurship/* = pApp->m_CEntrepreneurship*/;
			CBussinessUtils::GetEntrepreneurship(Entrepreneurship);
			for(int i=0; i< Entrepreneurship.m_nRowNum; i++)
			{
				CCninfoAnnouncement * cashFlows = new CCninfoAnnouncement;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode = Entrepreneurship.m_vec_code[i];
				cashFlows->m_orgId = Entrepreneurship.m_vec_cninfo_orgid[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);
			}

			CSmallMidEnterprise midsmall /*= pApp->m_CSmallMidEnterprise*/;
			CBussinessUtils::GetSmallMidEnterprise(midsmall);
			for(int i=0; i< midsmall.m_nRowNum; i++)
			{
				CCninfoAnnouncement * cashFlows = new CCninfoAnnouncement;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode = midsmall.m_vec_code[i];
				cashFlows->m_orgId = midsmall.m_vec_cninfo_orgid[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);
			}

			CShenzhenAshares shenzhenA /*= pApp->m_CShenzhenAshares*/;
			CBussinessUtils::GetShenzhenAshares(shenzhenA);
			for(int i=0; i< shenzhenA.m_nRowNum; i++)
			{
				CCninfoAnnouncement * cashFlows = new CCninfoAnnouncement;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode = shenzhenA.m_vec_code[i];
				cashFlows->m_orgId = shenzhenA.m_vec_cninfo_orgid[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);
			}

			CShanghaiStockInfo shanghai /*= pApp->m_CShanghaiStock*/;
			CBussinessUtils::GetShanghaiStockInfo(shanghai);
			for(int i=0; i< shanghai.m_nRowNum; i++)
			{
				CCninfoAnnouncement * cashFlows = new CCninfoAnnouncement;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode = shanghai.m_vec_code[i];
				cashFlows->m_orgId = shanghai.m_vec_cninfo_orgid[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);
			}

			// 打乱下载顺序
			srand(time(0));
			random_shuffle(vecStockModel.begin(), vecStockModel.end());

		}
		else if(pStock->m_nDownloadType == ID_DOWNLOAD_CNINFO_ANNOUNCEMENT_SHANGHAI)
		{
			CShanghaiStockInfo shanghai /*= pApp->m_CShanghaiStock*/;
			CBussinessUtils::GetShanghaiStockInfo(shanghai);
			for(int i=0; i< shanghai.m_nRowNum; i++)
			{
				CCninfoAnnouncement * cashFlows = new CCninfoAnnouncement;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode = shanghai.m_vec_code[i];
				cashFlows->m_orgId = shanghai.m_vec_cninfo_orgid[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);
			}

			// 打乱下载顺序
			srand(time(0));
			random_shuffle(vecStockModel.begin(), vecStockModel.end());

		}
		else if(pStock->m_nDownloadType == ID_DOWNLOAD_CNINFO_ANNOUNCEMENT_SHENZHEN_ASHARE)
		{
			CShenzhenAshares shenzhenA /*= pApp->m_CShenzhenAshares*/;
			CBussinessUtils::GetShenzhenAshares(shenzhenA);
			for(int i=0; i< shenzhenA.m_nRowNum; i++)
			{
				CCninfoAnnouncement * cashFlows = new CCninfoAnnouncement;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode = shenzhenA.m_vec_code[i];
				cashFlows->m_orgId = shenzhenA.m_vec_cninfo_orgid[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);
			}

			// 打乱下载顺序
			srand(time(0));
			random_shuffle(vecStockModel.begin(), vecStockModel.end());
		}
		else if(pStock->m_nDownloadType == ID_DOWNLOAD_CNINFO_ANNOUNCEMENT_SMALLANDMID)
		{
			CSmallMidEnterprise midsmall /*= pApp->m_CSmallMidEnterprise*/;
			CBussinessUtils::GetSmallMidEnterprise(midsmall);
			for(int i=0; i< midsmall.m_nRowNum; i++)
			{
				CCninfoAnnouncement * cashFlows = new CCninfoAnnouncement;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode = midsmall.m_vec_code[i];
				cashFlows->m_orgId = midsmall.m_vec_cninfo_orgid[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);
			}

			// 打乱下载顺序
			srand(time(0));
			random_shuffle(vecStockModel.begin(), vecStockModel.end());
		}
		else if(pStock->m_nDownloadType == ID_DOWNLOAD_CNINFO_ANNOUNCEMENT_ENTERPRENEUR)
		{
			CEntrepreneurship Entrepreneurship/* = pApp->m_CEntrepreneurship*/;
			CBussinessUtils::GetEntrepreneurship(Entrepreneurship);
			for(int i=0; i< Entrepreneurship.m_nRowNum; i++)
			{
				CCninfoAnnouncement * cashFlows = new CCninfoAnnouncement;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode = Entrepreneurship.m_vec_code[i];
				cashFlows->m_orgId = Entrepreneurship.m_vec_cninfo_orgid[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);
			}

			// 打乱下载顺序
			srand(time(0));
			random_shuffle(vecStockModel.begin(), vecStockModel.end());
		}
		//	历史交易报价
		else if(pStock->m_nDownloadType==ID_DOWNLOAD_TRADINGDAILY_ENTERPRENUER )
		{
			CStockApp * pApp = (CStockApp *)AfxGetApp();
			CEntrepreneurship Entrepreneurship /*= pApp->m_CEntrepreneurship*/;
			CBussinessUtils::GetEntrepreneurship(Entrepreneurship);


			for(int i=0; i< Entrepreneurship.m_nRowNum; i++)
			{
				CNetEaseTradeDaily * tradingDaily = new CNetEaseTradeDaily;
				tradingDaily->m_hWnd = pStock->m_hwnd;
				tradingDaily->m_nCode = Entrepreneurship.m_vec_code[i];
				tradingDaily->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(tradingDaily);
			}

			/*for(int i=0; i< Entrepreneurship.m_nRowNum; i++)
			{
				CSouhuTradingDaily * tradingDaily = new CSouhuTradingDaily;
				tradingDaily->m_hWnd = pStock->m_hwnd;
				tradingDaily->m_nCode = Entrepreneurship.m_vec_code[i];
				tradingDaily->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(tradingDaily);
			}*/

		}
		else if(pStock->m_nDownloadType==ID_DOWNLOAD_TRADINGDAILY_SHENZHEN_ASHARE )
		{
			CStockApp * pApp = (CStockApp *)AfxGetApp();
			CShenzhenAshares shenzhenAshares/* = pApp->m_CShenzhenAshares*/;
			CBussinessUtils::GetShenzhenAshares(shenzhenAshares);


			for(int i=0; i< shenzhenAshares.m_nRowNum; i++)
			{
				CNetEaseTradeDaily * tradingDaily = new CNetEaseTradeDaily;
				tradingDaily->m_hWnd = pStock->m_hwnd;
				tradingDaily->m_nCode = shenzhenAshares.m_vec_code[i];
				tradingDaily->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(tradingDaily);
			}

			/*for(int i=0; i< shenzhenAshares.m_nRowNum; i++)
			{
				CSouhuTradingDaily * tradingDaily = new CSouhuTradingDaily;
				tradingDaily->m_hWnd = pStock->m_hwnd;
				tradingDaily->m_nCode = shenzhenAshares.m_vec_code[i];
				tradingDaily->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(tradingDaily);
			}*/

		}
		else if(pStock->m_nDownloadType==ID_DOWNLOAD_TRADINGDAILY_SMALL_AND_MID )
		{
			CStockApp * pApp = (CStockApp *)AfxGetApp();
			CSmallMidEnterprise smallMidEnterprise /*= pApp->m_CSmallMidEnterprise*/;
			CBussinessUtils::GetSmallMidEnterprise(smallMidEnterprise);


			for(int i=0; i< smallMidEnterprise.m_nRowNum; i++)
			{
				CNetEaseTradeDaily * tradingDaily = new CNetEaseTradeDaily;
				tradingDaily->m_hWnd = pStock->m_hwnd;
				tradingDaily->m_nCode = smallMidEnterprise.m_vec_code[i];
				tradingDaily->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(tradingDaily);
			}

			/*for(int i=0; i< smallMidEnterprise.m_nRowNum; i++)
			{
				CSouhuTradingDaily * tradingDaily = new CSouhuTradingDaily;
				tradingDaily->m_hWnd = pStock->m_hwnd;
				tradingDaily->m_nCode = smallMidEnterprise.m_vec_code[i];
				tradingDaily->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(tradingDaily);
			}*/

		}
		else if(pStock->m_nDownloadType==ID_DOWNLOAD_TRADINGDAILY_SHANGHAISTOCK )
		{
			CStockApp * pApp = (CStockApp *)AfxGetApp();
			CShanghaiStockInfo shanghaiStockInfo /*= pApp->m_CShanghaiStock*/;
			CBussinessUtils::GetShanghaiStockInfo(shanghaiStockInfo);

			for(int i=0; i< shanghaiStockInfo.m_nRowNum; i++)
			{
				CNetEaseTradeDaily * tradingDaily = new CNetEaseTradeDaily;
				tradingDaily->m_hWnd = pStock->m_hwnd;
				tradingDaily->m_nCode = shanghaiStockInfo.m_vec_code[i];
				tradingDaily->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(tradingDaily);
			}

			/*for(int i=0; i< shanghaiStockInfo.m_nRowNum; i++)
			{
				CSouhuTradingDaily * tradingDaily = new CSouhuTradingDaily;
				tradingDaily->m_hWnd = pStock->m_hwnd;
				tradingDaily->m_nCode = shanghaiStockInfo.m_vec_code[i];
				tradingDaily->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(tradingDaily);
			}*/

		}
		//	每日资金流向
		else if(pStock->m_nDownloadType==ID_DOWNLOAD_CAPITALFLOW_ENTERPRENUER )
		{
			CStockApp * pApp = (CStockApp *)AfxGetApp();
			CEntrepreneurship Entrepreneurship /*= pApp->m_CEntrepreneurship*/;
			CBussinessUtils::GetEntrepreneurship(Entrepreneurship);

			for(int i=0; i< Entrepreneurship.m_nRowNum; i++)
			{
				CCapitalFlows163 * tradingDaily = new CCapitalFlows163;
				tradingDaily->m_hWnd = pStock->m_hwnd;
				tradingDaily->m_nCode = Entrepreneurship.m_vec_code[i];
				tradingDaily->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(tradingDaily);
			}

		}
		else if(pStock->m_nDownloadType==ID_DOWNLOAD_CAPITALFLOW_SHENZHEN_ASHARE )
		{
			CStockApp * pApp = (CStockApp *)AfxGetApp();
			CShenzhenAshares shenzhenAshares /*= pApp->m_CShenzhenAshares*/;
			CBussinessUtils::GetShenzhenAshares(shenzhenAshares);

			for(int i=0; i< shenzhenAshares.m_nRowNum; i++)
			{
				CCapitalFlows163 * tradingDaily = new CCapitalFlows163;
				tradingDaily->m_hWnd = pStock->m_hwnd;
				tradingDaily->m_nCode = shenzhenAshares.m_vec_code[i];
				tradingDaily->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(tradingDaily);
			}

		}
		else if(pStock->m_nDownloadType==ID_DOWNLOAD_CAPITALFLOW_SMALL_AND_MID )
		{
			CStockApp * pApp = (CStockApp *)AfxGetApp();
			CSmallMidEnterprise smallMidEnterprise /*= pApp->m_CSmallMidEnterprise*/;
			CBussinessUtils::GetSmallMidEnterprise(smallMidEnterprise);

			for(int i=0; i< smallMidEnterprise.m_nRowNum; i++)
			{
				CCapitalFlows163 * tradingDaily = new CCapitalFlows163;
				tradingDaily->m_hWnd = pStock->m_hwnd;
				tradingDaily->m_nCode = smallMidEnterprise.m_vec_code[i];
				tradingDaily->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(tradingDaily);
			}

		}
		else if(pStock->m_nDownloadType==ID_DOWNLOAD_CAPITALFLOW_SHANGHAISTOCK )
		{
			CStockApp * pApp = (CStockApp *)AfxGetApp();
			CShanghaiStockInfo shanghaiStockInfo /*= pApp->m_CShanghaiStock*/;
			CBussinessUtils::GetShanghaiStockInfo(shanghaiStockInfo);

			for(int i=0; i< shanghaiStockInfo.m_nRowNum; i++)
			{
				CCapitalFlows163 * tradingDaily = new CCapitalFlows163;
				tradingDaily->m_hWnd = pStock->m_hwnd;
				tradingDaily->m_nCode = shanghaiStockInfo.m_vec_code[i];
				tradingDaily->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(tradingDaily);
			}

		}
		//	财务报表
		else if(pStock->m_nDownloadType==ID_DOWNLOAD_FINANCE_ENTERPRENUER)
		{
			CStockApp * pApp = (CStockApp *)AfxGetApp();
			CEntrepreneurship Entrepreneurship/* = pApp->m_CEntrepreneurship*/;
			CBussinessUtils::GetEntrepreneurship(Entrepreneurship);

			for(int i=0; i< Entrepreneurship.m_nRowNum; i++)
			{
				// 资产负债表
				CCninfoBalance * tradingDaily = new CCninfoBalance;
				tradingDaily->m_hWnd = pStock->m_hwnd;
				tradingDaily->m_nCode = Entrepreneurship.m_vec_code[i];
				tradingDaily->m_nLogType = pStock->m_nLogType;
				tradingDaily->m_cninfo_orgid = Entrepreneurship.m_vec_cninfo_orgid[i];
				vecStockModel.push_back(tradingDaily);

				// 利润表
				CCninfoIncomeStatement * incomeStatement = new CCninfoIncomeStatement;
				incomeStatement->m_hWnd = pStock->m_hwnd;
				incomeStatement->m_nCode = Entrepreneurship.m_vec_code[i];
				incomeStatement->m_nLogType = pStock->m_nLogType;
				incomeStatement->m_cninfo_orgid = Entrepreneurship.m_vec_cninfo_orgid[i];
				vecStockModel.push_back(incomeStatement);

				//	现金流量表
				CCninfoCashFlows * cashFlows = new CCninfoCashFlows;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode = Entrepreneurship.m_vec_code[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				cashFlows->m_cninfo_orgid = Entrepreneurship.m_vec_cninfo_orgid[i];
				vecStockModel.push_back(cashFlows);

			}
		}
		else if(pStock->m_nDownloadType==ID_DOWNLOAD_FINANCE_SHANGHAI)
		{
			CStockApp * pApp = (CStockApp *)AfxGetApp();
			CShanghaiStockInfo Entrepreneurship /*= pApp->m_CShanghaiStock*/;
			CBussinessUtils::GetShanghaiStockInfo(Entrepreneurship);

			for(int i=0; i< Entrepreneurship.m_nRowNum; i++)
			{
				// 资产负债表
				CCninfoBalance * tradingDaily = new CCninfoBalance;
				tradingDaily->m_hWnd = pStock->m_hwnd;
				tradingDaily->m_nCode = Entrepreneurship.m_vec_code[i];
				tradingDaily->m_nLogType = pStock->m_nLogType;
				tradingDaily->m_cninfo_orgid = Entrepreneurship.m_vec_cninfo_orgid[i];
				vecStockModel.push_back(tradingDaily);

				// 利润表
				CCninfoIncomeStatement * incomeStatement = new CCninfoIncomeStatement;
				incomeStatement->m_hWnd = pStock->m_hwnd;
				incomeStatement->m_nCode = Entrepreneurship.m_vec_code[i];
				incomeStatement->m_nLogType = pStock->m_nLogType;
				incomeStatement->m_cninfo_orgid = Entrepreneurship.m_vec_cninfo_orgid[i];
				vecStockModel.push_back(incomeStatement);

				//	现金流量表
				CCninfoCashFlows * cashFlows = new CCninfoCashFlows;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode = Entrepreneurship.m_vec_code[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				cashFlows->m_cninfo_orgid = Entrepreneurship.m_vec_cninfo_orgid[i];
				vecStockModel.push_back(cashFlows);

			}
		}
		else if(pStock->m_nDownloadType==ID_DOWNLOAD_FINANCE_SMALL_AND_MID)
		{
			CStockApp * pApp = (CStockApp *)AfxGetApp();
			CSmallMidEnterprise Entrepreneurship /*= pApp->m_CSmallMidEnterprise*/;
			CBussinessUtils::GetSmallMidEnterprise(Entrepreneurship);

			for(int i=0; i< Entrepreneurship.m_nRowNum; i++)
			{
				// 资产负债表
				CCninfoBalance * tradingDaily = new CCninfoBalance;
				tradingDaily->m_hWnd = pStock->m_hwnd;
				tradingDaily->m_nCode = Entrepreneurship.m_vec_code[i];
				tradingDaily->m_nLogType = pStock->m_nLogType;
				tradingDaily->m_cninfo_orgid = Entrepreneurship.m_vec_cninfo_orgid[i];
				vecStockModel.push_back(tradingDaily);

				// 利润表
				CCninfoIncomeStatement * incomeStatement = new CCninfoIncomeStatement;
				incomeStatement->m_hWnd = pStock->m_hwnd;
				incomeStatement->m_nCode = Entrepreneurship.m_vec_code[i];
				incomeStatement->m_nLogType = pStock->m_nLogType;
				incomeStatement->m_cninfo_orgid = Entrepreneurship.m_vec_cninfo_orgid[i];
				vecStockModel.push_back(incomeStatement);

				//	现金流量表
				CCninfoCashFlows * cashFlows = new CCninfoCashFlows;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode = Entrepreneurship.m_vec_code[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				cashFlows->m_cninfo_orgid = Entrepreneurship.m_vec_cninfo_orgid[i];
				vecStockModel.push_back(cashFlows);

			}
		}
		else if(pStock->m_nDownloadType==ID_DOWNLOAD_FINANCE_SHENZHEN_ASHARE)
		{
			CStockApp * pApp = (CStockApp *)AfxGetApp();
			CShenzhenAshares Entrepreneurship /*= pApp->m_CShenzhenAshares*/;
			CBussinessUtils::GetShenzhenAshares(Entrepreneurship);

			for(int i=0; i< Entrepreneurship.m_nRowNum; i++)
			{
				// 资产负债表
				CCninfoBalance * tradingDaily = new CCninfoBalance;
				tradingDaily->m_hWnd = pStock->m_hwnd;
				tradingDaily->m_nCode = Entrepreneurship.m_vec_code[i];
				tradingDaily->m_nLogType = pStock->m_nLogType;
				tradingDaily->m_cninfo_orgid = Entrepreneurship.m_vec_cninfo_orgid[i];
				vecStockModel.push_back(tradingDaily);

				// 利润表
				CCninfoIncomeStatement * incomeStatement = new CCninfoIncomeStatement;
				incomeStatement->m_hWnd = pStock->m_hwnd;
				incomeStatement->m_nCode = Entrepreneurship.m_vec_code[i];
				incomeStatement->m_nLogType = pStock->m_nLogType;
				incomeStatement->m_cninfo_orgid = Entrepreneurship.m_vec_cninfo_orgid[i];
				vecStockModel.push_back(incomeStatement);

				//	现金流量表
				CCninfoCashFlows * cashFlows = new CCninfoCashFlows;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode = Entrepreneurship.m_vec_code[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				cashFlows->m_cninfo_orgid = Entrepreneurship.m_vec_cninfo_orgid[i];
				vecStockModel.push_back(cashFlows);

			}
		}
		else if(pStock->m_nDownloadType==ID_DOWNLOAD_FINANCE_SELECTED)
		{
			CStockApp * pApp = (CStockApp *)AfxGetApp();

			CShenzhenAshares shenzhenAshares;
			CBussinessUtils::GetShenzhenAshares(shenzhenAshares);

			CSmallMidEnterprise smallmid;
			CBussinessUtils::GetSmallMidEnterprise(smallmid);

			CShanghaiStockInfo shanghai;
			CBussinessUtils::GetShanghaiStockInfo(shanghai);

			CEntrepreneurship entrepreneur;
			CBussinessUtils::GetEntrepreneurship(entrepreneur);
			
			for(int i=0; i< pStock->m_vecCode.size(); i++)
			{
				string cninfo_orgid = "";
				/*if(cninfo_orgid.empty())
				{
					int idx = shenzhenAshares.SearchStockByCode(pStock->m_vecCode[i]);
					if(idx>=0)
						cninfo_orgid = shenzhenAshares.m_vec_cninfo_orgid[idx];
				}
				
				if(cninfo_orgid.empty())
				{
					int idx = smallmid.SearchStockByCode(pStock->m_vecCode[i]);
					if(idx>=0)
						cninfo_orgid = smallmid.m_vec_cninfo_orgid[idx];
				}

				if(cninfo_orgid.empty())
				{
					int idx = shanghai.SearchStockByCode(pStock->m_vecCode[i]);
					if(idx>=0)
						cninfo_orgid = shanghai.m_vec_cninfo_orgid[idx];
				}

				if(cninfo_orgid.empty())
				{
					int idx = entrepreneur.SearchStockByCode(pStock->m_vecCode[i]);
					if(idx>=0)
						cninfo_orgid = entrepreneur.m_vec_cninfo_orgid[idx];
				}*/
				
				cninfo_orgid = pStock->m_vec_cninfo_orgid[i];

				if(cninfo_orgid.empty()==false)
				{
					// 资产负债表
					CCninfoBalance * tradingDaily = new CCninfoBalance;
					tradingDaily->m_hWnd = pStock->m_hwnd;
					tradingDaily->m_nCode = pStock->m_vecCode[i];
					tradingDaily->m_nLogType = pStock->m_nLogType;
					tradingDaily->m_cninfo_orgid = cninfo_orgid;
					vecStockModel.push_back(tradingDaily);

					// 利润表
					CCninfoIncomeStatement * incomeStatement = new CCninfoIncomeStatement;
					incomeStatement->m_hWnd = pStock->m_hwnd;
					incomeStatement->m_nCode = pStock->m_vecCode[i];
					incomeStatement->m_nLogType = pStock->m_nLogType;
					incomeStatement->m_cninfo_orgid = cninfo_orgid;
					vecStockModel.push_back(incomeStatement);

					//	现金流量表
					CCninfoCashFlows * cashFlows = new CCninfoCashFlows;
					cashFlows->m_hWnd = pStock->m_hwnd;
					cashFlows->m_nCode = pStock->m_vecCode[i];
					cashFlows->m_nLogType = pStock->m_nLogType;
					cashFlows->m_cninfo_orgid = cninfo_orgid;
					vecStockModel.push_back(cashFlows);
				}
			}
			int tttt = 333;
		}
		//	基金持有状态
		else if(pStock->m_nDownloadType==ID_DOWNLOAD_FUND_HOLD_ENTERPRENUER)
		{
			CStockApp * pApp = (CStockApp *)AfxGetApp();
			CEntrepreneurship Entrepreneurship/* = pApp->m_CEntrepreneurship*/;
			CBussinessUtils::GetEntrepreneurship(Entrepreneurship);

			for(int i=0; i< Entrepreneurship.m_nRowNum; i++)
			{
				CSohuFundHold * tradingDaily = new CSohuFundHold;
				tradingDaily->m_hWnd = pStock->m_hwnd;
				tradingDaily->m_nCode = Entrepreneurship.m_vec_code[i];
				tradingDaily->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(tradingDaily);
			}
		}
		else if(pStock->m_nDownloadType==ID_DOWNLOAD_FUND_HOLD_SHANGHAI)
		{
			CStockApp * pApp = (CStockApp *)AfxGetApp();
			CShanghaiStockInfo Entrepreneurship /*= pApp->m_CShanghaiStock*/;
			CBussinessUtils::GetShanghaiStockInfo(Entrepreneurship);

			for(int i=0; i< Entrepreneurship.m_nRowNum; i++)
			{
				CSohuFundHold * tradingDaily = new CSohuFundHold;
				tradingDaily->m_hWnd = pStock->m_hwnd;
				tradingDaily->m_nCode = Entrepreneurship.m_vec_code[i];
				tradingDaily->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(tradingDaily);
			}
		}
		else if(pStock->m_nDownloadType==ID_DOWNLOAD_FUND_HOLD_SMALL_AND_MID)
		{
			CStockApp * pApp = (CStockApp *)AfxGetApp();
			CSmallMidEnterprise Entrepreneurship /*= pApp->m_CSmallMidEnterprise*/;
			CBussinessUtils::GetSmallMidEnterprise(Entrepreneurship);

			for(int i=0; i< Entrepreneurship.m_nRowNum; i++)
			{
				CSohuFundHold * tradingDaily = new CSohuFundHold;
				tradingDaily->m_hWnd = pStock->m_hwnd;
				tradingDaily->m_nCode = Entrepreneurship.m_vec_code[i];
				tradingDaily->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(tradingDaily);
			}
		}
		else if(pStock->m_nDownloadType==ID_DOWNLOAD_FUND_HOLD_SHENZHEN_ASHARE)
		{
			CStockApp * pApp = (CStockApp *)AfxGetApp();
			CShenzhenAshares Entrepreneurship /*= pApp->m_CShenzhenAshares*/;
			CBussinessUtils::GetShenzhenAshares(Entrepreneurship);

			for(int i=0; i< Entrepreneurship.m_nRowNum; i++)
			{
				CSohuFundHold * tradingDaily = new CSohuFundHold;
				tradingDaily->m_hWnd = pStock->m_hwnd;
				tradingDaily->m_nCode = Entrepreneurship.m_vec_code[i];
				tradingDaily->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(tradingDaily);
			}
		}


		//	股本结构
		else if(pStock->m_nDownloadType==ID_DOWNLOAD_CAPITIALSTRUCT_SHANGHAI)
		{
			CStockApp * pApp = (CStockApp *)AfxGetApp();
			CShanghaiStockInfo Entrepreneurship /*= pApp->m_CShanghaiStock*/;
			CBussinessUtils::GetShanghaiStockInfo(Entrepreneurship);

			for(int i=0; i< Entrepreneurship.m_nRowNum; i++)
			{
				CCapitalStructOf10jqka * cashFlows = new CCapitalStructOf10jqka;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode = Entrepreneurship.m_vec_code[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);
			}
		}
		else if(pStock->m_nDownloadType==ID_DOWNLOAD_CAPITIALSTRUCT_SHENZHEN)
		{
			CStockApp * pApp = (CStockApp *)AfxGetApp();
			CEntrepreneurship Entrepreneurship /*= pApp->m_CEntrepreneurship*/;
			CBussinessUtils::GetEntrepreneurship(Entrepreneurship);

			for(int i=0; i< Entrepreneurship.m_nRowNum; i++)
			{
				CCapitalStructOf10jqka * cashFlows = new CCapitalStructOf10jqka;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode = Entrepreneurship.m_vec_code[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);
			}

			CSmallMidEnterprise midsmall /*= pApp->m_CSmallMidEnterprise*/;
			CBussinessUtils::GetSmallMidEnterprise(midsmall);
			for(int i=0; i< midsmall.m_nRowNum; i++)
			{
				CCapitalStructOf10jqka * cashFlows = new CCapitalStructOf10jqka;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode = midsmall.m_vec_code[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);
			}

			CShenzhenAshares shenzhenA /*= pApp->m_CShenzhenAshares*/;
			CBussinessUtils::GetShenzhenAshares(shenzhenA);
			for(int i=0; i< shenzhenA.m_nRowNum; i++)
			{
				CCapitalStructOf10jqka * cashFlows = new CCapitalStructOf10jqka;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode = shenzhenA.m_vec_code[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);
			}
		}
		else if(pStock->m_nDownloadType==ID_DOWNLOAD_CAPITIALSTRUCT_SELECTED)
		{
			CStockApp * pApp = (CStockApp *)AfxGetApp();
			
			for(int i=0; i< pStock->m_vecCode.size(); i++)
			{
				CCapitalStructOf10jqka * cashFlows = new CCapitalStructOf10jqka;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode = pStock->m_vecCode[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);
			}
		}

		// 个股数据下载
		else if(pStock->m_nDownloadType==ID_DOWNLOAD_INDIVIDUAL)
		{
			for(int m =0 ; m< pStock->m_vecCode.size(); m++)
			{
				//	Souhu 每日交易数据下载
				CSouhuTradingDaily * tradingDaily = new CSouhuTradingDaily;
				tradingDaily->m_hWnd = pStock->m_hwnd;
				tradingDaily->m_nCode = pStock->m_vecCode[m];
				tradingDaily->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(tradingDaily);

				//	NetEase 每日交易数据下载
				CNetEaseTradeDaily * netease = new CNetEaseTradeDaily;
				netease->m_hWnd = pStock->m_hwnd;
				netease->m_nCode = pStock->m_vecCode[m];
				netease->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(netease);

				// 资产负债表
				CCninfoBalance * balance = new CCninfoBalance;
				balance->m_hWnd = pStock->m_hwnd;
				balance->m_nCode = pStock->m_vecCode[m]; 
				balance->m_nLogType = pStock->m_nLogType;
				balance->m_cninfo_orgid = pStock->m_vec_cninfo_orgid[m];
				vecStockModel.push_back(balance);

				// 利润表
				CCninfoIncomeStatement * incomeStatement = new CCninfoIncomeStatement;
				incomeStatement->m_hWnd = pStock->m_hwnd;
				incomeStatement->m_nCode = pStock->m_vecCode[m];
				incomeStatement->m_nLogType = pStock->m_nLogType;
				incomeStatement->m_cninfo_orgid = pStock->m_vec_cninfo_orgid[m];
				vecStockModel.push_back(incomeStatement);

				//	现金流量表
				CCninfoCashFlows * cashFlows = new CCninfoCashFlows;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode = pStock->m_vecCode[m];
				cashFlows->m_nLogType = pStock->m_nLogType;
				cashFlows->m_cninfo_orgid = pStock->m_vec_cninfo_orgid[m];
				vecStockModel.push_back(cashFlows);

				//	同花顺 股本结构
				CCapitalStructOf10jqka * capital = new CCapitalStructOf10jqka;
				capital->m_hWnd = pStock->m_hwnd;
				capital->m_nCode = pStock->m_vecCode[m];
				capital->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(capital);

				//	163 网易 每日资金流向
				CCapitalFlows163 * flow = new CCapitalFlows163;
				flow->m_hWnd = pStock->m_hwnd;
				flow->m_nCode = pStock->m_vecCode[m];
				flow->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(flow);

				// 搜狐 业绩预告
				CSohuPerformanceForecast * forecast = new CSohuPerformanceForecast;
				forecast->m_hWnd = pStock->m_hwnd;
				forecast->m_nCode = pStock->m_vecCode[m];
				forecast->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(forecast);

				// 搜狐 主营收入构成
				CSohuMainIncomeComposition * incomeComposition = new CSohuMainIncomeComposition;
				incomeComposition->m_hWnd = pStock->m_hwnd;
				incomeComposition->m_nCode = pStock->m_vecCode[m];
				incomeComposition->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(incomeComposition);

				//	搜狐 基金持有
				CSohuFundHold * fundHold = new CSohuFundHold;
				fundHold->m_hWnd = pStock->m_hwnd;
				fundHold->m_nCode = pStock->m_vecCode[m];
				fundHold->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(fundHold);

				// 下载巨潮网的公告
				CCninfoAnnouncement * announcement = new CCninfoAnnouncement;
				announcement->m_hWnd = pStock->m_hwnd;
				announcement->m_nCode =  pStock->m_vecCode[m];
				CStockInfoData sd = CBussinessUtils::GetStockInfo( pStock->m_vecCode[m]);
				announcement->m_orgId =  sd.cninfo_orgid;
				announcement->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(announcement);
			}

		}
		//	深圳每日行情数据
		else if(pStock->m_nDownloadType==60000)
		{
			time_t curtime = time (NULL);
			while(1)
			{
				struct tm ret;
				//ret = gmtime (&curtime);
				localtime_s(&ret, &curtime);

				if(ret.tm_wday==0 || ret.tm_wday==6)
				{

				}
				else
				{
					char tmp[256] = { 0 };
					snprintf (tmp, 255, "%d-%02d-%02d", 1900 + ret.tm_year, ret.tm_mon+1, ret.tm_mday);	

					CShenzhenMarketQuotation * quotation = new CShenzhenMarketQuotation;
					quotation->m_hWnd = pStock->m_hwnd;
					quotation->m_nLogType = pStock->m_nLogType;
					quotation->m_str_report_date = string(tmp);
					quotation->ExportFromDatabase();
					if(quotation->m_vec_code.size()>0)
					{
						delete quotation;
						break;
					}
					vecStockModel.push_back(quotation);
				}
				curtime = curtime - 24 * 3600;
			}

		}
		//	下载搜狐的板块信息
		else if(pStock->m_nDownloadType == ID_DOWNLOAD_SOUHU_STOCK_PLATE)
		{
			CSouhuStockPlates * capital = new CSouhuStockPlates;
			capital->m_hWnd = pStock->m_hwnd;
			capital->m_nLogType = pStock->m_nLogType;
			vecStockModel.push_back(capital);
		}
		//  下载和讯的研究报告
		else if(pStock->m_nDownloadType == ID_DOWNLOAD_HEXUN_RESEARCH_PAPER)
		{
			CStockApp * pApp = (CStockApp *)AfxGetApp();
			CEntrepreneurship Entrepreneurship/* = pApp->m_CEntrepreneurship*/;
			CBussinessUtils::GetEntrepreneurship(Entrepreneurship);
			for(int i=0; i< Entrepreneurship.m_nRowNum; i++)
			{
				CHexunResearchPaper * cashFlows = new CHexunResearchPaper;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode = Entrepreneurship.m_vec_code[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);
			}

			CSmallMidEnterprise midsmall /*= pApp->m_CSmallMidEnterprise*/;
			CBussinessUtils::GetSmallMidEnterprise(midsmall);
			for(int i=0; i< midsmall.m_nRowNum; i++)
			{
				CHexunResearchPaper * cashFlows = new CHexunResearchPaper;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode = midsmall.m_vec_code[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);
			}

			CShenzhenAshares shenzhenA /*= pApp->m_CShenzhenAshares*/;
			CBussinessUtils::GetShenzhenAshares(shenzhenA);
			for(int i=0; i< shenzhenA.m_nRowNum; i++)
			{
				CHexunResearchPaper * cashFlows = new CHexunResearchPaper;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode = shenzhenA.m_vec_code[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);
			}

			CShanghaiStockInfo shanghai /*= pApp->m_CShanghaiStock*/;
			CBussinessUtils::GetShanghaiStockInfo(shanghai);
			for(int i=0; i< shanghai.m_nRowNum; i++)
			{
				CHexunResearchPaper * cashFlows = new CHexunResearchPaper;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode = shanghai.m_vec_code[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);
			}

			// 打乱下载顺序
			srand(time(0));
			random_shuffle(vecStockModel.begin(), vecStockModel.end());

		}
		//  下载搜狐的业绩预告
		else if(pStock->m_nDownloadType == ID_DOWNLOAD_PERFORMANCE_FORECAST)
		{
			CStockApp * pApp = (CStockApp *)AfxGetApp();
			CEntrepreneurship Entrepreneurship/* = pApp->m_CEntrepreneurship*/;
			CBussinessUtils::GetEntrepreneurship(Entrepreneurship);
			for(int i=0; i< Entrepreneurship.m_nRowNum; i++)
			{
				CSohuPerformanceForecast * cashFlows = new CSohuPerformanceForecast;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode = Entrepreneurship.m_vec_code[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);
			}

			CSmallMidEnterprise midsmall /*= pApp->m_CSmallMidEnterprise*/;
			CBussinessUtils::GetSmallMidEnterprise(midsmall);
			for(int i=0; i< midsmall.m_nRowNum; i++)
			{
				CSohuPerformanceForecast * cashFlows = new CSohuPerformanceForecast;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode = midsmall.m_vec_code[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);
			}

			CShenzhenAshares shenzhenA /*= pApp->m_CShenzhenAshares*/;
			CBussinessUtils::GetShenzhenAshares(shenzhenA);
			for(int i=0; i< shenzhenA.m_nRowNum; i++)
			{
				CSohuPerformanceForecast * cashFlows = new CSohuPerformanceForecast;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode = shenzhenA.m_vec_code[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);
			}

			CShanghaiStockInfo shanghai /*= pApp->m_CShanghaiStock*/;
			CBussinessUtils::GetShanghaiStockInfo(shanghai);
			for(int i=0; i< shanghai.m_nRowNum; i++)
			{
				CSohuPerformanceForecast * cashFlows = new CSohuPerformanceForecast;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode = shanghai.m_vec_code[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);
			}


		}
		else if(pStock->m_nDownloadType == ID_DOWNLOAD_PERFORMANCE_FORECAST_SELECTED)
		{
			CStockApp * pApp = (CStockApp *)AfxGetApp();
			for(int i=0; i< pStock->m_vecCode.size(); i++)
			{
				CSohuPerformanceForecast * cashFlows = new CSohuPerformanceForecast;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode =pStock->m_vecCode[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);
			}

			// 打乱下载顺序
			srand(time(0));
			random_shuffle(vecStockModel.begin(), vecStockModel.end());
		}
		//  下载搜狐的主营收入构成
		else if(pStock->m_nDownloadType == ID_DOWNLOAD_INCOME_COMPOSITION)
		{
			CStockApp * pApp = (CStockApp *)AfxGetApp();
			CEntrepreneurship Entrepreneurship /*= pApp->m_CEntrepreneurship*/;
			CBussinessUtils::GetEntrepreneurship(Entrepreneurship);
			for(int i=0; i< Entrepreneurship.m_nRowNum; i++)
			{
				CSohuMainIncomeComposition * cashFlows = new CSohuMainIncomeComposition;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode = Entrepreneurship.m_vec_code[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);
			}

			CSmallMidEnterprise midsmall /*= pApp->m_CSmallMidEnterprise*/;
			CBussinessUtils::GetSmallMidEnterprise(midsmall);
			for(int i=0; i< midsmall.m_nRowNum; i++)
			{
				CSohuMainIncomeComposition * cashFlows = new CSohuMainIncomeComposition;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode = midsmall.m_vec_code[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);
			}

			CShenzhenAshares shenzhenA /*= pApp->m_CShenzhenAshares*/;
			CBussinessUtils::GetShenzhenAshares(shenzhenA);
			for(int i=0; i< shenzhenA.m_nRowNum; i++)
			{
				CSohuMainIncomeComposition * cashFlows = new CSohuMainIncomeComposition;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode = shenzhenA.m_vec_code[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);
			}

			CShanghaiStockInfo shanghai /*= pApp->m_CShanghaiStock*/;
			CBussinessUtils::GetShanghaiStockInfo(shanghai);
			for(int i=0; i< shanghai.m_nRowNum; i++)
			{
				CSohuMainIncomeComposition * cashFlows = new CSohuMainIncomeComposition;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode = shanghai.m_vec_code[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);
			}

		}
		else if(pStock->m_nDownloadType == ID_DOWNLOAD_INCOME_COMPOSITION_SELECTED)
		{
			CStockApp * pApp = (CStockApp *)AfxGetApp();
			for(int i=0; i< pStock->m_vecCode.size(); i++)
			{
				CSohuMainIncomeComposition * cashFlows = new CSohuMainIncomeComposition;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode =pStock->m_vecCode[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);
			}

			// 打乱下载顺序
			srand(time(0));
			random_shuffle(vecStockModel.begin(), vecStockModel.end());
		}
		//  下载中财网分红扩股数据, 已经不再下载该数据
		else if(pStock->m_nDownloadType == ID_DOWNLOAD_CFI_BONUS_SHARE)
		{
			CStockApp * pApp = (CStockApp *)AfxGetApp();
			CEntrepreneurship Entrepreneurship/* = pApp->m_CEntrepreneurship*/;
			CBussinessUtils::GetEntrepreneurship(Entrepreneurship);
			for(int i=0; i< Entrepreneurship.m_nRowNum; i++)
			{
				CCfiAddIssuance * cashFlows = new CCfiAddIssuance;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode = Entrepreneurship.m_vec_code[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);

				CCfiPlacementOfShare * cashFlows_1 = new CCfiPlacementOfShare;
				cashFlows_1->m_hWnd = pStock->m_hwnd;
				cashFlows_1->m_nCode = Entrepreneurship.m_vec_code[i];
				cashFlows_1->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows_1);
			}

			CSmallMidEnterprise midsmall /*= pApp->m_CSmallMidEnterprise*/;
			CBussinessUtils::GetSmallMidEnterprise(midsmall);
			for(int i=0; i< midsmall.m_nRowNum; i++)
			{
				CCfiAddIssuance * cashFlows = new CCfiAddIssuance;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode = midsmall.m_vec_code[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);

				CCfiPlacementOfShare * cashFlows_1 = new CCfiPlacementOfShare;
				cashFlows_1->m_hWnd = pStock->m_hwnd;
				cashFlows_1->m_nCode = midsmall.m_vec_code[i];
				cashFlows_1->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows_1);
			}

			CShenzhenAshares shenzhenA /*= pApp->m_CShenzhenAshares*/;
			CBussinessUtils::GetShenzhenAshares(shenzhenA);
			for(int i=0; i< shenzhenA.m_nRowNum; i++)
			{
				CCfiAddIssuance * cashFlows = new CCfiAddIssuance;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode = shenzhenA.m_vec_code[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);

				CCfiPlacementOfShare * cashFlows_1 = new CCfiPlacementOfShare;
				cashFlows_1->m_hWnd = pStock->m_hwnd;
				cashFlows_1->m_nCode = shenzhenA.m_vec_code[i];
				cashFlows_1->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows_1);
			}

			CShanghaiStockInfo shanghai /*= pApp->m_CShanghaiStock*/;
			CBussinessUtils::GetShanghaiStockInfo(shanghai);
			for(int i=0; i< shanghai.m_nRowNum; i++)
			{
				CCfiAddIssuance * cashFlows = new CCfiAddIssuance;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode = shanghai.m_vec_code[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);

				CCfiPlacementOfShare * cashFlows_1 = new CCfiPlacementOfShare;
				cashFlows_1->m_hWnd = pStock->m_hwnd;
				cashFlows_1->m_nCode = shanghai.m_vec_code[i];
				cashFlows_1->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows_1);
			}

			// 打乱下载顺序
			srand(time(0));
			random_shuffle(vecStockModel.begin(), vecStockModel.end());
		}
		//  下载巨潮网分红扩股数据
		else if(pStock->m_nDownloadType == ID_DOWNLOAD_CNINFO_BONUS_SHARE)
		{
			// 优先下载 分红转增送股等数据， 后下载 配股 
			vector<CStockDataModel *>	vecTmp;

			//	去年的年度预案未公布的，才需要下载
			COleDateTime t = COleDateTime::GetCurrentTime();
			int nYear = t.GetYear()-1;
			string lastYear = Int2String(nYear, "%4d") + "-12-31";

			CStockApp * pApp = (CStockApp *)AfxGetApp();
			CEntrepreneurship Entrepreneurship/* = pApp->m_CEntrepreneurship*/;
			CBussinessUtils::GetEntrepreneurship(Entrepreneurship);
			for(int i=0; i< Entrepreneurship.m_nRowNum; i++)
			{
				string strLastTradedate = pApp->m_StockDataLog.SearchStockByCode(Entrepreneurship.m_vec_code[i]).cninfo_bonus;
				if(strLastTradedate < lastYear)
				{
					CCninfoBonus * cashFlows = new CCninfoBonus;
					cashFlows->m_hWnd = pStock->m_hwnd;
					cashFlows->m_nCode = Entrepreneurship.m_vec_code[i];
					cashFlows->m_nLogType = pStock->m_nLogType;
					vecStockModel.push_back(cashFlows);
				}
				

				CCninfoPlacementOfShare * cashFlows_1 = new CCninfoPlacementOfShare;
				cashFlows_1->m_hWnd = pStock->m_hwnd;
				cashFlows_1->m_nCode = Entrepreneurship.m_vec_code[i];
				cashFlows_1->m_nLogType = pStock->m_nLogType;
				vecTmp.push_back(cashFlows_1);
			}

			CSmallMidEnterprise midsmall /*= pApp->m_CSmallMidEnterprise*/;
			CBussinessUtils::GetSmallMidEnterprise(midsmall);
			for(int i=0; i< midsmall.m_nRowNum; i++)
			{
				string strLastTradedate = pApp->m_StockDataLog.SearchStockByCode(midsmall.m_vec_code[i]).cninfo_bonus;
				if(strLastTradedate < lastYear)
				{
					CCninfoBonus * cashFlows = new CCninfoBonus;
					cashFlows->m_hWnd = pStock->m_hwnd;
					cashFlows->m_nCode = midsmall.m_vec_code[i];
					cashFlows->m_nLogType = pStock->m_nLogType;
					vecStockModel.push_back(cashFlows);
				}
				

				CCninfoPlacementOfShare * cashFlows_1 = new CCninfoPlacementOfShare;
				cashFlows_1->m_hWnd = pStock->m_hwnd;
				cashFlows_1->m_nCode = midsmall.m_vec_code[i];
				cashFlows_1->m_nLogType = pStock->m_nLogType;
				vecTmp.push_back(cashFlows_1);
			}

			CShenzhenAshares shenzhenA /*= pApp->m_CShenzhenAshares*/;
			CBussinessUtils::GetShenzhenAshares(shenzhenA);
			for(int i=0; i< shenzhenA.m_nRowNum; i++)
			{
				string strLastTradedate = pApp->m_StockDataLog.SearchStockByCode(shenzhenA.m_vec_code[i]).cninfo_bonus;
				if(strLastTradedate < lastYear)
				{
					CCninfoBonus * cashFlows = new CCninfoBonus;
					cashFlows->m_hWnd = pStock->m_hwnd;
					cashFlows->m_nCode = shenzhenA.m_vec_code[i];
					cashFlows->m_nLogType = pStock->m_nLogType;
					vecStockModel.push_back(cashFlows);
				}
			

				CCninfoPlacementOfShare * cashFlows_1 = new CCninfoPlacementOfShare;
				cashFlows_1->m_hWnd = pStock->m_hwnd;
				cashFlows_1->m_nCode = shenzhenA.m_vec_code[i];
				cashFlows_1->m_nLogType = pStock->m_nLogType;
				vecTmp.push_back(cashFlows_1);
			}

			CShanghaiStockInfo shanghai /*= pApp->m_CShanghaiStock*/;
			CBussinessUtils::GetShanghaiStockInfo(shanghai);
			for(int i=0; i< shanghai.m_nRowNum; i++)
			{
				string strLastTradedate = pApp->m_StockDataLog.SearchStockByCode(shanghai.m_vec_code[i]).cninfo_bonus;
				if(strLastTradedate < lastYear)
				{
					CCninfoBonus * cashFlows = new CCninfoBonus;
					cashFlows->m_hWnd = pStock->m_hwnd;
					cashFlows->m_nCode = shanghai.m_vec_code[i];
					cashFlows->m_nLogType = pStock->m_nLogType;
					vecStockModel.push_back(cashFlows);
				}
				

				CCninfoPlacementOfShare * cashFlows_1 = new CCninfoPlacementOfShare;
				cashFlows_1->m_hWnd = pStock->m_hwnd;
				cashFlows_1->m_nCode = shanghai.m_vec_code[i];
				cashFlows_1->m_nLogType = pStock->m_nLogType;
				vecTmp.push_back(cashFlows_1);
			}

			// 打乱下载顺序
			srand(time(0));
			random_shuffle(vecStockModel.begin(), vecStockModel.end());
			random_shuffle(vecTmp.begin(), vecTmp.end());

			vecStockModel.insert(vecStockModel.end(), vecTmp.begin(), vecTmp.end());
		}
		//  下载同花顺分红融资数据
		else if(pStock->m_nDownloadType == ID_DOWNLOAD_10JQKA_BONUS_SHARE)
		{
			CStockApp * pApp = (CStockApp *)AfxGetApp();
			CEntrepreneurship Entrepreneurship/* = pApp->m_CEntrepreneurship*/;
			CBussinessUtils::GetEntrepreneurship(Entrepreneurship);
			for(int i=0; i< Entrepreneurship.m_nRowNum; i++)
			{
				CBonusOf10jqka * cashFlows = new CBonusOf10jqka;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode = Entrepreneurship.m_vec_code[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);

			}

			CSmallMidEnterprise midsmall /*= pApp->m_CSmallMidEnterprise*/;
			CBussinessUtils::GetSmallMidEnterprise(midsmall);
			for(int i=0; i< midsmall.m_nRowNum; i++)
			{
				CBonusOf10jqka * cashFlows = new CBonusOf10jqka;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode = midsmall.m_vec_code[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);
			}

			CShenzhenAshares shenzhenA /*= pApp->m_CShenzhenAshares*/;
			CBussinessUtils::GetShenzhenAshares(shenzhenA);
			for(int i=0; i< shenzhenA.m_nRowNum; i++)
			{
				CBonusOf10jqka * cashFlows = new CBonusOf10jqka;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode = shenzhenA.m_vec_code[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);

			}

			CShanghaiStockInfo shanghai /*= pApp->m_CShanghaiStock*/;
			CBussinessUtils::GetShanghaiStockInfo(shanghai);
			for(int i=0; i< shanghai.m_nRowNum; i++)
			{
				CBonusOf10jqka * cashFlows = new CBonusOf10jqka;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode = shanghai.m_vec_code[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);

			}

			// 打乱下载顺序
			srand(time(0));
			random_shuffle(vecStockModel.begin(), vecStockModel.end());
		}
		// 下载 上证综合指数
		else if(pStock->m_nDownloadType==ID_DOWNLOAD_SHANGHAI_COMPOSITE_INDEX)
		{
			CSohuIndexDaily * tradingDaily = new CSohuIndexDaily;
			tradingDaily->m_nCode = 000001;
			tradingDaily->m_hWnd = pStock->m_hwnd;
			tradingDaily->m_nLogType = pStock->m_nLogType;
			vecStockModel.push_back(tradingDaily);
		}
		// 下载 沪深300 指数
		else if(pStock->m_nDownloadType==ID_DOWNLOAD_HUSHENG300_INDEX)
		{
			CSohuIndexDaily * tradingDaily = new CSohuIndexDaily;
			tradingDaily->m_nCode = 300;
			tradingDaily->m_hWnd = pStock->m_hwnd;
			tradingDaily->m_nLogType = pStock->m_nLogType;
			vecStockModel.push_back(tradingDaily);
		}
		// 下载 深证成指
		else if(pStock->m_nDownloadType==ID_DOWNLOAD_SHENZHENG_COMPOSITE_INDEX)
		{
			CSohuIndexDaily * tradingDaily = new CSohuIndexDaily;
			tradingDaily->m_nCode = 399001;
			tradingDaily->m_hWnd = pStock->m_hwnd;
			tradingDaily->m_nLogType = pStock->m_nLogType;
			vecStockModel.push_back(tradingDaily);
		}
		// 下载 中小板综合指数
		else if(pStock->m_nDownloadType==ID_DOWNLOAD_SMALL_MIDDLE_INDEX)
		{
			CSohuIndexDaily * tradingDaily = new CSohuIndexDaily;
			tradingDaily->m_nCode = 399101;
			tradingDaily->m_hWnd = pStock->m_hwnd;
			tradingDaily->m_nLogType = pStock->m_nLogType;
			vecStockModel.push_back(tradingDaily);
		}
		// 下载 创业板综合指数
		else if(pStock->m_nDownloadType==ID_DOWNLOAD_ENTERPRENUER_INDEX)
		{
			CSohuIndexDaily * tradingDaily = new CSohuIndexDaily;
			tradingDaily->m_nCode = 399102;
			tradingDaily->m_hWnd = pStock->m_hwnd;
			tradingDaily->m_nLogType = pStock->m_nLogType;
			vecStockModel.push_back(tradingDaily);
		}
		//	下载 中证指数公司每日市盈率 等数据
		else if(pStock->m_nDownloadType == ID_DOWNLOAD_CSINDEX_PE_DAILY_SELECTED)
		{
			CStockApp * pApp = (CStockApp *)AfxGetApp();
			for(int i=0; i< pStock->m_vecCode.size(); i++)
			{
				CCsiPeDaily * cashFlows = new CCsiPeDaily;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode =pStock->m_vecCode[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);
			}
		}
		//	下载 中证指数公司 证监会行业 每日市盈率 等数据
		else if(pStock->m_nDownloadType == ID_DOWNLOAD_CSINDEX_CSRC_PE_DAILY_SELECTED)
		{
			CStockApp * pApp = (CStockApp *)AfxGetApp();
			for(int i=0; i< pStock->m_vecCode.size(); i++)
			{
				CCsiCsrcPeDaily * cashFlows = new CCsiCsrcPeDaily;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode =pStock->m_vecCode[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);
			}
		}
		//	下载 投资者保护基金 银证转账交易金 等数据
		else if(pStock->m_nDownloadType == ID_DOWNLOAD_SIPF_SETTLEMENT_FUND_SELECTED)
		{
			CStockApp * pApp = (CStockApp *)AfxGetApp();
			for(int i=0; i< pStock->m_vecCode.size(); i++)
			{
				CFundInSettlement * cashFlows = new CFundInSettlement;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode =pStock->m_vecCode[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);
			}
		}
		//	下载 上海证券交易所 融资融券数据
		else if(pStock->m_nDownloadType == ID_DOWNLOAD_SSE_MARGIN_TRADING_SELECTED)
		{
			CStockApp * pApp = (CStockApp *)AfxGetApp();
			for(int i=0; i< pStock->m_vecCode.size(); i++)
			{
				CSseMarginTrading * cashFlows = new CSseMarginTrading;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode =pStock->m_vecCode[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);
			}
		}
		//	下载 深圳证券交易所 融资融券数据
		else if(pStock->m_nDownloadType == ID_DOWNLOAD_SZSE_MARGIN_TRADING_SELECTED)
		{
			CStockApp * pApp = (CStockApp *)AfxGetApp();
			for(int i=0; i< pStock->m_vecCode.size(); i++)
			{
				CSzseMarginTrading * cashFlows = new CSzseMarginTrading;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode =pStock->m_vecCode[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);
			}
		}
		//	下载 中国证券结算公司 投资者情况统计表
		else if(pStock->m_nDownloadType == ID_DOWNLOAD_CSDC_INVESTOR_STAT_SELECTED)
		{
			CStockApp * pApp = (CStockApp *)AfxGetApp();
			for(int i=0; i< pStock->m_vecCode.size(); i++)
			{
				CCsdcInvestorStat * cashFlows = new CCsdcInvestorStat;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode =pStock->m_vecCode[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);
			}
		}
		//	下载 中国证券结算公司 证券发行及登记统计表
		else if(pStock->m_nDownloadType == ID_DOWNLOAD_CSDC_SECURITY_ISSUE_STAT_SELECTED)
		{
			CStockApp * pApp = (CStockApp *)AfxGetApp();
			for(int i=0; i< pStock->m_vecCode.size(); i++)
			{
				CCsdcSecurityIssueStat * cashFlows = new CCsdcSecurityIssueStat;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode =pStock->m_vecCode[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);
			}
		}
		// 下载 中国金融期货交易所 各个期货合约每日行情
		else if(pStock->m_nDownloadType==ID_DOWNLOAD_CFFEX_FUTURE_DAILY)
		{
			CCffexFutureDaily * pCf1 = new CCffexFutureDaily;
			vector<string>	vecDate = pCf1->NeedDownload();
			delete pCf1;

			for(int k= 0; k < vecDate.size(); k++)
			{
				CCffexFutureDaily * pCf = new CCffexFutureDaily;
				pCf->m_request_trade_date = vecDate[k];
				pCf->m_hWnd = pStock->m_hwnd;
				pCf->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(pCf);
			}
		}
		// 下载 国家统计局 月度数据
		else if(pStock->m_nDownloadType==ID_DOWNLOAD_STATSGOV_HGYD)
		{
			CStatsGovHgyd * pCf1 = new CStatsGovHgyd;
			pCf1->m_hWnd = pStock->m_hwnd;
			pCf1->m_nLogType = pStock->m_nLogType;
			vector<string>	vecDate = pCf1->NeedDownload();
			delete pCf1;

			for(int k= 0; k < vecDate.size(); k++)
			{
				CStatsGovHgyd * pCf = new CStatsGovHgyd;
				pCf->m_request_zb_code = vecDate[k];
				pCf->m_hWnd = pStock->m_hwnd;
				pCf->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(pCf);
			}
		}
		// 下载 国家统计局 季度数据
		else if(pStock->m_nDownloadType==ID_DOWNLOAD_STATSGOV_HGJD)
		{
			CStatsGovHgjd * pCf1 = new CStatsGovHgjd;
			pCf1->m_hWnd = pStock->m_hwnd;
			pCf1->m_nLogType = pStock->m_nLogType;
			vector<string>	vecDate = pCf1->NeedDownload();
			delete pCf1;

			for(int k= 0; k < vecDate.size(); k++)
			{
				CStatsGovHgjd * pCf = new CStatsGovHgjd;
				pCf->m_request_zb_code = vecDate[k];
				pCf->m_hWnd = pStock->m_hwnd;
				pCf->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(pCf);
			}
		}
		// 下载 国家统计局 年度数据
		else if(pStock->m_nDownloadType==ID_DOWNLOAD_STATSGOV_HGND)
		{
			CStatsGovHgnd * pCf1 = new CStatsGovHgnd;
			pCf1->m_hWnd = pStock->m_hwnd;
			pCf1->m_nLogType = pStock->m_nLogType;
			vector<string>	vecDate = pCf1->NeedDownload();
			delete pCf1;

			for(int k= 0; k < vecDate.size(); k++)
			{
				CStatsGovHgnd * pCf = new CStatsGovHgnd;
				pCf->m_request_zb_code = vecDate[k];
				pCf->m_hWnd = pStock->m_hwnd;
				pCf->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(pCf);
			}
		}
		// 下载 国家统计局 分省月度数据
		else if(pStock->m_nDownloadType==ID_DOWNLOAD_STATSGOV_FSYD)
		{
			CStatsGovFsyd * pCf1 = new CStatsGovFsyd;
			pCf1->m_hWnd = pStock->m_hwnd;
			pCf1->m_nLogType = pStock->m_nLogType;
			vector<string>	vecDate = pCf1->NeedDownload();
			delete pCf1;

			for(int k= 0; k < vecDate.size(); k++)
			{
				CStatsGovFsyd * pCf = new CStatsGovFsyd;
				pCf->m_request_zb_code = vecDate[k];
				pCf->m_hWnd = pStock->m_hwnd;
				pCf->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(pCf);
			}
		}
		// 下载 国家统计局 分省	季度数据
		else if(pStock->m_nDownloadType==ID_DOWNLOAD_STATSGOV_FSJD)
		{
			CStatsGovFsjd * pCf1 = new CStatsGovFsjd;
			pCf1->m_hWnd = pStock->m_hwnd;
			pCf1->m_nLogType = pStock->m_nLogType;
			vector<string>	vecDate = pCf1->NeedDownload();
			delete pCf1;

			for(int k= 0; k < vecDate.size(); k++)
			{
				CStatsGovFsjd * pCf = new CStatsGovFsjd;
				pCf->m_request_zb_code = vecDate[k];
				pCf->m_hWnd = pStock->m_hwnd;
				pCf->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(pCf);
			}
		}
		// 下载 国家统计局 分省年度数据
		else if(pStock->m_nDownloadType==ID_DOWNLOAD_STATSGOV_FSND)
		{
			CStatsGovFsnd * pCf1 = new CStatsGovFsnd;
			pCf1->m_hWnd = pStock->m_hwnd;
			pCf1->m_nLogType = pStock->m_nLogType;
			vector<string>	vecDate = pCf1->NeedDownload();
			delete pCf1;

			for(int k= 0; k < vecDate.size(); k++)
			{
				CStatsGovFsnd * pCf = new CStatsGovFsnd;
				pCf->m_request_zb_code = vecDate[k];
				pCf->m_hWnd = pStock->m_hwnd;
				pCf->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(pCf);
			}
		}
		// 下载 深圳交易所 股指样本数据
		else if(pStock->m_nDownloadType==ID_DOWNLOAD_SHENZHEN_INDEX_YANGBEN)
		{
			CStockApp * pApp = (CStockApp *)AfxGetApp();
			for(int k= 0; k < pApp->m_ShenzhenIndexList.m_vec_code.size(); k++)
			{
				CShenzhenIndexYangben * pCf = new CShenzhenIndexYangben;
				pCf->m_nCode = atoi(pApp->m_ShenzhenIndexList.m_vec_code[k].c_str());
				pCf->m_hWnd = pStock->m_hwnd;
				pCf->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(pCf);
			}

		}
		//
		else if(pStock->m_nDownloadType==ID_DOWNLOAD_SZSE_INDEX_DAILY_SELECTED)
		{
			CStockApp * pApp = (CStockApp *)AfxGetApp();
			for(int i=0; i< pStock->m_vecCode.size(); i++)
			{
				CShenzhenIndexDaily * cashFlows = new CShenzhenIndexDaily;
				cashFlows->m_hWnd = pStock->m_hwnd;
				cashFlows->m_nCode =pStock->m_vecCode[i];
				cashFlows->m_nLogType = pStock->m_nLogType;
				vecStockModel.push_back(cashFlows);
			}
		}
		else 
		{

		}
	}
	//ReleaseReadLock(&g_RWlock);


	CString strOutput;  
	int	nTry = 0;
	while( vecStockModel.size()>0 && ((CStockApp *)AfxGetApp())->TestRequestExit()==FALSE)
	{
		// 通过发送信息到 AutoDownloadView， 可以判断是否视图已经被关闭，关闭则退出线程
		int nRemains = vecStockModel.size();
		CString strBegin;
		strBegin.Format("【%06d】 Download %s, 余下 %d  \r\n", 
			vecStockModel[0]->m_nCode, vecStockModel[0]->ModelType().c_str(), nRemains);
		strOutput = strBegin;


		if(nRemains%10 == 1 && sendToOutput(strOutput, pStock->m_hwnd, pStock->m_nLogType)==FALSE)
		{
			break;
		}

		//	发现 开始计算分析，则暂停下载，以便释放数据库连接
		if(CTechnicalAnalysis::getInstance().m_vecIndex.size()>0)
		{
			strOutput.Format("技术分析中，下载暂停。。。。。。");
			sendToOutput(strOutput, pStock->m_hwnd, pStock->m_nLogType);

			while(CTechnicalAnalysis::getInstance().m_vecIndex.size()>0)
				Sleep(10 * 1000);
		}

		//	发现 开始计算分析，则暂停下载，以便释放数据库连接
		if(CSimulatedAccountAnalysis::getInstance().m_mapSimulatedAccount.size()>0)
		{
			strOutput.Format("模拟交易中，下载暂停。。。。。。");
			sendToOutput(strOutput, pStock->m_hwnd, pStock->m_nLogType);

			while(CSimulatedAccountAnalysis::getInstance().m_mapSimulatedAccount.size()>0)
				Sleep(10 * 1000);
		}

		//	对于多次下载失败的任务，取消
		if(nTry>=3)
		{
			delete vecStockModel[0];
			vecStockModel.erase(vecStockModel.begin());
			nTry = 0;
			continue;
		}
		nTry ++;
		if(nTry>1)
			Sleep(100);

		string downloadUrl = vecStockModel[0]->GetDownloadUrl();
		if(downloadUrl.empty()==false && ((CStockApp *)AfxGetApp())->TestRequestExit()==FALSE)
		{
			//////////////////////////////////////////////////////////////////////////
			//	创建管道，用于与 wget.exe 进程通信
			SECURITY_ATTRIBUTES sa;    
			HANDLE hRead,hWrite;    

			sa.nLength = sizeof(SECURITY_ATTRIBUTES);    
			sa.lpSecurityDescriptor = NULL;    
			sa.bInheritHandle = TRUE;    
			if(!CreatePipe(&hRead,&hWrite,&sa,0))    
			{     
				AfxMessageBox("CreatePipe Failed");    
				break;    
			}    

			//////////////////////////////////////////////////////////////////////////
			//	开启 wget.exe 进程，开始报表下载
			STARTUPINFO si;     
			PROCESS_INFORMATION pi;    
			ZeroMemory(&si,sizeof(STARTUPINFO));    
			si.cb = sizeof(STARTUPINFO);    
			GetStartupInfo(&si);     
			si.hStdError = hWrite;    
			si.hStdOutput = hWrite;    
			si.wShowWindow = SW_HIDE;     
			si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;    



			if(!CreateProcess(NULL,(LPSTR)downloadUrl.c_str(),NULL,NULL,TRUE,NULL,NULL,NULL,&si,&pi))    
			{     
				AfxMessageBox("CreateProcess failed!");    
				break;    
			}     

			HANDLE hThreads[2];
			hThreads[0]=pi.hProcess;
			hThreads[1]=((CStockApp *)AfxGetApp())->m_bAppExit;
			//WaitForSingleObject( pi.hProcess, INFINITE );

			bool bAppExit = false;
			int  nTimeoutNum = 0;
			while(1)
			{
				DWORD wfm = WaitForMultipleObjects(2, hThreads, FALSE, 1000 /*INFINITE*/);
				if(wfm==WAIT_OBJECT_0 + 1)
				{
					// 主程序退出
					bAppExit = true;
					break;
				}
				else if(wfm == WAIT_TIMEOUT)
				{
					nTimeoutNum ++;
					/*超时，则判断超过 10 秒，终止 wget.exe 进程*/
					if(nTimeoutNum>=10)
					{
						TerminateProcess(pi.hProcess,0);
						break;
					}
				}
				else 
				{
					/*wget.exe 下载完毕后终止*/
					break;
				}
			}
			if(bAppExit)
				break;

			CloseHandle(pi.hProcess);

			CloseHandle(hWrite);    

			//////////////////////////////////////////////////////////////////////////
			//	监控 wget.exe 的输出

			char buffer[4096] = {0};    
			DWORD bytesRead;    

			while(1)    
			{     
				if(NULL == ReadFile(hRead,buffer,4095,&bytesRead,NULL))     
				{     
					break;    
				}     

				//strOutput += buffer;     
				strOutput = buffer;    
				sendToOutput(strOutput, pStock->m_hwnd,  pStock->m_nLogType);
				Sleep(500);    
			}    
			CloseHandle(hRead);    

		}

		//////////////////////////////////////////////////////////////////////////
		//	转成 CSV 文件
		string strCsvFile = vecStockModel[0]->SaveAsCsv();
		if(strCsvFile.empty()==false &&((CStockApp *)AfxGetApp())->TestRequestExit()==FALSE)
		{
			if(strCsvFile=="NoMoreData")
			{
				// 当前数据库中已为最新数据

				//	发送“数据库导入成功”通知
				vecStockModel[0]->NotifyDatabaseImportedToWindow();

				//	删除已经成功的任务
				delete vecStockModel[0];
				vecStockModel.erase(vecStockModel.begin());
				nTry = 0;

				Sleep(100);
			}
			else if(strCsvFile=="AlreadyInMemory")
			{
				vecStockModel[0]->ImportToDatabase();

				//	发送“数据库导入成功”通知
				vecStockModel[0]->NotifyDatabaseImportedToWindow();

				//	删除已经成功的任务
				delete vecStockModel[0];
				vecStockModel.erase(vecStockModel.begin());
				nTry = 0;

				pStock->m_strResult = "NewData";
			}
			else
			{
				//	读入 CSV 文件到内存
				vector<string>  arrayHeader;
				vector<vector<string>>  arrayData;
				try
				{
					CCSVFile csvFile(strCsvFile.c_str());
					csvFile.ReadAllData(arrayHeader, arrayData);
					csvFile.Close();
				}
				catch (.../*CFileException* e*/)
				{
					stringstream ss;
					ss<< "打开 CSV 文件错误，可以被 wget.exe 锁定";
					sendToOutput(ss.str().c_str(), vecStockModel[0]->m_hWnd,  pStock->m_nLogType);
				}


				//	导入数据库
				int checkResult = vecStockModel[0]->CheckReport(arrayHeader, arrayData);
				if(checkResult==CStockDataModel::CSV_CHECK_NO_ERROR)
				{
					CString str;
					str.Format("CSV 文件中内容符合 \"%s\"，确定开始导入数据库 \r\n", \
						vecStockModel[0]->ModelType().c_str());
					//AfxMessageBox(str);
					sendToOutput(str.GetBuffer(), vecStockModel[0]->m_hWnd,  pStock->m_nLogType);
					vecStockModel[0]->ParseCsvFile(arrayHeader, arrayData);
					vecStockModel[0]->ImportToDatabase();

					//	发送“数据库导入成功”通知
					vecStockModel[0]->NotifyDatabaseImportedToWindow();

					//	删除已经成功的任务
					delete vecStockModel[0];
					vecStockModel.erase(vecStockModel.begin());
					nTry = 0;

					pStock->m_strResult = "NewData";
				}
				else if(checkResult==CStockDataModel::CSV_CHECK_DATA_EXIST_ERROR)
				{
					//	发送“数据库导入成功”通知
					vecStockModel[0]->ParseCsvFile(arrayHeader, arrayData);
					vecStockModel[0]->NotifyDatabaseImportedToWindow();

					//	删除已经成功的任务
					delete vecStockModel[0];
					vecStockModel.erase(vecStockModel.begin());
					nTry = 0;
				}
				else
				{
					int k = 1;
				}
			}
		}

		Sleep(20);
	}

	for(int m=0; m< vecStockModel.size(); m++)
	{
		if(vecStockModel[m])
			delete vecStockModel[m];
	}

	if(pStock->m_nDownloadType==ID_DOWNLOAD_CNINFO_ANNOUNCEMENT &&((CStockApp *)AfxGetApp())->TestRequestExit()==FALSE)
	{
		//	下载完成后，补充填写“利率表”的财报发布日期
		session sql(g_MysqlPool);
		sql << "call FinanceRptReleaseDateForAllStock()";

	}

	{
		//	通知任务的源窗口，任务完成
		stringstream ss;
		ss << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>";
		ss << "<root>";
		ss << "<msg_type>DownloadFinished</msg_type>";
		ss << "<data param1=\"" << pStock->m_nDownloadType << "\" " ;
		ss << " param2=\"" << pStock->m_strResult << "\" >";
		ss << "</data></root>";

		sendToOutput(ss.str().c_str(), pStock->m_hwnd);
	}

	int nDownType = pStock->m_nDownloadType;
	delete pStock;

	//////////////////////////////////////////////////////////////////////////
	//	下载完成后，进行一些统计分析


	if(((CStockApp *)AfxGetApp())->TestRequestExit()==FALSE)
	{
		COleDateTime dt;
		dt = COleDateTime::GetCurrentTime();
		string strNow = dt.Format("%Y-%m-%d").GetBuffer();

		((CStockApp *)AfxGetApp())->m_StockDataLog.UpdateStockByCode(-1, "", "");

		if(nDownType == ID_DOWNLOAD_TRADINGDAILY_SHANGHAISTOCK )
		{
			string max = ((CStockApp *)AfxGetApp())->m_StockDataLog.MaxDateByTable("SouhuTradingDaily");
			string bRet = ((CStockApp *)AfxGetApp())->m_StockDataLog.CheckTable("SouhuTradingDaily");
			if(bRet == "000511,000629,000950,600432,600806")
				((CStockApp *)AfxGetApp())->m_StockDataLog.UpdateTable("SouhuTradingDaily", max);

			max = ((CStockApp *)AfxGetApp())->m_StockDataLog.MaxDateByTable("NetEase_Trade_Daily");
			bRet = ((CStockApp *)AfxGetApp())->m_StockDataLog.CheckTable("NetEase_Trade_Daily");
			if(bRet == "000511,000629,000950,600432,600806")
				((CStockApp *)AfxGetApp())->m_StockDataLog.UpdateTable("NetEase_Trade_Daily", max);

			stringstream ss;
			ss << "NetEase_Trade_Daily 下载完成， " << bRet;
			LOG4CPLUS_DEBUG(g_logger, ss.str());
			
		}
		if(nDownType == ID_DOWNLOAD_CNINFO_BONUS_SHARE )
		{
			((CStockApp *)AfxGetApp())->m_StockDataLog.UpdateTable("cninfo_bonus", strNow);
			((CStockApp *)AfxGetApp())->m_StockDataLog.UpdateTable("cninfo_placement_of_share", strNow);
		}


		TechnicalAnalysis(nDownType);

		// AnalyseShanghaiStock(nDownType);

	}
	
	return 0;
}
