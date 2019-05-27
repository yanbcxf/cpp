#include "InterfaceGlobal.h"
#include <Windows.h>
#include "GWInterface.h"
#include "InterfaceImp.h"
//#include "StandardInc.hpp"




int Sipd_Init()
{
	printf("%s\r\n",__FUNCTION__);
	if(g_pInterfaceImp==NULL)
	{
		g_pInterfaceImp = new InterfaceImp;
		g_pInterfaceImp->Init();
		return 0;
	}
	else
		return -1;
}

int Sipd_Start()
{
	printf("%s\r\n",__FUNCTION__);
	if(g_pInterfaceImp!=NULL)
	{
		int nRet = g_pInterfaceImp->Start();
		if(nRet<0)
			return -1;
		else
			return 0;
	}
	else
		return -1;
}

int Sipd_GetSIPServerCfg (SIPD_GATEWAY_CFG* pCfg)
{
	printf("Sipd_GetSIPServerCfg\r\n");
	if(g_pInterfaceImp!=NULL)
	{
		return g_pInterfaceImp->setSipServerCfg(pCfg);
	}
	else
		return -1;
	return 0;
}

int Sipd_Register_ServerConfig_CB (sip_config_pcb_func config_fun)
{
	if(g_pInterfaceImp!=NULL)
	{
		return g_pInterfaceImp->SetServerConfigCB(config_fun);
	}
	else
		return -1;
	return 0;
}

int Sipd_Register_AlarmInd_CB (sip_alarm_ind_pcb_func alarm_ind_fun)
{
	if(g_pInterfaceImp!=NULL)
	{
		return g_pInterfaceImp->SetAlarmIndCB(alarm_ind_fun);
	}
	else
		return -1;
	return 0;
}

int Sipd_settime(const sipd_id_info * pDeviceID,  const SIPD_TIME* pTime)
{
	if(g_pInterfaceImp!=NULL)
	{
		
	}
	else
		return -1;
	return 0;
}

int Sipd_PTZ_Move(const sipd_ptz_control* pControl)
{
	if(g_pInterfaceImp!=NULL)
	{
		return g_pInterfaceImp->PTZControl((void *)pControl,1);
	}
	else
		return -1;
	return 0;
}

int Sipd_PTZ_Tour(const sipd_tour_control * pTourControl)
{
	if(g_pInterfaceImp!=NULL)
	{
		return g_pInterfaceImp->PTZControl((void *)pTourControl,2);
	}
	else
		return -1;
	return 0;
}

int Sipd_PTZ_Preset(const sipd_preset_control * pPresetControl)
{
	if(g_pInterfaceImp!=NULL)
	{
		return g_pInterfaceImp->PTZControl((void *)pPresetControl,3);
	}
	else
		return -1;
	return 0;
}

int Sipd_PTZ_AutoScan(const sipd_autoscan_control * pAutoScanControl)
{
	if(g_pInterfaceImp!=NULL)
	{
		return g_pInterfaceImp->PTZControl((void *)pAutoScanControl,4);
	}
	else
		return -1;
	return 0;
}

int Sipd_Reboot(const sipd_id_info * pIdInfo)
{
	if(g_pInterfaceImp!=NULL)
	{
		return g_pInterfaceImp->RebootDevice((sipd_id_info *)pIdInfo);
	}
	else
		return -1;
	return 0;
}

int Sipd_record_control (const sipd_record_control * pRecordControl)
{
	if(g_pInterfaceImp!=NULL)
	{	
		return g_pInterfaceImp->RecordControl((sipd_record_control *) pRecordControl);
	}
	else
		return -1;
	return 0;
}

int Sipd_alarm_control (const sipd_alarm_control * pAlarmControl)
{
	if(g_pInterfaceImp!=NULL)
	{
		return g_pInterfaceImp->RequestSetDevAlarmStatus((sipd_alarm_control *)pAlarmControl);
	}
	else
		return -1;
	return 0;
}

int Sipd_Register_ControlRsp_CB(sip_control_req_pcb_func control_rsp_fun)
{
	if(g_pInterfaceImp!=NULL)
	{
		return g_pInterfaceImp->SetControlRspCB(control_rsp_fun);
	}
	else
		return -1;
	return 0;
}	

int Sipd_cataloginfo_request(const sipd_catalog_req * pCatalogReq)
{
	if(g_pInterfaceImp!=NULL)
	{
		return g_pInterfaceImp->RequestDevCataloginfo((sipd_catalog_req *)pCatalogReq);
	}
	else
		return -1;
	return 0;
}

int Sipd_deviceinfo_request(const sipd_deviceinfo_req * pDeviceinfoReq)
{
	if(g_pInterfaceImp!=NULL)
	{
		return g_pInterfaceImp->RequestDeviceInfo((sipd_deviceinfo_req *)pDeviceinfoReq);
	}
	else
		return -1;
	return 0;
}

int Sipd_stateinfo_request(const sipd_devicestate_req * pStateInfoReq)
{
	if(g_pInterfaceImp!=NULL)
	{
		return g_pInterfaceImp->RequestDeviceState((sipd_devicestate_req *)pStateInfoReq);
	}
	else
		return -1;
	return 0;
}

int Sipd_Register_InfoRsp_CB(sip_info_req_pcb_func rsp_fun)
{
	if(g_pInterfaceImp!=NULL)
	{
		return g_pInterfaceImp->SetInfoRspCB(rsp_fun);
	}
	else
		return -1;
	return 0;
}

int Sipd_search_recordfile_req (sipd_recordinfo_req * pRecordFileReq)
{
	if(g_pInterfaceImp!=NULL)
	{
		return g_pInterfaceImp->QueryRecord(pRecordFileReq);
	}
	else
		return -1;
	return 0;
}

int Sipd_Register_MediaControl_CB(sip_media_pcb_func media_control_fun)
{
	if(g_pInterfaceImp!=NULL)
	{
		return g_pInterfaceImp->SetMediaControlCB(media_control_fun);
	}
	else
		return -1;
	return 0;
}

int Invite_play_playback_download(const sipd_media_session* pMediaInfo)
{
	if(g_pInterfaceImp!=NULL)
	{
		return g_pInterfaceImp->AddMediaRequest(0,0,1,(sipd_media_session*)pMediaInfo,NULL);
	}
	else
		return -1;
	return 0;
}

int Sipd_media_control_notify_CMS(const int did, const int tid, const enum SIPD_TO_CMS_CMD_TYPE cmd_type, const sipd_playback_mans_rtsp_ind* playback_rtsp_control)
{
	if(g_pInterfaceImp!=NULL)
	{
		if(cmd_type==SIPD_TO_CMS_CMD_TYPE_START)
			return g_pInterfaceImp->AddMediaRequest(did,tid,2,NULL,NULL);
		else if(cmd_type==SIPD_TO_CMS_CMD_TYPE_STOP)
			return g_pInterfaceImp->AddMediaRequest(did,tid,4,NULL,NULL);
		else if(cmd_type==SIPD_TO_CMS_CMD_TYPE_PLAYBACK_CONTROL_RTSP)
		{
			return g_pInterfaceImp->AddMediaRequest(did,tid,3,NULL,(char *)playback_rtsp_control->rtsp_des);
		}
	}
	else
		return -1;
	return 0;
}

int Sipd_Stop()
{
	if(g_pInterfaceImp!=NULL)
	{
		delete g_pInterfaceImp;
		g_pInterfaceImp = NULL;
		return 0;
	}
	else
		return -1;
}

int Sipd_Uninit()
{
	return 0;
}