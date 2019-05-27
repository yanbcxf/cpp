
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展


#include <afxdisp.h>        // MFC 自动化类



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持


#include <afxsock.h>            // MFC 套接字扩展

//#include <atlbase.h>
//
//#include <atlcom.h>
//#include <atlwin.h>
//#include <atlhost.h>


#include <vector>
#include <map>
#include <list>
#include <string>
#include <iostream>
#include <fstream>
using namespace std; 

//	数据库存取
#include "soci.h"
#include "soci-mysql.h"
#include "test/common-tests.h"


#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cassert>
#include <cmath>
#include <ctime>
#include <ciso646>
#include <cstdlib>
#include <mysqld_error.h>
#include <errmsg.h>

#include "jmutex.h"
#include "jmutexautolock.h"

using namespace soci;
using namespace soci::tests;

//  html 解析
#include "html/ParserDom.h"
#include "html/utils.h"
#include "html/wincstring.h"
#include "css/parser_pp.h"
#ifndef WIN32
#include "config.h"
#else
#define VERSION "0.6"
#endif

using namespace htmlcxx;

#include <json/json.h>

//	使用 “孩子兄弟表示法”的树结构
// #include "Tree.h"

//	新板的 Tree 树形结构
#include ".\TreeContainer408\tree.h"

//	正则表达试
#define PCRE2_CODE_UNIT_WIDTH 8
#define PCRE2_STATIC

#include "pcre2.h"



#include "./MessageString/MessageString.h"

//#include "Log4cplusInc.hpp"
#include <log4cplus/logger.h>
#include <log4cplus/consoleappender.h>
#include <log4cplus/configurator.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/loggingmacros.h>
using namespace log4cplus;
using namespace log4cplus::helpers;

//	读写锁
#include "READWRIT.h"

//	DMGraph 控件
#include "CDMGraphAnnotation.h"
#include "CDMGraphAxis.h"
#include "CDMGraphCollection.h"
#include "CDMGraphCursor.h"
#include "CDMGraphElement.h"
#include "CIDMGraphCtrlEvents.h"

#include "CDMGraphCtrl.h"

// xml 解析
#include "expat.h"
#include "XmlParser.h"

// excel 导入
#include "libxl.h"

using namespace libxl;

#include "ComboBoxExt.h"
#include "SplashScreenEx.h"
#include "MultiLineListBox.h"

//	
#include "BtnST.h"


#define WANT_STREAM
#define WANT_MATH
#include "newmatnl.h"
#include "newmatio.h"
#include "newmatap.h"

// 颜色
#include "Color.h"


//////////////////////////////////////////////////////////////////////////
//	股票板块信息
class CStockPlateData
{
public:
	string	code;
	string	name;
	int		company_number;
	double	total_volume;
	double	total_turnover;
	string	parent_code;
	int		is_stock;

	//	以下数据用于 板块的指数计算
	string	tradedate;
	double	circulating_value;
	double  yesterday_circulating_value;
	double	repair_value;
	double  division;
	double	point;

	//	板块的 macd
	int		serial_num;
	double	ema12;
	double	ema26;
	double	dif;
	double	dea;
	string	remark;
	int		ranking;	//	板块排名 

	CStockPlateData()
	{
		tradedate = "";
		circulating_value = 0;
		yesterday_circulating_value = 0;
		repair_value = 0;
		division = 0;
		point = 0;

		company_number = 0;
		total_volume = 0;
		total_turnover = 0;
		parent_code = "00000000";

		serial_num = 0;
		remark = "";
	}

	bool operator == (const CStockPlateData &otherData)
	{
		if(code==otherData.code && is_stock == otherData.is_stock)
			return true;
		else
			return false;
	}

	bool operator <  (const CStockPlateData & otherData) const
	{
		if(is_stock == otherData.is_stock)
		{
			if(code<otherData.code)
				return true;
			else
				return false;
		}
		else if(is_stock < otherData.is_stock)
			return true;
		else
			return false;
	}
};

//////////////////////////////////////////////////////////////////////////

class CStockPlateTree : public tcl::tree<CStockPlateData> 
{
public:
	CStockPlateTree * SearchSpecialNode(CStockPlateData spd)
	{
		list<CStockPlateTree *>  list_level_node;
		CStockPlateTree * pNode = NULL;
		CStockPlateTree::iterator child_it;

		list_level_node.push_back(this);

		//	每颗树都有缺省的 Root 节点，采用层次（level）算法
		while(list_level_node.size()>0)
		{
			pNode = *list_level_node.begin();
			list_level_node.pop_front();

			//	查找它的子节点
			child_it = pNode->begin();
			while(child_it!=pNode->end())
			{
				CStockPlateData spd1 = *child_it;
				if(spd1==spd)
					break;

				// 查找过的节点要保存，以便查找他的子节点
				list_level_node.push_back((CStockPlateTree *)child_it.node());
				child_it++;
			}
			if(child_it!=pNode->end())
				return (CStockPlateTree *)child_it.node();
		}
		return NULL;
	}
};


