#include "StdAfx.h"
#include "GbUsageEnvironment.h"
#include "CatalogQuery.h"
#include "InviteOfBase.h"
#include "InviteOf3PCC.h"

#include "InvitePreviewOfClient.h"

CInvitePreviewOfClient::CInvitePreviewOfClient(CGbUsageEnvironment& env, sipd_media_session * pMediaSession)
:CInviteOfBase(env, pMediaSession)
{
	stringstream ss;
	ss << " CInvitePreviewOfClient::构造 , cid = " << pMediaSession->cid;
	ss << ", channel_id = " << string(pMediaSession->channel_id);
	LOG4CPLUS_DEBUG(env.m_Logger, ss.str().c_str());

	env.m_invite_preview_client[pMediaSession->cid] = this;

	m_sipd_sdp_info_of_media.cid = -1;

	m_sipd_sdp_info_of_client.cid = pMediaSession->cid;
	m_sipd_sdp_info_of_client.did = pMediaSession->did;
	m_sipd_sdp_info_of_client.tid = pMediaSession->tid;
	m_sipd_sdp_info_of_client.media_sender_or_receiver_id = string(pMediaSession->o_id);
	m_sipd_sdp_info_of_client.media_send_or_receive_ip = string(pMediaSession->media_recv_ip);
	m_sipd_sdp_info_of_client.media_send_or_receive_port = atoi(pMediaSession->media_recv_port);
	m_sipd_sdp_info_of_client.media_send_or_receive_ip_type = pMediaSession->media_recv_ip_type;

	m_channel_id = string(pMediaSession->channel_id);

	/*根据 channel_id 先在Sip 服务器中查找是否已经存在对应 该 channel_id 的转发流*/

	map<string,  CInviteOf3PCC *>::iterator it = env.m_invite_3pcc.find(m_channel_id);
	if(it!=env.m_invite_3pcc.end()
		&& (it->second)->m_lst_invite.size()>0	  /* m_list_invite 队列不为空，说明还未收到 Bye 命令，处于转发流中 */
		&& (it->second)->m_lst_notify.size()==0)
	{
		//	向媒体服务器 要求转发该流
		string ssrc = (it->second)->GetSSRCOfSender();

		(it->second)->Add_New_Invite(this);
		Send_Invite_To_Media(ssrc);
	}
	else
	{
		//	向媒体服务器，要求获取它的接收信息，启动三方控制 , Step 2
		string contact_of_media;
		string username_of_media;
		string realm_of_media;
		env.GetMediaServer(username_of_media, realm_of_media, contact_of_media);
		string contact_of_sender = env.GetNextHopUri(m_channel_id);

		if(contact_of_sender.empty()==false)
		{
			CInviteOf3PCC * p3PCC = new CInviteOf3PCC(env, contact_of_media, contact_of_sender, SIPD_STREAM_TYPE_PLAY);
			if(p3PCC)
			{
				p3PCC->Add_New_Invite(this);
				p3PCC->Send_Invite_To_Media();
				p3PCC->m_lst_notify.push_back(this);
			}
		}
		else 
		{

		}
		
	}
}

CInvitePreviewOfClient::~CInvitePreviewOfClient(void)
{
	CGbUsageEnvironment&	envir1 = (CGbUsageEnvironment&)envir();

	stringstream ss;
	ss << " ~CInvitePreviewOfClient::析构 , cid = " << m_sipd_sdp_info_of_client.cid;
	ss << ", channel_id = " << m_channel_id;
	LOG4CPLUS_DEBUG(envir1.m_Logger, ss.str().c_str());

	envir1.m_invite_preview_client.erase(m_sipd_sdp_info_of_client.cid);
}

//////////////////////////////////////////////////////////////////////////
//	与客户端的通讯

