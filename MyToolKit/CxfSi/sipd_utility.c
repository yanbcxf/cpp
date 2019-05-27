
#include "list.h"
#include "sipd_utility.h"

#define UA_STRING "SN SIPGW V1.0"
#define URI_MAX_LEN 100

#define SIPD_DEFAULT_AUTOSCAN_SPEED   	(15)
#define SIPD_START_DEVICE_SN				(1)

static CRITICAL_SECTION sipd_variable_lock;
static CRITICAL_SECTION sipd_almwork_lock;
static CRITICAL_SECTION sipd_context_eXosip_lock;
static CRITICAL_SECTION sipd_ev_wait_lock;
static CRITICAL_SECTION sipd_isOnLine_lock;
static CRITICAL_SECTION sipd_KeepAliveCn_lock;
static CRITICAL_SECTION sipd_CfgChange_lock;
static CRITICAL_SECTION sipd_regid_lock;
static CRITICAL_SECTION sipd_notify_lock;
static CRITICAL_SECTION sipd_logout_lock;
static CRITICAL_SECTION sipd_RegErrCnt_lock;
static CRITICAL_SECTION g_gw_variable_lock;

static int g_alm_work_flg = 0; 
static int g_ev_wait_flg = 1;
//	保存了不同的注册状态 ，  1 本域注册成功， -1 本域注册失败
static int g_isOnline = -1;   
static int g_notifyflg = 0;
static int g_logout_flg = 0;
static int g_reg_err_cnt = 0;

static int auto_scan_speed = SIPD_DEFAULT_AUTOSCAN_SPEED;
static unsigned int device_sn = SIPD_START_DEVICE_SN;

static SIPD_GATEWAY_CFG g_Cfg;
static SIPD_GATEWAY_CFG g_Cfg_Change;  

/*注册信息*/
typedef struct regparam_t {
	int regid;  
	int expiry;
	int auth;
} regparam_t;

struct regparam_t regparam = { 0, 3600, 0 };
static int g_keepalive_count = 0;

/*TESTCODE*/
#if 0
extern struct sipd_media_session test_media_session;
#endif


char * get_local_host_ip(void)
{
	char hostname[256]={0};
	int res;
	struct hostent* pHostent = NULL;
	struct sockaddr_in sa;

	memset(hostname, 0, sizeof(hostname));
	memset(&sa, 0, sizeof(sa));

	res = gethostname(hostname, sizeof(hostname));
	if (res != 0)
		return 0;

	pHostent = gethostbyname(hostname);
	if (pHostent==NULL)
		return 0;

	memcpy ( &sa.sin_addr.s_addr, pHostent->h_addr_list[0],pHostent->h_length);
	return inet_ntoa(sa.sin_addr);
} 

int sipd_init_ptz_variable(void)
{
	EnterCriticalSection(&sipd_variable_lock);
	auto_scan_speed = SIPD_DEFAULT_AUTOSCAN_SPEED;
	device_sn = SIPD_START_DEVICE_SN;
	LeaveCriticalSection(&sipd_variable_lock);
	return 0;
}

unsigned int get_device_sn(void)
{
	unsigned int ret;
	EnterCriticalSection(&sipd_variable_lock);
	ret = device_sn;
	device_sn++;
	LeaveCriticalSection(&sipd_variable_lock);

	return ret;
}

void Set_EvWaitFlg(int in)
{
	EnterCriticalSection(&sipd_ev_wait_lock);
	g_ev_wait_flg = in;
	LeaveCriticalSection(&sipd_ev_wait_lock);
}

int Get_EvWaitFlg()
{
	int out;
	EnterCriticalSection(&sipd_ev_wait_lock);
	out = g_ev_wait_flg;
	LeaveCriticalSection(&sipd_ev_wait_lock);

	return out;
}

void context_eXosip_lock()
{
	EnterCriticalSection(&sipd_context_eXosip_lock);
}

void context_eXosip_unlock()
{
	LeaveCriticalSection(&sipd_context_eXosip_lock);
}

void gw_g_variable_lock()
{
	EnterCriticalSection(&g_gw_variable_lock);
}

void gw_g_variable_unlock()
{
	LeaveCriticalSection(&g_gw_variable_lock);
}


void SetAlmWorkFlg(int in)
{
	EnterCriticalSection(&sipd_almwork_lock);
	g_alm_work_flg = in;
	LeaveCriticalSection(&sipd_almwork_lock);
}

int GetAlmWorkFlg()
{
	int out;
	
	EnterCriticalSection(&sipd_almwork_lock);
	out = g_alm_work_flg;
	LeaveCriticalSection(&sipd_almwork_lock);

	return out;
}

void Set_LogOutFlg(int in)
{
	EnterCriticalSection(&sipd_logout_lock);
	g_logout_flg= in;
	LeaveCriticalSection(&sipd_logout_lock);
}

int Get_LogOutFlg()
{
	int out;
	EnterCriticalSection(&sipd_logout_lock);
	out = g_logout_flg;
	LeaveCriticalSection(&sipd_logout_lock);

	return out;
}

void Set_RegErrCnt(int in)
{
	EnterCriticalSection(&sipd_RegErrCnt_lock);
	g_reg_err_cnt = in;
	LeaveCriticalSection(&sipd_RegErrCnt_lock);
}

int Get_RegErrCnt()
{
	int out = 0;
	EnterCriticalSection(&sipd_RegErrCnt_lock);
	out = g_reg_err_cnt;
	LeaveCriticalSection(&sipd_RegErrCnt_lock);

	return out;
}

void RegErrCounter()
{
	EnterCriticalSection(&sipd_RegErrCnt_lock);
	g_reg_err_cnt++;
	LeaveCriticalSection(&sipd_RegErrCnt_lock);
}

void Set_NotifyFlg(int in)
{
	EnterCriticalSection(&sipd_notify_lock);
	g_notifyflg = in;
	LeaveCriticalSection(&sipd_notify_lock);
}

int Get_NotifyFlg()
{
	int out;
	EnterCriticalSection(&sipd_notify_lock);
	out = g_notifyflg;
	LeaveCriticalSection(&sipd_notify_lock);

	return out;
}

void reg_id_lock()
{
	EnterCriticalSection(&sipd_regid_lock);
}

void reg_id_unlock()
{
	LeaveCriticalSection(&sipd_regid_lock);
}

void para_change_lock()
{
	EnterCriticalSection(&sipd_CfgChange_lock);
}

void para_change_unlock()
{
	LeaveCriticalSection(&sipd_CfgChange_lock);
}

void Set_Dev_Online(int isonline)
{
	EnterCriticalSection(&sipd_isOnLine_lock);
	g_isOnline = isonline;
	LeaveCriticalSection(&sipd_isOnLine_lock);
}

int Get_Dev_Online()
{
	int online;

	EnterCriticalSection(&sipd_isOnLine_lock);
       online = g_isOnline;
	LeaveCriticalSection(&sipd_isOnLine_lock);

	return online;
}

void keepalivecn_lock()
{
	EnterCriticalSection(&sipd_KeepAliveCn_lock);
}

void keepalivecn_unlock()
{
	LeaveCriticalSection(&sipd_KeepAliveCn_lock);
}

