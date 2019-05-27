
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


#include "StandardInc.hpp"
#include "Log4cplusInc.hpp"

#include "jmutex.h"
#include "jmutexautolock.h"

#include "FfplaySdl2Pure.h"
#include "Dll28181_Layer.h"

#include "ToolPic.h"
#include "Label.h"

typedef unsigned char  uint8_t;
typedef signed __int64   int64_t;
typedef unsigned __int64 uint64_t;

typedef struct H264_PARAM
{
	uint8_t head[8];	//包头8个字符A       8 
	int  version;		//版本				 4	 本号为1的为按帧率播放，版本号为2的为按时间戳播放	
	int64_t idx;		//索引				 8
	int  knd;			//0=H264视频，1=G711.A(1ch,8k,16bit)	 2=unsigned pcm(1ch,8k,8bit)  3=G711 ulaw(1ch,8k,16bit)				
	int  fbs;			//帧率或采样率		 4			
	time_t curtime;		//码流带时间		 8
	int ptsSetup;		//pts步进值			 4	
	int dtsSetup;		//dtsSetup			 4
	u_int64 pts;        //pts信息            8
	int64_t dts;        //dts信息			 8	
	int  nal_len;		//码流带长度		 4	

	H264_PARAM(){
		version=1;
		for(int i=0;i<8;i++) head[i]=0x41;
		pts = -1;
		dts = -1;
		knd =-1;
	}

}H264_PARAM;

//  "172.26.84.182"

#define LOCAL_DOMAIN_ID			"34020000002020000001"
#define LOCAL_IP				"10.18.72.35"
#define	LOCAL_PORT				7100

#define REMOTE_DOMAIN_ID		"340200000020200002"
//#define REMOTE_IP				"10.18.73.120"
//#define REMOTE_IP				"10.18.72.201"
#define REMOTE_IP				"10.18.72.231"
#define	REMOTE_PORT				5062


#define WM_USER_CATALOG_COMM	WM_USER + 1
#define WM_USER_CATALOG_DATA	WM_USER + 2
#define WM_USER_CALL_RESP		WM_USER + 3	

#define WM_USER_RECORDFILE_COMM	WM_USER + 4
#define WM_USER_RECORDFILE_DATA	WM_USER + 5


//////////////////////////////////////////////////////////////////////////
//	来自 playback 或者 client 控件 

#define MAX_WINDOW 4		// 回放视图使用
#define MAX_WNDNUM 25		// 预览视图使用
#define MAX_FILECNT 1024	// 最大支持文件查找的数量.

#define NVRTYPE     1	//远程播放（流媒体或是板卡远程播放的时候可以用）
#define DVRTYPE     2	//DVR远程播放文件
#define LOCALTYPE   3	//本地文件播放
#define BRSTYPE     4	//邦诺远程播放

#define PARA_STEP			26	// 注意目前只能是,海康/25.5 获取参数后+1再设置又是原来值.
#define VOLUME_STEP			1320

//画面分割类型
enum{
	SPLIT1 = 0,
	SPLIT4,
	SPLIT9,
	SPLIT16,
	SPLIT25,	
	SPLIT36,	
	SPLIT49,	
	SPLIT64,	
	SPLIT_TOTAL
};


#define  WM_EVENT_START	  		    WM_USER+100
#define  WM_EVENT_CHANGE_PARA		WM_EVENT_START+1	//修改开始、结束时间,传送包的大小
#define  WM_EVENT_RUN_CMD		    WM_EVENT_START+2	//执行命令
#define  WM_EVENT_SEL_WND		    WM_EVENT_START+3	//鼠标选择窗口
#define  WM_USER_NOTIFY_PLAYSTATUS  WM_EVENT_START + 4	// 通知网页播放状态改变.

struct  DOWNLOADPARA
{
	CTime  BeginTime;   //开始时间
	CTime  EndTime;     //结束时间
	LONG   FileTotleSize;//文件打包的大小
};

// 回放状态.
enum
{
	_STATUS_STOP = 0,
	_STATUS_PLAY,
	_STATUS_PAUSE,
};


/*日期格式*/
struct  LDATETIME
{
	short dwYear;		//年
	BYTE dwMonth;		//月
	BYTE dwDay;			//日
	BYTE dwHour;		//时
	BYTE dwMinute;		//分
	BYTE dwSecond;		//秒
};

//文件信息结构体
struct   RETSEARCH	{
	char  createtime[20];//文件建立时间 16
	char  finishtime[20];//文件结束时间 16
	char  fileName[256];  //文件名称
	long  totalsize; //文件大小
};



BOOL DebugTrace(char * lpszFormat,...);
void CTimeToLDateTime(const CTime &ctTm,LDATETIME &struTm);
CTime StrToTime(char *str);
void convert_to_utc(const char *cSource,char *cDest);

//////////////////////////////////////////////////////////////////////////

extern JMutex			g_mtxNet;
extern map<int, HWND>	g_mapNet2Hwnd;
extern map<int, int>	g_mapNet2Player;
extern Logger  g_Logger;



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


