#pragma once
#include "medium.h"

class CInviteWithSDP :
	public Medium
{
public:
	CInviteWithSDP(CGbUsageEnvironment& env, sipd_media_session * pMediaSession);
	~CInviteWithSDP(void);

	void Send_200_OK();
	void Receive_ACK(sipd_media_session * pMediaSession);
	void Receive_BYE(sipd_media_session * pMediaSession);

	sipd_sdp_info	m_sipd_sdp_info;
	string			m_ssrc;
	string			m_channel_id;
};
