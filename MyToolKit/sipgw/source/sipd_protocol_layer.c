#include "sipd.h"
//#include "proxycore.h"


struct eXosip_t *context_eXosip = NULL;


#define MAX_SIPD_BODY_LENGTH			(1024)
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

#define SIP_PS_H264 	96

/*TESTCODE*/
#if 0
struct sipd_media_session test_media_session;
#endif


extern void sipd_manscdp_xml_deal(const osip_message_t * sip_request, char *body);
extern int sip_sdp_create(const sipd_sender_sdp_info* pSenderSdp, char* sdp_buffer);
extern void Sip_Message_Answer(int tid, boll isOk);


int Invite_adaptor_ind(const struct sipd_invite_media_session* pSdpSession)
{
	sipd_media_session invite_adaptor;
	char* p=NULL;
	int len=0;
	int play_type=0;

	printf("Invite_adaptor_ind \n");
	memset(&invite_adaptor, 0, sizeof(invite_adaptor));

	invite_adaptor.did = pSdpSession->did;
	invite_adaptor.tid = pSdpSession->tid;
	strncpy(invite_adaptor.o_id, pSdpSession->o_field_username, MAX_SIPD_DEVICE_ID_LEN);
	
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
		strncpy(invite_adaptor.channel_id, pSdpSession->u_field, len);

		play_type = atoi(p+1);
		if(0 == play_type)
		{
			invite_adaptor.Type = SIPD_RECORD_INFO_TYPE_TIME;
		}
		else if(1 == play_type)
		{
			invite_adaptor.Type = SIPD_RECORD_INFO_TYPE_ALARM;
		}
		else if(2 == play_type)
		{
			invite_adaptor.Type = SIPD_RECORD_INFO_TYPE_MANUAL;
		}
		else if(3 == play_type)
		{
			invite_adaptor.Type = SIPD_RECORD_INFO_TYPE_ALL;
		}
		else
		{
			invite_adaptor.Type = SIPD_RECORD_INFO_TYPE_ALL;
		}
		
		//printf("Invite_adaptor_ind channel_id=%s play_type(U_field)=%d Type=%d u_field=%s \n", invite_adaptor.channel_id, play_type, invite_adaptor.Type, pSdpSession->u_field);
		
	}
	else
	{
		strncpy(invite_adaptor.channel_id, pSdpSession->u_field, MAX_SIPD_DEVICE_ID_LEN);
		invite_adaptor.Type = SIPD_RECORD_INFO_TYPE_ALL;
		//printf("Invite_adaptor_ind channel_id=%s  Type=%d u_field=%s \n", invite_adaptor.channel_id, invite_adaptor.Type, pSdpSession->u_field);
	}

	invite_adaptor.stream_type = pSdpSession->stream_type;
	invite_adaptor.media_recv_ip_type = SIPD_IPType_IPv4;
	strncpy(invite_adaptor.media_recv_ip, pSdpSession->connect_video_ip, MAX_SIPD_URI);
	strncpy(invite_adaptor.media_recv_port, pSdpSession->connect_video_port, MAX_SIPD_NORMAL_LEN);
	invite_adaptor.media_type = pSdpSession->media_type;
	strncpy(invite_adaptor.t_start_time, pSdpSession->t_start_time, MAX_SIPD_NORMAL_LEN);
	strncpy(invite_adaptor.t_end_time, pSdpSession->t_end_time, MAX_SIPD_NORMAL_LEN);
	strncpy(invite_adaptor.ssrc, pSdpSession->pri_ssrc_field, MAX_SIPD_NORMAL_LEN);
	
	invite_adaptor.f_video_type = pSdpSession->f_video_type;
	invite_adaptor.f_resolution = pSdpSession->f_resolution;
	invite_adaptor.f_framerate = pSdpSession->f_framerate;
	invite_adaptor.f_rate_type = pSdpSession->f_rate_type;
	invite_adaptor.bitrate = pSdpSession->bitrate;
	invite_adaptor.audio_type = pSdpSession->audio_type;
	invite_adaptor.audio_bitrate = pSdpSession->audio_bitrate;
	invite_adaptor.audio_samplerate = pSdpSession->audio_samplerate;
	
	

	Invite_play_playback_download(&invite_adaptor);

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

