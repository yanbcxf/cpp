#ifndef __SIPD_CMS_H_84892883498903__
#define __SIPD_CMS_H_84892883498903__

/*@jidayao 2014-1-9:Adapter Layer for SIP server*/

#define MAX_SIPD_DEVICE_ID_LEN					(20)
#define MAX_SIPD_NORMAL_LEN			(64)
#define MAX_SIPD_URI					(128)
#define MAX_SIPD_TIME_LEN				(32)
#define MAX_SIPD_LONGITUDE_LEN			(32)
#define MAX_SIPD_LATITUDE_LEN			(32)
#define MAX_SIPD_MANUFACTURER_NAME_LEN		(64)
#define MAX_SIPD_MODEL_NAME_LEN				(32)
#define MAX_SIPD_FIREWARE_LEN					(32)


#define MAX_SIPD_ITEM_RSP				(10)
#define MAX_SIPD_ALARM_ITEM_RSP		(30)



/*网关配置参数*/
typedef struct SIPD_GATEWAY_CFG
{
	char local_domain_name[MAX_SIPD_DEVICE_ID_LEN+1];/*本域SIP服务器域名,如3501040000*/
	char local_domain_id[MAX_SIPD_DEVICE_ID_LEN+1];/*本域SIP服务器编号，如35010400002000000001*/
	int local_port;/*本域SIP服务器端口号*/
	char local_ip[MAX_SIPD_NORMAL_LEN+1];/*本域SIP服务器IP*/
	char local_gateway[MAX_SIPD_NORMAL_LEN+1];
	char upper_server_name[MAX_SIPD_DEVICE_ID_LEN+1];/*上级SIP服务器域名*/
	char upper_server_id[MAX_SIPD_DEVICE_ID_LEN+1];/*上级SIP服务器编号*/
	char upper_server_ip[MAX_SIPD_NORMAL_LEN+1];/*上级SIP服务器IP地址*/
	int upper_server_port;/*上级SIP服务器端口号*/
	char register_upper_server_passwd[MAX_SIPD_NORMAL_LEN+1];/*本域系统往上级服务器注册的注册密码*/
	int register_keepalive;/*本域系统向上级服务器的注册间隔,默认3600秒*/
	int keepalive_internal; /*心跳间隔,默认为60*/
} SIPD_GATEWAY_CFG;

/*配置变化通知类型
SIPD_CONFIG_NOTIFY_CB_IP 表示IP地址变化；
SIPD_CONFIG_NOTIFY_CB_CFG 表示信令网关配置参数变化*/
enum SIPD_CONFIG_NOTIFY_CB_TYPE{ SIPD_CONFIG_NOTIFY_CB_IP = 0, SIPD_CONFIG_NOTIFY_CB_CFG = 1};

/*本域服务器（即本系统）IP地址变化通知信息*/
typedef struct sipd_ip_config_ind
{
	char old_ip[MAX_SIPD_NORMAL_LEN+1];/*改变前IP地址信息*/
       char new_ip[MAX_SIPD_NORMAL_LEN+1];/*改变后IP地址信息*/
} sipd_ip_config_ind;

/*网关配置参数变化通知*/
typedef struct sipd_servercfg_config_ind
{
	SIPD_GATEWAY_CFG old_cfg; /*改变前的配置信息*/
      SIPD_GATEWAY_CFG  new_cfg; /*改变后的配置信息*/
} sipd_servercfg_config_ind;

/*报警类型*/
enum SIPD_ALARM_TYPE {SIPD_ALARM_IO = 1, SIPD_ALARM_VIDEO = 2, SIPD_ALARM_OTHER = 3};
/*报警级别*/
enum sipd_alarm_level {sipd_alarm_levle_all = 0, sipd_alarm_levle_one = 1,sipd_alarm_levle_two = 2, sipd_alarm_levle_three = 3, sipd_alarm_levle_four = 4};


/*报警通知*/
typedef struct sipd_alarm_ind
{
	char  device_id[MAX_SIPD_DEVICE_ID_LEN+1]; /*发生报警事件的IPC国标编号ID*/
	char  alarm_id[MAX_SIPD_DEVICE_ID_LEN+1];/*发生报警事件的IPC报警输入国标编号ID*/
	enum SIPD_ALARM_TYPE alarm_type;/*报警类型*/
    enum sipd_alarm_level alarm_priority;/*报警优先级*/
    char  alarm_time[30];/*报警时间字符串，格式为YYYY-MM-DDThh:mm:ss，如2009-12-04T13:05:09*/

	int did; /*会话标识*/
	int tid; /*对应连接的sip事务id*/
} sipd_alarm_ind;

typedef struct
{
    unsigned short Year; 		                /* 年 */
    unsigned short Month; 	                    /* 月 */
    unsigned short Day; 		                /* 日 */
    unsigned short Hour; 	                    /* 时 */
    unsigned short Minute; 	                    /* 分 */
    unsigned short Second; 	                    /* 秒 */
} SIPD_TIME;

/*设备ID信息*/
typedef struct sipd_id_info
{
	char  device_id[MAX_SIPD_DEVICE_ID_LEN+1]; /*设备编号*/
} sipd_id_info;

