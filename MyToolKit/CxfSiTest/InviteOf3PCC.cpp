#include "StdAfx.h"
#include "GbUsageEnvironment.h"
#include "CatalogQuery.h"
#include "InviteOfBase.h"
#include "InviteOf3PCC.h"

CInviteOf3PCC::CInviteOf3PCC(CGbUsageEnvironment& env, string contact_of_media, string contact_of_sender,  SIPD_STREAM_TYPE steam_type)
//:Medium(env)
:fEnviron(env)
{
	m_contact_of_media = contact_of_media;
	m_contact_of_sender = contact_of_sender;
	m_steam_type = steam_type;

	m_is_bye_ok_of_sender = false;
	m_is_bye_ok_of_media = false;

	m_ssrc_of_using = "";

	char from_uri[MAX_SIPD_URI];
	memset(from_uri, MAX_SIPD_URI, 0);
	snprintf(from_uri, sizeof(from_uri), "sip:%s@%s:%d",\
		env.m_cfg.local_domain_id, env.m_cfg.local_ip, env.m_cfg.local_port);

	m_contact_of_sip = string(from_uri);

	string sender_username = env.GetUsernameFromUri(m_contact_of_sender);
	env.m_invite_3pcc[sender_username] = this;

	stringstream ss;
	ss << "CInviteOf3PCC 构造 , sender = " << sender_username;
	ss << ", meidaSrv = " << env.GetUsernameFromUri(m_contact_of_media);
	LOG4CPLUS_DEBUG(env.m_Logger, ss.str().c_str());

	m_sipd_sdp_info_of_media.cid = -1;
	m_sipd_sdp_info_of_sender.cid = -1;
}

CInviteOf3PCC::~CInviteOf3PCC(void)
{
	string sender_username = envir().GetUsernameFromUri(m_contact_of_sender);
	if(envir().m_invite_3pcc.count(sender_username)>0)
	{
		envir().m_invite_3pcc.erase(sender_username);
	}

	stringstream ss;
	ss << " ~CInviteOf3PCC 析构 , sender = " << sender_username;
	ss << ", meidaSrv = " << envir().GetUsernameFromUri(m_contact_of_media);
	ss << ",  从 m_invite_3pcc 中删除 ";
	LOG4CPLUS_DEBUG(envir().m_Logger, ss.str().c_str());
}


void CInviteOf3PCC::Send_Invite_To_Media()
{
	//	Step 2, 向 媒体服务器发送 invite ，以便获得 接收的 SDP

	
	sipd_message_query_or_response  msg;
	memset(&msg, 0 , sizeof(sipd_message_query_or_response));
	strncpy(msg.to_uri, m_contact_of_media.c_str(), MAX_SIPD_URI);
	strncpy(msg.from_uri, m_contact_of_sip.c_str(), MAX_SIPD_URI);
	
	build_and_send_invite(&msg, 0);
	m_sipd_sdp_info_of_media.cid = msg.cid;
	m_last_command_of_media = "Invite";

	stringstream ss;
	ss << "CInviteOf3PCC Send_Invite_To_Media , cid = " << msg.cid;
	ss << ", sender = " << m_contact_of_sender;
	ss << ", meidaSrv = " << m_contact_of_media;
	LOG4CPLUS_DEBUG(envir().m_Logger, ss.str().c_str());
}

void CInviteOf3PCC::Send_Invite_To_Sender()
{
	//	从本服务器分配一个 取流的 SSRC ，该 SSRC 在本域内不重复
	CGbUsageEnvironment&	envir1 = (CGbUsageEnvironment&)envir();
	if(m_steam_type==SIPD_STREAM_TYPE_PLAY)	
		m_ssrc_of_using =  envir1.fetch_ssrc_of_preview();

	//	Step 4, 向 媒体发送者 发送 invite ，以便获得 发送的 SDP

	sipd_message_query_or_response  msg;
	memset(&msg, 0 , sizeof(sipd_message_query_or_response));
	strncpy(msg.to_uri, m_contact_of_sender.c_str(), MAX_SIPD_URI);
	strncpy(msg.from_uri, m_contact_of_sip.c_str(), MAX_SIPD_URI);

	string s;
	if(m_steam_type==SIPD_STREAM_TYPE_PLAY)				s = "Play";
	else if(m_steam_type==SIPD_STREAM_TYPE_PLAYBACK)		s = "Playback";
	else if(m_steam_type==SIPD_STREAM_TYPE_DOWNLOAD)		s = "Download";

	snprintf(msg.body, MAX_SIPD_BODY_LENGTH,
		"v=0\r\n"\
		"o=%s 0 0 IN IP4 %s\r\n"\
		"s=%s\r\n"\
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
		m_sipd_sdp_info_of_media.media_sender_or_receiver_id.c_str(), 
		m_sipd_sdp_info_of_media.media_send_or_receive_ip.c_str(),
		s.c_str(),
		"GB28181 DEVICE", 
		m_sipd_sdp_info_of_media.media_send_or_receive_ip.c_str(),
		m_sipd_sdp_info_of_media.media_send_or_receive_port, SIP_PS_H264, SIP_PS_H264, m_ssrc_of_using.c_str());

	build_and_send_invite(&msg, 1);
	m_sipd_sdp_info_of_sender.cid = msg.cid;
	m_last_command_of_sender = "Invite";

	stringstream ss;
	ss << "CInviteOf3PCC Send_Invite_To_Sender , cid = " <<  msg.cid;
	ss << ", sender = " << m_contact_of_sender;
	ss << ", meidaSrv = " << m_contact_of_media;
	LOG4CPLUS_DEBUG(envir().m_Logger, ss.str().c_str());

}