//	设置 SIP 域参数
static void sip_config_rsp(int* type, void* param1)
{
	sipd_ip_config_ind* ipInd=NULL;
	sipd_servercfg_config_ind*  cfgInd=NULL;
	SIPD_GATEWAY_CFG  cfg;

	memset(&cfg, 0, sizeof(cfg));
	
	if(NULL == type || NULL == param1)
	{
		printf("sip_config_pcb [ERROR] Invalid input para, ERROR!\n");
		return 0;
	}

	if(SIPD_CONFIG_NOTIFY_CB_IP == *type)
	{
		printf("sip_config_pcb [INFO] ip change ind!\n");
	}
	else if(SIPD_CONFIG_NOTIFY_CB_CFG == *type)
	{
		printf("sip_config_pcb [INFO] cfg change ind!\n");
		
		cfgInd = (sipd_servercfg_config_ind*)param1;
		strncpy(cfg.local_domain_name, cfgInd->new_cfg.local_domain_name, MAX_SIPD_DEVICE_ID_LEN);
		strncpy(cfg.local_domain_id, cfgInd->new_cfg.local_domain_id, MAX_SIPD_DEVICE_ID_LEN);
		strncpy(cfg.local_ip, cfgInd->new_cfg.local_ip, MAX_SIPD_NORMAL_LEN);
		cfg.local_port = cfgInd->new_cfg.local_port;
		strncpy(cfg.local_gateway, cfgInd->new_cfg.local_gateway, MAX_SIPD_NORMAL_LEN);
		strncpy(cfg.upper_server_name, cfgInd->new_cfg.upper_server_name, MAX_SIPD_DEVICE_ID_LEN);
		strncpy(cfg.upper_server_id, cfgInd->new_cfg.upper_server_id, MAX_SIPD_DEVICE_ID_LEN);
		strncpy(cfg.upper_server_ip, cfgInd->new_cfg.upper_server_ip, MAX_SIPD_NORMAL_LEN);
		cfg.upper_server_port = cfgInd->new_cfg.upper_server_port;
		strncpy(cfg.register_upper_server_passwd, cfgInd->new_cfg.register_upper_server_passwd, MAX_SIPD_NORMAL_LEN);
		cfg.register_keepalive = cfgInd->new_cfg.register_keepalive;
		cfg.keepalive_internal = cfgInd->new_cfg.keepalive_internal;

		Set_Para_Change(&cfg);

		//gw_reinit_cfg();
		
	}
	else
	{
		
	}	
	return;
}


int get_domain_server_cfg(SIPD_GATEWAY_CFG* pCfg)
{
	if(NULL == pCfg)
	{
		return -1;
	}

	gw_g_variable_lock();
	memset(pCfg, 0, sizeof(SIPD_GATEWAY_CFG));
	strncpy(pCfg->local_domain_name, g_Cfg.local_domain_name, MAX_SIPD_DEVICE_ID_LEN);
	strncpy(pCfg->local_domain_id, g_Cfg.local_domain_id, MAX_SIPD_DEVICE_ID_LEN);
	pCfg->local_port = g_Cfg.local_port;
	strncpy(pCfg->local_ip, g_Cfg.local_ip, MAX_SIPD_NORMAL_LEN);
	strncpy(pCfg->local_gateway, g_Cfg.local_gateway, MAX_SIPD_NORMAL_LEN);
	strncpy(pCfg->upper_server_name, g_Cfg.upper_server_name, MAX_SIPD_DEVICE_ID_LEN);
	strncpy(pCfg->upper_server_id, g_Cfg.upper_server_id, MAX_SIPD_DEVICE_ID_LEN);
	strncpy(pCfg->upper_server_ip, g_Cfg.upper_server_ip, MAX_SIPD_NORMAL_LEN);
	pCfg->upper_server_port = g_Cfg.upper_server_port;
	strncpy(pCfg->register_upper_server_passwd, g_Cfg.register_upper_server_passwd, MAX_SIPD_NORMAL_LEN);
	pCfg->register_keepalive = g_Cfg.register_keepalive;
	pCfg->keepalive_internal = g_Cfg.keepalive_internal;
	gw_g_variable_unlock();

	return 0;
}


static void sip_alarm_ind_rsp(sipd_alarm_ind * alarm_info)
{
	if(NULL == alarm_info)
	{
		printf("sip_alarm_ind_pcb Invalid para! ERROR!\n");
		return;
	}

	if(SIPD_ALARM_IO == alarm_info->alarm_type)
	{
		char   server_sip[MAX_SIPD_URI+1] = {0};
		char   from_sip[MAX_SIPD_URI+1] = {0};
		SIPD_GATEWAY_CFG sipd_cfg;

		SetAlmWorkFlg(1);  

		memset(server_sip, 0, sizeof(server_sip));
		memset(from_sip, 0, sizeof(from_sip));
		memset(&sipd_cfg, 0, sizeof(sipd_cfg));

		get_PRM_SIPD_CFG(&sipd_cfg);

		memset(server_sip, 0, sizeof(server_sip));
		sprintf(server_sip,"sip:%s@%s:%d", sipd_cfg.upper_server_id, sipd_cfg.upper_server_ip, sipd_cfg.upper_server_port);

		memset(from_sip, 0, sizeof(from_sip));
		sprintf(from_sip,"sip:%s@%s", alarm_info->device_id, sipd_cfg.local_domain_name);

		if(Get_EvWaitFlg() == 1)
		{
			build_and_send_alarm_message(from_sip, server_sip, get_device_sn(), alarm_info->alarm_id, \
				alarm_info->alarm_priority, sipd_alarm_condition_device, NULL, alarm_info->alarm_time, NULL, NULL);
		}
	}
	else if (SIPD_ALARM_VIDEO== alarm_info->alarm_type)
	{
		char   server_sip[MAX_SIPD_URI+1] = {0};
		char   from_sip[MAX_SIPD_URI+1] = {0};
		SIPD_GATEWAY_CFG sipd_cfg;

		SetAlmWorkFlg(1);  

		memset(server_sip, 0, sizeof(server_sip));
		memset(from_sip, 0, sizeof(from_sip));
		memset(&sipd_cfg, 0, sizeof(sipd_cfg));

		get_PRM_SIPD_CFG(&sipd_cfg);

		memset(server_sip, 0, sizeof(server_sip));
		sprintf(server_sip,"sip:%s@%s:%d", sipd_cfg.upper_server_id, sipd_cfg.upper_server_ip, sipd_cfg.upper_server_port);

		memset(from_sip, 0, sizeof(from_sip));
		sprintf(from_sip,"sip:%s@%s", alarm_info->device_id, sipd_cfg.local_domain_name);

		if(Get_EvWaitFlg() == 1)
		{
			build_and_send_alarm_message(from_sip, server_sip, get_device_sn(), alarm_info->alarm_id,\
				alarm_info->alarm_priority, sipd_alarm_condition_video, NULL, alarm_info->alarm_time, NULL, NULL);
		}
	}
	else if (SIPD_ALARM_OTHER== alarm_info->alarm_type)
	{
		char   server_sip[MAX_SIPD_URI+1] = {0};
		char   from_sip[MAX_SIPD_URI+1] = {0};
		SIPD_GATEWAY_CFG sipd_cfg;

		SetAlmWorkFlg(1);  

		memset(server_sip, 0, sizeof(server_sip));
		memset(from_sip, 0, sizeof(from_sip));
		memset(&sipd_cfg, 0, sizeof(sipd_cfg));

		get_PRM_SIPD_CFG(&sipd_cfg);

		memset(server_sip, 0, sizeof(server_sip));
		sprintf(server_sip,"sip:%s@%s:%d", sipd_cfg.upper_server_id, sipd_cfg.upper_server_ip, sipd_cfg.upper_server_port);

		memset(from_sip, 0, sizeof(from_sip));
		sprintf(from_sip,"sip:%s@%s", alarm_info->device_id, sipd_cfg.local_domain_name);

		if(Get_EvWaitFlg() == 1)
		{
			build_and_send_alarm_message(from_sip, server_sip, get_device_sn(), alarm_info->alarm_id,\
				alarm_info->alarm_priority, sipd_alarm_condition_others, NULL, alarm_info->alarm_time, NULL, NULL);
		}
	}
	else
	{
		printf("sip_alarm_ind_pcb Unknow alarm type\n");
	}
	
	
	return;
}

