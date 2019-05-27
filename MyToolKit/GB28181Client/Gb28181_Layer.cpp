
#include "stdafx.h"
#include "Gb28181_Layer.h"

#include <WinSock2.h>
#include <Windows.h>
#include <process.h>

#include <eXosip.h>
#include <osip_headers.h>
#include <osip_mt.h>


struct eXosip_t *context_eXosip = NULL;

SIPD_GATEWAY_CFG g_Cfg;

Logger	gb_Logger;

void handle_register(const eXosip_event_t  *je);
bool handle_catalog_response(mxml_node_t *tree, mxml_node_t *response_node, sipd_message_query_or_response  * pReq);
bool handle_deviceinfo_response(mxml_node_t *tree, mxml_node_t *response_node, sipd_message_query_or_response  * pReq);
bool handle_stateinfo_response(mxml_node_t *tree, mxml_node_t *response_node, sipd_message_query_or_response  * pReq);
bool handle_search_recordfile_response(mxml_node_t *tree, mxml_node_t *response_node, sipd_message_query_or_response  * pReq);

bool handle_alarm_notify(mxml_node_t *tree, mxml_node_t *notify_node, sipd_message_query_or_response  * pReq);
bool handle_file_to_end_notify(mxml_node_t *tree, mxml_node_t *notify_node, sipd_message_query_or_response  * pReq);
bool handle_keepalive(int tid, mxml_node_t *tree, mxml_node_t *notify_node, sipd_message_query_or_response  * pReq);

bool handle_call_noanswer(const eXosip_event_t  *je);
bool handle_call_answer(const eXosip_event_t  *je);

bool handle_subscribe_200_ok(const eXosip_event_t  *je);
bool handle_subscribe_400_bad(const eXosip_event_t  *je);
bool handle_alarm_subscribe_notify(const eXosip_event_t  *je);

//////////////////////////////////////////////////////////////////////////

bool parse_deviceId_and_SN(mxml_node_t *tree, mxml_node_t *query_node, string & deviceId, string & SN)
{
	mxml_node_t *DeviceID_node , *sn_node;
	char id[MAX_SIPD_DEVICE_ID_LEN+1];
	char snbuf[MAX_SIPD_DEVICE_ID_LEN+1];

	DeviceID_node = mxmlFindElement(query_node, tree, "DeviceID", NULL, NULL,MXML_DESCEND);
	sn_node = mxmlFindElement(query_node, tree, "SN", NULL, NULL, MXML_DESCEND);

	if(DeviceID_node!=NULL  && mxmlGetText(DeviceID_node, NULL))
	{
		memset(id, 0, sizeof(id));
		strncpy_s(id, (const char *)mxmlGetText(DeviceID_node, NULL), sizeof(id));
		deviceId = string(id);
	}
	else
		return false;

	if(sn_node!=NULL  && mxmlGetText(sn_node, NULL))
	{
		memset(snbuf, 0, sizeof(snbuf));
		strncpy_s(snbuf, (const char *)mxmlGetText(sn_node, NULL), sizeof(snbuf));
		SN = string(snbuf);
	}
	else 
		return false;

	return true;
}



/*注册 Message 消息的回复*/
Sipd_Message_Response_func	pSipd_Message_200_OK_Resp;
Sipd_Message_Response_func	pSipd_Message_Failure_Resp;

void Register_Sipd_Message_200_Ok(Sipd_Message_Response_func pfunc)
{
	pSipd_Message_200_OK_Resp = pfunc;
}

void Register_Sipd_Message_Failure(Sipd_Message_Response_func pfunc)
{
	pSipd_Message_Failure_Resp = pfunc;
}


int Sip_Message_Answer(int tid , bool isOk)
{
	int ret;
	osip_message_t *answer = NULL;
	eXosip_lock (context_eXosip);
	if(isOk == false)
	{
		ret = eXosip_message_send_answer(context_eXosip, tid, 400, NULL);
	}
	else
	{
		ret = eXosip_message_build_answer (context_eXosip,tid, 200,&answer);
		if (ret !=OSIP_SUCCESS)
		{
			//printf("%s:%d eXosip_message_build_answer [EXOSIP_MESSAGE_NEW Message] failed\n", __FUNCTION__, __LINE__);
			stringstream ss;
			ss << "Sip_Message_Answer, eXosip_message_build_answer failed"
				<< ", tid = " << tid;
			LOG4CPLUS_DEBUG(gb_Logger, ss.str());
		}
		else
		{
			ret = eXosip_message_send_answer (context_eXosip,tid, 200,answer);
			if (ret !=OSIP_SUCCESS)
			{
				//printf("%s:%d eXosip_message_send_answer [EXOSIP_MESSAGE_NEW Message] failed\n", __FUNCTION__, __LINE__);
				stringstream ss;
				ss << "Sip_Message_Answer, eXosip_message_send_answer failed"
					<< ", tid = " << tid;
				LOG4CPLUS_DEBUG(gb_Logger, ss.str());
			}
		}
		
	}

	eXosip_unlock (context_eXosip);

	return ret;
}


