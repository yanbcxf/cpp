
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


#include <vector>
#include <map>
#include <list>
#include <string>
#include <iostream>
#include <fstream>
#include <stack>
using namespace std; 

#include "jmutex.h"
#include "jmutexautolock.h"

#include <iconv.h>

//	数据库存取
#include "soci.h"
#include "soci-mysql.h"
#include "test/common-tests.h"

using namespace soci;
using namespace soci::tests;

// xml 解析
#include "expat.h"
#include "XmlParser.h"


//	正则表达试
#define PCRE2_CODE_UNIT_WIDTH 8
#define PCRE2_STATIC

#include "pcre2.h"

#include "MessageString.h"
#include "MyDialog.h"

#include "podofo\podofo.h"
#include <cstdio>

using namespace PoDoFo;


//#include "friso_API.h"
#include "friso.h"



class CPdfCatalog
{
public:
	string	strName;	//	目录名称
	int		nPage;		//	页号
	string	strContent;	//	
};



class CLineOrCell{
public:
	double	x;
	double	y;
	double	font_size;
	string	font_name;
	double	trender;

	PdfRect rect;
	string	content;

	int		parent;			//	父结点
	double	max_right_x;	//	如果是段落，则保存最大的右边 x 坐标
	double	last_right_x;	//	如果是段落，则保存最末行右边 x 坐标
	double	last_x;			//	如果是段落，则保存最末行左边 x 坐标

	CLineOrCell()
	{
		rect.SetBottom(0);
		rect.SetLeft(0);
		rect.SetWidth(0);
		rect.SetHeight(0);

		parent = -1;	
		trender = 0.0;
	}


	bool operator == (const CLineOrCell &otherData)
	{
		return false;
	}

	bool operator <  (const CLineOrCell & otherData) const
	{
		return false;
	}
};


#define MYSQL_POOL_SIZE		1
#define WM_USER_LOG_1		WM_USER + 1



extern connection_pool g_MysqlPool;


extern friso_t		g_friso;
extern friso_config_t g_config;

//////////////////////////////////////////////////////////////////////////

extern BOOL sendToOutput(LPCTSTR lpch,  HWND hwnd = NULL, int logType = WM_USER_LOG_1);

extern int gettimeofday(struct timeval*, int*);

extern double String2Double(string str);
extern string String2Currency(string val);
string Double2Currency(double val, bool isInteger=false);
extern string Double2String(double db, string strFormat="");
extern string Int2String(int num, string strFormat="");
extern string Gbk2Utf8(string strGBK);
extern string Utf8_GBK(string strUtf8);
extern string Utf8_GBK_(char * strUtf8);
extern int Pcre2Grep(string patternStr,string subjectStr, vector<string> & vecMatch, vector<int> & vecOffset);
extern int Pcre2Split(string patternStr,string subjectStr, vector<string> & vecSplit);
extern bool  Pcre2Find(string strSrc, string strPattern);

extern UINT AutoDownloadThreadFunc(LPVOID param);
extern string ReplaceChar(string src, char target_char, string replce_char);
extern string FindLastString(string src, string target_string, int & nLastPosition);
extern string trimString(string  str );
extern string ReplaceString(string src, string target_string, string replace_string);
extern void splitString(string s, string dim, vector<string>& resultVector);
extern int RandomFromRange(int nMin, int nMax);
extern int CheckProcessOnlyOne(const char *name);

extern int UTF8ToGBK(unsigned char * lpUTF8Str,unsigned char * lpGBKStr,int nGBKStrLen);
extern int UnicodeToGBK(wchar_t * lpUnicodeStr, unsigned char * lpGBKStr,int nGBKStrLen);

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