static void sip_control_rsp(sipd_control_rsp* pControlRsp)
{
	if(NULL == pControlRsp)
	{
		printf("sip_control_req_pcb Invalid para! ERROR!\n");
		return;
	}

	if(Get_EvWaitFlg() == 1)
	{
		int	ret = build_and_send_device_control_response_message(pControlRsp->from_uri, pControlRsp->to_uri, \
			atoi(pControlRsp->SN), pControlRsp->device_id, pControlRsp->result);
		if(ret != OSIP_SUCCESS)
		{
			printf("sipd_MsgHandle MSG_ID_GW_CONTROL_RSP build_device_control_response_message ERROR \n");
		}
	}		
	return;
}


static void sip_info_rsp(int* type, void* param1, void* param2)
{
	sipd_deviceinfo_rsp* pdeviceinfoRsp = NULL;
	sipd_catainfo_adaptor_rsp catainfoRsp;
	sipd_stateinfo_adaptor_rsp stateinfoRsp;
	sipd_recordinfo_adaptor_rsp fileInfoRsp;
	
	sipd_catalog_rsp_basic_info* pCataBasicInfo = NULL;
	sipd_catalog_item* pCataItem = NULL;

	sipd_devicestate_rsp_basic_info* pStateBasicInfo = NULL;
	sipd_alarm_status_item* pStateItem = NULL;

	sipd_recordinfo_rsp_basic_info* pRecordBasicInfo = NULL;
	sipd_file_item* pFileItem = NULL; 

	int i=0;
	
	memset(&catainfoRsp, 0, sizeof(catainfoRsp));
	memset(&stateinfoRsp, 0, sizeof(stateinfoRsp));
	memset(&fileInfoRsp, 0, sizeof(fileInfoRsp));
	
	if(NULL == type || NULL == param1)
	{
		printf("sip_info_req_pcb [ERROR] Invalid input para, ERROR!\n");
		return 0;
	}

	if(SIPD_INFO_RSP_CB_CATALOG == *type)
	{
		pCataBasicInfo = (sipd_catalog_rsp_basic_info*)param1;
		if(NULL == param2)
		{
			catainfoRsp.basic_info.SumNum = 0;
			catainfoRsp.basic_info.DeviceList_Num = 0;

			strncpy(catainfoRsp.basic_info.from_uri, pCataBasicInfo->from_uri, MAX_SIPD_URI);
			strncpy(catainfoRsp.basic_info.to_uri, pCataBasicInfo->to_uri, MAX_SIPD_URI);
			strncpy(catainfoRsp.basic_info.SN, pCataBasicInfo->SN, MAX_SIPD_DEVICE_ID_LEN);
			strncpy(catainfoRsp.basic_info.device_id, pCataBasicInfo->device_id, MAX_SIPD_DEVICE_ID_LEN);
		}
		else
		{
			pCataItem = (sipd_catalog_item*)param2;

			strncpy(catainfoRsp.basic_info.from_uri, pCataBasicInfo->from_uri, MAX_SIPD_URI);
			strncpy(catainfoRsp.basic_info.to_uri, pCataBasicInfo->to_uri, MAX_SIPD_URI);
			strncpy(catainfoRsp.basic_info.SN, pCataBasicInfo->SN, MAX_SIPD_DEVICE_ID_LEN);
			strncpy(catainfoRsp.basic_info.device_id, pCataBasicInfo->device_id, MAX_SIPD_DEVICE_ID_LEN);
			catainfoRsp.basic_info.SumNum = pCataBasicInfo->SumNum;

			if(pCataBasicInfo->DeviceList_Num  > MAX_SIPD_ITEM_RSP)
			{
				catainfoRsp.basic_info.DeviceList_Num = MAX_SIPD_ITEM_RSP;
				printf("sip_info_req_pcb WARNING catalog item number DeviceList_Num=%d exceed MAX_SIPD_ITEM_RSP(%d) \n", pCataBasicInfo->DeviceList_Num, MAX_SIPD_ITEM_RSP);
			}
			else
			{
				catainfoRsp.basic_info.DeviceList_Num = pCataBasicInfo->DeviceList_Num;
			}

			for(i=0; i<(catainfoRsp.basic_info.DeviceList_Num); i++)
			{
				strncpy(catainfoRsp.cata_items[i].device_id, pCataItem[i].device_id, MAX_SIPD_DEVICE_ID_LEN);
				strncpy(catainfoRsp.cata_items[i].Name, pCataItem[i].Name, MAX_SIPD_NORMAL_LEN);
				strncpy(catainfoRsp.cata_items[i].Manufacturer, pCataItem[i].Manufacturer, MAX_SIPD_MANUFACTURER_NAME_LEN);
				strncpy(catainfoRsp.cata_items[i].Model, pCataItem[i].Model, MAX_SIPD_MODEL_NAME_LEN);
				strncpy(catainfoRsp.cata_items[i].Owner, pCataItem[i].Owner, MAX_SIPD_NORMAL_LEN);
				strncpy(catainfoRsp.cata_items[i].CivilCode, pCataItem[i].CivilCode, MAX_SIPD_NORMAL_LEN);
				strncpy(catainfoRsp.cata_items[i].Block, pCataItem[i].Block, MAX_SIPD_NORMAL_LEN);
				strncpy(catainfoRsp.cata_items[i].Address, pCataItem[i].Address, MAX_SIPD_URI);
				catainfoRsp.cata_items[i].Parental = pCataItem[i].Parental;
				strncpy(catainfoRsp.cata_items[i].ParentID, pCataItem[i].ParentID, MAX_SIPD_DEVICE_ID_LEN);
				catainfoRsp.cata_items[i].SafteyWay = pCataItem[i].SafteyWay;
				catainfoRsp.cata_items[i].RegisterWay = pCataItem[i].RegisterWay;
				strncpy(catainfoRsp.cata_items[i].CertNum, pCataItem[i].CertNum, MAX_SIPD_URI);
				catainfoRsp.cata_items[i].Certifiable = pCataItem[i].Certifiable;
				catainfoRsp.cata_items[i].ErrCode = pCataItem[i].ErrCode;
				strncpy(catainfoRsp.cata_items[i].EndTime, pCataItem[i].EndTime, MAX_SIPD_TIME_LEN);
				catainfoRsp.cata_items[i].Secrecy = pCataItem[i].Secrecy;
				strncpy(catainfoRsp.cata_items[i].IPAddress, pCataItem[i].IPAddress, MAX_SIPD_NORMAL_LEN);
				catainfoRsp.cata_items[i].Port = pCataItem[i].Port;
				strncpy(catainfoRsp.cata_items[i].Password, pCataItem[i].Password, MAX_SIPD_NORMAL_LEN);
				catainfoRsp.cata_items[i].Status = pCataItem[i].Status;
				strncpy(catainfoRsp.cata_items[i].Longitude, pCataItem[i].Longitude, MAX_SIPD_LONGITUDE_LEN);
				strncpy(catainfoRsp.cata_items[i].Latitude, pCataItem[i].Latitude, MAX_SIPD_LATITUDE_LEN);
			}
		}

		if(Get_EvWaitFlg() == 1)
		{
			sipd_dev_catalog_rsp(&catainfoRsp);
		}
	}
	else if(SIPD_INFO_RSP_CB_DEVICEINFO == *type)
	{
		pdeviceinfoRsp = (sipd_deviceinfo_rsp*)param1;
		if(Get_EvWaitFlg() == 1)
		{
			sipd_dev_info_rsp(pdeviceinfoRsp);
		}
	}
	else if(SIPD_INFO_RSP_CB_DEVICESTATE == *type)
	{
		pStateBasicInfo = (sipd_devicestate_rsp_basic_info*)param1;
		if(NULL == param2)
		{
			strncpy(stateinfoRsp.basic_info.from_uri, pStateBasicInfo->from_uri, MAX_SIPD_URI);
			strncpy(stateinfoRsp.basic_info.to_uri, pStateBasicInfo->to_uri, MAX_SIPD_URI);
			strncpy(stateinfoRsp.basic_info.SN, pStateBasicInfo->SN, MAX_SIPD_DEVICE_ID_LEN);
			strncpy(stateinfoRsp.basic_info.device_id, pStateBasicInfo->device_id, MAX_SIPD_DEVICE_ID_LEN);
			stateinfoRsp.basic_info.Result = pStateBasicInfo->Result;
			stateinfoRsp.basic_info.Online = pStateBasicInfo->Online;
			stateinfoRsp.basic_info.Status = pStateBasicInfo->Status;
			strncpy(stateinfoRsp.basic_info.Reason, pStateBasicInfo->Reason, MAX_SIPD_URI);
			stateinfoRsp.basic_info.Encode = pStateBasicInfo->Encode;
			stateinfoRsp.basic_info.Record = pStateBasicInfo->Record;
			strncpy(stateinfoRsp.basic_info.DeviceTime, pStateBasicInfo->DeviceTime, MAX_SIPD_TIME_LEN);
			stateinfoRsp.basic_info.AlaramDeviceNum = 0;
		}
		else
		{
			pStateItem = (sipd_alarm_status_item*)param2;

			strncpy(stateinfoRsp.basic_info.from_uri, pStateBasicInfo->from_uri, MAX_SIPD_URI);
			strncpy(stateinfoRsp.basic_info.to_uri, pStateBasicInfo->to_uri, MAX_SIPD_URI);
			strncpy(stateinfoRsp.basic_info.SN, pStateBasicInfo->SN, MAX_SIPD_DEVICE_ID_LEN);
			strncpy(stateinfoRsp.basic_info.device_id, pStateBasicInfo->device_id, MAX_SIPD_DEVICE_ID_LEN);
			stateinfoRsp.basic_info.Result = pStateBasicInfo->Result;
			stateinfoRsp.basic_info.Online = pStateBasicInfo->Online;
			stateinfoRsp.basic_info.Status = pStateBasicInfo->Status;
			strncpy(stateinfoRsp.basic_info.Reason, pStateBasicInfo->Reason, MAX_SIPD_URI);
			stateinfoRsp.basic_info.Encode = pStateBasicInfo->Encode;
			stateinfoRsp.basic_info.Record = pStateBasicInfo->Record;
			strncpy(stateinfoRsp.basic_info.DeviceTime, pStateBasicInfo->DeviceTime, MAX_SIPD_TIME_LEN);

			if(pStateBasicInfo->AlaramDeviceNum > MAX_SIPD_ALARM_ITEM_RSP)
			{
				stateinfoRsp.basic_info.AlaramDeviceNum = MAX_SIPD_ALARM_ITEM_RSP;
			}
			else
			{
				stateinfoRsp.basic_info.AlaramDeviceNum = pStateBasicInfo->AlaramDeviceNum;
			}

			for(i=0; i<(stateinfoRsp.basic_info.AlaramDeviceNum); i++)
			{
				strncpy(stateinfoRsp.state_items[i].device_id, pStateItem[i].device_id, MAX_SIPD_DEVICE_ID_LEN);
				stateinfoRsp.state_items[i].Status = pStateItem[i].Status;
			}
		}

		if(Get_EvWaitFlg() == 1)
		{
			sipd_dev_stat_rsp(&stateinfoRsp);
		}		
	}
	else if(SIPD_INFO_RSP_CB_RECORDFILE == *type)
	{
		pRecordBasicInfo = (sipd_recordinfo_rsp_basic_info*)param1;
		if(NULL == param2)
		{
			strncpy(fileInfoRsp.basic_info.from_uri, pRecordBasicInfo->from_uri, MAX_SIPD_URI);
			strncpy(fileInfoRsp.basic_info.to_uri, pRecordBasicInfo->to_uri, MAX_SIPD_URI);
			strncpy(fileInfoRsp.basic_info.SN, pRecordBasicInfo->SN, MAX_SIPD_DEVICE_ID_LEN);
			strncpy(fileInfoRsp.basic_info.device_id, pRecordBasicInfo->device_id, MAX_SIPD_DEVICE_ID_LEN);
			strncpy(fileInfoRsp.basic_info.Name, pRecordBasicInfo->Name, MAX_SIPD_NORMAL_LEN);
			fileInfoRsp.basic_info.SumNum = 0;
			fileInfoRsp.basic_info.RecordListNum = 0;
		}
		else
		{
			pFileItem = (sipd_file_item*)param2;

			strncpy(fileInfoRsp.basic_info.from_uri, pRecordBasicInfo->from_uri, MAX_SIPD_URI);
			strncpy(fileInfoRsp.basic_info.to_uri, pRecordBasicInfo->to_uri, MAX_SIPD_URI);
			strncpy(fileInfoRsp.basic_info.SN, pRecordBasicInfo->SN, MAX_SIPD_DEVICE_ID_LEN);
			strncpy(fileInfoRsp.basic_info.device_id, pRecordBasicInfo->device_id, MAX_SIPD_DEVICE_ID_LEN);
			strncpy(fileInfoRsp.basic_info.Name, pRecordBasicInfo->Name, MAX_SIPD_NORMAL_LEN);
			fileInfoRsp.basic_info.SumNum = pRecordBasicInfo->SumNum;

			if(pRecordBasicInfo->RecordListNum  > MAX_SIPD_ITEM_RSP)
			{
				fileInfoRsp.basic_info.RecordListNum = MAX_SIPD_ITEM_RSP;
			}
			else
			{
				fileInfoRsp.basic_info.RecordListNum = pRecordBasicInfo->RecordListNum;
			}

			for(i=0; i<(fileInfoRsp.basic_info.RecordListNum); i++)
			{
				strncpy(fileInfoRsp.file_items[i].device_id, pFileItem[i].device_id, MAX_SIPD_DEVICE_ID_LEN);
				strncpy(fileInfoRsp.file_items[i].Name, pFileItem[i].Name, MAX_SIPD_NORMAL_LEN);
				strncpy(fileInfoRsp.file_items[i].FilePath, pFileItem[i].FilePath, MAX_SIPD_URI);
				strncpy(fileInfoRsp.file_items[i].Address, pFileItem[i].Address, MAX_SIPD_URI);
				strncpy(fileInfoRsp.file_items[i].StartTime, pFileItem[i].StartTime, MAX_SIPD_TIME_LEN);
				strncpy(fileInfoRsp.file_items[i].EndTime, pFileItem[i].EndTime, MAX_SIPD_TIME_LEN);
				fileInfoRsp.file_items[i].Secrecy = pFileItem[i].Secrecy;
				fileInfoRsp.file_items[i].Type = pFileItem[i].Type;
				strncpy(fileInfoRsp.file_items[i].RecorderID, pFileItem[i].RecorderID, MAX_SIPD_DEVICE_ID_LEN);
			}
		}
		
		if(Get_EvWaitFlg() == 1)
		{
			build_and_send_recordinfo_rsp(&fileInfoRsp);
		}	
	}
	else
	{
		printf("sip_info_req_pcb Unknow media control type\n");
	}

	
	return;
}

