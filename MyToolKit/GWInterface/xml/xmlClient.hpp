#ifndef __XML_CLIENT_HPP
#define __XML_CLIENT_HPP

#include "./StandardInc.hpp"
#include "ParseXml.hpp"
#include "../GWInterfaceDef.h"
#include "../db/DBOperator.h"
//#include "client_socket_api.h"

#define CMS_END_FLAG "</STAR_NET2012>"
#define XML_SND_TIMEOUT 30
#define XML_REV_TIMEOUT 30
#define XML_SEL_TIMEOUT 5

//云台控制命令 上              下         左             右           右上              右下                右上          左下
enum PTZ_CMD{PTZ_CMD_UP=0,PTZ_CMD_DOWN,PTZ_CMD_LEFT,PTZ_CMD_RIGHT,PTZ_CMD_RIGHT_UP,PTZ_CMD_RIGHT_DOWN,PTZ_CMD_LEFT_UP,PTZ_CMD_LEFT_DOWN
//             焦点变小               焦点变大                光圈变小               光圈变大                变倍增大        变倍减小
,PTZ_CMD_FOCUS_SMALLER,PTZ_CMD_FOCUS_LARGEN,PTZ_CMD_APERTURE_SMALLER,PTZ_CMD_APERTURE_LARGEN,PTZ_CMD_ZOOMOUT,PTZ_CMD_ZOOMIN
//            自动          调动预置点      调动巡航路径      停止巡航             开启灯光            关闭灯光          打开雨刷
,PTZ_CMD_AUTO,PTZ_CMD_PRESET,PTZ_CMD_RUN_CRUISE,PTZ_CMD_STOP_CRUISE,PTZ_CMD_OPEN_LIGHT,PTZ_CMD_OFF_LIGHT,PTZ_CMD_OPEN_WIPER
//            关闭雨刷             停止           添加预置点    删除预置点           删除巡航路径       添加巡航路径
,PTZ_CMD_OFF_WIPER,PTZ_CMD_STOP,PTZ_CMD_ADD_PRESET,PTZ_CMD_DEL_PRESET,PTZ_CMD_DEL_CRUISE,PTZ_CMD_ADD_CRUISE};

//char *sql_field[]={"gb_gb_id","gb_app_name","gb_app_ip","gb_app_port","gb_username","gb_password","gb_parent_gb_id",
//"gb_parent_name","gb_parent_ip","gb_parent_port","gb_domain_port","gb_parent_username","gb_parent_password","gb_parent_authorize"};

enum sql_field_seq{gb_id_seq,app_name_seq,app_ip_seq,app_port_seq,username_seq,password_seq,parent_gb_id_seq,parent_name_seq,parent_ip
				,parent_port_seq,domain_port_seq,parent_username_seq,parent_password_seq,parent_authorize};

typedef struct _PTZControlInfo
{
	char cDevGBID[64];
	int nPTZCmdType;
	sipd_ptz_control ptzMoveControl;
	sipd_tour_control  ptzTourControl;
	sipd_preset_control ptzPresetControl;
	sipd_autoscan_control ptzAutoScanControl;
	bool bEnable;	
	time_t cmdTime;
	_PTZControlInfo()
	{
		nPTZCmdType = 0;
		bEnable = false;
		cmdTime = 0;
		memset(cDevGBID,0,sizeof(cDevGBID));
		memset(&ptzMoveControl,0,sizeof(sipd_ptz_control));
		memset(&ptzTourControl,0,sizeof(sipd_tour_control));
		memset(&ptzPresetControl,0,sizeof(sipd_preset_control));
		memset(&ptzAutoScanControl,0,sizeof(sipd_autoscan_control));
	}
	
}PTZControlInfo,*LPPTZControlInfo;

typedef struct _DeviceRight
{
	time_t check_time;
	bool bEnablePTZ;
	char cGBID[128];

	_DeviceRight()
	{
		check_time = 0;
		bEnablePTZ = false;
		memset(cGBID,0,sizeof(cGBID));
	}
}DeviceRight,*LPDeviceRight;

typedef struct _ServerInfo
{
	string strServerIP;
	string strUserName;
	string strUserID;
	string strPassWord;
	string strLocalIP;
	int nServerPort;
	int nServerState; //0:离线 1:在线
	SOCKET client_socket;
	bool bConnect;

	_ServerInfo()
	{
		strServerIP = "";
		strUserName = "";
		strPassWord = "";
		strLocalIP = "";
		strUserID = "";
		nServerPort = 0;
		nServerState = 0;
		client_socket = NULL;
		bConnect = false;
	}
}ServerInfo,*LPServerInfo;

typedef struct _ThreadInfo
{
	HANDLE ThreadHandle;
	bool isExit;
	_ThreadInfo()
	{
		ThreadHandle = NULL;
		isExit = false;
	}
}ThreadInfo,*LPThreadInfo;

typedef struct _RecordInfo
{
	string strStartRecordTime;
	string strEndRecordTime;
	string strGBID;
	string strDevName;
	_RecordInfo()
	{
		strStartRecordTime = "";
		strEndRecordTime = "";
		strGBID = "";
		strDevName = "";
	}
}RecordInfo,*LPRecordInfo;

//typedef void(*cms_notify_func)(session::Session_base *const sock,string &data);

int cms_logon(string strSourceIP,ServerInfo &cmsInfo);
bool Get_MD5_data(unsigned char *out_data, int out_data_length,unsigned char *in_data) /* 竦肕D5处理后的数据保存在out_password中，成功则 ?豻rue */;
bool IsRead(SOCKET sock);

bool SendHeart(ServerInfo &cmsInfo);
int QueryDeviceRight(ServerInfo &cmsInfo,string strUUID);
int QuerySingleRecordEx(ServerInfo VODServer,string strDevGBID,string strDevName,list<RecordInfo>&RecordTimeLst,char *cUrl);
void ParseRecordInfo(char *pos,int nMaxLen,string strDevGBID,string strDevName,list<RecordInfo>&RecordTime);
LONG xml_OpenCon(ServerInfo &cmsInfo);
void xml_closeCon(ServerInfo &cmsInfo);
bool ParseHttpParam(char *pBuf,string strKey,string strDelimiter,void *pOut,int nOutLen,int nType,int &nLen);
string trans(string time);

int SendPTZMoveRequest(ServerInfo DMSServer,SN_DEVICE SnDev,LPPTZControlInfo pPTZInfo);

int SendDMSCmd(ServerInfo DMSServer,char *SendBuf,int nLen,bool bRecv=false);

int transPTZCmdCode(int nPTZCmdType,int nCmd);

#endif