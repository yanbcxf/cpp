#ifndef __GB_28181_LAYER_H_84892883498903__
#define __GB_28181_LAYER_H_84892883498903__

#include "Dll28181_Layer.h"

#define MAX_SIPD_ALARM_CONDITION				(8)
#define MAX_SIPD_ALARM_DESCRIPTION_LEN		(128)
#define PTZ_CMD_LEN						(8*2)
#define MAX_SIPD_EVENT_LEN				(32)

#define MAX_SIPD_FILE_ITEM				(1000)


#define MAX_SIPD_RECORD_INFO_LEN		(6000)	//(1200)
#define MAX_SIPD_FILEITEM_LEN			(512)


struct sipd_device_control
{
	int sn;

	char SN[MAX_SIPD_DEVICE_ID_LEN+1];/*命令序列号*/
	char  from_uri[MAX_SIPD_URI];
	char  to_uri[MAX_SIPD_URI];

	char  device_id[MAX_SIPD_DEVICE_ID_LEN+1]; 
	char ptz_cmd[PTZ_CMD_LEN+1];
	int ControlPriority;
	int TeleBoot;
	char recordType[MAX_SIPD_NORMAL_LEN+1];
	char guardType[MAX_SIPD_NORMAL_LEN+1];
	char alarm[MAX_SIPD_NORMAL_LEN+1];
};


struct sipd_PTZCMD
{
	int result;
	unsigned char firstByteA5;
	unsigned char combineCode1;
	unsigned char addr;
	unsigned char instruction;
	unsigned char data1;
	unsigned char data2;
	unsigned char combineCode2;
	unsigned char combineCode2_data3;
	unsigned char combineCode2_addr2;
	unsigned char check_code;
};


struct sipd_invite_media_session
{
	/*int cid;
	int tid;
	int did;*/
	enum SIPD_STREAM_TYPE stream_type;
	char connect_video_ip[MAX_SIPD_URI];
	char connect_video_port[MAX_SIPD_NORMAL_LEN];
	char connect_video_addrtype[MAX_SIPD_NORMAL_LEN]; /*reserved*/
	char connect_audio_ip[MAX_SIPD_URI];
	char connect_audio_port[MAX_SIPD_NORMAL_LEN];
	char connect_audio_addrtype[MAX_SIPD_NORMAL_LEN]; /*reserved*/
	enum SIPD_MEDIA_TYPE media_type;
	char o_field_username[MAX_SIPD_NORMAL_LEN];
	char u_field[MAX_SIPD_URI];
	char t_start_time[MAX_SIPD_NORMAL_LEN];
	char t_end_time[MAX_SIPD_NORMAL_LEN];
	char pri_ssrc_field[MAX_SIPD_NORMAL_LEN];
	char pri_ssrc_audio_field[MAX_SIPD_NORMAL_LEN];
	char pri_f_field[MAX_SIPD_URI];
	int f_enable;
	enum SIPD_MEDIA_F_VIDEO_TYPE f_video_type;
	enum SIPD_MEDIA_F_VIDEO_RESOLUTION f_resolution;
	int f_framerate;
	enum SIPD_MEDIA_F_VIDEO_RATE_TYPE f_rate_type;
	int bitrate;
	enum SIPD_MEDIA_F_AUDIO_TYPE audio_type;
	enum SIPD_MEDIA_F_AUDIO_BITRATE audio_bitrate;
	enum SIPD_MEDIA_F_AUDIO_SAMPLE_RATE audio_samplerate;	
};




typedef struct sipd_message_query_or_response
{
	char from_uri[MAX_SIPD_URI];	/*发起者URI*/
	char to_uri[MAX_SIPD_URI];		/*接收者URI*/
	char to_username[MAX_SIPD_NORMAL_LEN];
	char to_host[MAX_SIPD_NORMAL_LEN];
	char body[MAX_SIPD_BODY_LENGTH /*MAX_SIPD_INFO_LENGTH*/];
	char subject[MAX_SIPD_SUBJECT_LEN];
	int	 tid;
	int	 did;
	int	 cid;	//	call id

} sipd_message_query_or_response;


//////////////////////////////////////////////////////////////////////////

extern struct eXosip_t *	context_eXosip;
extern Logger	gb_Logger;

//////////////////////////////////////////////////////////////////////////

int sipd_init(const SIPD_GATEWAY_CFG* pCfg);

typedef int (* Sipd_Register_Request_func)(sipd_register_req * pRegisterReq);
void Register_Sipd_Register_Request(Sipd_Register_Request_func pfunc);
void Sip_Register_Responce(int tid, int isOk);	/*是否允许注册*/

/* 心跳 */
typedef int (* Sipd_Keepalive_Notify_func)(int tid, sipd_keepalive_notify * pRegisterReq);
void Register_Sipd_Keepalive_Notify(Sipd_Keepalive_Notify_func pfunc);