/*PTZ操作命令码，包括方向键移动、变倍、变焦、光圈。
SIPD_PTZ_MOVE_STOP 表示停止当前PTZ的操作，包括方向键、变倍、变焦、光圈、巡航等*/
enum SIPD_PTZ_MOVE_OPER {SIPD_PTZ_MOVE_UP = 0, SIPD_PTZ_MOVE_DOWN = 1, SIPD_PTZ_MOVE_LEFT = 2, SIPD_PTZ_MOVE_RIGHT = 3, SIPD_PTZ_MOVE_LU = 4, SIPD_PTZ_MOVE_LD = 5, SIPD_PTZ_MOVE_RU = 6, SIPD_PTZ_MOVE_RD = 7, SIPD_PTZ_MOVE_STOP = 8, SIPD_PTZ_ZOOMTELE = 9, SIPD_PTZ_ZOOMWIDE = 10, SIPD_PTZ_FOCUS_NEAR = 11, SIPD_PTZ_FOCUS_FAR = 12, SIPD_PTZ_HALLO_SMALL = 13, SIPD_PTZ_HALLO_LARGE = 14};

/*PTZ控制结构体*/
typedef struct sipd_ptz_control
{
	char  device_id[MAX_SIPD_DEVICE_ID_LEN+1]; /*设备ID编号*/
       enum SIPD_PTZ_MOVE_OPER cmd_type;/*PTZ操作类型*/
	unsigned short speed;/*PTZ操作速度*/
} sipd_ptz_control;

/*巡航操作命令*/
enum SIP_PTZ_TOUR_OPER {SIPD_PTZ_TOUR_ADD = 0, SIPD_PTZ_TOUR_DEL = 1, SIPD_PTZ_TOUR_START = 2, SIPD_PTZ_TOUR_SET_STAYTIME = 3, SIPD_PTZ_TOUR_SET_SPEED = 4, SIPD_PTZ_TOUR_STOP = 5};

/*巡航控制结构体*/
typedef struct sipd_tour_control
{
	char  device_id[MAX_SIPD_DEVICE_ID_LEN+1]; /*设备ID编号*/
       enum SIP_PTZ_TOUR_OPER  tour_cmd;/*巡航命令类型*/
	unsigned char  tour_id;  /*巡航号，从0～255*/
       unsigned char  preset_id; /*预置点号，从1～255*/
       unsigned short  time;/*巡航组巡航停留时间*/
	unsigned short speed;/*巡航组巡航速度*/
} sipd_tour_control;

/*预置位操作类型*/
enum SIPD_PTZ_PRESET_OPER {SIPD_PTZ_PRESET_ADD = 0, SIPD_PTZ_PRESET_DEL = 1, SIPD_PTZ_PRESET_GOTO = 2};

/*预置位控制结构体*/
typedef struct sipd_preset_control
{
	char  device_id[MAX_SIPD_DEVICE_ID_LEN+1]; /*设备编号*/
      enum SIPD_PTZ_PRESET_OPER cmd_type;/*预置位操作类型*/
	unsigned char  preset_id; /*预置位ID，从1开始，1～255*/
}sipd_preset_control;

/*自动扫描命令类型*/
enum SIPD_PTZ_AUTOSCAN_OPER {SIPD_PTZ_AUTOSCAN_START = 0};

/*自动扫描控制结构体*/
typedef struct sipd_autoscan_control
{
	char  device_id[MAX_SIPD_DEVICE_ID_LEN+1]; /*设备编号*/
     enum SIPD_PTZ_AUTOSCAN_OPER cmd_type;/*主动扫描操作类型*/
} sipd_autoscan_control;



/*录像控制命令*/
enum SIPD_RECORD_CONTROL_OPER {SIPD_RECORD_CONTROL_START = 0, SIPD_RECORD_CONTROL_STOP = 1};
/*录像控制结构*/	
typedef struct sipd_record_control
{
       char from_uri[MAX_SIPD_URI];/*发起者URI*/
	char to_uri[MAX_SIPD_URI];/*接收者URI*/
       char SN[MAX_SIPD_DEVICE_ID_LEN+1];/*命令序列号*/
	char  device_id[MAX_SIPD_DEVICE_ID_LEN+1]; /*目标设备编号*/
      enum SIPD_RECORD_CONTROL_OPER cmd_type;
} sipd_record_control;

/*报警布防/撤防命令、报警复位命令
SIPD_ALARM_CONTROL_SET_GUARD ：报警布防
SIPD_ALARM_CONTROL_RESET_GUARD：报警撤防
SIPD_ALARM_CONTROL_RESET_ALARM：报警复位 */
enum SIPD_ALARM_CONTROL_OPER { SIPD_ALARM_CONTROL_SET_GUARD = 0, SIPD_ALARM_CONTROL_RESET_GUARD = 1, SIPD_ALARM_CONTROL_RESET_ALARM = 2};
/*报警布防/撤防、报警复位结构*/
typedef struct sipd_alarm_control
{
       char from_uri[MAX_SIPD_URI];/*发起者URI*/
	char to_uri[MAX_SIPD_URI];/*接收者URI*/
       char SN[MAX_SIPD_DEVICE_ID_LEN+1];/*命令序列号*/
      char  device_id[MAX_SIPD_DEVICE_ID_LEN+1]; /*目标设备编号*/
    enum SIPD_ALARM_CONTROL_OPER cmd_type;/*布防、撤防、复位命令*/
} sipd_alarm_control;

