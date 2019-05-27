
#include "stdafx.h"
#include "Gb28181_Layer.h"

#include <eXosip.h>
#include <osip_headers.h>
#include <osip_mt.h>

bool parse_deviceId_and_SN(mxml_node_t *tree, mxml_node_t *query_node, string & deviceId, string & SN);



//////////////////////////////////////////////////////////////////////////

extern Sipd_Alarm_Notify_func	pSipd_Alarm_Notify;

Sipd_subscribe_200_ok_func	pSipd_subscribe_200_OK;

void Register_Sipd_subscribe_200_OK(Sipd_subscribe_200_ok_func pfunc)
{
	pSipd_subscribe_200_OK = pfunc;
}

/* 设备报警设备订阅 */
int Sip_Alarm_Subscribe(const sipd_alarm_req * pAlarmReq)
{
	int ret = OSIP_UNDEFINED_ERROR;

	char buffer[MAX_SIPD_RECORD_INFO_LEN+1] = {0};
	memset(buffer, 0, sizeof(buffer));

	snprintf(buffer,MAX_SIPD_RECORD_INFO_LEN,
		"<?xml version=\"1.0\"?>\r\n"\
		"<Query>\r\n"\
		"<CmdType>Alarm</CmdType>\r\n"\
		"<SN>%s</SN>\r\n"\
		"<DeviceID>%s</DeviceID>\r\n"\
		"<StartAlarmPriority>%d</StartAlarmPriority>\r\n"\
		"<EndAlarmPriority>%d</EndAlarmPriority>\r\n"\
		"<AlarmMethod>%d</AlarmMethod>\r\n"\
		"<StartTime>%s</StartTime>\r\n"\
		"<EndTime>%s</EndTime>\r\n"\
		"</Query>",
		pAlarmReq->SN, pAlarmReq->device_id, pAlarmReq->StartAlarmPriority, pAlarmReq->EndAlarmPriority, \
		pAlarmReq->AlarmMethod, pAlarmReq->StartTime, pAlarmReq->EndTime );

	if(/*Get_EvWaitFlg() ==*/ 1)
	{
		osip_message_t *message = NULL;

		eXosip_lock(context_eXosip);
		ret = eXosip_subscribe_build_initial_request(context_eXosip, &message, \
			pAlarmReq->to_uri, pAlarmReq->from_uri, NULL, "presence", pAlarmReq->expires);
		if(ret != OSIP_SUCCESS)
		{
			eXosip_unlock(context_eXosip);
			LOG4CPLUS_DEBUG(gb_Logger, "Sip_Alarm_Subscribe eXosip_subscribe_build_initial_request failed");
			if(message != NULL)
			{
				osip_message_free(message);
			}
			//context_eXosip_unlock();
			return -1;
		}

		ret = osip_message_set_body(message, buffer, strlen(buffer));
		if(ret != OSIP_SUCCESS)
		{
			eXosip_unlock(context_eXosip);
			LOG4CPLUS_DEBUG(gb_Logger, "Sip_Alarm_Subscribe osip_message_set_body failed");
			if(message != NULL)
			{
				osip_message_free(message);
			}
			//context_eXosip_unlock();
			return -1;
		}

		ret = osip_message_set_content_type (message, "Application/MANSCDP+xml");
		if(ret != OSIP_SUCCESS)
		{
			eXosip_unlock(context_eXosip);
			LOG4CPLUS_DEBUG(gb_Logger, "Sip_Alarm_Subscribe osip_message_set_content_type failed");
			if(message != NULL)
			{
				osip_message_free(message);
			}
			//context_eXosip_unlock();
			return -1;
		}

		ret = eXosip_subscribe_send_initial_request(context_eXosip, message);
		eXosip_unlock(context_eXosip);
	}

	/*返回 sid >0*/
	stringstream ss;
	ss << "Sip_Alarm_Subscribe, SN = "<< string(pAlarmReq->SN) << ", sid = " << ret;
	LOG4CPLUS_DEBUG(gb_Logger,ss.str());

	return ret;

}



