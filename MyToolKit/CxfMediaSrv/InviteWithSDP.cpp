#include "StdAfx.h"
#include "GbUsageEnvironment.h"

#include "InviteWithSDP.h"

CInviteWithSDP::CInviteWithSDP(CGbUsageEnvironment& env, sipd_media_session * pMediaSession)
:Medium(env)
{
	/* SDP：描述了CS客户端的接收地址、端口、接收媒体类型等接收信息 */
	m_sipd_sdp_info.cid = pMediaSession->cid;
	m_sipd_sdp_info.did = pMediaSession->did;
	m_sipd_sdp_info.tid = pMediaSession->tid;
	m_sipd_sdp_info.media_send_or_receive_ip = string(pMediaSession->media_recv_ip);
	m_sipd_sdp_info.media_send_or_receive_port = atoi(pMediaSession->media_recv_port);
	m_sipd_sdp_info.media_send_or_receive_ip_type = pMediaSession->media_recv_ip_type;
	
	m_ssrc = string(pMediaSession->ssrc);

	env.m_lst_invite_with_sdp.push_back(this);
	// m_ssrc = "";
	m_channel_id = string(pMediaSession->channel_id);

	stringstream ss;
	ss << " CInviteWithSDP  构造 ，cid = " << m_sipd_sdp_info.cid ;
	ss << ",请求 m_ssrc = " << m_ssrc;
	LOG4CPLUS_DEBUG(g_logger, ss.str().c_str() );
}

CInviteWithSDP::~CInviteWithSDP(void)
{
	list<Medium *>::iterator it = ((CGbUsageEnvironment&)envir()).m_lst_invite_with_sdp.begin();
	for(; it!=((CGbUsageEnvironment&)envir()).m_lst_invite_with_sdp.end(); it++)
	{
		if(*it== this)
		{
			((CGbUsageEnvironment&)envir()).m_lst_invite_with_sdp.erase(it);
			break;
		}
	}

	stringstream ss;
	ss << "~CInviteWithSDP 析构 ，cid = " << m_sipd_sdp_info.cid ;
	ss << ", m_ssrc = " << m_ssrc;
	LOG4CPLUS_DEBUG(g_logger, ss.str().c_str() );
}


void CInviteWithSDP::Send_200_OK()
{
	/* 根据 Subject 中的发送者编码查找 SSRC， 放置到 SDP 的 y 字段中，回复 200 ok 
		（带 SDP：描述了媒体服务器的发送地址、端口等发送信息） */

	stringstream ss;

	if( m_ssrc.empty()==false /*((CGbUsageEnvironment&)envir()).m_sender_ssrc.count(m_channel_id)>0*/)
	{
		//m_ssrc= ((CGbUsageEnvironment&)envir()).m_sender_ssrc[m_channel_id];

		ss << "CInviteWithSDP::Send_200_OK ，cid = " << m_sipd_sdp_info.cid ;
		ss << ", ssrc = " << m_ssrc;
		LOG4CPLUS_DEBUG(g_logger, ss.str().c_str() );

		sipd_message_query_or_response  msg;
		memset(&msg, 0 , sizeof(sipd_message_query_or_response));
		msg.cid = m_sipd_sdp_info.cid;
		msg.tid = m_sipd_sdp_info.tid;
		msg.did = m_sipd_sdp_info.did;

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
			g_strLocalDomainID.c_str(), g_strLocalIP.c_str(), "GB28181 DEVICE", g_strLocalIP.c_str(),
			g_iRtpPort, SIP_PS_H264, SIP_PS_H264, m_ssrc.c_str());

		build_and_send_invite_200_ok(&msg, 1);
	}
	else
	{

	}
}


void CInviteWithSDP::Receive_ACK(sipd_media_session * pMediaSession)
{
	stringstream ss;
	ss << " CInviteWithSDP-->Receive_ACK ，cid = " << m_sipd_sdp_info.cid ;
	LOG4CPLUS_DEBUG(g_logger, ss.str().c_str() );

	/* 收到来自 Sip 服务器的 ACK （不带 SDP），将收到的 CS 客户端的接收地址增加到 对应 SSRC 的转发队列中，
		至此，当收到 SSRC 这个码流时，可以转发到所有队列中的接收地址 */

	CGbUsageEnvironment& envir1 =  (CGbUsageEnvironment&)envir();
	if(m_ssrc.empty()==false)
	{
		JMutexAutoLock lock(envir1.m_mtx_transfer);
		map<string, list<sipd_sdp_info>>::iterator it = envir1.m_ssrc_tranfer.find(m_ssrc);
		if(it!=((CGbUsageEnvironment&)envir()).m_ssrc_tranfer.end())
		{
			it->second.push_back(m_sipd_sdp_info);
		}

	}
	else
	{

	}


}

void CInviteWithSDP::Receive_BYE(sipd_media_session * pMediaSession)
{

	stringstream ss;
	ss << " CInviteWithSDP-->Receive_BYE ，cid = " << m_sipd_sdp_info.cid ;
	LOG4CPLUS_DEBUG(g_logger, ss.str().c_str() );

	//	从转发队列中删除
	if(m_ssrc.empty()==false)
	{
		CGbUsageEnvironment& envir1 =  (CGbUsageEnvironment&)envir();

		JMutexAutoLock lock(envir1.m_mtx_transfer);
		map<string, list<sipd_sdp_info>>::iterator it = envir1.m_ssrc_tranfer.find(m_ssrc);
		if(it!=((CGbUsageEnvironment&)envir()).m_ssrc_tranfer.end())
		{
			list<sipd_sdp_info>::iterator it_lst = it->second.begin();
			for(; it_lst!=it->second.end(); it_lst++)
			{
				if(it_lst->did == m_sipd_sdp_info.did && 
					it_lst->tid == m_sipd_sdp_info.tid)
				{
					it->second.erase(it_lst);
					break;
				}
			}
		}
	}
	else
	{

	}

	sipd_message_query_or_response  msg;
	memset(&msg, 0 , sizeof(sipd_message_query_or_response));
	msg.cid = m_sipd_sdp_info.cid;
	msg.tid = m_sipd_sdp_info.tid;
	msg.did = m_sipd_sdp_info.did;

	build_and_send_invite_200_ok(&msg, 0);

	Medium::close(this);
}