typedef struct sipd_control_rsp
{
       char from_uri[MAX_SIPD_URI];/*发起者URI*/
	char to_uri[MAX_SIPD_URI];/*接收者URI*/
      char SN[MAX_SIPD_DEVICE_ID_LEN+1];/*命令序列号*/
     char  device_id[MAX_SIPD_DEVICE_ID_LEN+1]; /*目标设备编号*/
    int result; /*0:成功， 1失败*/
} sipd_control_rsp;

/*设备目录查询/订阅请求*/
typedef struct sipd_catalog_req
{
	char from_uri[MAX_SIPD_URI];/*发起者URI*/
	char to_uri[MAX_SIPD_URI];/*接收者URI*/
    char SN[MAX_SIPD_DEVICE_ID_LEN+1];/*命令序列号*/
	char device_id[MAX_SIPD_DEVICE_ID_LEN+1]; /*目标设备的设备/安全防范视频监控联网系统/区域编码*/
	char StartTime[MAX_SIPD_TIME_LEN+1]; /*增加设备的起始时间（可选）,空表示不限。格式为YYYY-MM-DDThh:mm:ss，如2009-12-04T13:05:09*/
	char EndTime[MAX_SIPD_TIME_LEN+1]; /*增加设备的终止时间（可选）,空表示到当前时间。格式为YYYY-MM-DDThh:mm:ss，如2009-12-04T13:05:09*/

	int did; /*订阅会话标识*/
	int tid; /*对应订阅的SIP事务id*/
	int	expires;	/*订阅过期时间，单位秒*/
} sipd_catalog_req;

/*报警查询/订阅请求*/
typedef struct sipd_alarm_req
{
	char from_uri[MAX_SIPD_URI];/*发起者URI*/
	char to_uri[MAX_SIPD_URI];/*接收者URI*/
	char SN[MAX_SIPD_DEVICE_ID_LEN+1];/*命令序列号*/
	char device_id[MAX_SIPD_DEVICE_ID_LEN+1]; /*目标设备的设备/安全防范视频监控联网系统/区域编码*/
	char StartTime[MAX_SIPD_TIME_LEN+1]; /*增加设备的起始时间（可选）,空表示不限。格式为YYYY-MM-DDThh:mm:ss，如2009-12-04T13:05:09*/
	char EndTime[MAX_SIPD_TIME_LEN+1]; /*增加设备的终止时间（可选）,空表示到当前时间。格式为YYYY-MM-DDThh:mm:ss，如2009-12-04T13:05:09*/

	int	StartAlarmPriority;
	int	EndAlarmPriority;
	int	AlarmMethod;

	int did; /*订阅会话标识*/
	int tid; /*对应订阅的SIP事务id*/
	int	expires;	/*订阅过期时间，单位秒*/
} sipd_alarm_req;

/*设备预制点查询请求*/
typedef struct sipd_perset_query_req
{
	char from_uri[MAX_SIPD_URI];/*发起者URI*/
	char to_uri[MAX_SIPD_URI];/*接收者URI*/
	char SN[MAX_SIPD_DEVICE_ID_LEN+1];/*命令序列号*/
	char device_id[MAX_SIPD_DEVICE_ID_LEN+1]; /*目标设备的设备/安全防范视频监控联网系统/区域编码*/
	char StartTime[MAX_SIPD_TIME_LEN+1]; /*增加设备的起始时间（可选）,空表示不限。格式为YYYY-MM-DDThh:mm:ss，如2009-12-04T13:05:09*/
	char EndTime[MAX_SIPD_TIME_LEN+1]; /*增加设备的终止时间（可选）,空表示到当前时间。格式为YYYY-MM-DDThh:mm:ss，如2009-12-04T13:05:09*/
} sipd_perset_query_req;

/*设备目录查询/订阅应答基本信息*/
typedef struct sipd_catalog_rsp_basic_info
{	
        char from_uri[MAX_SIPD_URI];/*发起者URI*/
	char to_uri[MAX_SIPD_URI];/*接收者URI*/
      char SN[MAX_SIPD_DEVICE_ID_LEN+1];/*命令序列号*/
      char device_id[MAX_SIPD_DEVICE_ID_LEN+1]; /*目标设备的设备/安全防范视频监控联网系统/区域编码*/
      int SumNum;/*查询结果总数*/
     int DeviceList_Num;/*目录项个数,最大取值为MAX_SIPD_ITEM_RSP*/

	 int did; /*会话标识*/
	 int tid; /*对应连接的sip事务id*/
} sipd_catalog_rsp_basic_info;