//	目录订阅 、报警订阅的回调函数
static void sip_subscribe_rsp(int* type, void* param1)
{
	sipd_catainfo_adaptor_rsp* psdp_info = NULL;
	sipd_alarm_ind* pSipd_alarm_ind = NULL;
	
	
	if(NULL == type || NULL == param1)
	{
		printf("sip_subscribe_pcb [ERROR] Invalid input para, ERROR!\n");
		return 0;
	}

	if(SIPD_SUBSCRIBE_CB_CATALOG_RSP == *type)
	{
		psdp_info = (sipd_catainfo_adaptor_rsp*)param1;
		if(Get_EvWaitFlg() == 1)
		{
			sipd_dev_catalog_notify(psdp_info);
		}
	}
	else if(SIPD_SUBSCRIBE_CB_ALARM_RSP == *type)
	{
		pSipd_alarm_ind = (sipd_alarm_ind*)param1;
		if(Get_EvWaitFlg() == 1)
		{
			build_and_send_alarm_notify(pSipd_alarm_ind);
		}
	}
	else
	{
		printf("sip_subscribe_pcb Unknow media control type\n");
	}
	
	return;
}

static void sip_media_rsp(int* type, void* param1)
{
	sipd_sender_sdp_info* psdp_info = NULL;
	sipd_media_file_end_ind* pfile_end_ind = NULL;

	//printf("sip_media_pcb \n");


	if(NULL == type || NULL == param1)
	{
		printf("sip_media_pcb [ERROR] Invalid input para, ERROR!\n");
		return 0;
	}

	if(SIPD_MEDIA_CONTROL_CB_INVITE_RSP == *type)
	{
		psdp_info = (sipd_sender_sdp_info*)param1;
		if(Get_EvWaitFlg() == 1)
		{
			//build_and_send_invite_200_ok_old(psdp_info, 1);
		}
	}
	else if(SIPD_MEDIA_CONTROL_CB_FILE_TO_END == *type)
	{
		pfile_end_ind = (sipd_media_file_end_ind*)param1;
		if(Get_EvWaitFlg() == 1)
		{
			build_and_send_file_to_end_message(pfile_end_ind->tid, pfile_end_ind->did, pfile_end_ind->o_id);
		}
	}
	else
	{
		printf("sip_media_pcb Unknow media control type\n");
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
//	注册回调请求

void Sipd_GetSIPServerCfg_func_register(Sipd_GetSIPServerCfg_func pfunc)
{

}


void Invite_play_playback_download_register(Invite_play_playback_download_func pfunc)
{
	pInvite_play_playback_download = pfunc;
}

void Invite_200_ok_register(Invite_play_playback_download_func pfunc)
{
	pInvite_200_ok = pfunc;
}

void Invite_no_answer_register(Invite_play_playback_download_func pfunc)
{
	pInvite_no_answer = pfunc;
}

void Invite_ack_register(Invite_play_playback_download_func pfunc)
{
	pInvite_ack = pfunc;
}

void Invite_bye_register(Invite_play_playback_download_func pfunc)
{
	pInvite_bye = pfunc;
}

void Invite_release_register(Invite_play_playback_download_func pfunc)
{
	pInvite_release = pfunc;
}

void Sipd_PTZ_Preset_func_register(Sipd_PTZ_Preset_func pfunc)
{
	pSipd_PTZ_Preset = pfunc;
}

void Sipd_PTZ_Tour_func_register(Sipd_PTZ_Tour_func pfunc)
{
	pSipd_PTZ_Tour = pfunc;
}

void Sipd_PTZ_AutoScan_func_register(Sipd_PTZ_AutoScan_func pfunc)
{
	pSipd_PTZ_AutoScan = pfunc;
}

void Sipd_PTZ_Move_func_register(Sipd_PTZ_Move_func pfunc)
{
	pSipd_PTZ_Move = pfunc;
}

void Sipd_record_control_func_register(Sipd_record_control_func pfunc)
{
	pSipd_record_control = pfunc;
}

void Sipd_alarm_control_func_register(Sipd_alarm_control_func pfunc)
{
	pSipd_alarm_control = pfunc;
}

void Sipd_media_control_notify_CMS_func_register(Sipd_media_control_notify_CMS_func pfunc)
{
	pSipd_media_control_notify_CMS = pfunc;
}

void Sipd_stateinfo_request_func_register(Sipd_stateinfo_request_func pfunc)
{
	pSipd_stateinfo_request = pfunc;
}

void Sipd_cataloginfo_request_func_register(Sipd_cataloginfo_request_func pfunc)
{
	pSipd_cataloginfo_request = pfunc;
}


void Sipd_search_recordfile_req_func_register(Sipd_search_recordfile_req_func pfunc)
{
	pSipd_search_recordfile_req = pfunc;
}

void Sipd_deviceinfo_request_func_register(Sipd_deviceinfo_request_func pfunc)
{
	pSipd_deviceinfo_request = pfunc;
}

void Sipd_settime_func_register(Sipd_settime_func pfunc)
{
	pSipd_settime = pfunc;
}

void Sipd_register_req_func_register(Sipd_register_req_func pfunc)
{
	pSipd_register_req = pfunc;
}

void sipd_message_query_req_func_register(sipd_message_query_or_response_req_func pfunc)
{
	pSipd_message_query_req = pfunc;
}

void sipd_message_response_req_func_register(sipd_message_query_or_response_req_func pfunc)
{
	pSipd_message_response_req = pfunc;
}

void sipd_on_idle_func_register(sipd_on_idle_func pfunc)
{
	pSipd_on_idle = pfunc;
}

void Siqd_catalog_query_response(sipd_catalog_rsp_basic_info* pCataBasicInfo, sipd_catalog_item* pCataItem)
{
	sipd_catainfo_adaptor_rsp catainfoRsp;
	memset(&catainfoRsp, 0, sizeof(catainfoRsp));
	if(NULL == pCataItem)
	{
		catainfoRsp.basic_info.SumNum = 0;
		catainfoRsp.basic_info.DeviceList_Num = 0;

		strncpy(catainfoRsp.basic_info.from_uri, pCataBasicInfo->from_uri, MAX_SIPD_URI);
		strncpy(catainfoRsp.basic_info.to_uri, pCataBasicInfo->to_uri, MAX_SIPD_URI);
		strncpy(catainfoRsp.basic_info.SN, pCataBasicInfo->SN, MAX_SIPD_DEVICE_ID_LEN);
		strncpy(catainfoRsp.basic_info.device_id, pCataBasicInfo->device_id, MAX_SIPD_DEVICE_ID_LEN);
	}
	else
	{
		int i;
		strncpy(catainfoRsp.basic_info.from_uri, pCataBasicInfo->from_uri, MAX_SIPD_URI);
		strncpy(catainfoRsp.basic_info.to_uri, pCataBasicInfo->to_uri, MAX_SIPD_URI);
		strncpy(catainfoRsp.basic_info.SN, pCataBasicInfo->SN, MAX_SIPD_DEVICE_ID_LEN);
		strncpy(catainfoRsp.basic_info.device_id, pCataBasicInfo->device_id, MAX_SIPD_DEVICE_ID_LEN);
		catainfoRsp.basic_info.SumNum = pCataBasicInfo->SumNum;


		if(pCataBasicInfo->DeviceList_Num  > MAX_SIPD_ITEM_RSP)
		{
			catainfoRsp.basic_info.DeviceList_Num = MAX_SIPD_ITEM_RSP;
			printf("sip_info_req_pcb WARNING catalog item number DeviceList_Num=%d exceed MAX_SIPD_ITEM_RSP(%d) \n", pCataBasicInfo->DeviceList_Num, MAX_SIPD_ITEM_RSP);
		}
		else
		{
			catainfoRsp.basic_info.DeviceList_Num = pCataBasicInfo->DeviceList_Num;
		}

		for(i=0; i<(catainfoRsp.basic_info.DeviceList_Num); i++)
		{
			strncpy(catainfoRsp.cata_items[i].device_id, pCataItem[i].device_id, MAX_SIPD_DEVICE_ID_LEN);
			strncpy(catainfoRsp.cata_items[i].Name, pCataItem[i].Name, MAX_SIPD_NORMAL_LEN);
			strncpy(catainfoRsp.cata_items[i].Manufacturer, pCataItem[i].Manufacturer, MAX_SIPD_MANUFACTURER_NAME_LEN);
			strncpy(catainfoRsp.cata_items[i].Model, pCataItem[i].Model, MAX_SIPD_MODEL_NAME_LEN);
			strncpy(catainfoRsp.cata_items[i].Owner, pCataItem[i].Owner, MAX_SIPD_NORMAL_LEN);
			strncpy(catainfoRsp.cata_items[i].CivilCode, pCataItem[i].CivilCode, MAX_SIPD_NORMAL_LEN);
			strncpy(catainfoRsp.cata_items[i].Block, pCataItem[i].Block, MAX_SIPD_NORMAL_LEN);
			strncpy(catainfoRsp.cata_items[i].Address, pCataItem[i].Address, MAX_SIPD_URI);
			catainfoRsp.cata_items[i].Parental = pCataItem[i].Parental;
			strncpy(catainfoRsp.cata_items[i].ParentID, pCataItem[i].ParentID, MAX_SIPD_DEVICE_ID_LEN);
			catainfoRsp.cata_items[i].SafteyWay = pCataItem[i].SafteyWay;
			catainfoRsp.cata_items[i].RegisterWay = pCataItem[i].RegisterWay;
			strncpy(catainfoRsp.cata_items[i].CertNum, pCataItem[i].CertNum, MAX_SIPD_URI);
			catainfoRsp.cata_items[i].Certifiable = pCataItem[i].Certifiable;
			catainfoRsp.cata_items[i].ErrCode = pCataItem[i].ErrCode;
			strncpy(catainfoRsp.cata_items[i].EndTime, pCataItem[i].EndTime, MAX_SIPD_TIME_LEN);
			catainfoRsp.cata_items[i].Secrecy = pCataItem[i].Secrecy;
			strncpy(catainfoRsp.cata_items[i].IPAddress, pCataItem[i].IPAddress, MAX_SIPD_NORMAL_LEN);
			catainfoRsp.cata_items[i].Port = pCataItem[i].Port;
			strncpy(catainfoRsp.cata_items[i].Password, pCataItem[i].Password, MAX_SIPD_NORMAL_LEN);
			catainfoRsp.cata_items[i].Status = pCataItem[i].Status;
			strncpy(catainfoRsp.cata_items[i].Longitude, pCataItem[i].Longitude, MAX_SIPD_LONGITUDE_LEN);
			strncpy(catainfoRsp.cata_items[i].Latitude, pCataItem[i].Latitude, MAX_SIPD_LATITUDE_LEN);
		}
	}

	if(Get_EvWaitFlg() == 1)
	{
		sipd_dev_catalog_rsp(&catainfoRsp);
		//sipd_dev_catalog_rsp(pdev_info->from_uri, pdev_info->to_uri, pdev_info->SN, pdev_info->device_id, "Owner1", "Address 1");
	}
}

//////////////////////////////////////////////////////////////////////////
//	以下为 向上级域注册

void Set_Reg_Id(int in)
{
	reg_id_lock();
	regparam.regid = in;
	reg_id_unlock();
}

int Get_Reg_Id()
{
	int out;
	reg_id_lock();
	out = regparam.regid;
	reg_id_unlock();

	return out;
}

//	检测配置是否变化
static int para_change()
{
	if((strcmp((char *)g_Cfg_Change.local_domain_id, (char *)g_Cfg.local_domain_id) != 0)||
		(strcmp((char *)g_Cfg_Change.local_domain_name, (char *)g_Cfg.local_domain_name) != 0)||
		(strcmp((char *)g_Cfg_Change.register_upper_server_passwd, (char *)g_Cfg.register_upper_server_passwd) != 0)||
		(strcmp((char *)g_Cfg_Change.upper_server_id, (char *)g_Cfg.upper_server_id) != 0)||
		(g_Cfg_Change.upper_server_port!=  g_Cfg.upper_server_port)||
		(strcmp((char *)g_Cfg_Change.upper_server_ip, (char *)g_Cfg.upper_server_ip) != 0)||
		(g_Cfg_Change.register_keepalive!= g_Cfg.register_keepalive)||
		(g_Cfg_Change.keepalive_internal!= g_Cfg.keepalive_internal))
		return 1;

	return 0;
}

void Set_Para_Change(SIPD_GATEWAY_CFG *src)
{
	para_change_lock();
	memcpy(&g_Cfg_Change,  src, sizeof(g_Cfg_Change));
	para_change_unlock();
}



void SIPD_LogOut()
{
	osip_message_t *reg;

	context_eXosip_lock();
	eXosip_lock(context_eXosip); 	
	eXosip_register_build_register(context_eXosip, Get_Reg_Id(), 0, &reg);//
	//eXosip_register_send_register(context_eXosip, Get_Reg_Id(),&reg);

	osip_message_free(reg);

	eXosip_register_send_register(context_eXosip, Get_Reg_Id(),NULL);

	//regparam.auth = 0;
	//regparam.expiry = 3600;
	//Set_Reg_Id(0);
	eXosip_unlock(context_eXosip);
	context_eXosip_unlock();

	//osip_message_free(reg);
}

void Setkeepalive_count(int in)
{
	keepalivecn_lock();
	g_keepalive_count = in;
	keepalivecn_unlock();
}

int SIPD_send_register_req(char *username, char *password, char *identity, char *registerer, char* contact)
{
	int ret = -1;
	int rid = 0;

	osip_message_t *reg = NULL;

	if((username == NULL)||(password == NULL)||(identity == NULL)||(registerer == NULL))
		return -1;

	context_eXosip_lock();
	eXosip_lock(context_eXosip);

	eXosip_set_user_agent(context_eXosip, UA_STRING);

	rid = eXosip_register_build_initial_register(context_eXosip, identity, registerer, 
		contact, regparam.expiry, &reg);

	if (rid < 1) 
	{
		printf("eXosip_register_build_initial_register failed\n");
		eXosip_unlock(context_eXosip);
		context_eXosip_unlock();
		return -1;

	}

	printf("username: %s password: [removed]\n", username);

	eXosip_clear_authentication_info(context_eXosip);  //去除上次加入的错误认证信息
	if (eXosip_add_authentication_info(context_eXosip, username, username, password, "MD5", NULL)) 
	{
		printf("eXosip_add_authentication_info failed");
		osip_message_free(reg);
		eXosip_unlock(context_eXosip);
		context_eXosip_unlock();
		return -1;

	}

#if 1
	ret = eXosip_register_send_register(context_eXosip, rid, reg);


	if (ret != 0) 
	{
		printf("eXosip_register_send_register failed\n");
		osip_message_free(reg);
		eXosip_unlock(context_eXosip);
		context_eXosip_unlock();
		return -1;
	}
#endif

	eXosip_unlock(context_eXosip);
	context_eXosip_unlock();

	return 0;
}



int send_message_response(const sipd_message_query_or_response * prsp, const char * route)
{
	osip_message_t *message = NULL;
	int ret;

	if(NULL == prsp  || NULL == prsp->body)
	{
		return -1;
	}

	context_eXosip_lock();
	eXosip_lock(context_eXosip);
	//printf("send_cataloginfo_pair_rsp to=%s from=%s \n", prsp->basic_info.to_uri, prsp->basic_info.from_uri);
	ret = eXosip_message_build_request(context_eXosip, &message, "MESSAGE", prsp->to_uri, prsp->from_uri, route);
	if(ret != OSIP_SUCCESS)
	{
		eXosip_unlock(context_eXosip);
		printf("send_cataloginfo_pair_rsp eXosip_message_build_request failed\n");
		if(message != NULL)
		{
			osip_message_free(message);
		}
		context_eXosip_unlock();
		return -1;
	}

	ret = osip_message_set_body(message, prsp->body, strlen(prsp->body));
	//ret = osip_message_set_body(message, "good morning", strlen("good morning"));
	if(ret != OSIP_SUCCESS)
	{
		eXosip_unlock(context_eXosip);
		printf("send_cataloginfo_pair_rsp osip_message_set_body failed\n");
		if(message != NULL)
		{
			osip_message_free(message);
		}
		context_eXosip_unlock();
		return -1;
	}

	ret = osip_message_set_content_type (message, "Application/MANSCDP+xml");
	if(ret != OSIP_SUCCESS)
	{
		eXosip_unlock(context_eXosip);
		printf("send_cataloginfo_pair_rsp osip_message_set_content_type failed\n");
		if(message != NULL)
		{
			osip_message_free(message);
		}
		context_eXosip_unlock();
		return -1;
	}

#if 1
	{
		char *dest=NULL;
		size_t length=0;
		ret = osip_message_to_str(message, &dest, &length);
		if (ret !=0) 
		{
			printf("%s:%d osip_message_to_str failed\n", __FUNCTION__, __LINE__);
		}
		printf("%s line=%d message:\n%s\n", __FUNCTION__, __LINE__, dest);
		osip_free(dest);
	}
#endif


	ret = eXosip_message_send_request (context_eXosip, message);
	if(ret != OSIP_SUCCESS)
	{
		int i = 1;	
	}
	eXosip_unlock(context_eXosip);
	context_eXosip_unlock();

	return 0;
}


#if defined(WIN32)
DWORD WINAPI SIPD_Reg_Proc(void *data)
#else
void *SIPD_Reg_Proc(void *arg)
#endif
{
	char username[MAX_SIPD_DEVICE_ID_LEN+1];
	char password[MAX_SIPD_NORMAL_LEN+1];
	char identity[URI_MAX_LEN];
	char registerer[URI_MAX_LEN];
	char contact[URI_MAX_LEN];
	int ret = -1;
	int sleeptime = 2;
	int maxRegErrCnt = 8;
	

	memset(username,0, MAX_SIPD_DEVICE_ID_LEN+1);
	memset(password,0, MAX_SIPD_NORMAL_LEN+1);
	memset(identity,0, URI_MAX_LEN);
	memset(registerer,0, URI_MAX_LEN);
	memset(contact, 0, URI_MAX_LEN);
	//memset(&g_Cfg, 0, sizeof(g_Cfg));
	memset(&g_Cfg_Change, 0, sizeof(g_Cfg_Change));

	
	//	将 g_Cfg 与 g_Cfg_Change 保持一致
	Set_Para_Change(&g_Cfg);

	strcpy(username, (char*)g_Cfg.local_domain_id);
	memcpy(password, g_Cfg.register_upper_server_passwd, sizeof(password));

	
	snprintf(identity, URI_MAX_LEN, "sip:%s@%s", username, g_Cfg.local_domain_name);
	snprintf(registerer, URI_MAX_LEN, "sip:%s@%s:%d", g_Cfg.upper_server_id, g_Cfg.upper_server_ip, g_Cfg.upper_server_port);
	snprintf(contact, URI_MAX_LEN, "sip:%s@%s:%d",username, g_Cfg.local_ip, g_Cfg.local_port);

	strcpy(g_Cfg.local_domain_id, username);
	
	regparam.expiry = g_Cfg.register_keepalive;

	
	ret = SIPD_send_register_req(username, password, identity, registerer, contact);  //发送初始注册包
	if(ret == -1)
	{
		printf("%s line=%d SIPD_send_register_req ERROR!!!! ret=%d \n", __FUNCTION__, __LINE__, ret);
	}
	
	while(1)
	{	
		if(para_change() == 1)
		{
			Set_RegErrCnt(0);
			
			if(Get_Reg_Id() > 0)
			{
				//	参数发送变化，原先注册成功了，现在退出
				int i=0;
				SIPD_LogOut(); 
				printf("log_out\n");

				//	等待退出成功
				i=0;
				while(1)
				{
					if(Get_LogOutFlg() == 1)
					{
						break;
					}
					
					if(i >= 15)
					{
						printf("%s line=%d wait log out too loog, don't wait any more!\n", __FUNCTION__, __LINE__);
						i=0;
						break;
					}
					i++;

					#if defined(WIN32)
					Sleep(2*1000);
					#else
					sleep(2);
					#endif
				}

				Set_LogOutFlg(0);
			}

			Set_Dev_Online(0);
			Setkeepalive_count(0); 
			Set_NotifyFlg(0);
			memset(username,0, MAX_SIPD_DEVICE_ID_LEN+1);
			memset(password,0, MAX_SIPD_NORMAL_LEN+1);
			memset(identity,0, URI_MAX_LEN);
			memset(registerer,0, URI_MAX_LEN);
			memset(contact, 0, URI_MAX_LEN);

			//	拷贝最新的 配置信息
			memcpy(&g_Cfg, &g_Cfg_Change, sizeof(SIPD_GATEWAY_CFG));
			regparam.expiry = g_Cfg.register_keepalive;


			if(context_eXosip != NULL)
			{
#if 1				
				if(Get_Reg_Id()> 0)
				{
					context_eXosip_lock();
					eXosip_lock (context_eXosip);
					
					ret = eXosip_register_remove(context_eXosip, Get_Reg_Id());
					if(ret != OSIP_SUCCESS)
					{
						if(ret != OSIP_NOTFOUND)
						{
							printf("%s line=%d eXosip_register_remove failed! ret=%d\n", __FUNCTION__, __LINE__, ret);
						}
					}

					Set_Reg_Id(0);
					eXosip_unlock (context_eXosip);
					context_eXosip_unlock();
				}
#endif
				

				Set_EvWaitFlg(0);

				context_eXosip_lock();
				eXosip_quit(context_eXosip);
				context_eXosip_unlock();
			}

			// 参数变化了，可能使用了不同协议，重新初始化协议栈
			ret = init_sip_protocol_stack();
	        if (ret != 0)
	        {
		       	printf("%s line=%d init_sip_protocol_stack failed \n", __FUNCTION__, __LINE__);
				#if defined(WIN32)
				Sleep(2*1000);
				#else
				sleep(2);
				#endif
				continue;
	        }

			Set_EvWaitFlg(1);

			strcpy(username, (char*)g_Cfg.local_domain_id);
			strcpy(password, g_Cfg.register_upper_server_passwd);
			
			snprintf(identity, URI_MAX_LEN, "sip:%s@%s", username, g_Cfg.local_domain_name);
			snprintf(registerer, URI_MAX_LEN, "sip:%s@%s:%d", g_Cfg.upper_server_id, g_Cfg.upper_server_ip, g_Cfg.upper_server_port);
			snprintf(contact, URI_MAX_LEN, "sip:%s@%s:%d",username, g_Cfg.local_ip, g_Cfg.local_port);

			strcpy(g_Cfg.local_domain_id, username);

		}//	参数变化处理结束

		if(/*g_Cfg.enable*/1)
		{			
			if(Get_Dev_Online() == -2)  //设备重启
			{
				break;
			}
		
			if(Get_Dev_Online() == 0)
			{	
				ret = SIPD_send_register_req(username, password, identity, registerer, contact);  //发送初始注册包
				if(ret == -1)
				{
					printf("SIPD_send_register_req err\n");
				}

				Set_Dev_Online(-1);
			}
			else if (Get_Dev_Online() == -1)
			{
				/*register response is NULL*/
				RegErrCounter();

				if(Get_RegErrCnt() == maxRegErrCnt)
				{
					Set_RegErrCnt(0);
					Set_Dev_Online(0);
				}
			}
			else
			{
				//	本域注册成功后，死循环不断发送心跳
				Set_RegErrCnt(0);
			
				for(;;)
				{					
					if(para_change() == 1)
						break;

					if(Get_Dev_Online() == -1)
					{
						Set_Dev_Online(0);
					}

					if(Get_Dev_Online() == 0)
					{
						Setkeepalive_count(0);
						break;
					}

					if(Get_Dev_Online() == -2)  //设备重启
					{
						break;
					}

					if(g_keepalive_count == (int)(g_Cfg.keepalive_internal/sleeptime)) 
					{	
						context_eXosip_lock();
						eXosip_lock (context_eXosip);
						//	 发送刷新注册信息
						ret = eXosip_register_send_register (context_eXosip, Get_Reg_Id(), NULL);
						if (0 > ret) 
						{
							printf("eXosip_register_send_register keepalive err\n");
							Set_Dev_Online(0); 
						}
						else
						{
							Set_NotifyFlg(1);
						}
						
						Setkeepalive_count(0); 
					    eXosip_unlock (context_eXosip);
						context_eXosip_unlock();
					}	

					keepalivecn_lock();
					g_keepalive_count++;
					keepalivecn_unlock();

					#if defined(WIN32)
					Sleep(sleeptime*1000);
					#else
					sleep(sleeptime);
					#endif
				}
			}
		}

		#if defined(WIN32)
		Sleep(5*1000);
		#else
		sleep(5);
		#endif
	}
}

//////////////////////////////////////////////////////////////////////////

int sipd_init(const SIPD_GATEWAY_CFG* pCfg)
{
	int ret = -1;
	HANDLE hThrd1;
	HANDLE hThrd2;
	DWORD threadId1;
	DWORD threadId2;

	InitializeCriticalSection(&sipd_variable_lock);
	InitializeCriticalSection(&sipd_almwork_lock);
	InitializeCriticalSection(&sipd_context_eXosip_lock);
	InitializeCriticalSection(&sipd_ev_wait_lock);
	InitializeCriticalSection(&sipd_isOnLine_lock);
	InitializeCriticalSection(&sipd_KeepAliveCn_lock);
	InitializeCriticalSection(&sipd_CfgChange_lock);
	InitializeCriticalSection(&sipd_regid_lock);
	InitializeCriticalSection(&sipd_notify_lock);
	InitializeCriticalSection(&sipd_logout_lock);
	InitializeCriticalSection(&sipd_RegErrCnt_lock);
	InitializeCriticalSection(&g_gw_variable_lock);

	printf("gw_init \n");

	memcpy(&g_Cfg, pCfg, sizeof(SIPD_GATEWAY_CFG));

	ret = init_sip_protocol_stack();
	if (ret != 0)
	{
		printf("gw_init init_sip_protocol_stack failed \n");
		return -1;
	}

	hThrd1  =  CreateThread(NULL,  0 , sipd_protocolevent, (LPVOID)NULL ,  0 ,  &threadId1 );
	if(NULL == hThrd1)
	{
		return -1;
	}

	//	启动注册、心跳线程
	hThrd2  =  CreateThread(NULL,  0 , SIPD_Reg_Proc, (LPVOID)NULL ,  0 ,  &threadId2 );
	if(NULL == hThrd2)
	{
		CloseHandle(hThrd1);
		return -1;
	}

	printf("gw_init successful! \n");

	return 0;
}


//////////////////////////////////////////////////////////////////////////
//	UAC 行为

int Sip_cataloginfo_request(const sipd_catalog_req * pCatalogReq)
{
	char buffer[MAX_SIPD_RECORD_INFO_LEN+1] = {0};
	memset(buffer, 0, sizeof(buffer));

	snprintf(buffer,MAX_SIPD_RECORD_INFO_LEN,
		"<?xml version=\"1.0\"?>\r\n"\
		"<Query>\r\n"\
		"<CmdType>Catalog</CmdType>\r\n"\
		"<SN>%s</SN>\r\n"\
		"<DeviceID>%s</DeviceID>\r\n"\
		"</Query>",
		pCatalogReq->SN, pCatalogReq->device_id);

	if(Get_EvWaitFlg() == 1)
	{
		osip_message_t *message = NULL;
		int ret;

		eXosip_lock(context_eXosip);
		//printf("send_recordinfo_pair_rsp to=%s from=%s \n", prsp->basic_info.to_uri, prsp->basic_info.from_uri);
		ret = eXosip_message_build_request(context_eXosip, &message, "MESSAGE",\
			pCatalogReq->to_uri, pCatalogReq->from_uri, NULL);
		if(ret != OSIP_SUCCESS)
		{
			eXosip_unlock(context_eXosip);
			printf("send_recordinfo_pair_rsp eXosip_message_build_request failed\n");
			if(message != NULL)
			{
				osip_message_free(message);
			}
			context_eXosip_unlock();
			return -1;
		}

		ret = osip_message_set_body(message, buffer, strlen(buffer));
		if(ret != OSIP_SUCCESS)
		{
			eXosip_unlock(context_eXosip);
			printf("send_recordinfo_pair_rsp osip_message_set_body failed\n");
			if(message != NULL)
			{
				osip_message_free(message);
			}
			context_eXosip_unlock();
			return -1;
		}

		ret = osip_message_set_content_type (message, "Application/MANSCDP+xml");
		if(ret != OSIP_SUCCESS)
		{
			eXosip_unlock(context_eXosip);
			printf("send_recordinfo_pair_rsp osip_message_set_content_type failed\n");
			if(message != NULL)
			{
				osip_message_free(message);
			}
			context_eXosip_unlock();
			return -1;
		}

		eXosip_message_send_request (context_eXosip, message);
		eXosip_unlock(context_eXosip);
	}
}