void sipd_manscdp_xml_deal(const eXosip_event_t  *je , char *body)
{
	osip_message_t * sip_request;
	mxml_node_t *tree,   *query_node, * response_node, * control_node, * notify_node;
	char strbuf[MAX_SIPD_URI];

	sip_request = je->request;
	memset(strbuf, 0, sizeof(strbuf));

	bool ret = false;
	if(body != NULL)
	{
		string strUtf8 = Gbk2Utf8(body);
		tree = mxmlLoadString(NULL, strUtf8.c_str(), /*MXML_OPAQUE_CALLBACK*/ /*MXML_TEXT_CALLBACK*/  MXML_TEXT_CALLBACK);

		if(tree != NULL)
		{
			sipd_message_query_or_response  rsp;

			query_node = mxmlFindElement(tree, tree, "Query", NULL, NULL,MXML_DESCEND);
			response_node = mxmlFindElement(tree, tree, "Response", NULL, NULL,MXML_DESCEND);
			control_node = mxmlFindElement(tree, tree, "Control", NULL, NULL,MXML_DESCEND);
			notify_node = mxmlFindElement(tree, tree, "Notify", NULL, NULL,MXML_DESCEND);

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
			rsp.cid = je->cid;
			rsp.did = je->did;
			rsp.tid = je->tid;

			if(query_node != NULL )
			{
						

			}
			else if(control_node!=NULL)
			{

			}
			else if(notify_node !=NULL)
			{
				// 报警事件通知
				ret = handle_alarm_notify(tree, notify_node, &rsp);
				if(ret)
					Sip_Message_Answer(je->tid, true);

				if(ret== false)
				{
					ret = handle_file_to_end_notify(tree, notify_node, &rsp);
					if(ret)
						Sip_Message_Answer(je->tid, true);
				}
					
				/* keepalive 命令由 200OK 应答有命令本身来发 */
				if(ret == false)
					ret = handle_keepalive(je->tid, tree, notify_node, &rsp);
			}
			else if(response_node != NULL)
			{
				//	录像文件查询、设备目录查询、设备状态查询等命令的执行结果的回复
				ret = handle_catalog_response(tree, response_node, &rsp);
				if(ret == false)
					ret = handle_deviceinfo_response(tree, response_node, &rsp);
				if(ret == false)
					ret = handle_stateinfo_response(tree, response_node, &rsp);
				if(ret == false)
					ret = handle_search_recordfile_response(tree, response_node, &rsp);

				//	当为 true 时，改为在 DLL 层实现回复 200 OK 
				/*if(ret)
					Sip_Message_Answer(je->tid, true);*/

			}
			else
			{
				LOG4CPLUS_DEBUG(gb_Logger, "sipd_manscdp_xml_deal que_node is NULL" );
			}

			mxmlDelete(tree);
		}
		else
		{
			LOG4CPLUS_DEBUG(gb_Logger, "sipd_manscdp_xml_deal tree is NULL" );
		}
	}
	else
	{
		LOG4CPLUS_DEBUG(gb_Logger, "sipd_manscdp_xml_deal body is NULL" );

	}

	//	false 命令不接受 
	if(ret == false)
		Sip_Message_Answer(je->tid, false);
}




