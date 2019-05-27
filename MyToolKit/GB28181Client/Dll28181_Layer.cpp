
#include "stdafx.h"
#include "dll28181_Layer.h"

#include "DllAlarmControl.h"
#include "DllCallBye.h"
#include "DllCallPlayPlaybackDownload.h"
#include "DllCallMediaControl.h"
#include "DllCataloginfoRequest.h"
#include "DllDeviceinfoRequest.h"
#include "DllPtzAutoScan.h"
#include "DllPtzMove.h"
#include "DllPtzPreset.h"
#include "DllPtzTour.h"
#include "DllRecordControl.h"
#include "DllSearchRecordfileRequest.h"
#include "DllStateinfoRequest.h"
#include "DllRegisterResponse.h"
#include "DllAlarmSubscribeRequest.h"

#include "SipdAlarmNotify.h"
#include "SipdCall200ok.h"
#include "SipdCallNoAnswer.h"
#include "SipdCataloginfoResponse.h"
#include "SipdDeviceinfoResponse.h"
#include "SipdMessage200ok.h"
#include "SipdMessageFailure.h"
#include "SipdRegisterRequest.h"
#include "SipdSearchRecordfileResponse.h"
#include "SipdStateinfoResponse.h"
#include "SipdCallFileToEnd.h"
#include "SipdKeepaliveRequest.h"
#include "SipdSubscribe200ok.h"

//UsageEnvironment g_UsageEnvironment;

Dll_Register_Request_func g_Dll_Register_Request_func;
Dll_Unregister_Request_func g_Dll_Unregister_Request_func;
Dll_ptz_response_func g_Dll_ptz_response_func;
Dll_cataloginfo_response_func g_Dll_cataloginfo_response_func;
Dll_deviceinfo_response_func g_Dll_deviceinfo_response_func;
Dll_stateinfo_response_func g_Dll_stateinfo_response_func;
Dll_search_recordfile_response_func g_Dll_search_recordfile_response_func;
Dll_Alarm_Request_func g_Dll_Alarm_Request_func;
Dll_Subscribe_response_func g_Dll_Subscribe_response_func;

Dll_Call_Response_func g_Dll_Call_Response_func;


void Register_Dll_Register_Request(Dll_Register_Request_func pfunc)
{
	g_Dll_Register_Request_func = pfunc;
}

void Register_Dll_Unregister_Request(Dll_Unregister_Request_func pfunc)
{
	g_Dll_Unregister_Request_func = pfunc;
}

int Dll_Register_Responce(int commandport, int isOk)
{
	// 调用注册接口 ，允许或者禁止
	CDllRegisterResponse * pObj = new CDllRegisterResponse(commandport, isOk);
	return pObj->Call(UsageEnvironment::getInstance());
}


void Register_Dll_Alarm_Request(Dll_Alarm_Request_func pfunc)
{
	g_Dll_Alarm_Request_func = pfunc;
}

int Dll_PTZ_Move(const sipd_ptz_control* pControl)
{
	CDllPtzMove * pObj = new CDllPtzMove(pControl);
	return pObj->Call(UsageEnvironment::getInstance());
}

int Dll_PTZ_Tour(const sipd_tour_control * pTourControl)
{
	CDllPtzTour * pObj = new CDllPtzTour(pTourControl);
	return pObj->Call(UsageEnvironment::getInstance());
}

int Dll_PTZ_Preset(const sipd_preset_control * pPresetControl)
{
	CDllPtzPreset * pObj = new CDllPtzPreset(pPresetControl);
	return pObj->Call(UsageEnvironment::getInstance());
}

int Dll_PTZ_AutoScan(const sipd_autoscan_control * pAutoScanControl)
{
	CDllPtzAutoScan * pObj = new CDllPtzAutoScan(pAutoScanControl);
	return pObj->Call(UsageEnvironment::getInstance());
}

int Dll_Reboot(const sipd_id_info * pIdInfo)
{
	return 0;
}

int Dll_record_control(const sipd_record_control * pRecordControl)
{
	CDllRecordControl * pObj = new CDllRecordControl(pRecordControl);
	return pObj->Call(UsageEnvironment::getInstance());
}

int Dll_alarm_control (const sipd_alarm_control * pAlarmControl)
{
	CDllAlarmControl * pObj = new CDllAlarmControl(pAlarmControl);
	return pObj->Call(UsageEnvironment::getInstance());
}

int Register_Dll_ptz_response(Dll_ptz_response_func pfunc)
{
	g_Dll_ptz_response_func = pfunc;
	return 0;
}

/*报警事件订阅请求*/
int Dll_alarm_subscribe_request(const sipd_alarm_req * pStateInfoReq)
{
	CDllAlarmSubscribeRequest * pObj = new CDllAlarmSubscribeRequest(pStateInfoReq);
	return pObj->Call(UsageEnvironment::getInstance());
}

