#include "sipd.h"
//#include "proxycore.h"


struct eXosip_t *context_eXosip = NULL;
Invite_play_playback_download_func	pInvite_play_playback_download;
Invite_play_playback_download_func	pInvite_200_ok;
Invite_play_playback_download_func	pInvite_no_answer;
Invite_play_playback_download_func	pInvite_ack;
Invite_play_playback_download_func	pInvite_bye;
Invite_play_playback_download_func	pInvite_release;


Sipd_PTZ_Preset_func				pSipd_PTZ_Preset;
Sipd_PTZ_Tour_func					pSipd_PTZ_Tour;
Sipd_PTZ_AutoScan_func				pSipd_PTZ_AutoScan;
Sipd_PTZ_Move_func					pSipd_PTZ_Move;
Sipd_record_control_func			pSipd_record_control;
Sipd_alarm_control_func				pSipd_alarm_control;
Sipd_media_control_notify_CMS_func	pSipd_media_control_notify_CMS;
Sipd_stateinfo_request_func			pSipd_stateinfo_request;
Sipd_cataloginfo_request_func		pSipd_cataloginfo_request;
Sipd_search_recordfile_req_func		pSipd_search_recordfile_req;
Sipd_deviceinfo_request_func		pSipd_deviceinfo_request;
Sipd_GetSIPServerCfg_func			pSipd_GetSIPServerCfg;
Sipd_settime_func					pSipd_settime;
Sipd_register_req_func				pSipd_register_req;

sipd_message_query_or_response_req_func		pSipd_message_query_req;
sipd_message_query_or_response_req_func		pSipd_message_response_req;
sipd_on_idle_func					pSipd_on_idle;

//////////////////////////////////////////////////////////////////////////

#define CONTENT_TYPE_APP				("Application")
#define CONTENT_TYPE_SUB_MANSCMD		("MANSCDP+xml")
#define CONTENT_TYPE_SUB_MANSRTSP	("MANSRTSP")
#define CONTENT_TYPE_SUB_RTSP	("RTSP")
#define MAX_XML_BUFFER_LEN			(1024)
#define XML_NAME_CmdType			("CmdType")
#define XML_VALUE_CmdType_DeviceControl			("DeviceControl")
#define XML_VALUE_CmdType_Alarm					("Alarm")
#define XML_NAME_Result			("Result")
#define XML_NAME_SN			("SN")
#define XML_NAME_DeviceID			("DeviceID")
#define XML_NAME_PTZCmd			("PTZCmd")
#define XML_NAME_TeleBoot			("TeleBoot")
#define XML_VALUE_TeleBoot_Boot			("Boot")
#define XML_NAME_RecordCmd			("RecordCmd")
#define XML_VALUE_RecordCmd_Record			("Record")
#define XML_VALUE_RecordCmd_StopRecord			("StopRecord")
#define XML_NAME_GuardCmd			("GuardCmd")
#define XML_VALUE_GuardCmd_SetGuard			("SetGuard")
#define XML_VALUE_GuardCmd_ResetGuard			("ResetGuard")
#define XML_NAME_AlarmCmd			("AlarmCmd")
#define XML_VALUE_AlarmCmd_ResetAlarm			("ResetAlarm")
#define XML_NAME_ControlPriority			("ControlPriority")
#define XML_NAME_StartTime				("StartTime")
#define XML_NAME_EndTime				("EndTime")
#define XML_NAME_FilePath				("FilePath")
#define XML_NAME_Address				("Address")
#define XML_NAME_Secrecy				("Secrecy")
#define XML_NAME_Type				("Type")
#define XML_Value_Type_time				("time")
#define XML_Value_Type_alarm			("alarm")
#define XML_Value_Type_manual			("manual")
#define XML_Value_Type_all			("all")
#define XML_NAME_RecorderID				("RecorderID")
#define XML_NAME_StartAlarmPriority			("StartAlarmPriority")
#define XML_NAME_EndAlarmPriority			("EndAlarmPriority")
#define XML_NAME_AlarmMethod			("AlarmMethod")

#define MANSCDP_XML 	("Application/MANSCDP+xml")
#define QUERY 			("Query")
#define CMDTYPE 			("CmdType")
#define DEVICEINFO 		("DeviceInfo")
#define DEVICESTATUS 		("DeviceStatus")
#define DEVICECATALOG		("Catalog")
#define SIPD_RECORDINFO		("RecordInfo")
#define PERSETQUERY		("PersetQuery")
#define RESPONSE 			("Response")

#define SIPD_PTZ_INSTRUCTION_CODE_SET_PRESET			0x81
#define SIPD_PTZ_INSTRUCTION_CODE_CALL_PRESET			0x82
#define SIPD_PTZ_INSTRUCTION_CODE_DELETE_PRESET			0x83

#define SIPD_PTZ_INSTRUCTION_CODE_ADD_CRUISE			0x84
#define SIPD_PTZ_INSTRUCTION_CODE_DELETE_CRUISE			0x85
#define SIPD_PTZ_INSTRUCTION_CODE_SET_SPEED_CRUISE		0x86
#define SIPD_PTZ_INSTRUCTION_CODE_SET_TIME_CRUISE		0x87
#define SIPD_PTZ_INSTRUCTION_CODE_START_CRUISE			0x88

#define SIPD_PTZ_INSTRUCTION_CODE_AUTOSCAN				0x89
#define SIPD_PTZ_INSTRUCTION_CODE_SET_AUTOSCAN_SPEED	0x8A

#define SIPD_PTZ_INSTRUCTION_CODE_ZERO					0x0

#define SIPD_VIDEO_PLAY					("Play")
#define SIPD_VIDEO_PLAYBACK				("Playback")
#define SIPD_VIDEO_DOWNLOAD					("Download")

#define SIPD_RTP_DEFAULT_AUDIO_SSRC				("000000000")
#define SIPD_RTP_DEFAULT_VIDEO_SSRC				("000000001")
#define SIPD_RTP_SSCR_LENGTH				9



/*TESTCODE*/
#if 0
struct sipd_media_session test_media_session;
#endif


extern void sipd_manscdp_xml_deal(const osip_message_t * sip_request, char *body);
extern int sip_sender_sdp_create(const sipd_sender_sdp_info* pSenderSdp, char* sdp_buffer);
extern int sip_receiver_sdp_create(const sipd_sender_sdp_info* pSenderSdp, char* sdp_buffer);


int Invite_adaptor_ind(sipd_media_session * invite_adaptor, const struct sipd_invite_media_session* pSdpSession)
{
	//sipd_media_session invite_adaptor;
	char* p=NULL;
	int len=0;
	int play_type=0;

	printf("Invite_adaptor_ind \n");
	memset(invite_adaptor, 0, sizeof(sipd_media_session));

	invite_adaptor->cid = pSdpSession->cid;
	invite_adaptor->did = pSdpSession->did;
	invite_adaptor->tid = pSdpSession->tid;
	strncpy(invite_adaptor->o_id, pSdpSession->o_field_username, MAX_SIPD_DEVICE_ID_LEN);
	
	p = strchr(pSdpSession->u_field, ':');
	if(p != NULL)
	{
		if((p - pSdpSession->u_field) > MAX_SIPD_DEVICE_ID_LEN)
		{
			len = MAX_SIPD_DEVICE_ID_LEN;
		}
		else
		{
			len = (p - pSdpSession->u_field);
		}
		strncpy(invite_adaptor->channel_id, pSdpSession->u_field, len);

		play_type = atoi(p+1);
		if(0 == play_type)
		{
			invite_adaptor->Type = SIPD_RECORD_INFO_TYPE_TIME;
		}
		else if(1 == play_type)
		{
			invite_adaptor->Type = SIPD_RECORD_INFO_TYPE_ALARM;
		}
		else if(2 == play_type)
		{
			invite_adaptor->Type = SIPD_RECORD_INFO_TYPE_MANUAL;
		}
		else if(3 == play_type)
		{
			invite_adaptor->Type = SIPD_RECORD_INFO_TYPE_ALL;
		}
		else
		{
			invite_adaptor->Type = SIPD_RECORD_INFO_TYPE_ALL;
		}
		
		//printf("Invite_adaptor_ind channel_id=%s play_type(U_field)=%d Type=%d u_field=%s \n", invite_adaptor.channel_id, play_type, invite_adaptor.Type, pSdpSession->u_field);
		
	}
	else
	{
		strncpy(invite_adaptor->channel_id, pSdpSession->u_field, MAX_SIPD_DEVICE_ID_LEN);
		invite_adaptor->Type = SIPD_RECORD_INFO_TYPE_ALL;
		//printf("Invite_adaptor_ind channel_id=%s  Type=%d u_field=%s \n", invite_adaptor.channel_id, invite_adaptor.Type, pSdpSession->u_field);
	}

	invite_adaptor->stream_type = pSdpSession->stream_type;
	invite_adaptor->media_recv_ip_type = SIPD_IPType_IPv4;
	strncpy(invite_adaptor->media_recv_ip, pSdpSession->connect_video_ip, MAX_SIPD_URI);
	strncpy(invite_adaptor->media_recv_port, pSdpSession->connect_video_port, MAX_SIPD_NORMAL_LEN);
	invite_adaptor->media_type = pSdpSession->media_type;
	strncpy(invite_adaptor->t_start_time, pSdpSession->t_start_time, MAX_SIPD_NORMAL_LEN);
	strncpy(invite_adaptor->t_end_time, pSdpSession->t_end_time, MAX_SIPD_NORMAL_LEN);
	strncpy(invite_adaptor->ssrc, pSdpSession->pri_ssrc_field, MAX_SIPD_NORMAL_LEN);
	
	invite_adaptor->f_video_type = pSdpSession->f_video_type;
	invite_adaptor->f_resolution = pSdpSession->f_resolution;
	invite_adaptor->f_framerate = pSdpSession->f_framerate;
	invite_adaptor->f_rate_type = pSdpSession->f_rate_type;
	invite_adaptor->bitrate = pSdpSession->bitrate;
	invite_adaptor->audio_type = pSdpSession->audio_type;
	invite_adaptor->audio_bitrate = pSdpSession->audio_bitrate;
	invite_adaptor->audio_samplerate = pSdpSession->audio_samplerate;
	
	
	
	/*TESTCODE*/
	#if 0
	{
		sipd_sender_sdp_info temp_sdp;

		memset(&temp_sdp, 0, sizeof(temp_sdp));
		temp_sdp.did = invite_adaptor.did;
		temp_sdp.tid = invite_adaptor.tid;
		strncpy(temp_sdp.media_sender_id, "35010400001320000001", MAX_SIPD_DEVICE_ID_LEN);
		strcpy(temp_sdp.media_send_ip, "192.168.178.47");
		strcpy(temp_sdp.media_send_port, "70002");
		strcpy(temp_sdp.ssrc, invite_adaptor.ssrc);

		memset(&test_media_session, 0, sizeof(test_media_session));
		memcpy(&test_media_session, &invite_adaptor, sizeof(test_media_session));

		SendMessageEx(MOD_SIPD, MOD_SIPD, MSG_ID_GW_MEDIA_RSP, &temp_sdp, sizeof(temp_sdp));
		
		
	}
	#endif

	return 0;
}

int build_and_send_invite(sipd_message_query_or_response * msg, int isWithSDP)
{
	osip_message_t *invite = NULL;
	int ret ;
	int call_id; 

	ret = eXosip_call_build_initial_invite (context_eXosip, &invite,msg->to_uri,   
		msg->from_uri,   
		NULL,   
		msg->subject);  

	if(isWithSDP)
	{
		osip_message_set_body (invite, msg->body, strlen(msg->body));
		osip_message_set_content_type (invite, "application/sdp");  
	}
	

	eXosip_lock (context_eXosip);
	call_id = eXosip_call_send_initial_invite (context_eXosip, invite);  
	eXosip_unlock (context_eXosip);

	msg->cid = call_id;
	return 0;
}

int build_and_send_ack(const sipd_message_query_or_response * msg, int isWithSDP)
{
	osip_message_t *ack = NULL;
	int ret ;
	int call_id; 

	ret = eXosip_call_build_ack(context_eXosip, msg->did, &ack);  

	if(isWithSDP)
	{
		osip_message_set_body (ack, msg->body, strlen(msg->body));
		osip_message_set_content_type (ack, "application/sdp");  
	}


	eXosip_lock (context_eXosip);
	call_id = eXosip_call_send_ack(context_eXosip, msg->did, ack);  
	eXosip_unlock (context_eXosip);

	return 0;
}

int build_and_send_bye(const sipd_message_query_or_response * msg)
{
	osip_message_t *invite = NULL;
	int ret ;
	int call_id; 

	
	eXosip_lock (context_eXosip);
	call_id = eXosip_call_terminate (context_eXosip, msg->cid, msg->did);  
	eXosip_unlock (context_eXosip);

	return 0;
}


int build_and_send_invite_200_ok(sipd_message_query_or_response * msg, int isWithSDP)
{
	int ret=0;
	osip_message_t *answer = NULL;

	context_eXosip_lock();
	eXosip_lock (context_eXosip);

	ret = eXosip_call_build_answer (context_eXosip, msg->tid, 200, &answer);
	if (ret != OSIP_SUCCESS)
	{
		printf ("build_and_send_invite_200_ok This request msg is invalid!Cann't response! ERROR!!\n");
		eXosip_call_send_answer (context_eXosip, msg->tid, 400, NULL);
	}
	else
	{
		if(isWithSDP)
		{
			osip_message_set_body (answer, msg->body, strlen(msg->body));
			osip_message_set_content_type (answer, "application/sdp");  
		}
		
		ret = eXosip_call_send_answer (context_eXosip, msg->tid, 200, answer);
		if (ret != OSIP_SUCCESS)
		{
			printf ("build_and_send_invite_200_ok INVITE send 200 ERROR! [eXosip_call_send_answer] \n");
		}
		//printf("build_and_send_invite_200_ok INVITE send 200 over! OK\n", __FUNCTION__, __LINE__);
	}

	eXosip_unlock (context_eXosip);
	context_eXosip_unlock();
	return 0;
}

int build_and_send_invite_200_ok_old(const sipd_sender_sdp_info* pInviteRspSdp, int isSender)
{
	char local_sdp[MAX_SIPD_BODY_LENGTH];
	int ret=0;
	osip_message_t *answer = NULL;

	memset(local_sdp, 0, sizeof(local_sdp));

	if(isSender)
		sip_sender_sdp_create(pInviteRspSdp, local_sdp);
	else
		sip_receiver_sdp_create(pInviteRspSdp, local_sdp);

	context_eXosip_lock();
	eXosip_lock (context_eXosip);
	
       ret = eXosip_call_build_answer (context_eXosip, pInviteRspSdp->tid, 200, &answer);
       if (ret != OSIP_SUCCESS)
       {
          	printf ("build_and_send_invite_200_ok This request msg is invalid!Cann't response! ERROR!!\n");
              eXosip_call_send_answer (context_eXosip, pInviteRspSdp->tid, 400, NULL);
       }
       else
       {
          	osip_message_set_body (answer, local_sdp, strlen(local_sdp));
          	osip_message_set_content_type (answer, "application/sdp");

		   #if 0
		   {
			  	char *dest=NULL;
				size_t length=0;
				int ret;
				
				ret = osip_message_to_str(answer, &dest, &length);
				if (ret !=OSIP_SUCCESS) 
				{
					printf("%s:%d osip_message_to_str failed\n", __FUNCTION__, __LINE__);
				}
				printf("%s line=%d INVITE return 200 OK with SDP:\n%s\n", __FUNCTION__, __LINE__, dest);
				osip_free(dest);
		   }
		   #endif
          
          	ret = eXosip_call_send_answer (context_eXosip, pInviteRspSdp->tid, 200, answer);
		if (ret != OSIP_SUCCESS)
		{
			printf ("build_and_send_invite_200_ok INVITE send 200 ERROR! [eXosip_call_send_answer] \n");
		}
          	//printf("build_and_send_invite_200_ok INVITE send 200 over! OK\n", __FUNCTION__, __LINE__);
        }

	eXosip_unlock (context_eXosip);
       context_eXosip_unlock();

	return 0;
}


int build_and_send_file_to_end_message(int tid, int did, const char* deviceID)
{
	osip_message_t *message = NULL;
	char buffer[MAX_XML_BUFFER_LEN+1]={0};
	int ret;

	//printf("build_and_send_file_to_end_message deviceID=%s \n",  deviceID);
	
	memset(buffer, 0, sizeof(buffer));
	snprintf(buffer,MAX_XML_BUFFER_LEN,
	"<?xml version=\"1.0\"?>\r\n"\
	"<Notify>\r\n"\
	"<CmdType>MediaStatus</CmdType>\r\n"\
	"<SN>%u</SN>\r\n"\
	"<DeviceID>%s</DeviceID>\r\n"\
	"<NotifyType>121</NotifyType>\r\n"\
	"</Notify>",
	get_device_sn(), deviceID);
	
	context_eXosip_lock();
	eXosip_lock(context_eXosip);
	ret = eXosip_call_build_request(context_eXosip, did, "MESSAGE", &message);
	if(ret != OSIP_SUCCESS)
	{
	 	eXosip_unlock(context_eXosip);
	 	printf("build_and_send_file_to_end_message eXosip_call_build_request failed ret=%d\n",  ret);
		if(message != NULL)
		{
			osip_message_free(message);
		}
		context_eXosip_unlock();
		return -1;
	 }
	
	ret = osip_message_set_body(message, buffer, strlen(buffer));
	if(ret != OSIP_SUCCESS)
	{
	 	eXosip_unlock(context_eXosip);
	 	printf("build_and_send_file_to_end_message osip_message_set_body failed ret=%d\n",  ret);
		if(message != NULL)
		{
			osip_message_free(message);
		}
		
		context_eXosip_unlock();
		return -1;
	 }

	/*accordint to GB/T 28181-2011, there is no content_type, but specs is not perfert :<*/
	ret = osip_message_set_content_type (message, "Application/MANSCDP+xml");
	if(ret != OSIP_SUCCESS)
	{
	 	eXosip_unlock(context_eXosip);
	 	printf("build_and_send_file_to_end_message osip_message_set_content_type failed ret=%d\n",  ret);
		if(message != NULL)
		{
			osip_message_free(message);
		}
		context_eXosip_unlock();
		return -1;
	 }
	
	ret = eXosip_call_send_request(context_eXosip, did, message);
	if(ret != OSIP_SUCCESS)
	{
	 	eXosip_unlock(context_eXosip);
	 	printf("build_and_send_file_to_end_message eXosip_call_send_request failed ret=%d\n",  ret);
		if(message != NULL)
		{
			osip_message_free(message);
		}
		context_eXosip_unlock();
		return -1;
	 }
	
	eXosip_unlock(context_eXosip);
	context_eXosip_unlock();
	return 0;
}