//////////////////////////////////////////////////////////////////////////

class CPdfCatalog
{
public:
	string	strName;	//	目录名称
	int		nPage;		//	页号
	string	strContent;	//	
};

// 业务报表
#include "CSVFile.h"

#include "StockDataModel.h"
#include "FinanceReport.h"
#include "IncomeStatement.h"
#include "Balance.h"
#include "CashFlows.h"
#include "StockInfo.h"
#include "TradingDaily.h"
#include "CapitalStructure.h"
#include "ShanghaiStockInfo.h"
#include "CapitalStructOf10jqka.h"
#include "ShenzhenMarketQuotation.h"
#include "SouhuTradingDaily.h"
#include "SouhuStockPlates.h"
#include "CapitalFlows163.h"
#include "SohuPerformanceForecast.h"
#include "SohuMainIncomeComposition.h"
#include "SelfSelectStockPlates.h"
#include "SohuFundHold.h"
#include "HexunResearchPaper.h"
#include "CfiBonus.h"
#include "CfiPlacementOfShare.h"
#include "BonusOf10jqka.h"
#include "CfiAddIssuance.h"
#include "SohuIndexDaily.h"
#include "CsiPeDaily.h"
#include "CsiCsrcPeDaily.h"
#include "CsindexStockPlates.h"
#include "FundInSettlement.h"
#include "SzseMarginTrading.h"
#include "SseMarginTrading.h"
#include "SinaDealDetail.h"
#include "ShenzhenIndexYangben.h"
#include "NetEaseTradeDaily.h"
#include "ShenzhenIndexList.h"
#include "ShenzhenIndexDaily.h"

#include "CsdcFundInSettlementStat.h"
#include "CsdcInvestorStat.h"
#include "CsdcSecurityIssueStat.h"
#include "CsdcTransferBonusStat.h"

#include "CffexFutureDaily.h"

#include "CninfoAnnouncePDF.h"
#include "CninfoAnnouncement.h"
#include "CninfoStockInfo.h"
#include "CninfoDelistingList.h"
#include "CninfoSuspendList.h"
#include "CninfoBonus.h"
#include "CninfoPlacementOfShare.h"
#include "CninfoCsv.h"					//	包含财务报表

#include "StatsGovCn.h"
#include "StatsGovHgjd.h"
#include "StatsGovHgyd.h"
#include "StatsGovHgnd.h"
#include "StatsGovFsjd.h"
#include "StatsGovFsyd.h"
#include "StatsGovFsnd.h"

//	计算任务执行模块
#include "CalculateTask.h"

#include "BussinessUtils.h"

#include "BaseCsvFileShow.h"

#include "IndividualShareDoc.h"
#include "StatsGovCnDoc.h"

#include "MySplitterWndEx.h"

#include "BaseMessageControl.h"
#include "basegridctlview.h"

#include "TechnicalAnalysis.h"
#include "StockDataLog.h"

typedef struct _item_info
{
	DWORD	nType;
	TCHAR	caption[64];
	DWORD	dwStyle;
	bool	bKey;		//	是否关键域

	TCHAR	strValue[128];

}item_info;

#include "DyncItemEditDlg.h"
#include "DyncItemFindDlg.h"

#include <afxdisp.h>
#include <afxdisp.h>
#include <afxdisp.h>
#include <afxhtml.h>



#define WM_USER_XML		WM_USER + 1
#define WM_USER_LOG_1					WM_USER + 2
#define WM_USER_LOG_2					WM_USER + 3
#define WM_USER_LOG_3					WM_USER + 4
#define WM_USER_LOG_FinanceReport		WM_USER + 5
#define WM_USER_LOG_CapitalFlow163		WM_USER + 6
#define WM_USER_LOG_Forecast			WM_USER + 7
#define WM_USER_LOG_CapitalStruct		WM_USER + 8
#define WM_USER_LOG_ResearchPaper		WM_USER + 9
#define WM_USER_LOG_Income_Composition	WM_USER + 10
#define WM_USER_LOG_Cfi_Bonus			WM_USER + 11
#define WM_USER_LOG_Cninfo				WM_USER + 12
#define WM_USER_LOG_StatsGovCn			WM_USER + 13
#define WM_USER_LOG_CSDC				WM_USER + 14