int Register_Dll_Subscribe_response(Dll_Subscribe_response_func pfunc)
{
	g_Dll_Subscribe_response_func = pfunc;
	return 0;
}

/*目录查询请求*/
int Dll_cataloginfo_request(const sipd_catalog_req * pCatalogReq)
{
	CDllCataloginfoRequest * pObj = new CDllCataloginfoRequest(pCatalogReq);
	return pObj->Call(UsageEnvironment::getInstance());
}

int Register_Dll_cataloginfo_response(Dll_cataloginfo_response_func pfunc)
{
	g_Dll_cataloginfo_response_func = pfunc;
	return 0;
}


/*信息查询请求*/
int Dll_deviceinfo_request(const sipd_deviceinfo_req * pDeviceinfoReq)
{
	CDllDeviceinfoRequest * pObj = new CDllDeviceinfoRequest(pDeviceinfoReq);
	return pObj->Call(UsageEnvironment::getInstance());
}

int Register_Dll_deviceinfo_response(Dll_deviceinfo_response_func pfunc)
{
	g_Dll_deviceinfo_response_func = pfunc;
	return 0;
}


/*状态查询请求*/
int Dll_stateinfo_request(const sipd_devicestate_req * pStateInfoReq)
{
	CDllStateinfoRequest * pObj = new CDllStateinfoRequest(pStateInfoReq);
	return pObj->Call(UsageEnvironment::getInstance());
}

int Register_Dll_stateinfo_response(Dll_stateinfo_response_func pfunc)
{
	g_Dll_stateinfo_response_func = pfunc;
	return 0;
}

/*录像文件查询请求*/
int Dll_search_recordfile_request (sipd_recordinfo_req * pRecordFileReq)
{
	CDllSearchRecordfileRequest * pObj = new CDllSearchRecordfileRequest(pRecordFileReq);
	return pObj->Call(UsageEnvironment::getInstance());
}

int Register_Dll_search_recordfile_response(Dll_search_recordfile_response_func pfunc)
{
	g_Dll_search_recordfile_response_func = pfunc;
	return 0;
}

//////////////////////////////////////////////////////////////////////////

int Sipd_Register_Request_Callback(sipd_register_req * pRegisterReq)
{
	CSipdRegisterRequest * pObj = new CSipdRegisterRequest(pRegisterReq);
	return pObj->Call(UsageEnvironment::getInstance());
}


int Sipd_Keepalive_Request_Callback(int tid, sipd_keepalive_notify * pKeepaliveReq)
{
	CSipdKeepaliveRequest * pObj = new CSipdKeepaliveRequest(tid, pKeepaliveReq);
	return pObj->Call(UsageEnvironment::getInstance());
}


int Sipd_Message_200ok_Callback(int tid)
{
	CSipdMessage200ok * pObj = new CSipdMessage200ok(tid);
	return pObj->Call(UsageEnvironment::getInstance());
	return 0;
}

int Sipd_Message_Failure_Callback(int tid)
{
	CSipdMessageFailure * pObj = new CSipdMessageFailure(tid);
	return pObj->Call(UsageEnvironment::getInstance());
}

int Sipd_cataloginfo_response_Callback(sipd_catalog_rsp_basic_info* pCataBasicInfo, sipd_catalog_item* pCataItem, int tid)
{
	CSipdCataloginfoResponse * pObj = new CSipdCataloginfoResponse(pCataBasicInfo, pCataItem, tid);
	return pObj->Call(UsageEnvironment::getInstance());
}


int Sipd_deviceinfo_response_Callback(sipd_deviceinfo_rsp* pDeviceInfo, int tid)
{
	CSipdDeviceinfoResponse * pObj = new CSipdDeviceinfoResponse(pDeviceInfo, tid);
	return pObj->Call(UsageEnvironment::getInstance());
}


int Sipd_stateinfo_response_Callback(sipd_devicestate_rsp_basic_info* pStateBasicInfo, sipd_alarm_status_item* pStateItem, int tid)
{
	CSipdStateinfoResponse * pObj = new CSipdStateinfoResponse(pStateBasicInfo, pStateItem, tid);
	return pObj->Call(UsageEnvironment::getInstance());
}


int Sipd_search_recordfile_response_Callback(sipd_recordinfo_rsp_basic_info* pStateBasicInfo, sipd_file_item* pFileItem, int tid)
{
	CSipdSearchRecordfileResponse * pObj = new CSipdSearchRecordfileResponse(pStateBasicInfo, pFileItem, tid);
	return pObj->Call(UsageEnvironment::getInstance());
}