bool handle_subscribe_200_ok(const eXosip_event_t  *je)
{

	osip_message_t * sip_request = NULL;

	osip_body_t *body = NULL;
	osip_content_type_t* content_type=NULL;
	char buffer[MAX_XML_BUFFER_LEN+1]={0};

	mxml_node_t *tree,   *response_node,  * CmdType_node, * node;

	sip_request = je->request;
	if(NULL == sip_request)
	{
		return false;
	}

	content_type = osip_message_get_content_type(sip_request);
	if(NULL == content_type)
	{
		return false;
	}

	
	if((strcasecmp(content_type->type, "Application" ) == 0) && (strcasecmp(content_type->subtype, "MANSCDP+xml") == 0))
	{
		/*Application/MANSCDP+xml*/
		osip_message_get_body (sip_request, 0, &body); 

		/*allocate xml space*/
		memset(buffer, 0, sizeof(buffer));
	
		//	buffer 保存 xml 的内容
		strncpy_s(buffer, body->body, MAX_XML_BUFFER_LEN);

		tree = mxmlLoadString(NULL, buffer,  MXML_TEXT_CALLBACK);

		if(tree != NULL)
		{
			sipd_subscribe_200_ok_info	basic_info;
			memset(&basic_info, 0, sizeof(sipd_subscribe_200_ok_info));

			if(sip_request->to->url->port != NULL)
			{
				snprintf(basic_info.to_uri, sizeof(basic_info.to_uri), "sip:%s@%s:%s", sip_request->to->url->username,
					sip_request->to->url->host, sip_request->to->url->port);
			}
			else
			{
				snprintf(basic_info.to_uri, sizeof(basic_info.to_uri), "sip:%s@%s", sip_request->to->url->username,
					sip_request->to->url->host);
			}

			if(sip_request->from->url->port != NULL)
			{
				snprintf(basic_info.from_uri, sizeof(basic_info.from_uri), "sip:%s@%s:%s", sip_request->from->url->username,
					sip_request->from->url->host, sip_request->from->url->port);
			}
			else
			{
				snprintf(basic_info.from_uri, sizeof(basic_info.from_uri), "sip:%s@%s", sip_request->from->url->username,
					sip_request->from->url->host);
			}

			response_node = mxmlFindElement(tree, tree, "Response", NULL, NULL,MXML_DESCEND);

			CmdType_node = mxmlFindElement(response_node, tree, "CmdType", NULL, NULL,MXML_DESCEND);
			if(CmdType_node != NULL && mxmlGetText(CmdType_node, NULL))
			{
				char strbuf[MAX_SIPD_URI];
				memset(strbuf, 0, sizeof(strbuf));
				strncpy_s(strbuf,  (const char *)mxmlGetText(CmdType_node, NULL), sizeof(strbuf));
				if(strcmp(strbuf, "Alarm") != 0)
					return false;
			}

			string strDeviceId;
			string strSN;
			if(parse_deviceId_and_SN(tree, response_node, strDeviceId, strSN)==false)
				return false;
			
			strncpy_s(basic_info.SN, strSN.c_str(), MAX_SIPD_DEVICE_ID_LEN);
			strncpy_s(basic_info.device_id, strDeviceId.c_str(), MAX_SIPD_DEVICE_ID_LEN);

			node = mxmlFindElement(response_node, tree, "Result", NULL, NULL, MXML_DESCEND);
			if(node && mxmlGetText(node, NULL))
			{
				char strResult[MAX_SIPD_NORMAL_LEN];
				memset(strResult, 0 ,MAX_SIPD_NORMAL_LEN);
				strncpy_s(strResult, (const char *)mxmlGetText(node, NULL),MAX_SIPD_NORMAL_LEN );
				if(strcmp(strResult, "OK")==0)
					basic_info.Result = SIPD_RESULT_OK;
				else
					basic_info.Result = SIPD_RESULT_ERROR;
			}

			if(pSipd_subscribe_200_OK)
				pSipd_subscribe_200_OK(&basic_info, je->sid);

			return true;
		}
	}
	return false;
}



bool handle_subscribe_400_bad(const eXosip_event_t  *je)
{

	osip_message_t * sip_request = NULL;
	sip_request = je->request;
	if(NULL == sip_request)
	{
		return false;
	}

	sipd_subscribe_200_ok_info	basic_info;
	memset(&basic_info, 0, sizeof(sipd_subscribe_200_ok_info));

	if(sip_request->to->url->port != NULL)
	{
		snprintf(basic_info.to_uri, sizeof(basic_info.to_uri), "sip:%s@%s:%s", sip_request->to->url->username,
			sip_request->to->url->host, sip_request->to->url->port);
	}
	else
	{
		snprintf(basic_info.to_uri, sizeof(basic_info.to_uri), "sip:%s@%s", sip_request->to->url->username,
			sip_request->to->url->host);
	}

	if(sip_request->from->url->port != NULL)
	{
		snprintf(basic_info.from_uri, sizeof(basic_info.from_uri), "sip:%s@%s:%s", sip_request->from->url->username,
			sip_request->from->url->host, sip_request->from->url->port);
	}
	else
	{
		snprintf(basic_info.from_uri, sizeof(basic_info.from_uri), "sip:%s@%s", sip_request->from->url->username,
			sip_request->from->url->host);
	}
	basic_info.Result = SIPD_RESULT_ERROR;

	if(pSipd_subscribe_200_OK)
		pSipd_subscribe_200_OK(&basic_info, je->sid);

	return true;
}



