
#include "list.h"
#include "pplgwutility.h"


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
static int g_isOnline = -1;  
static int g_notifyflg = 0;
static int g_logout_flg = 0;
static int g_reg_err_cnt = 0;


static int auto_scan_speed = SIPD_DEFAULT_AUTOSCAN_SPEED;
static unsigned int device_sn = SIPD_START_DEVICE_SN;

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


static void sip_config_pcb(int* type, void* param1)
{
	sipd_ip_config_ind* ipInd=NULL;
	sipd_servercfg_config_ind*  cfgInd=NULL;
	SIPD_GATEWAY_CFG  cfg;

	//printf("sip_config_pcb \n");
	
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

		SendMessageEx(MOD_SIPD, MOD_SIPD, MSG_ID_GW_CONFIG_IND, &cfg, sizeof(cfg));
		
	}
	else
	{
		
	}
		
	
	return;
}

static void sip_alarm_ind_pcb(sipd_alarm_ind * alarm_info)
{

	//printf("sip_alarm_ind_pcb \n");

	if(NULL == alarm_info)
	{
		printf("sip_alarm_ind_pcb Invalid para! ERROR!\n");
		return;
	}

	if(SIPD_ALARM_IO == alarm_info->alarm_type)
	{
		SendMessageEx(MOD_SIPD, MOD_SIPD, MSG_ID_GW_IO_ALARM_IND, alarm_info, sizeof(sipd_alarm_ind));
	}
	else if (SIPD_ALARM_VIDEO== alarm_info->alarm_type)
	{
		SendMessageEx(MOD_SIPD, MOD_SIPD, MSG_ID_GW_MOTO_ALARM_IND, alarm_info, sizeof(sipd_alarm_ind));
	}
	else if (SIPD_ALARM_OTHER== alarm_info->alarm_type)
	{
		SendMessageEx(MOD_SIPD, MOD_SIPD, MSG_ID_GW_OTHER_ALARM_IND, alarm_info, sizeof(sipd_alarm_ind));
	}
	else
	{
		printf("sip_alarm_ind_pcb Unknow alarm type\n");
	}
	
	
	return;
}

static void sip_control_req_pcb(sipd_control_rsp* pControlRsp)
{

	//printf("sip_control_req_pcb \n");
	
	
	if(NULL == pControlRsp)
	{
		printf("sip_control_req_pcb Invalid para! ERROR!\n");
		return;
	}


	SendMessageEx(MOD_SIPD, MOD_SIPD, MSG_ID_GW_CONTROL_RSP, pControlRsp, sizeof(sipd_control_rsp));


	return;
}


static void sip_info_req_pcb(int* type, void* param1, void* param2)
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
	
	//printf("sip_info_req_pcb \n");

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
				sipd_catalog_item cata = pCataItem[i];

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

		SendMessageEx(MOD_SIPD, MOD_SIPD_CATALOG_QUERY, MSG_ID_GW_INFO_RSP_CATALOG, &catainfoRsp, sizeof(catainfoRsp));
		
	}
	else if(SIPD_INFO_RSP_CB_DEVICEINFO == *type)
	{
		pdeviceinfoRsp = (sipd_deviceinfo_rsp*)param1;
		SendMessageEx(MOD_SIPD, MOD_SIPD_QUERY, MSG_ID_GW_INFO_RSP_DEVICE, pdeviceinfoRsp, sizeof(sipd_deviceinfo_rsp));
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

		SendMessageEx(MOD_SIPD, MOD_SIPD_QUERY, MSG_ID_GW_INFO_RSP_STATE, &stateinfoRsp, sizeof(stateinfoRsp));		
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
		
		SendMessageEx(MOD_SIPD, MOD_SIPD_RECORD_QUERY, MSG_ID_GW_INFO_RSP_RECORD, &fileInfoRsp, sizeof(fileInfoRsp));		
	}
	else
	{
		printf("sip_info_req_pcb Unknow media control type\n");
	}

	
	return;
}

//	目录订阅 、报警订阅的回调函数
static void sip_subscribe_pcb(int* type, void* param1)
{
	sipd_catainfo_adaptor_rsp* psdp_info = NULL;
	sipd_alarm_ind* pSipd_alarm_ind = NULL;
	
	//printf("sip_media_pcb \n");

	
	if(NULL == type || NULL == param1)
	{
		printf("sip_subscribe_pcb [ERROR] Invalid input para, ERROR!\n");
		return 0;
	}

	if(SIPD_SUBSCRIBE_CB_CATALOG_RSP == *type)
	{
		psdp_info = (sipd_catainfo_adaptor_rsp*)param1;
		SendMessageEx(MOD_SIPD, MOD_SIPD_SUBSCRIBE, MSG_ID_GW_DESCRIBE_CATALOG_RSP, psdp_info, sizeof(sipd_catainfo_adaptor_rsp));
	
	}
	else if(SIPD_SUBSCRIBE_CB_ALARM_RSP == *type)
	{
		pSipd_alarm_ind = (sipd_alarm_ind*)param1;
		SendMessageEx(MOD_SIPD, MOD_SIPD_SUBSCRIBE, MSG_ID_GW_DESCRIBE_ALARM_RSP, pSipd_alarm_ind, sizeof(sipd_alarm_ind));
	}
	else
	{
		printf("sip_subscribe_pcb Unknow media control type\n");
	}
	
	return;
}

static void sip_media_pcb(int* type, void* param1)
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
		SendMessageEx(MOD_SIPD, MOD_SIPD, MSG_ID_GW_MEDIA_RSP, psdp_info, sizeof(sipd_sender_sdp_info));
	}
	else if(SIPD_MEDIA_CONTROL_CB_FILE_TO_END == *type)
	{
		pfile_end_ind = (sipd_media_file_end_ind*)param1;
		SendMessageEx(MOD_SIPD, MOD_SIPD, MSG_ID_GW_FILE_TO_END_IND, pfile_end_ind, sizeof(sipd_media_file_end_ind));
	}
	else
	{
		printf("sip_media_pcb Unknow media control type\n");
	}

	return;
}


int gw_register_callback(void)
{
	/*init first */
	
	Sipd_Register_ServerConfig_CB(sip_config_pcb);
	Sipd_Register_AlarmInd_CB(sip_alarm_ind_pcb);
	Sipd_Register_ControlRsp_CB(sip_control_req_pcb);
	Sipd_Register_InfoRsp_CB(sip_info_req_pcb);
	Sipd_Register_MediaControl_CB(sip_media_pcb);

	//Sipd_Register_Describle_CB(sip_subscribe_pcb);
	
	return 0;
}