int handle_method_MESSAGE(const eXosip_event_t  *je)
{
	osip_message_t * sip_request = NULL;

	osip_body_t *body = NULL;
	osip_content_type_t* content_type=NULL;
	char buffer[MAX_XML_BUFFER_LEN+1]={0};
		
	sip_request = je->request;
	if(NULL == sip_request)
	{
		Sip_Message_Answer(je->tid, false);
		return -1;
	}

	content_type = osip_message_get_content_type(sip_request);
	if(NULL == content_type)
	{
		printf("%s line=%d content_type is NULL! \n", __FUNCTION__, __LINE__);
		Sip_Message_Answer(je->tid, false);

		return -1;
	}

	if((strcasecmp(content_type->type, "Application" ) == 0) && (strcasecmp(content_type->subtype, "MANSCDP+xml") == 0))
	{
		/*Application/MANSCDP+xml*/
		osip_message_get_body (sip_request, 0, &body); 

		/*allocate xml space*/
		memset(buffer, 0, sizeof(buffer));
		if(body->length > MAX_XML_BUFFER_LEN)
		{
			printf("%s line=%d body->length=%d  body length exceed! ERROR......\n", __FUNCTION__, __LINE__, body->length);

			Sip_Message_Answer(je->tid, false);
			return -1;
		}
		
		//	buffer 保存 xml 的内容
		strncpy_s(buffer, body->body, MAX_XML_BUFFER_LEN);

		sipd_manscdp_xml_deal(je, buffer);
	}
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
		if(/*Get_EvWaitFlg() ==*/ 1)
		{

			je = eXosip_event_wait (context_eXosip, 0, 50);  

			if (je == NULL)  
			{
				/*if(pSipd_on_idle)
					pSipd_on_idle();*/

				Sleep(100);
				continue;  
			}

			//context_eXosip_lock();
			eXosip_lock(context_eXosip);  
			eXosip_automatic_action (context_eXosip);  
			eXosip_unlock(context_eXosip);
			//context_eXosip_unlock();

			unsigned long tvBegin = GetTickCount();

			switch (je->type) 
			{
				case EXOSIP_REGISTRATION_SUCCESS: 
					break;

				case EXOSIP_REGISTRATION_FAILURE: 
					break;

				case EXOSIP_MESSAGE_NEW:
					{

						/*{
							stringstream ss;
							ss << "EXOSIP_MESSAGE_NEW, tid = "<< je->tid << ", did = " << je->did;
							LOG4CPLUS_DEBUG(gb_Logger,ss.str());
						}*/

						 if(MSG_IS_REGISTER(je->request))
						 {
							 handle_register(je);
						 }
						 else if(MSG_IS_OPTIONS(je->request))
						 {
							 // context_eXosip_lock();
							 eXosip_lock (context_eXosip);
							 eXosip_message_build_answer (context_eXosip, je->tid, 200,&answer);
							 eXosip_message_send_answer (context_eXosip, je->tid, 200,answer);
							 eXosip_unlock (context_eXosip);
							 // context_eXosip_unlock();
						 }
						 else if (MSG_IS_MESSAGE (je->request))
						 {
							 osip_content_type_t* content_type=NULL;

							 content_type = osip_message_get_content_type(je->request);
							 if(NULL == content_type)
							 {
								Sip_Message_Answer(je->tid, false);
							 }
							 else
							 {
								 handle_method_MESSAGE(je);
							 }

						 }
					}
					break;

				case EXOSIP_MESSAGE_ANSWERED:
					{
						stringstream ss;
						ss << "EXOSIP_MESSAGE_ANSWERED, tid = "<< je->tid << ", did = " << je->did;
						LOG4CPLUS_DEBUG(gb_Logger,ss.str());

						if(pSipd_Message_200_OK_Resp)
							pSipd_Message_200_OK_Resp(je->tid);
					}
					break;

				case EXOSIP_MESSAGE_SERVERFAILURE:
				case EXOSIP_MESSAGE_REQUESTFAILURE:  //请求失败
					if(pSipd_Message_Failure_Resp)
						pSipd_Message_Failure_Resp(je->tid);
					break;


				case EXOSIP_CALL_INVITE:
				case EXOSIP_CALL_REINVITE:
					break;

				case EXOSIP_CALL_ANSWERED:		//	 200 OK
					{
						stringstream ss;
						ss << "EXOSIP_CALL_ANSWERED, tid = "<< je->tid << ", did = " << je->did;
						LOG4CPLUS_DEBUG(gb_Logger,ss.str());
					}
					handle_call_answer(je);
					break;

				case EXOSIP_CALL_NOANSWER:
				case EXOSIP_CALL_REQUESTFAILURE:   /*EXOSIP_CALL_NOANSWER:*/
				case EXOSIP_CALL_CANCELLED:
				case EXOSIP_CALL_SERVERFAILURE:
				case EXOSIP_CALL_GLOBALFAILURE:
				case EXOSIP_CALL_REDIRECTED:
					handle_call_noanswer(je);
					break;

				case EXOSIP_CALL_ACK:

					break;
				case EXOSIP_CALL_CLOSED:
					break;

				//	该信息表示：作为主叫，收到了被叫对 BYE 的确认消息
				case EXOSIP_CALL_RELEASED:
					break;

				case EXOSIP_CALL_MESSAGE_NEW:
					if (MSG_IS_MESSAGE (je->request))
					{
						/* eXosip_lock (context_eXosip);
							
						 ret = eXosip_call_build_answer (context_eXosip,je->tid, 200,&answer);
						 if (ret !=OSIP_SUCCESS)
						 {
							 printf("%s:%d eXosip_call_build_answer [EXOSIP_MESSAGE_NEW Message] failed\n", __FUNCTION__, __LINE__);
						 }

						 ret = eXosip_call_send_answer (context_eXosip,je->tid, 200,answer);
						 if (ret !=OSIP_SUCCESS)
						 {
							 printf("%s:%d eXosip_call_send_answer [EXOSIP_MESSAGE_NEW Message] failed\n", __FUNCTION__, __LINE__);
						 }

						 eXosip_unlock (context_eXosip);*/

						 handle_method_MESSAGE(je);
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

				case EXOSIP_SUBSCRIPTION_NOTIFY:
					{
						handle_alarm_subscribe_notify(je);
					}
					break;

				case EXOSIP_SUBSCRIPTION_REQUESTFAILURE:
					{
						handle_subscribe_400_bad(je);
					}
					break;
				case EXOSIP_SUBSCRIPTION_ANSWERED:
					{
						handle_subscribe_200_ok(je);
						printf("%s line=%d <- (nid(%d) did(%d)>  subscription answer 200 ok  from \n", __FUNCTION__, __LINE__, je->nid, je->did);
					}
					break;

					//	新增订阅功能
				case EXOSIP_IN_SUBSCRIPTION_NEW:
					break;

				default:
					break;

			}

			unsigned long tvNow = GetTickCount();

			long  delay = tvNow - tvBegin;;

			if(delay > 50)
			{
				stringstream ss;
				ss << "处理时长, delay = " << delay
					<< ", je->type = " << je->type
					<< ", tid = " << je->tid << ", did = " << je->did;
				LOG4CPLUS_DEBUG(gb_Logger,ss.str());
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



int init_sip_protocol_stack(void)
{
	int result;
	stringstream ss;

	if(NULL == context_eXosip)
	{
		context_eXosip = eXosip_malloc();
		if (context_eXosip==NULL)
		{
			ss.str("");
			ss << __FUNCTION__ << "line=" << __LINE__ << " eXosip_malloc err";
			LOG4CPLUS_DEBUG(gb_Logger, ss.str());
			return -1;
		}
	}

	
	result=eXosip_init(context_eXosip);
	if (result !=OSIP_SUCCESS)
	{
		ss.str("");
		ss << __FUNCTION__ << "line=" << __LINE__ << " eXosip_init err";
		LOG4CPLUS_DEBUG(gb_Logger, ss.str());

		osip_free(context_eXosip);
		return -1;
	}

	printf("%s line=%d local_gateway=%s \n", __FUNCTION__, __LINE__, g_Cfg.local_gateway);
	result = eXosip_set_option (context_eXosip, EXOSIP_OPT_SET_IPV4_FOR_GATEWAY, g_Cfg.local_gateway);
	if (result !=OSIP_SUCCESS)
	{
		ss.str("");
		ss << __FUNCTION__ << "line=" << __LINE__ << ",local_gateway= " << string(g_Cfg.local_gateway) << ",eXosip_set_option err";
		LOG4CPLUS_DEBUG(gb_Logger, ss.str());

		osip_free(context_eXosip);
		return -1;
	}


	result = eXosip_listen_addr (context_eXosip, IPPROTO_UDP, NULL, g_Cfg.local_port, AF_INET, 0);
	if(result != OSIP_SUCCESS)
	{
		ss.str("");
		ss << __FUNCTION__ << "line=" << __LINE__ << "local_port = " << g_Cfg.local_port << ",eXosip_listen_addr err";
		LOG4CPLUS_DEBUG(gb_Logger, ss.str());

		osip_free(context_eXosip);
		return -1;
	}

	printf("init_sip_protocol_stack Successful! local_port=%d \n", g_Cfg.local_port);
	return 0;
}



int sipd_init(const SIPD_GATEWAY_CFG* pCfg)
{
	int ret = -1;
	HANDLE hThrd1;
	DWORD threadId1;

	gb_Logger = Logger::getInstance(LOG4CPLUS_TEXT("Gb_28181"));

	LOG4CPLUS_DEBUG(gb_Logger, "gw_init");

	memcpy(&g_Cfg, pCfg, sizeof(SIPD_GATEWAY_CFG));

	ret = init_sip_protocol_stack();
	if (ret != 0)
	{
		stringstream ss;
		ss << "gw_init init_sip_protocol_stack failed, err code = " << ret;
		LOG4CPLUS_DEBUG(gb_Logger, ss.str());

		return -1;
	}

	hThrd1  =  CreateThread(NULL,  0 , sipd_protocolevent, (LPVOID)NULL ,  0 ,  &threadId1 );
	if(NULL == hThrd1)
	{
		return -1;
	}


	LOG4CPLUS_DEBUG(gb_Logger, "gw_init successful!");


	return 0;
}
