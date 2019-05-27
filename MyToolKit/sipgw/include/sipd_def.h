#ifndef __SIPD_DEF_739028938992__
#define __SIPD_DEF_739028938992__

#include "sipd_cms_layer.h"


#define MAX_SIPD_ALARM_CONDITION				(8)
#define MAX_SIPD_ALARM_DESCRIPTION_LEN		(128)
#define PTZ_CMD_LEN						(8*2)
#define MAX_SIPD_EVENT_LEN				(32)

#define MAX_SIPD_FILE_ITEM				(1000)


#define MAX_SIPD_RECORD_INFO_LEN		(6000)	//(1200)
#define MAX_SIPD_FILEITEM_LEN			(512)



#define SIPD_LOCAL_SDP_S_FIELD_IPC	("IP Camera")
#define SIPD_LOCAL_SDP_S_FIELD_DVR	("Embeded Net DVR")
#define SIPD_LOCAL_SDP_S_FIELD_NVR	("Embeded Net NVR")

#define SIPD_CAMERA_NAME				("IPCamera1")

#define SIPD_LOCAL_SDP_S_FIELD			SIPD_LOCAL_SDP_S_FIELD_IPC

#define SIPD_SUBSCRIBE_EVENT_PRESENT						("presence")
#define SIPD_SUBSCRIBE_EVENT_CONFERENCE					("conference")
#define SIPD_SUBSCRIBE_EVENT_DIALOG						("dialog")
#define SIPD_SUBSCRIBE_EVENT_MESSAGE_SUMMARY			("message-summary")
#define SIPD_SUBSCRIBE_EVENT_REFER						("refer")
#define SIPD_SUBSCRIBE_EVENT_REG							("reg")
#define SIPD_SUBSCRIBE_EVENT_WINFO						("winfo")

#define SIPD_SUBSCRIBE_IO_ALARM_DESCRIPTION_DEF			("IO Alarm")
#define SIPD_SUBSCRIBE_MOTO_ALARM_DESCRIPTION_DEF			("Motion Detection Alarm")

typedef struct
{
    unsigned short Year; 		                /* 年 */
    unsigned short Month; 	                    /* 月 */
    unsigned short Day; 		                /* 日 */
    unsigned short Hour; 	                    /* 时 */
    unsigned short Minute; 	                    /* 分 */
    unsigned short Second; 	                    /* 秒 */
}  PRM_TIME;

/*注册SIP校时请求结构体*/
typedef struct Sipd_timing_req
{
	char  device_id[MAX_SIPD_DEVICE_ID_LEN+1]; 
	PRM_TIME tm;
}Sipd_timing_req;


struct sipd_ptz_req
{
	char  device_id[MAX_SIPD_DEVICE_ID_LEN+1]; 
	unsigned short speed;
};

struct sipd_ptz_autoscan_req
{
	char  device_id[MAX_SIPD_DEVICE_ID_LEN+1]; 
	unsigned short  speed;
	int scan_id;
};

struct sipd_cruiseroute_req
{
	char  device_id[MAX_SIPD_DEVICE_ID_LEN+1]; 
	unsigned char  cruise_id;  
       unsigned char  preset_id; 
       unsigned short  time;
	unsigned short speed;
};


struct sipd_preset_req
{
	char  device_id[MAX_SIPD_DEVICE_ID_LEN+1]; 
	unsigned char  preset_id; 
};

struct sipd_reboot_req
{
	char  device_id[MAX_SIPD_DEVICE_ID_LEN+1]; 
};


struct sipd_record_req
{
	char  device_id[MAX_SIPD_DEVICE_ID_LEN+1]; 
	char   from[MAX_SIPD_URI+1];
	char   to[MAX_SIPD_URI+1];
	char cseq[MAX_SIPD_URI+1];
	unsigned int sn;
	int record_onOff;
};


struct sipd_alram_guard_req
{
	char  device_id[MAX_SIPD_DEVICE_ID_LEN+1]; 
	char   from[MAX_SIPD_URI+1];
	char   to[MAX_SIPD_URI+1];
	char cseq[MAX_SIPD_URI+1];
	unsigned int sn;
	int onOff; /*布防或撤防*/
};



struct sipd_alram_cmd_req
{
	char  device_id[MAX_SIPD_DEVICE_ID_LEN+1]; 
	char   from[MAX_SIPD_URI+1];
	char   to[MAX_SIPD_URI+1];
	char cseq[MAX_SIPD_URI+1];
	unsigned int sn;
	int reset; /*复位*/
};


enum sipd_alarm_condition {sipd_alarm_condition_all = 0, sipd_alarm_condition_phone = 1, sipd_alarm_condition_device = 2, sipd_alarm_condition_sms = 3, sipd_alarm_condition_GPS = 4, sipd_alarm_condition_video = 5, sipd_alarm_condition_devicefault = 6, sipd_alarm_condition_others = 7};
enum SIPD_SUBSCRIBE_STATE {SIPD_SUBSCRIBE_STATE_UNKNOWN = 0, SIPD_SUBSCRIBE_STATE_PENDING = 1, SIPD_SUBSCRIBE_STATE_ACTIVE = 2, SIPD_SUBSCRIBE_STATE_TERMINATED = 3};
enum SIPD_SUBSCRIBE_TYPE	{SIPD_SUBSCRIBE_TYPE_EVENT = 0, SIPD_SUBSCRIBE_TYPE_CATALOG = 1};





struct sipd_device_control
{
	int sn;
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
	int tid;
	int did;
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





struct sipd_recorditem
{
	char DeviceID[MAX_SIPD_DEVICE_ID_LEN+1]; 
	char StartTime[MAX_SIPD_TIME_LEN];
	char EndTime[MAX_SIPD_TIME_LEN]; 
	enum SIPD_RECORD_INFO_TYPE Type;
};


typedef struct sipd_catainfo_adaptor_rsp
{
	sipd_catalog_rsp_basic_info basic_info;
	sipd_catalog_item cata_items[MAX_SIPD_ITEM_RSP];
}sipd_catainfo_adaptor_rsp;

typedef struct sipd_stateinfo_adaptor_rsp
{
	sipd_devicestate_rsp_basic_info basic_info;
	sipd_alarm_status_item state_items[MAX_SIPD_ALARM_ITEM_RSP];
}sipd_stateinfo_adaptor_rsp;

typedef struct sipd_recordinfo_adaptor_rsp
{
	sipd_recordinfo_rsp_basic_info basic_info;
	sipd_file_item file_items[MAX_SIPD_ITEM_RSP];
}sipd_recordinfo_adaptor_rsp;

struct sipd_recorditem_pair
{
	int is_last_one_item; /*only one item? last one*/
	char item_pair_buffer[2][MAX_SIPD_FILEITEM_LEN+1];
};

struct sipd_playback_mans_rtsp_ctrl_ind
{
	char rtsp_des[MAX_SIPD_INFO_LENGTH+1];
	int tid;
	int did;
	int len;
};

enum SIPD_INVITE_CTRL_TYPE{SIPD_INVITE_CTRL_TYPE_START = 1, SIPD_INVITE_CTRL_TYPE_STOP = 2};

struct sipd_invite_ctrl_ind
{
	int tid;
	int did;
	enum SIPD_INVITE_CTRL_TYPE cmd_type;
};

#endif