int Sipd_Alarm_Notify_Callback(sipd_alarm_notify * pAlarmNotify)
{
	CSipdAlarmNotify * pObj = new CSipdAlarmNotify(pAlarmNotify);
	return pObj->Call(UsageEnvironment::getInstance());
}


int Sipd_Subscribe_200_OK_Callback(sipd_subscribe_200_ok_info * pInfo,int sid)
{
	CSipdSubscribe200ok * pObj = new CSipdSubscribe200ok(pInfo, sid);
	return pObj->Call(UsageEnvironment::getInstance());
	return 0;
}

//////////////////////////////////////////////////////////////////////////

int Register_Dll_Call_Response(Dll_Call_Response_func pfunc)
{
	g_Dll_Call_Response_func = pfunc;
	return 0;
}


int Sipd_Call_200_OK_Callback(int cid, int did, int tid, sipd_media_session* pSdp)
{
	CSipdCall200ok * pObj = new CSipdCall200ok(cid, did, tid, pSdp);
	return pObj->Call(UsageEnvironment::getInstance());
}

int Sipd_Call_File_to_end_Callback(int cid, int did, int tid, sipd_media_session* pSdp)
{
	CSipdCallFileToEnd * pObj = new CSipdCallFileToEnd(cid);
	return pObj->Call(UsageEnvironment::getInstance());
}

int Sipd_Call_No_Answer_Callback(int cid, int did, int tid, sipd_media_session* pSdp)
{
	CSipdCallNoAnswer * pObj = new CSipdCallNoAnswer(cid);
	return pObj->Call(UsageEnvironment::getInstance());
}

int Dll_Call_Bye(int playport)
{
	CDllCallBye * pObj = new CDllCallBye(playport);
	return pObj->Call(UsageEnvironment::getInstance());
}


int Dll_Call_play_playback_download( sipd_media_session* pMediaInfo)
{
	CDllCallPlayPlaybackDownload * pObj = new CDllCallPlayPlaybackDownload(pMediaInfo);
	return pObj->Call(UsageEnvironment::getInstance());
}

int Dll_Call_media_control(int playport, enum SIPD_TO_CMS_CMD_TYPE cmd_type, 
						   sipd_playback_mans_rtsp_ind* playback_rtsp_control)
{

	CDllCallMediaControl * pObj = new CDllCallMediaControl(playport, cmd_type, playback_rtsp_control);
	return pObj->Call(UsageEnvironment::getInstance());
}


//////////////////////////////////////////////////////////////////////////


void DLL_Init(const SIPD_GATEWAY_CFG* pCfg)
{
	
	g_Dll_Register_Request_func = NULL;
	g_Dll_Unregister_Request_func = NULL;
	
	g_Dll_ptz_response_func = NULL;
	g_Dll_cataloginfo_response_func = NULL;
	g_Dll_deviceinfo_response_func = NULL;
	g_Dll_stateinfo_response_func = NULL;
	g_Dll_search_recordfile_response_func = NULL;
	g_Dll_Alarm_Request_func = NULL;
	g_Dll_Call_Response_func = NULL;

	g_Dll_Subscribe_response_func = NULL;

	Register_Sipd_Register_Request(Sipd_Register_Request_Callback);
	Register_Sipd_Keepalive_Notify(Sipd_Keepalive_Request_Callback);
	Register_Sipd_Message_200_Ok(Sipd_Message_200ok_Callback);
	Register_Sipd_Message_Failure(Sipd_Message_Failure_Callback);

	Register_Sipd_cataloginfo_response(Sipd_cataloginfo_response_Callback);
	Register_Sipd_deviceinfo_response(Sipd_deviceinfo_response_Callback);
	Register_Sipd_stateinfo_response(Sipd_stateinfo_response_Callback);
	Register_Sipd_search_recordfile_response(Sipd_search_recordfile_response_Callback);
	Register_Sipd_Alarm_Notify(Sipd_Alarm_Notify_Callback);
	Register_Sipd_subscribe_200_OK(Sipd_Subscribe_200_OK_Callback);

	Register_Sipd_Call_200_OK(Sipd_Call_200_OK_Callback);
	Register_Sipd_Call_File_to_end(Sipd_Call_File_to_end_Callback);
	Register_Sipd_Call_No_Answer(Sipd_Call_No_Answer_Callback);
	//Register_Media_Data_Callback();

	//	初始化log4cplus
	PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));	
	

	avcodec_register_all();
	av_register_all();

	//	启动处理线程
	UsageEnvironment::getInstance().m_cfg = *pCfg;

	Sleep(300);
	//	启动 Sip 协议栈
	sipd_init(pCfg);

}


void Dll_PS_Parser(string inputfile, string outputfile)
{
	
}