#define MYSQL_POOL_SIZE		6
#define MYSQL_RECORD_NUM_PER_PAGE	500

//////////////////////////////////////////////////////////////////////////
//	下载请求的数据结构

class CDownloadRequest
{
public:
	int m_nDownloadType;
	vector<int> m_vecCode;
	vector<string>	m_vec_cninfo_orgid;
	HWND	m_hwnd;
	int		m_nLogType;
	string	m_strResult;
};

// 请求读入的个股
class StockInfo_Reqeust
{
public:
	StockInfo_Reqeust()
	{
		code = -10000;
		hwnd = NULL;
		strAfterDate = "";
		logType = WM_USER_LOG_1;
	};
	HWND hwnd;
	int	code;
	int logType;
	//	为空时，要求读入“每日交易信息”,且读入所有的财务报表
	//	不为空时，要求读入在此日期后的财务报表，不读入每日交易信息
	string strAfterDate;	
} ;




// 

class CStockInfoSummary
{
public:
	int	code;
	string	name;
	string	a_time_to_market;
	double	volume;
	double	turnover;
	double	turnover_rate;
	double	change;
	double	change_rate;

	double	a_total_share;
	double	estimate_profit;
	double  earning_per_share;
	double  pe;
	double	a_total_value;

	double  close;
	double	MainFlowIn;
	double	MainFlowOut;
	double  main_turnover_rate;	//	主力资金换手率
	string	performanceForecast_date;	//	业绩预告最晚日期
	double	performanceForecast_rate;	//	业绩预告幅度

	string	recent_finance_report_date;	//	最晚的财务报表时间

	CStockInfoSummary()
	{
		code = -1000;
		MainFlowIn = -1;
		MainFlowOut = -1;
		main_turnover_rate = -1;
		a_total_share = -1;
		estimate_profit = -1;
		earning_per_share = -1;
		pe = -1;

		name = "-----";
		volume = -1;
		turnover = -1;
		turnover_rate = -1;
		change_rate = -1;
		change = -1;
		close = -1;
		a_total_value = -1;
		performanceForecast_date = "";
		performanceForecast_rate = -1;

	};
	
};





extern string	g_strLocalIP;
extern string	g_strDBAddr;
extern int		g_iDBPort;
extern connection_pool g_MysqlPool;
extern string	g_strCurrentDir;
extern RWLock	g_RWlock;

extern Logger	g_logger;



//extern Logger  getDynamicLogger(string strLoggerName);
extern bool ReadCfg(string fullpath);
extern BOOL sendToOutput(LPCTSTR lpch,  HWND hwnd = NULL, int logType = WM_USER_XML);

extern int gettimeofday(struct timeval*, int*);
extern int API_StringToTime(const string &strDateStr,time_t &timeData);
extern time_t convert_string_to_time_t(string  time_string);
extern double String2Double(string str);
extern string String2Currency(string val);
string Double2Currency(double val, bool isInteger=false);
extern string Double2String(double db, string strFormat="");
extern string Int2String(int num, string strFormat="");
extern string Gbk2Utf8(string strGBK);
extern string Utf8_GBK(string strUtf8);
extern string Utf8_GBK_(char * strUtf8);
extern int Pcre2Grep(string patternStr,string subjectStr, vector<string> & vecMatch);
extern int Pcre2Split(string patternStr,string subjectStr, vector<string> & vecSplit);
extern UINT AutoDownloadThreadFunc(LPVOID param);
extern string ReplaceChar(string src, char target_char, string replce_char);
extern string FindLastString(string src, string target_string, int & nLastPosition);
extern string trimString(string & str );
extern string ReplaceString(string src, string target_string, string replace_string);
extern void splitString(string s, string dim, vector<string>& resultVector);
extern bool splitChinese(string sentence, vector<string> & vecWord);	//	中文分词
extern int RandomFromRange(int nMin, int nMax);

extern string UrlEncode(const string& str);
extern string UrlDecode(const string& str);

extern bool GetInfoFromDLK(string & strSLC, list<string> &listMac, list<string> &listDID);
extern int CheckProcessOnlyOne(const char *name);

extern bool friso_init();
extern void friso_uninit();
extern bool friso_split_chinese(string sentence, vector<string> & vecWord);

extern BOOL ModifyControlStyle(CWnd* pWnd,LPCTSTR lpszClassName,DWORD dwRemove,DWORD dwAdd,DWORD dwRemoveEx,DWORD dwAddEx);

extern void TechnicalAnalysis(int nDownloadType);

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