/*信令安全模式*/
enum SIPD_SAFETY_WAY_TYPE{ SIPD_SAFETY_WAY_NONE = 0, SIPD_SAFETY_WAY_SMIME = 2, SIPD_SAFETY_WAY_SMIME_DES = 3, SIPD_SAFETY_WAY_DIGEST = 4};
/*设备状态。SIPD_STATUS_ON ：设备在线；SIPD_STATUS_OFF：设备离线*/
enum SIPD_STATUS_TYPE{SIPD_STATUS_ON = 0, SIPD_STATUS_OFF = 1};
/*设备目录查询应答的设备目录项信息*/
typedef struct sipd_catalog_item
{
	char device_id[MAX_SIPD_DEVICE_ID_LEN+1];/*设备/区域/系统编码*/
	char Name[MAX_SIPD_NORMAL_LEN+1];/*设备/区域/系统名称*/
	char Manufacturer[MAX_SIPD_MANUFACTURER_NAME_LEN+1];/*当为设备时，设备厂商*/
	char Model[MAX_SIPD_MODEL_NAME_LEN +1];/*当为设备时，设备型号*/
	char Owner[MAX_SIPD_NORMAL_LEN+1];/*当为设备时，设备归属*/
	char CivilCode[MAX_SIPD_NORMAL_LEN+1];/*行政区域*/
	char Block[MAX_SIPD_NORMAL_LEN+1];/*警区（可选）*/
	char Address[MAX_SIPD_URI+1];/*当为设备时，安装地址*/
	int Parental;/*当为设备时，是否有子设备，1有，0没有*/
	char ParentID[MAX_SIPD_DEVICE_ID_LEN+1];/*父设备/区域/系统ID（可选，有父设备需要填写）*/
	/*信令安全模式（可选）缺省为0.  0：不采用；2：S/MIME签名方式；3：S/MIME加密签名同时采用方式；4：数字摘要方式*/
	int SafteyWay;
	/*注册方式，缺省为1。1：符合SIP 3261标准的认证注册模式；2：基于口令的双向认证注册模式；3：基于数字证书的双向认证注册模式*/
	int RegisterWay;
	char CertNum[MAX_SIPD_URI+1];/*证书序列号（有证书的设备必选）*/
	/*证书有效标识（有证书的设备必选）缺省为0.  0：无效；1：有效*/
	int Certifiable;
	/*无效原因码（有证书但证书无效的设备必选）*/
	int ErrCode;
	/*证书终止有效期（有证书的设备必选）. 格式为YYYY-MM-DDThh:mm:ss，如2009-12-04T13:05:09*/
	char EndTime[MAX_SIPD_TIME_LEN+1];
	int Secrecy;/*保密属性，缺省为0. 0：不涉密；1：涉密*/
	char IPAddress[MAX_SIPD_NORMAL_LEN+1];/*IP地址（可选）*/
	int Port;/*端口号（可选）*/
	char Password[MAX_SIPD_NORMAL_LEN+1];/*设备口令（可选）*/
	enum SIPD_STATUS_TYPE Status; /*设备状态*/
	char Longitude[MAX_SIPD_LONGITUDE_LEN+1];/*经度（可选）*/
	char Latitude[MAX_SIPD_LATITUDE_LEN+1];/*纬度（可选）*/

	/* 状态改变事件 ON ：上线， OFF ： 离线， VLOST ：视频丢失， DEFECT ： 故障， 
		ADD ： 增加， DEL ： 删除， UPDATE ： 更新 */
	char Event[MAX_SIPD_NORMAL_LEN];
} sipd_catalog_item;

/*设备信息查询请求*/
typedef struct sipd_deviceinfo_req
{	
	char from_uri[MAX_SIPD_URI];/*发起者URI*/
	char to_uri[MAX_SIPD_URI];/*接收者URI*/
	char SN[MAX_SIPD_DEVICE_ID_LEN+1];/*命令序列号*/
	char device_id[MAX_SIPD_DEVICE_ID_LEN+1]; /*设备编码*/
}sipd_deviceinfo_req;

enum SIPD_RESULT_TYPE {SIPD_RESULT_OK = 0, SIPD_RESULT_ERROR = 1};
/*设备信息查询应答*/
typedef struct sipd_deviceinfo_rsp
{
	char from_uri[MAX_SIPD_URI];/*发起者URI*/
	char to_uri[MAX_SIPD_URI];/*接收者URI*/
	char SN[MAX_SIPD_DEVICE_ID_LEN+1];/*命令序列号*/
	char  device_id[MAX_SIPD_DEVICE_ID_LEN+1]; /*目标设备/区域/系统的编码*/
	enum SIPD_RESULT_TYPE Result;/*查询结果*/
	char manufacturer[MAX_SIPD_MANUFACTURER_NAME_LEN];/*设备生产商（可选）*/
	char model[MAX_SIPD_MODEL_NAME_LEN];/*设备型号（可选）*/
	char firmware[MAX_SIPD_FIREWARE_LEN];/*设备固件版本（可选）*/
	char DeviceType[MAX_SIPD_NORMAL_LEN]; /*IPC, DVR, DECODER, NVR, etc.*/
	int MaxCamera;/*通道数，如ipc一个通道(可选)*/
	int MaxAlarm;/*报警输入个数*/
}sipd_deviceinfo_rsp;

/*设备状态查询请求*/
typedef struct sipd_devicestate_req
{	
	char from_uri[MAX_SIPD_URI];/*发起者URI*/
	char to_uri[MAX_SIPD_URI];/*接收者URI*/
	char SN[MAX_SIPD_DEVICE_ID_LEN+1];/*命令序列号*/
	char device_id[MAX_SIPD_DEVICE_ID_LEN+1]; /*目标设备的设备/区域、系统编码*/
}sipd_devicestate_req;