DWORD WINAPI sipd_MsgHandle(void *data)
{
	MSG msg;
	char *tmp = NULL;

	PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
	
	for(;;)
	{
		if( GetMessage( &msg, 0, 0, 0) )
		{
			switch (msg.message)
			{
				case MSG_ID_GW_CONFIG_IND:  
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					SIPD_GATEWAY_CFG *pCfg = (SIPD_GATEWAY_CFG *)pmsg->para;

					printf("sipd_MsgHandle [MSG_ID_SIPD_CFG_REQ]set PRM_SIPD_CFG \n");
				
					
					//set_CMD_CFG(pCfg);
					Set_Para_Change(pCfg);

					gw_reinit_cfg();
				}
				break;

				/*RELEASE HERE*/
				#if 1
				case MSG_ID_GW_PTZ_UP_REQ:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					struct sipd_ptz_req *pReq = (struct sipd_ptz_req*)pmsg->para;
					sipd_ptz_control ptz_up;
					
					printf("sipd_MsgHandle MSG_ID_GW_PTZ_UP_REQ speed=%d id=%s \n", pReq->speed, pReq->device_id);

					memset(&ptz_up, 0, sizeof(ptz_up));

					strncpy(ptz_up.device_id, pReq->device_id, MAX_SIPD_DEVICE_ID_LEN);
					ptz_up.cmd_type = SIPD_PTZ_MOVE_UP;
					ptz_up.speed = pReq->speed;

					Sipd_PTZ_Move(&ptz_up);
					
				}
				break;

				case MSG_ID_GW_PTZ_DOWN_REQ:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					struct sipd_ptz_req *pReq = (struct sipd_ptz_req*)pmsg->para;
					sipd_ptz_control ptz_down;
					
					printf("sipd_MsgHandle MSG_ID_GW_PTZ_DOWN_REQ speed=%d  id=%s \n",  pReq->speed, pReq->device_id);
				
					memset(&ptz_down, 0, sizeof(ptz_down));

					strncpy(ptz_down.device_id, pReq->device_id, MAX_SIPD_DEVICE_ID_LEN);
					ptz_down.cmd_type = SIPD_PTZ_MOVE_DOWN;
					ptz_down.speed = pReq->speed;

					Sipd_PTZ_Move(&ptz_down);
				}
				break;

				case MSG_ID_GW_PTZ_LEFT_REQ:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					struct sipd_ptz_req *pReq = (struct sipd_ptz_req*)pmsg->para;
					sipd_ptz_control ptz_left;
					
					printf("sipd_MsgHandle MSG_ID_GW_PTZ_LEFT_REQ speed=%d id=%s\n", pReq->speed, pReq->device_id);
				
					memset(&ptz_left, 0, sizeof(ptz_left));

					strncpy(ptz_left.device_id, pReq->device_id, MAX_SIPD_DEVICE_ID_LEN);
					ptz_left.cmd_type = SIPD_PTZ_MOVE_LEFT;
					ptz_left.speed = pReq->speed;

					Sipd_PTZ_Move(&ptz_left);
				}
				break;

				case MSG_ID_GW_PTZ_RIGHT_REQ:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					struct sipd_ptz_req *pReq = (struct sipd_ptz_req*)pmsg->para;
					sipd_ptz_control ptz_right;
					
					printf("sipd_MsgHandle MSG_ID_GW_PTZ_RIGHT_REQ speed=%d id=%s\n",  pReq->speed, pReq->device_id);

					memset(&ptz_right, 0, sizeof(ptz_right));

					strncpy(ptz_right.device_id, pReq->device_id, MAX_SIPD_DEVICE_ID_LEN);
					ptz_right.cmd_type = SIPD_PTZ_MOVE_RIGHT;
					ptz_right.speed = pReq->speed;

					Sipd_PTZ_Move(&ptz_right);
				}
				break;

				case MSG_ID_GW_PTZ_RU_REQ:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					struct sipd_ptz_req *pReq = (struct sipd_ptz_req*)pmsg->para;
					sipd_ptz_control ptz_ru;
					
					printf("sipd_MsgHandle MSG_ID_GW_PTZ_RU_REQ speed=%d id=%s\n", pReq->speed, pReq->device_id);

					memset(&ptz_ru, 0, sizeof(ptz_ru));

					strncpy(ptz_ru.device_id, pReq->device_id, MAX_SIPD_DEVICE_ID_LEN);
					ptz_ru.cmd_type = SIPD_PTZ_MOVE_RU;
					ptz_ru.speed = pReq->speed;

					Sipd_PTZ_Move(&ptz_ru);
				}
				break;

				case MSG_ID_GW_PTZ_LU_REQ:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					struct sipd_ptz_req *pReq = (struct sipd_ptz_req*)pmsg->para;
					sipd_ptz_control ptz_lu;
					
					printf("sipd_MsgHandle MSG_ID_GW_PTZ_LU_REQ speed=%d id=%s\n",  pReq->speed, pReq->device_id);

					memset(&ptz_lu, 0, sizeof(ptz_lu));

					strncpy(ptz_lu.device_id, pReq->device_id, MAX_SIPD_DEVICE_ID_LEN);
					ptz_lu.cmd_type = SIPD_PTZ_MOVE_LU;
					ptz_lu.speed = pReq->speed;

					Sipd_PTZ_Move(&ptz_lu);
				}
				break;

				case MSG_ID_GW_PTZ_RD_REQ:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					struct sipd_ptz_req *pReq = (struct sipd_ptz_req*)pmsg->para;
					sipd_ptz_control ptz_rd;
					
					printf("sipd_MsgHandle MSG_ID_GW_PTZ_RD_REQ speed=%d id=%s\n", pReq->speed, pReq->device_id);

					memset(&ptz_rd, 0, sizeof(ptz_rd));

					strncpy(ptz_rd.device_id, pReq->device_id, MAX_SIPD_DEVICE_ID_LEN);
					ptz_rd.cmd_type = SIPD_PTZ_MOVE_RD;
					ptz_rd.speed = pReq->speed;

					Sipd_PTZ_Move(&ptz_rd);					
				}
				break;

				case MSG_ID_GW_PTZ_LD_REQ:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					struct sipd_ptz_req *pReq = (struct sipd_ptz_req*)pmsg->para;
					sipd_ptz_control ptz_ld;
					
					printf("sipd_MsgHandle MSG_ID_GW_PTZ_LD_REQ speed=%d  id=%s\n", pReq->speed, pReq->device_id);
				
					memset(&ptz_ld, 0, sizeof(ptz_ld));

					strncpy(ptz_ld.device_id, pReq->device_id, MAX_SIPD_DEVICE_ID_LEN);
					ptz_ld.cmd_type = SIPD_PTZ_MOVE_LD;
					ptz_ld.speed = pReq->speed;

					Sipd_PTZ_Move(&ptz_ld);	
				}
				break;

				case MSG_ID_GW_PTZ_STOP_REQ:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					struct sipd_ptz_req *pReq = (struct sipd_ptz_req*)pmsg->para;
					sipd_ptz_control pPtz;
					
					printf("sipd_MsgHandle MSG_ID_GW_PTZ_STOP_REQ speed=%d id=%s\n",  pReq->speed, pReq->device_id);
				
					memset(&pPtz, 0, sizeof(pPtz));

					strncpy(pPtz.device_id, pReq->device_id, MAX_SIPD_DEVICE_ID_LEN);
					pPtz.cmd_type = SIPD_PTZ_MOVE_STOP;
					pPtz.speed = pReq->speed;

					Sipd_PTZ_Move(&pPtz);	
				}
				break;

				case MSG_ID_GW_PTZ_FOCUS_NEAR_REQ:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					struct sipd_ptz_req *pReq = (struct sipd_ptz_req*)pmsg->para;
					sipd_ptz_control  ptz_focusnear;
					
					printf("sipd_MsgHandle MSG_ID_GW_PTZ_FOCUS_NEAR_REQ speed=%d id=%s\n", pReq->speed, pReq->device_id);

					memset(&ptz_focusnear, 0, sizeof(ptz_focusnear));

					strncpy(ptz_focusnear.device_id, pReq->device_id, MAX_SIPD_DEVICE_ID_LEN);
					ptz_focusnear.cmd_type = SIPD_PTZ_FOCUS_NEAR;
					ptz_focusnear.speed = pReq->speed;

					Sipd_PTZ_Move(&ptz_focusnear);
				}
				break;

				case MSG_ID_GW_PTZ_FOCUS_FAR_REQ:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					struct sipd_ptz_req *pReq = (struct sipd_ptz_req*)pmsg->para;
					sipd_ptz_control  ptz_focusfar;
					
					printf("sipd_MsgHandle MSG_ID_GW_PTZ_FOCUS_FAR_REQ speed=%d id=%s\n",  pReq->speed, pReq->device_id);
				
					memset(&ptz_focusfar, 0, sizeof(ptz_focusfar));

					strncpy(ptz_focusfar.device_id, pReq->device_id, MAX_SIPD_DEVICE_ID_LEN);
					ptz_focusfar.cmd_type = SIPD_PTZ_FOCUS_FAR;
					ptz_focusfar.speed = pReq->speed;

					Sipd_PTZ_Move(&ptz_focusfar);
				}
				break;

				case MSG_ID_GW_PTZ_HALLO_SMALL_REQ:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					struct sipd_ptz_req *pReq = (struct sipd_ptz_req*)pmsg->para;
					sipd_ptz_control  ptz_hallosmall;

		
					
					printf("sipd_MsgHandle MSG_ID_GW_PTZ_HALLO_SMALL_REQ speed=%d id=%s\n", pReq->speed, pReq->device_id);

					memset(&ptz_hallosmall, 0, sizeof(ptz_hallosmall));

					strncpy(ptz_hallosmall.device_id, pReq->device_id, MAX_SIPD_DEVICE_ID_LEN);
					ptz_hallosmall.cmd_type = SIPD_PTZ_HALLO_SMALL;
					ptz_hallosmall.speed = pReq->speed;

					Sipd_PTZ_Move(&ptz_hallosmall);
				}
				break;

				case MSG_ID_GW_PTZ_HALLO_LARGE_REQ:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					struct sipd_ptz_req *pReq = (struct sipd_ptz_req*)pmsg->para;
					sipd_ptz_control  ptz_hallolarge;

					printf("sipd_MsgHandle MSG_ID_GW_PTZ_HALLO_LARGE_REQ speed=%d  id=%s\n", pReq->speed, pReq->device_id);

					memset(&ptz_hallolarge, 0, sizeof(ptz_hallolarge));

					strncpy(ptz_hallolarge.device_id, pReq->device_id, MAX_SIPD_DEVICE_ID_LEN);
					ptz_hallolarge.cmd_type = SIPD_PTZ_HALLO_LARGE;
					ptz_hallolarge.speed = pReq->speed;

					Sipd_PTZ_Move(&ptz_hallolarge);
				}
				break;

				case MSG_ID_GW_PTZ_ZOOMTELE_REQ:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					struct sipd_ptz_req *pReq = (struct sipd_ptz_req*)pmsg->para;
					sipd_ptz_control ptz_zoomtele;
					
					printf("sipd_MsgHandle MSG_ID_GW_PTZ_ZOOMTELE_REQ speed=%d id=%s\n",  pReq->speed, pReq->device_id);

					memset(&ptz_zoomtele, 0, sizeof(ptz_zoomtele));

					strncpy(ptz_zoomtele.device_id, pReq->device_id, MAX_SIPD_DEVICE_ID_LEN);
					ptz_zoomtele.cmd_type = SIPD_PTZ_ZOOMTELE;
					ptz_zoomtele.speed = pReq->speed;

					Sipd_PTZ_Move(&ptz_zoomtele);
				}
				break;

				case MSG_ID_GW_PTZ_ZOOMWIDE_REQ:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					struct sipd_ptz_req *pReq = (struct sipd_ptz_req*)pmsg->para;
					sipd_ptz_control ptz_zoomwide;
					
					printf("sipd_MsgHandle MSG_ID_GW_PTZ_ZOOMWIDE_REQ speed=%d id=%s\n",  pReq->speed, pReq->device_id);

					memset(&ptz_zoomwide, 0, sizeof(ptz_zoomwide));

					strncpy(ptz_zoomwide.device_id, pReq->device_id, MAX_SIPD_DEVICE_ID_LEN);
					ptz_zoomwide.cmd_type = SIPD_PTZ_ZOOMWIDE;
					ptz_zoomwide.speed = pReq->speed;

					Sipd_PTZ_Move(&ptz_zoomwide);				
				}
				break;

				case MSG_ID_GW_TOUR_ADD_REQ:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					struct sipd_cruiseroute_req *pReq = (struct sipd_cruiseroute_req*)pmsg->para;
					sipd_tour_control ptz_req;
					
					
					printf("sipd_MsgHandle MSG_ID_GW_TOUR_ADD_REQ cruise_id=%d preset_id=%d  device_id=%s\n",  pReq->cruise_id, pReq->preset_id, pReq->device_id);

					memset(&ptz_req, 0, sizeof(ptz_req));

					strncpy(ptz_req.device_id, pReq->device_id, MAX_SIPD_DEVICE_ID_LEN);
					ptz_req.tour_cmd = SIPD_PTZ_TOUR_ADD;
					ptz_req.tour_id = pReq->cruise_id;
					ptz_req.preset_id = pReq->preset_id;
					ptz_req.time = pReq->time;
					ptz_req.speed = pReq->speed;

					Sipd_PTZ_Tour(&ptz_req);
				}
				break;

				case MSG_ID_GW_TOUR_DEL_REQ:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					struct sipd_cruiseroute_req *pReq = (struct sipd_cruiseroute_req*)pmsg->para;
					sipd_tour_control ptz_req;
					
					
					printf("sipd_MsgHandle MSG_ID_GW_TOUR_DEL_REQ cruise_id=%d preset_id=%d device_id=%s\n", pReq->cruise_id, pReq->preset_id, pReq->device_id);
					
					memset(&ptz_req, 0, sizeof(ptz_req));
					
					strncpy(ptz_req.device_id, pReq->device_id, MAX_SIPD_DEVICE_ID_LEN);
					ptz_req.tour_cmd = SIPD_PTZ_TOUR_DEL;
					ptz_req.tour_id = pReq->cruise_id;
					ptz_req.preset_id = pReq->preset_id;
					ptz_req.time = pReq->time;
					ptz_req.speed = pReq->speed;

					Sipd_PTZ_Tour(&ptz_req);					
				}
				break;

				case MSG_ID_GW_TOUR_START_REQ:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					struct sipd_cruiseroute_req *pReq = (struct sipd_cruiseroute_req*)pmsg->para;
					sipd_tour_control ptz_req;
					
					printf("sipd_MsgHandle MSG_ID_GW_TOUR_START_REQ cruise_id=%d preset_id=%d  device_id=%s\n", pReq->cruise_id, pReq->preset_id, pReq->device_id);


					memset(&ptz_req, 0, sizeof(ptz_req));

					strncpy(ptz_req.device_id, pReq->device_id, MAX_SIPD_DEVICE_ID_LEN);
					ptz_req.tour_cmd = SIPD_PTZ_TOUR_START;
					ptz_req.tour_id = pReq->cruise_id;
					ptz_req.preset_id = pReq->preset_id;
					ptz_req.time = pReq->time;
					ptz_req.speed = pReq->speed;

					Sipd_PTZ_Tour(&ptz_req);	
				}
				break;

				case MSG_ID_GW_CRUISE_STOP_REQ:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					struct sipd_cruiseroute_req *pReq = (struct sipd_cruiseroute_req*)pmsg->para;
					sipd_tour_control ptz_req;
					
					printf("sipd_MsgHandle MSG_ID_GW_CRUISE_STOP_REQ cruise_id=%d preset_id=%d  device_id=%s\n", pReq->cruise_id, pReq->preset_id, pReq->device_id);


					memset(&ptz_req, 0, sizeof(ptz_req));

					strncpy(ptz_req.device_id, pReq->device_id, MAX_SIPD_DEVICE_ID_LEN);
					ptz_req.tour_cmd = SIPD_PTZ_TOUR_STOP;
					ptz_req.tour_id = pReq->cruise_id;
					ptz_req.preset_id = pReq->preset_id;
					ptz_req.time = pReq->time;
					ptz_req.speed = pReq->speed;

					Sipd_PTZ_Tour(&ptz_req);	
				}
				break;

				case MSG_ID_GW_TOUR_SET_SPEED_REQ:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					struct sipd_cruiseroute_req *pReq = (struct sipd_cruiseroute_req*)pmsg->para;
					sipd_tour_control ptz_req;
					
					printf("sipd_MsgHandle MSG_ID_GW_TOUR_SET_SPEED_REQ cruise_id=%d speed=%d  device_id=%s\n", pReq->cruise_id, pReq->speed, pReq->device_id);

					memset(&ptz_req, 0, sizeof(ptz_req));
					
					strncpy(ptz_req.device_id, pReq->device_id, MAX_SIPD_DEVICE_ID_LEN);
					ptz_req.tour_cmd = SIPD_PTZ_TOUR_SET_SPEED;
					ptz_req.tour_id = pReq->cruise_id;
					ptz_req.preset_id = pReq->preset_id;
					ptz_req.time = pReq->time;
					ptz_req.speed = pReq->speed;

					Sipd_PTZ_Tour(&ptz_req);	
				}
				break;

				case MSG_ID_GW_TOUR_SET_STAYTIME_REQ:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					struct sipd_cruiseroute_req *pReq = (struct sipd_cruiseroute_req*)pmsg->para;
					sipd_tour_control ptz_req;
					
					printf("sipd_MsgHandle MSG_ID_GW_TOUR_SET_STAYTIME_REQ cruise_id=%d time=%d id=%s\n", pReq->cruise_id, pReq->time, pReq->device_id);

					memset(&ptz_req, 0, sizeof(ptz_req));

					strncpy(ptz_req.device_id, pReq->device_id, MAX_SIPD_DEVICE_ID_LEN);
					ptz_req.tour_cmd = SIPD_PTZ_TOUR_SET_STAYTIME;
					ptz_req.tour_id = pReq->cruise_id;
					ptz_req.preset_id = pReq->preset_id;
					ptz_req.time = pReq->time;
					ptz_req.speed = pReq->speed;

					Sipd_PTZ_Tour(&ptz_req);	
					
				}
				break;

				case MSG_ID_GW_AUTOSCAN_START_REQ:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					struct sipd_ptz_autoscan_req *pReq = (struct sipd_ptz_autoscan_req*)pmsg->para;
					sipd_autoscan_control ptz_req;
					
					printf("sipd_MsgHandle MSG_ID_GW_AUTOSCAN_START_REQ scan_id=%d  device_id=%s\n", pReq->scan_id, pReq->device_id);

					memset(&ptz_req, 0, sizeof(ptz_req));

					strncpy(ptz_req.device_id, pReq->device_id, MAX_SIPD_DEVICE_ID_LEN);
					ptz_req.cmd_type = SIPD_PTZ_AUTOSCAN_START;

					Sipd_PTZ_AutoScan(&ptz_req);
				}
				break;

				case MSG_ID_GW_PRESET_ADD_REQ:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					struct sipd_preset_req *pReq = (struct sipd_preset_req*)pmsg->para;
					sipd_preset_control ptz_req;
					
					printf("sipd_MsgHandle MSG_ID_GW_PRESET_ADD_REQ preset_id=%d  device_id=%s\n", pReq->preset_id, pReq->device_id);

					memset(&ptz_req, 0, sizeof(ptz_req));

					strncpy(ptz_req.device_id, pReq->device_id, MAX_SIPD_DEVICE_ID_LEN);
					ptz_req.cmd_type = SIPD_PTZ_PRESET_ADD;
					ptz_req.preset_id = pReq->preset_id;

					Sipd_PTZ_Preset(&ptz_req);
				}
				break;

				case MSG_ID_GW_PRESET_GOTO_REQ:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					struct sipd_preset_req *pReq = (struct sipd_preset_req*)pmsg->para;
					sipd_preset_control ptz_req;
					
					printf("sipd_MsgHandle MSG_ID_GW_PRESET_GOTO_REQ preset_id=%d device_id=%s \n", pReq->preset_id, pReq->device_id);

					memset(&ptz_req, 0, sizeof(ptz_req));

					strncpy(ptz_req.device_id, pReq->device_id, MAX_SIPD_DEVICE_ID_LEN);
					ptz_req.cmd_type = SIPD_PTZ_PRESET_GOTO;
					ptz_req.preset_id = pReq->preset_id;

					Sipd_PTZ_Preset(&ptz_req);
				}
				break;

				case MSG_ID_GW_PRESET_DEL_REQ:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					struct sipd_preset_req *pReq = (struct sipd_preset_req*)pmsg->para;
					sipd_preset_control ptz_req;
					
					printf("sipd_MsgHandle MSG_ID_GW_PRESET_DEL_REQ preset_id=%d device_id=%s\n",  pReq->preset_id, pReq->device_id);

					memset(&ptz_req, 0, sizeof(ptz_req));

					strncpy(ptz_req.device_id, pReq->device_id, MAX_SIPD_DEVICE_ID_LEN);
					ptz_req.cmd_type = SIPD_PTZ_PRESET_DEL;
					ptz_req.preset_id = pReq->preset_id;

					Sipd_PTZ_Preset(&ptz_req);
				}
				break;
				#endif /*RELEASE HERE*/

				case MSG_ID_GW_REBOOT_REQ:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					struct sipd_reboot_req *pReq = (struct sipd_reboot_req*)pmsg->para;
					sipd_id_info reboot_id;
					SIPD_GATEWAY_CFG sip_cfg;
					
					printf("sipd_MsgHandle MSG_ID_GW_REBOOT_REQ \n");

					memset(&reboot_id, 0, sizeof(reboot_id));
					strncpy(reboot_id.device_id, pReq->device_id, MAX_SIPD_DEVICE_ID_LEN);
