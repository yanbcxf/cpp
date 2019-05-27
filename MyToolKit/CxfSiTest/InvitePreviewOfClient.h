#pragma once
#include "InviteOfBase.h"

class CInvitePreviewOfClient :
	public CInviteOfBase
{
public:
	CInvitePreviewOfClient(CGbUsageEnvironment& env, sipd_media_session * pMediaSession);
	~CInvitePreviewOfClient(void);

	void Send_200_OK_For_Invite_To_Client();
	void Send_200_OK_For_Bye_To_Client();
	void Receive_ACK_From_Client(sipd_media_session * pMediaSession);
	void Receive_BYE_From_Client(sipd_media_session * pMediaSession);

	void Receive_200_OK_From_Media(sipd_media_session * pMediaSession);
	void Send_Invite_To_Media(string ssrc);	//	与 媒体发送者
	void Send_ACK_To_Media();
	void Send_BYE_To_Media();

	void Notify_3PCC_Finished(string ssrc);

	

	sipd_sdp_info	m_sipd_sdp_info_of_client;
	sipd_sdp_info	m_sipd_sdp_info_of_media;

	string			m_channel_id;
	string			m_last_command_of_media;


	
};