void CInvitePreviewOfClient::Send_200_OK_For_Invite_To_Client()
{
	//	回复 媒体服务器的发送信息
	
	sipd_message_query_or_response  msg;
	memset(&msg, 0 , sizeof(sipd_message_query_or_response));
	msg.cid = m_sipd_sdp_info_of_client.cid;
	msg.tid = m_sipd_sdp_info_of_client.tid;
	msg.did = m_sipd_sdp_info_of_client.did;

	snprintf(msg.body,MAX_SIPD_BODY_LENGTH,
		"v=0\r\n"\
		"o=%s 0 0 IN IP4 %s\r\n"\
		"i=%s\r\n"\
		"c=IN IP4 %s\r\n"\
		"t=0 0\r\n"\
		/*"m=video %d RTP/AVP %d %d\r\n"\*/
		"m=video %d RTP/AVP %d\r\n"\
		"a=sendonly\r\n"\
		/*"a=rtpmap:%d PS/90000\r\n"\*/
		"a=rtpmap:%d PS/90000\r\n"\
		/*"a=username:%s\r\n"\*/
		/*"a=password:%s\r\n"\*/
		"y=%s\r\n" ,
		m_sipd_sdp_info_of_media.media_sender_or_receiver_id.c_str(), 
		m_sipd_sdp_info_of_media.media_send_or_receive_ip.c_str(), "GB28181 DEVICE", 
		m_sipd_sdp_info_of_media.media_send_or_receive_ip.c_str(),
		 m_sipd_sdp_info_of_media.media_send_or_receive_port, SIP_PS_H264, SIP_PS_H264, 
		 m_sipd_sdp_info_of_media.ssrc.c_str());

	build_and_send_invite_200_ok(&msg, 1);

}

void CInvitePreviewOfClient::Send_200_OK_For_Bye_To_Client()
{
	//	回复 媒体服务器的发送信息  
	sipd_message_query_or_response  msg;
	memset(&msg, 0 , sizeof(sipd_message_query_or_response));
	msg.cid = m_sipd_sdp_info_of_client.cid;
	msg.tid = m_sipd_sdp_info_of_client.tid;
	msg.did = m_sipd_sdp_info_of_client.did;

	build_and_send_invite_200_ok(&msg, 0);
}


void CInvitePreviewOfClient::Receive_ACK_From_Client(sipd_media_session * pMediaSession)
{
	//	Step 12, 向媒体服务器， 发送 ACK ， 不带

	stringstream ss;
	ss << " CInvitePreviewOfClient::Receive_ACK_From_Client , cid = " << m_sipd_sdp_info_of_client.cid;
	ss << ", channel_id = " << m_channel_id;
	LOG4CPLUS_DEBUG(envir().m_Logger, ss.str().c_str());

	Send_ACK_To_Media();
}

void CInvitePreviewOfClient::Receive_BYE_From_Client(sipd_media_session * pMediaSession)
{

	stringstream ss;
	ss << " CInvitePreviewOfClient::Receive_BYE_From_Client , cid = " << m_sipd_sdp_info_of_client.cid;
	ss << ", channel_id = " << m_channel_id;
	LOG4CPLUS_DEBUG(envir().m_Logger, ss.str().c_str());

	//  Step  14
	Send_200_OK_For_Bye_To_Client();

	//	Step  15
	Send_BYE_To_Media();

	map<string,  CInviteOf3PCC *>::iterator it = envir().m_invite_3pcc.find(m_channel_id);
	if(it!=envir().m_invite_3pcc.end())
	{
		it->second->Del_Old_Invite(this);
	}

}

//////////////////////////////////////////////////////////////////////////
//	与 媒体服务器之间通讯