/*设备重启，不是域服务器重启，不要发注销*/
#if 1   //设备重启之前先要注销设备
					memset(&sip_cfg, 0, sizeof(sip_cfg));
					get_PRM_SIPD_CFG(&sip_cfg);
					if(strcasecmp(sip_cfg.local_domain_id, reboot_id.device_id) == 0)
					{
						if(Get_Reg_Id() > 0)
						{
							int i=0;
							SIPD_LogOut(); 
							printf("sipd_MsgHandle MSG_ID_GW_REBOOT_REQ log_out\n");

							i=0;
							while(1)
							{
								Sleep(2*1000);

								if(Get_LogOutFlg() == 1)
								{
									break;
								}

								if(i >= 15)
								{
									printf("sipd_MsgHandle MSG_ID_GW_REBOOT_REQ wait log out too loog, don't wait any more!\n");
									i=0;
									break;
								}
								i++;
							}

							Set_LogOutFlg(0);
						}

						Set_Dev_Online(-2);//设备重启，不再重新注册
					}
#endif
					Sipd_Reboot(&reboot_id);
				}
				break;

				/*RELEASE HERE*/
				#if 1 
				case MSG_ID_GW_RECORD_CONTROL_REQ:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					struct sipd_record_req *pReq = (struct sipd_record_req*)pmsg->para;
					sipd_record_control  rsp;
					
					
					printf("sipd_MsgHandle MSG_ID_GW_RECORD_CONTROL_REQ SN=%d record_onoff=%d from=%s to=%s device_id=%s cseq=%s\n", pReq->sn, pReq->record_onOff, pReq->from, pReq->to, pReq->device_id, pReq->cseq);

					memset(&rsp, 0, sizeof(rsp));
					
					strncpy(rsp.device_id, pReq->device_id, MAX_SIPD_DEVICE_ID_LEN);
					snprintf(rsp.SN, MAX_SIPD_DEVICE_ID_LEN, "%d", pReq->sn);
					strip_from(pReq->to, rsp.from_uri);
					strip_to(pReq->from, rsp.to_uri);
					

					if(pReq->record_onOff)
					{
						/*Record*/
						rsp.cmd_type = SIPD_RECORD_CONTROL_START;
					}
					else
					{
						/*StopRecord*/
						rsp.cmd_type = SIPD_RECORD_CONTROL_STOP;
					}

					Sipd_record_control(&rsp);

					/*TESTCODE*/
					#if 0
					{
						sipd_control_rsp temp_rsp;

						memset(&temp_rsp, 0, sizeof(temp_rsp));
						strncpy(temp_rsp.from_uri, rsp.from_uri, MAX_SIPD_URI);
						strncpy(temp_rsp.to_uri, rsp.to_uri, MAX_SIPD_URI);
						strncpy(temp_rsp.SN, rsp.SN, MAX_SIPD_DEVICE_ID_LEN);
						strcpy(temp_rsp.device_id, rsp.device_id);
						temp_rsp.result = 0;

						SendMessageEx(MOD_SIPD, MOD_SIPD, MSG_ID_GW_CONTROL_RSP, &temp_rsp, sizeof(temp_rsp));
					}
					#endif
				}
				break;

				case MSG_ID_GW_CONTROL_RSP:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					int ret=0;
					
					sipd_control_rsp *rsp = (struct sipd_control_rsp*)pmsg->para;

					printf("sipd_MsgHandle MSG_ID_GW_CONTROL_RSP  result=%d device_id=%s \n", rsp->result, rsp->device_id);
					if(Get_EvWaitFlg() == 1)
					{
						ret = build_and_send_device_control_response_message(rsp->from_uri, rsp->to_uri, atoi(rsp->SN), rsp->device_id, rsp->result);
						if(ret != OSIP_SUCCESS)
						{
							printf("sipd_MsgHandle MSG_ID_GW_CONTROL_RSP build_device_control_response_message ERROR \n");
						}
					}		
				}
				break;

				case MSG_ID_GW_ALARM_GUARD_REQ:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					struct sipd_alram_guard_req *pReq = (struct sipd_alram_guard_req*)pmsg->para;
					sipd_alarm_control rsp;
					
					
					printf("sipd_MsgHandle MSG_ID_GW_ALARM_GUARD_REQ SN=%d onoff=%d from=%s to=%s device_id=%s cseq=%s\n", pReq->sn, pReq->onOff, pReq->from, pReq->to, pReq->device_id, pReq->cseq);

					memset(&rsp, 0, sizeof(rsp));
					
					strncpy(rsp.device_id, pReq->device_id, MAX_SIPD_DEVICE_ID_LEN);
					snprintf(rsp.SN, MAX_SIPD_DEVICE_ID_LEN, "%d", pReq->sn);
					strip_from(pReq->to, rsp.from_uri);
					strip_to(pReq->from, rsp.to_uri);
					

					if(pReq->onOff)
					{
						rsp.cmd_type = SIPD_ALARM_CONTROL_SET_GUARD;
					}
					else
					{
						rsp.cmd_type = SIPD_ALARM_CONTROL_RESET_GUARD;
					}

					Sipd_alarm_control(&rsp);

					/*TESTCODE*/
					#if 0
					{
						sipd_control_rsp temp_rsp;

						memset(&temp_rsp, 0, sizeof(temp_rsp));
						strncpy(temp_rsp.from_uri, rsp.from_uri, MAX_SIPD_URI);
						strncpy(temp_rsp.to_uri, rsp.to_uri, MAX_SIPD_URI);
						strncpy(temp_rsp.SN, rsp.SN, MAX_SIPD_DEVICE_ID_LEN);
						strcpy(temp_rsp.device_id, rsp.device_id);
						temp_rsp.result = 0;

						SendMessageEx(MOD_SIPD, MOD_SIPD, MSG_ID_GW_CONTROL_RSP, &temp_rsp, sizeof(temp_rsp));
					}
					#endif
				}
				break;

				case MSG_ID_GW_ALARM_CONTROL_REQ:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					struct sipd_alram_cmd_req *pReq = (struct sipd_alram_cmd_req*)pmsg->para;
					sipd_alarm_control rsp;
					
					printf("sipd_MsgHandle MSG_ID_GW_ALARM_CONTROL_REQ SN=%d reset=%d from=%s to=%s device_id=%s cseq=%s\n", pReq->sn, pReq->reset, pReq->from, pReq->to, pReq->device_id, pReq->cseq);

					memset(&rsp, 0, sizeof(rsp));
					
					strncpy(rsp.device_id, pReq->device_id, MAX_SIPD_DEVICE_ID_LEN);
					snprintf(rsp.SN, MAX_SIPD_DEVICE_ID_LEN, "%d", pReq->sn);
					strip_from(pReq->to, rsp.from_uri);
					strip_to(pReq->from, rsp.to_uri);

					rsp.cmd_type = SIPD_ALARM_CONTROL_RESET_ALARM;

					SetAlmWorkFlg(0);  
					Sipd_alarm_control(&rsp);

					/*TESTCODE*/
					#if 0
					{
						sipd_control_rsp temp_rsp;

						memset(&temp_rsp, 0, sizeof(temp_rsp));
						strncpy(temp_rsp.from_uri, rsp.from_uri, MAX_SIPD_URI);
						strncpy(temp_rsp.to_uri, rsp.to_uri, MAX_SIPD_URI);
						strncpy(temp_rsp.SN, rsp.SN, MAX_SIPD_DEVICE_ID_LEN);
						strcpy(temp_rsp.device_id, rsp.device_id);
						temp_rsp.result = 0;

						SendMessageEx(MOD_SIPD, MOD_SIPD, MSG_ID_GW_CONTROL_RSP, &temp_rsp, sizeof(temp_rsp));
					}
					#endif
				}
				break;
				
				#endif /*RELEASE HERE*/

				/*RELEASE HERE*/
				#if 1
				case MSG_ID_GW_IO_ALARM_IND:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					sipd_alarm_ind *pReq = (sipd_alarm_ind*)pmsg->para;
					char   server_sip[MAX_SIPD_URI+1] = {0};
					char   from_sip[MAX_SIPD_URI+1] = {0};
					SIPD_GATEWAY_CFG sipd_cfg;

					
					SetAlmWorkFlg(1);  
					printf("sipd_MsgHandle MSG_ID_GW_IO_ALARM_IND  device_sn=%d\n", device_sn);

					memset(server_sip, 0, sizeof(server_sip));
					memset(from_sip, 0, sizeof(from_sip));
					memset(&sipd_cfg, 0, sizeof(sipd_cfg));

					get_PRM_SIPD_CFG(&sipd_cfg);
					
					memset(server_sip, 0, sizeof(server_sip));
					sprintf(server_sip,"sip:%s@%s:%d", sipd_cfg.upper_server_id, sipd_cfg.upper_server_ip, sipd_cfg.upper_server_port);

										
					memset(from_sip, 0, sizeof(from_sip));
					//sprintf(from_sip,"sip:%s@%s:%s", pReq->device_id, sipd_cfg.local_ip, sipd_cfg.local_port);
					sprintf(from_sip,"sip:%s@%s", pReq->device_id, sipd_cfg.local_domain_name);


					printf("sipd_MsgHandle MSG_ID_GW_IO_ALARM_IND to(server_sip)=%s from_sip=%s \n", server_sip, from_sip);
					if(Get_EvWaitFlg() == 1)
					{
						build_and_send_alarm_message(from_sip, server_sip, get_device_sn(), pReq->alarm_id, pReq->alarm_priority, sipd_alarm_condition_device, NULL, pReq->alarm_time, NULL, NULL);
					}
				}
				break;

				case MSG_ID_GW_MOTO_ALARM_IND:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					sipd_alarm_ind *pReq = (sipd_alarm_ind*)pmsg->para;
					char   server_sip[MAX_SIPD_URI+1] = {0};
					char   from_sip[MAX_SIPD_URI+1] = {0};
					SIPD_GATEWAY_CFG sipd_cfg;

					
					SetAlmWorkFlg(1);  
					printf("sipd_MsgHandle MSG_ID_GW_MOTO_ALARM_IND  device_sn=%d\n", device_sn);

					memset(server_sip, 0, sizeof(server_sip));
					memset(from_sip, 0, sizeof(from_sip));
					memset(&sipd_cfg, 0, sizeof(sipd_cfg));

					get_PRM_SIPD_CFG(&sipd_cfg);
					
					memset(server_sip, 0, sizeof(server_sip));
					sprintf(server_sip,"sip:%s@%s:%d", sipd_cfg.upper_server_id, sipd_cfg.upper_server_ip, sipd_cfg.upper_server_port);

										
					memset(from_sip, 0, sizeof(from_sip));
					//sprintf(from_sip,"sip:%s@%s:%s", pReq->device_id, sipd_cfg.local_ip, sipd_cfg.local_port);
					sprintf(from_sip,"sip:%s@%s", pReq->device_id, sipd_cfg.local_domain_name);


					printf("sipd_MsgHandle MSG_ID_GW_MOTO_ALARM_IND to(server_sip)=%s from_sip=%s \n", server_sip, from_sip);
					if(Get_EvWaitFlg() == 1)
					{
						build_and_send_alarm_message(from_sip, server_sip, get_device_sn(), pReq->alarm_id, pReq->alarm_priority, sipd_alarm_condition_video, NULL, pReq->alarm_time, NULL, NULL);
					}
				}
				break;

				case MSG_ID_GW_OTHER_ALARM_IND:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					sipd_alarm_ind *pReq = (sipd_alarm_ind*)pmsg->para;
					char   server_sip[MAX_SIPD_URI+1] = {0};
					char   from_sip[MAX_SIPD_URI+1] = {0};
					SIPD_GATEWAY_CFG sipd_cfg;

					
					SetAlmWorkFlg(1);  
					printf("sipd_MsgHandle MSG_ID_GW_OTHER_ALARM_IND  device_sn=%d\n", device_sn);

					memset(server_sip, 0, sizeof(server_sip));
					memset(from_sip, 0, sizeof(from_sip));
					memset(&sipd_cfg, 0, sizeof(sipd_cfg));

					get_PRM_SIPD_CFG(&sipd_cfg);
					
					memset(server_sip, 0, sizeof(server_sip));
					sprintf(server_sip,"sip:%s@%s:%d", sipd_cfg.upper_server_id, sipd_cfg.upper_server_ip, sipd_cfg.upper_server_port);

										
					memset(from_sip, 0, sizeof(from_sip));
					//sprintf(from_sip,"sip:%s@%s:%s", pReq->device_id, sipd_cfg.local_ip, sipd_cfg.local_port);
					sprintf(from_sip,"sip:%s@%s", pReq->device_id, sipd_cfg.local_domain_name);


					printf("sipd_MsgHandle MSG_ID_GW_OTHER_ALARM_IND to(server_sip)=%s from_sip=%s \n", server_sip, from_sip);
					if(Get_EvWaitFlg() == 1)
					{
						build_and_send_alarm_message(from_sip, server_sip, get_device_sn(), pReq->alarm_id, pReq->alarm_priority, sipd_alarm_condition_others, NULL, pReq->alarm_time, NULL, NULL);
					}
				}
				break;
				#endif /*RELEASE HERE*/

				case MSG_ID_GW_MEDIA_RSP:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					sipd_sender_sdp_info	*invite_sdp_req = (sipd_sender_sdp_info *)pmsg->para;

					printf("sipd_MsgHandle MSG_ID_GW_MEDIA_RSP \n");
					
					if(Get_EvWaitFlg() == 1)
					{
						build_and_send_invite_200_ok(invite_sdp_req);
					}
				}
				break;

				case MSG_ID_GW_INVITE_CTRL_IND:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					struct sipd_invite_ctrl_ind	*invite_ctrl_req = (struct sipd_invite_ctrl_ind *)pmsg->para;

					printf("sipd_MsgHandle MSG_ID_GW_INVITE_CTRL_IND \n");

					if(SIPD_INVITE_CTRL_TYPE_START == invite_ctrl_req->cmd_type)
					{
						Sipd_media_control_notify_CMS(invite_ctrl_req->did, invite_ctrl_req->tid, SIPD_TO_CMS_CMD_TYPE_START, NULL);
					}
					else if(SIPD_INVITE_CTRL_TYPE_STOP == invite_ctrl_req->cmd_type)
					{
						Sipd_media_control_notify_CMS(invite_ctrl_req->did, invite_ctrl_req->tid, SIPD_TO_CMS_CMD_TYPE_STOP, NULL);
					}
					
				}
				break;

				case MSG_ID_GW_INVITE_INFO_IND:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					struct sipd_playback_mans_rtsp_ctrl_ind  *pmans_rtsp = (struct sipd_playback_mans_rtsp_ctrl_ind *)pmsg->para;
					sipd_playback_mans_rtsp_ind playback_ind;
					
					printf("sipd_MsgHandle MSG_ID_GW_INVITE_INFO_IND \n");
					printf("sipd_MsgHandle mans_rtsp_des=%s \n", pmans_rtsp->rtsp_des);

					memset(&playback_ind, 0, sizeof(playback_ind));
					strncpy(playback_ind.rtsp_des, pmans_rtsp->rtsp_des, MAX_SIPD_INFO_LENGTH);
					playback_ind.len = pmans_rtsp->len;

					Sipd_media_control_notify_CMS(pmans_rtsp->did, pmans_rtsp->tid, SIPD_TO_CMS_CMD_TYPE_PLAYBACK_CONTROL_RTSP, &playback_ind);
				}
				break;

				case MSG_ID_GW_FILE_TO_END_IND:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					sipd_media_file_end_ind *pReq = (sipd_media_file_end_ind*)pmsg->para;

					
					printf("sipd_MsgHandle MSG_ID_GW_FILE_TO_END_IND did=%d tid=%d\n", pReq->did, pReq->tid);
					
					if(Get_EvWaitFlg() == 1)
					{
						build_and_send_file_to_end_message(pReq->tid, pReq->did, pReq->o_id);
					}
				}
				break;

				case MSG_ID_GW_SET_TIME_REQ:  
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					Sipd_timing_req *ptm_req = (Sipd_timing_req *)pmsg->para;
					sipd_id_info id_info;

					printf("sipd_MsgHandle MSG_ID_GW_SET_TIME_REQ strlen(device_id)=%d device_id=%s\n", strlen(ptm_req->device_id), ptm_req->device_id);

					memset(&id_info, 0, sizeof(id_info));
					

					if(strlen(ptm_req->device_id) > 0)
					{
						strncpy(id_info.device_id, ptm_req->device_id, MAX_SIPD_DEVICE_ID_LEN);

						Sipd_settime(&id_info, &(ptm_req->tm));
					}
					else
					{
						Sipd_settime(NULL, &(ptm_req->tm));
					}
					
				}
				break;

				/*RELEASE HERE*/
				#if 0
				case MSG_ID_GW_DEVICEINFO_REQ:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					sipd_deviceinfo_req *pdev_info = (sipd_deviceinfo_req *)pmsg->para;

					printf("sipd_MsgHandle MSG_ID_GW_DEVICEINFO_REQ \n");

					Sipd_deviceinfo_request(pdev_info);

					/*TESTCODE*/
					#if 0
					{
						sipd_deviceinfo_rsp tmp_rsp;
						sipd_alarm_ind alarm_ind;

						memset(&tmp_rsp, 0, sizeof(tmp_rsp));
						
						strcpy(tmp_rsp.from_uri, pdev_info->from_uri);
						strcpy(tmp_rsp.to_uri, pdev_info->to_uri);
						strcpy(tmp_rsp.SN, pdev_info->SN);
						strcpy(tmp_rsp.device_id, pdev_info->device_id);
						tmp_rsp.Result = 0;
						strcpy(tmp_rsp.manufacturer, "star-net");
						strcpy(tmp_rsp.model, "model0");
						strcpy(tmp_rsp.firmware, "1.0.0.0");
						strcpy(tmp_rsp.DeviceType, "IPC");
						tmp_rsp.MaxAlarm = 1;
						tmp_rsp.MaxCamera = 1;


						memset(&alarm_ind, 0, sizeof(alarm_ind));
						strncpy(alarm_ind.device_id, "35010400001320000001", MAX_SIPD_DEVICE_ID_LEN);
						strncpy(alarm_ind.alarm_id, "35010400001340000001", MAX_SIPD_DEVICE_ID_LEN);
						alarm_ind.alarm_type = SIPD_ALARM_IO;
						alarm_ind.alarm_priority = sipd_alarm_levle_one;
						strcpy(alarm_ind.alarm_time, "2014-02-21T14:20:32");

						SendMessageEx(MOD_SIPD, MOD_SIPD, MSG_ID_GW_INFO_RSP_DEVICE, &tmp_rsp, sizeof(tmp_rsp));
						SendMessageEx(MOD_SIPD, MOD_SIPD, MSG_ID_GW_IO_ALARM_IND, &alarm_ind, sizeof(alarm_ind));
					}
					#endif
				}
				break;

				case MSG_ID_GW_INFO_RSP_DEVICE:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					sipd_deviceinfo_rsp *pdev_info_rsp = (sipd_deviceinfo_rsp *)pmsg->para;

					printf("sipd_MsgHandle MSG_ID_GW_INFO_RSP_DEVICE from=%s to=%s\n", pdev_info_rsp->from_uri, pdev_info_rsp->to_uri);

					if(Get_EvWaitFlg() == 1)
					{
						sipd_dev_info_rsp(pdev_info_rsp);
						//sipd_dev_info_rsp(pdev_info->from_uri, pdev_info->to_uri, pdev_info->SN, pdev_info->device_id);
					}
					
				}
				break;

				case MSG_ID_GW_STATEINFO_REQ:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					sipd_deviceinfo_req *pdev_info = (sipd_deviceinfo_req *)pmsg->para;
					sipd_devicestate_req state_req;	

					printf("sipd_MsgHandle MSG_ID_GW_STATEINFO_REQ \n");

					memset(&state_req, 0, sizeof(state_req));
					
					strncpy(state_req.from_uri, pdev_info->from_uri, MAX_SIPD_URI);
					strncpy(state_req.to_uri, pdev_info->to_uri, MAX_SIPD_URI);
					strncpy(state_req.SN, pdev_info->SN, MAX_SIPD_DEVICE_ID_LEN);
					strncpy(state_req.device_id, pdev_info->device_id, MAX_SIPD_DEVICE_ID_LEN);

					Sipd_stateinfo_request(&state_req);

					/*TESTCODE*/
					#if 0
					{
						sipd_stateinfo_adaptor_rsp tmp_rsp;
						sipd_alarm_ind alarm_ind;

						memset(&tmp_rsp, 0, sizeof(tmp_rsp));
						strcpy(tmp_rsp.basic_info.from_uri, state_req.from_uri);
						strcpy(tmp_rsp.basic_info.to_uri, state_req.to_uri);
						strcpy(tmp_rsp.basic_info.SN, state_req.SN);
						strcpy(tmp_rsp.basic_info.device_id, state_req.device_id);
						tmp_rsp.basic_info.Result = 0;
						tmp_rsp.basic_info.Online = 0;
						tmp_rsp.basic_info.Status = 0;
						tmp_rsp.basic_info.Encode = 0;
						tmp_rsp.basic_info.Record = 0;
						strcpy(tmp_rsp.basic_info.DeviceTime, "2014-02-21T14:25:30");
						tmp_rsp.basic_info.AlaramDeviceNum = 1;

						tmp_rsp.state_items[0].Status = 0;
						strcpy(tmp_rsp.state_items[0].device_id, "35010400001340000001");

						memset(&alarm_ind, 0, sizeof(alarm_ind));
						strncpy(alarm_ind.device_id, "35010400001320000001", MAX_SIPD_DEVICE_ID_LEN);
						strncpy(alarm_ind.alarm_id, "35010400001340000001", MAX_SIPD_DEVICE_ID_LEN);
						alarm_ind.alarm_type = SIPD_ALARM_VIDEO;
						alarm_ind.alarm_priority = sipd_alarm_levle_one;
						strcpy(alarm_ind.alarm_time, "2014-02-21T14:20:32");

						SendMessageEx(MOD_SIPD, MOD_SIPD, MSG_ID_GW_INFO_RSP_STATE, &tmp_rsp, sizeof(tmp_rsp));
						SendMessageEx(MOD_SIPD, MOD_SIPD, MSG_ID_GW_MOTO_ALARM_IND, &alarm_ind, sizeof(alarm_ind));
						
					}
					#endif
				}
				break;

				case MSG_ID_GW_INFO_RSP_STATE:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					sipd_stateinfo_adaptor_rsp *pstate_info_rsp = (sipd_stateinfo_adaptor_rsp *)pmsg->para;

					printf("sipd_MsgHandle MSG_ID_GW_INFO_RSP_STATE from=%s to=%s\n", pstate_info_rsp->basic_info.from_uri, pstate_info_rsp->basic_info.to_uri);

					if(Get_EvWaitFlg() == 1)
					{
						sipd_dev_stat_rsp(pstate_info_rsp);
						//sipd_dev_stat_rsp(pdev_info->from_uri, pdev_info->to_uri, pdev_info->SN, pdev_info->device_id);
					}
				}
				break;

				case MSG_ID_GW_CATALOGINFO_REQ:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					sipd_catalog_req *cata_req = (sipd_catalog_req *)pmsg->para;
					
					printf("sipd_MsgHandle MSG_ID_GW_CATALOGINFO_REQ \n");
					printf("sipd_MsgHandle, pdev_info->SN:%s\tpdev_info->from_uri:%s\tpdev_info->to_uri:%s\tpdev_info->device_id:%s\n", cata_req->SN, cata_req->from_uri, cata_req->to_uri, cata_req->device_id);

					Sipd_cataloginfo_request(cata_req);

					/*TESTCODE*/
					#if 0
					{
						/*构造sipd_catainfo_adaptor_rsp,发送消息MSG_ID_GW_INFO_RSP_CATALOG*/
						sipd_catainfo_adaptor_rsp catainfoRsp;
						SIPD_GATEWAY_CFG sip_cfg;
						int i=0;

						memset(&catainfoRsp, 0, sizeof(catainfoRsp));
						memset(&sip_cfg, 0, sizeof(sip_cfg));
						get_PRM_SIPD_CFG(&sip_cfg);
						printf("%s line=%d local_domain_id=%s \n", __FUNCTION__, __LINE__, sip_cfg.local_domain_id);

						strncpy(catainfoRsp.basic_info.from_uri, cata_req->from_uri, MAX_SIPD_URI);
						strncpy(catainfoRsp.basic_info.to_uri, cata_req->to_uri, MAX_SIPD_URI);
						strncpy(catainfoRsp.basic_info.SN, cata_req->SN, MAX_SIPD_DEVICE_ID_LEN);
						strncpy(catainfoRsp.basic_info.device_id, cata_req->device_id, MAX_SIPD_DEVICE_ID_LEN);
						catainfoRsp.basic_info.SumNum = 3;
						catainfoRsp.basic_info.DeviceList_Num = 3;

						/*item 0*/
						i=0;
						strncpy(catainfoRsp.cata_items[i].device_id, "35010400001320000001", MAX_SIPD_DEVICE_ID_LEN);
						strncpy(catainfoRsp.cata_items[i].Name, "Camera 1", MAX_SIPD_NORMAL_LEN);
						strncpy(catainfoRsp.cata_items[i].Manufacturer, "Star-netsecurity", MAX_SIPD_MANUFACTURER_NAME_LEN);
						strncpy(catainfoRsp.cata_items[i].Model, "model 1", MAX_SIPD_MODEL_NAME_LEN);
						strncpy(catainfoRsp.cata_items[i].Owner, "owner 1", MAX_SIPD_NORMAL_LEN);
						strncpy(catainfoRsp.cata_items[i].CivilCode, "civilcode 1", MAX_SIPD_NORMAL_LEN);
						strncpy(catainfoRsp.cata_items[i].Block, "block 1", MAX_SIPD_NORMAL_LEN);
						strncpy(catainfoRsp.cata_items[i].Address, "address 1", MAX_SIPD_URI);
						catainfoRsp.cata_items[i].Parental = 0;
						strncpy(catainfoRsp.cata_items[i].ParentID, sip_cfg.local_domain_id, MAX_SIPD_DEVICE_ID_LEN);
						catainfoRsp.cata_items[i].SafteyWay = 0;
						catainfoRsp.cata_items[i].RegisterWay = 1;
						//strncpy(catainfoRsp.cata_items[i].CertNum, pCataItem[i].CertNum, MAX_SIPD_URI);
						//catainfoRsp.cata_items[i].Certifiable = pCataItem[i].Certifiable;
						//catainfoRsp.cata_items[i].ErrCode = pCataItem[i].ErrCode;
						//strncpy(catainfoRsp.cata_items[i].EndTime, pCataItem[i].EndTime, MAX_SIPD_TIME_LEN);
						catainfoRsp.cata_items[i].Secrecy = 0;
						//strncpy(catainfoRsp.cata_items[i].IPAddress, pCataItem[i].IPAddress, MAX_SIPD_NORMAL_LEN);
						//catainfoRsp.cata_items[i].Port = pCataItem[i].Port;
						//strncpy(catainfoRsp.cata_items[i].Password, pCataItem[i].Password, MAX_SIPD_NORMAL_LEN);
						catainfoRsp.cata_items[i].Status = SIPD_STATUS_ON;
						//strncpy(catainfoRsp.cata_items[i].Longitude, pCataItem[i].Longitude, MAX_SIPD_LONGITUDE_LEN);
						//strncpy(catainfoRsp.cata_items[i].Latitude, pCataItem[i].Latitude, MAX_SIPD_LATITUDE_LEN);

						#if 1
						/*item 1*/
						i=1;
						strncpy(catainfoRsp.cata_items[i].device_id, "35010400001320000002", MAX_SIPD_DEVICE_ID_LEN);
						strncpy(catainfoRsp.cata_items[i].Name, "Camera 2", MAX_SIPD_NORMAL_LEN);
						strncpy(catainfoRsp.cata_items[i].Manufacturer, "Star-netsecurity", MAX_SIPD_MANUFACTURER_NAME_LEN);
						strncpy(catainfoRsp.cata_items[i].Model, "model 2", MAX_SIPD_MODEL_NAME_LEN);
						strncpy(catainfoRsp.cata_items[i].Owner, "owner 2", MAX_SIPD_NORMAL_LEN);
						strncpy(catainfoRsp.cata_items[i].CivilCode, "civilcode 2", MAX_SIPD_NORMAL_LEN);
						strncpy(catainfoRsp.cata_items[i].Block, "block 2", MAX_SIPD_NORMAL_LEN);
						strncpy(catainfoRsp.cata_items[i].Address, "address 2", MAX_SIPD_URI);
						catainfoRsp.cata_items[i].Parental = 0;
						strncpy(catainfoRsp.cata_items[i].ParentID, sip_cfg.local_domain_id, MAX_SIPD_DEVICE_ID_LEN);
						catainfoRsp.cata_items[i].SafteyWay = 0;
						catainfoRsp.cata_items[i].RegisterWay = 1;
						//strncpy(catainfoRsp.cata_items[i].CertNum, pCataItem[i].CertNum, MAX_SIPD_URI);
						//catainfoRsp.cata_items[i].Certifiable = pCataItem[i].Certifiable;
						//catainfoRsp.cata_items[i].ErrCode = pCataItem[i].ErrCode;
						//strncpy(catainfoRsp.cata_items[i].EndTime, pCataItem[i].EndTime, MAX_SIPD_TIME_LEN);
						catainfoRsp.cata_items[i].Secrecy = 0;
						//strncpy(catainfoRsp.cata_items[i].IPAddress, pCataItem[i].IPAddress, MAX_SIPD_NORMAL_LEN);
						//catainfoRsp.cata_items[i].Port = pCataItem[i].Port;
						//strncpy(catainfoRsp.cata_items[i].Password, pCataItem[i].Password, MAX_SIPD_NORMAL_LEN);
						catainfoRsp.cata_items[i].Status = SIPD_STATUS_ON;
						//strncpy(catainfoRsp.cata_items[i].Longitude, pCataItem[i].Longitude, MAX_SIPD_LONGITUDE_LEN);
						//strncpy(catainfoRsp.cata_items[i].Latitude, pCataItem[i].Latitude, MAX_SIPD_LATITUDE_LEN);

						/*item 1*/
						i=2;
						strncpy(catainfoRsp.cata_items[i].device_id, "35010400001320000003", MAX_SIPD_DEVICE_ID_LEN);
						strncpy(catainfoRsp.cata_items[i].Name, "Camera 3", MAX_SIPD_NORMAL_LEN);
						strncpy(catainfoRsp.cata_items[i].Manufacturer, "Star-netsecurity", MAX_SIPD_MANUFACTURER_NAME_LEN);
						strncpy(catainfoRsp.cata_items[i].Model, "model 3", MAX_SIPD_MODEL_NAME_LEN);
						strncpy(catainfoRsp.cata_items[i].Owner, "owner 3", MAX_SIPD_NORMAL_LEN);
						strncpy(catainfoRsp.cata_items[i].CivilCode, "civilcode 3", MAX_SIPD_NORMAL_LEN);
						strncpy(catainfoRsp.cata_items[i].Block, "block 3", MAX_SIPD_NORMAL_LEN);
						strncpy(catainfoRsp.cata_items[i].Address, "address 3", MAX_SIPD_URI);
						catainfoRsp.cata_items[i].Parental = 0;
						strncpy(catainfoRsp.cata_items[i].ParentID, sip_cfg.local_domain_id, MAX_SIPD_DEVICE_ID_LEN);
						catainfoRsp.cata_items[i].SafteyWay = 0;
						catainfoRsp.cata_items[i].RegisterWay = 1;
						//strncpy(catainfoRsp.cata_items[i].CertNum, pCataItem[i].CertNum, MAX_SIPD_URI);
						//catainfoRsp.cata_items[i].Certifiable = pCataItem[i].Certifiable;
						//catainfoRsp.cata_items[i].ErrCode = pCataItem[i].ErrCode;
						//strncpy(catainfoRsp.cata_items[i].EndTime, pCataItem[i].EndTime, MAX_SIPD_TIME_LEN);
						catainfoRsp.cata_items[i].Secrecy = 0;
						//strncpy(catainfoRsp.cata_items[i].IPAddress, pCataItem[i].IPAddress, MAX_SIPD_NORMAL_LEN);
						//catainfoRsp.cata_items[i].Port = pCataItem[i].Port;
						//strncpy(catainfoRsp.cata_items[i].Password, pCataItem[i].Password, MAX_SIPD_NORMAL_LEN);
						catainfoRsp.cata_items[i].Status = SIPD_STATUS_ON;
						//strncpy(catainfoRsp.cata_items[i].Longitude, pCataItem[i].Longitude, MAX_SIPD_LONGITUDE_LEN);
						//strncpy(catainfoRsp.cata_items[i].Latitude, pCataItem[i].Latitude, MAX_SIPD_LATITUDE_LEN);
						#endif
						
						SendMessageEx(MOD_SIPD, MOD_SIPD, MSG_ID_GW_INFO_RSP_CATALOG, &catainfoRsp, sizeof(catainfoRsp));
					}
					#endif
				}
				break;

				case MSG_ID_GW_INFO_RSP_CATALOG:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					sipd_catainfo_adaptor_rsp  *pcata_info_rsp = (sipd_catainfo_adaptor_rsp  *)pmsg->para;

					printf("sipd_MsgHandle MSG_ID_GW_INFO_RSP_CATALOG from=%s to=%s\n", pcata_info_rsp->basic_info.from_uri, pcata_info_rsp->basic_info.to_uri);

					if(Get_EvWaitFlg() == 1)
					{
						sipd_dev_catalog_rsp(pcata_info_rsp);
						//sipd_dev_catalog_rsp(pdev_info->from_uri, pdev_info->to_uri, pdev_info->SN, pdev_info->device_id, "Owner1", "Address 1");
					}
				}
				break;

				case MSG_ID_GW_RECORDINFO_REQ:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					sipd_recordinfo_req *precord_info = (sipd_recordinfo_req *)pmsg->para;

					printf("sipd_MsgHandle MSG_ID_GW_RECORDINFO_REQ \n");
					//printf("sipd_MsgHandle MSG_ID_GW_RECORDINFO_REQ  deviceID=%s sn=%s from_uri=%s to_uri=%s\n", precord_info->device_id, precord_info->SN, precord_info->from_uri, precord_info->to_uri);
					//printf("sipd_MsgHandle MSG_ID_GW_RECORDINFO_REQ  start_time=%s end_time=%s FilePath=%s address=%s recorderID=%s secrecy=%d type=%d\n", precord_info->StartTime, precord_info->EndTime, precord_info->FilePath, precord_info->Address, precord_info->RecorderID, precord_info->Secrecy, precord_info->Type);
					
					
					Sipd_search_recordfile_req(precord_info);

					/*TESTCODE*/
					#if 0
					{
						sipd_recordinfo_adaptor_rsp tmp_rsp;
						int i=0;
						sipd_media_file_end_ind file_to_end;

						memset(&tmp_rsp, 0, sizeof(tmp_rsp));
						strcpy(tmp_rsp.basic_info.from_uri, precord_info->from_uri);
						strcpy(tmp_rsp.basic_info.to_uri, precord_info->to_uri);
						strcpy(tmp_rsp.basic_info.SN, precord_info->SN);
						strcpy(tmp_rsp.basic_info.device_id, precord_info->device_id);
						strcpy(tmp_rsp.basic_info.Name, "IPC");
						tmp_rsp.basic_info.SumNum = 1;
						tmp_rsp.basic_info.RecordListNum = 1;

						i=0;
						strcpy(tmp_rsp.file_items[i].device_id, "35010400001320000001");
						strcpy(tmp_rsp.file_items[i].Name, "filename 1");
						strcpy(tmp_rsp.file_items[i].FilePath, "file path 1");
						strcpy(tmp_rsp.file_items[i].Address, "address 1");
						strcpy(tmp_rsp.file_items[i].StartTime, "2014-01-01T02:04:05");
						strcpy(tmp_rsp.file_items[i].EndTime, "2014-01-02T12:12:10");


						memset(&file_to_end, 0, sizeof(file_to_end));
						file_to_end.did = test_media_session.did;
						file_to_end.tid = test_media_session.tid;
						strcpy(file_to_end.o_id, test_media_session.o_id);

						SendMessageEx(MOD_SIPD, MOD_SIPD, MSG_ID_GW_INFO_RSP_RECORD, &tmp_rsp, sizeof(tmp_rsp));
						SendMessageEx(MOD_SIPD, MOD_SIPD, MSG_ID_GW_FILE_TO_END_IND, &file_to_end, sizeof(file_to_end));
					}
					#endif
					
				}
				break;

				case MSG_ID_GW_INFO_RSP_RECORD:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					sipd_recordinfo_adaptor_rsp  *precord_info_rsp = (sipd_recordinfo_adaptor_rsp  *)pmsg->para;

					printf("sipd_MsgHandle MSG_ID_GW_INFO_RSP_RECORD \n");
					if(Get_EvWaitFlg() == 1)
					{
						build_and_send_recordinfo_rsp(precord_info_rsp);
					}
					
				}
				break;
				#endif

				default:
				{
					printf("In sipd_MsgHandle,wrong msg id fail!\n");
				}
				break;
			}

			tmp = (char *)msg.wParam;
			if (tmp != NULL)
			{
				free((void *)msg.wParam);
			}
			
			Sleep(50);
		}
		else
		{
			Sleep(1000);
		}		

		
	}
}