enum SIPD_STATUS_ONLINE_TYPE { SIPD_STATUS_ONLINE_TYPE_ONLINE = 0, SIPD_STATUS_ONLINE_TYPE_OFFLINE = 1};
/*设备状态查询应答基本信息*/
typedef struct sipd_devicestate_rsp_basic_info
{	
	char from_uri[MAX_SIPD_URI];/*发起者URI*/
	char to_uri[MAX_SIPD_URI];/*接收者URI*/
	char SN[MAX_SIPD_DEVICE_ID_LEN+1];/*命令序列号*/
	char device_id[MAX_SIPD_DEVICE_ID_LEN+1]; /*目标设备的设备/区域、系统编码*/
	enum SIPD_RESULT_TYPE Result;/*查询结果*/
	enum SIPD_STATUS_ONLINE_TYPE Online;/*是否在线*/
	enum SIPD_RESULT_TYPE Status;/*是否正常工作*/
	char Reason[MAX_SIPD_URI+1];/*不正常工作原因（可选）*/
	enum SIPD_STATUS_TYPE Encode;/*是否编码（可选）*/
	enum SIPD_STATUS_TYPE Record;/*是否录像（可选）*/
	char DeviceTime[MAX_SIPD_TIME_LEN+1];/*设备时间和日期（可选. 格式为YYYY-MM-DDThh:mm:ss，如2009-12-04T13:05:09）*/
	int AlaramDeviceNum;/*报警设备状态列表个数,最大取值为MAX_SIPD_ALARM_ITEM_RSP*/
} sipd_devicestate_rsp_basic_info;
/*报警设备状态（报警设备指的是报警输入设备）*/
enum SIPD_ALARM_STATUS_TYPE{ SIPD_ALARM_STATUS_ONDUTY = 0, SIPD_ALARM_STATUS_OFFDUTY = 1, SIPD_ALARM_STATUS_ALARM = 2};
/*报警设备状态列表项*/
typedef struct sipd_alarm_status_item
{
	char device_id[MAX_SIPD_DEVICE_ID_LEN+1]; /*报警设备编码（报警设备指的是报警输入设备）*/
    	enum SIPD_ALARM_STATUS_TYPE Status;/*报警设备状态*/
} sipd_alarm_status_item;

/*回调返回的参数类型。
SIPD_INFO_RSP_CB_CATALOG：回复目录查询
SIPD_INFO_RSP_CB_DEVICEINFO：回复设备信息查询
SIPD_INFO_RSP_CB_DEVICESTATE：回复设备状态查询
SIPD_INFO_RSP_CB_RECORDFILE：回复录像文件检索查询*/
enum SIPD_INFO_RSP_CB_TYPE{SIPD_INFO_RSP_CB_CATALOG = 1, SIPD_INFO_RSP_CB_DEVICEINFO = 2,  SIPD_INFO_RSP_CB_DEVICESTATE = 3, SIPD_INFO_RSP_CB_RECORDFILE = 4};


/*录像产生类型*/
enum SIPD_RECORD_INFO_TYPE{SIPD_RECORD_INFO_TYPE_ALL = 1, SIPD_RECORD_INFO_TYPE_TIME = 2, SIPD_RECORD_INFO_TYPE_ALARM = 3, SIPD_RECORD_INFO_TYPE_MANUAL = 4};

/*音视频文件检索请求*/
typedef struct sipd_recordinfo_req
{
	char from_uri[MAX_SIPD_URI]; /*发起者URI*/
	char to_uri[MAX_SIPD_URI];/*接收者URI*/
	char SN[MAX_SIPD_DEVICE_ID_LEN+1];/*命令序列号*/
	char device_id[MAX_SIPD_DEVICE_ID_LEN+1]; /*目录设备/安全防范视频监控联网系统/区域*/
	char StartTime[MAX_SIPD_TIME_LEN]; /*录像起始时间（可选）空表示不限. 格式为YYYY-MM-DDThh:mm:ss，如2009-12-04T13:05:09*/
	char EndTime[MAX_SIPD_TIME_LEN]; /*增加录像终止时间（可选）空表示到当前时间. 格式为YYYY-MM-DDThh:mm:ss，如2009-12-04T13:05:09*/
	char FilePath[MAX_SIPD_URI]; /*文件路径名（可选）。注意这里的路径名可以是一个媒体存储服务器的设备编码*/
	char Address[MAX_SIPD_URI]; /*录像地址（可选 ,支持不完全查询）*/
	int Secrecy;/*保密属性（可选）缺省为0.  0:不涉密1:涉密*/
	enum SIPD_RECORD_INFO_TYPE Type;/*录像产生类型（可选）*/
	char RecorderID[MAX_SIPD_DEVICE_ID_LEN+1]; /*录像触发者ID（可选, 如某个中心用户（这里的中心用户也按国标进行编码，如某个客户端）发起的录像）*/
} sipd_recordinfo_req;

/*音视频文件检索请求应答基本信息*/
typedef struct sipd_recordinfo_rsp_basic_info
{
	char from_uri[MAX_SIPD_URI]; /*发起者URI*/
	char to_uri[MAX_SIPD_URI];/*接收者URI*/
	char SN[MAX_SIPD_DEVICE_ID_LEN+1];/*命令序列号*/
	char device_id[MAX_SIPD_DEVICE_ID_LEN+1]; /*设备/区域*/
	char Name[MAX_SIPD_NORMAL_LEN+1]; /*设备/区域名称*/
	int SumNum; /*查询结果总数*/
	int RecordListNum; /*文件目录项个数,最大取值为MAX_SIPD_ITEM_RSP*/
} sipd_recordinfo_rsp_basic_info
;
/*音视频文件检索应答的文件目录项信息*/
typedef struct sipd_file_item
{
	char device_id[MAX_SIPD_DEVICE_ID_LEN+1];/*设备/区域编码*/
	char Name[MAX_SIPD_NORMAL_LEN+1];/*设备/区域名称*/
	char FilePath[MAX_SIPD_URI+1];/*文件路径名（可选）*/
	char Address[MAX_SIPD_URI+1];/*录像地址（可选）*/
	char StartTime[MAX_SIPD_TIME_LEN+1];/*录像开始时间（可选）. 格式为YYYY-MM-DDThh:mm:ss，如2009-12-04T13:05:09*/
	char EndTime[MAX_SIPD_TIME_LEN+1];/*录像结束时间（可选）. 格式为YYYY-MM-DDThh:mm:ss，如2009-12-04T13:05:09*/
	int Secrecy;/*保密属性，缺省为0. 0：不涉密；1：涉密*/
	enum SIPD_RECORD_INFO_TYPE Type;/*录像产生类型（可选）*/
	char RecorderID[MAX_SIPD_DEVICE_ID_LEN+1];/*录像触发者ID（可选）*/
}sipd_file_item;