void CInviteOf3PCC::Receive_200_OK_From_Media(sipd_media_session * pMediaSession)
{
	if(m_last_command_of_media=="Invite")
	{
		stringstream ss;
		ss << "CInviteOf3PCC Receive_200_OK_From_Media (For Invite) , cid = "<< pMediaSession->cid;
		ss << ", sender = " << m_contact_of_sender;
		ss << ", meidaSrv = " << m_contact_of_media;
		LOG4CPLUS_DEBUG(envir().m_Logger, ss.str().c_str());

		//	保存媒体服务器的 SDP, Step 3
		m_sipd_sdp_info_of_media.did = pMediaSession->did;
		m_sipd_sdp_info_of_media.tid = pMediaSession->tid;
		m_sipd_sdp_info_of_media.media_send_or_receive_ip = string(pMediaSession->media_recv_ip);
		m_sipd_sdp_info_of_media.media_send_or_receive_port = atoi(pMediaSession->media_recv_port);
		m_sipd_sdp_info_of_media.media_send_or_receive_ip_type = pMediaSession->media_recv_ip_type;
		m_sipd_sdp_info_of_media.media_sender_or_receiver_id = string(pMediaSession->o_id);

		//	向媒体发送者 ，发送 INVITE , Step 4
		Send_Invite_To_Sender();
	}
	else if(m_last_command_of_media=="Bye")
	{
		stringstream ss;
		ss << "CInviteOf3PCC Receive_200_OK_From_Media (For Bye) , cid = "<< pMediaSession->cid;
		ss << ", sender = " << m_contact_of_sender;
		ss << ", meidaSrv = " << m_contact_of_media;
		LOG4CPLUS_DEBUG(envir().m_Logger, ss.str().c_str());

		m_is_bye_ok_of_media = true;
		if(m_is_bye_ok_of_media && m_is_bye_ok_of_sender)
			delete this;
	}
	
}

void CInviteOf3PCC::Receive_200_OK_From_Sender(sipd_media_session * pMediaSession)
{
	CGbUsageEnvironment&	envir1 = (CGbUsageEnvironment&)envir();
	stringstream ss;
	if(m_last_command_of_sender=="Invite")
	{
		ss << " CInviteOf3PCC::Receive_200_OK_From_Sender , sender = " << envir1.GetUsernameFromUri(m_contact_of_sender);
		ss << ", meidaSrv = " << envir1.GetUsernameFromUri(m_contact_of_media);
		ss << ", 分配的 SSRC = " << m_ssrc_of_using << ", 发送者返回的 SSRC = " << string(pMediaSession->ssrc);
		LOG4CPLUS_DEBUG(envir1.m_Logger, ss.str().c_str());

		// Step 5
		m_sipd_sdp_info_of_sender.did = pMediaSession->did;
		m_sipd_sdp_info_of_sender.tid = pMediaSession->tid;
		m_sipd_sdp_info_of_sender.media_send_or_receive_ip = string(pMediaSession->media_recv_ip);
		m_sipd_sdp_info_of_sender.media_send_or_receive_port = atoi(pMediaSession->media_recv_port);
		m_sipd_sdp_info_of_sender.media_send_or_receive_ip_type = pMediaSession->media_recv_ip_type;
		m_sipd_sdp_info_of_sender.media_sender_or_receiver_id = string(pMediaSession->o_id);
		m_sipd_sdp_info_of_sender.ssrc = string(pMediaSession->ssrc);

		m_ssrc_of_using = string(pMediaSession->ssrc);

		// Step 6, 发送 
		Send_ACK_To_Media();

		// Step  7
		Send_ACK_To_Sender();

		//	通知 InviteOfbase ，三方控制完成
		if(m_lst_notify.size()>0)
		{
			list<CInviteOfBase *>::iterator it= m_lst_notify.begin();
			for(; it!=m_lst_notify.end(); it++)
			{
				(*it)->Notify_3PCC_Finished(m_sipd_sdp_info_of_sender.ssrc);
			}
			m_lst_notify.clear();
		}
	}
	else if(m_last_command_of_sender=="Bye")
	{
		stringstream ss;
		ss << "CInviteOf3PCC Receive_200_OK_From_Sender (For Bye) , cid = "<< pMediaSession->cid;
		ss << ", sender = " << m_contact_of_sender;
		ss << ", meidaSrv = " << m_contact_of_media;
		LOG4CPLUS_DEBUG(envir().m_Logger, ss.str().c_str());

		m_is_bye_ok_of_sender = true;
		if(m_is_bye_ok_of_media && m_is_bye_ok_of_sender)
			delete this;
	}
}