void CInvitePreviewOfClient::Send_Invite_To_Media(string ssrc)
{
	//	step 8
	string contact_of_media;
	string username_of_media;
	string realm_of_media;
	envir().GetMediaServer(username_of_media, realm_of_media, contact_of_media);
	

	//	向媒体服务器发送 invite
	sipd_message_query_or_response  msg;
	memset(&msg, 0 , sizeof(sipd_message_query_or_response));
	strncpy(msg.to_uri, contact_of_media.c_str(), MAX_SIPD_URI);
	snprintf(msg.from_uri, sizeof(msg.from_uri), "sip:%s@%s:%d",\
		envir().m_cfg.local_domain_id, envir().m_cfg.local_ip, envir().m_cfg.local_port);

	//	传送 CS 客户端的接收信息
	snprintf(msg.body, MAX_SIPD_BODY_LENGTH,
		"v=0\r\n"\
		"o=%s 0 0 IN IP4 %s\r\n"\
		"s=Play\r\n"\
		"i=%s\r\n"\
		"c=IN IP4 %s\r\n"\
		"t=0 0\r\n"\
		/*"m=video %d RTP/AVP %d %d\r\n"\*/
		"m=video %d RTP/AVP %d\r\n"\
		"a=recvonly\r\n"\
		/*"a=rtpmap:%d PS/90000\r\n"\*/
		"a=rtpmap:%d PS/90000\r\n"\
		/*"a=username:%s\r\n"\*/
		/*"a=password:%s\r\n"\*/
		"y=%s\r\n",
		m_sipd_sdp_info_of_client.media_sender_or_receiver_id.c_str(), 
		m_sipd_sdp_info_of_client.media_send_or_receive_ip.c_str(), "GB28181 DEVICE", 
		m_sipd_sdp_info_of_client.media_send_or_receive_ip.c_str(),
		m_sipd_sdp_info_of_client.media_send_or_receive_port, SIP_PS_H264, SIP_PS_H264, ssrc.c_str());

	build_and_send_invite(&msg, 1);
	m_sipd_sdp_info_of_media.cid = msg.cid;
	m_last_command_of_media = "Invite";

	stringstream ss;
	ss << " CInvitePreviewOfClient::Send_Invite_To_Media , cid = " << m_sipd_sdp_info_of_media.cid;
	ss << ", channel_id = " << m_channel_id;
	ss << ", ssrc = " << ssrc;
	LOG4CPLUS_DEBUG(envir().m_Logger, ss.str().c_str());
}


void CInvitePreviewOfClient::Receive_200_OK_From_Media(sipd_media_session * pMediaSession)
{
	if(m_last_command_of_media=="Invite")
	{
		//	收到来自媒体服务器的 回复， 保存媒体服务器的发送信息

		stringstream ss;
		ss << " CInvitePreviewOfClient::Receive_200_OK_From_Media ( For Invite ) , cid = " << m_sipd_sdp_info_of_media.cid;
		ss << ", channel_id = " << m_channel_id;
		LOG4CPLUS_DEBUG(envir().m_Logger, ss.str().c_str());

		m_sipd_sdp_info_of_media.did = pMediaSession->did;
		m_sipd_sdp_info_of_media.tid = pMediaSession->tid;
		m_sipd_sdp_info_of_media.media_send_or_receive_ip = string(pMediaSession->media_recv_ip);
		m_sipd_sdp_info_of_media.media_send_or_receive_port = atoi(pMediaSession->media_recv_port);
		m_sipd_sdp_info_of_media.media_send_or_receive_ip_type = pMediaSession->media_recv_ip_type;
		m_sipd_sdp_info_of_media.ssrc = string(pMediaSession->ssrc);


		// Step 10,  向 CS 客户端发送 200 回复， 带 SDP
		Send_200_OK_For_Invite_To_Client();
	}
	else if(m_last_command_of_media=="Bye")
	{
		//	如果不存在其他的 CS 客户端预览同一路媒体流，则断开 三方呼叫控制建立的 “发送者与媒体服务器”之间的通道

		stringstream ss;
		ss << " CInvitePreviewOfClient::Receive_200_OK_From_Media ( For Bye ), cid = " << m_sipd_sdp_info_of_media.cid;
		ss << ", channel_id = " << m_channel_id;
		LOG4CPLUS_DEBUG(envir().m_Logger, ss.str().c_str());

		delete(this);
	}
}



void CInvitePreviewOfClient::Send_ACK_To_Media()
{
	sipd_message_query_or_response  msg;
	memset(&msg, 0 , sizeof(sipd_message_query_or_response));
	msg.cid = m_sipd_sdp_info_of_media.cid;
	msg.did = m_sipd_sdp_info_of_media.did;
	msg.tid = m_sipd_sdp_info_of_media.tid;

	build_and_send_ack(&msg, 0);
}

void CInvitePreviewOfClient::Send_BYE_To_Media()
{
	sipd_message_query_or_response  msg;
	memset(&msg, 0 , sizeof(sipd_message_query_or_response));
	msg.cid = m_sipd_sdp_info_of_media.cid;
	msg.did = m_sipd_sdp_info_of_media.did;
	msg.tid = m_sipd_sdp_info_of_media.tid;

	build_and_send_bye(&msg);

	m_last_command_of_media = "Bye";
}



void CInvitePreviewOfClient::Notify_3PCC_Finished(string ssrc)
{
	Send_Invite_To_Media(ssrc);
}