// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#pragma warning(disable:4996)

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <windows.h>



// TODO: 在此处引用程序需要的其他头文件



#include "StandardInc.hpp"
#include "Log4cplusInc.hpp"

typedef bool Boolean;

#define False false
#define True true

#ifdef WIN32
#define snprintf _snprintf
#endif

#ifdef _MSC_VER
#define strcasecmp _stricmp
#define strncasecmp  strnicmp 
#endif

#define UINT64_C unsigned __int64

#include <eXosip.h>
#include <osip_headers.h>
#include <osip_mt.h>

#include "Dll28181_Layer.h"
#include "Gb28181_Layer.h"


#include "mxml.h"



extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavfilter/avfilter.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
}


extern "C"
{
#include "libavutil/avstring.h"
	//#include "libavutil/colorspace.h"
#include "libavutil/eval.h"
#include "libavutil/mathematics.h"
#include "libavutil/pixdesc.h"
#include "libavutil/imgutils.h"
#include "libavutil/dict.h"
#include "libavutil/parseutils.h"
#include "libavutil/samplefmt.h"
#include "libavutil/avassert.h"
#include "libavutil/time.h"
#include "libavformat/avformat.h"
#include "libavdevice/avdevice.h"
#include "libswscale/swscale.h"
#include "libavutil/opt.h"
#include "libavcodec/avfft.h"
#include "libswresample/swresample.h"
}

#define  INTTYPES_DEFINED

#include "rtpsession.h"
#include "rtpsessionparams.h"
#include "rtpudpv4transmitter.h"
#include "rtpipv4address.h"
#include "rtptimeutilities.h"
#include "rtppacket.h"

using namespace jrtplib;

#include "jmutex.h"
#include "jmutexautolock.h"

#include "AbstractCommand.h"
#include "CommandCallback.h"
#include "AbstractObject.h"
#include "ActivePsParser.h"
#include "UsageEnvironment.h"

#include "DllRegisterSession.h"
#include "DllDeviceControlSession.h"
#include "DllCataloginfoSession.h"
#include "DllDeviceinfoSession.h"
#include "DllStateinfoSession.h"
#include "DllSearchRecordfileSession.h"
#include "DllAlarmSubscribeSession.h"

#include "DllMediaSession.h"

#include "SipdCataloginfoSession.h"

#include "CallbackAlarmNotify.h"
#include "CallbackDllCallResponse.h"
#include "CallbackDllCataloginfoResponse.h"
#include "CallbackDllDeviceinfoResponse.h"
#include "CallbackDllPtzResponse.h"
#include "CallbackDllRegisterRequest.h"
#include "CallbackDllSearchRecordfileResponse.h"
#include "CallbackDllStateinfoResponse.h"
#include "CallbackDllUnregisterRequest.h"
#include "CallbackDllAlarmSubscribeResponse.h"

#define MAX_XML_BUFFER_LEN			(65535)


struct H264_PARAM
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

};

#define UDP_MEDIA_START_PORT 40000

const int H264_PARAM_LEN=sizeof(H264_PARAM);	//window =72，0x48

extern string Gbk2Utf8(string strGBK);
extern string Utf8_GBK(string strUtf8);

extern Dll_Register_Request_func g_Dll_Register_Request_func;
extern Dll_Unregister_Request_func g_Dll_Unregister_Request_func;
extern Dll_ptz_response_func g_Dll_ptz_response_func;
extern Dll_cataloginfo_response_func g_Dll_cataloginfo_response_func;
extern Dll_deviceinfo_response_func g_Dll_deviceinfo_response_func;
extern Dll_stateinfo_response_func g_Dll_stateinfo_response_func;
extern Dll_search_recordfile_response_func g_Dll_search_recordfile_response_func;
extern Dll_Alarm_Request_func g_Dll_Alarm_Request_func;
extern Dll_Subscribe_response_func g_Dll_Subscribe_response_func;

extern Dll_Call_Response_func g_Dll_Call_Response_func;

extern char* strDup(char const* str) ;
extern int gettimeofday(struct timeval* tp, int* /*tz*/);

//extern Logger  getDynamicLogger(string strLoggerName);