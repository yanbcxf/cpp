#include "gw_ref.h"
#include "osip_message.h"

static SIPD_GATEWAY_CFG g_gw_Cfg;  

int handle_gw_route(const char* request_uri, int* pResult)
{
	if(NULL == pResult)
	{
		return -1;
	}

	*pResult = 0;

	return 0;
}

int init_domain_server(void)
{
	/*init dll and get serverip and port*/
	int ret = 0;

	printf("init_domain_server \n");
	
	memset(&g_gw_Cfg, 0, sizeof(g_gw_Cfg));

	/*init dll*/
	ret = Sipd_Init();
	if(ret != 0)
	{
		printf("init_domain_server Sipd_Init [DLL]  ERROR!! exit......\n");
		return -1;
	}

	ret = Sipd_Start();
	if(ret != 0)
	{
		printf("init_domain_server Sipd_Start [DLL]  ERROR!! exit......\n");
		return -1;
	}

	/*init cfg*/
	ret = Sipd_GetSIPServerCfg(&g_gw_Cfg);
	if(ret != 0)
	{
		printf("init_domain_server Sipd_GetSIPServerCfg  ERROR!! exit......\n");
		return -1;
	}

	printf("init_domain_server done, ok\n");
	return 0;
}

int unInit_gw(void)
{
	int ret = 0;

#if 1
	ret = Sipd_Stop();
	if(ret != 0)
	{
		printf("unInit_gw Sipd_Stop [DLL]  ERROR!! Continue......\n");
		
	}

	ret = Sipd_Uninit();
	if(ret != 0)
	{
		printf("unInit_gw Sipd_Uninit [DLL]  ERROR!! Continue......\n");
		
	}
#endif

	return ret;
	
}

int gw_reinit_cfg(void)
{
	int ret = 0;
	SIPD_GATEWAY_CFG tmp;

	memset(&tmp, 0, sizeof(tmp));
	
	ret = Sipd_GetSIPServerCfg(&tmp);
	if(ret != 0)
	{
		printf("%s line=%d para change! failed! \n", __FUNCTION__, __LINE__);
		return -1;
	}

	gw_set_domain_server_cfg(&tmp);

	return 0;
}




int gw_get_domain_server_cfg_init(SIPD_GATEWAY_CFG* pCfg)
{
	if(NULL == pCfg)
	{
		return -1;
	}

	memset(pCfg, 0, sizeof(SIPD_GATEWAY_CFG));
	strncpy(pCfg->local_domain_name, g_gw_Cfg.local_domain_name, MAX_SIPD_DEVICE_ID_LEN);
	strncpy(pCfg->local_domain_id, g_gw_Cfg.local_domain_id, MAX_SIPD_DEVICE_ID_LEN);
	pCfg->local_port = g_gw_Cfg.local_port;
	strncpy(pCfg->local_ip, g_gw_Cfg.local_ip, MAX_SIPD_NORMAL_LEN);
	strncpy(pCfg->local_gateway, g_gw_Cfg.local_gateway, MAX_SIPD_NORMAL_LEN);
	strncpy(pCfg->upper_server_name, g_gw_Cfg.upper_server_name, MAX_SIPD_DEVICE_ID_LEN);
	strncpy(pCfg->upper_server_id, g_gw_Cfg.upper_server_id, MAX_SIPD_DEVICE_ID_LEN);
	strncpy(pCfg->upper_server_ip, g_gw_Cfg.upper_server_ip, MAX_SIPD_NORMAL_LEN);
	pCfg->upper_server_port = g_gw_Cfg.upper_server_port;
	strncpy(pCfg->register_upper_server_passwd, g_gw_Cfg.register_upper_server_passwd, MAX_SIPD_NORMAL_LEN);
	pCfg->register_keepalive = g_gw_Cfg.register_keepalive;
	pCfg->keepalive_internal = g_gw_Cfg.keepalive_internal;
	
	
	return 0;
}




int gw_get_domain_server_cfg(SIPD_GATEWAY_CFG* pCfg)
{
	if(NULL == pCfg)
	{
		return -1;
	}

	gw_g_variable_lock();
	memset(pCfg, 0, sizeof(SIPD_GATEWAY_CFG));
	strncpy(pCfg->local_domain_name, g_gw_Cfg.local_domain_name, MAX_SIPD_DEVICE_ID_LEN);
	strncpy(pCfg->local_domain_id, g_gw_Cfg.local_domain_id, MAX_SIPD_DEVICE_ID_LEN);
	pCfg->local_port = g_gw_Cfg.local_port;
	strncpy(pCfg->local_ip, g_gw_Cfg.local_ip, MAX_SIPD_NORMAL_LEN);
	strncpy(pCfg->local_gateway, g_gw_Cfg.local_gateway, MAX_SIPD_NORMAL_LEN);
	strncpy(pCfg->upper_server_name, g_gw_Cfg.upper_server_name, MAX_SIPD_DEVICE_ID_LEN);
	strncpy(pCfg->upper_server_id, g_gw_Cfg.upper_server_id, MAX_SIPD_DEVICE_ID_LEN);
	strncpy(pCfg->upper_server_ip, g_gw_Cfg.upper_server_ip, MAX_SIPD_NORMAL_LEN);
	pCfg->upper_server_port = g_gw_Cfg.upper_server_port;
	strncpy(pCfg->register_upper_server_passwd, g_gw_Cfg.register_upper_server_passwd, MAX_SIPD_NORMAL_LEN);
	pCfg->register_keepalive = g_gw_Cfg.register_keepalive;
	pCfg->keepalive_internal = g_gw_Cfg.keepalive_internal;
	gw_g_variable_unlock();
	
	return 0;
}

