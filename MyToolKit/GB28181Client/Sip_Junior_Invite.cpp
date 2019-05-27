
#include "stdafx.h"
#include "Gb28181_Layer.h"

#include <eXosip.h>
#include <osip_headers.h>
#include <osip_mt.h>

#define SIPD_RTP_DEFAULT_AUDIO_SSRC				("000000000")
#define SIPD_RTP_DEFAULT_VIDEO_SSRC				("000000001")

bool parse_deviceId_and_SN(mxml_node_t *tree, mxml_node_t *query_node, string & deviceId, string & SN);

//////////////////////////////////////////////////////////////////////////

Sipd_Call_Response_func		pSipd_Call_200_OK_resp;
Sipd_Call_Response_func		pSipd_Call_File_to_end;
Sipd_Call_Response_func		pSipd_Call_No_Answer;

/*注册回调函数， 当收到 200 OK 回复时回调*/
void Register_Sipd_Call_200_OK(Sipd_Call_Response_func pfunc)
{
	pSipd_Call_200_OK_resp = pfunc;
}

/*注册回调函数, 当收到 Message ( 如 File_to_End)消息时回调*/
void Register_Sipd_Call_File_to_end(Sipd_Call_Response_func pfunc)
{
	pSipd_Call_File_to_end = pfunc;
}

/*注册回调函数, 当超时无应答时回调*/
void Register_Sipd_Call_No_Answer(Sipd_Call_Response_func pfunc)
{
	pSipd_Call_No_Answer = pfunc;
}


/*启动实时流、回放流、下载流*/
int Sip_Call_play_playback_download(const sipd_media_session* pMediaInfo, int isWithSDP)
{
	osip_message_t *invite = NULL;
	int ret ;
	
	sipd_message_query_or_response  msg;
	memset(&msg, 0 , sizeof(sipd_message_query_or_response));
	strncpy_s(msg.to_uri, pMediaInfo->to_uri, MAX_SIPD_URI);
	strncpy_s(msg.from_uri, pMediaInfo->from_uri, MAX_SIPD_URI);

	string s;
	if(pMediaInfo->stream_type==SIPD_STREAM_TYPE_PLAY)				s = "Play";
	else if(pMediaInfo->stream_type==SIPD_STREAM_TYPE_PLAYBACK)		s = "Playback";
	else if(pMediaInfo->stream_type==SIPD_STREAM_TYPE_DOWNLOAD)		s = "Download";

	if(pMediaInfo->stream_type==SIPD_STREAM_TYPE_PLAY)
	{
		snprintf(msg.body, MAX_SIPD_BODY_LENGTH,
			"v=0\r\n"\
			"o=%s 0 0 IN IP4 %s\r\n"\
			"s=%s\r\n"\
			"i=%s\r\n"\
			"c=IN IP4 %s\r\n"\
			"t=%s %s\r\n"\
			/*"m=video %d RTP/AVP %d %d\r\n"\*/
			"m=video %s RTP/AVP %d\r\n"\
			"a=recvonly\r\n"\
			/*"a=rtpmap:%d PS/90000\r\n"\*/
			"a=rtpmap:%d PS/90000\r\n"\
			/*"a=username:%s\r\n"\*/
			/*"a=password:%s\r\n"\*/
			"y=%s\r\n",
			pMediaInfo->o_id, 
			pMediaInfo->media_recv_ip,
			s.c_str(),
			"GB28181 DEVICE", 
			pMediaInfo->media_recv_ip,

			pMediaInfo->t_start_time,
			pMediaInfo->t_end_time,
			pMediaInfo->media_recv_port, 
			SIP_PS_H264, SIP_PS_H264, 
			pMediaInfo->ssrc);
	}
	else
	{
		//	回放增加了 U 域
		snprintf(msg.body, MAX_SIPD_BODY_LENGTH,
			"v=0\r\n"\
			"o=%s 0 0 IN IP4 %s\r\n"\
			"s=%s\r\n"\
			"u=%s\r\n"\
			/*"i=%s\r\n"\*/
			"c=IN IP4 %s\r\n"\
			"t=%s %s\r\n"\
			/*"m=video %d RTP/AVP %d %d\r\n"\*/
			"m=video %s RTP/AVP %d\r\n"\
			"a=recvonly\r\n"\
			/*"a=rtpmap:%d PS/90000\r\n"\*/
			"a=rtpmap:%d PS/90000\r\n"\
			/*"a=username:%s\r\n"\*/
			/*"a=password:%s\r\n"\*/
			"y=%s\r\n",
			pMediaInfo->o_id, 
			pMediaInfo->media_recv_ip,
			s.c_str(),
			pMediaInfo->channel_id,
			/*"GB28181 DEVICE", */
			pMediaInfo->media_recv_ip,

			pMediaInfo->t_start_time,
			pMediaInfo->t_end_time,
			pMediaInfo->media_recv_port, 
			SIP_PS_H264, SIP_PS_H264, 
			pMediaInfo->ssrc);
	}
		
	eXosip_lock (context_eXosip);
	do{
		ret = eXosip_call_build_initial_invite (context_eXosip, &invite,msg.to_uri,   
			msg.from_uri,   
			NULL,   
			msg.subject);  

		if(ret!=OSIP_SUCCESS)
		{
			LOG4CPLUS_DEBUG(gb_Logger, "Sip_Call_play_playback_download eXosip_call_build_initial_invite failed");
			break;
		}

		if(isWithSDP)
		{
			ret = osip_message_set_body (invite, msg.body, strlen(msg.body));
			if(ret!=OSIP_SUCCESS)
			{
				LOG4CPLUS_DEBUG(gb_Logger, "Sip_Call_play_playback_download osip_message_set_body failed");
				if(invite != NULL)
				{
					osip_message_free(invite);
				}
				break;
			}

			ret = osip_message_set_content_type (invite, "application/sdp");
			if(ret!=OSIP_SUCCESS)
			{
				LOG4CPLUS_DEBUG(gb_Logger, "Sip_Call_play_playback_download osip_message_set_content_type failed");
				if(invite != NULL)
				{
					osip_message_free(invite);
				}
				break;
			}
		}
		ret = eXosip_call_send_initial_invite (context_eXosip, invite);  
	}while(0);
	
	eXosip_unlock (context_eXosip);

	return ret;
}