DWORD WINAPI sipd_MsgHandle_catalogQuery(void *data)
{
	MSG msg;
	char *tmp = NULL;
	

	PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
	
	for(;;)
	{
		if( GetMessage( &msg, 0, 0, 0) )
		{
			switch (msg.message)
			{
				case MSG_ID_GW_CATALOGINFO_REQ:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					sipd_catalog_req *cata_req = (sipd_catalog_req *)pmsg->para;
					
					printf("sipd_MsgHandle MSG_ID_GW_CATALOGINFO_REQ \n");
					//printf("sipd_MsgHandle, pdev_info->SN:%s\tpdev_info->from_uri:%s\tpdev_info->to_uri:%s\tpdev_info->device_id:%s\n", cata_req->SN, cata_req->from_uri, cata_req->to_uri, cata_req->device_id);

					Sipd_cataloginfo_request(cata_req);

					/*TESTCODE*/
					#if 0
					{
						/*构造sipd_catainfo_adaptor_rsp,发送消息MSG_ID_GW_INFO_RSP_CATALOG*/
						sipd_catainfo_adaptor_rsp catainfoRsp;
						SIPD_GATEWAY_CFG sip_cfg;
						int i=0;

						memset(&catainfoRsp, 0, sizeof(catainfoRsp));
						memset(&sip_cfg, 0, sizeof(sip_cfg));
						get_PRM_SIPD_CFG(&sip_cfg);
						printf("%s line=%d local_domain_id=%s \n", __FUNCTION__, __LINE__, sip_cfg.local_domain_id);

						strncpy(catainfoRsp.basic_info.from_uri, cata_req->from_uri, MAX_SIPD_URI);
						strncpy(catainfoRsp.basic_info.to_uri, cata_req->to_uri, MAX_SIPD_URI);
						strncpy(catainfoRsp.basic_info.SN, cata_req->SN, MAX_SIPD_DEVICE_ID_LEN);
						strncpy(catainfoRsp.basic_info.device_id, cata_req->device_id, MAX_SIPD_DEVICE_ID_LEN);
						catainfoRsp.basic_info.SumNum = 3;
						catainfoRsp.basic_info.DeviceList_Num = 3;

						/*item 0*/
						i=0;
						strncpy(catainfoRsp.cata_items[i].device_id, "35010400001320000001", MAX_SIPD_DEVICE_ID_LEN);
						strncpy(catainfoRsp.cata_items[i].Name, "Camera 1", MAX_SIPD_NORMAL_LEN);
						strncpy(catainfoRsp.cata_items[i].Manufacturer, "Star-netsecurity", MAX_SIPD_MANUFACTURER_NAME_LEN);
						strncpy(catainfoRsp.cata_items[i].Model, "model 1", MAX_SIPD_MODEL_NAME_LEN);
						strncpy(catainfoRsp.cata_items[i].Owner, "owner 1", MAX_SIPD_NORMAL_LEN);
						strncpy(catainfoRsp.cata_items[i].CivilCode, "civilcode 1", MAX_SIPD_NORMAL_LEN);
						strncpy(catainfoRsp.cata_items[i].Block, "block 1", MAX_SIPD_NORMAL_LEN);
						strncpy(catainfoRsp.cata_items[i].Address, "address 1", MAX_SIPD_URI);
						catainfoRsp.cata_items[i].Parental = 0;
						strncpy(catainfoRsp.cata_items[i].ParentID, sip_cfg.local_domain_id, MAX_SIPD_DEVICE_ID_LEN);
						catainfoRsp.cata_items[i].SafteyWay = 0;
						catainfoRsp.cata_items[i].RegisterWay = 1;
						//strncpy(catainfoRsp.cata_items[i].CertNum, pCataItem[i].CertNum, MAX_SIPD_URI);
						//catainfoRsp.cata_items[i].Certifiable = pCataItem[i].Certifiable;
						//catainfoRsp.cata_items[i].ErrCode = pCataItem[i].ErrCode;
						//strncpy(catainfoRsp.cata_items[i].EndTime, pCataItem[i].EndTime, MAX_SIPD_TIME_LEN);
						catainfoRsp.cata_items[i].Secrecy = 0;
						//strncpy(catainfoRsp.cata_items[i].IPAddress, pCataItem[i].IPAddress, MAX_SIPD_NORMAL_LEN);
						//catainfoRsp.cata_items[i].Port = pCataItem[i].Port;
						//strncpy(catainfoRsp.cata_items[i].Password, pCataItem[i].Password, MAX_SIPD_NORMAL_LEN);
						catainfoRsp.cata_items[i].Status = SIPD_STATUS_ON;
						//strncpy(catainfoRsp.cata_items[i].Longitude, pCataItem[i].Longitude, MAX_SIPD_LONGITUDE_LEN);
						//strncpy(catainfoRsp.cata_items[i].Latitude, pCataItem[i].Latitude, MAX_SIPD_LATITUDE_LEN);

						#if 1
						/*item 1*/
						i=1;
						strncpy(catainfoRsp.cata_items[i].device_id, "35010400001320000002", MAX_SIPD_DEVICE_ID_LEN);
						strncpy(catainfoRsp.cata_items[i].Name, "Camera 2", MAX_SIPD_NORMAL_LEN);
						strncpy(catainfoRsp.cata_items[i].Manufacturer, "Star-netsecurity", MAX_SIPD_MANUFACTURER_NAME_LEN);
						strncpy(catainfoRsp.cata_items[i].Model, "model 2", MAX_SIPD_MODEL_NAME_LEN);
						strncpy(catainfoRsp.cata_items[i].Owner, "owner 2", MAX_SIPD_NORMAL_LEN);
						strncpy(catainfoRsp.cata_items[i].CivilCode, "civilcode 2", MAX_SIPD_NORMAL_LEN);
						strncpy(catainfoRsp.cata_items[i].Block, "block 2", MAX_SIPD_NORMAL_LEN);
						strncpy(catainfoRsp.cata_items[i].Address, "address 2", MAX_SIPD_URI);
						catainfoRsp.cata_items[i].Parental = 0;
						strncpy(catainfoRsp.cata_items[i].ParentID, sip_cfg.local_domain_id, MAX_SIPD_DEVICE_ID_LEN);
						catainfoRsp.cata_items[i].SafteyWay = 0;
						catainfoRsp.cata_items[i].RegisterWay = 1;
						//strncpy(catainfoRsp.cata_items[i].CertNum, pCataItem[i].CertNum, MAX_SIPD_URI);
						//catainfoRsp.cata_items[i].Certifiable = pCataItem[i].Certifiable;
						//catainfoRsp.cata_items[i].ErrCode = pCataItem[i].ErrCode;
						//strncpy(catainfoRsp.cata_items[i].EndTime, pCataItem[i].EndTime, MAX_SIPD_TIME_LEN);
						catainfoRsp.cata_items[i].Secrecy = 0;
						//strncpy(catainfoRsp.cata_items[i].IPAddress, pCataItem[i].IPAddress, MAX_SIPD_NORMAL_LEN);
						//catainfoRsp.cata_items[i].Port = pCataItem[i].Port;
						//strncpy(catainfoRsp.cata_items[i].Password, pCataItem[i].Password, MAX_SIPD_NORMAL_LEN);
						catainfoRsp.cata_items[i].Status = SIPD_STATUS_ON;
						//strncpy(catainfoRsp.cata_items[i].Longitude, pCataItem[i].Longitude, MAX_SIPD_LONGITUDE_LEN);
						//strncpy(catainfoRsp.cata_items[i].Latitude, pCataItem[i].Latitude, MAX_SIPD_LATITUDE_LEN);

						/*item 1*/
						i=2;
						strncpy(catainfoRsp.cata_items[i].device_id, "35010400001320000003", MAX_SIPD_DEVICE_ID_LEN);
						strncpy(catainfoRsp.cata_items[i].Name, "Camera 3", MAX_SIPD_NORMAL_LEN);
						strncpy(catainfoRsp.cata_items[i].Manufacturer, "Star-netsecurity", MAX_SIPD_MANUFACTURER_NAME_LEN);
						strncpy(catainfoRsp.cata_items[i].Model, "model 3", MAX_SIPD_MODEL_NAME_LEN);
						strncpy(catainfoRsp.cata_items[i].Owner, "owner 3", MAX_SIPD_NORMAL_LEN);
						strncpy(catainfoRsp.cata_items[i].CivilCode, "civilcode 3", MAX_SIPD_NORMAL_LEN);
						strncpy(catainfoRsp.cata_items[i].Block, "block 3", MAX_SIPD_NORMAL_LEN);
						strncpy(catainfoRsp.cata_items[i].Address, "address 3", MAX_SIPD_URI);
						catainfoRsp.cata_items[i].Parental = 0;
						strncpy(catainfoRsp.cata_items[i].ParentID, sip_cfg.local_domain_id, MAX_SIPD_DEVICE_ID_LEN);
						catainfoRsp.cata_items[i].SafteyWay = 0;
						catainfoRsp.cata_items[i].RegisterWay = 1;
						//strncpy(catainfoRsp.cata_items[i].CertNum, pCataItem[i].CertNum, MAX_SIPD_URI);
						//catainfoRsp.cata_items[i].Certifiable = pCataItem[i].Certifiable;
						//catainfoRsp.cata_items[i].ErrCode = pCataItem[i].ErrCode;
						//strncpy(catainfoRsp.cata_items[i].EndTime, pCataItem[i].EndTime, MAX_SIPD_TIME_LEN);
						catainfoRsp.cata_items[i].Secrecy = 0;
						//strncpy(catainfoRsp.cata_items[i].IPAddress, pCataItem[i].IPAddress, MAX_SIPD_NORMAL_LEN);
						//catainfoRsp.cata_items[i].Port = pCataItem[i].Port;
						//strncpy(catainfoRsp.cata_items[i].Password, pCataItem[i].Password, MAX_SIPD_NORMAL_LEN);
						catainfoRsp.cata_items[i].Status = SIPD_STATUS_ON;
						//strncpy(catainfoRsp.cata_items[i].Longitude, pCataItem[i].Longitude, MAX_SIPD_LONGITUDE_LEN);
						//strncpy(catainfoRsp.cata_items[i].Latitude, pCataItem[i].Latitude, MAX_SIPD_LATITUDE_LEN);
						#endif
						
						SendMessageEx(MOD_SIPD, MOD_SIPD, MSG_ID_GW_INFO_RSP_CATALOG, &catainfoRsp, sizeof(catainfoRsp));
					}
					#endif
				}
				break;

				case MSG_ID_GW_INFO_RSP_CATALOG:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					sipd_catainfo_adaptor_rsp  *pcata_info_rsp = (sipd_catainfo_adaptor_rsp  *)pmsg->para;

					printf("sipd_MsgHandle MSG_ID_GW_INFO_RSP_CATALOG from=%s to=%s\n", pcata_info_rsp->basic_info.from_uri, pcata_info_rsp->basic_info.to_uri);

					if(Get_EvWaitFlg() == 1)
					{
						sipd_dev_catalog_rsp(pcata_info_rsp);
						//sipd_dev_catalog_rsp(pdev_info->from_uri, pdev_info->to_uri, pdev_info->SN, pdev_info->device_id, "Owner1", "Address 1");
					}
				}
				break;

				

				default:
				{
					printf("In sipd_MsgHandle,wrong msg id fail!\n");
				}
				break;
			}

			tmp = (char *)msg.wParam;
			if (tmp != NULL)
			{
				free((void *)msg.wParam);
			}
			
			Sleep(50);
		}
		else
		{
			Sleep(1000);
		}		

		
	}
}