static int get_recordinfo_pair(int start_index, struct sipd_recorditem_pair* pitem_pair, const sipd_recordinfo_adaptor_rsp *prsp)
{
	int len=0;
	
	if(NULL == pitem_pair  || NULL == prsp)
	{
		printf("get_recordinfo_pair input param error!\n");
		return -1;
	}


	//printf("get_recordinfo_pair start_index=%d \n", start_index);

	if(start_index > (prsp->basic_info.RecordListNum-1))
	{
		printf("get_recordinfo_pair already get last item. Warning! RecordListNum=%d\n", prsp->basic_info.RecordListNum);
		return -1;
	}

	
	
	if(start_index == (prsp->basic_info.RecordListNum-1))
	{
		pitem_pair->is_last_one_item = 1;
	}
	else
	{
		pitem_pair->is_last_one_item = 0;
	}

	/*item1*/
	snprintf(pitem_pair->item_pair_buffer[0],MAX_SIPD_FILEITEM_LEN,
	"<Item>\r\n"\
	"<DeviceID>%s</DeviceID>\r\n"\
	"<Name>%s</Name>\r\n",
	prsp->file_items[start_index].device_id, prsp->file_items[start_index].Name);


	if(strlen(prsp->file_items[start_index].FilePath) > 0)
	{
		len = strlen(pitem_pair->item_pair_buffer[0]);
		snprintf(pitem_pair->item_pair_buffer[0]+len, (MAX_SIPD_FILEITEM_LEN-len),"<FilePath>%s</FilePath>\r\n", prsp->file_items[start_index].FilePath);
	}

	if(strlen(prsp->file_items[start_index].Address) > 0)
	{
		len = strlen(pitem_pair->item_pair_buffer[0]);
		snprintf(pitem_pair->item_pair_buffer[0]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Address>%s</Address>\r\n", prsp->file_items[start_index].Address);
	}

	if(strlen(prsp->file_items[start_index].StartTime) > 0)
	{
		len = strlen(pitem_pair->item_pair_buffer[0]);
		snprintf(pitem_pair->item_pair_buffer[0]+len, (MAX_SIPD_FILEITEM_LEN-len),"<StartTime>%s</StartTime>\r\n", prsp->file_items[start_index].StartTime);
	}

	if(strlen(prsp->file_items[start_index].EndTime) > 0)
	{
		len = strlen(pitem_pair->item_pair_buffer[0]);
		snprintf(pitem_pair->item_pair_buffer[0]+len, (MAX_SIPD_FILEITEM_LEN-len),"<EndTime>%s</EndTime>\r\n", prsp->file_items[start_index].EndTime);
	}

	len = strlen(pitem_pair->item_pair_buffer[0]);
	snprintf(pitem_pair->item_pair_buffer[0]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Secrecy>%d</Secrecy>\r\n", prsp->file_items[start_index].Secrecy);

	
	if(SIPD_RECORD_INFO_TYPE_TIME == prsp->file_items[start_index].Type )
	{
		len = strlen(pitem_pair->item_pair_buffer[0]);
		snprintf(pitem_pair->item_pair_buffer[0]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Type>%s</Type>\r\n", XML_Value_Type_time);
	}
	else if(SIPD_RECORD_INFO_TYPE_ALARM == prsp->file_items[start_index].Type )
	{
		len = strlen(pitem_pair->item_pair_buffer[0]);
		snprintf(pitem_pair->item_pair_buffer[0]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Type>%s</Type>\r\n", XML_Value_Type_alarm);
	}
	else if(SIPD_RECORD_INFO_TYPE_MANUAL == prsp->file_items[start_index].Type )
	{
		len = strlen(pitem_pair->item_pair_buffer[0]);
		snprintf(pitem_pair->item_pair_buffer[0]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Type>%s</Type>\r\n", XML_Value_Type_manual);
	}
	else
	{
		len = strlen(pitem_pair->item_pair_buffer[0]);
		snprintf(pitem_pair->item_pair_buffer[0]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Type>%s</Type>\r\n", XML_Value_Type_all);
	}

	if(strlen(prsp->file_items[start_index].RecorderID) > 0)
	{
		len = strlen(pitem_pair->item_pair_buffer[0]);
		snprintf(pitem_pair->item_pair_buffer[0]+len, (MAX_SIPD_FILEITEM_LEN-len),"<RecorderID>%s</RecorderID>\r\n", prsp->file_items[start_index].RecorderID);
	}
	
	len = strlen(pitem_pair->item_pair_buffer[0]);
	snprintf(pitem_pair->item_pair_buffer[0]+len, (MAX_SIPD_FILEITEM_LEN-len),"</Item>\r\n");

	/*item2*/
	if(0 == pitem_pair->is_last_one_item)
	{
		snprintf(pitem_pair->item_pair_buffer[1],MAX_SIPD_FILEITEM_LEN,
		"<Item>\r\n"\
		"<DeviceID>%s</DeviceID>\r\n"\
		"<Name>%s</Name>\r\n",
		prsp->file_items[start_index+1].device_id, prsp->file_items[start_index+1].Name);


		if(strlen(prsp->file_items[start_index+1].FilePath) > 0)
		{
			len = strlen(pitem_pair->item_pair_buffer[1]);
			snprintf(pitem_pair->item_pair_buffer[1]+len, (MAX_SIPD_FILEITEM_LEN-len),"<FilePath>%s</FilePath>\r\n", prsp->file_items[start_index+1].FilePath);
		}

		if(strlen(prsp->file_items[start_index+1].Address) > 0)
		{
			len = strlen(pitem_pair->item_pair_buffer[1]);
			snprintf(pitem_pair->item_pair_buffer[1]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Address>%s</Address>\r\n", prsp->file_items[start_index+1].Address);
		}

		if(strlen(prsp->file_items[start_index+1].StartTime) > 0)
		{
			len = strlen(pitem_pair->item_pair_buffer[1]);
			snprintf(pitem_pair->item_pair_buffer[1]+len, (MAX_SIPD_FILEITEM_LEN-len),"<StartTime>%s</StartTime>\r\n", prsp->file_items[start_index+1].StartTime);
		}

		if(strlen(prsp->file_items[start_index+1].EndTime) > 0)
		{
			len = strlen(pitem_pair->item_pair_buffer[1]);
			snprintf(pitem_pair->item_pair_buffer[1]+len, (MAX_SIPD_FILEITEM_LEN-len),"<EndTime>%s</EndTime>\r\n", prsp->file_items[start_index+1].EndTime);
		}

		len = strlen(pitem_pair->item_pair_buffer[1]);
		snprintf(pitem_pair->item_pair_buffer[1]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Secrecy>%d</Secrecy>\r\n", prsp->file_items[start_index+1].Secrecy);

		if(SIPD_RECORD_INFO_TYPE_TIME == prsp->file_items[start_index+1].Type )
		{
			len = strlen(pitem_pair->item_pair_buffer[1]);
			snprintf(pitem_pair->item_pair_buffer[1]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Type>%s</Type>\r\n", XML_Value_Type_time);
		}
		else if(SIPD_RECORD_INFO_TYPE_ALARM == prsp->file_items[start_index+1].Type )
		{
			len = strlen(pitem_pair->item_pair_buffer[1]);
			snprintf(pitem_pair->item_pair_buffer[1]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Type>%s</Type>\r\n", XML_Value_Type_alarm);
		}
		else if(SIPD_RECORD_INFO_TYPE_MANUAL == prsp->file_items[start_index+1].Type )
		{
			len = strlen(pitem_pair->item_pair_buffer[1]);
			snprintf(pitem_pair->item_pair_buffer[1]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Type>%s</Type>\r\n", XML_Value_Type_manual);
		}
		else
		{
			len = strlen(pitem_pair->item_pair_buffer[1]);
			snprintf(pitem_pair->item_pair_buffer[1]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Type>%s</Type>\r\n", XML_Value_Type_all);
		}

		if(strlen(prsp->file_items[start_index+1].RecorderID) > 0)
		{
			len = strlen(pitem_pair->item_pair_buffer[1]);
			snprintf(pitem_pair->item_pair_buffer[1]+len, (MAX_SIPD_FILEITEM_LEN-len),"<RecorderID>%s</RecorderID>\r\n", prsp->file_items[start_index+1].RecorderID);
		}
		
		len = strlen(pitem_pair->item_pair_buffer[1]);
		snprintf(pitem_pair->item_pair_buffer[1]+len, (MAX_SIPD_FILEITEM_LEN-len),"</Item>\r\n");
	}


	return 0;
}


static int send_recordinfo_pair_rsp(const sipd_recordinfo_adaptor_rsp *prsp, char* body_buffer)
{
	osip_message_t *message = NULL;
	int ret;

	if(NULL == prsp  || NULL == body_buffer)
	{
		return -1;
	}

	context_eXosip_lock();
	eXosip_lock(context_eXosip);
	//printf("send_recordinfo_pair_rsp to=%s from=%s \n", prsp->basic_info.to_uri, prsp->basic_info.from_uri);
	ret = eXosip_message_build_request(context_eXosip, &message, "MESSAGE", prsp->basic_info.to_uri, prsp->basic_info.from_uri, NULL);
	 if(ret != OSIP_SUCCESS)
	 {
	 	eXosip_unlock(context_eXosip);
	 	printf("send_recordinfo_pair_rsp eXosip_message_build_request failed\n");
		if(message != NULL)
		{
			osip_message_free(message);
		}
		context_eXosip_unlock();
		return -1;
	 }

	 ret = osip_message_set_body(message, body_buffer, strlen(body_buffer));
	  if(ret != OSIP_SUCCESS)
	 {
	 	eXosip_unlock(context_eXosip);
	 	printf("send_recordinfo_pair_rsp osip_message_set_body failed\n");
		if(message != NULL)
		{
			osip_message_free(message);
		}
		context_eXosip_unlock();
		return -1;
	 }
	  
	 ret = osip_message_set_content_type (message, "Application/MANSCDP+xml");
	  if(ret != OSIP_SUCCESS)
	 {
	 	eXosip_unlock(context_eXosip);
	 	printf("send_recordinfo_pair_rsp osip_message_set_content_type failed\n");
		if(message != NULL)
		{
			osip_message_free(message);
		}
		context_eXosip_unlock();
		return -1;
	 }
	
	#if 0
	  {
	  	char *dest=NULL;
		size_t length=0;
		ret = osip_message_to_str(message, &dest, &length);
		if (ret !=0) 
		{
			printf("%s:%d osip_message_to_str failed\n", __FUNCTION__, __LINE__);
		}
		printf("%s line=%d message:\n%s\n", __FUNCTION__, __LINE__, dest);
		osip_free(dest);
	 }
	  #endif
	 
	eXosip_message_send_request (context_eXosip, message);
	eXosip_unlock(context_eXosip);
	context_eXosip_unlock();

	return 0;
}



int build_and_send_recordinfo_rsp(const sipd_recordinfo_adaptor_rsp *prsp)
{
	char buffer[MAX_SIPD_RECORD_INFO_LEN+1] = {0};
	struct sipd_recorditem_pair item_pair;
	int index = 0, ret=0;
	int len= 0;

	
	
	if(NULL == prsp)
	{
		return -1;
	}

	memset(buffer, 0, sizeof(buffer));
	
	if(0 == prsp->basic_info.SumNum)
	{
	      memset(buffer, 0, sizeof(buffer));
		snprintf(buffer,MAX_SIPD_RECORD_INFO_LEN,
		"<?xml version=\"1.0\"?>\r\n"\
		"<Response>\r\n"\
		"<CmdType>RecordInfo</CmdType>\r\n"\
		"<SN>%s</SN>\r\n"\
		"<DeviceID>%s</DeviceID>\r\n"\
		"<Name>%s</Name>\r\n"\
		"<SumNum>%d</SumNum>\r\n"\
		"</Response>",
		prsp->basic_info.SN, prsp->basic_info.device_id, prsp->basic_info.Name, prsp->basic_info.SumNum);

		if(Get_EvWaitFlg() == 1)
		{
			send_recordinfo_pair_rsp(prsp, buffer);
		}
			
	}
	else
	{
		memset(buffer, 0, sizeof(buffer));
		snprintf(buffer,MAX_SIPD_RECORD_INFO_LEN,
			"<?xml version=\"1.0\"?>\r\n"\
			"<Response>\r\n"\
			"<CmdType>RecordInfo</CmdType>\r\n"\
			"<SN>%s</SN>\r\n"\
			"<DeviceID>%s</DeviceID>\r\n"\
			"<Name>%s</Name>\r\n"\
			"<SumNum>%d</SumNum>\r\n",
			prsp->basic_info.SN, prsp->basic_info.device_id, prsp->basic_info.Name, prsp->basic_info.SumNum);

		len = strlen(buffer);
		snprintf(buffer+len, (MAX_SIPD_RECORD_INFO_LEN-len),"<RecordList Num=\"%d\">\r\n", prsp->basic_info.RecordListNum);

		for(index = 0; index < (prsp->basic_info.RecordListNum); index+=2)
		{
			memset(&item_pair, 0, sizeof(item_pair));
			ret = get_recordinfo_pair(index, &item_pair, prsp);
			if(ret != 0)
			{
				printf("build_and_send_recordinfo_rsp get_recordinfo_pair error! ret=%d \n", ret);
				break;
			}

			if(item_pair.is_last_one_item)
			{
				len = strlen(buffer);
				snprintf(buffer+len, (MAX_SIPD_RECORD_INFO_LEN-len), item_pair.item_pair_buffer[0]);
			
			}
			else
			{
				len = strlen(buffer);
				snprintf(buffer+len, (MAX_SIPD_RECORD_INFO_LEN-len), item_pair.item_pair_buffer[0]);

				len = strlen(buffer);
				snprintf(buffer+len, (MAX_SIPD_RECORD_INFO_LEN-len), item_pair.item_pair_buffer[1]);
			}	
		}

		len = strlen(buffer);
		snprintf(buffer+len, (MAX_SIPD_RECORD_INFO_LEN-len),"</RecordList>\r\n</Response>");

		if(Get_EvWaitFlg() == 1)
		{
			send_recordinfo_pair_rsp(prsp, buffer);
		}
	}
	
	return 0;
}

static int get_media_session_priv(char* ori_str, struct sipd_invite_media_session* media_session)
{
	char* p=NULL;
	char* p_locator=NULL;
	char* p_locator_end=NULL;
	char temp_buffer[MAX_SIPD_NORMAL_LEN] = {0};

	//printf("%s line=%d ori_str=%s\n", __FUNCTION__, __LINE__, ori_str);
	
	if(NULL == ori_str || NULL == media_session)
	{
		return -1;
	}

	p = ori_str;
	p = strstr(p,"m=video");
	if(p != NULL)
	{
		//printf("%s line=%d find m=video, try to find ssrc for video (y=)\n", __FUNCTION__, __LINE__);
		p_locator = strstr(p,"y=");
		if(NULL == p_locator)
		{
			printf("%s line=%d can not find y= in remote sdp [error! Noticing]\n", __FUNCTION__, __LINE__);
			strcpy(media_session->pri_ssrc_field, SIPD_RTP_DEFAULT_VIDEO_SSRC);
		}
		else
		{
			p_locator_end = strchr(p_locator, '\r');
			if(NULL != p_locator_end)
			{	
				//printf("%s line=%d (p_locator_end - (p_locator+2))=%d\n", __FUNCTION__, __LINE__, (p_locator_end - (p_locator+2)));
				strncpy(media_session->pri_ssrc_field, p_locator+2, (p_locator_end - (p_locator+2))); 
			}
			else
			{
				//printf("%s line=%d can't find CLr, Noticing in y=! ERROR!! try to find CR...\n", __FUNCTION__, __LINE__);
				p_locator_end = strchr(p_locator, '\n');
				if(NULL != p_locator_end)
				{	
					//printf("%s line=%d (p_locator_end - (p_locator+2))=%d\n", __FUNCTION__, __LINE__, (p_locator_end - (p_locator+2)));
					strncpy(media_session->pri_ssrc_field, p_locator+2, (p_locator_end - (p_locator+2))); /*skip first digital, real time or history*/
					//printf("%s line=%d find CR pri_ssrc_field=%s\n", __FUNCTION__, __LINE__, media_session->pri_ssrc_field);
				}
				else
				{
					//printf("%s line=%d can't find CLr and CR, Noticing in y=! Real ERROR!!!\n", __FUNCTION__, __LINE__);
					strcpy(media_session->pri_ssrc_field, SIPD_RTP_DEFAULT_VIDEO_SSRC);
				}
			}
		}
	}

	p = ori_str;
	p = strstr(p,"m=audio");
	if(p != NULL)
	{
		//printf("%s line=%d find m=audio, try to find ssrc for audio (y=)\n", __FUNCTION__, __LINE__);
		p_locator = strstr(p,"y=");
		if(NULL == p_locator)
		{
			//printf("%s line=%d can not find y= in remote sdp [error! Noticing]\n", __FUNCTION__, __LINE__);
			strcpy(media_session->pri_ssrc_audio_field, SIPD_RTP_DEFAULT_AUDIO_SSRC);
		}
		else
		{
			p_locator_end = strchr(p_locator, '\r');
			if(NULL != p_locator_end)
			{	
				//printf("%s line=%d (p_locator_end - (p_locator+2))=%d\n", __FUNCTION__, __LINE__, (p_locator_end - (p_locator+2)));
				strncpy(media_session->pri_ssrc_audio_field, p_locator+2, (p_locator_end - (p_locator+2))); 
			}
			else
			{
				//printf("%s line=%d can't find CLr, Noticing in y=! ERROR!! try to find CR...\n", __FUNCTION__, __LINE__);
				p_locator_end = strchr(p_locator, '\n');
				if(NULL != p_locator_end)
				{	
					//printf("%s line=%d (p_locator_end - (p_locator+2))=%d\n", __FUNCTION__, __LINE__, (p_locator_end - (p_locator+2)));
					strncpy(media_session->pri_ssrc_audio_field, p_locator+2, (p_locator_end - (p_locator+2))); 
					//printf("%s line=%d find CR pri_ssrc_audio_field=%s\n", __FUNCTION__, __LINE__, media_session->pri_ssrc_audio_field);
				}
				else
				{
					//printf("%s line=%d can't find CLr and CR, Noticing in y=! Real ERROR!!!\n", __FUNCTION__, __LINE__);
					strcpy(media_session->pri_ssrc_audio_field, SIPD_RTP_DEFAULT_AUDIO_SSRC);
				}			
			}
		}
	}

	p = ori_str;
	p_locator = strstr(p,"f=");
	if(NULL == p_locator)
	{
		//printf("%s line=%d can not find f= in remote sdp. Notice\n", __FUNCTION__, __LINE__);
	}
	else
	{
		p_locator_end = strchr(p_locator, '\r');
		if(NULL != p_locator_end)
		{	
			//printf("%s line=%d (p_locator_end - (p_locator+2))=%d\n", __FUNCTION__, __LINE__, (p_locator_end - (p_locator+2)));
			strncpy(media_session->pri_f_field, p_locator+2, (p_locator_end - (p_locator+2))); 
		}
		else
		{
			//printf("%s line=%d can't find CLr in f=, Noticing! ERROR. try to find CR...\n", __FUNCTION__, __LINE__);
			p_locator_end = strchr(p_locator, '\n');
			if(NULL != p_locator_end)
			{	
				//printf("%s line=%d (p_locator_end - (p_locator+2))=%d\n", __FUNCTION__, __LINE__, (p_locator_end - (p_locator+2)));
				strncpy(media_session->pri_f_field, p_locator+2, (p_locator_end - (p_locator+2))); 
			}
			else
			{
				//printf("%s line=%d can't find CLr and CR in f=, Noticing! ERROR!!!\n", __FUNCTION__, __LINE__);
			}
			
		}
	}

	if(strlen(media_session->pri_f_field) > 0)
	{
		char* pos= NULL;
		char* pos2 = NULL;

		//printf("%s line=%d f field in sdp is exist something there need to analyze, f=%s \n", __FUNCTION__, __LINE__, media_session->pri_f_field);
		

		p = NULL;
		p_locator = NULL;
		p_locator_end = NULL;
		
		p = media_session->pri_f_field;
		p_locator = strstr(p,"v/");
		p_locator_end = strstr(p, "a/");
		if(NULL == p_locator)
		{
			//printf("%s line=%d can't find v/ in f field. ERROR\n", __FUNCTION__, __LINE__);
			media_session->f_enable = 0;
		}
		else if (NULL == p_locator_end)
		{
			//printf("%s line=%d can't find a/ in f field. ERROR\n", __FUNCTION__, __LINE__);
			media_session->f_enable = 0;
		}
		else
		{
			media_session->f_enable = 1;


			memset(temp_buffer, 0, sizeof(temp_buffer));
			/*find coding type*/
			pos = strchr(p_locator+2, '/');
			if(pos != NULL)
			{
				if(*(p_locator+2) == '/')
				{
					//printf("%s line=%d video coding type is empty\n", __FUNCTION__, __LINE__);
					media_session->f_video_type = 0;
				}
				else
				{
					strncpy(temp_buffer, p_locator+2, pos-(p_locator+2));
					//printf("%s line=%d video coding type=%s strlen(temp_buffer)=%d\n", __FUNCTION__, __LINE__, temp_buffer, strlen(temp_buffer));
					if(strlen(temp_buffer) > 0)
					{
						media_session->f_video_type = atoi(temp_buffer);
						//printf("%s line=%d video coding type=%d\n", __FUNCTION__, __LINE__, media_session->f_video_type);
					}
				}
			}
			else
			{
				//printf("%s line=%d is something wrong in f string format\n", __FUNCTION__, __LINE__);
				media_session->f_enable = 0;
				goto F_FORMAT_ERROR;
			}

			/*resolution*/
			if(*(pos+1) == '/')
			{
				//printf("%s line=%d video resolution is empty\n", __FUNCTION__, __LINE__);
				media_session->f_resolution = 0;
				pos2 = pos+1;
			}
			else
			{
				pos2 = strchr(pos+1, '/');
				if(pos2 != NULL)
				{
					memset(temp_buffer, 0, sizeof(temp_buffer));
					strncpy(temp_buffer, pos+1, pos2 -(pos+1));
					//printf("%s line=%d video resolution=%s strlen(temp_buffer)=%d\n", __FUNCTION__, __LINE__, temp_buffer, strlen(temp_buffer));
					if(strlen(temp_buffer) > 0)
					{
						media_session->f_resolution = atoi(temp_buffer);
						//printf("%s line=%d video resolution=%d\n", __FUNCTION__, __LINE__, media_session->f_resolution);
					}
				}
				else
				{
					//printf("%s line=%d is something wrong in f string format\n", __FUNCTION__, __LINE__);
					media_session->f_enable = 0;
					goto F_FORMAT_ERROR;
				}
			}

			/*framerate*/
			if(*(pos2+1) == '/')
			{
				//printf("%s line=%d video framerate is empty\n", __FUNCTION__, __LINE__);
				media_session->f_framerate = 0;
				pos = pos2+1;
			}
			else
			{
				pos = strchr(pos2+1, '/');
				if(pos != NULL)
				{
					memset(temp_buffer, 0, sizeof(temp_buffer));
					strncpy(temp_buffer, pos2+1, pos - (pos2+1));
					//printf("%s line=%d video framerate=%s strlen(temp_buffer)=%d\n", __FUNCTION__, __LINE__, temp_buffer, strlen(temp_buffer));
					if(strlen(temp_buffer) > 0)
					{
						media_session->f_framerate = atoi(temp_buffer);
						//printf("%s line=%d video framerate=%d\n", __FUNCTION__, __LINE__, media_session->f_framerate);
					}
				}
				else
				{
					//printf("%s line=%d is something wrong in f string format\n", __FUNCTION__, __LINE__);
					media_session->f_enable = 0;
					goto F_FORMAT_ERROR;
				}
			}


			/*rate type , cbr or vbr*/
			if(*(pos+1) == '/')
			{
				//printf("%s line=%d video rate type is empty\n", __FUNCTION__, __LINE__);
				media_session->f_rate_type = 0;
				pos2 = pos+1;
			}
			else
			{
				pos2 = strchr(pos+1, '/');
				if(pos2 != NULL)
				{
					memset(temp_buffer, 0, sizeof(temp_buffer));
					strncpy(temp_buffer, pos+1, pos2 -(pos+1));
					//printf("%s line=%d video rate_type=%s strlen(temp_buffer)=%d\n", __FUNCTION__, __LINE__, temp_buffer, strlen(temp_buffer));
					if(strlen(temp_buffer) > 0)
					{
						media_session->f_rate_type = atoi(temp_buffer);
						//printf("%s line=%d video f_rate_type=%d\n", __FUNCTION__, __LINE__, media_session->f_rate_type);
					}
				}
				else
				{
					//printf("%s line=%d is something wrong in f string format\n", __FUNCTION__, __LINE__);
					media_session->f_enable = 0;
					goto F_FORMAT_ERROR;
				}
			}

			/*video bitrate*/
			if(*(pos2+1) == 'a')
			{
				//printf("%s line=%d video bitrate is empty\n", __FUNCTION__, __LINE__);
				media_session->bitrate = 0;
			}
			else
			{
				pos = strchr(pos2+1, 'a');
				if(pos != NULL)
				{
					memset(temp_buffer, 0, sizeof(temp_buffer));
					strncpy(temp_buffer, pos2+1, pos-(pos2+1));
					//printf("%s line=%d video bitrate=%s strlen(temp_buffer)=%d\n", __FUNCTION__, __LINE__, temp_buffer, strlen(temp_buffer));
					if(strlen(temp_buffer) > 0)
					{
						media_session->bitrate = atoi(temp_buffer);
						//printf("%s line=%d video bitrate=%d\n", __FUNCTION__, __LINE__, media_session->bitrate);
					}
				}
				else
				{
					//printf("%s line=%d is something wrong in f string format\n", __FUNCTION__, __LINE__);
					media_session->f_enable = 0;
					goto F_FORMAT_ERROR;
				}
			}
			

			/*audio*/
			if(*(p_locator_end+2) == '/')
			{
				//printf("%s line=%d audio coding type is empty\n", __FUNCTION__, __LINE__);
				media_session->audio_type = 0;
				pos = p_locator_end+2;
			}
			else
			{
				pos = strchr(p_locator_end+2, '/');
				if(pos != NULL)
				{
					memset(temp_buffer, 0, sizeof(temp_buffer));
					strncpy(temp_buffer, p_locator_end+2, pos-(p_locator_end+2));
					//printf("%s line=%d audio coding_type=%s strlen(temp_buffer)=%d\n", __FUNCTION__, __LINE__, temp_buffer, strlen(temp_buffer));
					if(strlen(temp_buffer) > 0)
					{
						media_session->audio_type = atoi(temp_buffer);
						//printf("%s line=%d audio audio_type=%d\n", __FUNCTION__, __LINE__, media_session->audio_type);
					}
				}
				else
				{
					//printf("%s line=%d is something wrong in f string format\n", __FUNCTION__, __LINE__);
					media_session->f_enable = 0;
					goto F_FORMAT_ERROR;
				}
			}


			/*audio bitrate*/
			if(*(pos+1) == '/')
			{
				//printf("%s line=%d audio birate is empty\n", __FUNCTION__, __LINE__);
				media_session->audio_bitrate = 0;
				pos2 = pos+1;
			}
			else
			{
				pos2 = strchr(pos, '/');
				if(pos2 != NULL)
				{
					memset(temp_buffer, 0, sizeof(temp_buffer));
					strncpy(temp_buffer, pos+1, pos2-(pos+1));
					//printf("%s line=%d audio birate=%s strlen(temp_buffer)=%d\n", __FUNCTION__, __LINE__, temp_buffer, strlen(temp_buffer));
					if(strlen(temp_buffer) > 0)
					{
						media_session->audio_bitrate = atoi(temp_buffer);
						//printf("%s line=%d audio audio_bitrate=%d\n", __FUNCTION__, __LINE__, media_session->audio_bitrate);
					}
				}
				else
				{
					//printf("%s line=%d is something wrong in f string format\n", __FUNCTION__, __LINE__);
					media_session->f_enable = 0;
					goto F_FORMAT_ERROR;
				}
			}

			/*audio sample rate*/
			if(*(pos2+1) == '\0')
			{
				//printf("%s line=%d audio sample rate is empty\n", __FUNCTION__, __LINE__);
				media_session->audio_samplerate = 0;
			}
			else
			{
				memset(temp_buffer, 0, sizeof(temp_buffer));
				strncpy(temp_buffer, pos2+1, MAX_SIPD_NORMAL_LEN);
				//printf("%s line=%d audio sample rate=%s strlen(temp_buffer)=%d\n", __FUNCTION__, __LINE__, temp_buffer, strlen(temp_buffer));
				if(strlen(temp_buffer) > 0)
				{
					media_session->audio_samplerate = atoi(temp_buffer);
					//printf("%s line=%d audio audio_samplerate=%d\n", __FUNCTION__, __LINE__, media_session->audio_samplerate);
				}
			}
			
		}
		
	}
	else
	{
		//printf("%s line=%d f field in sdp is nothing to analyze, strlen is 0, f=%s \n", __FUNCTION__, __LINE__, media_session->pri_f_field);
		media_session->f_enable = 0;
	}

	return 0;
	
F_FORMAT_ERROR:

	return -1;
}

static int get_media_session(sdp_message_t *remote_sdp, struct sipd_invite_media_session* media_session)
{
	 sdp_connection_t* video_connection = NULL;
	 sdp_connection_t* audio_connection = NULL;
	 sdp_media_t* video_media = NULL;
	 sdp_media_t* audio_media = NULL;
	 char* session_name = NULL;
	 char* uri_sdp = NULL;
	 char* start_time = NULL;
	 char* end_time = NULL;
	 char* o_username=NULL;
	 
	 
	 
       if(NULL == remote_sdp || NULL == media_session)
       {
       	return -1;
       }

	video_connection = eXosip_get_video_connection(remote_sdp);
	if(NULL == video_connection)
	{
		printf("%s line=%d eXosip_get_video_connection failed, don't find video connection in remote sdp \n", __FUNCTION__, __LINE__);
	}
	else
	{
		strncpy(media_session->connect_video_ip, video_connection->c_addr, MAX_SIPD_URI);
		strncpy(media_session->connect_video_addrtype, video_connection->c_addrtype, MAX_SIPD_NORMAL_LEN);
	}

	audio_connection = eXosip_get_audio_connection(remote_sdp);
	if(NULL == audio_connection)
	{
		//printf("%s line=%d eXosip_get_audio_connection failed, don't find audio connection in remote sdp \n", __FUNCTION__, __LINE__);
	}
	else
	{
		strncpy(media_session->connect_audio_ip, audio_connection->c_addr, MAX_SIPD_URI);
		strncpy(media_session->connect_audio_addrtype, audio_connection->c_addrtype, MAX_SIPD_NORMAL_LEN);
	}

	
	video_media = eXosip_get_video_media(remote_sdp);
	if(NULL == video_media)
	{
		printf("%s line=%d eXosip_get_video_media failed, cann't find any video in remote sdp \n", __FUNCTION__, __LINE__);
	}
	else
	{
		strncpy(media_session->connect_video_port, video_media->m_port, MAX_SIPD_NORMAL_LEN);
	}

	audio_media = eXosip_get_audio_media(remote_sdp);
	if(NULL == audio_media)
	{
		//printf("%s line=%d eXosip_get_audio_media failed, cann't find any audio in remote sdp \n", __FUNCTION__, __LINE__);
	}
	else
	{
		strncpy(media_session->connect_audio_port, audio_media->m_port, MAX_SIPD_NORMAL_LEN);
	}

	if((video_media != NULL)  && (audio_media != NULL))
	{
		media_session->media_type = SIPD_MEDIA_TYPE_MIX;
	}
	else if (video_media != NULL)
	{
		media_session->media_type = SIPD_MEDIA_TYPE_VIDEO;
	}
	else if (audio_media != NULL)
	{
		media_session->media_type = SIPD_MEDIA_TYPE_AUDIO;
	}

	o_username = sdp_message_o_username_get(remote_sdp);
	if(o_username != NULL)
	{
		strncpy(media_session->o_field_username, o_username, MAX_SIPD_NORMAL_LEN);
	}

	session_name = sdp_message_s_name_get(remote_sdp);
	if(NULL == session_name)
	{
		printf("%s line=%d sdp_message_s_name_get failed, [NOTICE!!!] use default session name Play to instead \n", __FUNCTION__, __LINE__);
		media_session->stream_type = SIPD_STREAM_TYPE_PLAY;
	}
	else
	{
		//printf("%s line=%d sdp_message_s_name_get ok. s=%s \n", __FUNCTION__, __LINE__, session_name);
		if(strcmp(session_name, SIPD_VIDEO_PLAY) == 0)
		{
			media_session->stream_type = SIPD_STREAM_TYPE_PLAY;
		}
		else if(strcmp(session_name, SIPD_VIDEO_PLAYBACK) == 0)
		{
			media_session->stream_type = SIPD_STREAM_TYPE_PLAYBACK;
			uri_sdp = sdp_message_u_uri_get(remote_sdp);
			if(NULL == uri_sdp)
			{
				//printf("%s line=%d Playback, but uri is NULL (get sdp_message_u_uri_get failed)\n", __FUNCTION__, __LINE__);
			}
			else
			{
				//printf("%s line=%d Playback, u=%s \n", __FUNCTION__, __LINE__, uri_sdp);
				strncpy(media_session->u_field, uri_sdp, MAX_SIPD_URI);
			}

			start_time = sdp_message_t_start_time_get(remote_sdp, 0);
			if(NULL == start_time)
			{
				//printf("%s line=%d Playback, start_time is NULL  (get sdp_message_t_start_time_get failed)\n", __FUNCTION__, __LINE__);
			}
			else
			{
				//printf("%s line=%d Playback, start_time=%s \n", __FUNCTION__, __LINE__, start_time);
				strncpy(media_session->t_start_time, start_time, MAX_SIPD_NORMAL_LEN);
			}

			end_time = sdp_message_t_stop_time_get(remote_sdp, 0);
			if(NULL == end_time)
			{
				//printf("%s line=%d Playback, end_time is NULL  (get sdp_message_t_stop_time_get failed)\n", __FUNCTION__, __LINE__);
			}
			else
			{
				//printf("%s line=%d Playback, end_time=%s \n", __FUNCTION__, __LINE__, end_time);
				strncpy(media_session->t_end_time, end_time, MAX_SIPD_NORMAL_LEN);
			}
			
		}
		else if(strcmp(session_name, SIPD_VIDEO_DOWNLOAD) == 0)
		{
			media_session->stream_type = SIPD_STREAM_TYPE_DOWNLOAD;
			uri_sdp = sdp_message_u_uri_get(remote_sdp);
			if(NULL == uri_sdp)
			{
				//printf("%s line=%d Download, but uri is NULL (get sdp_message_u_uri_get failed)\n", __FUNCTION__, __LINE__);
			}
			else
			{
				//printf("%s line=%d Download, u=%s \n", __FUNCTION__, __LINE__, uri_sdp);
				strncpy(media_session->u_field, uri_sdp, MAX_SIPD_URI);
			}

			start_time = sdp_message_t_start_time_get(remote_sdp, 0);
			if(NULL == start_time)
			{
				//printf("%s line=%d Download, start_time is NULL  (get sdp_message_t_start_time_get failed)\n", __FUNCTION__, __LINE__);
			}
			else
			{
				//printf("%s line=%d Download, start_time=%s \n", __FUNCTION__, __LINE__, start_time);
				strncpy(media_session->t_start_time, start_time, MAX_SIPD_NORMAL_LEN);
			}

			end_time = sdp_message_t_stop_time_get(remote_sdp, 0);
			if(NULL == end_time)
			{
				//printf("%s line=%d Download, end_time is NULL  (get sdp_message_t_stop_time_get failed)\n", __FUNCTION__, __LINE__);
			}
			else
			{
				//printf("%s line=%d Download, end_time=%s \n", __FUNCTION__, __LINE__, end_time);
				strncpy(media_session->t_end_time, end_time, MAX_SIPD_NORMAL_LEN);
			}
		}
		else
		{
			media_session->stream_type = SIPD_STREAM_TYPE_PLAY;
		}
	}

	
	
	return 0;
}


int sip_sender_sdp_create(const sipd_sender_sdp_info* pSenderSdp, char* sdp_buffer)
{
	snprintf(sdp_buffer,MAX_SIPD_BODY_LENGTH,
		"v=0\r\n"\
		"o=%s 0 0 IN IP4 %s\r\n"\
		"s=Play\r\n"\
		"i=%s\r\n"\
		"c=IN IP4 %s\r\n"\
		"t=0 0\r\n"\
		/*"m=video %d RTP/AVP %d %d\r\n"\*/
		"m=video %s RTP/AVP %d\r\n"\
		"a=sendonly\r\n"\
		/*"a=rtpmap:%d PS/90000\r\n"\*/
		"a=rtpmap:%d PS/90000\r\n"\
		/*"a=username:%s\r\n"\*/
		/*"a=password:%s\r\n"\*/
		/*"y=%s\r\n"*/  ,
		pSenderSdp->media_sender_or_receiver_id, pSenderSdp->media_send_or_receive_ip, "GB28181 DEVICE", pSenderSdp->media_send_or_receive_ip, 
		pSenderSdp->media_send_or_receive_port, SIP_PS_H264, SIP_PS_H264);

     return 0;
}

int sip_receiver_sdp_create(const sipd_sender_sdp_info* pSenderSdp, char* sdp_buffer)
{
	snprintf(sdp_buffer,MAX_SIPD_BODY_LENGTH,
		"v=0\r\n"\
		"o=%s 0 0 IN IP4 %s\r\n"\
		"i=%s\r\n"\
		"c=IN IP4 %s\r\n"\
		"t=0 0\r\n"\
		/*"m=video %d RTP/AVP %d %d\r\n"\*/
		"m=video %s RTP/AVP %d\r\n"\
		"a=recvonly\r\n"\
		/*"a=rtpmap:%d PS/90000\r\n"\*/
		"a=rtpmap:%d PS/90000\r\n"\
		/*"a=username:%s\r\n"\*/
		/*"a=password:%s\r\n"\*/
		"y=%s\r\n",
		pSenderSdp->media_sender_or_receiver_id, pSenderSdp->media_send_or_receive_ip, "GB28181 DEVICE", pSenderSdp->media_send_or_receive_ip,
		pSenderSdp->media_send_or_receive_port, SIP_PS_H264, SIP_PS_H264, pSenderSdp->ssrc);

	return 0;
}

int build_and_send_alarm_message(char* from, char* to, unsigned int sn, char* device_id, int alarm_level, int alarm_method, char* description, char* AlarmTime, char* longitude, char* latitiude)
{
	osip_message_t *message = NULL;
	char buffer[MAX_XML_BUFFER_LEN+1]={0};
	int ret=0;
	int len=0;

	if(NULL == from ||NULL == to ||NULL == device_id ||NULL == AlarmTime )
	{
	 	printf("%s:%d input parameters illegal\n", __FUNCTION__, __LINE__);
		return -1;
	 }

	//printf("%s:%d from=%s to=%s\n", __FUNCTION__, __LINE__, from, to);



	memset(buffer, 0, sizeof(buffer));
	snprintf(buffer,MAX_XML_BUFFER_LEN,
	"<?xml version=\"1.0\"?>\r\n"\
	"<Notify>\r\n"\
	"<CmdType>Alarm</CmdType>\r\n"\
	"<SN>%d</SN>\r\n"\
	"<DeviceID>%s</DeviceID>\r\n"\
	"<AlarmPriority>%d</AlarmPriority>\r\n"\
	"<AlarmTime>%s</AlarmTime>\r\n"\
	"<AlarmMethod>%d</AlarmMethod>\r\n",
	sn, device_id, alarm_level, AlarmTime, alarm_method);

	if(description != NULL)
	{
		len = strlen(buffer);
		snprintf(buffer+len, (MAX_XML_BUFFER_LEN-len),"<AlarmDescription>%s</AlarmDescription>\r\n", description);
	}

	if(longitude != NULL)
	{
		if(strlen(longitude) > 0)
		{
			len = strlen(buffer);
			snprintf(buffer+len, (MAX_XML_BUFFER_LEN-len),"<Longitude>%s</Longitude>\r\n", longitude);
		}
	}

	if (latitiude != NULL)
	{
		if(strlen(latitiude) > 0)
		{
			len = strlen(buffer);
			snprintf(buffer+len, (MAX_XML_BUFFER_LEN-len),"<Latitude>%s</Latitude>\r\n", latitiude);
		}
	}

	len = strlen(buffer);
	snprintf(buffer+len, (MAX_XML_BUFFER_LEN-len),"</Notify>");

	context_eXosip_lock();
	eXosip_lock(context_eXosip);
	//printf("%s:%d to=%s from=%s \n", __FUNCTION__, __LINE__, to, from);
	ret = eXosip_message_build_request(context_eXosip, &message, "MESSAGE", to, from, NULL);
	 if(ret != OSIP_SUCCESS)
	 {
	 	eXosip_unlock(context_eXosip);
	 	printf("%s:%d eXosip_message_build_request failed\n", __FUNCTION__, __LINE__);
		if(message != NULL)
		{
			osip_message_free(message);
		}
		context_eXosip_unlock();
		return -1;
	 }

	 ret = osip_message_set_body(message, buffer, strlen(buffer));
	  if(ret != OSIP_SUCCESS)
	 {
	 	eXosip_unlock(context_eXosip);
	 	printf("%s:%d osip_message_set_body failed\n", __FUNCTION__, __LINE__);
		if(message != NULL)
		{
			osip_message_free(message);
		}
		context_eXosip_unlock();
		return -1;
	 }
	  
	 ret = osip_message_set_content_type (message, "Application/MANSCDP+xml");
	  if(ret != OSIP_SUCCESS)
	 {
	 	eXosip_unlock(context_eXosip);
	 	printf("%s:%d osip_message_set_content_type failed\n", __FUNCTION__, __LINE__);
		if(message != NULL)
		{
			osip_message_free(message);
		}
		context_eXosip_unlock();
		return -1;
	 }
	
	#if 0
	  {
	  	char *dest=NULL;
		size_t length=0;
		ret = osip_message_to_str(message, &dest, &length);
		if (ret !=0) 
		{
			printf("%s:%d osip_message_to_str failed\n", __FUNCTION__, __LINE__);
		}
		printf("%s line=%d message:\n%s\n", __FUNCTION__, __LINE__, dest);
		osip_free(dest);
	 }
	  #endif
	 
	eXosip_message_send_request (context_eXosip, message);
	eXosip_unlock(context_eXosip);
	context_eXosip_unlock();

	 return 0;
	
}


int strip_to(char* to, char* newto)
{
	osip_to_t * to_t=NULL;
	int ret=0;
	char* dest=NULL;
	
	if(NULL == to || NULL == newto)
	{
		return -1;
	}

	//printf("%s:%d to=%s \n", __FUNCTION__, __LINE__, to);

	ret = osip_to_init (&to_t);
	if(ret != OSIP_SUCCESS)
	{
	 	printf("%s:%d osip_to_init  (to_t) failed\n", __FUNCTION__, __LINE__);
		return -1;
	 }

	ret = osip_to_parse(to_t, to);
	if(ret != OSIP_SUCCESS)
	{
	 	printf("%s:%d osip_to_parse (to_t) failed\n", __FUNCTION__, __LINE__);
		return -1;
	 }

	 osip_generic_param_freelist (&to_t->gen_params);

	

	 ret = osip_to_to_str(to_t, &dest);
	if(ret != OSIP_SUCCESS)
	{
	 	printf("%s:%d osip_to_to_str (newto_t) failed\n", __FUNCTION__, __LINE__);
		return -1;
	 }

	if(strlen(dest) > MAX_SIPD_URI)
	{
		printf("%s:%d be careful, strlen(dest)=%d is too long. dest=%s \n", __FUNCTION__, __LINE__, strlen(dest), dest);
		strncpy(newto, dest, MAX_SIPD_URI);
	}
	else
	{
		strcpy(newto, dest);
	}

	//printf("%s:%d newto=%s \n", __FUNCTION__, __LINE__, newto);

	osip_free(dest);
	osip_to_free(to_t);
	

	return 0;
}


int strip_from(char* from, char* newfrom)
{
	osip_from_t * from_t=NULL;
	int ret=0;
	char* dest=NULL;
	
	if(NULL == from || NULL == newfrom)
	{
		return -1;
	}

	//printf("%s:%d from=%s \n", __FUNCTION__, __LINE__, from);

	ret = osip_from_init (&from_t);
	if(ret != OSIP_SUCCESS)
	{
	 	printf("%s:%d osip_from_init  (from_t) failed\n", __FUNCTION__, __LINE__);
		return -1;
	 }

	ret = osip_from_parse(from_t, from);
	if(ret != OSIP_SUCCESS)
	{
	 	printf("%s:%d osip_from_parse (from_t) failed\n", __FUNCTION__, __LINE__);
		return -1;
	 }
	 osip_generic_param_freelist (&from_t->gen_params);

	

	 ret = osip_from_to_str(from_t, &dest);
	if(ret != OSIP_SUCCESS)
	{
	 	printf("%s:%d osip_from_to_str  failed\n", __FUNCTION__, __LINE__);
		return -1;
	 }

	if(strlen(dest) > MAX_SIPD_URI)
	{
		printf("%s:%d be careful, strlen(dest)=%d is too long. dest=%s \n", __FUNCTION__, __LINE__, strlen(dest), dest);
		strncpy(newfrom, dest, MAX_SIPD_URI);
	}
	else
	{
		strcpy(newfrom, dest);
	}

	//printf("%s:%d newfrom=%s \n", __FUNCTION__, __LINE__, newfrom);

	osip_free(dest);
	osip_from_free(from_t);

	return 0;
}

int build_and_send_device_control_response_message(char* from, char* to, unsigned int sn, char* device_id, int result)
{
	 osip_message_t *message = NULL;
	 char buffer[MAX_XML_BUFFER_LEN+1]={0};
	 int ret=0;

	 if(NULL == from || NULL == to || NULL == device_id)
	 {
	 	printf("%s:%d input parameters illegal\n", __FUNCTION__, __LINE__);
		return -1;
	 }
	 
	//printf("%s:%d from=%s to=%s\n", __FUNCTION__, __LINE__, from, to);

	memset(buffer, 0, sizeof(buffer));
	snprintf(buffer,MAX_XML_BUFFER_LEN,
	"<?xml version=\"1.0\"?>\r\n"\
	"<Response>\r\n"\
	"<CmdType>DeviceControl</CmdType>\r\n"\
	"<SN>%d</SN>\r\n"\
	"<DeviceID>%s</DeviceID>\r\n"\
	"<Result>%s</Result>\r\n"\
	"</Response>",
	sn, device_id, (result ==0)?"OK":"ERROR");

	context_eXosip_lock();
	eXosip_lock(context_eXosip);
	 ret = eXosip_message_build_request(context_eXosip, &message, "MESSAGE", to, from, NULL);
	 if(ret != OSIP_SUCCESS)
	 {
	 	eXosip_unlock(context_eXosip);
	 	printf("%s:%d eXosip_message_build_request failed\n", __FUNCTION__, __LINE__);
		if(message != NULL)
		{
			osip_message_free(message);
		}
		context_eXosip_unlock();
		return -1;
	 }

	 ret = osip_message_set_body(message, buffer, strlen(buffer));
	  if(ret != OSIP_SUCCESS)
	 {
	 	eXosip_unlock(context_eXosip);
	 	printf("%s:%d osip_message_set_body failed\n", __FUNCTION__, __LINE__);
		if(message != NULL)
		{
			osip_message_free(message);
		}
		context_eXosip_unlock();
		return -1;
	 }
	  
	 ret = osip_message_set_content_type (message, "Application/MANSCDP+xml");
	  if(ret != OSIP_SUCCESS)
	 {
	 	eXosip_unlock(context_eXosip);
	 	printf("%s:%d osip_message_set_content_type failed\n", __FUNCTION__, __LINE__);
		if(message != NULL)
		{
			osip_message_free(message);
		}
		context_eXosip_unlock();
		return -1;
	 }

	  

	  #if 0
	  {
	  	char *dest=NULL;
		size_t length=0;
		ret = osip_message_to_str(message, &dest, &length);
		if (ret !=0) 
		{
			printf("%s:%d osip_message_to_str failed\n", __FUNCTION__, __LINE__);
		}
		printf("%s line=%d message:\n%s\n", __FUNCTION__, __LINE__, dest);
		osip_free(dest);
	 }
	  #endif
	 
	eXosip_message_send_request (context_eXosip, message);
	eXosip_unlock(context_eXosip);
	context_eXosip_unlock();
	 return 0;
}


int get_PRM_SIPD_CFG(SIPD_GATEWAY_CFG* pCfg)
{

	int ret = 0;
	
	if (NULL == pCfg)
	{
		printf("get_PRM_SIPD_CFG Invalid input parar pCfg is NULL. ERROR! \n");
		return -1;
	}

	ret = get_domain_server_cfg(pCfg);
	if(-1 == ret )
	{
	        printf("get_PRM_SIPD_CFG Failed!\n");
		
		return -1;
	}

	return 0;

}

static int get_PTZCMD_code(char* ptzcmd, struct sipd_PTZCMD* ptzcode)
{
	char pbyte[2+1];
	char* p=NULL;
	int check_sum=0;
	char* str;
	
	if(NULL == ptzcmd || NULL == ptzcode)
	{
		return -1;
	}

	//printf("%s line=%d  ptzcmd=%s\n", __FUNCTION__, __LINE__, ptzcmd);

	memset(pbyte, 0, sizeof(pbyte));
	p=ptzcmd;

	if(p != NULL)
	{
		strncpy(pbyte, p, 2);
		p+=2;
	}
	ptzcode->firstByteA5 = (unsigned char)strtol(pbyte, &str, 16);
	//printf("%s line=%d  pbyte 1=%s ptzcode->firstByteA5=%d ptzcode->firstByteA5 hex=%#x \n", __FUNCTION__, __LINE__, pbyte, ptzcode->firstByteA5, ptzcode->firstByteA5);

	memset(pbyte, 0, sizeof(pbyte));
	if(p != NULL)
	{
		strncpy(pbyte, p, 2);
		p+=2;
	}
	ptzcode->combineCode1 = (unsigned char)strtol(pbyte, &str, 16);
	//printf("%s line=%d  pbyte 2=%s ptzcode->combineCode1=%d ptzcode->combineCode1 hex=%#x\n", __FUNCTION__, __LINE__, pbyte, ptzcode->combineCode1, ptzcode->combineCode1);

	memset(pbyte, 0, sizeof(pbyte));
	if(p != NULL)
	{
		strncpy(pbyte, p, 2);
		p+=2;
	}
	ptzcode->addr = (unsigned char)strtol(pbyte, &str, 16);
	//printf("%s line=%d  pbyte 3=%s ptzcode->addr=%d ptzcode->addr hex=%#x\n", __FUNCTION__, __LINE__, pbyte, ptzcode->addr, ptzcode->addr);
	
	memset(pbyte, 0, sizeof(pbyte));
	if(p != NULL)
	{
		strncpy(pbyte, p, 2);
		p+=2;
	}
	ptzcode->instruction = (unsigned char)strtol(pbyte, &str, 16);
	//printf("%s line=%d  pbyte 4=%s ptzcode->instruction=%d ptzcode->instruction hex=%#x\n", __FUNCTION__, __LINE__, pbyte, ptzcode->instruction, ptzcode->instruction);

	memset(pbyte, 0, sizeof(pbyte));
	if(p != NULL)
	{
		strncpy(pbyte, p, 2);
		p+=2;
	}
	ptzcode->data1 = (unsigned char)strtol(pbyte, &str, 16);
	//printf("%s line=%d  pbyte 5=%s ptzcode->data1=%d ptzcode->data1 hex=%#x\n", __FUNCTION__, __LINE__, pbyte, ptzcode->data1, ptzcode->data1);


	memset(pbyte, 0, sizeof(pbyte));
	if(p != NULL)
	{
		strncpy(pbyte, p, 2);
		p+=2;
	}
	ptzcode->data2 = (unsigned char)strtol(pbyte, &str, 16);
	//printf("%s line=%d  pbyte 6=%s ptzcode->data2=%d ptzcode->data2 hex=%#x\n", __FUNCTION__, __LINE__, pbyte, ptzcode->data2, ptzcode->data2);

	memset(pbyte, 0, sizeof(pbyte));
	if(p != NULL)
	{
		strncpy(pbyte, p, 2);
		p+=2;
	}
	ptzcode->combineCode2= (unsigned char)strtol(pbyte, &str, 16);
	ptzcode->combineCode2_data3 = ((ptzcode->combineCode2 & 0xF0)>>4);
	ptzcode->combineCode2_addr2 = (ptzcode->combineCode2 & 0xF);
	//printf("%s line=%d  pbyte 7=%s ptzcode->combineCode2=%d ptzcode->combineCode2 hex=%#x\n", __FUNCTION__, __LINE__, pbyte, ptzcode->combineCode2, ptzcode->combineCode2);
	//printf("%s line=%d  pbyte 7=%s ptzcode->combineCode2_data3=%d ptzcode->combineCode2_data3 hex=%#x\n", __FUNCTION__, __LINE__, pbyte, ptzcode->combineCode2_data3, ptzcode->combineCode2_data3);
	//printf("%s line=%d  pbyte 7=%s ptzcode->combineCode2_addr2=%d ptzcode->combineCode2_addr2 hex=%#x\n", __FUNCTION__, __LINE__, pbyte, ptzcode->combineCode2_addr2, ptzcode->combineCode2_addr2);

	memset(pbyte, 0, sizeof(pbyte));
	if(p != NULL)
	{
		strncpy(pbyte, p, 2);
		p+=2;
	}
	ptzcode->check_code = (unsigned char)strtol(pbyte, &str, 16);
	//printf("%s line=%d  pbyte 8=%s ptzcode->check_code=%d ptzcode->check_code hex=%#x\n", __FUNCTION__, __LINE__, pbyte, ptzcode->check_code, ptzcode->check_code);
	
	/*check check-sum*/
	check_sum =( ((ptzcode->firstByteA5 & 0xF0)>>4) + (ptzcode->firstByteA5 & 0xF) + ((ptzcode->combineCode1 & 0xF0)>>4))%16;
	//printf("%s line=%d  first compute_check_sum=%d combinecode1_check_sum=%d\n", __FUNCTION__, __LINE__, check_sum, (ptzcode->combineCode1 & 0xF));
	if(check_sum != (ptzcode->combineCode1 & 0xF))
	{
		ptzcode->result = -1;
		printf("%s line=%d  combine code 1 check sum  Error\n", __FUNCTION__, __LINE__);
		return -1;
	}

	check_sum = (ptzcode->firstByteA5 + ptzcode->combineCode1 + ptzcode->addr + ptzcode->instruction + ptzcode->data1 + ptzcode->data2 + ptzcode->combineCode2)%256;
	//printf("%s line=%d  total compute_check_sum=%d 8bytechecksum=%d\n", __FUNCTION__, __LINE__, check_sum, ptzcode->check_code);
	if(check_sum != ptzcode->check_code)
	{
		ptzcode->result = -1;
		printf("%s line=%d  total check code  Error\n", __FUNCTION__, __LINE__);
		return -1;
	}
	
	return 0;
	
}

static int handle_PTZCmd(char* ptzcmd, char* device_id)
{
	struct sipd_PTZCMD ptzcode;
	int ret;
	
	if(NULL == ptzcmd || NULL == device_id)
	{
		return -1;
	}

	memset(&ptzcode, 0, sizeof(ptzcode));
	ret = get_PTZCMD_code(ptzcmd, &ptzcode);
	if(-1 == ret )
	{
		printf("%s line=%d  get_PTZCMD_code Error\n", __FUNCTION__, __LINE__);
		return -1;
	}

	if(-1 == ptzcode.result)
	{
		printf("%s line=%d  get_PTZCMD_code check-sum Error\n", __FUNCTION__, __LINE__);
		return -1;
	}

	//printf("%s line=%d  instruction code =%#x \n", __FUNCTION__, __LINE__, ptzcode.instruction);

	//	预制点指令
	if(SIPD_PTZ_INSTRUCTION_CODE_SET_PRESET == ptzcode.instruction)
	{
		/*set preset*/
		struct sipd_preset_req preset_req;
		sipd_preset_control ptz_req;

		memset(&preset_req, 0, sizeof(preset_req));
		strncpy(preset_req.device_id, device_id, MAX_SIPD_DEVICE_ID_LEN);
		preset_req.preset_id = ptzcode.data2;

		memset(&ptz_req, 0, sizeof(ptz_req));
		strncpy(ptz_req.device_id, preset_req.device_id, MAX_SIPD_DEVICE_ID_LEN);
		ptz_req.cmd_type = SIPD_PTZ_PRESET_ADD;
		ptz_req.preset_id = preset_req.preset_id;

		if(pSipd_PTZ_Preset)
			pSipd_PTZ_Preset(&ptz_req);
		
	}
	else if(SIPD_PTZ_INSTRUCTION_CODE_CALL_PRESET == ptzcode.instruction)
	{
		/*call preset*/
		struct sipd_preset_req preset_req;
		sipd_preset_control ptz_req;

		memset(&preset_req, 0, sizeof(preset_req));
		strncpy(preset_req.device_id, device_id, MAX_SIPD_DEVICE_ID_LEN);
		preset_req.preset_id = ptzcode.data2;
		
		memset(&ptz_req, 0, sizeof(ptz_req));
		strncpy(ptz_req.device_id, preset_req.device_id, MAX_SIPD_DEVICE_ID_LEN);
		ptz_req.cmd_type = SIPD_PTZ_PRESET_GOTO;
		ptz_req.preset_id = preset_req.preset_id;

		if(pSipd_PTZ_Preset)
			pSipd_PTZ_Preset(&ptz_req);
	}
	else if(SIPD_PTZ_INSTRUCTION_CODE_DELETE_PRESET == ptzcode.instruction)
	{
		/*delete preset*/
		struct sipd_preset_req preset_req;
		sipd_preset_control ptz_req;

		memset(&preset_req, 0, sizeof(preset_req));
		strncpy(preset_req.device_id, device_id, MAX_SIPD_DEVICE_ID_LEN);
		preset_req.preset_id = ptzcode.data2;

		memset(&ptz_req, 0, sizeof(ptz_req));
		strncpy(ptz_req.device_id, preset_req.device_id, MAX_SIPD_DEVICE_ID_LEN);
		ptz_req.cmd_type = SIPD_PTZ_PRESET_DEL;
		ptz_req.preset_id = preset_req.preset_id;

		if(pSipd_PTZ_Preset)
			pSipd_PTZ_Preset(&ptz_req);
	}
	//	巡航指令
	else if(SIPD_PTZ_INSTRUCTION_CODE_ADD_CRUISE == ptzcode.instruction)
	{
		/*add cruise*/
		struct sipd_cruiseroute_req cruise_req;
		sipd_tour_control ptz_req;
		
		memset(&cruise_req, 0, sizeof(cruise_req));
		strncpy(cruise_req.device_id, device_id, MAX_SIPD_DEVICE_ID_LEN);

		cruise_req.cruise_id = ptzcode.data1;
		cruise_req.preset_id = ptzcode.data2;
		//printf("%s line=%d add cruise cruise_id=%d preset_id =%d\n", __FUNCTION__, __LINE__, cruise_req.cruise_id, cruise_req.preset_id);

		memset(&ptz_req, 0, sizeof(ptz_req));
		strncpy(ptz_req.device_id, cruise_req.device_id, MAX_SIPD_DEVICE_ID_LEN);
		ptz_req.tour_cmd = SIPD_PTZ_TOUR_ADD;
		ptz_req.tour_id = cruise_req.cruise_id;
		ptz_req.preset_id = cruise_req.preset_id;
		ptz_req.time = cruise_req.time;
		ptz_req.speed = cruise_req.speed;

		if(pSipd_PTZ_Tour)
			pSipd_PTZ_Tour(&ptz_req);
	}
	else if(SIPD_PTZ_INSTRUCTION_CODE_DELETE_CRUISE == ptzcode.instruction)
	{
		/*delete cruise*/
		struct sipd_cruiseroute_req cruise_req;
		sipd_tour_control ptz_req;
		
		memset(&cruise_req, 0, sizeof(cruise_req));
		strncpy(cruise_req.device_id, device_id, MAX_SIPD_DEVICE_ID_LEN);

		cruise_req.cruise_id = ptzcode.data1;
		cruise_req.preset_id = ptzcode.data2;

		memset(&ptz_req, 0, sizeof(ptz_req));
		strncpy(ptz_req.device_id, cruise_req.device_id, MAX_SIPD_DEVICE_ID_LEN);
		ptz_req.tour_cmd = SIPD_PTZ_TOUR_DEL;
		ptz_req.tour_id = cruise_req.cruise_id;
		ptz_req.preset_id = cruise_req.preset_id;
		ptz_req.time = cruise_req.time;
		ptz_req.speed = cruise_req.speed;

		if(pSipd_PTZ_Tour)
			pSipd_PTZ_Tour(&ptz_req);				
	}
	else if(SIPD_PTZ_INSTRUCTION_CODE_SET_SPEED_CRUISE == ptzcode.instruction)
	{
		/*set cruise speed*/
		struct sipd_cruiseroute_req cruise_req;
		sipd_tour_control ptz_req;
		
		memset(&cruise_req, 0, sizeof(cruise_req));
		strncpy(cruise_req.device_id, device_id, MAX_SIPD_DEVICE_ID_LEN);
		
		cruise_req.cruise_id = ptzcode.data1;
		cruise_req.speed= ptzcode.data2 + ((ptzcode.combineCode2_data3) << 8);
		//printf("%s line=%d set cruise speed cruise_id=%d speed =%d ptzcode.data2=%d data3=%d\n", __FUNCTION__, __LINE__, cruise_req.cruise_id, cruise_req.speed, ptzcode.data2, ptzcode.combineCode2_data3);

		memset(&ptz_req, 0, sizeof(ptz_req));
		strncpy(ptz_req.device_id, cruise_req.device_id, MAX_SIPD_DEVICE_ID_LEN);
		ptz_req.tour_cmd = SIPD_PTZ_TOUR_SET_SPEED;
		ptz_req.tour_id = cruise_req.cruise_id;
		ptz_req.preset_id = cruise_req.preset_id;
		ptz_req.time = cruise_req.time;
		ptz_req.speed = cruise_req.speed;

		if(pSipd_PTZ_Tour)
			pSipd_PTZ_Tour(&ptz_req);		
	}
	else if(SIPD_PTZ_INSTRUCTION_CODE_SET_TIME_CRUISE == ptzcode.instruction)
	{
		/*set cruise time*/
		struct sipd_cruiseroute_req cruise_req;
		sipd_tour_control ptz_req;

		memset(&cruise_req, 0, sizeof(cruise_req));
		strncpy(cruise_req.device_id, device_id, MAX_SIPD_DEVICE_ID_LEN);
		
		cruise_req.cruise_id = ptzcode.data1;
		cruise_req.time = ptzcode.data2 + ((ptzcode.combineCode2_data3) << 8);
		//printf("%s line=%d set cruise time cruise_id=%d time =%d ptzcode.data2=%d data3=%d\n", __FUNCTION__, __LINE__, cruise_req.cruise_id, cruise_req.time, ptzcode.data2, ptzcode.combineCode2_data3);

		memset(&ptz_req, 0, sizeof(ptz_req));
		strncpy(ptz_req.device_id, cruise_req.device_id, MAX_SIPD_DEVICE_ID_LEN);
		ptz_req.tour_cmd = SIPD_PTZ_TOUR_SET_STAYTIME;
		ptz_req.tour_id = cruise_req.cruise_id;
		ptz_req.preset_id = cruise_req.preset_id;
		ptz_req.time = cruise_req.time;
		ptz_req.speed = cruise_req.speed;

		if(pSipd_PTZ_Tour)
			pSipd_PTZ_Tour(&ptz_req);	
	}
	else if(SIPD_PTZ_INSTRUCTION_CODE_START_CRUISE == ptzcode.instruction)
	{
		/*start cruise*/
		struct sipd_cruiseroute_req cruise_req;
		sipd_tour_control ptz_req;
		
		memset(&cruise_req, 0, sizeof(cruise_req));
		strncpy(cruise_req.device_id, device_id, MAX_SIPD_DEVICE_ID_LEN);
		
		cruise_req.cruise_id = ptzcode.data1;
		cruise_req.preset_id = ptzcode.data2;
		//printf("%s line=%d start cruise cruise_id=%d preset_id =%d\n", __FUNCTION__, __LINE__, cruise_req.cruise_id, cruise_req.preset_id);

		memset(&ptz_req, 0, sizeof(ptz_req));

		strncpy(ptz_req.device_id, cruise_req.device_id, MAX_SIPD_DEVICE_ID_LEN);
		ptz_req.tour_cmd = SIPD_PTZ_TOUR_START;
		ptz_req.tour_id = cruise_req.cruise_id;
		ptz_req.preset_id = cruise_req.preset_id;
		ptz_req.time = cruise_req.time;
		ptz_req.speed = cruise_req.speed;

		if(pSipd_PTZ_Tour)
			pSipd_PTZ_Tour(&ptz_req);		
	}
	//	扫描指令
	else if(SIPD_PTZ_INSTRUCTION_CODE_AUTOSCAN == ptzcode.instruction)
	{
		struct sipd_ptz_autoscan_req req;
			

		memset(&req, 0, sizeof(req));
		strncpy(req.device_id, device_id, MAX_SIPD_DEVICE_ID_LEN);
		
		//	开始自动扫描
		if(0x00 == ptzcode.data2)
		{
			sipd_autoscan_control ptz_req;
			/*auto scan*/
			req.scan_id = ptzcode.data1;
			//printf("%s line=%d start auto scan scan_id=%d \n", __FUNCTION__, __LINE__, req.scan_id);
			
			memset(&ptz_req, 0, sizeof(ptz_req));
			strncpy(ptz_req.device_id, req.device_id, MAX_SIPD_DEVICE_ID_LEN);
			ptz_req.cmd_type = SIPD_PTZ_AUTOSCAN_START;

			if(pSipd_PTZ_AutoScan)
				pSipd_PTZ_AutoScan(&ptz_req);
		}
		//	设置自动巡航左边界
		else if(0x01 == ptzcode.data2)
		{
			/*auto scan, set left    MSG_ID_GW_AUTOSCAN_SET_LEFT_REQ */
			req.scan_id = ptzcode.data1;
			//printf("%s line=%d set left auto scan scan_id=%d \n", __FUNCTION__, __LINE__, req.scan_id);
			
			
		}
		//	设置自动巡航右边界
		else if(0x02 == ptzcode.data2)
		{
			/*auto scan, set right   MSG_ID_GW_AUTOSCAN_SET_RIGHT_REQ */
			req.scan_id = ptzcode.data1;
			//printf("%s line=%d set right auto scan scan_id=%d \n", __FUNCTION__, __LINE__, req.scan_id);

			
		}
	}
	else if(SIPD_PTZ_INSTRUCTION_CODE_SET_AUTOSCAN_SPEED == ptzcode.instruction)
	{
		/*  MSG_ID_GW_AUTOSCAN_SET_SPEED_REQ */
		struct sipd_ptz_autoscan_req req;
		memset(&req, 0, sizeof(req));
		strncpy(req.device_id, device_id, MAX_SIPD_DEVICE_ID_LEN);

		req.scan_id = ptzcode.data1;
		req.speed = ptzcode.data2 + ((ptzcode.combineCode2_data3) << 8);
		//printf("%s line=%d set auto scan speed scan_id=%d time =%d ptzcode.data2=%d data3=%d\n", __FUNCTION__, __LINE__, req.scan_id, req.speed, ptzcode.data2, ptzcode.combineCode2_data3);

	}
	//	ptz 的所有操作均停止
	else if(SIPD_PTZ_INSTRUCTION_CODE_ZERO == ptzcode.instruction)
	{
		struct sipd_ptz_req req;
		struct sipd_cruiseroute_req cruise_req;

		sipd_ptz_control pPtz;
		sipd_tour_control ptz_req;
		
		memset(&req, 0, sizeof(req));
		strncpy(req.device_id, device_id, MAX_SIPD_DEVICE_ID_LEN);

		memset(&cruise_req, 0, sizeof(cruise_req));
		strncpy(cruise_req.device_id, device_id, MAX_SIPD_DEVICE_ID_LEN);
		req.speed = 0;

		memset(&pPtz, 0, sizeof(pPtz));
		strncpy(pPtz.device_id, req.device_id, MAX_SIPD_DEVICE_ID_LEN);
		pPtz.cmd_type = SIPD_PTZ_MOVE_STOP;
		pPtz.speed = req.speed;

		if(pSipd_PTZ_Move)
			pSipd_PTZ_Move(&pPtz);	

		#if 1
		cruise_req.cruise_id = ptzcode.data1;
		cruise_req.preset_id = ptzcode.data2;
		//printf("%s line=%d stop all (all zero) cruise_id=%d preset_id=%d \n", __FUNCTION__, __LINE__, cruise_req.cruise_id, cruise_req.preset_id);

		memset(&ptz_req, 0, sizeof(ptz_req));
		strncpy(ptz_req.device_id, cruise_req.device_id, MAX_SIPD_DEVICE_ID_LEN);
		ptz_req.tour_cmd = SIPD_PTZ_TOUR_STOP;
		ptz_req.tour_id = cruise_req.cruise_id;
		ptz_req.preset_id = cruise_req.preset_id;
		ptz_req.time = cruise_req.time;
		ptz_req.speed = cruise_req.speed;

		if(pSipd_PTZ_Tour)
			pSipd_PTZ_Tour(&ptz_req);	
		
		#endif
	}
	else
	{
		if((((ptzcode.instruction & 0x80)>>7) == 0) && (((ptzcode.instruction & 0x40)>>6) == 0))
		{
			/*bit7,6 = 0， PTZ 指令 */
			struct sipd_ptz_req req;
			
			//printf("%s line=%d bit7,6 all 0 \n", __FUNCTION__, __LINE__);
			
			memset(&req, 0, sizeof(req));
			strncpy(req.device_id, device_id, MAX_SIPD_DEVICE_ID_LEN);

			if(((ptzcode.instruction & 0x20)>>5) == 1)
			{
				sipd_ptz_control ptz_zoomwide;
				//	镜头变倍缩小（OUT）
				req.speed = 0;
				
				/*zoom out*/
				req.speed = ptzcode.combineCode2_data3;
				//printf("%s line=%d zoom out speed=%d \n", __FUNCTION__, __LINE__, req.speed);

				memset(&ptz_zoomwide, 0, sizeof(ptz_zoomwide));
				strncpy(ptz_zoomwide.device_id, req.device_id, MAX_SIPD_DEVICE_ID_LEN);
				ptz_zoomwide.cmd_type = SIPD_PTZ_ZOOMWIDE;
				ptz_zoomwide.speed = req.speed;

				if(pSipd_PTZ_Move)
					pSipd_PTZ_Move(&ptz_zoomwide);				
			}
			else if(((ptzcode.instruction & 0x10)>>4) == 1)
			{
				sipd_ptz_control ptz_zoomtele;

				//	镜头变倍放大（IN）
				req.speed = 0;

				/*zoom in*/
				req.speed = ptzcode.combineCode2_data3;
				//printf("%s line=%d zoom in speed=%d \n", __FUNCTION__, __LINE__, req.speed);

				memset(&ptz_zoomtele, 0, sizeof(ptz_zoomtele));
				strncpy(ptz_zoomtele.device_id, req.device_id, MAX_SIPD_DEVICE_ID_LEN);
				ptz_zoomtele.cmd_type = SIPD_PTZ_ZOOMTELE;
				ptz_zoomtele.speed = req.speed;

				if(pSipd_PTZ_Move)
					pSipd_PTZ_Move(&ptz_zoomtele);
			}
			else
			{
				sipd_ptz_control pPtz;

				req.speed = 0;
				//printf("%s line=%d zoom stop\n", __FUNCTION__, __LINE__);
				memset(&pPtz, 0, sizeof(pPtz));

				strncpy(pPtz.device_id, req.device_id, MAX_SIPD_DEVICE_ID_LEN);
				pPtz.cmd_type = SIPD_PTZ_MOVE_STOP;
				pPtz.speed = req.speed;

				if(pSipd_PTZ_Move)
					pSipd_PTZ_Move(&pPtz);	
			}

			if(((ptzcode.instruction & 0x8)>>3) == 1)
			{
				//	向上
				req.speed = 0;
				
				/*up*/
				if(((ptzcode.instruction & 0x2)>>1) == 1)
				{
					sipd_ptz_control ptz_lu;
					/*left， 左上 */
					req.speed = SIPD_MAX2(ptzcode.data1, ptzcode.data2);
					//printf("%s line=%d UP_LEFT in speed=%d \n", __FUNCTION__, __LINE__, req.speed);

					memset(&ptz_lu, 0, sizeof(ptz_lu));
					strncpy(ptz_lu.device_id, req.device_id, MAX_SIPD_DEVICE_ID_LEN);
					ptz_lu.cmd_type = SIPD_PTZ_MOVE_LU;
					ptz_lu.speed = req.speed;

					if(pSipd_PTZ_Move)
						pSipd_PTZ_Move(&ptz_lu);
				}
				else if((ptzcode.instruction & 0x1) == 1)
				{
					sipd_ptz_control ptz_ru;
					/*right， 右上 */
					req.speed = SIPD_MAX2(ptzcode.data1, ptzcode.data2);
					//printf("%s line=%d UP_RIGHT in speed=%d \n", __FUNCTION__, __LINE__, req.speed);

					memset(&ptz_ru, 0, sizeof(ptz_ru));

					strncpy(ptz_ru.device_id, req.device_id, MAX_SIPD_DEVICE_ID_LEN);
					ptz_ru.cmd_type = SIPD_PTZ_MOVE_RU;
					ptz_ru.speed = req.speed;

					if(pSipd_PTZ_Move)
						pSipd_PTZ_Move(&ptz_ru);
				}
				else
				{
					sipd_ptz_control ptz_up;
					//	垂直向上
					req.speed = ptzcode.data2;
					//printf("%s line=%d UP in speed=%d \n", __FUNCTION__, __LINE__, req.speed);

					memset(&ptz_up, 0, sizeof(ptz_up));
					strncpy(ptz_up.device_id, req.device_id, MAX_SIPD_DEVICE_ID_LEN);
					ptz_up.cmd_type = SIPD_PTZ_MOVE_UP;
					ptz_up.speed = req.speed;

					if(pSipd_PTZ_Move)
						pSipd_PTZ_Move(&ptz_up);
				}
			}
			else if(((ptzcode.instruction & 0x4)>>2) == 1)
			{
				//	向下
				req.speed = 0;
				
				/*down*/
				if(((ptzcode.instruction & 0x2)>>1) == 1)
				{
					sipd_ptz_control ptz_ld;
					/*left，  左下 */
					req.speed = SIPD_MAX2(ptzcode.data1, ptzcode.data2);
					//printf("%s line=%d DOWN_LEFT in speed=%d \n", __FUNCTION__, __LINE__, req.speed);

					memset(&ptz_ld, 0, sizeof(ptz_ld));
					strncpy(ptz_ld.device_id, req.device_id, MAX_SIPD_DEVICE_ID_LEN);
					ptz_ld.cmd_type = SIPD_PTZ_MOVE_LD;
					ptz_ld.speed = req.speed;

					if(pSipd_PTZ_Move)
						pSipd_PTZ_Move(&ptz_ld);
				}
				else if((ptzcode.instruction & 0x1) == 1)
				{
					sipd_ptz_control ptz_rd;
					/*right， 右下 */
					req.speed = SIPD_MAX2(ptzcode.data1, ptzcode.data2);
					//printf("%s line=%d DOWN_RIGHT in speed=%d \n", __FUNCTION__, __LINE__, req.speed);

					memset(&ptz_rd, 0, sizeof(ptz_rd));
					strncpy(ptz_rd.device_id, req.device_id, MAX_SIPD_DEVICE_ID_LEN);
					ptz_rd.cmd_type = SIPD_PTZ_MOVE_RD;
					ptz_rd.speed = req.speed;

					if(pSipd_PTZ_Move)
						pSipd_PTZ_Move(&ptz_rd);
				}
				else
				{
					sipd_ptz_control ptz_down;

					//	垂直向下
					req.speed = ptzcode.data2;
					//printf("%s line=%d DOWN in speed=%d \n", __FUNCTION__, __LINE__, req.speed);

					memset(&ptz_down, 0, sizeof(ptz_down));
					strncpy(ptz_down.device_id, req.device_id, MAX_SIPD_DEVICE_ID_LEN);
					ptz_down.cmd_type = SIPD_PTZ_MOVE_DOWN;
					ptz_down.speed = req.speed;

					if(pSipd_PTZ_Move)
						pSipd_PTZ_Move(&ptz_down);
				}
			}
			else
			{
				if(((ptzcode.instruction & 0x2)>>1) == 1)
				{
					sipd_ptz_control ptz_left;

					//	简单向左
					req.speed = 0;
					
					/*left*/
					req.speed = ptzcode.data1;
					//printf("%s line=%d Left in speed=%d \n", __FUNCTION__, __LINE__, req.speed);

					memset(&ptz_left, 0, sizeof(ptz_left));
					strncpy(ptz_left.device_id, req.device_id, MAX_SIPD_DEVICE_ID_LEN);
					ptz_left.cmd_type = SIPD_PTZ_MOVE_LEFT;
					ptz_left.speed = req.speed;

					if(pSipd_PTZ_Move)
						pSipd_PTZ_Move(&ptz_left);
				}
				else if((ptzcode.instruction & 0x1) == 1)
				{
					sipd_ptz_control ptz_right;
					//`简单向右
					req.speed = 0;
					
					/*right*/
					req.speed = ptzcode.data1;
					//printf("%s line=%d RIGHT in speed=%d \n", __FUNCTION__, __LINE__, req.speed);

					memset(&ptz_right, 0, sizeof(ptz_right));
					strncpy(ptz_right.device_id, req.device_id, MAX_SIPD_DEVICE_ID_LEN);
					ptz_right.cmd_type = SIPD_PTZ_MOVE_RIGHT;
					ptz_right.speed = req.speed;

					if(pSipd_PTZ_Move)
						pSipd_PTZ_Move(&ptz_right);
				}
				#if 0
				else
				{
					req.speed = 0;
					SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD, MOD_SIPD, 0, 0, 0, MSG_ID_SIPD_STOP_REQ, &req, sizeof(req));
				}
				#endif
			}
		}
		else if((((ptzcode.instruction & 0x80)>>7) == 0) && (((ptzcode.instruction & 0x40)>>6) == 1))
		{
			//	bit7 =0, bit6 = 1 , FI 指令
			struct sipd_ptz_req req;
			
			memset(&req, 0, sizeof(req));
			strncpy(req.device_id, device_id, MAX_SIPD_DEVICE_ID_LEN);

			//printf("%s line=%d bit7=0, bit6=1\n", __FUNCTION__, __LINE__);
			
			/*bit7 = 0,6 = 1*/
			if(((ptzcode.instruction & 0x8)>>3) == 1)
			{
				sipd_ptz_control  ptz_hallosmall;
				//	光圈缩小
				req.speed = 0;

				/*iris -*/
				req.speed = ptzcode.data2;
				//printf("%s line=%d iris - in speed=%d \n", __FUNCTION__, __LINE__, req.speed);

				memset(&ptz_hallosmall, 0, sizeof(ptz_hallosmall));
				strncpy(ptz_hallosmall.device_id, req.device_id, MAX_SIPD_DEVICE_ID_LEN);
				ptz_hallosmall.cmd_type = SIPD_PTZ_HALLO_SMALL;
				ptz_hallosmall.speed = req.speed;

				if(pSipd_PTZ_Move)
					pSipd_PTZ_Move(&ptz_hallosmall);
			}
			else if(((ptzcode.instruction & 0x4)>>2) == 1)
			{
				sipd_ptz_control  ptz_hallolarge;
				//	光圈放大
				req.speed = 0;
				
				/*iris +*/
				req.speed = ptzcode.data2;
				//printf("%s line=%d iris + in speed=%d \n", __FUNCTION__, __LINE__, req.speed);

				memset(&ptz_hallolarge, 0, sizeof(ptz_hallolarge));
				strncpy(ptz_hallolarge.device_id, req.device_id, MAX_SIPD_DEVICE_ID_LEN);
				ptz_hallolarge.cmd_type = SIPD_PTZ_HALLO_LARGE;
				ptz_hallolarge.speed = req.speed;

				if(pSipd_PTZ_Move)
					pSipd_PTZ_Move(&ptz_hallolarge);
			}
			else
			{
				sipd_ptz_control pPtz;
				req.speed = 0;
				//printf("%s line=%d iris stop in speed=%d \n", __FUNCTION__, __LINE__, req.speed);
				memset(&pPtz, 0, sizeof(pPtz));
				strncpy(pPtz.device_id, req.device_id, MAX_SIPD_DEVICE_ID_LEN);
				pPtz.cmd_type = SIPD_PTZ_MOVE_STOP;
				pPtz.speed = req.speed;

				if(pSipd_PTZ_Move)
					pSipd_PTZ_Move(&pPtz);
			}


			if(((ptzcode.instruction & 0x2)>>1) == 1)
			{
				sipd_ptz_control  ptz_focusnear;

				//	聚焦更近
				req.speed = 0;
				
				/*focus near*/
				req.speed = ptzcode.data1;
				//printf("%s line=%d focus near in speed=%d \n", __FUNCTION__, __LINE__, req.speed);

				memset(&ptz_focusnear, 0, sizeof(ptz_focusnear));
				strncpy(ptz_focusnear.device_id, req.device_id, MAX_SIPD_DEVICE_ID_LEN);
				ptz_focusnear.cmd_type = SIPD_PTZ_FOCUS_NEAR;
				ptz_focusnear.speed = req.speed;

				if(pSipd_PTZ_Move)
					pSipd_PTZ_Move(&ptz_focusnear);
			}
			else if((ptzcode.instruction & 0x1) == 1)
			{
				sipd_ptz_control  ptz_focusfar;
				//	聚焦更远
				req.speed = 0;
				
				/*focus far*/
				req.speed = ptzcode.data1;
				//printf("%s line=%d focus far in speed=%d \n", __FUNCTION__, __LINE__, req.speed);

				memset(&ptz_focusfar, 0, sizeof(ptz_focusfar));
				strncpy(ptz_focusfar.device_id, req.device_id, MAX_SIPD_DEVICE_ID_LEN);
				ptz_focusfar.cmd_type = SIPD_PTZ_FOCUS_FAR;
				ptz_focusfar.speed = req.speed;

				if(pSipd_PTZ_Move)
					pSipd_PTZ_Move(&ptz_focusfar);
			}
			#if 0
			else
			{
				req.speed = 0;
				SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD, MOD_SIPD, 0, 0, 0, MSG_ID_SIPD_STOP_REQ, &req, sizeof(req));
			}
			#endif
			
			
		}
		else
		{
			printf("%s line=%d  instruction code =%#x ERROR, not defined\n", __FUNCTION__, __LINE__, ptzcode.instruction);
			return -1;
		}
	}
	
	

	return 0;
}


static int handle_method_INFO(const osip_message_t * sip_request, int tid, int did)
{
	osip_body_t *body;
	osip_content_type_t* content_type=NULL;
	struct sipd_playback_mans_rtsp_ctrl_ind  mans_rtsp_ind;


	if(NULL == sip_request)
	{
		printf("%s line=%d sip_request is NULL, return \n", __FUNCTION__, __LINE__);
		return -1;
	}

	content_type = osip_message_get_content_type(sip_request);
	if(NULL == content_type)
	{
		printf("%s line=%d content_type is NULL! \n", __FUNCTION__, __LINE__);
		return -1;
	}

	if((strcasecmp(content_type->type, CONTENT_TYPE_APP) == 0) && (strcasecmp(content_type->subtype, CONTENT_TYPE_SUB_MANSRTSP) == 0))
	{
		sipd_playback_mans_rtsp_ind playback_ind;

		osip_message_get_body (sip_request, 0, &body); 

		memset(&mans_rtsp_ind, 0, sizeof(mans_rtsp_ind));
		strncpy(mans_rtsp_ind.rtsp_des, body->body,MAX_SIPD_INFO_LENGTH);
		mans_rtsp_ind.len = strlen(mans_rtsp_ind.rtsp_des);
		mans_rtsp_ind.tid = tid;
		mans_rtsp_ind.did = did;


		printf("sipd_MsgHandle MSG_ID_GW_INVITE_INFO_IND \n");
		printf("sipd_MsgHandle mans_rtsp_des=%s \n", mans_rtsp_ind.rtsp_des);

		memset(&playback_ind, 0, sizeof(playback_ind));
		strncpy(playback_ind.rtsp_des, mans_rtsp_ind.rtsp_des, MAX_SIPD_INFO_LENGTH);
		playback_ind.len = mans_rtsp_ind.len;

		if(pSipd_media_control_notify_CMS)
			pSipd_media_control_notify_CMS(mans_rtsp_ind.did, mans_rtsp_ind.tid, SIPD_TO_CMS_CMD_TYPE_PLAYBACK_CONTROL_RTSP, &playback_ind);
	}
	else if((strcasecmp(content_type->type, CONTENT_TYPE_APP) == 0) && (strcasecmp(content_type->subtype, CONTENT_TYPE_SUB_RTSP) == 0))
	{
		sipd_playback_mans_rtsp_ind playback_ind;

		osip_message_get_body (sip_request, 0, &body); 

		memset(&mans_rtsp_ind, 0, sizeof(mans_rtsp_ind));
		strncpy(mans_rtsp_ind.rtsp_des, body->body,MAX_SIPD_INFO_LENGTH);
		mans_rtsp_ind.len = strlen(mans_rtsp_ind.rtsp_des);
		mans_rtsp_ind.tid = tid;
		mans_rtsp_ind.did = did;

		printf("sipd_MsgHandle MSG_ID_GW_INVITE_INFO_IND \n");
		printf("sipd_MsgHandle mans_rtsp_des=%s \n", mans_rtsp_ind.rtsp_des);

		memset(&playback_ind, 0, sizeof(playback_ind));
		strncpy(playback_ind.rtsp_des, mans_rtsp_ind.rtsp_des, MAX_SIPD_INFO_LENGTH);
		playback_ind.len = mans_rtsp_ind.len;

		if(pSipd_media_control_notify_CMS)
			pSipd_media_control_notify_CMS(mans_rtsp_ind.did, mans_rtsp_ind.tid, SIPD_TO_CMS_CMD_TYPE_PLAYBACK_CONTROL_RTSP, &playback_ind);
	}

	return 0;
}


static int handle_method_SUBSCRIBE(const osip_message_t * sip_request, int tid, int did)
{
	osip_body_t *body = NULL;
	osip_content_type_t* content_type=NULL;
	mxml_node_t *node;
	int whitespace;
	mxml_node_t *tree, *que_node, *CmdType_node, *sn_node;
	SIPD_GATEWAY_CFG sipd_cfg;

	osip_header_t *expires_header = NULL;
	int expires = 0;

	char strbuf[MAX_SIPD_URI];
	memset(strbuf, 0, sizeof(strbuf));

	memset(&sipd_cfg, 0, sizeof(sipd_cfg));
	get_PRM_SIPD_CFG(&sipd_cfg);

	if(NULL == sip_request)
	{
		return -1;
	}

	content_type = osip_message_get_content_type(sip_request);
	if(NULL == content_type)
	{
		printf("%s line=%d content_type is NULL! \n", __FUNCTION__, __LINE__);
		return -1;
	}
	osip_message_get_expires(sip_request, 0, &expires_header);
	expires = osip_atoi(expires_header->hvalue);

	if((strcasecmp(content_type->type, CONTENT_TYPE_APP) == 0) && (strcasecmp(content_type->subtype, CONTENT_TYPE_SUB_MANSCMD) == 0))
	{
		osip_message_get_body (sip_request, 0, &body); 

		tree = mxmlLoadString(NULL, body->body, MXML_TEXT_CALLBACK);
		if(NULL == tree)
		{
			printf("%s line=%d mxmlLoadString is NULL. ERROR......\n", __FUNCTION__, __LINE__);
			return -1;
		}

		que_node = mxmlFindElement(tree, tree, QUERY, NULL, NULL,MXML_DESCEND);

		if(que_node != NULL)
		{
			CmdType_node = mxmlFindElement(que_node, tree, CMDTYPE, NULL, NULL,MXML_DESCEND);
			if(CmdType_node != NULL)
			{
				mxml_node_t *DeviceID_node = NULL;
				memcpy(strbuf,  (void *)mxmlGetText(CmdType_node, NULL), sizeof(strbuf));
				DeviceID_node = mxmlFindElement(que_node, tree, "DeviceID", NULL, NULL,MXML_DESCEND);

				if(DeviceID_node != NULL)
				{
					char id[MAX_SIPD_DEVICE_ID_LEN+1];
					char snbuf[MAX_SIPD_DEVICE_ID_LEN+1];

					memset(id, 0, sizeof(id));
					memcpy(id, (void *)mxmlGetText(DeviceID_node, NULL), sizeof(id));

					memset(snbuf, 0, sizeof(snbuf));
					sn_node = mxmlFindElement(que_node, tree, "SN", NULL, NULL, MXML_DESCEND);

					if(sn_node != NULL)
					{
						sipd_deviceinfo_req devinfo;
						memcpy(snbuf, (void *)mxmlGetText(sn_node, NULL), sizeof(snbuf));

						if(strcmp(sip_request->from->url->host, (char*)sipd_cfg.upper_server_name) == 0)
						{
							sprintf(devinfo.to_uri,"sip:%s@%s:%d", (char*)sipd_cfg.upper_server_id, (char*)sipd_cfg.upper_server_ip, (char*)sipd_cfg.upper_server_port);

							if(sip_request->to->url->port != NULL)
							{
								snprintf(devinfo.from_uri, sizeof(devinfo.from_uri), "sip:%s@%s:%s", sip_request->to->url->username,
									sip_request->to->url->host, sip_request->to->url->port);
							}
							else
							{
								snprintf(devinfo.from_uri, sizeof(devinfo.from_uri), "sip:%s@%s", sip_request->to->url->username,
									sip_request->to->url->host);
							}

						}
						else	
						{
							if(sip_request->to->url->port != NULL)
							{
								snprintf(devinfo.from_uri, sizeof(devinfo.from_uri), "sip:%s@%s:%s", sip_request->to->url->username,
									sip_request->to->url->host, sip_request->to->url->port);
							}
							else
							{
								snprintf(devinfo.from_uri, sizeof(devinfo.from_uri), "sip:%s@%s", sip_request->to->url->username,
									sip_request->to->url->host);
							}

							if(sip_request->from->url->port != NULL)
							{
								snprintf(devinfo.to_uri, sizeof(devinfo.to_uri), "sip:%s@%s:%s", sip_request->from->url->username,
									sip_request->from->url->host, sip_request->from->url->port);
							}
							else
							{
								snprintf(devinfo.to_uri, sizeof(devinfo.to_uri), "sip:%s@%s", sip_request->from->url->username,
									sip_request->from->url->host);
							}
						}


						memcpy(devinfo.device_id, id, sizeof(devinfo.device_id));
						memcpy(devinfo.SN, snbuf, sizeof(devinfo.SN));


						if(strcmp(strbuf, DEVICECATALOG) == 0) 
						{
							sipd_catalog_req catalog_req;


							memset(&catalog_req, 0, sizeof(catalog_req));

							strncpy(catalog_req.device_id, devinfo.device_id, MAX_SIPD_DEVICE_ID_LEN);
							strncpy(catalog_req.SN, devinfo.SN, MAX_SIPD_DEVICE_ID_LEN);
							strncpy(catalog_req.from_uri, devinfo.from_uri, MAX_SIPD_URI);
							strncpy(catalog_req.to_uri, devinfo.to_uri, MAX_SIPD_URI);
							catalog_req.tid = tid;
							catalog_req.did = did;
							catalog_req.expires = expires;

							node = mxmlFindElement(tree, tree, XML_NAME_StartTime, NULL, NULL, MXML_DESCEND);
							if(NULL != node)
							{	
								strncpy(catalog_req.StartTime, mxmlGetText(node, &whitespace), MAX_SIPD_TIME_LEN);
							}

							node = mxmlFindElement(tree, tree, XML_NAME_EndTime, NULL, NULL, MXML_DESCEND);
							if(NULL != node)
							{	
								strncpy(catalog_req.EndTime, mxmlGetText(node, &whitespace), MAX_SIPD_TIME_LEN);
							}

//#ifdef WIN32
//							SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD_SUBSCRIBE, MSG_ID_GW_DESCRIBE_CATALOG_REQ, &catalog_req, sizeof(catalog_req));
//#else
//							SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0,MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, 
//								MSG_ID_GW_CATALOGINFO_REQ, (void *) &catalog_req, sizeof(catalog_req));
//#endif
						}

						else if(strcmp(strbuf, XML_VALUE_CmdType_Alarm) == 0) 
						{
							sipd_perset_query_req perset_query_req;

							printf("%s line=%d device perset query \n", __FUNCTION__, __LINE__);

							memset(&perset_query_req, 0, sizeof(perset_query_req));

							strncpy(perset_query_req.device_id, devinfo.device_id, MAX_SIPD_DEVICE_ID_LEN);
							strncpy(perset_query_req.SN, devinfo.SN, MAX_SIPD_DEVICE_ID_LEN);
							strncpy(perset_query_req.from_uri, devinfo.from_uri, MAX_SIPD_URI);
							strncpy(perset_query_req.to_uri, devinfo.to_uri, MAX_SIPD_URI);

							node = mxmlFindElement(tree, tree, XML_NAME_StartTime, NULL, NULL, MXML_DESCEND);
							if(NULL != node)
							{	
								strncpy(perset_query_req.StartTime, mxmlGetText(node, &whitespace), MAX_SIPD_TIME_LEN);
							}

							node = mxmlFindElement(tree, tree, XML_NAME_EndTime, NULL, NULL, MXML_DESCEND);
							if(NULL != node)
							{	
								strncpy(perset_query_req.EndTime, mxmlGetText(node, &whitespace), MAX_SIPD_TIME_LEN);
							}

//#ifdef WIN32
//							SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD_SUBSCRIBE, MSG_ID_GW_DESCRIBE_ALARM_REQ, &perset_query_req, sizeof(perset_query_req));
//#else
//							SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0,MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, 
//								MSG_ID_GW_CATALOGINFO_REQ, (void *) &perset_query_req, sizeof(perset_query_req));
//#endif
						}
					}

				}
			}
		}
		else
		{
			printf("sipd_manscdp_xml_deal que_node is NULL\n");
		}

		mxmlDelete(tree);
	}
	else
		return -1;
}

static int handle_method_MESSAGE(const osip_message_t * sip_request)
{
	osip_body_t *body = NULL;
	osip_content_type_t* content_type=NULL;
	char buffer[MAX_XML_BUFFER_LEN+1]={0};
       mxml_node_t *tree;
	struct sipd_device_control device_control;
	mxml_node_t *node;
	int ret;
	int whitespace;
	int is_device_control_message = 0;
	int is_alarm_message_response = 0;
	SIPD_GATEWAY_CFG sipd_cfg;
	
	if(NULL == sip_request)
	{
		return -1;
	}
	
	memset(&device_control, 0, sizeof(device_control));
	is_device_control_message = 0;
	is_alarm_message_response = 0;

	memset(&sipd_cfg, 0, sizeof(sipd_cfg));
	get_PRM_SIPD_CFG(&sipd_cfg);
	
	
	content_type = osip_message_get_content_type(sip_request);
	if(NULL == content_type)
	{
		printf("%s line=%d content_type is NULL! \n", __FUNCTION__, __LINE__);
		return -1;
	}
	
	//printf("%s line=%d contetnt_type type=%s sub_type=%s \n", __FUNCTION__, __LINE__, content_type->type, content_type->subtype);
       if((strcasecmp(content_type->type, CONTENT_TYPE_APP) == 0) && (strcasecmp(content_type->subtype, CONTENT_TYPE_SUB_MANSCMD) == 0))
       {
       	/*Application/MANSCDP+xml*/
		//printf("%s line=%d \n", __FUNCTION__, __LINE__);
		osip_message_get_body (sip_request, 0, &body); 
             // printf("%s line=%d body->length=%d I get the msg is: %s\n", __FUNCTION__, __LINE__, body->length, body->body);
			   
		/*allocate xml space*/
		memset(buffer, 0, sizeof(buffer));
		if(body->length > MAX_XML_BUFFER_LEN)
		{
			printf("%s line=%d body->length=%d  body length exceed! ERROR......\n", __FUNCTION__, __LINE__, body->length);
			return -1;
		}

		strncpy(buffer, body->body,MAX_XML_BUFFER_LEN);
		
		tree = mxmlLoadString(NULL, buffer,MXML_TEXT_CALLBACK);
		if(NULL == tree)
		{
			printf("%s line=%d mxmlLoadString is NULL. ERROR......\n", __FUNCTION__, __LINE__);
			return -1;
		}

		/*fill ptz_control*/
		node = mxmlFindElement(tree, tree, XML_NAME_CmdType, NULL, NULL, MXML_DESCEND);
		if(NULL == node)
		{	
		       printf("%s line=%d parse error. XML Cann't Find CmdType......\n", __FUNCTION__, __LINE__);
			if(tree != NULL)
			{
				mxmlDelete(tree);
			}
			return -1;
		}
		//printf("%s line=%d  CmdType=%s\n", __FUNCTION__, __LINE__, mxmlGetText(node, &whitespace));
		#if 1
		if(strcmp(mxmlGetText(node, &whitespace), XML_VALUE_CmdType_DeviceControl) == 0)
		{
			is_device_control_message = 1;
		}
		else if(strcmp(mxmlGetText(node, &whitespace), XML_VALUE_CmdType_Alarm) == 0)
		{
			node = mxmlFindElement(tree, tree, XML_NAME_Result, NULL, NULL, MXML_DESCEND);
			if(NULL == node)
			{	
			       printf("%s line=%d parse error. XML Cann't Find Result in alram response message......\n", __FUNCTION__, __LINE__);
				if(tree != NULL)
				{
					mxmlDelete(tree);
				} 
				return -1;
			}
			//TRACE(SCI_TRACE_HIGH, MOD_SIPD, "%s line=%d  Result=%s\n", __FUNCTION__, __LINE__, mxmlGetText(node, &whitespace));
			is_alarm_message_response = 1;
		}
		else
		{
			is_alarm_message_response = 0;
			is_device_control_message = 0;
		}
		#else
		if(strcmp(mxmlGetText(node, &whitespace), XML_VALUE_CmdType_DeviceControl) != 0)
		{
			printf ("%s line=%d parse error. CmdType=%s is not equal DeviceControl....../n", __FUNCTION__, __LINE__, mxmlGetText(node, &whitespace));
			 return -1;
		}
		#endif

		//printf("%s line=%d  is_device_control_message = %d is_alarm_message_response=%d \n", __FUNCTION__, __LINE__, is_device_control_message, is_alarm_message_response);
		//	设备控制等
		if(is_device_control_message)
		{
	
		}
		
		/*free xml space*/
		if(tree != NULL)
		{
			mxmlDelete(tree);
		}
		
       }
	else
	{
		return -1;
	}

	sipd_manscdp_xml_deal(sip_request, buffer);
	
	if(is_device_control_message)
	{
		
		
	}
	else if (is_alarm_message_response)
	{
		//printf("%s line=%d  message is_alarm_message_response\n", __FUNCTION__, __LINE__);
	}
	else
	{
		//printf("%s line=%d  check is query MESSAGE or not! \n", __FUNCTION__, __LINE__);

		// 处理目录查询、设备信息查询、设备状态查询
		
	}

	return 0;
        
}

int GMTStrToTime_t(char *timestr, PRM_TIME *outtime)
{
	char *Month[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	char date_buf[6][32];
	char time_buf[3][3];
	char *ptr = NULL;
	int i = 0, j = 0;

	if(timestr == NULL || NULL == outtime)
	{
		return -1;
	}

	if(strstr(timestr, "GMT") != NULL)
	{
		for(ptr = timestr; *ptr != '\0'; )
		{
			if(*ptr == ' ')
			{
				date_buf[i][j] = '\0';
				ptr++;
				i++;
				j = 0;
			}

			date_buf[i][j] = *ptr;
			j++;

			ptr++;
		}

		i = 0;
		j = 0;

		for(ptr = date_buf[4]; *ptr != '\0';)
		{
			if(*ptr == ':')
			{
				time_buf[i][j] = '\0';
				ptr++;
				i++;
				j = 0;
			}

			time_buf[i][j] = *ptr;
			j++;

			ptr++;
		}

		for(i = 0; i<12; i++)
		{
			if(strcmp(Month[i], date_buf[2]) == 0)
			{
				outtime->Month = (UINT16)(i+1);
				break;
			}
		}

		outtime->Day = (unsigned short)osip_atoi(date_buf[1]);
		outtime->Year = (unsigned short)osip_atoi(date_buf[3]);
		outtime->Hour = (unsigned short)osip_atoi(time_buf[0]);
		outtime->Minute = (unsigned short)osip_atoi(time_buf[1]);
		outtime->Second = (unsigned short)osip_atoi(time_buf[2]);

		return 0;
	}
	else
	{
		for(ptr = timestr; *ptr != '\0'; )
		{
			if((*ptr == '-')||(*ptr == 'T')||(*ptr == ':'))
			{
				date_buf[i][j] = '\0';
				ptr++;
				i++;
				j = 0;
			}

			if(*ptr == '.')
				break;

			date_buf[i][j] = *ptr;
			j++;

			ptr++;
		}

		outtime->Year = (unsigned short)osip_atoi(date_buf[0]);
		outtime->Month = (unsigned short)osip_atoi(date_buf[1]);
		outtime->Day = (unsigned short)osip_atoi(date_buf[2]);
		outtime->Hour = (unsigned short)osip_atoi(date_buf[3]);
		outtime->Minute = (unsigned short)osip_atoi(date_buf[4]);
		outtime->Second = (unsigned short)osip_atoi(date_buf[5]);

		return 0;
	}
}


static int get_cataloginfo_pair(int start_index, struct sipd_recorditem_pair* pitem_pair, const sipd_catainfo_adaptor_rsp *prsp, BOOL isNotify)
{
	int len=0;
	BOOL isDetail;

	if(NULL == pitem_pair  || NULL == prsp)
	{
		printf("get_recordinfo_pair input param error!\n");
		return -1;
	}


	//printf("get_recordinfo_pair start_index=%d \n", start_index);

	if(start_index > (prsp->basic_info.DeviceList_Num-1))
	{
		printf("get_recordinfo_pair already get last item. Warning! DeviceList_Num=%d\n", prsp->basic_info.DeviceList_Num);
		return -1;
	}

	
	
	if(start_index == (prsp->basic_info.DeviceList_Num-1))
	{
		pitem_pair->is_last_one_item = 1;
	}
	else
	{
		pitem_pair->is_last_one_item = 0;
	}

	
	/*item1*/
	snprintf(pitem_pair->item_pair_buffer[0],MAX_SIPD_FILEITEM_LEN,
	"<Item>\r\n"\
	"<DeviceID>%s</DeviceID>\r\n",
	prsp->cata_items[start_index].device_id);

	isDetail = FALSE;
	if(isNotify)
	{
		len = strlen(pitem_pair->item_pair_buffer[0]);
		snprintf(pitem_pair->item_pair_buffer[0]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Event>%s</Event>\r\n", prsp->cata_items[start_index].Event);
		if(strcmp(prsp->cata_items[start_index].Event,"ADD")==0)
			isDetail = TRUE;
		else if(strcmp(prsp->cata_items[start_index].Event,"UPDATE")==0)
			isDetail = TRUE;
	}

	if(isDetail)
	{
		if(strlen(prsp->cata_items[start_index].Name) > 0)
		{
			len = strlen(pitem_pair->item_pair_buffer[0]);
			snprintf(pitem_pair->item_pair_buffer[0]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Name>%s</Name>\r\n", prsp->cata_items[start_index].Name);
		}

		if(strlen(prsp->cata_items[start_index].Manufacturer) > 0)
		{
			len = strlen(pitem_pair->item_pair_buffer[0]);
			snprintf(pitem_pair->item_pair_buffer[0]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Manufacturer>%s</Manufacturer>\r\n", prsp->cata_items[start_index].Manufacturer);
		}

		if(strlen(prsp->cata_items[start_index].Model) > 0)
		{
			len = strlen(pitem_pair->item_pair_buffer[0]);
			snprintf(pitem_pair->item_pair_buffer[0]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Model>%s</Model>\r\n", prsp->cata_items[start_index].Model);
		}

		if(strlen(prsp->cata_items[start_index].Owner) > 0)
		{
			len = strlen(pitem_pair->item_pair_buffer[0]);
			snprintf(pitem_pair->item_pair_buffer[0]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Owner>%s</Owner>\r\n", prsp->cata_items[start_index].Owner);
		}

		if(strlen(prsp->cata_items[start_index].CivilCode) > 0)
		{
			len = strlen(pitem_pair->item_pair_buffer[0]);
			snprintf(pitem_pair->item_pair_buffer[0]+len, (MAX_SIPD_FILEITEM_LEN-len),"<CivilCode>%s</CivilCode>\r\n", prsp->cata_items[start_index].CivilCode);
		}

		if(strlen(prsp->cata_items[start_index].Block) > 0)
		{
			len = strlen(pitem_pair->item_pair_buffer[0]);
			snprintf(pitem_pair->item_pair_buffer[0]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Block>%s</Block>\r\n", prsp->cata_items[start_index].Block);
		}

		if(strlen(prsp->cata_items[start_index].Address) > 0)
		{
			len = strlen(pitem_pair->item_pair_buffer[0]);
			snprintf(pitem_pair->item_pair_buffer[0]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Address>%s</Address>\r\n", prsp->cata_items[start_index].Address);
		}

		len = strlen(pitem_pair->item_pair_buffer[0]);
		snprintf(pitem_pair->item_pair_buffer[0]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Parental>%d</Parental>\r\n", prsp->cata_items[start_index].Parental);

		if(strlen(prsp->cata_items[start_index].ParentID) > 0)
		{
			len = strlen(pitem_pair->item_pair_buffer[0]);
			snprintf(pitem_pair->item_pair_buffer[0]+len, (MAX_SIPD_FILEITEM_LEN-len),"<ParentID>%s</ParentID>\r\n", prsp->cata_items[start_index].ParentID);
		}

		len = strlen(pitem_pair->item_pair_buffer[0]);
		snprintf(pitem_pair->item_pair_buffer[0]+len, (MAX_SIPD_FILEITEM_LEN-len),"<SafetyWay>%d</SafetyWay>\r\n", prsp->cata_items[start_index].SafteyWay);

		len = strlen(pitem_pair->item_pair_buffer[0]);
		snprintf(pitem_pair->item_pair_buffer[0]+len, (MAX_SIPD_FILEITEM_LEN-len),"<RegisterWay>%d</RegisterWay>\r\n", prsp->cata_items[start_index].RegisterWay);

		if(strlen(prsp->cata_items[start_index].CertNum) > 0)
		{
			len = strlen(pitem_pair->item_pair_buffer[0]);
			snprintf(pitem_pair->item_pair_buffer[0]+len, (MAX_SIPD_FILEITEM_LEN-len),"<CertNum>%s</CertNum>\r\n", prsp->cata_items[start_index].CertNum);
		}

		len = strlen(pitem_pair->item_pair_buffer[0]);
		snprintf(pitem_pair->item_pair_buffer[0]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Certifiable>%d</Certifiable>\r\n", prsp->cata_items[start_index].Certifiable);

		len = strlen(pitem_pair->item_pair_buffer[0]);
		snprintf(pitem_pair->item_pair_buffer[0]+len, (MAX_SIPD_FILEITEM_LEN-len),"<ErrCode>%d</ErrCode>\r\n", prsp->cata_items[start_index].ErrCode);

		if(strlen(prsp->cata_items[start_index].EndTime) > 0)
		{
			len = strlen(pitem_pair->item_pair_buffer[0]);
			snprintf(pitem_pair->item_pair_buffer[0]+len, (MAX_SIPD_FILEITEM_LEN-len),"<EndTime>%s</EndTime>\r\n", prsp->cata_items[start_index].EndTime);
		}

		len = strlen(pitem_pair->item_pair_buffer[0]);
		snprintf(pitem_pair->item_pair_buffer[0]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Secrecy>%d</Secrecy>\r\n", prsp->cata_items[start_index].Secrecy);

		if(strlen(prsp->cata_items[start_index].IPAddress) > 0)
		{
			len = strlen(pitem_pair->item_pair_buffer[0]);
			snprintf(pitem_pair->item_pair_buffer[0]+len, (MAX_SIPD_FILEITEM_LEN-len),"<IPAddress>%s</IPAddress>\r\n", prsp->cata_items[start_index].IPAddress);
		}

		if(prsp->cata_items[start_index].Port != 0)
		{
			len = strlen(pitem_pair->item_pair_buffer[0]);
			snprintf(pitem_pair->item_pair_buffer[0]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Port>%d</Port>\r\n", prsp->cata_items[start_index].Port);
		}

		if(strlen(prsp->cata_items[start_index].Password) > 0)
		{
			len = strlen(pitem_pair->item_pair_buffer[0]);
			snprintf(pitem_pair->item_pair_buffer[0]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Password>%s</Password>\r\n", prsp->cata_items[start_index].Password);
		}

		if(SIPD_STATUS_ON == prsp->cata_items[start_index].Status)
		{
			len = strlen(pitem_pair->item_pair_buffer[0]);
			snprintf(pitem_pair->item_pair_buffer[0]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Status>%s</Status>\r\n", "ON");
		}
		else if(SIPD_STATUS_OFF == prsp->cata_items[start_index].Status)
		{
			len = strlen(pitem_pair->item_pair_buffer[0]);
			snprintf(pitem_pair->item_pair_buffer[0]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Status>%s</Status>\r\n", "OFF");
		}

		if(strlen(prsp->cata_items[start_index].Longitude) > 0)
		{
			len = strlen(pitem_pair->item_pair_buffer[0]);
			snprintf(pitem_pair->item_pair_buffer[0]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Longitude>%s</Longitude>\r\n", prsp->cata_items[start_index].Longitude);
		}

		if(strlen(prsp->cata_items[start_index].Latitude) > 0)
		{
			len = strlen(pitem_pair->item_pair_buffer[0]);
			snprintf(pitem_pair->item_pair_buffer[0]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Latitude>%s</Latitude>\r\n", prsp->cata_items[start_index].Latitude);
		}
		
	}
	len = strlen(pitem_pair->item_pair_buffer[0]);
	snprintf(pitem_pair->item_pair_buffer[0]+len, (MAX_SIPD_FILEITEM_LEN-len),"</Item>\r\n");


	/*item2*/
	if(0 == pitem_pair->is_last_one_item)
	{
		/*item2*/
		snprintf(pitem_pair->item_pair_buffer[1],MAX_SIPD_FILEITEM_LEN,
		"<Item>\r\n"\
		"<DeviceID>%s</DeviceID>\r\n",
		prsp->cata_items[start_index+1].device_id);

		isDetail = FALSE;
		if(isNotify)
		{
			len = strlen(pitem_pair->item_pair_buffer[1]);
			snprintf(pitem_pair->item_pair_buffer[1]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Event>%s</Event>\r\n", prsp->cata_items[start_index+1].Event);
			if(strcmp(prsp->cata_items[start_index+1].Event,"ADD")==0)
				isDetail = TRUE;
			else if(strcmp(prsp->cata_items[start_index+1].Event,"UPDATE")==0)
				isDetail = TRUE;
		}

		if(isDetail)
		{
			if(strlen(prsp->cata_items[start_index+1].Name) > 0)
			{
				len = strlen(pitem_pair->item_pair_buffer[1]);
				snprintf(pitem_pair->item_pair_buffer[1]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Name>%s</Name>\r\n", prsp->cata_items[start_index+1].Name);
			}

			if(strlen(prsp->cata_items[start_index+1].Manufacturer) > 0)
			{
				len = strlen(pitem_pair->item_pair_buffer[1]);
				snprintf(pitem_pair->item_pair_buffer[1]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Manufacturer>%s</Manufacturer>\r\n", prsp->cata_items[start_index+1].Manufacturer);
			}

			if(strlen(prsp->cata_items[start_index+1].Model) > 0)
			{
				len = strlen(pitem_pair->item_pair_buffer[1]);
				snprintf(pitem_pair->item_pair_buffer[1]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Model>%s</Model>\r\n", prsp->cata_items[start_index+1].Model);
			}

			if(strlen(prsp->cata_items[start_index+1].Owner) > 0)
			{
				len = strlen(pitem_pair->item_pair_buffer[1]);
				snprintf(pitem_pair->item_pair_buffer[1]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Owner>%s</Owner>\r\n", prsp->cata_items[start_index+1].Owner);
			}

			if(strlen(prsp->cata_items[start_index+1].CivilCode) > 0)
			{
				len = strlen(pitem_pair->item_pair_buffer[1]);
				snprintf(pitem_pair->item_pair_buffer[1]+len, (MAX_SIPD_FILEITEM_LEN-len),"<CivilCode>%s</CivilCode>\r\n", prsp->cata_items[start_index+1].CivilCode);
			}

			if(strlen(prsp->cata_items[start_index+1].Block) > 0)
			{
				len = strlen(pitem_pair->item_pair_buffer[1]);
				snprintf(pitem_pair->item_pair_buffer[1]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Block>%s</Block>\r\n", prsp->cata_items[start_index+1].Block);
			}

			if(strlen(prsp->cata_items[start_index+1].Address) > 0)
			{
				len = strlen(pitem_pair->item_pair_buffer[1]);
				snprintf(pitem_pair->item_pair_buffer[1]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Address>%s</Address>\r\n", prsp->cata_items[start_index+1].Address);
			}

			len = strlen(pitem_pair->item_pair_buffer[1]);
			snprintf(pitem_pair->item_pair_buffer[1]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Parental>%d</Parental>\r\n", prsp->cata_items[start_index+1].Parental);

			if(strlen(prsp->cata_items[start_index+1].ParentID) > 0)
			{
				len = strlen(pitem_pair->item_pair_buffer[1]);
				snprintf(pitem_pair->item_pair_buffer[1]+len, (MAX_SIPD_FILEITEM_LEN-len),"<ParentID>%s</ParentID>\r\n", prsp->cata_items[start_index+1].ParentID);
			}

			len = strlen(pitem_pair->item_pair_buffer[1]);
			snprintf(pitem_pair->item_pair_buffer[1]+len, (MAX_SIPD_FILEITEM_LEN-len),"<SafetyWay>%d</SafetyWay>\r\n", prsp->cata_items[start_index+1].SafteyWay);

			len = strlen(pitem_pair->item_pair_buffer[1]);
			snprintf(pitem_pair->item_pair_buffer[1]+len, (MAX_SIPD_FILEITEM_LEN-len),"<RegisterWay>%d</RegisterWay>\r\n", prsp->cata_items[start_index+1].RegisterWay);

			if(strlen(prsp->cata_items[start_index+1].CertNum) > 0)
			{
				len = strlen(pitem_pair->item_pair_buffer[1]);
				snprintf(pitem_pair->item_pair_buffer[1]+len, (MAX_SIPD_FILEITEM_LEN-len),"<CertNum>%s</CertNum>\r\n", prsp->cata_items[start_index+1].CertNum);
			}

			len = strlen(pitem_pair->item_pair_buffer[1]);
			snprintf(pitem_pair->item_pair_buffer[1]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Certifiable>%d</Certifiable>\r\n", prsp->cata_items[start_index+1].Certifiable);

			len = strlen(pitem_pair->item_pair_buffer[1]);
			snprintf(pitem_pair->item_pair_buffer[1]+len, (MAX_SIPD_FILEITEM_LEN-len),"<ErrCode>%d</ErrCode>\r\n", prsp->cata_items[start_index+1].ErrCode);

			if(strlen(prsp->cata_items[start_index+1].EndTime) > 0)
			{
				len = strlen(pitem_pair->item_pair_buffer[1]);
				snprintf(pitem_pair->item_pair_buffer[1]+len, (MAX_SIPD_FILEITEM_LEN-len),"<EndTime>%s</EndTime>\r\n", prsp->cata_items[start_index+1].EndTime);
			}

			len = strlen(pitem_pair->item_pair_buffer[1]);
			snprintf(pitem_pair->item_pair_buffer[1]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Secrecy>%d</Secrecy>\r\n", prsp->cata_items[start_index+1].Secrecy);

			if(strlen(prsp->cata_items[start_index+1].IPAddress) > 0)
			{
				len = strlen(pitem_pair->item_pair_buffer[1]);
				snprintf(pitem_pair->item_pair_buffer[1]+len, (MAX_SIPD_FILEITEM_LEN-len),"<IPAddress>%s</IPAddress>\r\n", prsp->cata_items[start_index+1].IPAddress);
			}

			if(prsp->cata_items[start_index+1].Port != 0)
			{
				len = strlen(pitem_pair->item_pair_buffer[1]);
				snprintf(pitem_pair->item_pair_buffer[1]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Port>%d</Port>\r\n", prsp->cata_items[start_index+1].Port);
			}

			if(strlen(prsp->cata_items[start_index+1].Password) > 0)
			{
				len = strlen(pitem_pair->item_pair_buffer[1]);
				snprintf(pitem_pair->item_pair_buffer[1]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Password>%s</Password>\r\n", prsp->cata_items[start_index+1].Password);
			}

			if(SIPD_STATUS_ON == prsp->cata_items[start_index+1].Status)
			{
				len = strlen(pitem_pair->item_pair_buffer[1]);
				snprintf(pitem_pair->item_pair_buffer[1]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Status>%s</Status>\r\n", "ON");
			}
			else if(SIPD_STATUS_OFF == prsp->cata_items[start_index+1].Status)
			{
				len = strlen(pitem_pair->item_pair_buffer[1]);
				snprintf(pitem_pair->item_pair_buffer[1]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Status>%s</Status>\r\n", "OFF");
			}

			if(strlen(prsp->cata_items[start_index+1].Longitude) > 0)
			{
				len = strlen(pitem_pair->item_pair_buffer[1]);
				snprintf(pitem_pair->item_pair_buffer[1]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Longitude>%s</Longitude>\r\n", prsp->cata_items[start_index+1].Longitude);
			}

			if(strlen(prsp->cata_items[start_index+1].Latitude) > 0)
			{
				len = strlen(pitem_pair->item_pair_buffer[1]);
				snprintf(pitem_pair->item_pair_buffer[1]+len, (MAX_SIPD_FILEITEM_LEN-len),"<Latitude>%s</Latitude>\r\n", prsp->cata_items[start_index+1].Latitude);
			}

		}

		len = strlen(pitem_pair->item_pair_buffer[1]);
		snprintf(pitem_pair->item_pair_buffer[1]+len, (MAX_SIPD_FILEITEM_LEN-len),"</Item>\r\n");
	}

	
	return 0;
}



static int send_cataloginfo_pair_rsp(const sipd_catainfo_adaptor_rsp *prsp, char* body_buffer)
{
	osip_message_t *message = NULL;
	int ret;

	if(NULL == prsp  || NULL == body_buffer)
	{
		return -1;
	}

	context_eXosip_lock();
	eXosip_lock(context_eXosip);
	//printf("send_cataloginfo_pair_rsp to=%s from=%s \n", prsp->basic_info.to_uri, prsp->basic_info.from_uri);
	ret = eXosip_message_build_request(context_eXosip, &message, "MESSAGE", prsp->basic_info.to_uri, prsp->basic_info.from_uri, NULL);
	 if(ret != OSIP_SUCCESS)
	 {
	 	eXosip_unlock(context_eXosip);
	 	printf("send_cataloginfo_pair_rsp eXosip_message_build_request failed\n");
		if(message != NULL)
		{
			osip_message_free(message);
		}
		context_eXosip_unlock();
		return -1;
	 }

	 ret = osip_message_set_body(message, body_buffer, strlen(body_buffer));
	 //ret = osip_message_set_body(message, "good morning", strlen("good morning"));
	  if(ret != OSIP_SUCCESS)
	 {
	 	eXosip_unlock(context_eXosip);
	 	printf("send_cataloginfo_pair_rsp osip_message_set_body failed\n");
		if(message != NULL)
		{
			osip_message_free(message);
		}
		context_eXosip_unlock();
		return -1;
	 }
	  
	 ret = osip_message_set_content_type (message, "Application/MANSCDP+xml");
	  if(ret != OSIP_SUCCESS)
	 {
	 	eXosip_unlock(context_eXosip);
	 	printf("send_cataloginfo_pair_rsp osip_message_set_content_type failed\n");
		if(message != NULL)
		{
			osip_message_free(message);
		}
		context_eXosip_unlock();
		return -1;
	 }
	
	#if 1
	  {
	  	char *dest=NULL;
		size_t length=0;
		ret = osip_message_to_str(message, &dest, &length);
		if (ret !=0) 
		{
			printf("%s:%d osip_message_to_str failed\n", __FUNCTION__, __LINE__);
		}
		printf("%s line=%d message:\n%s\n", __FUNCTION__, __LINE__, dest);
		osip_free(dest);
	 }
	  #endif
	 
	
	 ret = eXosip_message_send_request (context_eXosip, message);
	 if(ret != OSIP_SUCCESS)
	 {
		int i = 1;	
	 }
	eXosip_unlock(context_eXosip);
	context_eXosip_unlock();

	return 0;
}

//	发送报警订阅的 notify
int build_and_send_alarm_notify(const sipd_alarm_ind *prsp)
{
	osip_message_t *message = NULL;
	int ret;

	char buffer[MAX_XML_BUFFER_LEN+1]={0};
	int longitude=0;
	int latitiude = 0;
	int len=0;

	//printf("%s:%d from=%s to=%s\n", __FUNCTION__, __LINE__, from, to);

	memset(buffer, 0, sizeof(buffer));
	snprintf(buffer,MAX_XML_BUFFER_LEN,
		"<?xml version=\"1.0\"?>\r\n"\
		"<Notify>\r\n"\
		"<CmdType>Alarm</CmdType>\r\n"\
		"<SN>%d</SN>\r\n"\
		"<DeviceID>%s</DeviceID>\r\n"\
		"<AlarmPriority>%d</AlarmPriority>\r\n"\
		"<AlarmTime>%s</AlarmTime>\r\n"\
		"<AlarmMethod>%d</AlarmMethod>\r\n",
		get_device_sn(), prsp->device_id, prsp->alarm_priority, prsp->alarm_time, sipd_alarm_condition_others);

	if(0)
	{
		len = strlen(buffer);
		//snprintf(buffer+len, (MAX_XML_BUFFER_LEN-len),"<AlarmDescription>%s</AlarmDescription>\r\n", description);
	}

	if(longitude != NULL)
	{
		if(strlen(longitude) > 0)
		{
			len = strlen(buffer);
			snprintf(buffer+len, (MAX_XML_BUFFER_LEN-len),"<Longitude>%s</Longitude>\r\n", longitude);
		}
	}

	if (latitiude != NULL)
	{
		if(strlen(latitiude) > 0)
		{
			len = strlen(buffer);
			snprintf(buffer+len, (MAX_XML_BUFFER_LEN-len),"<Latitude>%s</Latitude>\r\n", latitiude);
		}
	}

	len = strlen(buffer);
	snprintf(buffer+len, (MAX_XML_BUFFER_LEN-len),"</Notify>");

	context_eXosip_lock();
	eXosip_lock(context_eXosip);
	//printf("send_cataloginfo_pair_rsp to=%s from=%s \n", prsp->basic_info.to_uri, prsp->basic_info.from_uri);
	ret = eXosip_insubscription_build_notify(context_eXosip, prsp->tid, EXOSIP_SUBCRSTATE_ACTIVE, 0, &message);
	if(ret != OSIP_SUCCESS)
	{
		eXosip_unlock(context_eXosip);
		printf("build_and_send_alarm_notify eXosip_insubscription_build_notify failed\n");
		if(message != NULL)
		{
			osip_message_free(message);
		}
		context_eXosip_unlock();
		return -1;
	}

	ret = osip_message_set_body(message, buffer, strlen(buffer));
	//ret = osip_message_set_body(message, "good morning", strlen("good morning"));
	if(ret != OSIP_SUCCESS)
	{
		eXosip_unlock(context_eXosip);
		printf("build_and_send_alarm_notify osip_message_set_body failed\n");
		if(message != NULL)
		{
			osip_message_free(message);
		}
		context_eXosip_unlock();
		return -1;
	}

	ret = osip_message_set_content_type (message, "Application/MANSCDP+xml");
	if(ret != OSIP_SUCCESS)
	{
		eXosip_unlock(context_eXosip);
		printf("build_and_send_alarm_notify osip_message_set_content_type failed\n");
		if(message != NULL)
		{
			osip_message_free(message);
		}
		context_eXosip_unlock();
		return -1;
	}

	ret = eXosip_insubscription_send_request (context_eXosip, prsp->tid, message);
	if(ret != OSIP_SUCCESS)
	{
		int i = 1;	
	}
	eXosip_unlock(context_eXosip);
	context_eXosip_unlock();

	return 0;
}

//	发送目录订阅的 notify
static int send_cataloginfo_pair_notify(const sipd_catainfo_adaptor_rsp *prsp, char* body_buffer)
{
	osip_message_t *message = NULL;
	int ret;

	if(NULL == prsp  || NULL == body_buffer)
	{
		return -1;
	}

	context_eXosip_lock();
	eXosip_lock(context_eXosip);
	//printf("send_cataloginfo_pair_rsp to=%s from=%s \n", prsp->basic_info.to_uri, prsp->basic_info.from_uri);
	ret = eXosip_insubscription_build_notify(context_eXosip, prsp->basic_info.tid, EXOSIP_SUBCRSTATE_ACTIVE, 0, &message);
	if(ret != OSIP_SUCCESS)
	{
		eXosip_unlock(context_eXosip);
		printf("send_cataloginfo_pair_notify eXosip_insubscription_build_notify failed\n");
		if(message != NULL)
		{
			osip_message_free(message);
		}
		context_eXosip_unlock();
		return -1;
	}

	ret = osip_message_set_body(message, body_buffer, strlen(body_buffer));
	//ret = osip_message_set_body(message, "good morning", strlen("good morning"));
	if(ret != OSIP_SUCCESS)
	{
		eXosip_unlock(context_eXosip);
		printf("send_cataloginfo_pair_notify osip_message_set_body failed\n");
		if(message != NULL)
		{
			osip_message_free(message);
		}
		context_eXosip_unlock();
		return -1;
	}

	ret = osip_message_set_content_type (message, "Application/MANSCDP+xml");
	if(ret != OSIP_SUCCESS)
	{
		eXosip_unlock(context_eXosip);
		printf("send_cataloginfo_pair_notify osip_message_set_content_type failed\n");
		if(message != NULL)
		{
			osip_message_free(message);
		}
		context_eXosip_unlock();
		return -1;
	}

#if 1
	{
		char *dest=NULL;
		size_t length=0;
		ret = osip_message_to_str(message, &dest, &length);
		if (ret !=0) 
		{
			printf("%s:%d osip_message_to_str failed\n", __FUNCTION__, __LINE__);
		}
		printf("%s line=%d message:\n%s\n", __FUNCTION__, __LINE__, dest);
		osip_free(dest);
	}
#endif


	ret = eXosip_insubscription_send_request (context_eXosip, prsp->basic_info.tid, message);
	if(ret != OSIP_SUCCESS)
	{
		int i = 1;	
	}
	eXosip_unlock(context_eXosip);
	context_eXosip_unlock();

	return 0;
}


//	目录订阅后向上级网关的通知
int sipd_dev_catalog_notify(const sipd_catainfo_adaptor_rsp* prsp)
{
	char buffer[MAX_SIPD_RECORD_INFO_LEN+1] = {0};
	int ret, index;
	struct sipd_recorditem_pair item_pair;
	int len= 0;

	if(NULL == prsp)
	{
		return -1;
	}

	memset(buffer, 0, sizeof(buffer));
	if(0 == prsp->basic_info.SumNum)
		return -2;

	memset(buffer, 0, sizeof(buffer));
	snprintf(buffer,MAX_SIPD_RECORD_INFO_LEN,
		"<?xml version=\"1.0\"?>\r\n"\
		"<Notify>\r\n"\
		"<CmdType>Catalog</CmdType>\r\n"\
		"<SN>%s</SN>\r\n"\
		"<DeviceID>%s</DeviceID>\r\n"\
		"<SumNum>%d</SumNum>\r\n",
		prsp->basic_info.SN, prsp->basic_info.device_id, prsp->basic_info.SumNum);

	len = strlen(buffer);
	snprintf(buffer+len, (MAX_SIPD_RECORD_INFO_LEN-len),"<DeviceList Num=\"%d\">\r\n", prsp->basic_info.DeviceList_Num);

	for(index = 0; index < (prsp->basic_info.DeviceList_Num); index+=2)
	{
		memset(&item_pair, 0, sizeof(item_pair));
		ret = get_cataloginfo_pair(index, &item_pair, prsp, TRUE);
		if(ret != 0)
		{
			printf("sipd_dev_catalog_notify get_cataloginfo_pair error! ret=%d \n", ret);
			break;
		}

		if(item_pair.is_last_one_item)
		{
			len = strlen(buffer);
			snprintf(buffer+len, (MAX_SIPD_RECORD_INFO_LEN-len), item_pair.item_pair_buffer[0]);
		}
		else
		{
			len = strlen(buffer);
			snprintf(buffer+len, (MAX_SIPD_RECORD_INFO_LEN-len), item_pair.item_pair_buffer[0]);

			len = strlen(buffer);
			snprintf(buffer+len, (MAX_SIPD_RECORD_INFO_LEN-len), item_pair.item_pair_buffer[1]);
		}
	}

	len = strlen(buffer);
	snprintf(buffer+len, (MAX_SIPD_RECORD_INFO_LEN-len),"</DeviceList>\r\n</Notify>");
	if(Get_EvWaitFlg() == 1)
	{
		send_cataloginfo_pair_notify(prsp, buffer);
	}
	return 0;

}

int sipd_dev_catalog_rsp(const sipd_catainfo_adaptor_rsp* prsp)
{
	char buffer[MAX_SIPD_RECORD_INFO_LEN+1] = {0};
	int ret, index;
	struct sipd_recorditem_pair item_pair;
	int len= 0;

	if(NULL == prsp)
	{
		return -1;
	}

	memset(buffer, 0, sizeof(buffer));
	
	if(0 == prsp->basic_info.SumNum)
	{
	      memset(buffer, 0, sizeof(buffer));
		snprintf(buffer,MAX_SIPD_RECORD_INFO_LEN,
		"<?xml version=\"1.0\"?>\r\n"\
		"<Response>\r\n"\
		"<CmdType>Catalog</CmdType>\r\n"\
		"<SN>%s</SN>\r\n"\
		"<DeviceID>%s</DeviceID>\r\n"\
		"<SumNum>%d</SumNum>\r\n"\
		"</Response>",
		prsp->basic_info.SN, prsp->basic_info.device_id, prsp->basic_info.SumNum);

		if(Get_EvWaitFlg() == 1)
		{
			send_cataloginfo_pair_rsp(prsp, buffer);
		}
			
	}
	else
	{
		memset(buffer, 0, sizeof(buffer));
		snprintf(buffer,MAX_SIPD_RECORD_INFO_LEN,
			"<?xml version=\"1.0\"?>\r\n"\
			"<Response>\r\n"\
			"<CmdType>Catalog</CmdType>\r\n"\
			"<SN>%s</SN>\r\n"\
			"<DeviceID>%s</DeviceID>\r\n"\
			"<SumNum>%d</SumNum>\r\n",
			prsp->basic_info.SN, prsp->basic_info.device_id, prsp->basic_info.SumNum);

		len = strlen(buffer);
		snprintf(buffer+len, (MAX_SIPD_RECORD_INFO_LEN-len),"<DeviceList Num=\"%d\">\r\n",prsp->basic_info.DeviceList_Num);

		for(index = 0; index < (prsp->basic_info.DeviceList_Num); index+=2)
		{
			memset(&item_pair, 0, sizeof(item_pair));
			ret = get_cataloginfo_pair(index, &item_pair, prsp, FALSE);
			if(ret != 0)
			{
				printf("sipd_dev_catalog_rsp get_cataloginfo_pair error! ret=%d \n", ret);
				break;
			}

			if(item_pair.is_last_one_item)
			{
				
				len = strlen(buffer);
				snprintf(buffer+len, (MAX_SIPD_RECORD_INFO_LEN-len), item_pair.item_pair_buffer[0]);
		
			}
			else
			{
				len = strlen(buffer);
				snprintf(buffer+len, (MAX_SIPD_RECORD_INFO_LEN-len), item_pair.item_pair_buffer[0]);

				len = strlen(buffer);
				snprintf(buffer+len, (MAX_SIPD_RECORD_INFO_LEN-len), item_pair.item_pair_buffer[1]);
			}
		}

		len = strlen(buffer);
		snprintf(buffer+len, (MAX_SIPD_RECORD_INFO_LEN-len),"</DeviceList>\r\n</Response>");

		if(Get_EvWaitFlg() == 1)
		{
			send_cataloginfo_pair_rsp(prsp, buffer);
		}
	}

	return 0;
}



void sipd_dev_info_rsp(const sipd_deviceinfo_rsp* pDeviceInfo)
{
	osip_message_t *message = NULL;
	char tmp[MAX_XML_BUFFER_LEN];
	int ret = 0;
	int len=0;

	if(NULL == pDeviceInfo)
	{
		printf("sipd_dev_info_rsp inout err\n");
		return ;
	}


	memset(tmp,0,sizeof(tmp));

	//printf("sipd_dev_info_rsp:from_uri%s\nto_uri:%s\n", pDeviceInfo->from_uri, pDeviceInfo->to_uri);
		

	if(pDeviceInfo->Result != SIPD_RESULT_OK)
	{
		snprintf(tmp, sizeof(tmp),
			"<?xml version=\"1.0\"?>\r\n"\
			"<Response>\r\n"\
			"<CmdType>DeviceInfo</CmdType>\r\n"\
			"<SN>%s</SN>\r\n"\
			"<DeviceID>%s</DeviceID>\r\n"\
			"<Result>ERROR</Result>\r\n"\
			"</Response>",
			pDeviceInfo->SN, pDeviceInfo->device_id);
	}
	else
	{
		snprintf(tmp, sizeof(tmp),
			"<?xml version=\"1.0\"?>\r\n"\
			"<Response>\r\n"\
			"<CmdType>DeviceInfo</CmdType>\r\n"\
			"<SN>%s</SN>\r\n"\
			"<DeviceID>%s</DeviceID>\r\n"\
			"<Result>OK</Result>\r\n",
			pDeviceInfo->SN, pDeviceInfo->device_id);

		if(strlen(pDeviceInfo->DeviceType) > 0)
		{
			len = strlen(tmp);
			snprintf(tmp+len, (MAX_XML_BUFFER_LEN-len),"<DeviceType>%s</DeviceType>\r\n", pDeviceInfo->DeviceType);
		}

		if(strlen(pDeviceInfo->manufacturer) > 0)
		{
			len = strlen(tmp);
			snprintf(tmp+len, (MAX_XML_BUFFER_LEN-len),"<Manufacturer>%s</Manufacturer>\r\n", pDeviceInfo->manufacturer);
		}

		
		if(strlen(pDeviceInfo->model) > 0)
		{
			len = strlen(tmp);
			snprintf(tmp+len, (MAX_XML_BUFFER_LEN-len),"<Model>%s</Model>\r\n", pDeviceInfo->model);
		}

		if(strlen(pDeviceInfo->firmware) > 0)
		{
			len = strlen(tmp);
			snprintf(tmp+len, (MAX_XML_BUFFER_LEN-len),"<Firmware>%s</Firmware>\r\n", pDeviceInfo->firmware);
		}

		if((pDeviceInfo->MaxCamera)  > 0)
		{
			len = strlen(tmp);
			snprintf(tmp+len, (MAX_XML_BUFFER_LEN-len),"<MaxCamera>%d</MaxCamera>\r\n", pDeviceInfo->MaxCamera);
		}

		if((pDeviceInfo->MaxAlarm)  > 0)
		{
			len = strlen(tmp);
			snprintf(tmp+len, (MAX_XML_BUFFER_LEN-len),"<MaxAlarm>%d</MaxAlarm>\r\n", pDeviceInfo->MaxAlarm);
		}
		
		
		snprintf(tmp+len, (MAX_XML_BUFFER_LEN-len),"%s", "</Response>");
	}

	
	//printf("sipd_dev_info_rsp:%s\n", tmp);

	context_eXosip_lock();
	eXosip_lock(context_eXosip);
	ret = eXosip_message_build_request(context_eXosip, &message, "MESSAGE", pDeviceInfo->to_uri, pDeviceInfo->from_uri, NULL);
	 if(ret != OSIP_SUCCESS)
	 {
	 	eXosip_unlock(context_eXosip);
	 	printf("sipd_dev_info_rsp eXosip_message_build_request failed\n");
		if(message != NULL)
		{
			osip_message_free(message);
		}
		context_eXosip_unlock();
		return ;
	 }

	 
	ret = osip_message_set_content_type(message, MANSCDP_XML);
	 if(ret != OSIP_SUCCESS)
	 {
	 	eXosip_unlock(context_eXosip);
	 	printf("sipd_dev_info_rsp osip_message_set_content_type failed\n");
		if(message != NULL)
		{
			osip_message_free(message);
		}
		context_eXosip_unlock();
		return ;
	 }

	ret = osip_message_set_body (message, tmp, strlen(tmp));
	 if(ret != OSIP_SUCCESS)
	 {
	 	eXosip_unlock(context_eXosip);
	 	printf("sipd_dev_info_rsp  osip_message_set_body failed\n");
		if(message != NULL)
		{
			osip_message_free(message);
		}
		context_eXosip_unlock();
		return ;
	 }
	 
	eXosip_message_send_request (context_eXosip, message);
	eXosip_unlock(context_eXosip);
	context_eXosip_unlock();
	return ;
}


void sipd_dev_stat_rsp(const sipd_stateinfo_adaptor_rsp* pStateinfoRsp)
{
	osip_message_t *message = NULL;
	char tmp[MAX_XML_BUFFER_LEN]={0};
	int almnum = 0;
	int ret= 0;
	int len=0;
	int i=0;

	if(NULL == pStateinfoRsp)
	{
		printf("sipd_dev_stat_rsp invalid input param, ERROR!\n");
		return;
	}
	
	
	//printf("sipd_dev_stat_rsp AlarmDeviceNum=%d \n", pStateinfoRsp->basic_info.AlaramDeviceNum);
	

	memset(tmp, 0, sizeof(tmp));
	if(pStateinfoRsp->basic_info.Result != SIPD_RESULT_OK)
	{
		snprintf(tmp, sizeof(tmp),
			"<?xml version=\"1.0\"?>\r\n"\
			"<Response>\r\n"\
			"<CmdType>DeviceStatus</CmdType>\r\n"\
			"<SN>%s</SN>\r\n"\
			"<DeviceID>%s</DeviceID>\r\n"\
			"<Result>ERROR</Result>\r\n"\
			"</Response>",
			pStateinfoRsp->basic_info.SN, pStateinfoRsp->basic_info.device_id, (pStateinfoRsp->basic_info.Encode== SIPD_STATUS_ON)?"ON":"OFF", (pStateinfoRsp->basic_info.Record== SIPD_STATUS_OFF)?"OFF":"ON");  
		
	}
	else
	{
		snprintf(tmp, sizeof(tmp),
			"<?xml version=\"1.0\"?>\r\n"\
			"<Response>\r\n"\
			"<CmdType>DeviceStatus</CmdType>\r\n"\
			"<SN>%s</SN>\r\n"\
			"<DeviceID>%s</DeviceID>\r\n"\
			"<Result>OK</Result>\r\n",
			pStateinfoRsp->basic_info.SN, pStateinfoRsp->basic_info.device_id); 

		if((pStateinfoRsp->basic_info.Online== SIPD_STATUS_ONLINE_TYPE_ONLINE))
		{
			len = strlen(tmp);
			snprintf(tmp+len, (MAX_XML_BUFFER_LEN-len),"<Online>%s</Online>\r\n", "ONLINE");
		}
		else
		{
			len = strlen(tmp);
			snprintf(tmp+len, (MAX_XML_BUFFER_LEN-len),"<Online>%s</Online>\r\n", "OFFLINE");
		}

		if(pStateinfoRsp->basic_info.Status == SIPD_RESULT_OK)
		{
			len = strlen(tmp);
			snprintf(tmp+len, (MAX_XML_BUFFER_LEN-len),"<Status>%s</Status>\r\n", "OK");
		}
		else
		{
			len = strlen(tmp);
			snprintf(tmp+len, (MAX_XML_BUFFER_LEN-len),"<Status>%s</Status>\r\n", "ERROR");

			len = strlen(tmp);
			snprintf(tmp+len, (MAX_XML_BUFFER_LEN-len),"<Reason>%s</Reason>\r\n", pStateinfoRsp->basic_info.Reason);
		}

		
		if(pStateinfoRsp->basic_info.Encode == SIPD_STATUS_ON)
		{
			len = strlen(tmp);
			snprintf(tmp+len, (MAX_XML_BUFFER_LEN-len),"<Encode>%s</Encode>\r\n", "ON");
		}
		else
		{
			len = strlen(tmp);
			snprintf(tmp+len, (MAX_XML_BUFFER_LEN-len),"<Encode>%s</Encode>\r\n", "OFF");
		}

		if(pStateinfoRsp->basic_info.Record== SIPD_STATUS_ON)
		{
			len = strlen(tmp);
			snprintf(tmp+len, (MAX_XML_BUFFER_LEN-len),"<Record>%s</Record>\r\n", "ON");
		}
		else
		{
			len = strlen(tmp);
			snprintf(tmp+len, (MAX_XML_BUFFER_LEN-len),"<Record>%s</Record>\r\n", "OFF");
		}

		len = strlen(tmp);
		snprintf(tmp+len, (MAX_XML_BUFFER_LEN-len),"<DeviceTime>%s</DeviceTime>\r\n", pStateinfoRsp->basic_info.DeviceTime);

		almnum = pStateinfoRsp->basic_info.AlaramDeviceNum;
		if(almnum > 0)
		{
			len = strlen(tmp);
			snprintf(tmp+len, (MAX_XML_BUFFER_LEN-len),"<Alarmstatus Num=\"%d\">\r\n", pStateinfoRsp->basic_info.AlaramDeviceNum);

			for(i=0; i<almnum; i++)
			{
				len = strlen(tmp);
				snprintf(tmp+len, (MAX_XML_BUFFER_LEN-len),"<Item>\r\n");

				len = strlen(tmp);
				snprintf(tmp+len, (MAX_XML_BUFFER_LEN-len),"<DeviceID>%s</DeviceID>\r\n", pStateinfoRsp->state_items[i].device_id);

				if(SIPD_ALARM_STATUS_ONDUTY == pStateinfoRsp->state_items[i].Status)
				{
					len = strlen(tmp);
					snprintf(tmp+len, (MAX_XML_BUFFER_LEN-len),"<DutyStatus>%s</DutyStatus>\r\n", "ONDUTY");
				}
				else if(SIPD_ALARM_STATUS_OFFDUTY == pStateinfoRsp->state_items[i].Status)
				{
					len = strlen(tmp);
					snprintf(tmp+len, (MAX_XML_BUFFER_LEN-len),"<DutyStatus>%s</DutyStatus>\r\n", "OFFDUTY");
				}
				else if(SIPD_ALARM_STATUS_ALARM== pStateinfoRsp->state_items[i].Status)
				{
					len = strlen(tmp);
					snprintf(tmp+len, (MAX_XML_BUFFER_LEN-len),"<DutyStatus>%s</DutyStatus>\r\n", "ALARM");
				}
				else
				{
					len = strlen(tmp);
					snprintf(tmp+len, (MAX_XML_BUFFER_LEN-len),"<DutyStatus>%s</DutyStatus>\r\n", "ONDUTY");
				}

			}
		}
		
		len = strlen(tmp);
		snprintf(tmp+len, (MAX_XML_BUFFER_LEN-len),"</Response>");
	}
	

	
	context_eXosip_lock();
	eXosip_lock(context_eXosip);
	ret = eXosip_message_build_request(context_eXosip, &message, "MESSAGE", pStateinfoRsp->basic_info.to_uri, pStateinfoRsp->basic_info.from_uri, NULL);
	 if(ret != OSIP_SUCCESS)
	 {
	 	eXosip_unlock(context_eXosip);
	 	printf("sipd_dev_stat_rsp eXosip_message_build_request failed\n");
		if(message != NULL)
		{
			osip_message_free(message);
		}
		context_eXosip_unlock();
		return ;
	 }
	 
	ret = osip_message_set_content_type(message, MANSCDP_XML);
	if(ret != OSIP_SUCCESS)
	 {
	 	eXosip_unlock(context_eXosip);
	 	printf("sipd_dev_stat_rsp osip_message_set_content_type failed\n");
		if(message != NULL)
		{
			osip_message_free(message);
		}
		context_eXosip_unlock();
		return ;
	 }
	
	ret = osip_message_set_body (message, tmp, strlen(tmp));
	if(ret != OSIP_SUCCESS)
	 {
	 	eXosip_unlock(context_eXosip);
	 	printf("sipd_dev_stat_rsp osip_message_set_body failed\n");
		if(message != NULL)
		{
			osip_message_free(message);
		}
		context_eXosip_unlock();
		return ;
	 }
	
	eXosip_message_send_request (context_eXosip, message);
	eXosip_unlock(context_eXosip);
	context_eXosip_unlock();
}


void sipd_manscdp_xml_deal(const osip_message_t * sip_request, char *body)
{
	mxml_node_t *tree, *que_node, *CmdType_node, *sn_node, * response_node, * control_node;
	char strbuf[MAX_SIPD_URI];
	mxml_node_t *node;
	int whitespace;
	SIPD_GATEWAY_CFG sipd_cfg;

	memset(strbuf, 0, sizeof(strbuf));

	memset(&sipd_cfg, 0, sizeof(sipd_cfg));
	get_PRM_SIPD_CFG(&sipd_cfg);
	

	if(body != NULL)
	{
		tree = mxmlLoadString(NULL, body, MXML_TEXT_CALLBACK);

		if(tree != NULL)
		{
			sipd_message_query_or_response  rsp;

			que_node = mxmlFindElement(tree, tree, QUERY, NULL, NULL,MXML_DESCEND);
			response_node = mxmlFindElement(tree, tree, RESPONSE, NULL, NULL,MXML_DESCEND);
			control_node = mxmlFindElement(tree, tree, "Control", NULL, NULL,MXML_DESCEND);

			memset(&rsp, 0, sizeof(sipd_message_query_or_response));
			strcpy(rsp.to_username, sip_request->to->url->username);
			strcpy(rsp.to_host, sip_request->to->url->host);

			if(sip_request->to->url->port != NULL)
			{
				snprintf(rsp.to_uri, sizeof(rsp.to_uri), "sip:%s@%s:%s", sip_request->to->url->username,
					sip_request->to->url->host, sip_request->to->url->port);
			}
			else
			{
				snprintf(rsp.to_uri, sizeof(rsp.to_uri), "sip:%s@%s", sip_request->to->url->username,
					sip_request->to->url->host);
			}

			if(sip_request->from->url->port != NULL)
			{
				snprintf(rsp.from_uri, sizeof(rsp.from_uri), "sip:%s@%s:%s", sip_request->from->url->username,
					sip_request->from->url->host, sip_request->from->url->port);
			}
			else
			{
				snprintf(rsp.from_uri, sizeof(rsp.from_uri), "sip:%s@%s", sip_request->from->url->username,
					sip_request->from->url->host);
			}

			strcpy(rsp.body, body);

			if(que_node != NULL || control_node!=NULL)
			{

				if(pSipd_message_query_req)
					pSipd_message_query_req(&rsp);					
				
			}
			else if(response_node != NULL)
			{
				//	录像文件查询、设备目录查询、设备状态查询等命令的执行结果的回复
				

				if(pSipd_message_response_req)
					pSipd_message_response_req(&rsp);
			}
			else
			{
				printf("sipd_manscdp_xml_deal que_node is NULL\n");
			}

			mxmlDelete(tree);
		}
		else
		{
			printf("sipd_manscdp_xml_deal tree is NULL\n");
		}
	}
	else
	{
		printf("sipd_manscdp_xml_deal body is NULL\n");
	}
}


void sipd_dev_stat_notify()
{
	osip_message_t *message = NULL;
	char from_uri[100];
	char to_uri[100];
	char tmp[1000];
	int ret = 0;
	SIPD_GATEWAY_CFG sip_cfg;

	memset(from_uri, 0, sizeof(from_uri));
	memset(to_uri, 0, sizeof(to_uri));
	memset(tmp, 0, sizeof(tmp));

	memset(&sip_cfg, 0, sizeof(sip_cfg));
	if(pSipd_GetSIPServerCfg)
		pSipd_GetSIPServerCfg(&sip_cfg);

	printf("%s line=%d \n", __FUNCTION__, __LINE__);
	
	snprintf(from_uri, sizeof(from_uri), "sip:%s@%s", sip_cfg.local_domain_id, sip_cfg.local_domain_name);
	snprintf(to_uri, sizeof(to_uri), "sip:%s@%s:%d", sip_cfg.upper_server_id, sip_cfg.upper_server_ip, sip_cfg.upper_server_port);
	
	snprintf(tmp, sizeof(tmp), 
			"<?xml version=\"1.0\"?>\r\n"\
			"<Notify>\r\n"\
			"<CmdType>Keepalive</CmdType>\r\n"\
			"<SN>%u</SN>\r\n"\
			"<DeviceID>%s</DeviceID>\r\n"\
			"<Status>OK</Status>\r\n"\
			"</Notify>",
			get_device_sn(),
			sip_cfg.local_domain_id//g_DeviceID
	);

	context_eXosip_lock();
	eXosip_lock(context_eXosip);
	ret = eXosip_message_build_request(context_eXosip, &message, "MESSAGE", to_uri, from_uri, NULL);
	if(ret != OSIP_SUCCESS)
	{
		eXosip_unlock(context_eXosip);
		printf("%s line=%d eXosip_message_build_request failed! ret=%d\n", __FUNCTION__, __LINE__, ret);
		if(message != NULL)
		{
			osip_message_free(message);
		}
		context_eXosip_unlock();

		return;
	}
	ret = osip_message_set_content_type(message, MANSCDP_XML);
	if(ret != OSIP_SUCCESS)
	{
		eXosip_unlock(context_eXosip);
		printf("%s line=%d osip_message_set_content_type failed! ret=%d\n", __FUNCTION__, __LINE__, ret);
		if(message != NULL)
		{
			osip_message_free(message);
		}
		context_eXosip_unlock();

		return;
	}
	
	ret = osip_message_set_body (message, tmp, strlen(tmp));
	if(ret != OSIP_SUCCESS)
	{
		eXosip_unlock(context_eXosip);
		printf("%s line=%d osip_message_set_body failed! ret=%d\n", __FUNCTION__, __LINE__, ret);
		if(message != NULL)
		{
			osip_message_free(message);
		}
		context_eXosip_unlock();

		return;
	}
	
	eXosip_message_send_request (context_eXosip, message);
	eXosip_unlock(context_eXosip);
	context_eXosip_unlock();
	return;
}



void sipd_send_timing_req(eXosip_event_t *je)
{
	Sipd_timing_req tm_req;
	osip_header_t *date_header = NULL; 
	sipd_id_info id_info;
	
	memset(&tm_req, 0, sizeof(tm_req));

	osip_message_get_date(je->response, 0, &date_header);

	if(date_header == NULL)
	{
		printf("date_header is NULL\n");
		return;
	}

	GMTStrToTime_t(date_header->hvalue, &(tm_req.tm));

	memset(&id_info, 0, sizeof(id_info));

	if(pSipd_settime)
	{
		if(strlen(tm_req.device_id) > 0)
		{
			strncpy(id_info.device_id, tm_req.device_id, MAX_SIPD_DEVICE_ID_LEN);

			pSipd_settime(&id_info, &(tm_req.tm));
		}
		else
		{
			pSipd_settime(NULL, &(tm_req.tm));
		}
	}
}


void sipd_reg_failure_deal(eXosip_event_t *je)
{
	int ret;

	if(je == NULL)
		return;

	if(je->response != NULL)
	{
		printf("%s line=%d status_code=%d \n", __FUNCTION__, __LINE__, je->response->status_code);
		if(je->response->status_code == SIP_UNAUTHORIZED)
		{

			return;

		}
		else if(je->response->status_code == SIP_PROXY_AUTHENTICATION_REQUIRED)
		{		
			printf("eXosip_automatic_action\n");
#if 0
			context_eXosip_lock();
			eXosip_lock(context_eXosip);         
			eXosip_automatic_action(context_eXosip); 
			eXosip_unlock(context_eXosip); 
			context_eXosip_unlock();
#endif
		
			return;
		}
		else if(je->response->status_code == SIP_NOT_FOUND)
		{
			printf("usr not exist\n");	
		}
		else if(je->response->status_code == SIP_406_NOT_ACCEPTABLE)
		{
			printf("password error\n");
		}
		else if(je->response->status_code == SIP_REQUEST_TIME_OUT)
		{
			printf("usr has been existed\n");
		}
	}
	else
	{
		printf("%s line=%d sipd_reg_failure_deal je->response is NULL\n", __FUNCTION__, __LINE__);
	}

	context_eXosip_lock();
	eXosip_lock (context_eXosip);
	ret = eXosip_register_remove(context_eXosip, je->rid);
	if(ret != OSIP_SUCCESS)
	{
		printf("%s line=%d eXosip_register_remove failed! ret=%d\n", __FUNCTION__, __LINE__, ret);
	}

	Set_Dev_Online(-1);		//	本域注册失败
	
	eXosip_unlock (context_eXosip);
	context_eXosip_unlock();
}


int init_sip_protocol_stack(void)
{
	int result;
	SIPD_GATEWAY_CFG sip_cfg;

	if(NULL == context_eXosip)
	{
		context_eXosip = eXosip_malloc();
		if (context_eXosip==NULL)
		{
			printf("%s line=%d eXosip_malloc err\n", __FUNCTION__, __LINE__);
		  	return -1;
		}
	}

	memset(&sip_cfg, 0, sizeof(sip_cfg));
	get_domain_server_cfg(&sip_cfg);
	
	result=eXosip_init(context_eXosip);
	if (result !=OSIP_SUCCESS)
	{
		printf("%s line=%d eXosip_init err\n", __FUNCTION__, __LINE__);
		osip_free(context_eXosip);
	  	return -1;
	}
	
	printf("%s line=%d local_gateway=%s \n", __FUNCTION__, __LINE__, sip_cfg.local_gateway);
	result = eXosip_set_option (context_eXosip, EXOSIP_OPT_SET_IPV4_FOR_GATEWAY, sip_cfg.local_gateway);
	if (result !=OSIP_SUCCESS)
	{
		printf("%s line=%d eXosip_set_option err\n", __FUNCTION__, __LINE__);
		osip_free(context_eXosip);
	  	return -1;
	}

	
	result = eXosip_listen_addr (context_eXosip, IPPROTO_UDP, NULL, sip_cfg.local_port, AF_INET, 0);
	if(result != OSIP_SUCCESS)
	{
		printf("%s line=%d eXosip_listen_addr err\n", __FUNCTION__, __LINE__);
		osip_free(context_eXosip);
	  	return -1;
	}
	
	printf("init_sip_protocol_stack Successful! local_port=%d \n", sip_cfg.local_port);
	return 0;
}

#if defined(WIN32)
DWORD WINAPI sipd_protocolevent(void *data)
#else
void *sipd_protocolevent(void* data)
#endif
{
	eXosip_event_t *je = NULL;
    osip_message_t *answer = NULL;
	sdp_message_t *remote_sdp = NULL;
	int ret=0;
	osip_header_t *expires_header = NULL;
	int expires = 0;
  
	for (;;)  
	{  
		if(Get_EvWaitFlg() == 1)
		{
			je = eXosip_event_wait (context_eXosip, 0, 50);  
		
		    if (je == NULL)  
			{
				if(pSipd_on_idle)
					pSipd_on_idle();
		    	continue;  
			}
#if 1
			context_eXosip_lock();
		 	eXosip_lock(context_eXosip);  
	      	eXosip_automatic_action (context_eXosip);  
	      	eXosip_unlock(context_eXosip);
		  	context_eXosip_unlock();
#endif

		    //printf("%s line=%d get event type=%d  EXOSIP_MESSAGE_NEW=%d\n", __FUNCTION__, __LINE__, je->type, EXOSIP_MESSAGE_NEW);
		    switch (je->type)  
		    {  
		    	case EXOSIP_REGISTRATION_SUCCESS: 
				osip_message_get_expires(je->request, 0, &expires_header);
				expires = osip_atoi(expires_header->hvalue);

				printf("EXOSIP_REGISTRATION_SUCCESS expires:%d\n", expires);

				if(expires>0)  
				{
					Set_Dev_Online(1);	//	本域注册成功
					Set_Reg_Id(je->rid);
					sipd_send_timing_req(je);	//	按照上级域时间对本域进行校时

					if(Get_NotifyFlg() == 1)
					{
						sipd_dev_stat_notify();
						Set_NotifyFlg(0);
					}
				}
				else  
				{
					printf("Dev Log out\n");
					if(Get_Reg_Id()> 0)
					{
						context_eXosip_lock();
						eXosip_lock (context_eXosip);
						
						ret = eXosip_register_remove(context_eXosip, Get_Reg_Id());
						if(ret != OSIP_SUCCESS)
						{
							if(ret != OSIP_NOTFOUND)
							{
								printf("%s line=%d eXosip_register_remove failed! ret=%d\n", __FUNCTION__, __LINE__, ret);
							}
						}

						Set_Reg_Id(0);
						eXosip_unlock (context_eXosip);
						context_eXosip_unlock();
					}
					Set_LogOutFlg(1);
				}
	            break;  
		  
	          	case EXOSIP_REGISTRATION_FAILURE:  
				  	printf("textinfo is %s\n", je->textinfo); 
					sipd_reg_failure_deal(je);
					
					//Set_Reg_Id(0);
	            	break;  
		  
		        case EXOSIP_MESSAGE_NEW:
			    printf("%s line=%d received new message sip_method=%s\n", __FUNCTION__, __LINE__, je->request->sip_method);
			    if (MSG_IS_MESSAGE (je->request))
			    {
			    	osip_content_type_t* content_type=NULL;
				   	int is_message_legal = 0;

				   	//context_eXosip_lock();
			      	eXosip_lock (context_eXosip);
				   	content_type = osip_message_get_content_type(je->request);
			        if(NULL == content_type)
				   	{
						printf("%s line=%d Message content_type is NULL! return 400 \n", __FUNCTION__, __LINE__);
						eXosip_call_send_answer (context_eXosip, je->tid, 400, NULL);
				   	}
				   	else
				   	{
				   		is_message_legal = 0/*check_deviceID_MESSAGE(je->request)*/;
					   	if(0 == is_message_legal)
					   	{
					    	ret = eXosip_message_build_answer (context_eXosip,je->tid, 200,&answer);
						   	if (ret !=OSIP_SUCCESS)
						   	{
						   		printf("%s:%d eXosip_message_build_answer [EXOSIP_MESSAGE_NEW Message] failed\n", __FUNCTION__, __LINE__);
						   	}
						 
					        ret = eXosip_message_send_answer (context_eXosip,je->tid, 200,answer);
						   	if (ret !=OSIP_SUCCESS)
						   	{
						   		printf("%s:%d eXosip_message_send_answer [EXOSIP_MESSAGE_NEW Message] failed\n", __FUNCTION__, __LINE__);
						   	}
					   	}
					   	else
					   	{
					   		printf("%s line=%d check_deviceID_MESSAGE ERROR! \n", __FUNCTION__, __LINE__);
							eXosip_call_send_answer (context_eXosip, je->tid, 400, NULL);
					   	}
				   	}

					eXosip_unlock (context_eXosip);
					//context_eXosip_unlock();

				   	if(0 == is_message_legal)
				   	{
				   		handle_method_MESSAGE(je->request);
				   	}
			    }

				if(MSG_IS_OPTIONS(je->request))
				{
				  	context_eXosip_lock();
				  	eXosip_lock (context_eXosip);
					eXosip_message_build_answer (context_eXosip, je->tid, 200,&answer);
			        eXosip_message_send_answer (context_eXosip, je->tid, 200,answer);
					eXosip_unlock (context_eXosip);
					context_eXosip_unlock();
				}

				if (MSG_IS_INFO(je->request))
			    {
			       	osip_content_type_t* content_type=NULL;
					
			        printf("%s line=%d is INFO message\n", __FUNCTION__, __LINE__);
					context_eXosip_lock();
					eXosip_lock (context_eXosip);
				    content_type = osip_message_get_content_type(je->request);
				    if(NULL == content_type)
					{
						printf("%s line=%d Message content_type is NULL! return 400 \n", __FUNCTION__, __LINE__);
						eXosip_message_send_answer (context_eXosip, je->tid, 400, NULL);
					}
					else
					{
					    if((strcasecmp(content_type->type, CONTENT_TYPE_APP) == 0) && (strcasecmp(content_type->subtype, CONTENT_TYPE_SUB_MANSRTSP) == 0))
					    {
					    	ret = eXosip_message_build_answer (context_eXosip,je->tid, 200, &answer);
					        if (ret != OSIP_SUCCESS)
					        {
					        	printf("%s line=%d [EXOSIP_CALL_MESSAGE_NEW] info.eXosip_call_build_answer  build 200 OK ERROR!\n", __FUNCTION__, __LINE__);
	          					eXosip_message_send_answer (context_eXosip, je->tid, 400, NULL);
					        }
						    else
						    {
								eXosip_message_send_answer (context_eXosip,je->tid, 200, answer);
						    }
					    }
					    else if((strcasecmp(content_type->type, CONTENT_TYPE_APP) == 0) && (strcasecmp(content_type->subtype, CONTENT_TYPE_SUB_RTSP) == 0))
					    {
					    	ret = eXosip_message_build_answer (context_eXosip,je->tid, 200, &answer);
					        if (ret != OSIP_SUCCESS)
					        {
					        	printf("%s line=%d [EXOSIP_CALL_MESSAGE_NEW] info.eXosip_call_build_answer  build 200 OK ERROR!\n", __FUNCTION__, __LINE__);
	          					eXosip_message_send_answer (context_eXosip, je->tid, 400, NULL);
					        }
						    else
						    {
								eXosip_message_send_answer (context_eXosip,je->tid, 200, answer);
						    }
					    }
					    else
					    {
					    	eXosip_message_send_answer (context_eXosip, je->tid, 400, NULL);
					    }
					}

					eXosip_unlock (context_eXosip);
					context_eXosip_unlock();

				    if(content_type != NULL)
				    {
					    if((strcasecmp(content_type->type, CONTENT_TYPE_APP) == 0) && (strcasecmp(content_type->subtype, CONTENT_TYPE_SUB_MANSRTSP) == 0))
					    {
				        	handle_method_INFO(je->request, je->tid, je->did);
					    }
					    else if((strcasecmp(content_type->type, CONTENT_TYPE_APP) == 0) && (strcasecmp(content_type->subtype, CONTENT_TYPE_SUB_RTSP) == 0))
					    {
				        	handle_method_INFO(je->request, je->tid, je->did);
					    }
				     }

			      }

				  if(MSG_IS_SUBSCRIBE(je->request))
				  {
				  	printf("%s line=%d get subscribe message! \n", __FUNCTION__, __LINE__);
					printf("%s line=%d <- (nid(%d) did(%d)> SUBSCRIBE in EXOSIP_MESSAGE_NEW\n", __FUNCTION__, __LINE__, je->nid, je->did);
					/*
					{
							  	char *dest=NULL;
								size_t length=0;
								int ret;
								
								ret = osip_message_to_str(je->request, &dest, &length);
								if (ret !=OSIP_SUCCESS) 
								{
									printf("%s:%d osip_message_to_str failed\n", __FUNCTION__, __LINE__);
								}
								printf("%s line=%d check subscription where content_type is NULL, the message is:\n%s\n", __FUNCTION__, __LINE__, dest);
								osip_free(dest);
					 }*/
				  }

				  if(MSG_IS_REGISTER(je->request))
				  {
					  osip_authorization_t *auth;
					  char buff[MAX_SIPD_BODY_LENGTH];
					  
					  memset(buff, 0, sizeof(buff));
					 
					eXosip_lock (context_eXosip);
					  
					ret = osip_message_get_authorization(je->request, 0, &auth);					
					if(ret!=OSIP_SUCCESS)
					{
					  ret = eXosip_message_build_answer (context_eXosip,je->tid, 401,&answer);
					
					  snprintf(buff,MAX_SIPD_BODY_LENGTH,\
						  "Digest realm=\"43000000\",nonce=\"181b4c350c00f3c2\"");
					  osip_message_set_www_authenticate(answer, buff);
					 
					  eXosip_message_send_answer (context_eXosip,je->tid, 401, answer);
					}
					else
					{
						osip_contact_t * contact;
						sipd_register_req  RegisterReq;
						memset(&RegisterReq, 0, sizeof(sipd_register_req));

						if(je->request->from->url->port != NULL)
						{
							snprintf(RegisterReq.from_uri, sizeof(RegisterReq.from_uri), "sip:%s@%s:%s", je->request->from->url->username,
								je->request->from->url->host, je->request->from->url->port);
						}
						else
						{
							snprintf(RegisterReq.from_uri, sizeof(RegisterReq.from_uri), "sip:%s@%s", je->request->from->url->username,
								je->request->from->url->host);
						}

						if(je->request->to->url->port != NULL)
						{
							snprintf(RegisterReq.to_uri, sizeof(RegisterReq.to_uri), "sip:%s@%s:%s", je->request->to->url->username,
								je->request->to->url->host, je->request->to->url->port);
						}
						else
						{
							snprintf(RegisterReq.to_uri, sizeof(RegisterReq.to_uri), "sip:%s@%s", je->request->to->url->username,
								je->request->to->url->host);
						}

						
						ret = osip_message_get_contact(je->request, 0, &contact);
						if(ret!=OSIP_SUCCESS)
						{

						}
						else
						{
							if(contact->url->port != NULL)
							{
								snprintf(RegisterReq.contact_uri, sizeof(RegisterReq.contact_uri), "sip:%s@%s:%s", contact->url->username,
									contact->url->host, contact->url->port);
							}
							else
							{
								snprintf(RegisterReq.contact_uri, sizeof(RegisterReq.contact_uri), "sip:%s@%s", contact->url->username,
									contact->url->host);
							}
						}

						memcpy(&RegisterReq.username, auth->username, strlen(auth->username));
						memcpy(&RegisterReq.realm, auth->realm, strlen(auth->realm));
						memcpy(&RegisterReq.uri, auth->uri, strlen(auth->uri));
						memcpy(&RegisterReq.nonce, auth->nonce, strlen(auth->nonce));
						memcpy(&RegisterReq.response, auth->response, strlen(auth->response));
						memcpy(&RegisterReq.algorithm, auth->algorithm, strlen(auth->algorithm));
						RegisterReq.tid = je->tid;

						if(pSipd_register_req)
							pSipd_register_req(&RegisterReq);

						eXosip_message_build_answer (context_eXosip,je->tid, 200,&answer);
						eXosip_message_send_answer (context_eXosip,je->tid, 200, answer);
					}

					eXosip_unlock (context_eXosip);
				  }
			      break;

				  case EXOSIP_MESSAGE_REQUESTFAILURE:  //请求失败
					{
						if(je->response != NULL)
						{
							printf("je->response->status_code:%d\n", je->response->status_code);

							if(je->response->status_code == SIP_NOT_FOUND)
							{
								printf("Not Found the user you wanted\n");
							}
						}
						else
						{
							printf("EXOSIP_MESSAGE_REQUESTFAILURE je->response is NULL\n");
						}
					}
					break;

				case EXOSIP_MESSAGE_SERVERFAILURE:   
					{
						if(je->response != NULL)
						{
							if(je->response->status_code == SIP_INTERNAL_SERVER_ERROR)
							{
								printf("SIP_INTERNAL_SERVER_ERROR\n");
							}
						}
						else
						{
							printf("EXOSIP_MESSAGE_SERVERFAILURE je->response is NULL\n");
						}
					}
					break;

				
				case EXOSIP_CALL_INVITE:
				case EXOSIP_CALL_REINVITE:
				{
					/**< announce a new call                   */
					char local_sdp[MAX_SIPD_BODY_LENGTH];
					osip_body_t* body = NULL;
					struct sipd_invite_media_session  invite_media_session;


					/*1.回复100
					    2.构造sipd_media_session，然后调用Invite_play_playback_download
					    3.在MSG_ID_GW_MEDIA_RSP消息里面,回复带sdp的200 ok
					    4.收到ack后, 发送消息struct sipd_invite_ctrl_ind(调用Sipd_media_control_notify_CMS发送start)
					    5.close后,发送消息struct sipd_invite_ctrl_ind(调用Sipd_media_control_notify_CMS发送stop)*/
					
					//printf("sipd_protocolevent Received a INVITE msg from %s:%s, UserName is %s, password is %s\n",  je->request->req_uri->host,je->request->req_uri->port, je->request->req_uri->username, je->request->req_uri->password);
					memset(&invite_media_session, 0, sizeof(invite_media_session));

					context_eXosip_lock();
					eXosip_lock (context_eXosip);
					
					remote_sdp = eXosip_get_remote_sdp (context_eXosip, je->did);
					if(remote_sdp != NULL)
					{
						sipd_media_session invite_adaptor;

						get_media_session(remote_sdp, &invite_media_session);
						invite_media_session.cid = je->cid;
						invite_media_session.tid = je->tid;
						invite_media_session.did = je->did;

						ret = osip_message_get_body(je->request, 0, &body);
						if (ret !=OSIP_SUCCESS) 
						{
							printf("sipd_protocolevent  EXOSIP_CALL_INVITE osip_message_get_body failed\n");
						 }
						//printf("%s:%d get body=%s \n", __FUNCTION__, __LINE__, body->body);
						//	解析 sdp 中的 “y” 域
						get_media_session_priv(body->body, &invite_media_session);
						
						if(SIPD_STREAM_TYPE_PLAY == invite_media_session.stream_type)
						{
							strncpy(invite_media_session.u_field, je->request->to->url->username, MAX_SIPD_URI);
						}
							  
						
						eXosip_call_send_answer (context_eXosip, je->tid, 180, NULL);

						/*critical*/
						sdp_message_free (remote_sdp);
	    					remote_sdp = NULL;

						Invite_adaptor_ind(&invite_adaptor, &invite_media_session);

						if(pInvite_play_playback_download)
							pInvite_play_playback_download(&invite_adaptor);

					}
					else
					{
						//	不带 SDP 的 Invite
						sipd_media_session invite_adaptor;
						memset(&invite_adaptor, 0, sizeof(sipd_media_session));
						
						invite_adaptor.cid = je->cid;
						invite_adaptor.did = je->did;
						invite_adaptor.tid = je->tid;
						invite_adaptor.stream_type = SIPD_STREAM_TYPE_NO_SDP;

						if(pInvite_play_playback_download)
							pInvite_play_playback_download(&invite_adaptor);
						/*printf("sipd_protocolevent  Invite eXosip_get_remote_sdp get fail ! ERROR !\n");
						eXosip_call_send_answer (context_eXosip, je->tid, 400, NULL);*/
					}

					eXosip_unlock (context_eXosip);
					context_eXosip_unlock();
			    	}
				break;

			#if 0
				case EXOSIP_CALL_REINVITE:
				{
					/**< announce a new INVITE within call     */
					printf("%s:%d We don't want to do anything about invite in invite [EXOSIP_CALL_REINVITE]\n", __FUNCTION__, __LINE__);	
					context_eXosip_lock();
					eXosip_lock (context_eXosip);
					eXosip_call_send_answer (context_eXosip, je->tid, 488, NULL);
					eXosip_unlock (context_eXosip);
					context_eXosip_unlock();
			    	}
				break;
			#endif

				case EXOSIP_CALL_ANSWERED:		//	 200 OK
					{
						context_eXosip_lock();
						eXosip_lock (context_eXosip);

						remote_sdp = eXosip_get_remote_sdp (context_eXosip, je->did);
						if(remote_sdp != NULL)
						{
							struct sipd_invite_media_session  invite_media_session;
							sipd_media_session invite_adaptor;
							osip_body_t* body = NULL;
							memset(&invite_media_session, 0, sizeof(invite_media_session));

							get_media_session(remote_sdp, &invite_media_session);
							invite_media_session.cid = je->cid;
							invite_media_session.tid = je->tid;
							invite_media_session.did = je->did;

							ret = osip_message_get_body(je->response, 0, &body);
							if (ret ==OSIP_SUCCESS) 
							{
								//	解析 sdp 中的 “y” 域
								get_media_session_priv(body->body, &invite_media_session);
							}

							

							sdp_message_free (remote_sdp);
							remote_sdp = NULL;

							Invite_adaptor_ind(&invite_adaptor, &invite_media_session);

							if(pInvite_200_ok)
								pInvite_200_ok(&invite_adaptor);
						}
						else
						{
							sipd_media_session invite_adaptor;
							memset(&invite_adaptor, 0, sizeof(sipd_media_session));

							invite_adaptor.cid = je->cid;
							invite_adaptor.did = je->did;
							invite_adaptor.tid = je->tid;

							if(pInvite_200_ok)
								pInvite_200_ok(&invite_adaptor);
						}

						eXosip_unlock (context_eXosip);
						context_eXosip_unlock();
					}
					break;

				case EXOSIP_CALL_NOANSWER:
					{
						sipd_media_session invite_adaptor;
						memset(&invite_adaptor, 0, sizeof(sipd_media_session));

						invite_adaptor.cid = je->cid;
						invite_adaptor.did = je->did;
						invite_adaptor.tid = je->tid;

						if(pInvite_no_answer)
							pInvite_no_answer(&invite_adaptor);
					}
					break;

				case EXOSIP_CALL_ACK:
				{
					struct sipd_invite_ctrl_ind invite_ctrl_req;
					sipd_media_session invite_adaptor;

					 /**< ACK received for 200ok to INVITE      */
					printf("sipd_protocolevent ACK recieved! Start Transmit RTP media data. did=%d tid=%d\n",  je->did, je->tid);

					context_eXosip_lock();
					eXosip_lock (context_eXosip);
					
					memset(&invite_adaptor, 0, sizeof(sipd_media_session));

					invite_adaptor.cid = je->cid;
					invite_adaptor.did = je->did;
					invite_adaptor.tid = je->tid;

					remote_sdp = eXosip_get_sdp_info(je->ack);
					if(remote_sdp != NULL)
					{
						struct sipd_invite_media_session  invite_media_session;
						osip_body_t* body = NULL;
						memset(&invite_media_session, 0, sizeof(invite_media_session));

						get_media_session(remote_sdp, &invite_media_session);
						invite_media_session.cid = je->cid;
						invite_media_session.tid = je->tid;
						invite_media_session.did = je->did;

						ret = osip_message_get_body(je->ack, 0, &body);
						if (ret ==OSIP_SUCCESS) 
						{
							//	解析 sdp 中的 “y” 域
							get_media_session_priv(body->body, &invite_media_session);
						}
						
						Invite_adaptor_ind(&invite_adaptor, &invite_media_session);

						sdp_message_free (remote_sdp);
						remote_sdp = NULL;
					}

					if(pInvite_ack)
						pInvite_ack(&invite_adaptor);
					
					eXosip_unlock (context_eXosip);
					context_eXosip_unlock();
			    }
				break;

				case EXOSIP_CALL_CLOSED:
				{
					sipd_media_session invite_adaptor;
					memset(&invite_adaptor, 0, sizeof(sipd_media_session));

					/**< a BYE was received for this call      */
					printf("sipd_protocolevent received a Bye from this call [EXOSIP_CALL_CLOSED]. the remote hang up the session! did=%d tid=%d\n", je->did, je->tid);

					invite_adaptor.cid = je->cid;
					invite_adaptor.did = je->did;
					invite_adaptor.tid = je->tid;

					if(pInvite_bye)
						pInvite_bye(&invite_adaptor);
								
					context_eXosip_lock();
					eXosip_lock (context_eXosip);
					ret= eXosip_call_build_answer (context_eXosip, je->tid, 200, &answer);
					if (ret != OSIP_SUCCESS)
					{
						printf("sipd_protocolevent [EXOSIP_CALL_CLOSED] This request msg is invalid!Cann't response!\n");
						eXosip_call_send_answer (context_eXosip, je->tid, 400, NULL);

					}
					else
					{
						ret = eXosip_call_send_answer (context_eXosip, je->tid, 200, answer);
						if (ret != OSIP_SUCCESS)
						{
							printf("sipd_protocolevent [EXOSIP_CALL_CLOSED] eXosip_call_send_answer failed\n");
						}
						else
						{
							printf("sipd_protocolevent [EXOSIP_CALL_CLOSED] receive bye and send 200 OK over\n");
						}
					} 
					eXosip_unlock (context_eXosip);
					context_eXosip_unlock();	
			    }
				break;

				//	该信息表示：作为主叫，收到了被叫对 BYE 的确认消息
				case EXOSIP_CALL_RELEASED:
					{
						sipd_media_session invite_adaptor;
						memset(&invite_adaptor, 0, sizeof(sipd_media_session));

						invite_adaptor.cid = je->cid;
						invite_adaptor.did = je->did;
						invite_adaptor.tid = je->tid;

						if(pInvite_release)
							pInvite_release(&invite_adaptor);
					}
					break;

				case EXOSIP_CALL_MESSAGE_NEW:
				{
					printf("%s line=%d get message in call. EXOSIP_CALL_MESSAGE_NEW sip_method=%s\n", __FUNCTION__, __LINE__, je->request->sip_method);
					if (MSG_IS_INFO(je->request))
					{
						osip_content_type_t* content_type=NULL;

						printf("%s line=%d is INFO message\n", __FUNCTION__, __LINE__);
						context_eXosip_lock();
						eXosip_lock (context_eXosip);
						content_type = osip_message_get_content_type(je->request);
						if(NULL == content_type)
						{
							printf("%s line=%d Message content_type is NULL! return 400 \n", __FUNCTION__, __LINE__);
							eXosip_call_send_answer (context_eXosip, je->tid, 400, NULL);
						}
						else
						{
							if((strcasecmp(content_type->type, CONTENT_TYPE_APP) == 0) && (strcasecmp(content_type->subtype, CONTENT_TYPE_SUB_MANSRTSP) == 0))
							{
								ret = eXosip_call_build_answer (context_eXosip,je->tid, 200, &answer);
								if (ret != OSIP_SUCCESS)
								{
									printf("%s line=%d [EXOSIP_CALL_MESSAGE_NEW] info.eXosip_call_build_answer  build 200 OK ERROR!\n", __FUNCTION__, __LINE__);
									eXosip_call_send_answer (context_eXosip, je->tid, 400, NULL);
								}
								else
								{
									eXosip_call_send_answer (context_eXosip,je->tid, 200, answer);
								}
							}
							else  if((strcasecmp(content_type->type, CONTENT_TYPE_APP) == 0) && (strcasecmp(content_type->subtype, CONTENT_TYPE_SUB_RTSP) == 0))
							{
								ret = eXosip_call_build_answer (context_eXosip,je->tid, 200, &answer);
								if (ret != OSIP_SUCCESS)
								{
									printf("%s line=%d [EXOSIP_CALL_MESSAGE_NEW] info.eXosip_call_build_answer  build 200 OK ERROR!\n", __FUNCTION__, __LINE__);
									eXosip_call_send_answer (context_eXosip, je->tid, 400, NULL);
								}
								else
								{
									eXosip_call_send_answer (context_eXosip,je->tid, 200, answer);
								}
							}
							else
							{
								eXosip_call_send_answer (context_eXosip, je->tid, 400, NULL);
							}
						}
						eXosip_unlock (context_eXosip);
						context_eXosip_unlock();

						if(content_type != NULL)
						{
							if((strcasecmp(content_type->type, CONTENT_TYPE_APP) == 0) && (strcasecmp(content_type->subtype, CONTENT_TYPE_SUB_MANSRTSP) == 0))
							{
								handle_method_INFO(je->request, je->tid, je->did);
							}
							else if((strcasecmp(content_type->type, CONTENT_TYPE_APP) == 0) && (strcasecmp(content_type->subtype, CONTENT_TYPE_SUB_RTSP) == 0))
							{
								handle_method_INFO(je->request, je->tid, je->did);
							}

						}

					}

					if(MSG_IS_SUBSCRIBE(je->request))
					{
						printf("%s line=%d get subscribe message! \n", __FUNCTION__, __LINE__);
						printf("%s line=%d <- (nid(%d) did(%d)> SUBSCRIBE in EXOSIP_CALL_MESSAGE_NEW\n", __FUNCTION__, __LINE__, je->nid, je->did);

					}		
				}
				break;

				

				case EXOSIP_NOTIFICATION_ANSWERED:
				{
						/**< announce a 200ok                */
					printf("%s line=%d <- (nid(%d) did(%d)> 200 OK for NOTIFY from \n", __FUNCTION__, __LINE__, je->nid, je->did);
				
					}
				break;

				case EXOSIP_NOTIFICATION_NOANSWER:
				{
					printf("%s line=%d <- (nid(%d) did(%d)> NOTIFY No Answer  from\n", __FUNCTION__, __LINE__, je->nid, je->did);

					
				}
				break;

				case EXOSIP_NOTIFICATION_PROCEEDING:
				{
					printf("%s line=%d <- (nid(%d) did(%d)> NOTIFY Processding  from\n", __FUNCTION__, __LINE__, je->nid, je->did);
					
				}
				break;

				case EXOSIP_NOTIFICATION_REDIRECTED:
				{
					printf("%s line=%d <- (nid(%d) did(%d)> NOTIFY redirected  from \n", __FUNCTION__, __LINE__, je->nid, je->did);	
				}
				break;

				

				case EXOSIP_NOTIFICATION_SERVERFAILURE:
				{
					printf("%s line=%d <- (nid(%d) did(%d)> NOTIFY server failure  from\n", __FUNCTION__, __LINE__, je->nid, je->did);		
				}
				break;

				case EXOSIP_NOTIFICATION_GLOBALFAILURE:
				{
					printf("%s line=%d <- (nid(%d) did(%d)> NOTIFY global failure  from \n", __FUNCTION__, __LINE__, je->nid, je->did);		
				}
				break;

				case EXOSIP_SUBSCRIPTION_ANSWERED:
				{
					
					printf("%s line=%d <- (nid(%d) did(%d)>  subscription answer 200 ok  from \n", __FUNCTION__, __LINE__, je->nid, je->did);
				}
				break;

				//	新增订阅功能
				case EXOSIP_IN_SUBSCRIPTION_NEW:
				{
					osip_content_type_t* content_type=NULL;
					int is_message_legal = 0;

					context_eXosip_lock();
					eXosip_lock (context_eXosip);
					content_type = osip_message_get_content_type(je->request);
					if(NULL == content_type)
					{
						printf("%s line=%d Message content_type is NULL! return 400 \n", __FUNCTION__, __LINE__);
						eXosip_call_send_answer (context_eXosip, je->tid, 400, NULL);
					}
					else
					{
						is_message_legal = 0/*check_deviceID_MESSAGE(je->request)*/;
						if(0 == is_message_legal)
						{
							ret = eXosip_insubscription_build_answer(context_eXosip,je->tid, 200,&answer);
							if (ret !=OSIP_SUCCESS)
							{
								printf("%s:%d eXosip_insubscription_build_answer [EXOSIP_IN_SUBSCRIPTION_NEW Message] failed\n", __FUNCTION__, __LINE__);
							}

							ret = eXosip_insubscription_send_answer (context_eXosip,je->tid, 200,answer);
							if (ret !=OSIP_SUCCESS)
							{
								printf("%s:%d eXosip_message_send_answer [EXOSIP_IN_SUBSCRIPTION_NEW Message] failed\n", __FUNCTION__, __LINE__);
							}
						}
						else
						{
							printf("%s line=%d check_deviceID_MESSAGE ERROR! \n", __FUNCTION__, __LINE__);
							eXosip_call_send_answer (context_eXosip, je->tid, 400, NULL);
						}
					}

					eXosip_unlock (context_eXosip);
					context_eXosip_unlock();

					if(0 == is_message_legal)
					{
						handle_method_SUBSCRIBE(je->request, je->tid, je->did);
					}
				}
				break;
		  
				/*case EXOSIP_REGISTRATION_NEW:

				break;*/

				default:  
		        break;  
		  
		      }  

		     if(je->external_reference != NULL)
		     {
		  		printf("%s line=%d Notice external_reference is not NULL ! \n", __FUNCTION__, __LINE__);
				osip_free(je->external_reference);
		      }
		      eXosip_event_free(je); 

			#if defined(WIN32)
			Sleep(10);
			#else
			  sleep(1);
			#endif
		}
		else
		{
			#if defined(WIN32)
			Sleep(3*1000);
			#else
			sleep(3);
			#endif
		}	 
	}  
}


