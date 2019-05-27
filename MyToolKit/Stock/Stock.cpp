
// Stock.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"

#include "TechAnalysisEvent.h"

#include "afxwinappex.h"
#include "Stock.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "ChildSplitterFrm.h"
#include "StockDoc.h"
#include "StockView.h"

#include "CsvDocument.h"
#include "CsvView.h"

#include "DatabaseDoc.h"
#include "DatabaseView.h"

#include "AutoDownloadDoc.h"
#include "AutoDownloadView.h"

#include "IndividualShareDoc.h"
#include "IndividualShareView.h"
#include "IndividualSummaryView.h"
#include "IndividualCostView.h"
#include "IndividualSzsePeView.h"
#include "IndividualVolumeView.h"
#include "IndividualYahooPeView.h"
#include "IndividualNetProfitsView.h"
#include "IndividualCurrentAssetView.h"
#include "IndividualNonCurrentAssetView.h"
#include "IndividualCapitalStructView.h"
#include "IndividualCsvFileView.h"
#include "IndividualCapitalFlowView.h"
#include "IndiCashFlowSummaryView.h"
#include "IndiZombieAnalysisView.h"
#include "IndiFundHoldValueView.h"
#include "IndiMacdView.h"
#include "IndiMainIncomeGrowthView.h"
#include "IndiMcstView.h"
#include "Indi10jqkaBonusView.h"
#include "IndiHexunResearchPapaerView.h"
#include "IndiMarketIndexAnalysisView.h"
#include "IndiCsindexView.h"
#include "IndiGrossProfitRateView.h"
#include "IndiCninfoAnnouncementRoadView.h"
#include "IndiPeAndValueDailyView.h"
#include "IndiIncomestatementForecastView.h"
#include "IndiDmiView.h"
#include "IndiTechnicalAnalysisRoadView.h"

#include "PlateStockListView.h"
#include "PlateGrossProfitListView.h"
#include "PlateLiquidityRatioView.h"
#include "PlateNetProfitsRatioView.h"
#include "PlateTotalAssetsTurnoverRatioView.h"
#include "PlateDebtToAssetsRatioView.h"
#include "PlateDebtToCapitalRatioView.h"
#include "PlateInventoryTurnoverRatioView.h"
#include "PlateCurrentAssetsTurnoverRatioView.h"
#include "PlateFixedAssetsTurnoverRatioView.h"
#include "PlateFundHoldView.h"
#include "PlateTotalAssetsGrowthView.h"
#include "PlateNetFixedAssestsGrowthView.h"
#include "PlateMainIncomeGrowthView.h"
#include "PlateCorrectedMarketValueView.h"
#include "PlateCsindexDynamicPeView.h"
#include "PlateDealDetailDailyView.h"
#include "PlateTurnoverRateView.h"
#include "PlateChangeRateSummaryView.h"
#include "PlateGrossProfitRateGrowthView.h"
#include "PlateShanghaiIndexView.h"

#include "BaseHtmlView.h"
#include "BaseChartCtlView.h"

#include "AllStockListView.h"
#include "AllMainIncomeYearGrowthStockListView.h"
#include "AllMainProfitYearGrowthStockListView.h"
#include "AllRecentBonusCashStockListView.h"
#include "AllThisYearBonusListView.h"
#include "AllMcstStockListView.h"
#include "AllAddIssuanceListView.h"
#include "AllCsiDynamicPeStockListView.h"
#include "SimulatedAccountListView.h"

#include "ZombieStockListView.h"
#include "MainProductQueryView.h"
#include "PeStockListView.h"
#include "MacdStockListView.h"
#include "HexunResearchPaperListView.h"
#include "CninfoAnnouncementListView.h"
#include "CninfoAnnouncePDFListView.h"
#include "CninfoIncentivePlanListView.h"
#include "CninfoGetLandListView.h"
#include "CninfoShareHoldingListView.h"
#include "CninfoShareHoldingStatisticsView.h"


#include "MarketExchageFundView.h"
#include "MarketMarginGuaranteeFundView.h"
#include "MarketCffexHs300FutureView.h"
#include "MarketSohuIndexView.h"
#include "MarketCsdcInvestorStatView.h"

#include "StatsGovCnChartView.h"
#include "StatsGovCnGridView.h"
#include "StockHomePageView.h"

#include "ShanghaiIndexView.h"

#include "Newmat11TestView.h"

#include "SplashScreenEx.h"

#include "StockGroupDlg.h"

#include "SimpleInputBox.h"

#include "DyncItemEditDlg.h"

#include "nr.h"

#include "SendMail.h"

#include "compileUTF8range.h"

#include "PDFStructureDlg.h"



#include "CandlesticksJapanListView.h"
#include "MacdListView.h"
#include "BollListView.h"

//#include "stocktest.h"
//#include "StOpenWindow.h"
//#include "StCloseWindow.h"
//#include "StShanghaiIndex.h"
//#include "StShanghaiSohuPlateIndex.h"
//#include "StShanghaiStockIndex.h"
#include "StockMatlab.h"
#include "mclmcrrt.h"


#include "UrlUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define USERNAME "yss2002@qq.com"  
#define PASSWORD "h09161972"  
#define SMTPSERVER "smtp.qq.com"  
#define SMTPPORT ":25"   
#define RECIPIENT "<yss2002@qq.com>"  
#define MAILFROM "<yss2002@qq.com>" 