int gw_set_domain_server_cfg(const SIPD_GATEWAY_CFG* pCfg)
{
	if(NULL == pCfg)
	{
		return -1;
	}

	gw_g_variable_lock();
	memset(&g_gw_Cfg, 0, sizeof(g_gw_Cfg));

	strncpy(g_gw_Cfg.local_domain_name, pCfg->local_domain_name, MAX_SIPD_DEVICE_ID_LEN);
	strncpy(g_gw_Cfg.local_domain_id, pCfg->local_domain_id, MAX_SIPD_DEVICE_ID_LEN);
	g_gw_Cfg.local_port = pCfg->local_port;
	strncpy(g_gw_Cfg.local_ip, pCfg->local_ip, MAX_SIPD_NORMAL_LEN);
	strncpy(g_gw_Cfg.local_gateway, pCfg->local_gateway, MAX_SIPD_NORMAL_LEN);
	strncpy(g_gw_Cfg.upper_server_name, pCfg->upper_server_name, MAX_SIPD_DEVICE_ID_LEN);
	strncpy(g_gw_Cfg.upper_server_id, pCfg->upper_server_id, MAX_SIPD_DEVICE_ID_LEN);
	strncpy(g_gw_Cfg.upper_server_ip, pCfg->upper_server_ip, MAX_SIPD_NORMAL_LEN);
	g_gw_Cfg.upper_server_port = pCfg->upper_server_port;
	strncpy(g_gw_Cfg.register_upper_server_passwd, pCfg->register_upper_server_passwd, MAX_SIPD_NORMAL_LEN);
	g_gw_Cfg.register_keepalive = pCfg->register_keepalive;
	g_gw_Cfg.keepalive_internal = pCfg->keepalive_internal;
	gw_g_variable_unlock();
	
	return 0;
}


/*TESTCODE*/
#if 0
int Sipd_GetSIPServerCfg (SIPD_GATEWAY_CFG* pCfg)
{
	if(NULL == pCfg)
	{
		return -1;
	}

	memset(pCfg, 0, sizeof(SIPD_GATEWAY_CFG));
	strncpy(pCfg->local_domain_name, "3501040000", MAX_SIPD_DEVICE_ID_LEN);
	strncpy(pCfg->local_domain_id, "35010400002000000001", MAX_SIPD_DEVICE_ID_LEN);
	pCfg->local_port = 5062;
	strncpy(pCfg->local_ip, "192.168.178.47", MAX_SIPD_NORMAL_LEN);
	strcpy(pCfg->local_gateway, "192.168.178.1");

	
	strncpy(pCfg->upper_server_name, "3501010000", MAX_SIPD_DEVICE_ID_LEN);
	strncpy(pCfg->upper_server_id, "35010100002000000001", MAX_SIPD_DEVICE_ID_LEN);
	strncpy(pCfg->upper_server_ip, "192.168.178.47", MAX_SIPD_NORMAL_LEN);
	pCfg->upper_server_port = 5060;
	strncpy(pCfg->register_upper_server_passwd, "12345678", MAX_SIPD_NORMAL_LEN);
	pCfg->register_keepalive = 3600;
	pCfg->keepalive_internal = 60;
	return 0;
}

int Sipd_Init()
{
	return 0;
}

int Sipd_Start()
{
	return 0;
}

 int Sipd_Stop()
 {
 	return 0;
 }

 int Sipd_Uninit()
 {
 	return 0;
 }


int Sipd_Register_ServerConfig_CB (sip_config_pcb_func config_fun)
{
	return 0;
}

int Sipd_Register_AlarmInd_CB (sip_alarm_ind_pcb_func alarm_ind_fun)
{
	return 0;
}

int Sipd_settime(const sipd_id_info * pDeviceID,  const SIPD_TIME* pTime)
{
	return 0;
}

int Sipd_PTZ_Move(const sipd_ptz_control* pControl)
{
	return 0;
}

int Sipd_PTZ_Tour(const sipd_tour_control * pTourControl)
{
	return 0;
}

int Sipd_PTZ_Preset(const sipd_preset_control * pPresetControl)
{
	return 0;
}

int Sipd_PTZ_AutoScan(const sipd_autoscan_control * pAutoScanControl)
{
	return 0;
}

int Sipd_Reboot(const sipd_id_info * pIdInfo)
{
	return 0;
}

int Sipd_record_control (const sipd_record_control * pRecordControl)
{
	return 0;
}

int Sipd_alarm_control (const sipd_alarm_control * pAlarmControl)
{
	return 0;
}

 int Sipd_Register_ControlRsp_CB(sip_control_req_pcb_func control_rsp_fun)
 {
 	return 0;
 }

  int Sipd_cataloginfo_request(const sipd_catalog_req * pCatalogReq)
  {
  	return 0;
  }

  int Sipd_deviceinfo_request(const sipd_deviceinfo_req * pDeviceinfoReq)
  {
  	return 0;
  }

   int Sipd_stateinfo_request(const sipd_devicestate_req * pStateInfoReq)
   {
   	return 0;
   }

    int Sipd_Register_InfoRsp_CB(sip_info_req_pcb_func rsp_fun)
    {
    	return 0;
    }

  int Sipd_search_recordfile_req (sipd_recordinfo_req * pRecordFileReq)
  {
  	return 0;
  }

  int Sipd_Register_MediaControl_CB(sip_media_pcb_func media_control_fun)
  {
  	return 0;
  }

  int Invite_play_playback_download(const sipd_media_session* pMediaInfo)
  {
  	return 0;
  }


   int Sipd_media_control_notify_CMS(const int did, const int tid, const enum SIPD_TO_CMS_CMD_TYPE cmd_type, const sipd_playback_mans_rtsp_ind* playback_rtsp_control)
   {
   	return 0;
   }
   #endif

