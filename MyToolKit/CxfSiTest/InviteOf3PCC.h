#pragma once
#include "medium.h"

class CInviteOf3PCC /*:	public Medium*/
{
public:
	CInviteOf3PCC(CGbUsageEnvironment& env, string contact_of_media, string contact_of_sender, SIPD_STREAM_TYPE steam_type);
	~CInviteOf3PCC(void);

	void Send_Invite_To_Media();			//	与 媒体服务
	void Send_Invite_To_Sender();

	void Receive_200_OK_From_Media(sipd_media_session * pMediaSession);
	void Receive_200_OK_From_Sender(sipd_media_session * pMediaSession);

	void Send_ACK_To_Media();
	void Send_ACK_To_Sender();

	void Send_BYE_To_Media();
	void Send_BYE_To_Sender();

	void Add_New_Invite(CInviteOfBase * pInvite);
	void Del_Old_Invite(CInviteOfBase * pInvite);	

	string GetSSRCOfSender(){  return m_sipd_sdp_info_of_sender.ssrc; }

	CGbUsageEnvironment& envir() const {return fEnviron;}

	list<CInviteOfBase *>	m_lst_notify;

	sipd_sdp_info	m_sipd_sdp_info_of_media;
	sipd_sdp_info	m_sipd_sdp_info_of_sender;

	string			m_ssrc_of_using;		//	本对象真正使用的 域内 ssrc
	SIPD_STREAM_TYPE	m_steam_type;		//	play， playback， download 

	//	保存 正在使用该 “三方呼叫控制”的所有来自 CS 客户端的 invite
	list<CInviteOfBase *>	m_lst_invite;	
	
private:
	

	string			m_contact_of_media;
	string			m_contact_of_sender;
	string			m_contact_of_sip;

	bool			m_is_bye_ok_of_media;
	bool			m_is_bye_ok_of_sender;

	string			m_last_command_of_media;
	string			m_last_command_of_sender;

	CGbUsageEnvironment& fEnviron;
};
