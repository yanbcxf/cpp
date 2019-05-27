#include "StdAfx.h"

#include "GbUsageEnvironment.h"

#include "InviteWithoutSDP.h"
#include "InviteWithSDP.h"


CGbUsageEnvironment * CGbUsageEnvironment::instance = NULL;

CGbUsageEnvironment::CGbUsageEnvironment(void)
{
	m_mtx_transfer.Init();
}

CGbUsageEnvironment::~CGbUsageEnvironment(void)
{
}

CGbUsageEnvironment* CGbUsageEnvironment::getInstance()
{
	if(instance==NULL)
		instance = new CGbUsageEnvironment;
	return instance;
}

int CGbUsageEnvironment::Sipd_cataloginfo_request_cb(const sipd_catalog_req * pCatalogReq)
{
	getInstance()->SendMessageEx(0, msg_sipd_catalog_req, (LPVOID)pCatalogReq, sizeof(sipd_catalog_req));
	return 0;
}


int CGbUsageEnvironment::Sipd_PTZ_Preset_request_cb(const sipd_preset_control * pPresetControl)
{
	getInstance()->SendMessageEx(0, msg_Sipd_PTZ_Preset_request, (LPVOID)pPresetControl, sizeof(sipd_preset_control));
	return 0;
}

int CGbUsageEnvironment::Sipd_PTZ_Tour_request_cb(const sipd_tour_control * pTourControl)
{
	getInstance()->SendMessageEx(0, msg_Sipd_PTZ_Tour_request, (LPVOID)pTourControl, sizeof(sipd_tour_control));
	return 0;
}




int  CGbUsageEnvironment::Sipd_PTZ_AutoScan_request_cb(const sipd_autoscan_control * pAutoScanControl)
{
	getInstance()->SendMessageEx(0, msg_Sipd_PTZ_AutoScan_request, (LPVOID)pAutoScanControl, sizeof(sipd_autoscan_control));
	return 0;
}

int  CGbUsageEnvironment::Sipd_PTZ_Move_request_cb(const sipd_ptz_control* pControl)
{
	getInstance()->SendMessageEx(0, msg_Sipd_PTZ_Move_request, (LPVOID)pControl, sizeof(sipd_ptz_control));
	return 0;
}

int  CGbUsageEnvironment::Sipd_record_control_request_cb(const sipd_record_control * pRecordControl)
{
	getInstance()->SendMessageEx(0, msg_Sipd_record_control_request, (LPVOID)pRecordControl, sizeof(sipd_record_control));
	return 0;
}

int  CGbUsageEnvironment::Sipd_alarm_control_request_cb(const sipd_alarm_control * pAlarmControl)
{
	getInstance()->SendMessageEx(0, msg_Sipd_alarm_control_request, (LPVOID)pAlarmControl, sizeof(sipd_alarm_control));
	return 0;
}

int  CGbUsageEnvironment::Sipd_stateinfo_request_cb(const sipd_devicestate_req * pStateInfoReq)
{
	getInstance()->SendMessageEx(0, msg_Sipd_stateinfo_request, (LPVOID)pStateInfoReq, sizeof(sipd_devicestate_req));
	return 0;
}


int  CGbUsageEnvironment::Sipd_search_recordfile_request_cb (sipd_recordinfo_req * pRecordFileReq)
{
	getInstance()->SendMessageEx(0, msg_Sipd_search_recordfile_request, (LPVOID)pRecordFileReq, sizeof(sipd_recordinfo_req));
	return 0;
}

int  CGbUsageEnvironment::Sipd_deviceinfo_request_cb(const sipd_deviceinfo_req * pDeviceinfoReq)
{
	getInstance()->SendMessageEx(0, msg_Sipd_deviceinfo_request, (LPVOID)pDeviceinfoReq, sizeof(sipd_deviceinfo_req));
	return 0;
}

//int CGbUsageEnvironment::Sipd_settime_request_cb(const sipd_id_info * pDeviceID,  const SIPD_TIME* pTime)
//{
//	getInstance()->SendMessageEx(0, msg_Sipd_PTZ_Tour_request, (LPVOID)pTourControl, sizeof(sipd_tour_control));
//}

int  CGbUsageEnvironment::Sipd_register_request_cb(const sipd_register_req * pRegisterReq)
{
	getInstance()->SendMessageEx(0, msg_Sipd_register_request, (LPVOID)pRegisterReq, sizeof(sipd_register_req));
	return 0;
}

int  CGbUsageEnvironment::sipd_message_query_request_cb(const sipd_message_query_or_response * pMessageResponseReq)
{
	getInstance()->SendMessageEx(0, msg_sipd_message_query_req, (LPVOID)pMessageResponseReq, sizeof(sipd_message_query_or_response));
	return 0;
}

int  CGbUsageEnvironment::sipd_message_response_request_cb(const sipd_message_query_or_response * pMessageResponseReq)
{
	getInstance()->SendMessageEx(0, msg_sipd_message_response_req, (LPVOID)pMessageResponseReq, sizeof(sipd_message_query_or_response));
	return 0;
}

