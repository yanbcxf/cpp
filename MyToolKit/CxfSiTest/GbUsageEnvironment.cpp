#include "StdAfx.h"

#include "GbUsageEnvironment.h"
#include "CatalogQuery.h"
#include "InviteOfBase.h"
#include "InviteOf3PCC.h"

#include "InvitePreviewOfClient.h"
#include "MessageTransfer.h"

CGbUsageEnvironment * CGbUsageEnvironment::instance = NULL;

CGbUsageEnvironment::CGbUsageEnvironment(void)
{
	m_ssrc_serial_preview = 0;
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

//	invite 会话开始
int	CGbUsageEnvironment::Invite_play_playback_download_cb(const sipd_media_session* pMediaInfo)
{
	getInstance()->SendMessageEx(0, msg_Invite_play_playback_download, (LPVOID)pMediaInfo, sizeof(sipd_media_session));
	return 0;
}

int	CGbUsageEnvironment::Invite_200_ok_cb(const sipd_media_session* pMediaInfo)
{
	getInstance()->SendMessageEx(0, msg_Invite_200_ok, (LPVOID)pMediaInfo, sizeof(sipd_media_session));
	return 0;
}

int	CGbUsageEnvironment::Invite_no_answer_cb(const sipd_media_session* pMediaInfo)
{
	getInstance()->SendMessageEx(0, msg_Invite_no_answer, (LPVOID)pMediaInfo, sizeof(sipd_media_session));
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

//	invite 会话中 ACK、Bye，以及回放中的各种控制
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
	Invite_200_ok_register(CGbUsageEnvironment::Invite_200_ok_cb);
	Invite_no_answer_register(CGbUsageEnvironment::Invite_no_answer_cb);
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
			//	扫描各个子域最后目录更新时间， 超过 20 秒未更新的子域，开启下一轮
			map<string, CCatalogTree *>::iterator it = m_catalog_tree.begin();
			for(; it!=m_catalog_tree.end(); it++)
			{
				CCatalogTree * pCatalogTree = it->second;
				if( IsMediaServer(pCatalogTree->m_username))
					continue;

				time_t time_now = time(NULL);
				int durration = time_now - pCatalogTree->m_last_update;

				if(durration > 5)
				{
					ss.str("");
					ss << "ThreadId = " << this->m_ThreadId << ",";
					ss << "开始扫描子域 （" << pCatalogTree->m_contact << ")  下的目录";
					//LOG4CPLUS_DEBUG(m_Logger, ss.str().c_str());

					CCatalogQuery * pCatalogQuery = new CCatalogQuery(*this);
					if(pCatalogQuery)
					{
						sipd_catalog_req req;
						memset(&req, 0 , sizeof(sipd_catalog_req));
						strncpy(req.device_id, pCatalogTree->m_username.c_str(), MAX_SIPD_DEVICE_ID_LEN);
						snprintf(req.from_uri, sizeof(req.from_uri), "sip:%s@%s:%d",\
							m_cfg.local_domain_id, m_cfg.local_ip, m_cfg.local_port);

						strncpy(req.to_uri, pCatalogTree->m_contact.c_str(), MAX_SIPD_URI);
						pCatalogQuery->send_request(&req, NULL);

						pCatalogTree->m_last_update = time(NULL);
					}
				}
			}
		}
		break;
	case msg_Sipd_register_request:
		{
			SN_MSG *pmsg = (SN_MSG *)msg->wParam;
			sipd_register_req  * pReq = (sipd_register_req  *)pmsg->para;
			string username = string(pReq->username);
			username = username.substr(1,username.length()-2);
			string realm = string(pReq->realm);
			realm = realm.substr(1,realm.length()-2);
			map<string, CCatalogTree *>::iterator it = m_catalog_tree.find(username);
			if(it==m_catalog_tree.end())
			{
				CCatalogTree * pTree = new CCatalogTree(username, realm, string(pReq->contact_uri));
				m_catalog_tree[username] = pTree;
			}
		}
		break;

	case msg_sipd_message_query_req:
		{
			SN_MSG *pmsg = (SN_MSG *)msg->wParam;
			sipd_message_query_or_response  * pReq = (sipd_message_query_or_response  *)pmsg->para;
			if( 0 /*string(m_cfg.local_domain_id)!= pReq->to_username*/)
			{
				//	不是以本域为目的地，转发到目的地
				CMessageTransfer * pRequest = new CMessageTransfer(*this);

				sipd_message_query_or_response rsp;
				memcpy(&rsp, pReq, sizeof(sipd_message_query_or_response));
				/*strcpy(rsp.to_uri, pReq->from_uri);
				strcpy(rsp.from_uri, pReq->to_uri);*/

				snprintf(rsp.from_uri, sizeof(rsp.from_uri), "sip:%s@%s:%d",\
					m_cfg.local_domain_id, m_cfg.local_ip, m_cfg.local_port);
				
				string nextHopUri = GetNextHopUri(pReq->to_username);
				if(nextHopUri.empty()==false)
				{
					strncpy(rsp.to_uri, nextHopUri.c_str(), MAX_SIPD_URI);
					pRequest->sipd_request(&rsp, NULL);
				}
				delete pRequest;

			}
			else
			{
				//	以本域为目的，对 xml 内容进行解析
				CXmlQuery * pXmlQuery = CXmlQuery::manscdp_xml_deal(*this, pReq);
				if(pXmlQuery)
				{
					pXmlQuery->handle_request();
				}
			}		
		}
		break;

	case msg_sipd_message_response_req:
		{
			SN_MSG *pmsg = (SN_MSG *)msg->wParam;
			sipd_message_query_or_response  * pReq = (sipd_message_query_or_response  *)pmsg->para;

			/*ss.str("");
			ss << "收到来自 （" << string(pReq->from_uri) << ") 的消息回复, ";
			ss << "msg_len = " << pmsg->msg_len;
			LOG4CPLUS_DEBUG(m_Logger, ss.str().c_str());*/

			if( 0 /*string(m_cfg.local_domain_id)!= pReq->to_username*/)
			{
				//	不是以本域为目的地，转发到目的地
				sipd_message_query_or_response rsp;
				memcpy(&rsp, pReq, sizeof(sipd_message_query_or_response));

				/*strcpy(rsp.to_uri, "sip:43000000002000000000@10.18.72.35");
				strcpy(rsp.from_uri, pReq->to_uri);*/

				string nextHopUri = GetNextHopUri(pReq->to_username);
				CMessageTransfer * pRequest = new CMessageTransfer(*this);
				pRequest->sipd_request(&rsp, nextHopUri.c_str());
				delete pRequest;
			}
			else
			{
				//	以本域为目的，对 xml 内容进行解析
				CGbUsageEnvironment& tttt = *this;
				CXmlQuery::manscdp_xml_deal1(*this, pReq);
				int k = 1;
				k++;
				CXmlQuery * pXmlQuery = CXmlQuery::manscdp_xml_deal(*this, pReq);
				if(pXmlQuery)
				{
					pXmlQuery->handle_response();
				}
			}
			
		}
		break;
	case msg_Invite_play_playback_download:
		{
			SN_MSG *pmsg = (SN_MSG *)msg->wParam;
			sipd_media_session * pMediaInfo = (sipd_media_session  *)pmsg->para;
			if(pMediaInfo)
			{
				string channel_id = string(pMediaInfo->channel_id);
				CInvitePreviewOfClient * pInvite = new CInvitePreviewOfClient(*this, pMediaInfo);
			}
		}
		break;
	case msg_Invite_200_ok:
		{
			SN_MSG *pmsg = (SN_MSG *)msg->wParam;
			sipd_media_session * pMediaInfo = (sipd_media_session  *)pmsg->para;
			if(pMediaInfo)
			{

				/*ss.str("");
				ss << "msg_Invite_200_ok , cid = " <<  pMediaInfo->cid;
				LOG4CPLUS_DEBUG(m_Logger, ss.str().c_str());*/

				map<int, CInviteOfBase *>::iterator it =  m_invite_preview_client.begin();
				for( ;  it!= m_invite_preview_client.end(); it++)
				{
					CInvitePreviewOfClient * pInvite = (CInvitePreviewOfClient *)it->second;
					if(pInvite->m_sipd_sdp_info_of_media.cid == pMediaInfo->cid)
					{
						pInvite->Receive_200_OK_From_Media(pMediaInfo);
						break;
					}
				}
				
				if(it==m_invite_preview_client.end())
				{
					//	寻找是否来自 “三方呼叫控制”
					map<string, CInviteOf3PCC *>::iterator it =  m_invite_3pcc.begin();
					for( ;  it!= m_invite_3pcc.end(); it++)
					{
						CInviteOf3PCC * pInvite = it->second;
						if(pInvite->m_sipd_sdp_info_of_media.cid == pMediaInfo->cid)
						{
							pInvite->Receive_200_OK_From_Media(pMediaInfo);
							break;
						}
						else if(pInvite->m_sipd_sdp_info_of_sender.cid == pMediaInfo->cid)
						{
							pInvite->Receive_200_OK_From_Sender(pMediaInfo);
							break;
						}
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
				if(m_invite_preview_client.count(pMediaInfo->cid)>0)
				{
					CInvitePreviewOfClient * pInvite = (CInvitePreviewOfClient * )m_invite_preview_client[pMediaInfo->cid];
					pInvite->Receive_ACK_From_Client(pMediaInfo);
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
				if(m_invite_preview_client.count(pMediaInfo->cid)>0)
				{
					CInvitePreviewOfClient * pInvite = (CInvitePreviewOfClient * )m_invite_preview_client[pMediaInfo->cid];
					pInvite->Receive_BYE_From_Client(pMediaInfo);
				}
			}
		}
		break;

		//	发出 Bye 后，收到了 200 OK 的回复
	case msg_Invite_release:
		{
			SN_MSG *pmsg = (SN_MSG *)msg->wParam;
			sipd_media_session * pMediaInfo = (sipd_media_session  *)pmsg->para;
			if(pMediaInfo)
			{
				/*ss.str("");
				ss << "msg_Invite_release , cid = " <<  pMediaInfo->cid;
				LOG4CPLUS_DEBUG(m_Logger, ss.str().c_str());*/
				
				bool bHandled = false;
				map<int, CInviteOfBase *>::iterator it =  m_invite_preview_client.begin();
				for( ;  it!= m_invite_preview_client.end(); it++)
				{
					CInvitePreviewOfClient * pInvite = (CInvitePreviewOfClient *)it->second;
					if(pInvite->m_sipd_sdp_info_of_media.cid == pMediaInfo->cid)
					{
						pInvite->Receive_200_OK_From_Media(pMediaInfo);
						bHandled = true;
						break;
					}
				}

				if(bHandled==false)
				{
					//	寻找是否来自 “三方呼叫控制”
					map<string, CInviteOf3PCC *>::iterator it =  m_invite_3pcc.begin();
					for( ;  it!= m_invite_3pcc.end(); it++)
					{
						CInviteOf3PCC * pInvite = it->second;
						if(pInvite->m_sipd_sdp_info_of_media.cid == pMediaInfo->cid)
						{
							pInvite->Receive_200_OK_From_Media(pMediaInfo);
							break;
						}
						else if(pInvite->m_sipd_sdp_info_of_sender.cid == pMediaInfo->cid)
						{
							pInvite->Receive_200_OK_From_Sender(pMediaInfo);
							break;
						}
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


void CGbUsageEnvironment::update_catalog_from(CCatalogQuery * pCatalog)
{
	// 根据收到的 目录回复进行更新

	string from_username = GetUsernameFromUri(pCatalog->m_basic_info.from_uri);

	map<string, CCatalogTree *>::iterator it = m_catalog_tree.find(from_username);
	if(it!= m_catalog_tree.end())
	{
		CCatalogTree * pCatalogTree = it->second;
		pCatalogTree->m_last_update = time(NULL);

		
		// 对各个子目录项进行递归生成新的目录查找申请
		for(int i=0; i<pCatalog->m_basic_info.SumNum; i++)
		{
			pCatalogTree->SaveOrUpdateItem(&pCatalog->m_cata_items[i]);

			if(string(pCatalog->m_cata_items[i].ParentID)==string(pCatalog->m_catalog_req.device_id)
				&& string(pCatalog->m_catalog_req.device_id)!=string(pCatalog->m_cata_items[i].device_id))
			{
				
				CCatalogQuery * pCatalogQuery =  new CCatalogQuery(*this);
				if(pCatalogQuery)
				{
					sipd_catalog_req req;
					memset(&req, 0 , sizeof(sipd_catalog_req));
					strncpy(req.device_id, pCatalog->m_cata_items[i].device_id, MAX_SIPD_DEVICE_ID_LEN);
					snprintf(req.from_uri, sizeof(req.from_uri), "sip:%s@%s",\
						m_cfg.local_domain_id, m_cfg.local_domain_name);

					strncpy(req.to_uri, pCatalogTree->m_contact.c_str(), MAX_SIPD_URI);

					pCatalogQuery->send_request(&req, NULL);
				}
			}
			
		}
	}
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

bool CGbUsageEnvironment::IsMediaServer(string uri)
{
	int len = uri.length();
	if(len==20)
	{
		string type = uri.substr(10,3);
		if(type=="202")
			return true;
	}
	return false;
}


bool CGbUsageEnvironment::GetMediaServer(string & username, string& realm, string & contact)
{
	map<string, CCatalogTree *>::iterator it = m_catalog_tree.begin();
	for(; it!=m_catalog_tree.end(); it++)
	{
		CCatalogTree * pCatalogTree = it->second;
		if( IsMediaServer(pCatalogTree->m_username))
		{
			username = pCatalogTree->m_username;
			realm = pCatalogTree->m_realm;
			contact = pCatalogTree->m_contact;
			return true;
		}
	}
	return false;
}

string CGbUsageEnvironment::GetNextHopUri(string device_id)
{
	vector<string> firstMatch;
	if(Pcre2Grep(_T("sip\\:\\d+@"), device_id.c_str(),firstMatch )>0)
	{
		string::size_type pos1 = firstMatch[0].find("@");
		device_id = firstMatch[0].substr(4, pos1-4);
	}

	string nextHop = "";
	if(string(m_cfg.upper_server_id)== device_id)
	{
		// 查找是否指向上级域
		char uri[MAX_SIPD_URI];
		memset(uri, 0 ,MAX_SIPD_URI);
		snprintf(uri,MAX_SIPD_URI, "sip:%s@%s", m_cfg.upper_server_id, m_cfg.upper_server_ip );
		nextHop = string(uri);
	}
	else
	{
		map<string, CCatalogTree *>::iterator it =  m_catalog_tree.begin();
		for(;  it!= m_catalog_tree.end(); it++)
		{
			CCatalogTree * pTree = it->second;
			nextHop = pTree->GetNextHopUri(device_id);
			if(nextHop.empty()==false)
				break;
		}
	}

	return nextHop;
}


string CGbUsageEnvironment::fetch_ssrc_of_preview()
{
	char ssrc[11];
	memset(ssrc, 0, 11);
	int curSerail = m_ssrc_serial_preview;
	for(; m_ssrc_serial_preview < 10000;  m_ssrc_serial_preview++)
	{
		snprintf(ssrc, 10, "0%s%04d", g_strLocalDomainID.substr(4,5).c_str(), m_ssrc_serial_preview);
		if(is_ssrc_using(ssrc)==false)
		{
			m_ssrc_serial_preview++;
			if(m_ssrc_serial_preview==10000)
				m_ssrc_serial_preview = 0;
			return string(ssrc);
		}
	}

	m_ssrc_serial_preview = 0;
	for(; m_ssrc_serial_preview < curSerail; m_ssrc_serial_preview++)
	{
		snprintf(ssrc, 10, "0%s%04d", g_strLocalDomainID.substr(4,5).c_str(), m_ssrc_serial_preview);
		if(is_ssrc_using(ssrc)==false)
		{
			m_ssrc_serial_preview++;
			if(m_ssrc_serial_preview==10000)
				m_ssrc_serial_preview = 0;
			return string(ssrc);
		}
	}
	
	return "";
}

bool CGbUsageEnvironment::is_ssrc_using(string ssrc)
{
	bool result = false;
	map<string, CInviteOf3PCC *>::iterator it =  m_invite_3pcc.begin();
	for( ;  it!= m_invite_3pcc.end(); it++)
	{
		CInviteOf3PCC * pInvite = it->second;
		if(pInvite->m_ssrc_of_using == ssrc)
		{
			result = true;
			break;
		}
	}
	return result;
}