
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

#include "sipd_def.h"

#include "StandardInc.hpp"

#include "mxml.h"

#pragma warning(disable : 4996)

#include "CatalogTree.h"
#include "Log4cplusInc.hpp"
#include "UsageEnvironment.h"

typedef bool Boolean;

#define False false
#define True true

#ifdef WIN32
#define snprintf _snprintf
#endif

extern	string	g_strLocalIP;
extern	string	g_strLocalDomainID;

class sipd_sdp_info
{
public:
	int did; /*该实时/回放/下载连接的会话标识*/
	int tid; /*对应连接的sip事务id*/
	int cid;	/* call id */

	/*媒体发送者设备ID编号，如IPC或DVR设备编号*/
	string  media_sender_or_receiver_id;

	/*媒体流发送者IP地址类型*/
	enum SIPD_IPType media_send_or_receive_ip_type;

	/*媒体流发送者IP地址*/
	string media_send_or_receive_ip;

	/*媒体流发送者RTP端口号*/
	int  media_send_or_receive_port;

	string ssrc;
};

extern char* strDup(char const* str) ;
extern Logger  getDynamicLogger(string strLoggerName);
extern int Pcre2Grep(string patternStr,string subjectStr, vector<string> & vecMatch);

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