/* 回复报警事件通知 */
int Sip_Alarm_Notify_200_OK(const sipd_alarm_notify	 * pAlarmReq, int tid)
{
	int ret = OSIP_UNDEFINED_ERROR;

	char buffer[MAX_SIPD_RECORD_INFO_LEN+1] = {0};
	memset(buffer, 0, sizeof(buffer));

	snprintf(buffer,MAX_SIPD_RECORD_INFO_LEN,
		"<?xml version=\"1.0\"?>\r\n"\
		"<Response>\r\n"\
		"<CmdType>Alarm</CmdType>\r\n"\
		"<SN>%s</SN>\r\n"\
		"<DeviceID>%s</DeviceID>\r\n"\
		"<Result>OK</Result>\r\n"\
		"</Response>",
		pAlarmReq->SN, pAlarmReq->device_id );

	if(/*Get_EvWaitFlg() ==*/ 1)
	{
		osip_message_t *answer = NULL;

		eXosip_lock(context_eXosip);
		
		ret = eXosip_message_build_answer (context_eXosip,tid, 200,&answer);
		if (ret !=OSIP_SUCCESS)
		{
			//printf("%s:%d eXosip_message_build_answer [EXOSIP_MESSAGE_NEW Message] failed\n", __FUNCTION__, __LINE__);
			stringstream ss;
			ss << "Sip_Alarm_Notify_200_OK, eXosip_message_build_answer failed"
				<< ", tid = " << tid;
			LOG4CPLUS_DEBUG(gb_Logger, ss.str());
		}
		else
		{

			ret = osip_message_set_body(answer, buffer, strlen(buffer));
			if(ret != OSIP_SUCCESS)
			{
				eXosip_unlock(context_eXosip);
				LOG4CPLUS_DEBUG(gb_Logger, "Sip_Alarm_Notify_200_OK osip_message_set_body failed");
				if(answer != NULL)
				{
					osip_message_free(answer);
				}
				//context_eXosip_unlock();
				return -1;
			}

			ret = osip_message_set_content_type (answer, "Application/MANSCDP+xml");
			if(ret != OSIP_SUCCESS)
			{
				eXosip_unlock(context_eXosip);
				LOG4CPLUS_DEBUG(gb_Logger, "Sip_Alarm_Notify_200_OK osip_message_set_content_type failed");
				if(answer != NULL)
				{
					osip_message_free(answer);
				}
				//context_eXosip_unlock();
				return -1;
			}

			ret = eXosip_message_send_answer (context_eXosip,tid, 200,answer);
			if (ret !=OSIP_SUCCESS)
			{
				//printf("%s:%d eXosip_message_send_answer [EXOSIP_MESSAGE_NEW Message] failed\n", __FUNCTION__, __LINE__);
				stringstream ss;
				ss << "Sip_Alarm_Notify_200_OK, eXosip_message_send_answer failed"
					<< ", tid = " << tid;
				LOG4CPLUS_DEBUG(gb_Logger, ss.str());
			}
		}
	
		eXosip_unlock(context_eXosip);
	}

	/*返回 sid >0*/
	stringstream ss;
	ss << "Sip_Alarm_Notify_200_OK, SN = "<< string(pAlarmReq->SN) << ", sid = " << ret;
	LOG4CPLUS_DEBUG(gb_Logger,ss.str());

	return ret;

}