/*IP地址类型，设计预留，当前实现只支持IPv4*/
enum SIPD_IPType {SIPD_IPType_IPv4 = 0, SIPD_IPType_IPv6 = 1};

/* SIPD_STREAM_TYPE_PLAY 表示实时点播
SIPD_STREAM_TYPE_PLAYBACK 表示历史音视频回放
SIPD_STREAM_TYPE_DOWNLOAD 表示历史音视频下载*/
enum SIPD_STREAM_TYPE{SIPD_STREAM_TYPE_PLAY = 1, SIPD_STREAM_TYPE_PLAYBACK = 2, SIPD_STREAM_TYPE_DOWNLOAD = 3};

/*媒体类型，视频/音频/音视频。当取值为SIPD_MEDIA_TYPE_NONE 时设备根据自己的情况默认发送视频或者视音频，推荐只发送视频即可*/
enum SIPD_MEDIA_TYPE{SIPD_MEDIA_TYPE_NONE = 0, SIPD_MEDIA_TYPE_VIDEO = 1, SIPD_MEDIA_TYPE_AUDIO = 2, SIPD_MEDIA_TYPE_MIX = 3};

/*视频编码格式，当取值为SIPD_MEDIA_F_VIDEO_TYPE_DEF 时设备使用自己的默认视频编码格式，推荐此时使用H264*/
enum SIPD_MEDIA_F_VIDEO_TYPE{SIPD_MEDIA_F_VIDEO_TYPE_DEF = 0, SIPD_MEDIA_F_VIDEO_TYPE_MPEG4 = 1, SIPD_MEDIA_F_VIDEO_TYPE_H264 = 2, SIPD_MEDIA_F_VIDEO_TYPE_SVAC = 3, SIPD_MEDIA_F_VIDEO_TYPE_3GP = 4};

/*视频分辨率，当取值为SIPD_MEDIA_F_VIDEO_RESOLUTION_DEF 时设备使用自己的默认分辨率*/
enum SIPD_MEDIA_F_VIDEO_RESOLUTION{SIPD_MEDIA_F_VIDEO_RESOLUTION_DEF = 0, SIPD_MEDIA_F_VIDEO_RESOLUTION_QCIF = 1, SIPD_MEDIA_F_VIDEO_RESOLUTION_CIF = 2, SIPD_MEDIA_F_VIDEO_RESOLUTION_4CIF = 3, SIPD_MEDIA_F_VIDEO_RESOLUTION_D1 = 4, SIPD_MEDIA_F_VIDEO_RESOLUTION_720P = 5, SIPD_MEDIA_F_VIDEO_RESOLUTION_1080P = 6};

/*视频码率类型，当取值为SIPD_MEDIA_F_VIDEO_RATE_TYPE_DEF 时设备使用自己默认的码率类型*/
enum SIPD_MEDIA_F_VIDEO_RATE_TYPE{SIPD_MEDIA_F_VIDEO_RATE_TYPE_DEF = 0, SIPD_MEDIA_F_VIDEO_RATE_TYPE_CBR = 1, SIPD_MEDIA_F_VIDEO_RATE_TYPE_VBR = 2};


/*音频编码格式，当取值为SIPD_MEDIA_F_ AUDIO _TYPE_DEF 时设备使用自己的默认音频编码格式，推荐此时使用G711*/
enum SIPD_MEDIA_F_AUDIO_TYPE{SIPD_MEDIA_F_AUDIO_TYPE_DEF = 0, SIPD_MEDIA_F_AUDIO_TYPE_G711 = 1, SIPD_MEDIA_F_AUDIO_TYPE_G723 = 2, SIPD_MEDIA_F_AUDIO_TYPE_G729 = 3, SIPD_MEDIA_F_AUDIO_TYPE_G722 = 4};

/*音频码率大小。当为SIPD_MEDIA_F_AUDIO_BITRATE_DEF 时设备使用自己默认的码率大小。
SIPD_MEDIA_F_AUDIO_BITRATE_5P3 表示5.3 kbps
SIPD_MEDIA_F_AUDIO_BITRATE_6P3 表示6.3 kbps 
SIPD_MEDIA_F_AUDIO_BITRATE_8 表示8 kbps 
SIPD_MEDIA_F_AUDIO_BITRATE_16 表示16 kbps 
SIPD_MEDIA_F_AUDIO_BITRATE_24 表示24 kbps 
SIPD_MEDIA_F_AUDIO_BITRATE_32 表示32kbps 
SIPD_MEDIA_F_AUDIO_BITRATE_48 表示48 kbps 
SIPD_MEDIA_F_AUDIO_BITRATE_64 表示64 kbps */
enum SIPD_MEDIA_F_AUDIO_BITRATE{SIPD_MEDIA_F_AUDIO_BITRATE_DEF = 0, SIPD_MEDIA_F_AUDIO_BITRATE_5P3 = 1, SIPD_MEDIA_F_AUDIO_BITRATE_6P3 = 2, SIPD_MEDIA_F_AUDIO_BITRATE_8 = 3, SIPD_MEDIA_F_AUDIO_BITRATE_16 = 4, SIPD_MEDIA_F_AUDIO_BITRATE_24 = 5, SIPD_MEDIA_F_AUDIO_BITRATE_32 = 6, SIPD_MEDIA_F_AUDIO_BITRATE_48 = 7, SIPD_MEDIA_F_AUDIO_BITRATE_64 = 8};