int Sip_Call_Ack(int did, int tid, sipd_media_session* pSdp)
{

	osip_message_t *ack = NULL;
	int ret ;
	int call_id; 

	ret = eXosip_call_build_ack(context_eXosip, did, &ack);  

	if(pSdp)
	{
		sipd_message_query_or_response  msg;
		memset(&msg, 0 , sizeof(sipd_message_query_or_response));

		snprintf(msg.body, MAX_SIPD_BODY_LENGTH,
			"v=0\r\n"\
			"o=%s 0 0 IN IP4 %s\r\n"\
			"s=%s\r\n"\
			"c=IN IP4 %s\r\n"\
			"t=0 0\r\n"\
			/*"m=video %d RTP/AVP %d %d\r\n"\*/
			"m=video %d RTP/AVP %d\r\n"\
			"a=recvonly\r\n"\
			"a=rtpmap:%d PS/90000\r\n"\
			"y=%s\r\n",
			pSdp->o_id, 
			pSdp->media_recv_ip,
			"GB28181 DEVICE", 
			pSdp->media_recv_ip,
			pSdp->media_recv_port, 
			SIP_PS_H264, SIP_PS_H264, 
			pSdp->ssrc);

		osip_message_set_body (ack, msg.body, strlen(msg.body));
		osip_message_set_content_type (ack, "application/sdp");  
	}


	eXosip_lock (context_eXosip);
	call_id = eXosip_call_send_ack(context_eXosip, did, ack);  
	eXosip_unlock (context_eXosip);

	return 0;
}