DWORD WINAPI sipd_MsgHandle_subscribeQuery(void *data)
{
	MSG msg;
	char *tmp = NULL;


	PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);

	for(;;)
	{
		if( GetMessage( &msg, 0, 0, 0) )
		{
			switch (msg.message)
			{
			case MSG_ID_GW_DESCRIBE_CATALOG_REQ:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;

					sipd_catalog_req *cata_req = (sipd_catalog_req *)pmsg->para;

					printf("sipd_MsgHandle MSG_ID_GW_CATALOGINFO_REQ \n");
					//printf("sipd_MsgHandle, pdev_info->SN:%s\tpdev_info->from_uri:%s\tpdev_info->to_uri:%s\tpdev_info->device_id:%s\n", cata_req->SN, cata_req->from_uri, cata_req->to_uri, cata_req->device_id);

					//Sipd_cataloginfo_request(cata_req);
					//Sipd_cataloginfo_subscribe(cata_req);

				}
				break;

			case MSG_ID_GW_DESCRIBE_CATALOG_RSP:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;

					sipd_catainfo_adaptor_rsp  *pcata_info_rsp = (sipd_catainfo_adaptor_rsp  *)pmsg->para;

					printf("sipd_MsgHandle MSG_ID_GW_DESCRIBE_CATALOG_RSP from=%s to=%s\n", pcata_info_rsp->basic_info.from_uri, pcata_info_rsp->basic_info.to_uri);

					if(Get_EvWaitFlg() == 1)
					{
						sipd_dev_catalog_notify(pcata_info_rsp);
						//sipd_dev_catalog_rsp(pdev_info->from_uri, pdev_info->to_uri, pdev_info->SN, pdev_info->device_id, "Owner1", "Address 1");
					}
				}
				break;

			case MSG_ID_GW_DESCRIBE_ALARM_REQ:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;

					sipd_alarm_req *alarm_req = (sipd_alarm_req *)pmsg->para;

					printf("sipd_MsgHandle MSG_ID_GW_DESCRIBE_ALARM_REQ \n");
					//printf("sipd_MsgHandle, pdev_info->SN:%s\tpdev_info->from_uri:%s\tpdev_info->to_uri:%s\tpdev_info->device_id:%s\n", cata_req->SN, cata_req->from_uri, cata_req->to_uri, cata_req->device_id);

					//Sipd_alarm_subscribe(alarm_req);

				}
				break;

			case MSG_ID_GW_DESCRIBE_ALARM_RSP:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;

					sipd_alarm_ind  *palarm_info_rsp = (sipd_alarm_ind  *)pmsg->para;

					//printf("sipd_MsgHandle MSG_ID_GW_DESCRIBE_ALARM_RSP from=%s to=%s\n", pcata_info_rsp->basic_info.from_uri, pcata_info_rsp->basic_info.to_uri);

					if(Get_EvWaitFlg() == 1)
					{
						build_and_send_alarm_notify(palarm_info_rsp);
						//sipd_dev_catalog_rsp(pdev_info->from_uri, pdev_info->to_uri, pdev_info->SN, pdev_info->device_id, "Owner1", "Address 1");
					}
				}
				break;

			default:
				{
					printf("In sipd_MsgHandle,wrong msg id fail!\n");
				}
				break;
			}

			tmp = (char *)msg.wParam;
			if (tmp != NULL)
			{
				free((void *)msg.wParam);
			}

			Sleep(50);
		}
		else
		{
			Sleep(1000);
		}		


	}
}


