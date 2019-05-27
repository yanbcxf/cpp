#pragma once

class CDllEnvironment
{
public:
	CDllEnvironment(void);
	~CDllEnvironment(void);

	enum MSG_TYPE {
		
		msg_Sipd_PTZ_Preset_request  = WM_USER,
		msg_Sipd_PTZ_Tour_request,
		msg_Sipd_PTZ_AutoScan_request,
		msg_Sipd_PTZ_Move_request,
		msg_Sipd_record_control_request,
		msg_Sipd_alarm_control_request,

		/*PTZ 等控制命令的回复*/
		msg_Sipd_Message_200_Ok,
		msg_Sipd_Message_Failure,
		
		msg_Sipd_register_request,

		msg_sipd_message_query_req,
		msg_sipd_message_response_req,
		msg_sipd_on_idle_req,

		/*实时流、回放流、下载流等的请求*/
		msg_Sipd_Call_play_playback_download,
		msg_Sipd_Call_ack,
		msg_Sipd_Call_bye,
		msg_Sipd_Call_release,
		
		/*实时流、回放流、下载流等的回复*/
		msg_Sipd_Call_200_OK,
		msg_Sipd_Call_Message_New,
		msg_Sipd_Call_No_Answer,

		/*通过 Message_New 的请求或者回复*/
		msg_Sipd_stateinfo_request,
		msg_Sipd_search_recordfile_request,
		msg_Sipd_deviceinfo_request,
		msg_sipd_catalog_request,

		msg_Sipd_stateinfo_response,
		msg_Sipd_search_recordfile_response,
		msg_Sipd_deviceinfo_response,
		msg_sipd_catalog_response,

		/* 以下来自 exe 上层 */
		msg_Dll_PTZ_Preset_request,
		msg_Dll_PTZ_Tour_request,
		msg_Dll_PTZ_AutoScan_request,
		msg_Dll_PTZ_Move_request,
		msg_Dll_record_control_request,
		msg_Dll_alarm_control_request,

		msg_Dll_stateinfo_request,
		msg_Dll_search_recordfile_request,
		msg_Dll_deviceinfo_request,
		msg_Dll_catalog_request,

		msg_Dll_Call_play_playback_download,
		msg_Dll_Call_bye

	};
};