/* 处理报警事件通知 */
bool handle_alarm_subscribe_notify(const eXosip_event_t  *je)
{

	osip_message_t * sip_request = NULL;

	osip_body_t *body = NULL;
	osip_content_type_t* content_type=NULL;
	char buffer[MAX_XML_BUFFER_LEN+1]={0};

	mxml_node_t *tree,   *notify_node,  * CmdType_node, * node;

	sip_request = je->request;
	if(NULL == sip_request)
	{
		return false;
	}

	content_type = osip_message_get_content_type(sip_request);
	if(NULL == content_type)
	{
		return false;
	}


	if((strcasecmp(content_type->type, "Application" ) == 0) && (strcasecmp(content_type->subtype, "MANSCDP+xml") == 0))
	{
		/*Application/MANSCDP+xml*/
		osip_message_get_body (sip_request, 0, &body); 

		/*allocate xml space*/
		memset(buffer, 0, sizeof(buffer));

		//	buffer 保存 xml 的内容
		strncpy_s(buffer, body->body, MAX_XML_BUFFER_LEN);

		tree = mxmlLoadString(NULL, buffer,  MXML_TEXT_CALLBACK);

		if(tree != NULL)
		{
			sipd_alarm_notify	basic_info;
			memset(&basic_info, 0, sizeof(sipd_subscribe_200_ok_info));

			if(sip_request->to->url->port != NULL)
			{
				snprintf(basic_info.to_uri, sizeof(basic_info.to_uri), "sip:%s@%s:%s", sip_request->to->url->username,
					sip_request->to->url->host, sip_request->to->url->port);
			}
			else
			{
				snprintf(basic_info.to_uri, sizeof(basic_info.to_uri), "sip:%s@%s", sip_request->to->url->username,
					sip_request->to->url->host);
			}

			if(sip_request->from->url->port != NULL)
			{
				snprintf(basic_info.from_uri, sizeof(basic_info.from_uri), "sip:%s@%s:%s", sip_request->from->url->username,
					sip_request->from->url->host, sip_request->from->url->port);
			}
			else
			{
				snprintf(basic_info.from_uri, sizeof(basic_info.from_uri), "sip:%s@%s", sip_request->from->url->username,
					sip_request->from->url->host);
			}

			notify_node = mxmlFindElement(tree, tree, "Notify", NULL, NULL,MXML_DESCEND);

			CmdType_node = mxmlFindElement(notify_node, tree, "CmdType", NULL, NULL,MXML_DESCEND);
			if(CmdType_node != NULL && mxmlGetText(CmdType_node, NULL))
			{
				char strbuf[MAX_SIPD_URI];
				memset(strbuf, 0, sizeof(strbuf));
				strncpy_s(strbuf,  (const char *)mxmlGetText(CmdType_node, NULL), sizeof(strbuf));
				if(strcmp(strbuf, "Alarm") != 0)
					return false;
			}

			string strDeviceId;
			string strSN;
			if(parse_deviceId_and_SN(tree, notify_node, strDeviceId, strSN)==false)
				return false;

			strncpy_s(basic_info.SN, strSN.c_str(), MAX_SIPD_DEVICE_ID_LEN);
			strncpy_s(basic_info.device_id, strDeviceId.c_str(), MAX_SIPD_DEVICE_ID_LEN);

			node = mxmlFindElement(notify_node, tree, "AlarmTime", NULL, NULL,MXML_DESCEND);
			if(node)
			{
				memset(basic_info.AlarmTime , 0 ,MAX_SIPD_TIME_LEN + 1);
				strncpy_s(basic_info.AlarmTime, (const char *)mxmlGetText(node, NULL), MAX_SIPD_TIME_LEN );
			}

			node = mxmlFindElement(notify_node, tree, "AlarmMethod", NULL, NULL,MXML_DESCEND);
			if(node)
			{
				char strResult[MAX_SIPD_NORMAL_LEN];
				memset(strResult, 0 ,MAX_SIPD_NORMAL_LEN);
				strncpy_s(strResult, (const char *)mxmlGetText(node, NULL),MAX_SIPD_NORMAL_LEN );
				basic_info.AlarmMethod = (enum sipd_alarm_condition)atoi(strResult);
			}

			node = mxmlFindElement(notify_node, tree, "AlarmPriority", NULL, NULL,MXML_DESCEND);
			if(node)
			{
				char strResult[MAX_SIPD_NORMAL_LEN];
				memset(strResult, 0 ,MAX_SIPD_NORMAL_LEN);
				strncpy_s(strResult, (const char *)mxmlGetText(node, NULL),MAX_SIPD_NORMAL_LEN );
				basic_info.AlarmPriority = (enum sipd_alarm_level)atoi(strResult);
			}

			if(pSipd_Alarm_Notify)
				pSipd_Alarm_Notify(&basic_info);

			Sip_Alarm_Notify_200_OK(&basic_info, je->tid);

			return true;
		}
	}
	return false;
}