DWORD WINAPI sipd_MsgHandle_infoQuery(void *data)
{
	MSG msg;
	char *tmp = NULL;

	PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);

	for(;;)
	{
		if( GetMessage( &msg, 0, 0, 0) )
		{
			switch (msg.message)
			{
				
				/*RELEASE HERE*/
				#if 1
				case MSG_ID_GW_DEVICEINFO_REQ:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					sipd_deviceinfo_req *pdev_info = (sipd_deviceinfo_req *)pmsg->para;

					printf("sipd_MsgHandle MSG_ID_GW_DEVICEINFO_REQ \n");

					Sipd_deviceinfo_request(pdev_info);

					/*TESTCODE*/
					#if 0
					{
						sipd_deviceinfo_rsp tmp_rsp;
						sipd_alarm_ind alarm_ind;

						memset(&tmp_rsp, 0, sizeof(tmp_rsp));
						
						strcpy(tmp_rsp.from_uri, pdev_info->from_uri);
						strcpy(tmp_rsp.to_uri, pdev_info->to_uri);
						strcpy(tmp_rsp.SN, pdev_info->SN);
						strcpy(tmp_rsp.device_id, pdev_info->device_id);
						tmp_rsp.Result = 0;
						strcpy(tmp_rsp.manufacturer, "star-net");
						strcpy(tmp_rsp.model, "model0");
						strcpy(tmp_rsp.firmware, "1.0.0.0");
						strcpy(tmp_rsp.DeviceType, "IPC");
						tmp_rsp.MaxAlarm = 1;
						tmp_rsp.MaxCamera = 1;


						memset(&alarm_ind, 0, sizeof(alarm_ind));
						strncpy(alarm_ind.device_id, "35010400001320000001", MAX_SIPD_DEVICE_ID_LEN);
						strncpy(alarm_ind.alarm_id, "35010400001340000001", MAX_SIPD_DEVICE_ID_LEN);
						alarm_ind.alarm_type = SIPD_ALARM_IO;
						alarm_ind.alarm_priority = sipd_alarm_levle_one;
						strcpy(alarm_ind.alarm_time, "2014-02-21T14:20:32");

						SendMessageEx(MOD_SIPD, MOD_SIPD, MSG_ID_GW_INFO_RSP_DEVICE, &tmp_rsp, sizeof(tmp_rsp));
						SendMessageEx(MOD_SIPD, MOD_SIPD, MSG_ID_GW_IO_ALARM_IND, &alarm_ind, sizeof(alarm_ind));
					}
					#endif
				}
				break;

				case MSG_ID_GW_INFO_RSP_DEVICE:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					sipd_deviceinfo_rsp *pdev_info_rsp = (sipd_deviceinfo_rsp *)pmsg->para;

					printf("sipd_MsgHandle MSG_ID_GW_INFO_RSP_DEVICE from=%s to=%s\n", pdev_info_rsp->from_uri, pdev_info_rsp->to_uri);

					if(Get_EvWaitFlg() == 1)
					{
						sipd_dev_info_rsp(pdev_info_rsp);
						//sipd_dev_info_rsp(pdev_info->from_uri, pdev_info->to_uri, pdev_info->SN, pdev_info->device_id);
					}
					
				}
				break;

				case MSG_ID_GW_STATEINFO_REQ:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					sipd_deviceinfo_req *pdev_info = (sipd_deviceinfo_req *)pmsg->para;
					sipd_devicestate_req state_req;	

					printf("sipd_MsgHandle MSG_ID_GW_STATEINFO_REQ \n");

					memset(&state_req, 0, sizeof(state_req));
					
					strncpy(state_req.from_uri, pdev_info->from_uri, MAX_SIPD_URI);
					strncpy(state_req.to_uri, pdev_info->to_uri, MAX_SIPD_URI);
					strncpy(state_req.SN, pdev_info->SN, MAX_SIPD_DEVICE_ID_LEN);
					strncpy(state_req.device_id, pdev_info->device_id, MAX_SIPD_DEVICE_ID_LEN);

					Sipd_stateinfo_request(&state_req);

					/*TESTCODE*/
					#if 0
					{
						sipd_stateinfo_adaptor_rsp tmp_rsp;
						sipd_alarm_ind alarm_ind;

						memset(&tmp_rsp, 0, sizeof(tmp_rsp));
						strcpy(tmp_rsp.basic_info.from_uri, state_req.from_uri);
						strcpy(tmp_rsp.basic_info.to_uri, state_req.to_uri);
						strcpy(tmp_rsp.basic_info.SN, state_req.SN);
						strcpy(tmp_rsp.basic_info.device_id, state_req.device_id);
						tmp_rsp.basic_info.Result = 0;
						tmp_rsp.basic_info.Online = 0;
						tmp_rsp.basic_info.Status = 0;
						tmp_rsp.basic_info.Encode = 0;
						tmp_rsp.basic_info.Record = 0;
						strcpy(tmp_rsp.basic_info.DeviceTime, "2014-02-21T14:25:30");
						tmp_rsp.basic_info.AlaramDeviceNum = 1;

						tmp_rsp.state_items[0].Status = 0;
						strcpy(tmp_rsp.state_items[0].device_id, "35010400001340000001");

						memset(&alarm_ind, 0, sizeof(alarm_ind));
						strncpy(alarm_ind.device_id, "35010400001320000001", MAX_SIPD_DEVICE_ID_LEN);
						strncpy(alarm_ind.alarm_id, "35010400001340000001", MAX_SIPD_DEVICE_ID_LEN);
						alarm_ind.alarm_type = SIPD_ALARM_VIDEO;
						alarm_ind.alarm_priority = sipd_alarm_levle_one;
						strcpy(alarm_ind.alarm_time, "2014-02-21T14:20:32");

						SendMessageEx(MOD_SIPD, MOD_SIPD, MSG_ID_GW_INFO_RSP_STATE, &tmp_rsp, sizeof(tmp_rsp));
						SendMessageEx(MOD_SIPD, MOD_SIPD, MSG_ID_GW_MOTO_ALARM_IND, &alarm_ind, sizeof(alarm_ind));
						
					}
					#endif
				}
				break;

				case MSG_ID_GW_INFO_RSP_STATE:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					sipd_stateinfo_adaptor_rsp *pstate_info_rsp = (sipd_stateinfo_adaptor_rsp *)pmsg->para;

					printf("sipd_MsgHandle MSG_ID_GW_INFO_RSP_STATE from=%s to=%s\n", pstate_info_rsp->basic_info.from_uri, pstate_info_rsp->basic_info.to_uri);

					if(Get_EvWaitFlg() == 1)
					{
						sipd_dev_stat_rsp(pstate_info_rsp);
						//sipd_dev_stat_rsp(pdev_info->from_uri, pdev_info->to_uri, pdev_info->SN, pdev_info->device_id);
					}
				}
				break;
				#endif

				default:
				{
					printf("In sipd_MsgHandle,wrong msg id fail!\n");
				}
				break;
			}

			tmp = (char *)msg.wParam;
			if (tmp != NULL)
			{
				free((void *)msg.wParam);
			}
			
			Sleep(50);
		}
		else
		{
			Sleep(1000);
		}		

		
	}
}



