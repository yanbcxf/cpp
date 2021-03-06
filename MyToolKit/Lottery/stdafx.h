
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

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

//	正则表达试
#define PCRE2_CODE_UNIT_WIDTH 8
#define PCRE2_STATIC

#include "pcre2.h"

#include "./MessageString/MessageString.h"

#include "mxml.h"

// xml 解析
#include "expat.h"
#include "XmlParser.h"

// excel 导入
#include "libxl.h"

using namespace libxl;

//	
#include "BtnST.h"

// 颜色
#include "Color.h"

#include "Log4cplusInc.hpp"
#include <afxcontrolbars.h>

#include "LotteryDataModel.h"

#define WM_USER_XML		WM_USER + 1
#define WM_USER_LOG_1					WM_USER + 2
#define WM_USER_LOG_2					WM_USER + 3
#define WM_USER_LOG_3					WM_USER + 4


#define MYSQL_POOL_SIZE		2
#define MYSQL_RECORD_NUM_PER_PAGE	500

#define NUM_PER_LINE	6

typedef struct _item_info
{
	DWORD	nType;
	TCHAR	caption[64];
	DWORD	dwStyle;	//	附加的样式
	bool	bKey;		//	edit 是否关键域（必填）, radio 是否为本主的第一个按钮

	
	TCHAR	strValue[128];

	CString	strItem;	//	edit ,combobox 使用
	int		intItem;	//	radio , checkbox, icon (存放 icon 标识) 使用
	CString	strData;	//	combobox 的下拉获选数据，以分号间隔
	HICON	hicon;		//	存放 icon 句柄
	int		nID;
	int		nID1;

	int		nMin;		//  用于 edit 的最大最小值校验
	int		nMax;		
	double	dbMin;
	double	dbMax;

	_item_info()
	{
		nMin = nMax = 0;
		dbMin = dbMax = 0.0;
		dwStyle = 0;
		intItem = 0;
	}
}item_info;

#include "DyncItemEditDlg.h"
#include "DyncItemFindDlg.h"

//	计算任务执行模块
#include "CalculateTask.h"

#include "BaseMessageControl.h"

#include <afxwin.h>
#include <afxwin.h>

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

typedef struct _LotteryInfo {
	string code;
	string name;
	int		totalnum;
	int		regularnum;
	int		specialnum;

	_LotteryInfo()
	{
		code = "";
		name = "";
		totalnum = 0;
		regularnum = 0;
		specialnum = 0;
	}
}LotteryInfo;


typedef struct _LotteryNumber {
	string code;
	string name;
	int	periods;	//	期数
	int	number[10];	//	正选号码

	int	special_number;	//	特别号

	int sum;
	int	odd;
	int even;
	string equal_tail;
	
	//	连号分析
	int		Consecutive0[5][10];	//	间 0 
	int		Consecutive1[5][10];	//	间 1
	int		Consecutive2[5][10];	//	间 2

	//  相邻号码间隔分析
	int	adjacent_interval[10];

	//  区间内号码个数分析
	int	region_count[10];
	
	//	统计特征
	double var;
	double stdev;
	double devsq;
	double kurt;
	double skew;

	int		ac;
	int		group_qty;	//	集团数
	int		fringe_no_qty;	//	边缘数

	double  fred;		

	int	special_first;		//	是否为有特别号的 号码的第一个组合

	_LotteryNumber()
	{
		code = "";
		name = "";

		periods = 0;
		for (int i = 0; i < 10; i++) number[i] = 0;
		
		special_number = 0;
		sum = 0;
		odd = 0;
		even = 0;
		equal_tail = "";

		var = 0.0;
		stdev = 0.0;
		devsq = 0.0;
		kurt = 0.0;
		skew = 0.0;

		ac = 0;
		group_qty = 0;
		fringe_no_qty = 0;
		fred = 0;

		for (int i = 0; i < 10; i++) adjacent_interval[i] = 0;

		for (int i = 0; i < 10; i++) region_count[i] = 0;

		special_first = 0;
	}
} LotteryNumber;

typedef struct _Assignment{
	int	total_periods;			//	参与分析的期数
	int periods_different;		//	期差
	int	goal_count;				//	累计球数
	int periods_max_internal;	//	最大的间隔期数
	int	periods_recent_number;	//	最近出现的期数序号

	double dResult;				//	暂存各种赋值分析的结果
}Assignment;

#include "basegridctlview.h"

extern Logger  g_logger;
extern connection_pool g_MysqlPool;
extern JMutex  g_mtx_igraph;

extern string Gbk2Utf8(string strGBK);
extern string Utf8_GBK(string strUtf8);
extern BOOL sendToOutput(LPCTSTR lpch, HWND hwnd, int logType = WM_USER_XML);
extern string trimString(string & str);
extern string String2Currency(string val);
extern string ReplaceString(string src, string target_string, string replace_string);
extern string FindLastString(string src, string target_string, int & nLastPosition);
extern string ReplaceChar(string src, char target_char, string replce_char);
extern double String2Double(string str);
extern string Double2String(double db, string strFormat = "");
extern string Int2String(int num, string strFormat = "");
extern BOOL ModifyControlStyle(CWnd* pWnd, LPCTSTR lpszClassName, DWORD dwRemove, DWORD dwAdd, DWORD dwRemoveEx, DWORD dwAddEx);
extern void splitString(string s, string dim, vector<string>& resultVector);
extern string consecutive_number_to_string(int(&cons)[5][10]);
extern LotteryNumber  CalcCharacter(int regular_num, int total_num, LotteryNumber lotto, 
	vector<int> region_start, vector<int> region_end, vector<double> vecFred, bool bContainSpecial = false);
extern int MbcsToUnicode(const char * lpMbcsStr, wchar_t * lpUnicodeStr, int nUnicodeStrLen);

extern int group_number(int * p, int nCount, int nNumPerLine, int isLog = 0);

extern int Pcre2Grep(string patternStr, string subjectStr, vector<string> & vecMatch);

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