int build_and_send_invite_200_ok(const sipd_sender_sdp_info* pInviteRspSdp)
{
	char local_sdp[MAX_SIPD_BODY_LENGTH];
	int ret=0;
	osip_message_t *answer = NULL;

	memset(local_sdp, 0, sizeof(local_sdp));

	sip_sdp_create(pInviteRspSdp, local_sdp);
	//printf("build_and_send_invite_200_ok create local sdp len=%d sdp:\n%s\n",  strlen(local_sdp), local_sdp);
	

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


int sip_sdp_create(const sipd_sender_sdp_info* pSenderSdp, char* sdp_buffer)
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
		"y=%s\r\n",
		pSenderSdp->media_sender_id, pSenderSdp->media_send_ip, "GB28181 DEVICE", pSenderSdp->media_send_ip, pSenderSdp->media_send_port, SIP_PS_H264, SIP_PS_H264, pSenderSdp->ssrc);

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

	ret = gw_get_domain_server_cfg(pCfg);
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

		memset(&preset_req, 0, sizeof(preset_req));
		strncpy(preset_req.device_id, device_id, MAX_SIPD_DEVICE_ID_LEN);

		preset_req.preset_id = ptzcode.data2;
		//printf("%s line=%d  set preset preset_id =%d\n", __FUNCTION__, __LINE__, preset_req.preset_id);
		
		#ifdef WIN32
		SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_PRESET_ADD_REQ, &preset_req, sizeof(preset_req));
		#else
		SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_PRESET_ADD_REQ, &preset_req, sizeof(preset_req));
		#endif	
		
	}
	else if(SIPD_PTZ_INSTRUCTION_CODE_CALL_PRESET == ptzcode.instruction)
	{
		/*call preset*/
		struct sipd_preset_req preset_req;

		memset(&preset_req, 0, sizeof(preset_req));
		strncpy(preset_req.device_id, device_id, MAX_SIPD_DEVICE_ID_LEN);

		preset_req.preset_id = ptzcode.data2;
		//printf("%s line=%d  call preset preset_id =%d\n", __FUNCTION__, __LINE__, preset_req.preset_id);
		
		#ifdef WIN32
		SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_PRESET_GOTO_REQ, &preset_req, sizeof(preset_req));
		#else
		SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_PRESET_GOTO_REQ, &preset_req, sizeof(preset_req));
		#endif
	}
	else if(SIPD_PTZ_INSTRUCTION_CODE_DELETE_PRESET == ptzcode.instruction)
	{
		/*delete preset*/
		struct sipd_preset_req preset_req;

		memset(&preset_req, 0, sizeof(preset_req));
		strncpy(preset_req.device_id, device_id, MAX_SIPD_DEVICE_ID_LEN);

		preset_req.preset_id = ptzcode.data2;
		//printf("%s line=%d  delete preset preset_id =%d\n", __FUNCTION__, __LINE__, preset_req.preset_id);
		
		#ifdef WIN32
		SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_PRESET_DEL_REQ, &preset_req, sizeof(preset_req));
		#else
		SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_PRESET_DEL_REQ, &preset_req, sizeof(preset_req));
		#endif
	}
	//	巡航指令
	else if(SIPD_PTZ_INSTRUCTION_CODE_ADD_CRUISE == ptzcode.instruction)
	{
		/*add cruise*/
		struct sipd_cruiseroute_req cruise_req;
		
		memset(&cruise_req, 0, sizeof(cruise_req));
		strncpy(cruise_req.device_id, device_id, MAX_SIPD_DEVICE_ID_LEN);

		cruise_req.cruise_id = ptzcode.data1;
		cruise_req.preset_id = ptzcode.data2;
		//printf("%s line=%d add cruise cruise_id=%d preset_id =%d\n", __FUNCTION__, __LINE__, cruise_req.cruise_id, cruise_req.preset_id);

		#ifdef WIN32
		SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_TOUR_ADD_REQ, &cruise_req, sizeof(cruise_req));
		#else
		SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_TOUR_ADD_REQ, &cruise_req, sizeof(cruise_req));
		#endif
	}
	else if(SIPD_PTZ_INSTRUCTION_CODE_DELETE_CRUISE == ptzcode.instruction)
	{
		/*delete cruise*/
		struct sipd_cruiseroute_req cruise_req;
		
		memset(&cruise_req, 0, sizeof(cruise_req));
		strncpy(cruise_req.device_id, device_id, MAX_SIPD_DEVICE_ID_LEN);

		cruise_req.cruise_id = ptzcode.data1;
		cruise_req.preset_id = ptzcode.data2;
		//printf("%s line=%d delete cruise cruise_id=%d preset_id =%d\n", __FUNCTION__, __LINE__, cruise_req.cruise_id, cruise_req.preset_id);

		#ifdef WIN32
		SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_TOUR_DEL_REQ, &cruise_req, sizeof(cruise_req));
		#else
		SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_TOUR_DEL_REQ, &cruise_req, sizeof(cruise_req));
		#endif
	}
	else if(SIPD_PTZ_INSTRUCTION_CODE_SET_SPEED_CRUISE == ptzcode.instruction)
	{
		/*set cruise speed*/
		struct sipd_cruiseroute_req cruise_req;
		
		memset(&cruise_req, 0, sizeof(cruise_req));
		strncpy(cruise_req.device_id, device_id, MAX_SIPD_DEVICE_ID_LEN);
		
		cruise_req.cruise_id = ptzcode.data1;
		cruise_req.speed= ptzcode.data2 + ((ptzcode.combineCode2_data3) << 8);
		//printf("%s line=%d set cruise speed cruise_id=%d speed =%d ptzcode.data2=%d data3=%d\n", __FUNCTION__, __LINE__, cruise_req.cruise_id, cruise_req.speed, ptzcode.data2, ptzcode.combineCode2_data3);

		#ifdef WIN32
		SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_TOUR_SET_SPEED_REQ, &cruise_req, sizeof(cruise_req));
		#else
		SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_TOUR_SET_SPEED_REQ, &cruise_req, sizeof(cruise_req));
		#endif
	}
	else if(SIPD_PTZ_INSTRUCTION_CODE_SET_TIME_CRUISE == ptzcode.instruction)
	{
		/*set cruise time*/
		struct sipd_cruiseroute_req cruise_req;
		
		memset(&cruise_req, 0, sizeof(cruise_req));
		strncpy(cruise_req.device_id, device_id, MAX_SIPD_DEVICE_ID_LEN);
		
		cruise_req.cruise_id = ptzcode.data1;
		cruise_req.time = ptzcode.data2 + ((ptzcode.combineCode2_data3) << 8);
		//printf("%s line=%d set cruise time cruise_id=%d time =%d ptzcode.data2=%d data3=%d\n", __FUNCTION__, __LINE__, cruise_req.cruise_id, cruise_req.time, ptzcode.data2, ptzcode.combineCode2_data3);

		#ifdef WIN32
		SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_TOUR_SET_STAYTIME_REQ, &cruise_req, sizeof(cruise_req));
		#else
		SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_TOUR_SET_STAYTIME_REQ, &cruise_req, sizeof(cruise_req));
		#endif
	}
	else if(SIPD_PTZ_INSTRUCTION_CODE_START_CRUISE == ptzcode.instruction)
	{
		/*start cruise*/
		struct sipd_cruiseroute_req cruise_req;
		
		memset(&cruise_req, 0, sizeof(cruise_req));
		strncpy(cruise_req.device_id, device_id, MAX_SIPD_DEVICE_ID_LEN);
		
		cruise_req.cruise_id = ptzcode.data1;
		cruise_req.preset_id = ptzcode.data2;
		//printf("%s line=%d start cruise cruise_id=%d preset_id =%d\n", __FUNCTION__, __LINE__, cruise_req.cruise_id, cruise_req.preset_id);

		#ifdef WIN32
		SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_TOUR_START_REQ, &cruise_req, sizeof(cruise_req));
		#else
		SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_TOUR_START_REQ, &cruise_req, sizeof(cruise_req));
		#endif
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
			/*auto scan*/
			req.scan_id = ptzcode.data1;
			//printf("%s line=%d start auto scan scan_id=%d \n", __FUNCTION__, __LINE__, req.scan_id);
			
			#ifdef WIN32
			SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_AUTOSCAN_START_REQ, &req, sizeof(req));
			#else
			SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_AUTOSCAN_START_REQ, &req, sizeof(req));
			#endif
		}
		//	设置自动巡航左边界
		else if(0x01 == ptzcode.data2)
		{
			/*auto scan, set left */
			req.scan_id = ptzcode.data1;
			//printf("%s line=%d set left auto scan scan_id=%d \n", __FUNCTION__, __LINE__, req.scan_id);
			
			#ifdef WIN32
			SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_AUTOSCAN_SET_LEFT_REQ, &req, sizeof(req));
			#else
			SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_AUTOSCAN_SET_LEFT_REQ, &req, sizeof(req));
			#endif
		}
		//	设置自动巡航右边界
		else if(0x02 == ptzcode.data2)
		{
			/*auto scan, set right */
			req.scan_id = ptzcode.data1;
			//printf("%s line=%d set right auto scan scan_id=%d \n", __FUNCTION__, __LINE__, req.scan_id);

			#ifdef WIN32
			SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_AUTOSCAN_SET_RIGHT_REQ, &req, sizeof(req));
			#else
			SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_AUTOSCAN_SET_RIGHT_REQ, &req, sizeof(req));
			#endif
		}
	}
	else if(SIPD_PTZ_INSTRUCTION_CODE_SET_AUTOSCAN_SPEED == ptzcode.instruction)
	{
		struct sipd_ptz_autoscan_req req;
			

		memset(&req, 0, sizeof(req));
		strncpy(req.device_id, device_id, MAX_SIPD_DEVICE_ID_LEN);

		req.scan_id = ptzcode.data1;
		req.speed = ptzcode.data2 + ((ptzcode.combineCode2_data3) << 8);
		//printf("%s line=%d set auto scan speed scan_id=%d time =%d ptzcode.data2=%d data3=%d\n", __FUNCTION__, __LINE__, req.scan_id, req.speed, ptzcode.data2, ptzcode.combineCode2_data3);

		#ifdef WIN32
		SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_AUTOSCAN_SET_SPEED_REQ, &req, sizeof(req));
		#else
		SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_AUTOSCAN_SET_SPEED_REQ, &req, sizeof(req));
		#endif
	}
	//	ptz 的所有操作均停止
	else if(SIPD_PTZ_INSTRUCTION_CODE_ZERO == ptzcode.instruction)
	{
		struct sipd_ptz_req req;
		struct sipd_cruiseroute_req cruise_req;
			
		
		memset(&req, 0, sizeof(req));
		strncpy(req.device_id, device_id, MAX_SIPD_DEVICE_ID_LEN);

		memset(&cruise_req, 0, sizeof(cruise_req));
		strncpy(cruise_req.device_id, device_id, MAX_SIPD_DEVICE_ID_LEN);

		req.speed = 0;

		#ifdef WIN32
		SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_PTZ_STOP_REQ, &req, sizeof(req));
		#else
		SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_PTZ_STOP_REQ, &req, sizeof(req));
		#endif

		#if 1
		cruise_req.cruise_id = ptzcode.data1;
		cruise_req.preset_id = ptzcode.data2;
		//printf("%s line=%d stop all (all zero) cruise_id=%d preset_id=%d \n", __FUNCTION__, __LINE__, cruise_req.cruise_id, cruise_req.preset_id);

		#ifdef WIN32
		SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_CRUISE_STOP_REQ, &cruise_req, sizeof(cruise_req));
		#else
		SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_CRUISE_STOP_REQ, &cruise_req, sizeof(cruise_req));
		#endif
		
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
				//	镜头变倍缩小（OUT）
				req.speed = 0;

				#if 0
				#ifdef WIN32
				SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_PTZ_STOP_REQ, &req, sizeof(req));
				#else
				SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_PTZ_STOP_REQ, &req, sizeof(req));
				#endif
				#endif
				
				/*zoom out*/
				req.speed = ptzcode.combineCode2_data3;
				//printf("%s line=%d zoom out speed=%d \n", __FUNCTION__, __LINE__, req.speed);

				#ifdef WIN32
				SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_PTZ_ZOOMWIDE_REQ, &req, sizeof(req));
				#else
				SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_PTZ_ZOOMWIDE_REQ, &req, sizeof(req));
				#endif
			}
			else if(((ptzcode.instruction & 0x10)>>4) == 1)
			{
				//	镜头变倍放大（IN）
				req.speed = 0;

				#if 0
				#ifdef WIN32
				SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_PTZ_STOP_REQ, &req, sizeof(req));
				#else
				SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_PTZ_STOP_REQ, &req, sizeof(req));
				#endif
				#endif
				
				/*zoom in*/
				req.speed = ptzcode.combineCode2_data3;
				//printf("%s line=%d zoom in speed=%d \n", __FUNCTION__, __LINE__, req.speed);

				#ifdef WIN32
				SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_PTZ_ZOOMTELE_REQ, &req, sizeof(req));
				#else
				SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_PTZ_ZOOMTELE_REQ, &req, sizeof(req));
				#endif
			}
			else
			{
				req.speed = 0;
				//printf("%s line=%d zoom stop\n", __FUNCTION__, __LINE__);
				#ifdef WIN32
				SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_PTZ_STOP_REQ, &req, sizeof(req));
				#else
				SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_PTZ_STOP_REQ, &req, sizeof(req));
				#endif
			}

			if(((ptzcode.instruction & 0x8)>>3) == 1)
			{
				//	向上
				req.speed = 0;

				#if 0
				#ifdef WIN32
				SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_PTZ_STOP_REQ, &req, sizeof(req));
				#else
				SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_PTZ_STOP_REQ, &req, sizeof(req));
				#endif
				#endif
				
				/*up*/
				if(((ptzcode.instruction & 0x2)>>1) == 1)
				{
					/*left， 左上 */
					req.speed = SIPD_MAX2(ptzcode.data1, ptzcode.data2);
					//printf("%s line=%d UP_LEFT in speed=%d \n", __FUNCTION__, __LINE__, req.speed);

					#ifdef WIN32
					SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_PTZ_LU_REQ, &req, sizeof(req));
					#else
					SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_PTZ_LU_REQ, &req, sizeof(req));
					#endif
				}
				else if((ptzcode.instruction & 0x1) == 1)
				{
					/*right， 右上 */
					req.speed = SIPD_MAX2(ptzcode.data1, ptzcode.data2);
					//printf("%s line=%d UP_RIGHT in speed=%d \n", __FUNCTION__, __LINE__, req.speed);

					#ifdef WIN32
					SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_PTZ_RU_REQ, &req, sizeof(req));
					#else
					SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_PTZ_RU_REQ, &req, sizeof(req));
					#endif
				}
				else
				{
					//	垂直向上
					req.speed = ptzcode.data2;
					//printf("%s line=%d UP in speed=%d \n", __FUNCTION__, __LINE__, req.speed);

					#ifdef WIN32
					SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_PTZ_UP_REQ, &req, sizeof(req));
					#else
					SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_PTZ_UP_REQ, &req, sizeof(req));
					#endif
				}
			}
			else if(((ptzcode.instruction & 0x4)>>2) == 1)
			{
				//	向下
				req.speed = 0;

				#if 0
				#ifdef WIN32
				SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_PTZ_STOP_REQ, &req, sizeof(req));
				#else
				SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_PTZ_STOP_REQ, &req, sizeof(req));
				#endif
				#endif
				
				/*down*/
				if(((ptzcode.instruction & 0x2)>>1) == 1)
				{
					/*left，  左下 */
					req.speed = SIPD_MAX2(ptzcode.data1, ptzcode.data2);
					//printf("%s line=%d DOWN_LEFT in speed=%d \n", __FUNCTION__, __LINE__, req.speed);

					#ifdef WIN32
					SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_PTZ_LD_REQ, &req, sizeof(req));
					#else
					SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_PTZ_LD_REQ, &req, sizeof(req));
					#endif
				}
				else if((ptzcode.instruction & 0x1) == 1)
				{
					/*right， 右下 */
					req.speed = SIPD_MAX2(ptzcode.data1, ptzcode.data2);
					//printf("%s line=%d DOWN_RIGHT in speed=%d \n", __FUNCTION__, __LINE__, req.speed);

					#ifdef WIN32
					SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_PTZ_RD_REQ, &req, sizeof(req));
					#else
					SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_PTZ_RD_REQ, &req, sizeof(req));
					#endif
				}
				else
				{
					//	垂直向下
					req.speed = ptzcode.data2;
					//printf("%s line=%d DOWN in speed=%d \n", __FUNCTION__, __LINE__, req.speed);

					#ifdef WIN32
					SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_PTZ_DOWN_REQ, &req, sizeof(req));
					#else
					SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_PTZ_DOWN_REQ, &req, sizeof(req));
					#endif
				}
			}
			else
			{
				if(((ptzcode.instruction & 0x2)>>1) == 1)
				{
					//	简单向左
					req.speed = 0;

					#if 0
					#ifdef WIN32
					SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_PTZ_STOP_REQ, &req, sizeof(req));
					#else
					SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_PTZ_STOP_REQ, &req, sizeof(req));
					#endif
					#endif
					
					/*left*/
					req.speed = ptzcode.data1;
					//printf("%s line=%d Left in speed=%d \n", __FUNCTION__, __LINE__, req.speed);

					#ifdef WIN32
					SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_PTZ_LEFT_REQ, &req, sizeof(req));
					#else
					SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD, MOD_SIPD, 0, 0, 0, MSG_ID_GW_PTZ_LEFT_REQ, &req, sizeof(req));
					#endif
				}
				else if((ptzcode.instruction & 0x1) == 1)
				{
					//`简单向右
					req.speed = 0;

					#if 0
					#ifdef WIN32
					SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_PTZ_STOP_REQ, &req, sizeof(req));
					#else
					SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_PTZ_STOP_REQ, &req, sizeof(req));
					#endif
					#endif
					
					/*right*/
					req.speed = ptzcode.data1;
					//printf("%s line=%d RIGHT in speed=%d \n", __FUNCTION__, __LINE__, req.speed);

					#ifdef WIN32
					SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_PTZ_RIGHT_REQ, &req, sizeof(req));
					#else
					SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_PTZ_RIGHT_REQ, &req, sizeof(req));
					#endif
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
				//	光圈缩小
				req.speed = 0;

				#if 0
				#ifdef WIN32
				SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_PTZ_STOP_REQ, &req, sizeof(req));
				#else
				SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_PTZ_STOP_REQ, &req, sizeof(req));
				#endif
				#endif
				
				/*iris -*/
				req.speed = ptzcode.data2;
				//printf("%s line=%d iris - in speed=%d \n", __FUNCTION__, __LINE__, req.speed);

				#ifdef WIN32
				SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_PTZ_HALLO_SMALL_REQ, &req, sizeof(req));
				#else
				SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_PTZ_HALLO_SMALL_REQ, &req, sizeof(req));
				#endif
			}
			else if(((ptzcode.instruction & 0x4)>>2) == 1)
			{
				//	光圈放大
				req.speed = 0;

				#if 0
				#ifdef WIN32
				SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_PTZ_STOP_REQ, &req, sizeof(req));
				#else
				SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_PTZ_STOP_REQ, &req, sizeof(req));
				#endif
				#endif
				
				/*iris +*/
				req.speed = ptzcode.data2;
				//printf("%s line=%d iris + in speed=%d \n", __FUNCTION__, __LINE__, req.speed);

				#ifdef WIN32
				SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_PTZ_HALLO_LARGE_REQ, &req, sizeof(req));
				#else
				SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_PTZ_HALLO_LARGE_REQ, &req, sizeof(req));
				#endif
			}
			else
			{
				req.speed = 0;
				//printf("%s line=%d iris stop in speed=%d \n", __FUNCTION__, __LINE__, req.speed);
				#ifdef WIN32
				SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_PTZ_STOP_REQ, &req, sizeof(req));
				#else
				SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_PTZ_STOP_REQ, &req, sizeof(req));
				#endif
			}


			if(((ptzcode.instruction & 0x2)>>1) == 1)
			{
				//	聚焦更近
				req.speed = 0;

				#if 0
				#ifdef WIN32
				SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_PTZ_STOP_REQ, &req, sizeof(req));
				#else
				SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_PTZ_STOP_REQ, &req, sizeof(req));
				#endif
				#endif
				
				/*focus near*/
				req.speed = ptzcode.data1;
				//printf("%s line=%d focus near in speed=%d \n", __FUNCTION__, __LINE__, req.speed);

				#ifdef WIN32
				SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_PTZ_FOCUS_NEAR_REQ, &req, sizeof(req));
				#else
				SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_PTZ_FOCUS_NEAR_REQ, &req, sizeof(req));
				#endif
			}
			else if((ptzcode.instruction & 0x1) == 1)
			{
				//	聚焦更远
				req.speed = 0;

				#if 0
				#ifdef WIN32
				SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_PTZ_STOP_REQ, &req, sizeof(req));
				#else
				SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_PTZ_STOP_REQ, &req, sizeof(req));
				#endif
				#endif
				
				/*focus far*/
				req.speed = ptzcode.data1;
				//printf("%s line=%d focus far in speed=%d \n", __FUNCTION__, __LINE__, req.speed);

				#ifdef WIN32
				SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_PTZ_FOCUS_FAR_REQ, &req, sizeof(req));
				#else
				SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_PTZ_FOCUS_FAR_REQ, &req, sizeof(req));
				#endif
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
	//printf("%s line=%d \n", __FUNCTION__, __LINE__);
	
	content_type = osip_message_get_content_type(sip_request);
	if(NULL == content_type)
	{
		printf("%s line=%d content_type is NULL! \n", __FUNCTION__, __LINE__);
		return -1;
	}
	
	//printf("%s line=%d contetnt_type type=%s sub_type=%s \n", __FUNCTION__, __LINE__, content_type->type, content_type->subtype);
       if((strcasecmp(content_type->type, CONTENT_TYPE_APP) == 0) && (strcasecmp(content_type->subtype, CONTENT_TYPE_SUB_MANSRTSP) == 0))
       {
       	osip_message_get_body (sip_request, 0, &body); 
             // printf("%s line=%d body->length=%d I get the msg is: %s\n", __FUNCTION__, __LINE__, body->length, body->body);

		memset(&mans_rtsp_ind, 0, sizeof(mans_rtsp_ind));
		strncpy(mans_rtsp_ind.rtsp_des, body->body,MAX_SIPD_INFO_LENGTH);
		mans_rtsp_ind.len = strlen(mans_rtsp_ind.rtsp_des);
		mans_rtsp_ind.tid = tid;
		mans_rtsp_ind.did = did;

		#ifdef WIN32
		SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_INVITE_INFO_IND, &mans_rtsp_ind, sizeof(mans_rtsp_ind));
		#else
		SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_INVITE_INFO_IND, &mans_rtsp_ind, sizeof(mans_rtsp_ind));
		#endif
       }
	else if((strcasecmp(content_type->type, CONTENT_TYPE_APP) == 0) && (strcasecmp(content_type->subtype, CONTENT_TYPE_SUB_RTSP) == 0))
       {
       	osip_message_get_body (sip_request, 0, &body); 
              //printf("%s line=%d body->length=%d I get the msg is: %s\n", __FUNCTION__, __LINE__, body->length, body->body);

		memset(&mans_rtsp_ind, 0, sizeof(mans_rtsp_ind));
		strncpy(mans_rtsp_ind.rtsp_des, body->body,MAX_SIPD_INFO_LENGTH);
		mans_rtsp_ind.len = strlen(mans_rtsp_ind.rtsp_des);
		mans_rtsp_ind.tid = tid;
		mans_rtsp_ind.did = did;

		#ifdef WIN32
		SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_INVITE_INFO_IND, &mans_rtsp_ind, sizeof(mans_rtsp_ind));
		#else
		SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_INVITE_INFO_IND, &mans_rtsp_ind, sizeof(mans_rtsp_ind));
		#endif
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

				strncpy(strbuf,  (char *)mxmlGetText(CmdType_node, NULL), sizeof(strbuf));
				DeviceID_node = mxmlFindElement(que_node, tree, "DeviceID", NULL, NULL,MXML_DESCEND);

				if(DeviceID_node != NULL)
				{
					char id[MAX_SIPD_DEVICE_ID_LEN+1];
					char snbuf[MAX_SIPD_DEVICE_ID_LEN+1];

					memset(id, 0, sizeof(id));

					strncpy(id, (char *)mxmlGetText(DeviceID_node, NULL), sizeof(id));


					memset(snbuf, 0, sizeof(snbuf));
					sn_node = mxmlFindElement(que_node, tree, "SN", NULL, NULL, MXML_DESCEND);

					if(sn_node != NULL)
					{
						sipd_deviceinfo_req devinfo;

						strncpy(snbuf, (char *)mxmlGetText(sn_node, NULL), sizeof(snbuf));

						if(strcmp(sip_request->from->url->host, (char*)sipd_cfg.upper_server_name) == 0)
						{
							//printf("%s line=%d  GB28181 use sipserver domain \n", __FUNCTION__, __LINE__);
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

							//printf("%s line=%d  from=%s to=%s\n", __FUNCTION__, __LINE__, devinfo.from_uri, devinfo.to_uri);
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

						//printf("je->request->from->url->host:%s:%s\n",sip_request->from->url->host, sip_request->from->url->port);

						strncpy(devinfo.device_id, id, sizeof(devinfo.device_id));
						strncpy(devinfo.SN, snbuf, sizeof(devinfo.SN));

						//	设备目录订阅
						if(strcmp(strbuf, DEVICECATALOG) == 0) 
						{
							sipd_catalog_req catalog_req;

							//printf("%s line=%d device catalog query \n", __FUNCTION__, __LINE__);

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
								// printf("%s line=%d  StartTime=%s\n", __FUNCTION__, __LINE__, mxmlGetText(node, &whitespace));
								strncpy(catalog_req.StartTime, mxmlGetText(node, &whitespace), MAX_SIPD_TIME_LEN);
							}

							node = mxmlFindElement(tree, tree, XML_NAME_EndTime, NULL, NULL, MXML_DESCEND);
							if(NULL != node)
							{	
								//printf( "%s line=%d  EndTime=%s\n", __FUNCTION__, __LINE__, mxmlGetText(node, &whitespace));
								strncpy(catalog_req.EndTime, mxmlGetText(node, &whitespace), MAX_SIPD_TIME_LEN);
							}

							#ifdef WIN32
							SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD_SUBSCRIBE, MSG_ID_GW_DESCRIBE_CATALOG_REQ, &catalog_req, sizeof(catalog_req));
							#else
							SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0,MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, 
								MSG_ID_GW_CATALOGINFO_REQ, (void *) &catalog_req, sizeof(catalog_req));
							#endif
						}
						//	报警事件订阅
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
								//printf("%s line=%d  StartTime=%s\n", __FUNCTION__, __LINE__, mxmlGetText(node, &whitespace));
								strncpy(perset_query_req.StartTime, mxmlGetText(node, &whitespace), MAX_SIPD_TIME_LEN);
							}

							node = mxmlFindElement(tree, tree, XML_NAME_EndTime, NULL, NULL, MXML_DESCEND);
							if(NULL != node)
							{	
								//printf( "%s line=%d  EndTime=%s\n", __FUNCTION__, __LINE__, mxmlGetText(node, &whitespace));
								strncpy(perset_query_req.EndTime, mxmlGetText(node, &whitespace), MAX_SIPD_TIME_LEN);
							}

							#ifdef WIN32
							SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD_SUBSCRIBE, MSG_ID_GW_DESCRIBE_ALARM_REQ, &perset_query_req, sizeof(perset_query_req));
							#else
							SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0,MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, 
								MSG_ID_GW_CATALOGINFO_REQ, (void *) &perset_query_req, sizeof(perset_query_req));
							#endif
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
			node = mxmlFindElement(tree, tree, XML_NAME_SN, NULL, NULL, MXML_DESCEND);
			if(NULL == node)
			{	
			       printf ("%s line=%d parse error. XML Cann't Find SN......\n", __FUNCTION__, __LINE__);
				if(tree != NULL)
				{
					mxmlDelete(tree);
				}
				return -1;
			}
			//printf("%s line=%d  SN=%s\n", __FUNCTION__, __LINE__, mxmlGetText(node, &whitespace));
			device_control.sn = atoi(mxmlGetText(node, &whitespace));

			node = mxmlFindElement(tree, tree, XML_NAME_DeviceID, NULL, NULL, MXML_DESCEND);
			if(NULL == node)
			{	
			       printf ("%s line=%d parse error. XML Cann't Find DeviceID......\n", __FUNCTION__, __LINE__);
				if(tree != NULL)
				{
					mxmlDelete(tree);
				}
				return -1;
			}
			//printf("%s line=%d  DeviceID=%s\n", __FUNCTION__, __LINE__, mxmlGetText(node, &whitespace));
			strncpy(device_control.device_id, mxmlGetText(node, &whitespace), MAX_SIPD_DEVICE_ID_LEN);

			
			
			node = mxmlFindElement(tree, tree, XML_NAME_PTZCmd, NULL, NULL, MXML_DESCEND);
			if(NULL != node)
			{	
			       //printf("%s line=%d  PTZCmd=%s\n", __FUNCTION__, __LINE__, mxmlGetText(node, &whitespace));
				strncpy(device_control.ptz_cmd, mxmlGetText(node, &whitespace), PTZ_CMD_LEN);
			}

			node = mxmlFindElement(tree, tree, XML_NAME_TeleBoot, NULL, NULL, MXML_DESCEND);
			if(NULL != node)
			{	
			       //printf("%s line=%d  TeleBoot=%s\n", __FUNCTION__, __LINE__, mxmlGetText(node, &whitespace));
				device_control.TeleBoot = 1;
			}

			node = mxmlFindElement(tree, tree, XML_NAME_RecordCmd, NULL, NULL, MXML_DESCEND);
			if(NULL != node)
			{	
			      // printf("%s line=%d  RecordCmd=%s\n", __FUNCTION__, __LINE__, mxmlGetText(node, &whitespace));
				strncpy(device_control.recordType, mxmlGetText(node, &whitespace), MAX_SIPD_NORMAL_LEN);
			}

			node = mxmlFindElement(tree, tree, XML_NAME_GuardCmd, NULL, NULL, MXML_DESCEND);
			if(NULL != node)
			{	
			      //printf("%s line=%d  GuardCmd=%s\n", __FUNCTION__, __LINE__, mxmlGetText(node, &whitespace));
				strncpy(device_control.guardType, mxmlGetText(node, &whitespace), MAX_SIPD_NORMAL_LEN);
			}

			node = mxmlFindElement(tree, tree, XML_NAME_AlarmCmd, NULL, NULL, MXML_DESCEND);
			if(NULL != node)
			{	
			       //printf("%s line=%d  AlarmCmd=%s\n", __FUNCTION__, __LINE__, mxmlGetText(node, &whitespace));
				strncpy(device_control.alarm, mxmlGetText(node, &whitespace), MAX_SIPD_NORMAL_LEN);
			}

			node = mxmlFindElement(tree, tree, XML_NAME_ControlPriority, NULL, NULL, MXML_DESCEND);
			if(NULL != node)
			{	
			       //printf("%s line=%d  ControlPriority=%s\n", __FUNCTION__, __LINE__, mxmlGetText(node, &whitespace));
				device_control.ControlPriority = atoi(mxmlGetText(node, &whitespace));
			}
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

	
	
	if(is_device_control_message)
	{
		//printf("%s line=%d  device_control.SN=%d ControlPriority=%d teleboot=%d\n", __FUNCTION__, __LINE__, device_control.sn, device_control.ControlPriority, device_control.TeleBoot);
		//printf("%s line=%d  device_control.device_id=%s\n", __FUNCTION__, __LINE__, device_control.device_id);
		//printf("%s line=%d  device_control.ptz_cmd=%s\n", __FUNCTION__, __LINE__, device_control.ptz_cmd);
		//printf("%s line=%d  device_control.recordType=%s\n", __FUNCTION__, __LINE__, device_control.recordType);
		//printf("%s line=%d  device_control.guardType=%s\n", __FUNCTION__, __LINE__, device_control.guardType);
		//printf("%s line=%d  device_control.alarm=%s\n", __FUNCTION__, __LINE__, device_control.alarm);
	
		if(strlen(device_control.ptz_cmd) > 0)
		{
			ret = handle_PTZCmd(device_control.ptz_cmd, device_control.device_id);
			if(-1 == ret)
			{
				printf("%s line=%d  handle_PTZCmd Error\n", __FUNCTION__, __LINE__);
				return -1;
			}
		}
		else if (device_control.TeleBoot)
		{
			struct sipd_reboot_req req;

			memset(&req, 0, sizeof(req));
			strncpy(req.device_id, device_control.device_id, MAX_SIPD_DEVICE_ID_LEN);

			#ifdef WIN32
			SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_REBOOT_REQ, &req, sizeof(req));
			#else
			SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_REBOOT_REQ, &req, sizeof(req));
			#endif
		}
		else if(strlen(device_control.recordType) > 0)
		{
			struct sipd_record_req req;
			char* dest=NULL;
			int ret;

			memset(&req, 0, sizeof(req));
			strncpy(req.device_id, device_control.device_id, MAX_SIPD_DEVICE_ID_LEN);
			req.sn = device_control.sn;
			if(strcmp(device_control.recordType, XML_VALUE_RecordCmd_Record) == 0)
			{
				req.record_onOff = 1;
			}
			else
			{
				req.record_onOff = 0;
			}

			printf("%s line=%d  sip_request->from->url->host=%s  sipd_cfg.sipserver_domain=%s\n", __FUNCTION__, __LINE__, sip_request->from->url->host, (char*)sipd_cfg.upper_server_name);
			if(strcmp(sip_request->from->url->host, (char*)sipd_cfg.upper_server_name) == 0)
			{
				dest = NULL;
				
				printf("%s line=%d  GB28181 use sipserver domain \n", __FUNCTION__, __LINE__);
				sprintf(req.from,"sip:%s@%s:%d", (char*)sipd_cfg.upper_server_id, (char*)sipd_cfg.upper_server_ip, (char*)sipd_cfg.upper_server_port);

				dest = NULL;
				ret = osip_to_to_str(sip_request->to, &dest);
				if(ret != OSIP_SUCCESS)
				{
					printf("%s line=%d  osip_to_to_str Error\n", __FUNCTION__, __LINE__);
				}
				//printf("%s line=%d  to=%s \n", __FUNCTION__, __LINE__, dest);
				strncpy(req.to, dest, MAX_SIPD_URI);
				osip_free(dest);
				
				//printf("%s line=%d  from=%s to=%s\n", __FUNCTION__, __LINE__, req.from, req.to);
			}
			else
			{
				dest = NULL;
				ret = osip_from_to_str(sip_request->from, &dest);
				if(ret != OSIP_SUCCESS)
				{
					printf("%s line=%d  osip_from_to_str Error\n", __FUNCTION__, __LINE__);
				}
				//printf("%s line=%d  from=%s \n", __FUNCTION__, __LINE__, dest);
				strncpy(req.from, dest, MAX_SIPD_URI);
				osip_free(dest);

				dest = NULL;
				ret = osip_to_to_str(sip_request->to, &dest);
				if(ret != OSIP_SUCCESS)
				{
					printf("%s line=%d  osip_to_to_str Error\n", __FUNCTION__, __LINE__);
				}
				//printf("%s line=%d  to=%s \n", __FUNCTION__, __LINE__, dest);
				strncpy(req.to, dest, MAX_SIPD_URI);
				osip_free(dest);
			}


			dest = NULL;
			ret = osip_cseq_to_str(sip_request->cseq, &dest);
			if(ret != OSIP_SUCCESS)
			{
				printf("%s line=%d  osip_cseq_to_str Error\n", __FUNCTION__, __LINE__);
			}
			//printf("%s line=%d  to=%s \n", __FUNCTION__, __LINE__, dest);
			strncpy(req.cseq, dest, MAX_SIPD_URI);
			osip_free(dest);
			
			#ifdef WIN32
			SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_RECORD_CONTROL_REQ, &req, sizeof(req));
			#else
			SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_RECORD_CONTROL_REQ, &req, sizeof(req));
			#endif
		}
		else if(strlen(device_control.guardType) > 0)
		{
			struct sipd_alram_guard_req req;
			char* dest=NULL;
			int ret;

			memset(&req, 0, sizeof(req));
			strncpy(req.device_id, device_control.device_id, MAX_SIPD_DEVICE_ID_LEN);
			req.sn = device_control.sn;
			if(strcmp(device_control.guardType, XML_VALUE_GuardCmd_SetGuard) == 0)
			{
				req.onOff = 1;
			}
			else
			{
				req.onOff = 0;
			}

			printf("%s line=%d  sip_request->from->url->host=%s  sipd_cfg.sipserver_domain=%s\n", __FUNCTION__, __LINE__, sip_request->from->url->host, (char*)sipd_cfg.upper_server_name);
			if(strcmp(sip_request->from->url->host, (char*)sipd_cfg.upper_server_name) == 0)
			{
				printf("%s line=%d  GB28181 use sipserver domain \n", __FUNCTION__, __LINE__);
				sprintf(req.from,"sip:%s@%s:%d", (char*)sipd_cfg.upper_server_id, (char*)sipd_cfg.upper_server_ip, (char*)sipd_cfg.upper_server_port);

				dest = NULL;
				ret = osip_to_to_str(sip_request->to, &dest);
				if(ret != OSIP_SUCCESS)
				{
					printf("%s line=%d  osip_to_to_str Error\n", __FUNCTION__, __LINE__);
				}
				//printf("%s line=%d  to=%s \n", __FUNCTION__, __LINE__, dest);
				strncpy(req.to, dest, MAX_SIPD_URI);
				osip_free(dest);
				//printf("%s line=%d  from=%s to=%s\n", __FUNCTION__, __LINE__, req.from, req.to);
			}
			else
			{
				dest = NULL;
				ret = osip_from_to_str(sip_request->from, &dest);
				if(ret != OSIP_SUCCESS)
				{
					printf("%s line=%d  osip_from_to_str Error\n", __FUNCTION__, __LINE__);
				}
				//printf("%s line=%d  from=%s \n", __FUNCTION__, __LINE__, dest);
				strncpy(req.from, dest, MAX_SIPD_URI);
				osip_free(dest);

				dest = NULL;
				ret = osip_to_to_str(sip_request->to, &dest);
				if(ret != OSIP_SUCCESS)
				{
					printf("%s line=%d  osip_to_to_str Error\n", __FUNCTION__, __LINE__);
				}
				//printf("%s line=%d  to=%s \n", __FUNCTION__, __LINE__, dest);
				strncpy(req.to, dest, MAX_SIPD_URI);
				osip_free(dest);
			}

			dest = NULL;
			ret = osip_cseq_to_str(sip_request->cseq, &dest);
			if(ret != OSIP_SUCCESS)
			{
				printf("%s line=%d  osip_cseq_to_str Error\n", __FUNCTION__, __LINE__);
			}
			//printf("%s line=%d  to=%s \n", __FUNCTION__, __LINE__, dest);
			strncpy(req.cseq, dest, MAX_SIPD_URI);
			osip_free(dest);

			#ifdef WIN32
			SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_ALARM_GUARD_REQ, &req, sizeof(req));
			#else
			SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_ALARM_GUARD_REQ, &req, sizeof(req));
			#endif
		}
		else if(strlen(device_control.alarm) > 0)
		{
			//	报警复位
			struct sipd_alram_cmd_req req;
			char* dest=NULL;
			int ret;

			memset(&req, 0, sizeof(req));
			strncpy(req.device_id, device_control.device_id, MAX_SIPD_DEVICE_ID_LEN);
			req.sn = device_control.sn;
			if(strcmp(device_control.alarm, XML_VALUE_AlarmCmd_ResetAlarm) == 0)
			{
				req.reset = 1;
			}
			else
			{
				req.reset = 0;
			}

			printf("%s line=%d  sip_request->from->url->host=%s  sipd_cfg.sipserver_domain=%s\n", __FUNCTION__, __LINE__, sip_request->from->url->host, (char*)sipd_cfg.upper_server_name);
			if(strcmp(sip_request->from->url->host, (char*)sipd_cfg.upper_server_name) == 0)
			{
				//printf("%s line=%d  GB28181 use sipserver domain \n", __FUNCTION__, __LINE__);
				sprintf(req.from,"sip:%s@%s:%d", (char*)sipd_cfg.upper_server_id, (char*)sipd_cfg.upper_server_ip, (char*)sipd_cfg.upper_server_port);

				dest = NULL;
				ret = osip_to_to_str(sip_request->to, &dest);
				if(ret != OSIP_SUCCESS)
				{
					printf("%s line=%d  osip_to_to_str Error\n", __FUNCTION__, __LINE__);
				}
				//printf("%s line=%d  to=%s \n", __FUNCTION__, __LINE__, dest);
				strncpy(req.to, dest, MAX_SIPD_URI);
				osip_free(dest);
				//printf("%s line=%d  from=%s to=%s\n", __FUNCTION__, __LINE__, req.from, req.to);
			}
			else
			{
				dest = NULL;
				ret = osip_from_to_str(sip_request->from, &dest);
				if(ret != OSIP_SUCCESS)
				{
					printf("%s line=%d  osip_from_to_str Error\n", __FUNCTION__, __LINE__);
				}
				//printf("%s line=%d  from=%s \n", __FUNCTION__, __LINE__, dest);
				strncpy(req.from, dest, MAX_SIPD_URI);
				osip_free(dest);

				dest = NULL;
				ret = osip_to_to_str(sip_request->to, &dest);
				if(ret != OSIP_SUCCESS)
				{
					printf("%s line=%d  osip_to_to_str Error\n", __FUNCTION__, __LINE__);
				}
				//printf("%s line=%d  to=%s \n", __FUNCTION__, __LINE__, dest);
				strncpy(req.to, dest, MAX_SIPD_URI);
				osip_free(dest);
			}

			dest = NULL;
			ret = osip_cseq_to_str(sip_request->cseq, &dest);
			if(ret != OSIP_SUCCESS)
			{
				printf("%s line=%d  osip_cseq_to_str Error\n", __FUNCTION__, __LINE__);
			}
			//printf("%s line=%d  to=%s \n", __FUNCTION__, __LINE__, dest);
			strncpy(req.cseq, dest, MAX_SIPD_URI);
			osip_free(dest);

			#ifdef WIN32
			SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_ALARM_CONTROL_REQ, &req, sizeof(req));
			#else
			SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_ALARM_CONTROL_REQ, &req, sizeof(req));
			#endif
		}
	}
	else if (is_alarm_message_response)
	{
		//printf("%s line=%d  message is_alarm_message_response\n", __FUNCTION__, __LINE__);
	}
	else
	{
		//printf("%s line=%d  check is query MESSAGE or not! \n", __FUNCTION__, __LINE__);

		// 处理目录查询、设备信息查询、设备状态查询
		sipd_manscdp_xml_deal(sip_request, buffer);
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

	isDetail = TRUE;
	if(isNotify)
	{
		isDetail = FALSE;
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

		isDetail = TRUE;
		if(isNotify)
		{
			isDetail = FALSE;
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
	 if(ret >= OSIP_SUCCESS)
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

				len = strlen(tmp);
				snprintf(tmp+len, (MAX_XML_BUFFER_LEN-len),"</Item>\r\n");

			}

			len = strlen(tmp);
			snprintf(tmp+len, (MAX_XML_BUFFER_LEN-len),"</Alarmstatus>\r\n");
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
	mxml_node_t *tree, *que_node, *CmdType_node, *sn_node;
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
			que_node = mxmlFindElement(tree, tree, QUERY, NULL, NULL,MXML_DESCEND);

			if(que_node != NULL)
			{
				CmdType_node = mxmlFindElement(que_node, tree, CMDTYPE, NULL, NULL,MXML_DESCEND);
				if(CmdType_node != NULL)
				{
					mxml_node_t *DeviceID_node = NULL;

					strncpy(strbuf,  (char *)mxmlGetText(CmdType_node, NULL), sizeof(strbuf));

					DeviceID_node = mxmlFindElement(que_node, tree, "DeviceID", NULL, NULL,MXML_DESCEND);
					
					if(DeviceID_node != NULL)
					{
						char id[MAX_SIPD_DEVICE_ID_LEN+1];
						char snbuf[MAX_SIPD_DEVICE_ID_LEN+1];
						
						memset(id, 0, sizeof(id));

						strncpy(id, (char *)mxmlGetText(DeviceID_node, NULL), sizeof(id));
						

						memset(snbuf, 0, sizeof(snbuf));
						sn_node = mxmlFindElement(que_node, tree, "SN", NULL, NULL, MXML_DESCEND);

						if(sn_node != NULL)
						{
							sipd_deviceinfo_req devinfo;

							strncpy(snbuf, (char *)mxmlGetText(sn_node, NULL), MAX_SIPD_DEVICE_ID_LEN);
							

							//printf("%s line=%d  sip_request->from->url->host=%s  sipd_cfg.sipserver_domain=%s\n", __FUNCTION__, __LINE__, sip_request->from->url->host, (char*)sipd_cfg.upper_server_name);
							if(strcmp(sip_request->from->url->host, (char*)sipd_cfg.upper_server_name) == 0)
							{
								//printf("%s line=%d  GB28181 use sipserver domain \n", __FUNCTION__, __LINE__);
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
								
								//printf("%s line=%d  from=%s to=%s\n", __FUNCTION__, __LINE__, devinfo.from_uri, devinfo.to_uri);
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

							//printf("je->request->from->url->host:%s:%s\n",sip_request->from->url->host, sip_request->from->url->port);

							strncpy(devinfo.device_id, id, MAX_SIPD_DEVICE_ID_LEN);
							strncpy(devinfo.SN, snbuf, MAX_SIPD_DEVICE_ID_LEN);
							
							
							if(strcmp(strbuf, DEVICEINFO) == 0) 
							{
								//printf("come to device info query\n");

								#ifdef WIN32
								SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD_QUERY, MSG_ID_GW_DEVICEINFO_REQ, &devinfo, sizeof(devinfo));
								#else
								SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0,MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, 
										MSG_ID_GW_DEVICEINFO_REQ, (void *) &devinfo, sizeof(devinfo));	
								#endif
							}
							else if(strcmp(strbuf, DEVICESTATUS) == 0) 
							{
								//printf("come to device state query\n");

								#ifdef WIN32
								SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD_QUERY, MSG_ID_GW_STATEINFO_REQ, &devinfo, sizeof(devinfo));
								#else
								SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0,MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, 
										MSG_ID_GW_STATEINFO_REQ, (void *) &devinfo, sizeof(devinfo));
								#endif
							}
							else if(strcmp(strbuf, DEVICECATALOG) == 0) 
							{
								sipd_catalog_req catalog_req;
								
								//printf("%s line=%d device catalog query \n", __FUNCTION__, __LINE__);

								memset(&catalog_req, 0, sizeof(catalog_req));

								strncpy(catalog_req.device_id, devinfo.device_id, MAX_SIPD_DEVICE_ID_LEN);
								strncpy(catalog_req.SN, devinfo.SN, MAX_SIPD_DEVICE_ID_LEN);
								strncpy(catalog_req.from_uri, devinfo.from_uri, MAX_SIPD_URI);
								strncpy(catalog_req.to_uri, devinfo.to_uri, MAX_SIPD_URI);

								node = mxmlFindElement(tree, tree, XML_NAME_StartTime, NULL, NULL, MXML_DESCEND);
								if(NULL != node)
								{	
								      // printf("%s line=%d  StartTime=%s\n", __FUNCTION__, __LINE__, mxmlGetText(node, &whitespace));
									strncpy(catalog_req.StartTime, mxmlGetText(node, &whitespace), MAX_SIPD_TIME_LEN);
								}

								node = mxmlFindElement(tree, tree, XML_NAME_EndTime, NULL, NULL, MXML_DESCEND);
								if(NULL != node)
								{	
								       //printf( "%s line=%d  EndTime=%s\n", __FUNCTION__, __LINE__, mxmlGetText(node, &whitespace));
									strncpy(catalog_req.EndTime, mxmlGetText(node, &whitespace), MAX_SIPD_TIME_LEN);
								}

								#ifdef WIN32
								SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD_CATALOG_QUERY, MSG_ID_GW_CATALOGINFO_REQ, &catalog_req, sizeof(catalog_req));
								#else
								SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0,MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, 
										MSG_ID_GW_CATALOGINFO_REQ, (void *) &catalog_req, sizeof(catalog_req));
								#endif
							}
							else if(strcmp(strbuf, SIPD_RECORDINFO) == 0) 
							{
								sipd_recordinfo_req recordinfo_req;
								
								//printf("%s line=%d RecordInfo query \n", __FUNCTION__, __LINE__);
								memset(&recordinfo_req, 0, sizeof(recordinfo_req));

								strncpy(recordinfo_req.device_id, devinfo.device_id, MAX_SIPD_DEVICE_ID_LEN);
								strncpy(recordinfo_req.SN, devinfo.SN, MAX_SIPD_DEVICE_ID_LEN);
								strncpy(recordinfo_req.from_uri, devinfo.from_uri, MAX_SIPD_URI);
								strncpy(recordinfo_req.to_uri, devinfo.to_uri, MAX_SIPD_URI);

								
								node = mxmlFindElement(tree, tree, XML_NAME_StartTime, NULL, NULL, MXML_DESCEND);
								if(NULL != node)
								{	
								      // printf("%s line=%d  StartTime=%s\n", __FUNCTION__, __LINE__, mxmlGetText(node, &whitespace));
									if(mxmlGetText(node, &whitespace))
										strncpy(recordinfo_req.StartTime, mxmlGetText(node, &whitespace), MAX_SIPD_TIME_LEN);
								}

								node = mxmlFindElement(tree, tree, XML_NAME_EndTime, NULL, NULL, MXML_DESCEND);
								if(NULL != node)
								{	
								      // printf( "%s line=%d  EndTime=%s\n", __FUNCTION__, __LINE__, mxmlGetText(node, &whitespace));
									if(mxmlGetText(node, &whitespace))
										strncpy(recordinfo_req.EndTime, mxmlGetText(node, &whitespace), MAX_SIPD_TIME_LEN);
								}

								node = mxmlFindElement(tree, tree, XML_NAME_FilePath, NULL, NULL, MXML_DESCEND);
								if(NULL != node)
								{	
								       //printf("%s line=%d  FilePath=%s\n", __FUNCTION__, __LINE__, mxmlGetText(node, &whitespace));
									if(mxmlGetText(node, &whitespace))
										strncpy(recordinfo_req.FilePath, mxmlGetText(node, &whitespace), MAX_SIPD_URI);
								}

								node = mxmlFindElement(tree, tree, XML_NAME_Address, NULL, NULL, MXML_DESCEND);
								if(NULL != node)
								{	
								       //printf("%s line=%d  Address=%s\n", __FUNCTION__, __LINE__, mxmlGetText(node, &whitespace));
									if(mxmlGetText(node, &whitespace))
										strncpy(recordinfo_req.Address, mxmlGetText(node, &whitespace), MAX_SIPD_URI);
								}

								node = mxmlFindElement(tree, tree, XML_NAME_Secrecy, NULL, NULL, MXML_DESCEND);
								if(NULL != node)
								{	
								       //printf("%s line=%d  Secrecy=%s\n", __FUNCTION__, __LINE__, mxmlGetText(node, &whitespace));
									if(mxmlGetText(node, &whitespace))
										recordinfo_req.Secrecy = atoi(mxmlGetText(node, &whitespace));
								}

								node = mxmlFindElement(tree, tree, XML_NAME_Type, NULL, NULL, MXML_DESCEND);
								if(NULL != node)
								{	
								      // printf("%s line=%d  Type=%s\n", __FUNCTION__, __LINE__, mxmlGetText(node, &whitespace));
									if(strcmp(mxmlGetText(node, &whitespace), XML_Value_Type_time) == 0)
									{
										recordinfo_req.Type = SIPD_RECORD_INFO_TYPE_TIME;
									}
									else if(strcmp(mxmlGetText(node, &whitespace), XML_Value_Type_alarm) == 0)
									{
										recordinfo_req.Type = SIPD_RECORD_INFO_TYPE_ALARM;
									}
									else if(strcmp(mxmlGetText(node, &whitespace), XML_Value_Type_manual) == 0)
									{
										recordinfo_req.Type = SIPD_RECORD_INFO_TYPE_MANUAL;
									}
									else if(strcmp(mxmlGetText(node, &whitespace), XML_Value_Type_all) == 0)
									{
										recordinfo_req.Type = SIPD_RECORD_INFO_TYPE_ALL;
									}
								}

								node = mxmlFindElement(tree, tree, XML_NAME_RecorderID, NULL, NULL, MXML_DESCEND);
								if(NULL != node)
								{	
								      // printf("%s line=%d  RecorderID=%s\n", __FUNCTION__, __LINE__, mxmlGetText(node, &whitespace));
									if(mxmlGetText(node, &whitespace))
										strncpy(recordinfo_req.RecorderID, mxmlGetText(node, &whitespace), MAX_SIPD_DEVICE_ID_LEN);
								}
								
								#ifdef WIN32
								SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD_RECORD_QUERY, MSG_ID_GW_RECORDINFO_REQ, &recordinfo_req, sizeof(recordinfo_req));
								#else
								SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0,MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, 
										MSG_ID_GW_RECORDINFO_REQ, (void *) &recordinfo_req, sizeof(recordinfo_req));
								#endif
							}
							//	新增“预制点查询”
							else if(strcmp(strbuf, PERSETQUERY) == 0) 
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
									//printf("%s line=%d  StartTime=%s\n", __FUNCTION__, __LINE__, mxmlGetText(node, &whitespace));
									strncpy(perset_query_req.StartTime, mxmlGetText(node, &whitespace), MAX_SIPD_TIME_LEN);
								}

								node = mxmlFindElement(tree, tree, XML_NAME_EndTime, NULL, NULL, MXML_DESCEND);
								if(NULL != node)
								{	
									//printf( "%s line=%d  EndTime=%s\n", __FUNCTION__, __LINE__, mxmlGetText(node, &whitespace));
									strncpy(perset_query_req.EndTime, mxmlGetText(node, &whitespace), MAX_SIPD_TIME_LEN);
								}

								#ifdef WIN32
								SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD_PERSET_QUERY, MSG_ID_GW_PERSET_QUERY_REQ, &perset_query_req, sizeof(perset_query_req));
								#else
								SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0,MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, 
									MSG_ID_GW_CATALOGINFO_REQ, (void *) &perset_query_req, sizeof(perset_query_req));
								#endif
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
	Sipd_GetSIPServerCfg(&sip_cfg);

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
	
	memset(&tm_req, 0, sizeof(tm_req));

	osip_message_get_date(je->response, 0, &date_header);

	if(date_header == NULL)
	{
		printf("date_header is NULL\n");
		return;
	}
		
	//printf("sipd_send_timing_req  %s:%s\n", date_header->hname, date_header->hvalue);

	GMTStrToTime_t(date_header->hvalue, &(tm_req.tm));

	#ifdef WIN32
	SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_SET_TIME_REQ, &tm_req, sizeof(Sipd_timing_req));
	#else
	SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0,MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, 
				MSG_ID_GW_SET_TIME_REQ, (void *) &tm_req, sizeof(Sipd_timing_req));
	#endif
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
#if 0
			context_eXosip_lock();
			eXosip_lock(context_eXosip);

			#if 0
			eXosip_register_build_register(context_eXosip, je->rid, 300, &reg);
			eXosip_register_send_register(context_eXosip, je->rid,reg);
			#endif

			eXosip_automatic_action(context_eXosip); 
			
			eXosip_unlock(context_eXosip); 
			context_eXosip_unlock();
