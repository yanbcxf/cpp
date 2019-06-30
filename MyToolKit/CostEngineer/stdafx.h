
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

#include <afxole.h>         // MFC OLE 类
#include <afxodlgs.h>       // MFC OLE 对话框类

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
#include <sstream>
using namespace std;

#include "Resource.h"

#include "jmutex.h"
#include "jmutexautolock.h"


//	正则表达试
#define PCRE2_CODE_UNIT_WIDTH 8
#define PCRE2_STATIC

#include "pcre2.h"

#include "./MessageString/MessageString.h"

#include "mxml.h"

// xml 解析
#include "expat.h"
#include "XmlParser.h"

#include "BtnST.h"

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

#include "BalloonTip.h"

#include "DyncItemEditDlg.h"
#include "DyncItemFindDlg.h"
#include "DyncItemGroupDlg.h"
#include "TreeGridDlg.h"

//	计算任务执行模块
#include "CalculateTask.h"

#include "BaseMessageControl.h"
#include "BaseGridCtlView.h"
#include "BaseChartCtlView.h"
#include "BaseMessageFormView.h"

#include "ConsumptionQuota.h"
#include "ColumnObj.h"
#include "BeamObj.h"
#include "SimilarEngineerBudget.h"
#include "BudgetIndex.h"
#include "BuildingBudget.h"
#include "ItemOfUnitPrice.h"
#include "ItemOfTotalPrice.h"
#include "DecisionTree.h"
#include "ActivityOnArrow.h"


#define WM_USER_XML		WM_USER + 1

typedef struct _CostEngineerInfo {
	string code;
	string name;
	vector<struct _CostEngineerInfo> list;

	_CostEngineerInfo()
	{
		code = "";
		name = "";
	}
}CostEngineerInfo;



extern string Gbk2Utf8(string strGBK);
extern string Utf8_GBK(string strUtf8);
extern BOOL sendToOutput(LPCTSTR lpch, HWND hwnd, int logType = WM_USER_XML);
extern double String2Double(string str);
extern string Double2String(double db, string strFormat = "");
extern string Int2String(int num, string strFormat = "");
extern string String2Currency(string val);
extern BOOL ModifyControlStyle(CWnd* pWnd, LPCTSTR lpszClassName, DWORD dwRemove, DWORD dwAdd, DWORD dwRemoveEx, DWORD dwAddEx);
extern double String2Double(string str);
extern void splitString(string s, string dim, vector<string>& resultVector);
extern int MbcsToUnicode(const char * lpMbcsStr, wchar_t * lpUnicodeStr, int nUnicodeStrLen);
extern int Pcre2Split(string patternStr, string subjectStr, vector<string> & vecSplit);
extern int Pcre2Grep(string patternStr, string subjectStr, vector<string> & vecMatch);
extern bool parseSteelMarking(string marking, int* quantity =NULL, int* diameter = NULL, double* weight = NULL, int* first = NULL, int* second = NULL);

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


