#include "StdAfx.h"
#include "DllCallPlayPlaybackDownload.h"
#include "DllMediaSession.h"

CDllCallPlayPlaybackDownload::CDllCallPlayPlaybackDownload(sipd_media_session* pMediaInfo)
:AbstractCommand(false)
{
	//m_MediaInfo = *pMediaInfo;
	memcpy(&m_MediaInfo, pMediaInfo, sizeof(sipd_media_session));
}

CDllCallPlayPlaybackDownload::~CDllCallPlayPlaybackDownload(void)
{
}


int CDllCallPlayPlaybackDownload::Execute(UsageEnvironment* pEnviron)
{
	stringstream ss;
	CDllMediaSession * pSession = new CDllMediaSession(*pEnviron, m_nSerialNumber);
	//	分配接收 PS 流的端口

	int status;
	
	int nRtpPort = pEnviron->m_iRtpPort + 2;
	while(true)
	{
		if(nRtpPort>=UDP_MEDIA_START_PORT + 1000)
			nRtpPort = UDP_MEDIA_START_PORT;
		if(nRtpPort == pEnviron->m_iRtpPort)
			break;

		RTPSessionParams sessionparams;
		sessionparams.SetOwnTimestampUnit(1.0/90000.0);
		sessionparams.SetUsePollThread(false);

		RTPUDPv4TransmissionParams transparams;
		transparams.SetPortbase( nRtpPort);
		transparams.SetRTPReceiveBuffer(RTPUDPV4TRANS_RTPRECEIVEBUFFER * 10);

		status = pSession->m_rtp_session.Create(sessionparams,&transparams);
		if (status < 0)
		{
			if(status != ERR_RTP_UDPV4TRANS_CANTBINDRTPSOCKET && status != ERR_RTP_UDPV4TRANS_CANTBINDRTCPSOCKET)
				break;
			else
				nRtpPort +=2;
		}
		else
		{
			break;
		}
	}

	if(nRtpPort == pEnviron->m_iRtpPort)
	{
		//	创建 rtp 会话失败

		CCallbackDllCallResponse *pcb = new CCallbackDllCallResponse(pSession->SerailNumber(), 0, -1);
		pEnviron->PostCallBack(pcb);

		delete pSession;
		return -1;
	}
	else
		pEnviron->m_iRtpPort = nRtpPort;
	
	sprintf_s(m_MediaInfo.media_recv_port, MAX_SIPD_NORMAL_LEN, "%d", nRtpPort);
	sprintf_s(m_MediaInfo.media_recv_ip, MAX_SIPD_URI, "%s", pEnviron->m_cfg.local_ip);
	sprintf_s(m_MediaInfo.ssrc, MAX_SIPD_NORMAL_LEN, "%s", "123456");

	if(m_MediaInfo.stream_type == SIPD_STREAM_TYPE_PLAY)
	{
		sprintf_s(m_MediaInfo.t_start_time, MAX_SIPD_URI, "%s",  "0");
		sprintf_s(m_MediaInfo.t_end_time,   MAX_SIPD_URI, "%s",  "0");
	}	
	
	ss.str("");
	ss << "CDllCallPlayPlaybackDownload, Sip_Call_play_playback_download 开始 nRtpPort =  " << nRtpPort;
	ss << ", local_ip = " << string(pEnviron->m_cfg.local_ip);
	ss << ", o_id = " << string(m_MediaInfo.o_id);
	ss << ", t_start_time = " << pSession->m_start_time;
	LOG4CPLUS_DEBUG(pEnviron->m_Logger, ss.str());

	//	向下级域 发送 启动流命令
	int call_id = Sip_Call_play_playback_download(&m_MediaInfo, 1);

	ss.str("");
	if(call_id>=0)
	{
		pSession->m_cid = call_id;
		pSession->m_request_sdp = m_MediaInfo;
		pSession->m_start_rtp = 0;
		pSession->m_rtp_seq = -1;
		pSession->m_hasMarker = true;
		if(m_MediaInfo.stream_type == SIPD_STREAM_TYPE_PLAY)
		{
			pSession->m_start_time = 0;
		}
		else
		{
			pSession->m_start_time = strtoul(m_MediaInfo.t_start_time, 0, 10);
		}

		pSession->m_KeepaliveTime = GetTickCount();

		ss << "CDllCallPlayPlaybackDownload, Sip_Call_play_playback_download 成功 nRtpPort =  " << nRtpPort;
		ss << ", local_ip = " << string(pEnviron->m_cfg.local_ip);
		ss << ", o_id = " << string(m_MediaInfo.o_id);
		ss << ", t_start_time = " << pSession->m_start_time;
		LOG4CPLUS_DEBUG(pEnviron->m_Logger, ss.str());

		return pSession->SerailNumber();
	}
	else
	{
		ss << "CDllCallPlayPlaybackDownload, Sip_Call_play_playback_download 失败 call_id =  " << call_id;
		LOG4CPLUS_DEBUG(pEnviron->m_Logger, ss.str());

		CCallbackDllCallResponse *pcb = new CCallbackDllCallResponse(pSession->SerailNumber(), 0, -1);
		pEnviron->PostCallBack(pcb);
		delete pSession;
		return call_id;
	}
	
}