#include "stdafx.h"
#include "Gb28181_Layer.h"

#include <eXosip.h>
#include <osip_headers.h>
#include <osip_mt.h>

bool parse_deviceId_and_SN(mxml_node_t *tree, mxml_node_t *query_node, string & deviceId, string & SN);

//////////////////////////////////////////////////////////////////////////

/*下级注册回调, 会每个 2 分钟重新注册一次 ，相当于心跳 */
Sipd_Register_Request_func				pSipd_register_req;

void Register_Sipd_Register_Request(Sipd_Register_Request_func pfunc)
{
	pSipd_register_req = pfunc;
}

/*下级心跳回调*/
Sipd_Keepalive_Notify_func				pSipd_Keepalive_req;

void Register_Sipd_Keepalive_Notify(Sipd_Keepalive_Notify_func pfunc)
{
	pSipd_Keepalive_req = pfunc;
}

void handle_register(const eXosip_event_t  *je)
{
	int ret=0;
	osip_message_t *answer = NULL;
	osip_authorization_t *auth;
	osip_header_t *expires_header = NULL;
	char buff[MAX_SIPD_BODY_LENGTH];

	memset(buff, 0, sizeof(buff));

	ret = osip_message_get_authorization(je->request, 0, &auth);					
	if(ret!=OSIP_SUCCESS)
	{
		eXosip_lock (context_eXosip);

		ret = eXosip_message_build_answer (context_eXosip,je->tid, 401,&answer);

		snprintf(buff,MAX_SIPD_BODY_LENGTH,\
			"Digest realm=\"43000000\",nonce=\"181b4c350c00f3c2\"");
		osip_message_set_www_authenticate(answer, buff);

		eXosip_message_send_answer (context_eXosip,je->tid, 401, answer);

		eXosip_unlock (context_eXosip);
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

		strncpy_s(RegisterReq.username, auth->username, MAX_SIPD_NORMAL_LEN);
		strncpy_s(RegisterReq.realm, auth->realm, MAX_SIPD_NORMAL_LEN);
		strncpy_s(RegisterReq.uri, auth->uri, MAX_SIPD_URI);
		strncpy_s(RegisterReq.nonce, auth->nonce, MAX_SIPD_NORMAL_LEN);
		strncpy_s(RegisterReq.response, auth->response, MAX_SIPD_NORMAL_LEN);
		strncpy_s(RegisterReq.algorithm, auth->algorithm, MAX_SIPD_NORMAL_LEN);
		RegisterReq.tid = je->tid;
		RegisterReq.rid = je->rid;

		osip_message_get_expires(je->request, 0, &expires_header);
		RegisterReq.expires = osip_atoi(expires_header->hvalue);

		if(pSipd_register_req)
			pSipd_register_req(&RegisterReq);


	}

}



void Sip_Register_Responce(int tid, int isOk)
{
	int ret = OSIP_UNDEFINED_ERROR;
	osip_message_t *answer = NULL;
	eXosip_lock (context_eXosip);
	
	do{
		if(isOk)
		{
			ret = eXosip_message_build_answer (context_eXosip,tid, 200,&answer);
			if(ret != OSIP_SUCCESS)
			{
				LOG4CPLUS_DEBUG(gb_Logger, "Sip_Register_Responce, eXosip_message_build_answer(200) failed");
				break;
			}

			ret = eXosip_message_send_answer (context_eXosip,tid, 200, answer);
			if(ret < OSIP_SUCCESS)
			{
				LOG4CPLUS_DEBUG(gb_Logger, "Sip_Register_Responce, eXosip_message_send_answer(200) failed");
				break;
			}
		}
		else
		{
			ret = eXosip_message_build_answer (context_eXosip,tid, 401,&answer);
			if(ret != OSIP_SUCCESS)
			{
				LOG4CPLUS_DEBUG(gb_Logger, "Sip_Register_Responce, eXosip_message_build_answer(401) failed");
				break;
			}

			ret = eXosip_message_send_answer (context_eXosip,tid, 401, answer);
			if(ret < OSIP_SUCCESS)
			{
				LOG4CPLUS_DEBUG(gb_Logger, "Sip_Register_Responce, eXosip_message_send_answer(401) failed");
				break;
			}
		}
	}while(0);
	
	eXosip_unlock (context_eXosip);
}




bool handle_keepalive(int tid, mxml_node_t *tree, mxml_node_t *notify_node, sipd_message_query_or_response  * pReq)
{

	sipd_keepalive_notify		basic_info;
	mxml_node_t  *node,  * CmdType_node;

	CmdType_node = mxmlFindElement(notify_node, tree, "CmdType", NULL, NULL,MXML_DESCEND);
	if(CmdType_node != NULL && mxmlGetText(CmdType_node, NULL))
	{
		char strbuf[MAX_SIPD_URI];
		memset(strbuf, 0, sizeof(strbuf));
		strncpy_s(strbuf,  (const char *)mxmlGetText(CmdType_node, NULL), sizeof(strbuf));
		if(strcmp(strbuf, "Keepalive") != 0)
			return false;
	}

	string strDeviceId;
	string strSN;
	if(parse_deviceId_and_SN(tree, notify_node,strDeviceId, strSN)==false)
		return false;

	stringstream ss;
	/*ss << "CCatalogQuery::parse_response m_current_item = " << m_current_item;
	LOG4CPLUS_DEBUG(envir().m_Logger, ss.str().c_str());*/

	memset(&basic_info, 0, sizeof(sipd_catalog_rsp_basic_info));
	strncpy_s(basic_info.to_uri, pReq->to_uri, MAX_SIPD_URI);
	strncpy_s(basic_info.from_uri, pReq->from_uri,  MAX_SIPD_URI);
	strncpy_s(basic_info.SN, strSN.c_str(), MAX_SIPD_DEVICE_ID_LEN);
	strncpy_s(basic_info.device_id, strDeviceId.c_str(), MAX_SIPD_DEVICE_ID_LEN);


	node = mxmlFindElement(notify_node, tree, "Status", NULL, NULL,MXML_DESCEND);
	if(node)
	{
		memset(basic_info.Status , 0 ,MAX_SIPD_TIME_LEN + 1);
		strncpy_s(basic_info.Status, (const char *)mxmlGetText(node, NULL), MAX_SIPD_TIME_LEN );
	}

	
	if(pSipd_Keepalive_req)
		pSipd_Keepalive_req(tid, &basic_info);

	return true;
}