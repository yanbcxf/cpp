// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once


#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include "StandardInc.hpp"
#include "Log4cplusInc.hpp"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 某些 CString 构造函数将是显式的

#include <atlbase.h>
#include <atlutil.h>				//	包含 CString

#include "jmutexautolock.h"

#include "owsp_def.h"


typedef bool Boolean;
typedef long int64_t;

#define False false
#define True true

enum IOType 
{
	IOInitialize, // The client just connected
	IORead, // Read from the client. 
	IOReadCompleted, // Read completed
	IOWrite, // Write to the Client
	IOWriteCompleted, // Write Completed.
	IOZeroByteRead, // Read zero Byte from client (dummy for avoiding The System Blocking error) 
	IOZeroReadCompleted, // Read Zero Byte  completed. (se IOZeroByteRead)
	IOTransmitFileCompleted, //TransmitFileCompleted.
	IOPostedPackage, // Used to post Packages into IOCP port. 

	IOCommand,		//	对应 AbstractCommand 对象
};

// Determines the size of the first bytes who tells you how big the message are. (pakage heap)  
#define MINIMUMPACKAGESIZE sizeof(UINT)
#define MAXIMUMPACKAGESIZE 65536 /*14600*/ /*512*/
#define MAXIMUMSEQUENSENUMBER 5001

#include "IOCPBuffer.h"
#include "BufferManager.h"

#include "AbstractCommand.h"
#include "DelayQueue.h"
#include "UsageEnvironment.h"
#include "Medium.h"
#include "BaseClient.h"

#include <json/json.h>

// TODO: 在此处引用程序需要的其他头文件

enum {
	RESULT_CODE_SERVER_PRIMARY = 2,
	RESULT_CODE_SERVER_SLAVE = 1,
	RESULT_CODE_OK = 0,
	RESULT_LOGON_ALREADY = -1,				//	用户已登陆
	RESULT_LOGON_DOG_EXPIRED = -2,			//	加密狗过期
	RESULT_LOGON_DOG_CHECK_FAIL = -3,		//	加密狗检测失败
	RESULT_LOGON_DOG_USER_EXCEEDING = -4,		//	用户数量已满

	RESULT_CODE_SERVER_CONNECTION_ERR = -5,	//	连接失败
	RESULT_CODE_LOCAL_PARAMETER_ERR = -6,	//	登录参数错误
	RESULT_CODE_SERVER_NOT_FOUND = -7,		//	主服务器未找到
	RESULT_CODE_SERVER_NOT_LOGON = -8,		//	未登录
	RESULT_CODE_SERVER_UNKOWN = -9,			//	未知错误
	RESULT_CODE_SERVER_ERR = -10,
	RESULT_CODE_SERVER_TIMEOUT = -11,		//	超时
	RESULT_CODE_MEMORY_ERR = -12,
	RESULT_CODE_SERVER_ALREADY_LOGON = -13,	//	已经登录过来
	RESULT_CODE_LOCAL_ERR = -14

};



extern BufferManager g_BufferMgr;
extern UsageEnvironment * g_pUsageEnvironment;

extern string	g_strDevsn;
extern int		g_nChannel;
extern string	g_strServerIP;
extern int		g_nServerPort;

extern Logger	g_Logger;

extern char* strDup(char const* str);
extern Logger  getDynamicLogger(string strLoggerName);
extern int gettimeofday(struct timeval* tp, int* /*tz*/);
extern bool ReadCfg();
extern void splitString(string s, string dim, vector<string>& resultVector);
extern string NowTime();