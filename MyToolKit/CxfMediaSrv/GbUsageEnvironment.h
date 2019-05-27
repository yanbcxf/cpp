#pragma once
#include "medium.h"

class CGbUsageEnvironment : public UsageEnvironment
{
public:

	CGbUsageEnvironment(void);
	~CGbUsageEnvironment(void);


	enum MSG_TYPE {
		msg_sipd_catalog_req = WM_USER,
		msg_Sipd_PTZ_Preset_request,
		msg_Sipd_PTZ_Tour_request,
		msg_Sipd_PTZ_AutoScan_request,
		msg_Sipd_PTZ_Move_request,
		msg_Sipd_record_control_request,
		msg_Sipd_alarm_control_request,
		msg_Sipd_stateinfo_request,
		msg_Sipd_search_recordfile_request,
		msg_Sipd_deviceinfo_request,

		msg_Sipd_register_request,
		msg_sipd_message_query_req,
		msg_sipd_message_response_req,
		msg_sipd_on_idle_req,
		msg_Invite_play_playback_download,
		msg_Invite_ack,
		msg_Invite_bye,
		msg_Invite_release
	};

	static CGbUsageEnvironment* getInstance();

	static int Sipd_cataloginfo_request_cb(const sipd_catalog_req * pCatalogReq);
	static int Sipd_PTZ_Preset_request_cb(const sipd_preset_control * pPresetControl);
	static int Sipd_PTZ_Tour_request_cb(const sipd_tour_control * pTourControl);


	static int  Sipd_PTZ_AutoScan_request_cb(const sipd_autoscan_control * pAutoScanControl);
	static int  Sipd_PTZ_Move_request_cb(const sipd_ptz_control* pControl);
	static int  Sipd_record_control_request_cb(const sipd_record_control * pRecordControl);
	static int  Sipd_alarm_control_request_cb(const sipd_alarm_control * pAlarmControl);
	static int  Sipd_stateinfo_request_cb(const sipd_devicestate_req * pStateInfoReq);
	static int  Sipd_search_recordfile_request_cb (sipd_recordinfo_req * pRecordFileReq);
	static int  Sipd_deviceinfo_request_cb(const sipd_deviceinfo_req * pDeviceinfoReq);
	static int  Sipd_register_request_cb(const sipd_register_req * pRegisterReq);
	static int  Sipd_settime_request_cb(const sipd_id_info * pDeviceID,  const SIPD_TIME* pTime);
	static int  sipd_message_query_request_cb(const sipd_message_query_or_response * pMessageResponseReq);
	static int  sipd_message_response_request_cb(const sipd_message_query_or_response * pMessageResponseReq);
	static int  sipd_on_idle_cb();

	static int	Invite_play_playback_download_cb(const sipd_media_session* pMediaInfo);
	static int	Invite_ack_cb(const sipd_media_session* pMediaInfo);
	static int	Invite_bye_cb(const sipd_media_session* pMediaInfo);
	static int	Invite_release_cb(const sipd_media_session* pMediaInfo);


	static int	Sipd_media_control_notify_CMS_cb(const int did, const int tid, const enum SIPD_TO_CMS_CMD_TYPE cmd_type, 
		const sipd_playback_mans_rtsp_ind* playback_rtsp_control);


	void MessageHandler(MSG *);
	void init(SIPD_GATEWAY_CFG cfg);
	
	string GetUsernameFromUri(string uri);
	Medium * FindInvite(bool isWithSDP, const sipd_media_session * pMediaInfo);
	list<sipd_sdp_info> GetTransferList(string ssrc);

public:
	
	SIPD_GATEWAY_CFG	m_cfg;
	list<Medium *>		m_lst_invite_without_sdp;
	list<Medium *>		m_lst_invite_with_sdp;

	map<string, string>	m_sender_ssrc;		//	发送者发出的 ssrc 
	map<string, list<sipd_sdp_info>>	m_ssrc_tranfer;	//	转发队列
	JMutex m_mtx_transfer;


private:
	static CGbUsageEnvironment * instance;
};