/*音频采样率，当为SIPD_MEDIA_F_AUDIO_SAMPLE_RATE_DEF 时设备使用自己默认的音频采样率。
SIPD_MEDIA_F_AUDIO_SAMPLE_RATE_8表示 8kHz
SIPD_MEDIA_F_AUDIO_SAMPLE_RATE_14表示14kHz
SIPD_MEDIA_F_AUDIO_SAMPLE_RATE_16表示16kHz
SIPD_MEDIA_F_AUDIO_SAMPLE_RATE_32表示32kHz*/
enum SIPD_MEDIA_F_AUDIO_SAMPLE_RATE{SIPD_MEDIA_F_AUDIO_SAMPLE_RATE_DEF = 0, SIPD_MEDIA_F_AUDIO_SAMPLE_RATE_8 = 1, SIPD_MEDIA_F_AUDIO_SAMPLE_RATE_14 = 2, SIPD_MEDIA_F_AUDIO_SAMPLE_RATE_16 = 3, SIPD_MEDIA_F_AUDIO_SAMPLE_RATE_32 = 4};


typedef struct sipd_media_session
{
	int did; /*该实时/回放/下载连接的会话标识*/
	int tid; /*对应连接的SIP事务id*/
	/*对于实时视频点播来说，o_id表示媒体发送设备ID编码，如IPC或DVR/NVR编码。对于回放和下载来说，o_id表示录像文件所有者的编码*/
	char o_id[MAX_SIPD_DEVICE_ID_LEN+1];
	char channel_id[MAX_SIPD_DEVICE_ID_LEN+1];/*点播/回放/下载通道*/
	enum SIPD_RECORD_INFO_TYPE Type;/*回放类型/下载类型*/
	enum SIPD_STREAM_TYPE stream_type; /*操作类型：实时点播或回放或下载*/
    /*媒体流接收者IP地址类型*/
	enum SIPD_IPType media_recv_ip_type;
/*媒体流接收者IP地址*/
	char media_recv_ip[MAX_SIPD_URI];
    /*媒体流接收者RTP端口号*/
	char media_recv_port[MAX_SIPD_NORMAL_LEN];
	enum SIPD_MEDIA_TYPE media_type;/*媒体类型*/
	/*start_time和end_time这两个字段对于stream_type == SIPD_STREAM_TYPE_PLAY 实时点播来说，这两个字段都取值为"0"和"0".
	对于回放SIPD_STREAM_TYPE_PLAYBACK和下载SIPD_STREAM_TYPE_DOWNLOAD来说，这两个字段表示开始和结束时间。时间格式见IETF RFC 4566的5.9，开始时间和结束时间都是要回放或下载的音视频文件录制时间段中的某个时刻。如start_time为"1288625085"  
	,end_time为"1288625871"*/
    	char t_start_time[MAX_SIPD_NORMAL_LEN];
	char t_end_time[MAX_SIPD_NORMAL_LEN];
	/*十进制整数字符串表示SSRC值*/
	char ssrc[MAX_SIPD_NORMAL_LEN];
	/*视频编码格式*/
	enum SIPD_MEDIA_F_VIDEO_TYPE f_video_type;
    /*视频分辨率*/
	enum SIPD_MEDIA_F_VIDEO_RESOLUTION f_resolution;
    /*视频帧率：0～99*/
	int f_framerate;
    /*码率类型*/
	enum SIPD_MEDIA_F_VIDEO_RATE_TYPE f_rate_type;
    /*码率大小*/
	int bitrate;
    /*音频编码格式*/
	enum SIPD_MEDIA_F_AUDIO_TYPE audio_type;
    /*音频码率大小*/
	enum SIPD_MEDIA_F_AUDIO_BITRATE audio_bitrate;
    /*音频采样率*/
	enum SIPD_MEDIA_F_AUDIO_SAMPLE_RATE audio_samplerate;	
} sipd_media_session;

typedef struct sipd_sender_sdp_info
{
	int did; /*该实时/回放/下载连接的会话标识*/
	int tid; /*对应连接的sip事务id*/
    /*媒体发送者设备ID编号，如IPC或DVR设备编号*/
	char media_sender_id[MAX_SIPD_DEVICE_ID_LEN+1];
	/*媒体流发送者IP地址类型*/
	enum SIPD_IPType media_send_ip_type;
	/*媒体流发送者IP地址*/
	char media_send_ip[MAX_SIPD_URI];
    /*媒体流发送者RTP端口号*/
	char media_send_port[MAX_SIPD_NORMAL_LEN];
	char ssrc[MAX_SIPD_NORMAL_LEN];
} sipd_sender_sdp_info;