#endif
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

	Set_Dev_Online(-1);
	
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
	result = gw_get_domain_server_cfg_init(&sip_cfg);
	if(-1 == result)
	{
		printf("%s line=%d gw_get_domain_server_cfg_init err\n", __FUNCTION__, __LINE__);
		return -1;
	}
	
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
		    	continue;  

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
					Set_Dev_Online(1);
					Set_Reg_Id(je->rid);
					sipd_send_timing_req(je);

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
					context_eXosip_unlock();

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
						get_media_session(remote_sdp, &invite_media_session);
						invite_media_session.tid = je->tid;
						invite_media_session.did = je->did;

						ret = osip_message_get_body(je->request, 0, &body);
						if (ret !=OSIP_SUCCESS) 
						{
							printf("sipd_protocolevent  EXOSIP_CALL_INVITE osip_message_get_body failed\n");
						 }
						//printf("%s:%d get body=%s \n", __FUNCTION__, __LINE__, body->body);
						get_media_session_priv(body->body, &invite_media_session);
						
						if(SIPD_STREAM_TYPE_PLAY == invite_media_session.stream_type)
						{
							strncpy(invite_media_session.u_field, je->request->to->url->username, MAX_SIPD_URI);
						}
							  
						//printf("sipd_protocolevent  EXOSIP_CALL_INVITE after get video_ssrc=%s , audio_ssrc=%s and f=%s priv sdp attribute\n",  invite_media_session.pri_ssrc_field,  invite_media_session.pri_ssrc_audio_field, invite_media_session.pri_f_field);

						//printf("check invite_media_session: o_username=%s\n", invite_media_session.o_field_username);
						//printf("\t\t cid=%d did=%d media_type(video=1)=%d stream_type(Play=1)=%d \n", invite_media_session.tid, invite_media_session.did, invite_media_session.media_type, invite_media_session.stream_type);
						//printf("\t\t video ip=%s addrtype=%s port=%s \n", invite_media_session.connect_video_ip, invite_media_session.connect_video_addrtype, invite_media_session.connect_video_port);
						//printf("\t\t audio ip=%s addrtype=%s port=%s \n", invite_media_session.connect_audio_ip, invite_media_session.connect_audio_addrtype, invite_media_session.connect_audio_port);
						//printf("\t\t u_field=%s start_time=%s end_time=%s \n", invite_media_session.u_field, invite_media_session.t_start_time, invite_media_session.t_end_time);
						//printf("\t\t y=%s f=%s \n", invite_media_session.pri_ssrc_field, invite_media_session.pri_f_field);
						//printf("\t\t f_enable=%d f_video_type=%d f_resolution=%d f_framerate=%d f_rate_type=%d\n", invite_media_session.f_enable, invite_media_session.f_video_type, invite_media_session.f_resolution, invite_media_session.f_framerate, invite_media_session.f_rate_type);
						//printf("\t\t bitrate=%d audio_type=%d audio_bitrate=%d audio_samplerate=%d \n", invite_media_session.bitrate, invite_media_session.audio_type, invite_media_session.audio_bitrate, invite_media_session.audio_samplerate);

						eXosip_call_send_answer (context_eXosip, je->tid, 180, NULL);

						/*critical*/
						sdp_message_free (remote_sdp);
	    					remote_sdp = NULL;

						Invite_adaptor_ind(&invite_media_session);
					}
					else
					{
						printf("sipd_protocolevent  Invite eXosip_get_remote_sdp get fail ! ERROR !\n");
						eXosip_call_send_answer (context_eXosip, je->tid, 400, NULL);
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

				case EXOSIP_CALL_ACK:
				{
					struct sipd_invite_ctrl_ind invite_ctrl_req;

					 /**< ACK received for 200ok to INVITE      */
					printf("sipd_protocolevent ACK recieved! Start Transmit RTP media data. did=%d tid=%d\n",  je->did, je->tid);

					memset(&invite_ctrl_req, 0, sizeof(invite_ctrl_req));
					invite_ctrl_req.did = je->did;
					invite_ctrl_req.tid = je->tid;
					invite_ctrl_req.cmd_type = SIPD_INVITE_CTRL_TYPE_START;

					#ifdef WIN32
					SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_INVITE_CTRL_IND, &invite_ctrl_req, sizeof(invite_ctrl_req));
					#else
					SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_INVITE_CTRL_IND, &invite_ctrl_req, sizeof(invite_ctrl_req));
					#endif			

			    	}
				break;

				case EXOSIP_CALL_CLOSED:
				{
					struct sipd_invite_ctrl_ind invite_ctrl_req;
					
					 /**< a BYE was received for this call      */
					printf("sipd_protocolevent received a Bye from this call [EXOSIP_CALL_CLOSED]. the remote hang up the session! did=%d tid=%d\n", je->did, je->tid);

					memset(&invite_ctrl_req, 0, sizeof(invite_ctrl_req));
					invite_ctrl_req.did = je->did;
					invite_ctrl_req.tid = je->tid;
					invite_ctrl_req.cmd_type = SIPD_INVITE_CTRL_TYPE_STOP;

					#ifdef WIN32
					SendMessageEx(MOD_SIPD_PROTOC, MOD_SIPD, MSG_ID_GW_INVITE_CTRL_IND, &invite_ctrl_req, sizeof(invite_ctrl_req));
					#else
					SendMessageEx(SUPER_USER_NAME, SUPER_USER_PASSWORD,0, MOD_SIPD_PROTOC, MOD_SIPD, 0, 0, 0, MSG_ID_GW_INVITE_CTRL_IND, &invite_ctrl_req, sizeof(invite_ctrl_req));
					#endif			


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
			Sleep(100);
			#else
			sleep(3);
			#endif
		}	 
	}  
}