DWORD WINAPI sipd_MsgHandle_recordQuery(void *data)
{
	MSG msg;
	char *tmp = NULL;

	PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);

	for(;;)
	{
		if( GetMessage( &msg, 0, 0, 0) )
		{
			switch (msg.message)
			{
				
				/*RELEASE HERE*/
				#if 1
				case MSG_ID_GW_RECORDINFO_REQ:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					sipd_recordinfo_req *precord_info = (sipd_recordinfo_req *)pmsg->para;

					printf("sipd_MsgHandle MSG_ID_GW_RECORDINFO_REQ \n");
					//printf("sipd_MsgHandle MSG_ID_GW_RECORDINFO_REQ  deviceID=%s sn=%s from_uri=%s to_uri=%s\n", precord_info->device_id, precord_info->SN, precord_info->from_uri, precord_info->to_uri);
					//printf("sipd_MsgHandle MSG_ID_GW_RECORDINFO_REQ  start_time=%s end_time=%s FilePath=%s address=%s recorderID=%s secrecy=%d type=%d\n", precord_info->StartTime, precord_info->EndTime, precord_info->FilePath, precord_info->Address, precord_info->RecorderID, precord_info->Secrecy, precord_info->Type);
					
					
					Sipd_search_recordfile_req(precord_info);

					/*TESTCODE*/
					#if 0
					{
						sipd_recordinfo_adaptor_rsp tmp_rsp;
						int i=0;
						sipd_media_file_end_ind file_to_end;

						memset(&tmp_rsp, 0, sizeof(tmp_rsp));
						strcpy(tmp_rsp.basic_info.from_uri, precord_info->from_uri);
						strcpy(tmp_rsp.basic_info.to_uri, precord_info->to_uri);
						strcpy(tmp_rsp.basic_info.SN, precord_info->SN);
						strcpy(tmp_rsp.basic_info.device_id, precord_info->device_id);
						strcpy(tmp_rsp.basic_info.Name, "IPC");
						tmp_rsp.basic_info.SumNum = 1;
						tmp_rsp.basic_info.RecordListNum = 1;

						i=0;
						strcpy(tmp_rsp.file_items[i].device_id, "35010400001320000001");
						strcpy(tmp_rsp.file_items[i].Name, "filename 1");
						strcpy(tmp_rsp.file_items[i].FilePath, "file path 1");
						strcpy(tmp_rsp.file_items[i].Address, "address 1");
						strcpy(tmp_rsp.file_items[i].StartTime, "2014-01-01T02:04:05");
						strcpy(tmp_rsp.file_items[i].EndTime, "2014-01-02T12:12:10");


						memset(&file_to_end, 0, sizeof(file_to_end));
						file_to_end.did = test_media_session.did;
						file_to_end.tid = test_media_session.tid;
						strcpy(file_to_end.o_id, test_media_session.o_id);

						SendMessageEx(MOD_SIPD, MOD_SIPD, MSG_ID_GW_INFO_RSP_RECORD, &tmp_rsp, sizeof(tmp_rsp));
						SendMessageEx(MOD_SIPD, MOD_SIPD, MSG_ID_GW_FILE_TO_END_IND, &file_to_end, sizeof(file_to_end));
					}
					#endif
					
				}
				break;

				case MSG_ID_GW_INFO_RSP_RECORD:
				{
					SN_MSG *pmsg = (SN_MSG *)msg.wParam;
					
					sipd_recordinfo_adaptor_rsp  *precord_info_rsp = (sipd_recordinfo_adaptor_rsp  *)pmsg->para;

					printf("sipd_MsgHandle MSG_ID_GW_INFO_RSP_RECORD \n");
					if(Get_EvWaitFlg() == 1)
					{
						build_and_send_recordinfo_rsp(precord_info_rsp);
					}
					
				}
				break;
				#endif

				default:
				{
					printf("In sipd_MsgHandle,wrong msg id fail!\n");
				}
				break;
			}

			tmp = (char *)msg.wParam;
			if (tmp != NULL)
			{
				free((void *)msg.wParam);
			}
			
			Sleep(50);
		}
		else
		{
			Sleep(1000);
		}		

		
	}
}