int	 CGbUsageEnvironment::sipd_on_idle_cb()
{
	getInstance()->SendMessageEx(0, msg_sipd_on_idle_req, NULL, 0);
	return 0;
}

//	invite 会话开始， 收到 invite 请求m
int	CGbUsageEnvironment::Invite_play_playback_download_cb(const sipd_media_session* pMediaInfo)
{
	getInstance()->SendMessageEx(0, msg_Invite_play_playback_download, (LPVOID)pMediaInfo, sizeof(sipd_media_session));
	return 0;
}


int	CGbUsageEnvironment::Invite_ack_cb(const sipd_media_session* pMediaInfo)
{
	getInstance()->SendMessageEx(0, msg_Invite_ack, (LPVOID)pMediaInfo, sizeof(sipd_media_session));
	return 0;
}


int	CGbUsageEnvironment::Invite_bye_cb(const sipd_media_session* pMediaInfo)
{
	getInstance()->SendMessageEx(0, msg_Invite_bye, (LPVOID)pMediaInfo, sizeof(sipd_media_session));
	return 0;
}

int	CGbUsageEnvironment::Invite_release_cb(const sipd_media_session* pMediaInfo)
{
	getInstance()->SendMessageEx(0, msg_Invite_release, (LPVOID)pMediaInfo, sizeof(sipd_media_session));
	return 0;
}

//	invite 会话中, 收到 ACK、Bye，以及回放中的各种控制
int	CGbUsageEnvironment::Sipd_media_control_notify_CMS_cb(const int did, const int tid, const enum SIPD_TO_CMS_CMD_TYPE cmd_type,  
														  const sipd_playback_mans_rtsp_ind* playback_rtsp_control)
{
	//getInstance()->SendMessageEx(0, msg_sipd_message_response_req, (LPVOID)pMessageResponseReq, sizeof(sipd_message_query_or_response));
	return 0;
}

void CGbUsageEnvironment::init(SIPD_GATEWAY_CFG cfg)
{
	Sipd_cataloginfo_request_func_register(CGbUsageEnvironment::Sipd_cataloginfo_request_cb);
	Sipd_PTZ_Preset_func_register(CGbUsageEnvironment::Sipd_PTZ_Preset_request_cb);
	Sipd_PTZ_Tour_func_register(CGbUsageEnvironment::Sipd_PTZ_Tour_request_cb);
	Sipd_PTZ_AutoScan_func_register(CGbUsageEnvironment::Sipd_PTZ_AutoScan_request_cb);
	Sipd_PTZ_Move_func_register(CGbUsageEnvironment::Sipd_PTZ_Move_request_cb);
	Sipd_record_control_func_register(CGbUsageEnvironment::Sipd_record_control_request_cb);
	Sipd_alarm_control_func_register(CGbUsageEnvironment::Sipd_alarm_control_request_cb);
	Sipd_stateinfo_request_func_register(CGbUsageEnvironment::Sipd_stateinfo_request_cb);
	Sipd_search_recordfile_req_func_register(CGbUsageEnvironment::Sipd_search_recordfile_request_cb);
	Sipd_deviceinfo_request_func_register(CGbUsageEnvironment::Sipd_deviceinfo_request_cb);
	//Sipd_settime_func_register(CGb28181MessageQueue::Sipd_catalogin);
	Sipd_register_req_func_register(CGbUsageEnvironment::Sipd_register_request_cb);
	sipd_message_query_req_func_register(CGbUsageEnvironment::sipd_message_query_request_cb);
	sipd_message_response_req_func_register(CGbUsageEnvironment::sipd_message_response_request_cb);
	sipd_on_idle_func_register(CGbUsageEnvironment::sipd_on_idle_cb);


	Invite_play_playback_download_register(CGbUsageEnvironment::Invite_play_playback_download_cb);
	Invite_ack_register(CGbUsageEnvironment::Invite_ack_cb);
	Invite_bye_register(CGbUsageEnvironment::Invite_bye_cb);
	Invite_release_register(CGbUsageEnvironment::Invite_release_cb);

	Sipd_media_control_notify_CMS_func_register(CGbUsageEnvironment::Sipd_media_control_notify_CMS_cb);

	memcpy(&m_cfg, &cfg, sizeof(SIPD_GATEWAY_CFG));
}

