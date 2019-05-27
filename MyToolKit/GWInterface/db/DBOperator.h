#ifndef _DB_OPERATOR_HPP
#define _DB_OPERATOR_HPP

#include "./StandardInc.hpp"
#include "../GWInterfaceDef.h"

#define CMS_SERVER_ID	"1"
#define DMS_SERVER_ID	"3"
#define VOD_SERVER_ID	"9"



typedef struct  _SnDevice
{
	string	strDevId;		// dvr 或 nvr 或 ipc 的设备ID
	string   strParentDevId;
	string  strGBID;	//国标ID
	string	strDevName;
	string	strDevIP;
	string	strDevUser;
	string	strDevPass;
	string  strConnectType;
	string  strInstallTime;
	string  strPosID;
	int		nDevState;
	int		nChannelCnt;
	int		nAlarmInIOCnt;
	int		nAlarmOutCnt;
	int		nDevPort;
	int		nDevTypeId; //1:编码设备 2:报警设备
	int		nDevFirm;	//设备类型
	int		nDevChannelNum;	//	dvr 或者 ipc 设备的通道数

	string	strDevFirm;
	string	strDevSN;
	int	nDevTypeChild;	// 以上来自 sn_device 1:DVR 2:IPC 3:NVR	
	int		nVideoPort;		//	来自 sn_video_dvr
	int		nMobilePort;	// 来自 sn_video_dvr
	int     nWebPort;		//	来自 sn_video_dvr
	int		nChannelNO; //通道号
	int		nMode;	//协议 4:onvif
	_SnDevice()
	{
		strDevId = "";
		strParentDevId = "";
		strGBID = "";
		strDevName = "";
		strDevIP = "";
		strDevUser = "";
		strDevPass = "";
		nDevPort = 0;
		nDevTypeId = 0;
		nDevChannelNum = 0;
		strDevFirm = "";
		
		strDevSN = "";
		strConnectType = "";
		strInstallTime = "";
		strPosID = "";
		nVideoPort = 0;
		nMobilePort = 0;
		nWebPort = 0;
		nChannelNO = 0;
		nChannelCnt = 0;
		nAlarmInIOCnt = 0;
		nAlarmOutCnt=0;
		nDevFirm = 0;
		nDevTypeChild = 0;
	}
}SN_DEVICE,*LPSN_DEVICE;

typedef struct _SN_Position
{
	string strPosID;
	string strParentPosID;
	string strPosName;
	string strPosAddr;
	string strDoMail; //邮编
	string strAbsoluteAddr; //绝对地址
	int nPosLevel; //优先级
	
	_SN_Position()
	{
		strPosID = "";
		strParentPosID = "";
		strPosName = "";
		strPosAddr = "";
		strDoMail = "";
		strAbsoluteAddr = "";
		nPosLevel = -1;
	}
	
}SN_Position,*LPSN_Position;

typedef struct _AlarmInfo
{
	string strAlarmID;
	string strDevID;
	string strParentDevID;
	string strAlarmTime;
	int nAlarmLevel;
	int nAlarmType;

	_AlarmInfo()
	{
		strAlarmID = "";
		strDevID = "";
		strParentDevID = "";
		strAlarmTime = "";
		nAlarmLevel = 0;
		nAlarmType = 0;
	}
}AlarmInfo,*LPAlarmInfo;

int getUserID(string strDBAddr,int nPort,string strUserName,string &strUserID);
int getServer(string server_type_id,string strDBAddr,int nDBPort,string &strCMSAddr,int &nCMSPort);
int getAllGBSnDevice(string strDBAddr,int nPort,map<string,SN_DEVICE> &SnDeviceLst,string strUserName);
int UpdateSnDeviceByID(string strDBAddr,int nPort,SN_DEVICE &SnDevice);
int GetSnPosition(string strDBAddr,int nPort,map<string,SN_Position> &SnPositionLst);
int read_profile_string( const char *section, const char *key,char *value, int size, const char *default_value, const char *file);
int ParaseAlarmReport(string strDBAddr,int nPort,char *cAlarmReport,list<AlarmInfo> &alarmLst);

int GetDevTypeName(int nDevType,char *cDevName);

static int load_ini_file(const char *file, char *buf,int *file_size);
static int parse_file(const char *section, const char *key, const char *buf,int *sec_s,int *sec_e, int *key_s,int *key_e, int *value_s, int *value_e);
#endif