void CInviteOf3PCC::Send_ACK_To_Media()
{
	sipd_message_query_or_response  msg;
	memset(&msg, 0 , sizeof(sipd_message_query_or_response));
	/*strncpy(msg.to_uri, m_contact_of_media.c_str(), MAX_SIPD_URI);
	strncpy(msg.from_uri, m_contact_of_sip.c_str(), MAX_SIPD_URI);*/
	msg.cid = m_sipd_sdp_info_of_media.cid;
	msg.did = m_sipd_sdp_info_of_media.did;
	msg.tid = m_sipd_sdp_info_of_media.tid;

	snprintf(msg.body, MAX_SIPD_BODY_LENGTH,
		"v=0\r\n"\
		"o=%s 0 0 IN IP4 %s\r\n"\
		"s=%s\r\n"\
		"c=IN IP4 %s\r\n"\
		"t=0 0\r\n"\
		/*"m=video %d RTP/AVP %d %d\r\n"\*/
		"m=video %d RTP/AVP %d\r\n"\
		"a=sendonly\r\n"\
		/*"a=rtpmap:%d PS/90000\r\n"\*/
		"a=rtpmap:%d PS/90000\r\n"\
		/*"a=username:%s\r\n"\*/
		/*"a=password:%s\r\n"\*/
		"y=%s\r\n",
		m_sipd_sdp_info_of_sender.media_sender_or_receiver_id.c_str(), 
		m_sipd_sdp_info_of_sender.media_send_or_receive_ip.c_str(), "GB28181 DEVICE", 
		m_sipd_sdp_info_of_sender.media_send_or_receive_ip.c_str(),
		m_sipd_sdp_info_of_sender.media_send_or_receive_port, SIP_PS_H264, SIP_PS_H264, m_sipd_sdp_info_of_sender.ssrc.c_str());

	build_and_send_ack(&msg, 1);
	m_last_command_of_media = "Ack";
}



void CInviteOf3PCC::Send_ACK_To_Sender()
{
	sipd_message_query_or_response  msg;
	memset(&msg, 0 , sizeof(sipd_message_query_or_response));
	msg.cid = m_sipd_sdp_info_of_sender.cid;
	msg.did = m_sipd_sdp_info_of_sender.did;
	msg.tid = m_sipd_sdp_info_of_sender.tid;
	/*strncpy(msg.to_uri, m_contact_of_sender.c_str(), MAX_SIPD_URI);
	strncpy(msg.from_uri, m_contact_of_sip.c_str(), MAX_SIPD_URI);*/


	build_and_send_ack(&msg, 0);
}

void CInviteOf3PCC::Send_BYE_To_Sender()
{

	stringstream ss;
	ss << "CInviteOf3PCC Send_BYE_To_Sender , cid = "<< m_sipd_sdp_info_of_sender.cid;
	ss << ", sender = " << m_contact_of_sender;
	ss << ", meidaSrv = " << m_contact_of_media;
	LOG4CPLUS_DEBUG(envir().m_Logger, ss.str().c_str());

	sipd_message_query_or_response  msg;
	memset(&msg, 0 , sizeof(sipd_message_query_or_response));
	msg.cid = m_sipd_sdp_info_of_sender.cid;
	msg.did = m_sipd_sdp_info_of_sender.did;
	msg.tid = m_sipd_sdp_info_of_sender.tid;


	build_and_send_bye(&msg);
	m_last_command_of_sender = "Bye";
}


void CInviteOf3PCC::Send_BYE_To_Media()
{

	stringstream ss;
	ss << "CInviteOf3PCC Send_BYE_To_Media , cid = "<< m_sipd_sdp_info_of_media.cid;
	ss << ", sender = " << m_contact_of_sender;
	ss << ", meidaSrv = " << m_contact_of_media;
	LOG4CPLUS_DEBUG(envir().m_Logger, ss.str().c_str());

	sipd_message_query_or_response  msg;
	memset(&msg, 0 , sizeof(sipd_message_query_or_response));
	msg.cid = m_sipd_sdp_info_of_media.cid;
	msg.did = m_sipd_sdp_info_of_media.did;
	msg.tid = m_sipd_sdp_info_of_media.tid;

	build_and_send_bye(&msg);
	m_last_command_of_media = "Bye";
}


void CInviteOf3PCC::Add_New_Invite(CInviteOfBase * pInvite)
{
	m_lst_invite.push_back(pInvite);
}

void CInviteOf3PCC::Del_Old_Invite(CInviteOfBase * pInvite)
{
	list<CInviteOfBase *>::iterator it = m_lst_invite.begin();
	for( ;  it!=m_lst_invite.end(); it++)
	{
		if((*it)==pInvite)
		{
			m_lst_invite.erase(it);
			break;
		}
	}

	if(m_lst_invite.size()==0)
	{
		Send_BYE_To_Sender();
		Send_BYE_To_Media();
	}
}