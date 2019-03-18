// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers


#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif


#include <stdio.h>
#include <tchar.h>



#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 某些 CString 构造函数将是显式的

#include <atlbase.h>
#include <atlutil.h>				//	包含 CString

#include "StandardInc.hpp"
#include "Log4cplusInc.hpp"

#include "../FfplaySdl2Pure/media_def.h"


#include "ProtocolUtils.h"

extern BufferManager g_BufferMgr;
extern UsageEnvironment * g_pUsageEnvironment;

extern string	g_strDevsn;
extern int		g_nChannel;
extern string	g_strServerIP;
extern int		g_nServerPort;

// TODO: reference additional headers your program requires here

extern int SearchTag2(unsigned char * pBuffer, int begin, int dwBufSize, int & next);
extern char* strDup(char const* str);
extern int gettimeofday(struct timeval* tp, int* /*tz*/);