int gw_create_threads(void)
{
	int ret;
	HANDLE hThrd1;
	HANDLE hThrd2;
	HANDLE hThrd3;
	
	HANDLE hThrd4;
	HANDLE hThrd5;
	HANDLE hThrd6;
	HANDLE hThrd7;
	HANDLE hThrd8;
	
	DWORD threadId1; 
	DWORD threadId2; 
	DWORD threadId3; 

	DWORD threadId4;
	DWORD threadId5;
	DWORD threadId6;
	DWORD threadId7;
	DWORD threadId8;

	
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

	hThrd1  =  CreateThread(NULL,  0 , SIPD_Reg_Proc, (LPVOID)NULL ,  0 ,  &threadId1 );
	if(NULL == hThrd1)
	{
		printf("gw_create_threads CreateThread hThrd1 failed \n");
		return -1;
	}
	SetQueque(MOD_SIPD_REG, threadId1);

	hThrd2  =  CreateThread(NULL,  0 , sipd_MsgHandle, (LPVOID)NULL ,  0 ,  &threadId2 );
	if(NULL == hThrd2)
	{
		printf("gw_create_threads CreateThread hThrd2 failed \n");
		CloseHandle(hThrd1);
		return -1;
	}
	SetQueque(MOD_SIPD, threadId2);

	hThrd3  =  CreateThread(NULL,  0 , sipd_protocolevent, (LPVOID)NULL ,  0 ,  &threadId3 );
	if(NULL == hThrd3)
	{
		printf("gw_create_threads CreateThread hThrd3 failed \n");
		CloseHandle(hThrd1);
		CloseHandle(hThrd2);
		return -1;
	}
	SetQueque(MOD_SIPD_PROTOC, threadId3);


	hThrd4  =  CreateThread(NULL,  0 , sipd_MsgHandle_recordQuery, (LPVOID)NULL ,  0 ,  &threadId4 );
	if(NULL == hThrd4)
	{
		printf("gw_create_threads CreateThread hThrd4 failed \n");
		CloseHandle(hThrd1);
		CloseHandle(hThrd2);
		CloseHandle(hThrd3);
		return -1;
	}
	SetQueque(MOD_SIPD_RECORD_QUERY, threadId4);

	hThrd5  =  CreateThread(NULL,  0 , sipd_MsgHandle_catalogQuery, (LPVOID)NULL ,  0 ,  &threadId5 );
	if(NULL == hThrd5)
	{
		printf("gw_create_threads CreateThread hThrd5 failed \n");
		CloseHandle(hThrd1);
		CloseHandle(hThrd2);
		CloseHandle(hThrd3);
		CloseHandle(hThrd4);
		return -1;
	}
	SetQueque(MOD_SIPD_CATALOG_QUERY, threadId5);

	hThrd6  =  CreateThread(NULL,  0 , sipd_MsgHandle_infoQuery, (LPVOID)NULL ,  0 ,  &threadId6);
	if(NULL == hThrd6)
	{
		printf("gw_create_threads CreateThread hThrd6 failed \n");
		CloseHandle(hThrd1);
		CloseHandle(hThrd2);
		CloseHandle(hThrd3);
		CloseHandle(hThrd4);
		CloseHandle(hThrd5);
		return -1;
	}
	SetQueque(MOD_SIPD_QUERY, threadId6);

	//	报警订阅 、 目录订阅
	hThrd7  =  CreateThread(NULL,  0 , sipd_MsgHandle_subscribeQuery, (LPVOID)NULL ,  0 ,  &threadId7);
	if(NULL == hThrd7)
	{
		printf("gw_create_threads CreateThread hThrd6 failed \n");
		CloseHandle(hThrd1);
		CloseHandle(hThrd2);
		CloseHandle(hThrd3);
		CloseHandle(hThrd4);
		CloseHandle(hThrd5);
		CloseHandle(hThrd6);
		return -1;
	}
	SetQueque(MOD_SIPD_SUBSCRIBE, threadId7);

	//	预制点查询
	hThrd8  =  CreateThread(NULL,  0 , sipd_MsgHandle_catalogQuery, (LPVOID)NULL ,  0 ,  &threadId8);
	if(NULL == hThrd8)
	{
		printf("gw_create_threads CreateThread hThrd6 failed \n");
		CloseHandle(hThrd1);
		CloseHandle(hThrd2);
		CloseHandle(hThrd3);
		CloseHandle(hThrd4);
		CloseHandle(hThrd5);
		CloseHandle(hThrd6);
		CloseHandle(hThrd7);
		return -1;
	}
	SetQueque(MOD_SIPD_PERSET_QUERY, threadId8);

	sipd_init_ptz_variable();

	//WaitForSingleObject(hThrd1, INFINITE); 
	//WaitForSingleObject(hThrd2, INFINITE);
	//WaitForSingleObject(hThrd3, INFINITE);


	return 0;
	
}