int Sip_Call_200_OK(int did, int tid, sipd_media_session* pSdp)
{
	int ret=0;
	osip_message_t *answer = NULL;

	eXosip_lock (context_eXosip);

	ret = eXosip_call_build_answer (context_eXosip, tid, 200, &answer);
	if (ret != OSIP_SUCCESS)
	{
		printf ("build_and_send_invite_200_ok This request msg is invalid!Cann't response! ERROR!!\n");
		
		eXosip_call_send_answer (context_eXosip, tid, 400, NULL);
	}
	else
	{
		if(pSdp)
		{

			sipd_message_query_or_response  msg;
			memset(&msg, 0 , sizeof(sipd_message_query_or_response));

			snprintf(msg.body, MAX_SIPD_BODY_LENGTH,
				"v=0\r\n"\
				"o=%s 0 0 IN IP4 %s\r\n"\
				"s=%s\r\n"\
				"c=IN IP4 %s\r\n"\
				"t=0 0\r\n"\
				/*"m=video %d RTP/AVP %d %d\r\n"\*/
				"m=video %d RTP/AVP %d\r\n"\
				"a=recvonly\r\n"\
				"a=rtpmap:%d PS/90000\r\n"\
				"y=%s\r\n",
				pSdp->o_id, 
				pSdp->media_recv_ip,
				"GB28181 DEVICE", 
				pSdp->media_recv_ip,
				pSdp->media_recv_port, 
				SIP_PS_H264, SIP_PS_H264, 
				pSdp->ssrc);

			osip_message_set_body (answer, msg.body, strlen(msg.body));
			osip_message_set_content_type (answer, "application/sdp");  
		}

		ret = eXosip_call_send_answer (context_eXosip, tid, 200, answer);
		if (ret != OSIP_SUCCESS)
		{
			printf ("build_and_send_invite_200_ok INVITE send 200 ERROR! [eXosip_call_send_answer] \n");
		}
	}

	eXosip_unlock (context_eXosip);
	//context_eXosip_unlock();
	return 0;
}


int Sip_Call_Bye(int cid, int did)
{
	osip_message_t *invite = NULL;
	int call_id; 

	eXosip_lock (context_eXosip);
	call_id = eXosip_call_terminate (context_eXosip, cid, did);  
	eXosip_unlock (context_eXosip);

	return 0;
}


int Sip_Call_media_control(int did, int tid, const enum SIPD_TO_CMS_CMD_TYPE cmd_type, 
						   const sipd_playback_mans_rtsp_ind* playback_rtsp_control)
{
	osip_message_t *message = NULL;
	int ret; 

	eXosip_lock(context_eXosip);
	ret = eXosip_call_build_info(context_eXosip, did, &message);

	if(ret == OSIP_SUCCESS)
	{
		ret = osip_message_set_body(message, playback_rtsp_control->rtsp_des, playback_rtsp_control->len);
		if(ret == OSIP_SUCCESS)
		{
			ret = osip_message_set_content_type (message, "Application/MANSRTSP");
			if(ret == OSIP_SUCCESS)
			{
				ret = eXosip_call_send_request(context_eXosip, did, message);
			}
			else
			{
				LOG4CPLUS_DEBUG(gb_Logger, "Sip_Call_media_control osip_message_set_content_type failed");
				if(message != NULL)
				{
					osip_message_free(message);
				}
			}
		}
		else
		{
			LOG4CPLUS_DEBUG(gb_Logger, "Sip_Call_media_control osip_message_set_body failed");
			if(message != NULL)
			{
				osip_message_free(message);
			}
		}
	}
	else
	{
		LOG4CPLUS_DEBUG(gb_Logger, "Sip_Call_media_control eXosip_call_build_info failed");
	}

	eXosip_unlock (context_eXosip);
	return ret;
}

//////////////////////////////////////////////////////////////////////////