void CGbUsageEnvironment::MessageHandler(MSG * msg)
{
	stringstream ss;
	
	//LOG4CPLUS_DEBUG(m_Logger, "MessageHandler......Enter");
	switch (msg->message)
	{
	case msg_sipd_on_idle_req:
		{
			
		}
		break;
	case msg_Sipd_register_request:
		{
			SN_MSG *pmsg = (SN_MSG *)msg->wParam;
			
		}
		break;

	case msg_sipd_message_query_req:
		{
			SN_MSG *pmsg = (SN_MSG *)msg->wParam;
			sipd_message_query_or_response  * pReq = (sipd_message_query_or_response  *)pmsg->para;
				
		}
		break;

	case msg_sipd_message_response_req:
		{
			SN_MSG *pmsg = (SN_MSG *)msg->wParam;
			sipd_message_query_or_response  * pReq = (sipd_message_query_or_response  *)pmsg->para;
			
		}
		break;
	case msg_Invite_play_playback_download:
		{
			SN_MSG *pmsg = (SN_MSG *)msg->wParam;
			sipd_media_session * pMediaInfo = (sipd_media_session  *)pmsg->para;
			if(pMediaInfo)
			{
				ss << "msg_Invite_play_playback_download, cid = " << pMediaInfo->cid;
				ss << ", stream_type = " << pMediaInfo->stream_type;
				LOG4CPLUS_DEBUG(m_Logger, ss.str().c_str());

				string channel_id = string(pMediaInfo->channel_id);
				if(pMediaInfo->stream_type == SIPD_STREAM_TYPE_NO_SDP)
				{
					CInviteWithoutSDP * pInvite = new CInviteWithoutSDP(*this, pMediaInfo);
					if(pInvite)
					{
						
						pInvite->Send_200_OK();
					}
				}
				else
				{
					CInviteWithSDP * pInvite = new CInviteWithSDP(*this, pMediaInfo);
					if(pInvite)
					{
						pInvite->Send_200_OK();

					}

				}
			}
		}
		break;
	case msg_Invite_ack:
		{
			SN_MSG *pmsg = (SN_MSG *)msg->wParam;
			sipd_media_session * pMediaInfo = (sipd_media_session  *)pmsg->para;
			if(pMediaInfo)
			{
				// 根据 tid 找到 Medium
				CInviteWithSDP * pInvite = (CInviteWithSDP *)FindInvite(true, pMediaInfo);
				if(pInvite)
				{
					pInvite->Receive_ACK(pMediaInfo);
				}
				else
				{
					CInviteWithoutSDP * pInvite = (CInviteWithoutSDP *)FindInvite(false, pMediaInfo);
					if(pInvite)
					{
						pInvite->Receive_ACK(pMediaInfo);
					}
				}
			}
		}
		break;

	case msg_Invite_bye:
		{
			SN_MSG *pmsg = (SN_MSG *)msg->wParam;
			sipd_media_session * pMediaInfo = (sipd_media_session  *)pmsg->para;
			if(pMediaInfo)
			{
				// 根据 tid 找到 Medium
				CInviteWithSDP * pInvite = (CInviteWithSDP *)FindInvite(true, pMediaInfo);
				if(pInvite)
				{
					pInvite->Receive_BYE(pMediaInfo);
				}
				else
				{
					CInviteWithoutSDP * pInvite = (CInviteWithoutSDP *)FindInvite(false, pMediaInfo);
					if(pInvite)
					{
						pInvite->Receive_BYE(pMediaInfo);
					}
				}
			}
		}
		break;
	default:
		break;
	}
	//LOG4CPLUS_DEBUG(m_Logger, "MessageHandler......Exit");

}


string CGbUsageEnvironment::GetUsernameFromUri(string uri)
{
	string to_username;
	vector<string> firstMatch;
	if(Pcre2Grep(_T("sip\\:\\d+@"), uri.c_str(),firstMatch )>0)
	{
		string::size_type pos1 = firstMatch[0].find("@");
		to_username = firstMatch[0].substr(4, pos1-4);
	}
	return to_username;
}

Medium * CGbUsageEnvironment::FindInvite(bool isWithSDP, const sipd_media_session * pMediaInfo)
{
	if(isWithSDP)
	{
		list<Medium *>::iterator it = m_lst_invite_with_sdp.begin();
		for(; it!= m_lst_invite_with_sdp.end(); it++)
		{
			CInviteWithSDP * pInvite = (CInviteWithSDP *)*it;
			if(pInvite->m_sipd_sdp_info.cid == pMediaInfo->cid )
			{
				return pInvite;
			}
		}
	}
	else
	{
		list<Medium *>::iterator it = m_lst_invite_without_sdp.begin();
		for(; it!= m_lst_invite_without_sdp.end(); it++)
		{
			CInviteWithoutSDP * pInvite = (CInviteWithoutSDP *)*it;
			if(pInvite->m_sipd_sdp_info.cid == pMediaInfo->cid	)
			{
				return pInvite;
			}
		}
	}
	return NULL;
}

list<sipd_sdp_info> CGbUsageEnvironment::GetTransferList(string ssrc)
{	
	JMutexAutoLock lock(m_mtx_transfer);
	list<sipd_sdp_info> lst;
	map<string, list<sipd_sdp_info>>::iterator it = m_ssrc_tranfer.find(ssrc);
	if(it!=m_ssrc_tranfer.end())
	{
		lst = it->second;
	}

	return lst;
}