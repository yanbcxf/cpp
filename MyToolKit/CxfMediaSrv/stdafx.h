// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once


#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#define WIN32_LEAN_AND_MEAN

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include "sipd_def.h"

#include "StandardInc.hpp"

#include "mxml.h"

#pragma warning(disable : 4996)


#include "Log4cplusInc.hpp"
#include "UsageEnvironment.h"

#include "./jthread/jmutexautolock.h"

using namespace jthread;

class sipd_sdp_info
{
public:
	int did; /*该实时/回放/下载连接的会话标识*/
	int tid; /*对应连接的sip事务id*/
	int	cid;

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

typedef bool Boolean;

#define False false
#define True true

#ifdef WIN32
#define snprintf _snprintf
#endif


extern	string	g_strLocalIP;
extern	string	g_strDBAddr;
extern	int		g_iRtpPort;
extern	string	g_strLocalDomainID;

extern	Logger	g_logger;
extern	string	g_strCurrentDir;

extern char* strDup(char const* str) ;
extern bool ReadCfg(string fullpath);
extern Logger  getDynamicLogger(string strLoggerName);
extern int Pcre2Grep(string patternStr,string subjectStr, vector<string> & vecMatch);



// TODO: 在此处引用程序需要的其他头文件