typedef int (* Sipd_Message_Response_func)(int tid);	/*回调类型定义*/

void Register_Sipd_Message_200_Ok(Sipd_Message_Response_func pfunc);
void Register_Sipd_Message_Failure(Sipd_Message_Response_func pfunc);

int Sip_Message_Answer(int tid , bool isOk);

int Sip_settime(const sipd_id_info * pDeviceID,  const SIPD_TIME* pTime);	/*返回值为tid*/
int Sip_PTZ_Move(const sipd_ptz_control* pControl);/*返回值为tid*/
int Sip_PTZ_Tour(const sipd_tour_control * pTourControl);/*返回值为tid*/
int Sip_PTZ_Preset(const sipd_preset_control * pPresetControl);/*返回值为tid*/
int Sip_PTZ_AutoScan(const sipd_autoscan_control * pAutoScanControl);/*返回值为tid*/
int Sip_Reboot(const sipd_id_info * pIdInfo);/*返回值为tid*/
int Sip_record_control(const sipd_record_control * pRecordControl);/*返回值为tid*/
int Sip_alarm_control (const sipd_alarm_control * pAlarmControl);/*返回值为tid*/

int Sip_cataloginfo_request(const sipd_catalog_req * pCatalogReq);/*返回值为tid*/
int Sip_deviceinfo_request(const sipd_deviceinfo_req * pDeviceinfoReq);/*返回值为tid*/
int Sip_stateinfo_request(const sipd_devicestate_req * pStateInfoReq);/*返回值为tid*/

int Sip_search_recordfile_request (sipd_recordinfo_req * pRecordFileReq);/*返回值为tid*/

int Sip_Alarm_Subscribe(const sipd_alarm_req * pAlarmReq);	/*返回值为sid*/


typedef int (* Sipd_cataloginfo_response_func)(sipd_catalog_rsp_basic_info* pCataBasicInfo, sipd_catalog_item* pCataItem, int tid);
void Register_Sipd_cataloginfo_response(Sipd_cataloginfo_response_func pfunc);

typedef int (* Sipd_deviceinfo_response_func)(sipd_deviceinfo_rsp* pDeviceInfo, int tid);
void Register_Sipd_deviceinfo_response(Sipd_deviceinfo_response_func pfunc);

typedef int (* Sipd_stateinfo_response_func)(sipd_devicestate_rsp_basic_info* pStateBasicInfo, sipd_alarm_status_item* pStateItem, int tid);
void Register_Sipd_stateinfo_response(Sipd_stateinfo_response_func pfunc);

typedef int (* Sipd_search_recordfile_response_func)(sipd_recordinfo_rsp_basic_info* pStateBasicInfo, sipd_file_item* pFileItem, int tid);
void Register_Sipd_search_recordfile_response(Sipd_search_recordfile_response_func pfunc);

/*报警上报 */
typedef int (* Sipd_Alarm_Notify_func)(sipd_alarm_notify * pRegisterReq);
void Register_Sipd_Alarm_Notify(Sipd_Alarm_Notify_func pfunc);

typedef int (* Sipd_subscribe_200_ok_func)(sipd_subscribe_200_ok_info* pSubscribeInfo, int sid);
void Register_Sipd_subscribe_200_OK(Sipd_subscribe_200_ok_func pfunc);

//////////////////////////////////////////////////////////////////////////
/*实时流、回放流、下载流的接口*/

typedef int (* Sipd_Call_Response_func)(int cid, int did, int tid, sipd_media_session* pSdp);	/*回调类型定义*/

void Register_Sipd_Call_200_OK(Sipd_Call_Response_func pfunc);		/*注册回调函数， 当收到 200 OK 回复时回调*/
void Register_Sipd_Call_File_to_end(Sipd_Call_Response_func pfunc); /*注册回调函数, 当收到 Message ( 如 File_to_End)消息时回调*/
void Register_Sipd_Call_No_Answer(Sipd_Call_Response_func pfunc);	/*注册回调函数, 当超时无应答时回调*/

 /*启动实时流、回放流、下载流*/
int Sip_Call_play_playback_download(const sipd_media_session* pMediaInfo, int isWithSDP); /*返回值为cid*/
int Sip_Call_Ack(int did, int tid, sipd_media_session* pSdp);					/*需填写 did 和 tid*/

 /*仅文件回放流使用*/
int Sip_Call_media_control(int did, int tid, const enum SIPD_TO_CMS_CMD_TYPE cmd_type, 
									 const sipd_playback_mans_rtsp_ind* playback_rtsp_control);

int Sip_Call_Bye(int cid, int did);										/*需填写 did 和 tid*/
int Sip_Call_200_OK(int did, int tid, sipd_media_session* pSdp);		/*需填写 did 和 tid*/



#endif