static int get_media_session_priv(char* ori_str, struct sipd_invite_media_session* media_session)
{
	char* p=NULL;
	char* p_locator=NULL;
	char* p_locator_end=NULL;
	char temp_buffer[MAX_SIPD_NORMAL_LEN] = {0};

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
				int len = (p_locator_end - (p_locator+2));
				if(len>=MAX_SIPD_NORMAL_LEN)
					len = MAX_SIPD_NORMAL_LEN -1;
				strncpy_s(media_session->pri_ssrc_field, p_locator+2, len); 
			}
			else
			{
				//printf("%s line=%d can't find CLr, Noticing in y=! ERROR!! try to find CR...\n", __FUNCTION__, __LINE__);
				p_locator_end = strchr(p_locator, '\n');
				if(NULL != p_locator_end)
				{	
					//printf("%s line=%d (p_locator_end - (p_locator+2))=%d\n", __FUNCTION__, __LINE__, (p_locator_end - (p_locator+2)));
					strncpy_s(media_session->pri_ssrc_field, p_locator+2, (p_locator_end - (p_locator+2))); /*skip first digital, real time or history*/
					//printf("%s line=%d find CR pri_ssrc_field=%s\n", __FUNCTION__, __LINE__, media_session->pri_ssrc_field);
				}
				else
				{
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
				strncpy_s(media_session->pri_ssrc_audio_field, p_locator+2, (p_locator_end - (p_locator+2))); 
			}
			else
			{
				//printf("%s line=%d can't find CLr, Noticing in y=! ERROR!! try to find CR...\n", __FUNCTION__, __LINE__);
				p_locator_end = strchr(p_locator, '\n');
				if(NULL != p_locator_end)
				{	
					//printf("%s line=%d (p_locator_end - (p_locator+2))=%d\n", __FUNCTION__, __LINE__, (p_locator_end - (p_locator+2)));
					strncpy_s(media_session->pri_ssrc_audio_field, p_locator+2, (p_locator_end - (p_locator+2))); 
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
			strncpy_s(media_session->pri_f_field, p_locator+2, (p_locator_end - (p_locator+2))); 
		}
		else
		{
			//printf("%s line=%d can't find CLr in f=, Noticing! ERROR. try to find CR...\n", __FUNCTION__, __LINE__);
			p_locator_end = strchr(p_locator, '\n');
			if(NULL != p_locator_end)
			{	
				//printf("%s line=%d (p_locator_end - (p_locator+2))=%d\n", __FUNCTION__, __LINE__, (p_locator_end - (p_locator+2)));
				strncpy_s(media_session->pri_f_field, p_locator+2, (p_locator_end - (p_locator+2))); 
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
					media_session->f_video_type = (SIPD_MEDIA_F_VIDEO_TYPE)0;
				}
				else
				{
					strncpy_s(temp_buffer, p_locator+2, pos-(p_locator+2));
					//printf("%s line=%d video coding type=%s strlen(temp_buffer)=%d\n", __FUNCTION__, __LINE__, temp_buffer, strlen(temp_buffer));
					if(strlen(temp_buffer) > 0)
					{
						media_session->f_video_type = (SIPD_MEDIA_F_VIDEO_TYPE)atoi(temp_buffer);
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
				media_session->f_resolution = (SIPD_MEDIA_F_VIDEO_RESOLUTION)0;
				pos2 = pos+1;
			}
			else
			{
				pos2 = strchr(pos+1, '/');
				if(pos2 != NULL)
				{
					memset(temp_buffer, 0, sizeof(temp_buffer));
					strncpy_s(temp_buffer, pos+1, pos2 -(pos+1));
					//printf("%s line=%d video resolution=%s strlen(temp_buffer)=%d\n", __FUNCTION__, __LINE__, temp_buffer, strlen(temp_buffer));
					if(strlen(temp_buffer) > 0)
					{
						media_session->f_resolution = (SIPD_MEDIA_F_VIDEO_RESOLUTION)atoi(temp_buffer);
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
					strncpy_s(temp_buffer, pos2+1, pos - (pos2+1));
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
				media_session->f_rate_type = (SIPD_MEDIA_F_VIDEO_RATE_TYPE)0;
				pos2 = pos+1;
			}
			else
			{
				pos2 = strchr(pos+1, '/');
				if(pos2 != NULL)
				{
					memset(temp_buffer, 0, sizeof(temp_buffer));
					strncpy_s(temp_buffer, pos+1, pos2 -(pos+1));
					//printf("%s line=%d video rate_type=%s strlen(temp_buffer)=%d\n", __FUNCTION__, __LINE__, temp_buffer, strlen(temp_buffer));
					if(strlen(temp_buffer) > 0)
					{
						media_session->f_rate_type = (SIPD_MEDIA_F_VIDEO_RATE_TYPE)atoi(temp_buffer);
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
					strncpy_s(temp_buffer, pos2+1, pos-(pos2+1));
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
				media_session->audio_type = (SIPD_MEDIA_F_AUDIO_TYPE)0;
				pos = p_locator_end+2;
			}
			else
			{
				pos = strchr(p_locator_end+2, '/');
				if(pos != NULL)
				{
					memset(temp_buffer, 0, sizeof(temp_buffer));
					strncpy_s(temp_buffer, p_locator_end+2, pos-(p_locator_end+2));
					//printf("%s line=%d audio coding_type=%s strlen(temp_buffer)=%d\n", __FUNCTION__, __LINE__, temp_buffer, strlen(temp_buffer));
					if(strlen(temp_buffer) > 0)
					{
						media_session->audio_type = (SIPD_MEDIA_F_AUDIO_TYPE)atoi(temp_buffer);
						//printf("%s line=%d audio audio_type=%d\n", __FUNCTION__, __LINE__, media_session->audio_type);
					}
				}
				else
				{
					media_session->f_enable = 0;
					goto F_FORMAT_ERROR;
				}
			}


			/*audio bitrate*/
			if(*(pos+1) == '/')
			{
				//printf("%s line=%d audio birate is empty\n", __FUNCTION__, __LINE__);
				media_session->audio_bitrate = (SIPD_MEDIA_F_AUDIO_BITRATE)0;
				pos2 = pos+1;
			}
			else
			{
				pos2 = strchr(pos, '/');
				if(pos2 != NULL)
				{
					memset(temp_buffer, 0, sizeof(temp_buffer));
					strncpy_s(temp_buffer, pos+1, pos2-(pos+1));
					//printf("%s line=%d audio birate=%s strlen(temp_buffer)=%d\n", __FUNCTION__, __LINE__, temp_buffer, strlen(temp_buffer));
					if(strlen(temp_buffer) > 0)
					{
						media_session->audio_bitrate = (SIPD_MEDIA_F_AUDIO_BITRATE)atoi(temp_buffer);
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
				media_session->audio_samplerate =(SIPD_MEDIA_F_AUDIO_SAMPLE_RATE) 0;
			}
			else
			{
				memset(temp_buffer, 0, sizeof(temp_buffer));
				strncpy_s(temp_buffer, pos2+1, MAX_SIPD_NORMAL_LEN);
				//printf("%s line=%d audio sample rate=%s strlen(temp_buffer)=%d\n", __FUNCTION__, __LINE__, temp_buffer, strlen(temp_buffer));
				if(strlen(temp_buffer) > 0)
				{
					media_session->audio_samplerate = (SIPD_MEDIA_F_AUDIO_SAMPLE_RATE)atoi(temp_buffer);
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
		strncpy_s(media_session->connect_video_ip, video_connection->c_addr, MAX_SIPD_URI);
		strncpy_s(media_session->connect_video_addrtype, video_connection->c_addrtype, MAX_SIPD_NORMAL_LEN);
	}

	audio_connection = eXosip_get_audio_connection(remote_sdp);
	if(NULL == audio_connection)
	{
		//printf("%s line=%d eXosip_get_audio_connection failed, don't find audio connection in remote sdp \n", __FUNCTION__, __LINE__);
	}
	else
	{
		strncpy_s(media_session->connect_audio_ip, audio_connection->c_addr, MAX_SIPD_URI);
		strncpy_s(media_session->connect_audio_addrtype, audio_connection->c_addrtype, MAX_SIPD_NORMAL_LEN);
	}


	video_media = eXosip_get_video_media(remote_sdp);
	if(NULL == video_media)
	{
		printf("%s line=%d eXosip_get_video_media failed, cann't find any video in remote sdp \n", __FUNCTION__, __LINE__);
	}
	else
	{
		strncpy_s(media_session->connect_video_port, video_media->m_port, MAX_SIPD_NORMAL_LEN);
	}

	audio_media = eXosip_get_audio_media(remote_sdp);
	if(NULL == audio_media)
	{
		//printf("%s line=%d eXosip_get_audio_media failed, cann't find any audio in remote sdp \n", __FUNCTION__, __LINE__);
	}
	else
	{
		strncpy_s(media_session->connect_audio_port, audio_media->m_port, MAX_SIPD_NORMAL_LEN);
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
		strncpy_s(media_session->o_field_username, o_username, MAX_SIPD_NORMAL_LEN);
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
		if(strcmp(session_name, "Play") == 0)
		{
			media_session->stream_type = SIPD_STREAM_TYPE_PLAY;
		}
		else if(strcmp(session_name, "Playback") == 0)
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
				strncpy_s(media_session->u_field, uri_sdp, MAX_SIPD_URI);
			}

			start_time = sdp_message_t_start_time_get(remote_sdp, 0);
			if(NULL == start_time)
			{
				//printf("%s line=%d Playback, start_time is NULL  (get sdp_message_t_start_time_get failed)\n", __FUNCTION__, __LINE__);
			}
			else
			{
				//printf("%s line=%d Playback, start_time=%s \n", __FUNCTION__, __LINE__, start_time);
				strncpy_s(media_session->t_start_time, start_time, MAX_SIPD_NORMAL_LEN);
			}

			end_time = sdp_message_t_stop_time_get(remote_sdp, 0);
			if(NULL == end_time)
			{
				//printf("%s line=%d Playback, end_time is NULL  (get sdp_message_t_stop_time_get failed)\n", __FUNCTION__, __LINE__);
			}
			else
			{
				//printf("%s line=%d Playback, end_time=%s \n", __FUNCTION__, __LINE__, end_time);
				strncpy_s(media_session->t_end_time, end_time, MAX_SIPD_NORMAL_LEN);
			}

		}
		else if(strcmp(session_name, "Download") == 0)
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
				strncpy_s(media_session->u_field, uri_sdp, MAX_SIPD_URI);
			}

			start_time = sdp_message_t_start_time_get(remote_sdp, 0);
			if(NULL == start_time)
			{
				//printf("%s line=%d Download, start_time is NULL  (get sdp_message_t_start_time_get failed)\n", __FUNCTION__, __LINE__);
			}
			else
			{
				//printf("%s line=%d Download, start_time=%s \n", __FUNCTION__, __LINE__, start_time);
				strncpy_s(media_session->t_start_time, start_time, MAX_SIPD_NORMAL_LEN);
			}

			end_time = sdp_message_t_stop_time_get(remote_sdp, 0);
			if(NULL == end_time)
			{
				//printf("%s line=%d Download, end_time is NULL  (get sdp_message_t_stop_time_get failed)\n", __FUNCTION__, __LINE__);
			}
			else
			{
				//printf("%s line=%d Download, end_time=%s \n", __FUNCTION__, __LINE__, end_time);
				strncpy_s(media_session->t_end_time, end_time, MAX_SIPD_NORMAL_LEN);
			}
		}
		else
		{
			media_session->stream_type = SIPD_STREAM_TYPE_PLAY;
		}
	}

	return 0;
}

int Invite_adaptor_ind(sipd_media_session * invite_adaptor, const struct sipd_invite_media_session* pSdpSession)
{
	//sipd_media_session invite_adaptor;
	const char* p=NULL;
	int len=0;
	int play_type=0;

	// printf("Invite_adaptor_ind \n");
	memset(invite_adaptor, 0, sizeof(sipd_media_session));

	strncpy_s(invite_adaptor->o_id, pSdpSession->o_field_username, MAX_SIPD_DEVICE_ID_LEN);

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
		strncpy_s(invite_adaptor->channel_id, pSdpSession->u_field, len);

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
		strncpy_s(invite_adaptor->channel_id, pSdpSession->u_field, MAX_SIPD_DEVICE_ID_LEN);
		invite_adaptor->Type = SIPD_RECORD_INFO_TYPE_ALL;
		//printf("Invite_adaptor_ind channel_id=%s  Type=%d u_field=%s \n", invite_adaptor.channel_id, invite_adaptor.Type, pSdpSession->u_field);
	}

	invite_adaptor->stream_type = pSdpSession->stream_type;
	invite_adaptor->media_recv_ip_type = SIPD_IPType_IPv4;
	strncpy_s(invite_adaptor->media_recv_ip, pSdpSession->connect_video_ip, MAX_SIPD_URI);
	strncpy_s(invite_adaptor->media_recv_port, pSdpSession->connect_video_port, MAX_SIPD_NORMAL_LEN);
	invite_adaptor->media_type = pSdpSession->media_type;
	strncpy_s(invite_adaptor->t_start_time, pSdpSession->t_start_time, MAX_SIPD_NORMAL_LEN);
	strncpy_s(invite_adaptor->t_end_time, pSdpSession->t_end_time, MAX_SIPD_NORMAL_LEN);
	strncpy_s(invite_adaptor->ssrc, pSdpSession->pri_ssrc_field, MAX_SIPD_NORMAL_LEN);

	invite_adaptor->f_video_type = pSdpSession->f_video_type;
	invite_adaptor->f_resolution = pSdpSession->f_resolution;
	invite_adaptor->f_framerate = pSdpSession->f_framerate;
	invite_adaptor->f_rate_type = pSdpSession->f_rate_type;
	invite_adaptor->bitrate = pSdpSession->bitrate;
	invite_adaptor->audio_type = pSdpSession->audio_type;
	invite_adaptor->audio_bitrate = pSdpSession->audio_bitrate;
	invite_adaptor->audio_samplerate = pSdpSession->audio_samplerate;

	return 0;
}


bool handle_call_answer(const eXosip_event_t  *je)
{
	sdp_message_t *remote_sdp = NULL;
	int ret;
	eXosip_lock (context_eXosip);
	remote_sdp = eXosip_get_remote_sdp (context_eXosip, je->did);
	eXosip_unlock (context_eXosip);

	if(remote_sdp != NULL)
	{
		// 200 OK 中带有 SDP
		struct sipd_invite_media_session  invite_media_session;
		sipd_media_session invite_adaptor;
		osip_body_t* body = NULL;
		memset(&invite_media_session, 0, sizeof(invite_media_session));

		get_media_session(remote_sdp, &invite_media_session);
		
		ret = osip_message_get_body(je->response, 0, &body);
		if (ret ==OSIP_SUCCESS) 
		{
			//	解析 sdp 中的 “y” 域
			get_media_session_priv(body->body, &invite_media_session);
		}

		sdp_message_free (remote_sdp);
		remote_sdp = NULL;

		Invite_adaptor_ind(&invite_adaptor, &invite_media_session);

		if(pSipd_Call_200_OK_resp)
			pSipd_Call_200_OK_resp(je->cid, je->did, je->tid, &invite_adaptor);
	}
	else
	{
		// 不带 SDP 信息
		sipd_media_session invite_adaptor;
		memset(&invite_adaptor, 0, sizeof(sipd_media_session));

		/*invite_adaptor.cid = je->cid;
		invite_adaptor.did = je->did;
		invite_adaptor.tid = je->tid;*/

		if(pSipd_Call_200_OK_resp)
			pSipd_Call_200_OK_resp(je->cid, je->did, je->tid, &invite_adaptor);
	}

	return true;
}


bool handle_call_noanswer(const eXosip_event_t  *je)
{
	//eXosip_lock (context_eXosip);


	if(pSipd_Call_No_Answer)
		pSipd_Call_No_Answer(je->cid, je->did, je->tid, NULL);

	//eXosip_unlock (context_eXosip);

	return true;
}

bool handle_file_to_end_notify(mxml_node_t *tree, mxml_node_t *notify_node, sipd_message_query_or_response  * pReq)
{
	mxml_node_t    * CmdType_node, * NotifyType_node;

	CmdType_node = mxmlFindElement(notify_node, tree, "CmdType", NULL, NULL,MXML_DESCEND);
	if(CmdType_node != NULL && mxmlGetText(CmdType_node, NULL))
	{
		char strbuf[MAX_SIPD_URI];
		memset(strbuf, 0, sizeof(strbuf));
		strncpy_s(strbuf,  (const char *)mxmlGetText(CmdType_node, NULL), sizeof(strbuf));
		if(strcmp(strbuf, "MediaStatus") != 0)
			return false;
	}

	NotifyType_node = mxmlFindElement(notify_node, tree, "NotifyType", NULL, NULL,MXML_DESCEND);
	if(NotifyType_node != NULL &&  mxmlGetText(NotifyType_node, NULL))
	{
		char strbuf[MAX_SIPD_URI];
		memset(strbuf, 0, sizeof(strbuf));
		strncpy_s(strbuf,  (const char *)mxmlGetText(NotifyType_node, NULL), sizeof(strbuf));
		if(strcmp(strbuf, "121") != 0)
			return false;
	}

	if(pSipd_Call_File_to_end)
		pSipd_Call_File_to_end(pReq->cid, pReq->did, pReq->tid, NULL);

	return true;
}