UINT InitialReadStockInfoThreadFunc(LPVOID param)
{
	BOOL bForever = (BOOL)param;
	CStockApp * pApp = (CStockApp *)AfxGetApp();
	stringstream ss;
	
	
	while(pApp->TestRequestExit()==FALSE)
	{
		//	根据窗口的请求，进行个股数据的读入
		StockInfo_Reqeust req;
		{
			//JMutexAutoLock lock(pApp->m_mutexStockinfo);
			if(pApp->m_listStockinfoReq.size()>0)
			{
				req = *pApp->m_listStockinfoReq.begin();
				pApp->m_listStockinfoReq.erase(pApp->m_listStockinfoReq.begin());
			}
			else
			{
				if(bForever==FALSE)
					break;
				else
				{
					Sleep(1000);
					continue;
				}
			}
		}
		if(req.code>=0)
		{
			//////////////////////////////////////////////////////////////////////////
			//	下载个股信息

			//	股本结构
			if(pApp->m_mapCapitalStruct.count(req.code)==0 && ((CStockApp *)AfxGetApp())->TestRequestExit()==FALSE)
			{
				CCapitalStructOf10jqka capital;
				capital.m_nCode = req.code;
				capital.m_nLogType = req.logType;
				capital.ExportFromDatabase();
				if(capital.m_vec_real_date.size()>0)
				{
					AcquireWriteLock(&g_RWlock);
					pApp->m_mapCapitalStruct[req.code] = capital;
					ReleaseWriteLock(&g_RWlock);
				}					
			}
			
			//////////////////////////////////////////////////////////////////////////
			//	基金持有
			if(pApp->m_mapFundHold.count(req.code)==0 && ((CStockApp *)AfxGetApp())->TestRequestExit()==FALSE)
			{
				CSohuFundHold capital;
				capital.m_nCode = req.code;
				capital.m_nLogType = req.logType;
				capital.ExportFromDatabase();
				if(capital.m_vec_report_date.size()>0)
				{
					AcquireWriteLock(&g_RWlock);
					pApp->m_mapFundHold[req.code] = capital;
					ReleaseWriteLock(&g_RWlock);
				}					
			}
			
			//////////////////////////////////////////////////////////////////////////
			//	资产负债表

			if(pApp->TestRequestExit()==FALSE)
			{
				if(pApp->m_mapBalance.count(req.code)==0)
				{
					//	内存中不存在该股票财报
					CCninfoBalance balance;
					balance.m_nCode = req.code;
					balance.m_nLogType =  req.logType;
					if(req.strAfterDate.empty())
						balance.ExportFromDatabase();
					
					if(balance.m_vec_row.size()>0 && pApp->TestRequestExit()==FALSE)
					{
						AcquireWriteLock(&g_RWlock);
						pApp->m_mapBalance[req.code]=balance;
						ReleaseWriteLock(&g_RWlock);
					}					
				}
			}

			//////////////////////////////////////////////////////////////////////////
			//	现金流量表

			if(pApp->TestRequestExit()==FALSE)
			{
				if(pApp->m_mapCashFlows.count(req.code)==0)
				{
					//	内存中不存在该股票财报
					CCninfoCashFlows balance;
					balance.m_nCode = req.code;
					balance.m_nLogType =  req.logType;
					if(req.strAfterDate.empty())
						balance.ExportFromDatabase();
					
					if(balance.m_vec_row.size()>0  && pApp->TestRequestExit()==FALSE)
					{
						AcquireWriteLock(&g_RWlock);
						pApp->m_mapCashFlows[req.code]=balance;
						ReleaseWriteLock(&g_RWlock);
					}					
				}
			}

			//////////////////////////////////////////////////////////////////////////
			//	利润表

			if(pApp->TestRequestExit()==FALSE)
			{
				if(pApp->m_mapIncomeStatement.count(req.code)==0)
				{
					//	内存中不存在该股票财报
					CCninfoIncomeStatement balance;
					balance.m_nCode = req.code;
					balance.m_nLogType =  req.logType;
					if(req.strAfterDate.empty())
						balance.ExportFromDatabase();
					
					if(balance.m_vec_row.size()>0  && pApp->TestRequestExit()==FALSE)
					{
						AcquireWriteLock(&g_RWlock);
						pApp->m_mapIncomeStatement[req.code]=balance;
						ReleaseWriteLock(&g_RWlock);
					}					
				}
		
			}
			
			//////////////////////////////////////////////////////////////////////////
			//	每日交易数据
			if(req.strAfterDate.empty())
			{
				//	搜狐每日交易数据
				if(pApp->m_mapTradingDaily.count(req.code)==0 && pApp->TestRequestExit()==FALSE)
				{
					CNetEaseTradeDaily trading;
					trading.m_nCode = req.code;
					trading.m_nLogType =  req.logType;
					trading.ExportFromDatabase();
					trading.Reorganize();
					if(trading.m_vec_reorganize.size()>0)
					{
						AcquireWriteLock(&g_RWlock);
						pApp->m_mapTradingDaily[req.code]= trading;
						ReleaseWriteLock(&g_RWlock);
					}					
						
				}

				//	163 每日资金流向数据
				if(pApp->m_mapCapitalFlows.count(req.code)==0 && pApp->TestRequestExit()==FALSE)
				{
					CCapitalFlows163 trading;
					trading.m_nCode = req.code;
					trading.m_nLogType =  req.logType;
					trading.ExportFromDatabase();
					if(trading.m_vec_report_date.size()>0)
					{
						AcquireWriteLock(&g_RWlock);
						pApp->m_mapCapitalFlows[req.code]= trading;
						ReleaseWriteLock(&g_RWlock);
					}				
						
				}
				//	搜狐业绩预告数据
				if(pApp->m_CSohuPerformanceForecast.count(req.code)==0 && pApp->TestRequestExit()==FALSE)
				{
					CSohuPerformanceForecast trading;
					trading.m_nCode = req.code;
					trading.m_nLogType =  req.logType;
					trading.ExportFromDatabase();
					if(trading.m_vec_report_date.size()>0)
					{
						AcquireWriteLock(&g_RWlock);
						pApp->m_CSohuPerformanceForecast[req.code]= trading;
						ReleaseWriteLock(&g_RWlock);
					}				
				}

				if(pApp->m_mapShenzhenQuotation.count(req.code)==0 && pApp->TestRequestExit()==FALSE)
				{
					CShenzhenMarketQuotation quotation;
					quotation.m_nCode = req.code;
					quotation.m_nLogType =  req.logType;
					quotation.ExportFromDatabaseByCode();
					if(quotation.m_vec_report_date.size()>0)
					{
						AcquireWriteLock(&g_RWlock);
						pApp->m_mapShenzhenQuotation[req.code]= quotation;
						ReleaseWriteLock(&g_RWlock);
					}				
				}
			}
			
				
			//	通知窗口，任务已经完成, 当 req.hwnd == NULL，说明为后台读入
			if(req.hwnd!=NULL)
			{
				stringstream ss;
				ss << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>";
				ss << "<root>";
				ss << "<msg_type>ReadFinished</msg_type>";
				ss << "<data>";
				ss << "</data></root>";

				sendToOutput(ss.str().c_str(), req.hwnd, WM_USER_XML);
			}
			
		}
		else if(req.code == -ID_READ_STOCKINFO_ENTERPRENUER)
		{
			//////////////////////////////////////////////////////////////////////////
			//	读入股票信息
			{
				//	此处添加了 {  } ，以便让 Entrepreneurship、SmallMidEnterprise 等局部变量尽快释放，
				//	避免程序结束时候，来不及释放，导致内存泄露
				CEntrepreneurship Entrepreneurship;
				Entrepreneurship.ExportFromDatabase();

				AcquireWriteLock(&g_RWlock);
				pApp->m_CEntrepreneurship= Entrepreneurship;
				ReleaseWriteLock(&g_RWlock);
			}

			//	通知主窗口，更新“股票信息”展示树
			ss.str("");
			ss << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>";
			ss << "<root>";
			ss << "<msg_type>ReadFinished</msg_type>";
			ss << "<data param1=\""<< req.code <<"\"  >";
			ss << "</data></root>";
			sendToOutput(ss.str().c_str(), AfxGetMainWnd()->m_hWnd, WM_USER_XML);

		}
		else if(req.code == -ID_READ_STOCKINFO_SMALLMID)
		{
			//////////////////////////////////////////////////////////////////////////
			//	读入股票信息
			CSmallMidEnterprise SmallMidEnterprise;
			SmallMidEnterprise.ExportFromDatabase();

			AcquireWriteLock(&g_RWlock);
			pApp->m_CSmallMidEnterprise = SmallMidEnterprise;
			ReleaseWriteLock(&g_RWlock);

			//	通知主窗口，更新“股票信息”展示树
			ss.str("");
			ss << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>";
			ss << "<root>";
			ss << "<msg_type>ReadFinished</msg_type>";
			ss << "<data param1=\""<< req.code <<"\"  >";
			ss << "</data></root>";
			sendToOutput(ss.str().c_str(), AfxGetMainWnd()->m_hWnd, WM_USER_XML);

		}
		else if(req.code == -ID_READ_STOCKINFO_SHENZHENA)
		{
			//////////////////////////////////////////////////////////////////////////
			//	读入股票信息
			CShenzhenAshares ShenzhenAshares;
			ShenzhenAshares.ExportFromDatabase();

			AcquireWriteLock(&g_RWlock);
			pApp->m_CShenzhenAshares = ShenzhenAshares;
			ReleaseWriteLock(&g_RWlock);

			//	通知主窗口，更新“股票信息”展示树
			ss.str("");
			ss << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>";
			ss << "<root>";
			ss << "<msg_type>ReadFinished</msg_type>";
			ss << "<data param1=\""<< req.code <<"\"  >";
			ss << "</data></root>";
			sendToOutput(ss.str().c_str(), AfxGetMainWnd()->m_hWnd, WM_USER_XML);

		}
		else if(req.code == -ID_READ_STOCKINFO_SHANGHAISTOCK)
		{
			//////////////////////////////////////////////////////////////////////////
			//	读入股票信息
			CShanghaiStockInfo ShanghaiStock;
			ShanghaiStock.ExportFromDatabase();

			AcquireWriteLock(&g_RWlock);
			pApp->m_CShanghaiStock = ShanghaiStock;
			ReleaseWriteLock(&g_RWlock);

			//	通知主窗口，更新“股票信息”展示树
			ss.str("");
			ss << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>";
			ss << "<root>";
			ss << "<msg_type>ReadFinished</msg_type>";
			ss << "<data param1=\""<< req.code <<"\"  >";
			ss << "</data></root>";
			sendToOutput(ss.str().c_str(), AfxGetMainWnd()->m_hWnd, WM_USER_XML);
		}
		else if(req.code == -ID_READ_STOCKPLATE_SOHU )
		{
			//////////////////////////////////////////////////////////////////////////
			//	读入搜狐板块信息
			{
				CSouhuStockPlates  SouhuStockPlates;
				SouhuStockPlates.ExportFromDatabase();
				CStockPlateTree * pTree = SouhuStockPlates.GenerateTree();
				vector<CStockPlateData>  vec1 = SouhuStockPlates.FetchDataFromTree(pTree, 1632);
				vector<CStockPlateData>  vec2 = SouhuStockPlates.FetchDataFromTree(pTree, 1630);
				vector<CStockPlateData>  vec3 = SouhuStockPlates.FetchDataFromTree(pTree, 1631);

				AcquireWriteLock(&g_RWlock);
				if(pApp->m_pCSouhuStockPlates!=NULL)
					delete pApp->m_pCSouhuStockPlates;
				pApp->m_pCSouhuStockPlates = pTree;

				pApp->m_vecStockPlateArea = vec1;
				pApp->m_vecStockPlateConcept = vec2;
				pApp->m_vecStockPlateIndustry = vec3;
				ReleaseWriteLock(&g_RWlock);
			}

			//	通知主窗口，更新“股票板块”展示树
			ss.str("");
			ss << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>";
			ss << "<root>";
			ss << "<msg_type>ReadFinished</msg_type>";
			ss << "<data param1=\""<< req.code <<"\"  >";
			ss << "</data></root>";
			sendToOutput(ss.str().c_str(), AfxGetMainWnd()->m_hWnd, WM_USER_XML);
		}
		else if(req.code == -ID_READ_STOCKPLATE_SELF_SELECT )
		{
			//////////////////////////////////////////////////////////////////////////
			//	读入搜狐板块信息
			{
				JMutexAutoLock lock(pApp->m_pCSelfSelectStockPlates->m_mutex);
				pApp->m_pCSelfSelectStockPlates->ExportFromDatabase();
			}

			//	通知主窗口，更新“股票板块”展示树
			ss.str("");
			ss << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>";
			ss << "<root>";
			ss << "<msg_type>ReadFinished</msg_type>";
			ss << "<data param1=\""<< req.code <<"\"  >";
			ss << "</data></root>";
			sendToOutput(ss.str().c_str(), AfxGetMainWnd()->m_hWnd, WM_USER_XML);

			//////////////////////////////////////////////////////////////////////////
			//	邮件发送
#ifndef	_DEBUG
			vector<string> vecMailContents = pApp->m_pCSelfSelectStockPlates->MailContents();

			const int MAX_BUFFER_LEN = 500;  
			char  szBuffer[MAX_BUFFER_LEN];  

			string computerName = "Unknown";
			string userName = "Unknown";
			DWORD dwNameLen;  
			dwNameLen = MAX_BUFFER_LEN;
			memset(szBuffer, 0, MAX_BUFFER_LEN);
			if (GetComputerName(szBuffer, &dwNameLen)) 
				computerName = string(szBuffer);

			dwNameLen = MAX_BUFFER_LEN;
			memset(szBuffer, 0, MAX_BUFFER_LEN);
			if (GetUserName(szBuffer, &dwNameLen)) 
				userName = string(szBuffer);

			CSendMail sendMail(USERNAME, PASSWORD, SMTPSERVER, 25, MAILFROM);      
			sendMail.AddRecipient(RECIPIENT);  
			sendMail.ConstructHead("System Start from " + userName + " on " + computerName, "");
			for(int i=0; i<vecMailContents.size(); i++)
			{
				sendMail.AddBodyContents(vecMailContents[i]);
			}
			sendMail.SendMail("", "");  
#endif

		}
		else if(req.code == -ID_READ_STOCKPLATE_CSINDEX )
		{
			//////////////////////////////////////////////////////////////////////////
			//	读入中证指数公司的行业板块信息
			CCsiPeDaily  CsindexStockPlates;
			string lastTradeDate = CsindexStockPlates.LastTradeDateFromDatabase();
			if(lastTradeDate.empty()==false)
			{
				CsindexStockPlates.m_nCode = atoi(lastTradeDate.c_str());
				CsindexStockPlates.ExportFromDatabase();
				CStockPlateTree * pTree = CsindexStockPlates.GenerateTree();

				AcquireWriteLock(&g_RWlock);
				if(pApp->m_pCsindexStockPlates!=NULL)
					delete pApp->m_pCsindexStockPlates;
				pApp->m_pCsindexStockPlates = pTree;
				ReleaseWriteLock(&g_RWlock);
			}

			//	通知主窗口，更新“股票板块”展示树
			ss.str("");
			ss << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>";
			ss << "<root>";
			ss << "<msg_type>ReadFinished</msg_type>";
			ss << "<data param1=\""<< req.code <<"\"  >";
			ss << "</data></root>";
			sendToOutput(ss.str().c_str(), AfxGetMainWnd()->m_hWnd, WM_USER_XML);
		}
		else if(req.code == -ID_READ_STOCKPLATE_CSINDEX_CSRC )
		{
			//////////////////////////////////////////////////////////////////////////
			//	读入中证指数公司的 证监会 行业板块信息
			CCsiCsrcPeDaily  CsindexStockPlates;
			string lastTradeDate = CsindexStockPlates.LastTradeDateFromDatabase();
			if(lastTradeDate.empty()==false)
			{
				CsindexStockPlates.m_nCode = atoi(lastTradeDate.c_str());
				CsindexStockPlates.ExportFromDatabase();
				CStockPlateTree * pTree = CsindexStockPlates.GenerateTree();

				AcquireWriteLock(&g_RWlock);
				if(pApp->m_pCsindexCsrcStockPlates!=NULL)
					delete pApp->m_pCsindexCsrcStockPlates;
				pApp->m_pCsindexCsrcStockPlates = pTree;
				ReleaseWriteLock(&g_RWlock);
			}

			//	通知主窗口，更新“股票板块”展示树
			ss.str("");
			ss << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>";
			ss << "<root>";
			ss << "<msg_type>ReadFinished</msg_type>";
			ss << "<data param1=\""<< req.code <<"\"  >";
			ss << "</data></root>";
			sendToOutput(ss.str().c_str(), AfxGetMainWnd()->m_hWnd, WM_USER_XML);
		}
		else if(req.code==-ID_READ_STATSGOV_HGYD)
		{
			//	读入国家统计局的月度数据菜单
			CStatsGovCn  statsGov;
			CStockPlateTree * pTree = statsGov.GenerateTree("hgyd");

			AcquireWriteLock(&g_RWlock);
			if(pApp->m_pCStatsGovHgydPlates!=NULL)
				delete pApp->m_pCStatsGovHgydPlates;
			pApp->m_pCStatsGovHgydPlates = pTree;
			ReleaseWriteLock(&g_RWlock);
		}
		else if(req.code==-ID_READ_STATSGOV_HGJD)
		{
			//	读入国家统计局的季度数据菜单
			CStatsGovCn  statsGov;
			CStockPlateTree * pTree = statsGov.GenerateTree("hgjd");

			AcquireWriteLock(&g_RWlock);
			if(pApp->m_pCStatsGovHgjdPlates!=NULL)
				delete pApp->m_pCStatsGovHgjdPlates;
			pApp->m_pCStatsGovHgjdPlates = pTree;
			ReleaseWriteLock(&g_RWlock);
		}
		else if(req.code==-ID_READ_STATSGOV_HGND)
		{
			//	读入国家统计局的年度数据菜单
			CStatsGovCn  statsGov;
			CStockPlateTree * pTree = statsGov.GenerateTree("hgnd");

			AcquireWriteLock(&g_RWlock);
			if(pApp->m_pCStatsGovHgndPlates!=NULL)
				delete pApp->m_pCStatsGovHgndPlates;
			pApp->m_pCStatsGovHgndPlates = pTree;
			ReleaseWriteLock(&g_RWlock);
		}
		else if(req.code==-ID_READ_STATSGOV_FSJD)
		{
			//	读入国家统计局的年度数据菜单
			CStatsGovCn  statsGov;
			CStockPlateTree * pTree = statsGov.GenerateTree("fsjd");

			AcquireWriteLock(&g_RWlock);
			if(pApp->m_pCStatsGovFsjdPlates!=NULL)
				delete pApp->m_pCStatsGovFsjdPlates;
			pApp->m_pCStatsGovFsjdPlates = pTree;
			ReleaseWriteLock(&g_RWlock);
		}
		else if(req.code==-ID_READ_STATSGOV_FSYD)
		{
			//	读入国家统计局的年度数据菜单
			CStatsGovCn  statsGov;
			CStockPlateTree * pTree = statsGov.GenerateTree("fsyd");

			AcquireWriteLock(&g_RWlock);
			if(pApp->m_pCStatsGovFsydPlates!=NULL)
				delete pApp->m_pCStatsGovFsydPlates;
			pApp->m_pCStatsGovFsydPlates = pTree;
			ReleaseWriteLock(&g_RWlock);
		}
		else if(req.code==-ID_READ_STATSGOV_FSND)
		{
			//	读入国家统计局的年度数据菜单
			CStatsGovCn  statsGov;
			CStockPlateTree * pTree = statsGov.GenerateTree("fsnd");

			AcquireWriteLock(&g_RWlock);
			if(pApp->m_pCStatsGovFsndPlates!=NULL)
				delete pApp->m_pCStatsGovFsndPlates;
			pApp->m_pCStatsGovFsndPlates = pTree;
			ReleaseWriteLock(&g_RWlock);
		}
		else if(req.code == -ID_READ_SHENZHEN_INDEXINFO)
		{
			//////////////////////////////////////////////////////////////////////////
			//	读入股票信息

			AcquireWriteLock(&g_RWlock);
			pApp->m_ShenzhenIndexList.SaveAsCsv();
			//	从 xls 文件中读入
			pApp->m_ShenzhenIndexList.GenerateTree();
			ReleaseWriteLock(&g_RWlock);

			//	通知主窗口，更新“股票信息”展示树
			ss.str("");
			ss << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>";
			ss << "<root>";
			ss << "<msg_type>ReadFinished</msg_type>";
			ss << "<data param1=\""<< req.code <<"\"  >";
			ss << "</data></root>";
			sendToOutput(ss.str().c_str(), AfxGetMainWnd()->m_hWnd, WM_USER_XML);
		}
		else
		{
			Sleep(1000);
		}
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
//

CMvDocTemplate::CMvDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
							   CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass)
							   : CMultiDocTemplate(nIDResource, pDocClass, pFrameClass, pViewClass)
{
}



//////////////////////////////////////////////////////////////////////////

// CStockApp

BEGIN_MESSAGE_MAP(CStockApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CStockApp::OnAppAbout)
	// 基于文件的标准文档命令
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// 标准打印设置命令
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// CStockApp 构造

CStockApp::CStockApp()
{

	m_bHiColorIcons = TRUE;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
	m_bAppExit = FALSE;
	//m_mutexStockinfo.Init();

	m_pCSouhuStockPlates = NULL;
	m_pCSelfSelectStockPlates = new CSelfSelectStockPlates;
	m_pCsindexStockPlates = NULL;
	m_pCsindexCsrcStockPlates = NULL;

	m_pCStatsGovHgydPlates = NULL;
	m_pCStatsGovHgjdPlates = NULL;
	m_pCStatsGovHgndPlates = NULL;

}

// 唯一的一个 CStockApp 对象

CStockApp theApp;


// CStockApp 初始化


BOOL CStockApp::InitInstance()
{
	//TODO: call AfxInitRichEdit2() to initialize richedit2 library.

	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	if( ! ::AfxInitRichEdit() )
	{
		return FALSE ;
	}

	

	// 初始化log4cplus
	PropertyConfigurator::doConfigure("log4cplus.properties");
	g_logger =	Logger::getInstance("Stock");

	LOG4CPLUS_INFO(g_logger, "==============  程序开始 ============");

	//////////////////////////////////////////////////////////////////////////
	//	读入 INI 文件

	stringstream ss;
	
	GetModuleFileName( AfxGetInstanceHandle(), theAppPath, MAX_PATH );

	string fullpath = theAppPath;
	int ipos = fullpath.find_last_of("\\");

	// 获取上级路径
	fullpath = fullpath.substr(0,ipos);
	fullpath += "\\stock.ini";

	ss << "fullpath = " << fullpath ;
	//LOG4CPLUS_DEBUG(g_logger, ss.str());
	ReadCfg(fullpath);

	CHAR theCurrPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, theCurrPath); 
	g_strCurrentDir = string(theCurrPath);

	//////////////////////////////////////////////////////////////////////////
	//

	/*std::locale langLocale("");
	std::locale::global(langLocale);*/

	compileUTF8range::init();

	string ssssss = compileUTF8range::printTest();

	

	//////////////////////////////////////////////////////////////////////////
	// 防止软件被跟踪及破解

#ifndef	_DEBUG
	BOOL bRes = FALSE;
	if (CheckRemoteDebuggerPresent(GetCurrentProcess(), &bRes) && bRes)
	{
		
		ss.str("");
		ss << "Oooo, Debugger Found !";
		AfxMessageBox(ss.str().c_str());
		return FALSE;
		
	}	
#endif

	//////////////////////////////////////////////////////////////////////////
	//

	list<string>	listMAC;
	list<string>	listCPU;
	string			strSLC;
	/*if(GetInfoFromDLK(strSLC, listMAC, listCPU)==false)
		return FALSE;*/

	/*time_t time_limit = convert_string_to_time_t(strSLC + " 23:59:59");
	if(time_limit <0 || time(NULL) >= time_limit)
		return FALSE;*/


	//////////////////////////////////////////////////////////////////////////
	//	
	m_bAppExit = CreateEvent(NULL, TRUE, FALSE, NULL);

	//	初始化，全局数据读写锁
	if (InitRWLock(&g_RWlock) == FALSE)
	{
		ss.str("");
		ss << "初始化全局数据读写锁,失败";
		AfxMessageBox(ss.str().c_str());
		return FALSE;
	}

	//////////////////////////////////////////////////////////////////////////
	//	启动 MYSQL 数据库的连接池

	try{
		string connectString_ =  "dbname=stock  port=3366   host=127.0.0.1 user=root  charset=gbk ";   // utf8
		backend_factory const &backEndFactory_ = *soci::factory_mysql();

		for (std::size_t i = 0; i != MYSQL_POOL_SIZE; ++i)
		{
			session & sql = g_MysqlPool.at(i);
			sql.open(backEndFactory_, connectString_);
		}
	}catch (std::exception const & e)
	{
		ss.str("");
		ss << "Error ！初始化 Mysql 连接池失败，请在桌面快捷方式打开 Mysql 数据库";
		ss << e.what() << ' \r\n';
		AfxMessageBox(ss.str().c_str());
		return FALSE;
	}

	//////////////////////////////////////////////////////////////////////////
	/*建立存储过程, sql 文件是以 unicode 编码的，如果sql 文件的开头两个字节是0xFFFE，那么就是Unicode，否则是ANSI */

	bool isOpenFile = true;
	CFile file;
	try
	{
		isOpenFile =file.Open("procedure.sql",CFile::modeRead);
	}
	catch(...)
	{
		isOpenFile = false;
	}
	
	if(/*isOpenFile*/  0 )
	{
		wchar_t * temp =new wchar_t[file.GetLength()/2+1];  
		file.Read(temp,file.GetLength());  
		temp[file.GetLength()/2]=0;  
		file.Close();

		CString str1(&temp[1]);		//	丢弃第一个 wchar 字符， CString 会实现 unicode 到 ansi 字符的转换
		delete[] temp;

		vector<string> firstMatch;
		if(Pcre2Split(_T("[#]+[@]+"), str1.GetBuffer(),firstMatch )>0)
		{
			MYSQL mysql;
			mysql_init(&mysql); //初始化mysql结构
			if(!mysql_real_connect(&mysql,"127.0.0.1","root",NULL,"stock",3366,NULL,0))//连接服务器
			{
				return FALSE;
			}

			for(int i=0; i< firstMatch.size(); i++)
			{
				try
				{
					if(mysql_query(&mysql, firstMatch[i].c_str()))
					{
						/*return FALSE;*/
						ss.str("");
						ss << "Error ！建立存储过程失败";
						AfxMessageBox(ss.str().c_str());
						return FALSE;
					}
				}
				catch (...)
				{
					ss.str("");
					ss << "Error ！建立存储过程失败";
					AfxMessageBox(ss.str().c_str());
					return FALSE;
				}
			}
			mysql_close(&mysql);//释放连接
		}
	}

	//////////////////////////////////////////////////////////////////////////

	CCninfoCashFlows   pdf;
	pdf.m_nCode = 2554;
	pdf.m_cninfo_orgid = "9900018066";
	//pdf.SaveAsCsv();
	//pdf.KaptchaAsJpeg();
	//pdf.Kaptcha();

	/*CCninfoCashFlows   pdf;
	pdf.m_nCode = 2554;
	for(int i=0; i<30; i++)
	{
		pdf.SaveAsJpeg();
		Sleep(5000);
	}*/
	
	/*{
		CCninfoCashFlows   pdf;
		pdf.m_nCode = 20150611;

		pdf.SaveAsCsv();

		int k = 1;
	}*/

	m_StockDataLog.FetchFromDB();

	/*string bRet =((CStockApp *)AfxGetApp())->m_StockDataLog.CheckTable("NetEase_Trade_Daily");
	
	TechnicalAnalysis(ID_DOWNLOAD_TRADINGDAILY_SHANGHAISTOCK);*/

	/*{
		CPDFStructureDlg  dlg;
		dlg.m_announcementId = "1202720571";
		dlg.DoModal();

		return FALSE;
	}*/

	
	

	//////////////////////////////////////////////////////////////////////////

	if(!friso_init())
		return FALSE;


	//	启动 巨潮网 PDF 分析程序
	if(CheckProcessOnlyOne("CninfoPDF.exe")==0)
	{
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		string fullpath = theAppPath;
		int ipos = fullpath.find_last_of("\\");

		fullpath = fullpath.substr(0,ipos);
		fullpath += "\\CninfoPDF.exe";

		memset(&si, 0, sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);
		CreateProcess(NULL, (LPSTR)fullpath.c_str() , NULL, NULL,
			FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
	}

	//////////////////////////////////////////////////////////////////////////

	AfxEnableControlContainer();
	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	LoadStdProfileSettings(4);  // 加载标准 INI 文件选项(包括 MRU)

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// 注册应用程序的文档模板。文档模板
	// 将用作文档、框架窗口和视图之间的连接
	CMultiDocTemplate* pDocTemplate;
	/*pDocTemplate = new CMvDocTemplate(IDR_StockTYPE,
		RUNTIME_CLASS(CStockDoc),
		RUNTIME_CLASS(CChildFrame), 
		RUNTIME_CLASS(CStockView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);*/

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,  /*IDR_StockTYPE*/
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CIndividualYahooPeView/*CIndividualShareView*/));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	//	2016-4-2 以下两个 DocTemplate 在调试程序时，可以打开	

	//pDocTemplate = new CMvDocTemplate(IDR_CsvTYPE,
	//	RUNTIME_CLASS(CCsvDocument),
	//	RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
	//	RUNTIME_CLASS(CCsvView));
	//if (!pDocTemplate)
	//	return FALSE;
	//AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,  /*IDR_StockTYPE*/
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CNewmat11TestView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_DatabaseTYPE,
		RUNTIME_CLASS(CDatabaseDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CDatabaseView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_AutodownloadTYPE,
		RUNTIME_CLASS(CIndividualShareDoc /*CAutoDownloadDoc*/),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CAutoDownloadView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CIndividualSummaryView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CIndividualCostView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CIndividualSzsePeView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CIndividualVolumeView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CIndividualNetProfitsView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CIndividualCurrentAssetView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CIndividualNonCurrentAssetView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CIndividualCapitalStructView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CIndividualCsvFileView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CBaseHtmlView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CPlateStockListView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CIndividualCapitalFlowView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// 此处必须注释，否则 以下 CBaseChartCtlView 的子类在 StockInfoView 中创建时
	//	均会失效，因为先查找到了该 DocTemplate
	/*pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CBaseChartCtlView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);*/

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CAllStockListView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CIndiCashFlowSummaryView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CPlateGrossProfitListView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CPlateLiquidityRatioView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CPlateNetProfitsRatioView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CPlateTotalAssetsTurnoverRatioView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CPlateDebtToAssetsRatioView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CPlateDebtToCapitalRatioView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
	
	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CPlateInventoryTurnoverRatioView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CPlateCurrentAssetsTurnoverRatioView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	
	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CPlateFixedAssetsTurnoverRatioView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CPlateFundHoldView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CZombieStockListView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CIndiZombieAnalysisView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CIndiFundHoldValueView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CMainProductQueryView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CPeStockListView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CIndiMacdView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CMacdStockListView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CHexunResearchPaperListView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CPlateTotalAssetsGrowthView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CPlateNetFixedAssestsGrowthView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CPlateMainIncomeGrowthView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CIndiMainIncomeGrowthView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CAllMainIncomeYearGrowthStockListView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CAllMainProfitYearGrowthStockListView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CAllRecentBonusCashStockListView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CAllThisYearBonusListView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CAllMcstStockListView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
	
	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CIndiMcstView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CAllAddIssuanceListView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CIndi10jqkaBonusView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CIndiHexunResearchPapaerView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CPlateCorrectedMarketValueView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CIndiMarketIndexAnalysisView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CPlateCsindexDynamicPeView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CAllCsiDynamicPeStockListView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildSplitterFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CIndiCsindexView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CMarketExchageFundView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CMarketMarginGuaranteeFundView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CPlateDealDetailDailyView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
	
	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CPlateTurnoverRateView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CMarketCffexHs300FutureView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CMarketSohuIndexView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CStockHomePageView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CMarketCsdcInvestorStatView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CStatsGovCnDoc),
		RUNTIME_CLASS(CChildSplitterFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CStatsGovCnChartView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CStatsGovCnDoc),
		RUNTIME_CLASS(CChildSplitterFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CStatsGovCnGridView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CPlateChangeRateSummaryView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CPlateGrossProfitRateGrowthView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildSplitterFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CIndiGrossProfitRateView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CIndiCninfoAnnouncementRoadView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CCninfoAnnouncementListView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildSplitterFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CIndiPeAndValueDailyView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CIndiIncomestatementForecastView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CCninfoAnnouncePDFListView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CShanghaiIndexView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CPlateShanghaiIndexView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CCninfoIncentivePlanListView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CCninfoGetLandListView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CCninfoShareHoldingListView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CCninfoShareHoldingStatisticsView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CCandlesticksJapanListView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CMacdListView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CIndiDmiView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CIndiTechnicalAnalysisRoadView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CBollListView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_IndividualSummary,
		RUNTIME_CLASS(CIndividualShareDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CSimulatedAccountListView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	//////////////////////////////////////////////////////////////////////////
	
	/*{
		CDyncItemEditDlg infd;
		vector<item_info>  vec;
		{
			item_info it =  { CDlgTemplateBuilder::EDIT, "标题 1", 
				WS_VISIBLE|WS_CHILD|WS_BORDER|ES_AUTOHSCROLL|WS_TABSTOP | ES_READONLY};
			vec.insert(vec.end(), it);
		}
		

		{
			item_info it =  { CDlgTemplateBuilder::EDIT, "标题 2", WS_VISIBLE|WS_CHILD|WS_BORDER|ES_AUTOHSCROLL|WS_TABSTOP};
			vec.insert(vec.end(), it);
		}

		{
			item_info it =  { CDlgTemplateBuilder::EDIT, "标题 3", WS_VISIBLE|WS_CHILD|WS_BORDER|ES_AUTOHSCROLL|WS_TABSTOP};
			vec.insert(vec.end(), it);
		}

		
		infd.Init("绘制信息","以下为重要信息:" , vec);
		if(infd.DoModal()==IDOK)
		{
			int tttt = 1;
		}
		return FALSE;
	}*/

	/*{
		CCninfoAnnouncePDF  pdf;
		pdf.announcementId = "1202820236";
		pdf.adjunctUrl = "finalpage/2016-11-09/1202820236.PDF";
		pdf.announcementTitle = "1202820236";

		pdf.SaveAsCsv();

		int k = 1;
	}*/


	/*CCninfoAnnouncement * pAnnouce = new CCninfoAnnouncement;
	pAnnouce->SaveAsCsv();
	pAnnouce->ImportToDatabase();*/

	{
		Vec_DP xa(10);
		Vec_DP ya(10);
		/*xa[0] = 1;
		ya[0] = 1 + 1;

		xa[1] = 2;
		ya[1] = 4 + 1;

		xa[2] = 3;
		ya[2] = 9 + 1;

		xa[3] = 4;
		ya[3] = 16 + 1;*/
		COleDateTime odt;

		ya[0] = 79425600;
		odt.ParseDateTime("2006-12-31");
		xa[0] = odt.m_dt;

		ya[1] = 83774400;
		odt.ParseDateTime("2007-12-31");
		xa[1] = odt.m_dt;

		ya[2] = 107600000;
		odt.ParseDateTime("2008-12-31");
		xa[2] = odt.m_dt;

		ya[3] = 245373000;
		odt.ParseDateTime("2009-12-31");
		xa[3] = odt.m_dt;

		ya[4] = 295362000;
		odt.ParseDateTime("2010-12-31");
		xa[4] = odt.m_dt;

		ya[5] = 308961000;
		odt.ParseDateTime("2011-12-31");
		xa[5] = odt.m_dt;

		ya[6] = 272326000;
		odt.ParseDateTime("2012-12-31");
		xa[6] = odt.m_dt;

		ya[7] = 451768000;
		odt.ParseDateTime("2013-12-31");
		xa[7] = odt.m_dt;

		ya[8] = 795723000;
		odt.ParseDateTime("2014-12-31");
		xa[8] = odt.m_dt;

		ya[9] = 1144580000;
		odt.ParseDateTime("2015-12-31");
		xa[9] = odt.m_dt;

		DP	yp1 = (ya[1] - ya[0]) / (xa[1] - xa[0]);
		DP	yp9 = (ya[9] - ya[8]) / (xa[9] - xa[8]);
		Vec_DP y2(10);
		NR::spline(xa, ya, yp1, yp9 ,y2);

		odt.ParseDateTime("2017-12-31");
		DP x  = odt.m_dt;
		DP y;
		DP dy;
		NR::splint(xa, ya, y2, x, y);
		//NR::polint(xa, ya, x, y, dy);

		int k = 1;
	}
	
			
	//////////////////////////////////////////////////////////////////////////
	// 创建主 MDI 框架窗口
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame( IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;

	
	// 仅当具有后缀时才调用 DragAcceptFiles
	//  在 MDI 应用程序中，这应在设置 m_pMainWnd 之后立即发生


	// 分析标准外壳命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	//	修改命令，避免一启动就建立新文档
	cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;

	// 调度在命令行中指定的命令。如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// 主窗口已初始化，因此显示它并对其进行更新
	pMainFrame->ShowWindow(FALSE /*m_nCmdShow*/);
	// pMainFrame->UpdateWindow();

	//////////////////////////////////////////////////////////////////////////
	bool res = false;
	res = mclInitializeApplication(NULL,0);
	/*if(res)
		res = StockMatlabInitialize();*/

	/*if(res)
		res  = stocktestInitialize();
	if(res)
		res  = StOpenWindowInitialize();
	if(res)
		res  = StCloseWindowInitialize();
	if(res)
		res  = StShanghaiIndexInitialize();
	if(res)
		res  = StShanghaiSohuPlateIndexInitialize();
	if(res)
		res  = StShanghaiStockIndexInitialize();*/
	if(!res)
	{
		AfxMessageBox("Matlab Runtime 初始化错误!");
		return FALSE;
	}


	
	//////////////////////////////////////////////////////////////////////////
	// 这步可以省略
	CMessageString initString("初始化 MessageStringManager");

	//////////////////////////////////////////////////////////////////////////
	//	启动计算任务执行模块

	CCalculateTask::init();

	//////////////////////////////////////////////////////////////////////////
	//	首先启动股票基础资料的下载，然后再读入数据，展开页面

	CDownloadRequest * reqDN = new CDownloadRequest;
	reqDN->m_nDownloadType = ID_DOWNLOAD_SHANGHAI_COMPOSITE_INDEX;
	reqDN->m_hwnd = NULL;
	reqDN->m_nLogType = WM_USER_LOG_1;
	StartWorkerThread(AutoDownloadThreadFunc, reqDN);
	
	((CMainFrame *)AfxGetMainWnd())->m_pWndSplashScreen->SetText("上证综指下载中...");  // 上证综值下载

	
	
	//////////////////////////////////////////////////////////////////////////
	
	
	/*pThread = AfxBeginThread(InitialReadStockInfoThreadFunc, (LPVOID)FALSE);
	((CStockApp *)AfxGetApp())->m_vec_pThread.push_back(pThread);*/

	/*pThread = AfxBeginThread(InitialReadStockInfoThreadFunc, (LPVOID)FALSE);
	((CStockApp *)AfxGetApp())->m_vec_pThread.push_back(pThread);

	pThread = AfxBeginThread(InitialReadStockInfoThreadFunc, (LPVOID)FALSE);
	((CStockApp *)AfxGetApp())->m_vec_pThread.push_back(pThread);*/

	//////////////////////////////////////////////////////////////////////////
	// 启动下载 搜狐 板块分类数据
	CDownloadRequest * req_download = new CDownloadRequest;
	req_download->m_nDownloadType = ID_DOWNLOAD_SOUHU_STOCK_PLATE;
	req_download->m_hwnd = NULL;
	req_download->m_nLogType = WM_USER_LOG_3;
	//pThread = AfxBeginThread(AutoDownloadThreadFunc, req_download);
	//((CStockApp *)AfxGetApp())->m_vec_pThread.push_back(pThread);	

	return TRUE;
}



// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CStockApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CStockApp 自定义加载/保存方法

void CStockApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

void CStockApp::LoadCustomState()
{
}

void CStockApp::SaveCustomState()
{
}



CDocTemplate* CStockApp::GetCurrDocTemplate(CView * pView)
{
	ASSERT_VALID(pView);
	// CDocTemplate* pDocTemp = NULL;
	CMvDocTemplate * pDocTemp = NULL;
	

	// Get the first document template position and iterate through the document template
	POSITION posDocTemplate = GetFirstDocTemplatePosition();
	while (posDocTemplate != NULL)
	{
		pDocTemp = (CMvDocTemplate *)GetNextDocTemplate(posDocTemplate);
		ASSERT_VALID(pDocTemp);

		CRuntimeClass * pClassView = pDocTemp->GetCurrView();		
		if(pView->IsKindOf(pClassView))	
		{
			return pDocTemp;
		}	
	}
	return NULL;
}


// CStockApp 消息处理程序




int CStockApp::ExitInstance()
{
	// TODO: 在此添加专用代码和/或调用基类
	
	//	设置信号量，停止所有线程
	 SetEvent(m_bAppExit);

	 friso_uninit();

	 //	Matlab 反初始化
	 /*stocktestTerminate();
	 StOpenWindowTerminate();
	 StCloseWindowTerminate();
	 StShanghaiIndexTerminate();
	 StShanghaiSohuPlateIndexTerminate();
	 StShanghaiStockIndexTerminate();*/
	 StockMatlabTerminate();
	 mclTerminateApplication();

	 HANDLE hThreads[64];
	 int nThreadNum = m_vec_pThread.size() <64? m_vec_pThread.size() : 64;
	 for(int i=0;i< nThreadNum; i++)
	 {
		 hThreads[i] = m_vec_pThread[i]->m_hThread;
	 }
	 //	等待所有后台工作线程停止
	// WaitForMultipleObjects(nThreadNum, hThreads, TRUE, INFINITE);

	 DestroyRWLock(&g_RWlock);

	TerminateProcess(GetCurrentProcess(), 1);  

	CCalculateTask::unInit();

	
	delete m_pCSouhuStockPlates;
	delete m_pCsindexStockPlates;
	delete m_pCsindexCsrcStockPlates;

	{
		m_mapBalance.clear();
		m_mapIncomeStatement.clear();
		m_mapCashFlows.clear();
	}
	
	return CWinAppEx::ExitInstance();


}

BOOL CStockApp::TestRequestExit()
{
	if(WaitForSingleObject(((CStockApp *)AfxGetApp())->m_bAppExit, 0) == WAIT_TIMEOUT)
		return FALSE;
	else
		return TRUE;
}

CWinThread * CStockApp::StartWorkerThread(AFX_THREADPROC pfnThreadProc, LPVOID pParam)
{
	CWinThread * pThread = AfxBeginThread(pfnThreadProc, pParam, \
		THREAD_PRIORITY_BELOW_NORMAL /*THREAD_PRIORITY_NORMAL*/, 1 * 1024 * 1024, CREATE_SUSPENDED);
	pThread->m_bAutoDelete = FALSE;
	m_vec_pThread.push_back(pThread);
	pThread->ResumeThread();
	
	return pThread;
}

BOOL CStockApp::OnIdle(LONG lCount)
{
	// TODO: 在此添加专用代码和/或调用基类

	// 检测已经开始的工作线程是否结束
	bool bContinue = true;
	/*while(bContinue && lCount > 10)
	{
		bContinue = false;
		vector<CWinThread *>::iterator it = m_vec_pThread.begin();
		for(; it!= m_vec_pThread.end(); it++)
		{
			CWinThread * pThread = *it;
			DWORD nExitCode;
			::GetExitCodeThread(pThread->m_hThread, &nExitCode);
			if(nExitCode!=STILL_ACTIVE)
			{
				m_vec_pThread.erase(it);
				delete pThread;
				bContinue = true;
				break;
			}
		}
	}*/

	return CWinAppEx::OnIdle(lCount);
}