typedef struct sipd_media_file_end_ind
{
	int did; /*该实时/回放/下载连接的会话标识*/
	int tid;/*对应连接的sip事务id*/
	
	/*对于实时视频点播来说，o_id表示媒体发送设备ID编码，如IPC或DVR/NVR编码。对于回放和下载来说，o_id表示录像文件所有者的编码*/
	char o_id[MAX_SIPD_DEVICE_ID_LEN+1]; /*取自sipd_media_session中的同名字段*/
} sipd_media_file_end_ind;

/*媒体控制回调类型*/
enum SIPD_MEDIA_CONTROL_CB_TYPE{ SIPD_MEDIA_CONTROL_CB_INVITE_RSP = 0, SIPD_MEDIA_CONTROL_CB_FILE_TO_END = 1};

/*订阅回调类型*/
enum SIPD_SUBSCRIBE_CB_TYPE{ SIPD_SUBSCRIBE_CB_CATALOG_RSP = 0, SIPD_SUBSCRIBE_CB_ALARM_RSP = 1};

/*信令网关控制媒体网关的命令类型
SIPD_TO_CMS_CMD_TYPE_START 开始播放
SIPD_TO_CMS_CMD_TYPE_STOP 结束播放
SIPD_TO_CMS_CMD_TYPE _PLAYBACK_CONTROL_RTSP 回放控制*/
enum SIPD_TO_CMS_CMD_TYPE{ SIPD_TO_CMS_CMD_TYPE_START = 1, SIPD_TO_CMS_CMD_TYPE_STOP = 2,  SIPD_TO_CMS_CMD_TYPE_PLAYBACK_CONTROL_RTSP = 3};

#define MAX_SIPD_INFO_LENGTH			(1024)
/*保存回放控制的RTSP报文*/
typedef struct sipd_playback_mans_rtsp_ind
{
	char rtsp_des[MAX_SIPD_INFO_LENGTH+1];/* 回放控制的RTSP报文*/
    int len;/*报文长度*/
} sipd_playback_mans_rtsp_ind;





typedef void(*sip_config_pcb_func)(int* type, void* param1);
typedef void(*sip_alarm_ind_pcb_func)( sipd_alarm_ind * alarm_info);
typedef void(*sip_control_req_pcb_func)( sipd_control_rsp* pControlRsp);
typedef void(*sip_info_req_pcb_func)(int* type, void* param1, void* param2);
typedef void(*sip_media_pcb_func)(int* type, void* param1);
typedef void(*sip_subscribe_pcb_func)(int* type, void* param1);


 //-----------------------------------------------------------------------
/*函数说明:初始化
/*参数:无
/*返回值:0表示成功，<0表示失败*/
//-----------------------------------------------------------------------
extern int Sipd_Init();

//-----------------------------------------------------------------------
/*函数说明:程序开始
/*参数:无
/*返回值:0表示成功，<0表示失败*/
//-----------------------------------------------------------------------
extern int Sipd_Start();

extern int Sipd_GetSIPServerCfg (SIPD_GATEWAY_CFG* pCfg);
extern int Sipd_Register_ServerConfig_CB (sip_config_pcb_func config_fun);
extern int Sipd_Register_AlarmInd_CB (sip_alarm_ind_pcb_func alarm_ind_fun);
extern int Sipd_settime(const sipd_id_info * pDeviceID,  const SIPD_TIME* pTime);
extern int Sipd_PTZ_Move(const sipd_ptz_control* pControl);
extern int Sipd_PTZ_Tour(const sipd_tour_control * pTourControl);
extern int Sipd_PTZ_Preset(const sipd_preset_control * pPresetControl);
extern int Sipd_PTZ_AutoScan(const sipd_autoscan_control * pAutoScanControl);
extern int Sipd_Reboot(const sipd_id_info * pIdInfo);
extern int Sipd_record_control (const sipd_record_control * pRecordControl);
extern int Sipd_alarm_control (const sipd_alarm_control * pAlarmControl);
extern int Sipd_Register_ControlRsp_CB(sip_control_req_pcb_func control_rsp_fun);
extern int Sipd_cataloginfo_request(const sipd_catalog_req * pCatalogReq);
extern int Sipd_deviceinfo_request(const sipd_deviceinfo_req * pDeviceinfoReq);
extern int Sipd_stateinfo_request(const sipd_devicestate_req * pStateInfoReq);
extern int Sipd_Register_InfoRsp_CB(sip_info_req_pcb_func rsp_fun);
extern int Sipd_search_recordfile_req (sipd_recordinfo_req * pRecordFileReq);
extern int Sipd_Register_MediaControl_CB(sip_media_pcb_func media_control_fun);
extern int Invite_play_playback_download(const sipd_media_session* pMediaInfo);
extern int Sipd_media_control_notify_CMS(const int did, const int tid, const enum SIPD_TO_CMS_CMD_TYPE cmd_type, const sipd_playback_mans_rtsp_ind* playback_rtsp_control);


//-----------------------------------------------------------------------
/*函数说明:停止
/*参数:无
/*返回值:0表示成功，其他值表示失败*/
//-----------------------------------------------------------------------

extern int Sipd_Stop();

//-----------------------------------------------------------------------
/*函数说明:反初始化
/*参数:无
/*返回值:0表示成功，其他值表示失败*/
//-----------------------------------------------------------------------

extern int Sipd_Uninit();






#endif

