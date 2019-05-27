#include "StdAfx.h"

#include "GbUsageEnvironment.h"
#include "CatalogQuery.h"

#include "DeviceInfoQuery.h"

CDeviceInfoQuery::CDeviceInfoQuery(CGbUsageEnvironment& env)
:CXmlQuery(env)
{
}

CDeviceInfoQuery::~CDeviceInfoQuery(void)
{
}


bool CDeviceInfoQuery::parse_request(mxml_node_t *tree, mxml_node_t *query_node, sipd_message_query_or_response  * pReq)
{
	int whitespace;
	mxml_node_t  *node;
	
	string strDeviceId;
	string strSN;
	if(CXmlQuery::parse_deviceId_and_SN(tree, query_node, strDeviceId, strSN)==false)
		return false;

	memset(&m_deviceinfo_req, 0, sizeof(sipd_deviceinfo_req));
	strncpy(m_deviceinfo_req.to_uri, pReq->to_uri, MAX_SIPD_URI);
	strncpy(m_deviceinfo_req.from_uri, pReq->from_uri,  MAX_SIPD_URI);
	strncpy(m_deviceinfo_req.SN, strSN.c_str(), MAX_SIPD_DEVICE_ID_LEN);
	strncpy(m_deviceinfo_req.device_id, strDeviceId.c_str(), MAX_SIPD_DEVICE_ID_LEN);


	return true;
}

void CDeviceInfoQuery::send_request(sipd_deviceinfo_req  * pReq, CDeviceInfoQuery * pDeviceInfo)
{
	//	请求本域为本域编码，则回复本域下注册的所有 设备

	memcpy(&m_deviceinfo_req, pReq, sizeof(sipd_deviceinfo_req));
	strncpy(m_deviceinfo_req.SN, name(), MAX_SIPD_DEVICE_ID_LEN );

	sipd_message_query_or_response  msg;
	memset(&msg, 0 , sizeof(sipd_message_query_or_response));
	strncpy(msg.to_uri, m_deviceinfo_req.to_uri, MAX_SIPD_URI);
	strncpy(msg.from_uri, m_deviceinfo_req.from_uri,  MAX_SIPD_URI);
	
	snprintf(msg.body, MAX_SIPD_BODY_LENGTH,
		"<?xml version=\"1.0\"?>\r\n"\
		"<Query>\r\n"\
		"<CmdType>DeviceInfo</CmdType>\r\n"\
		"<SN>%s</SN>\r\n"\
		"<DeviceID>%s</DeviceID>\r\n"\
		"</Query>",
		m_deviceinfo_req.SN, m_deviceinfo_req.device_id);

	send_message_response(&msg, NULL);
	m_next = pDeviceInfo;

}


void CDeviceInfoQuery::send_response( sipd_deviceinfo_rsp* pDeviceInfo)
{
	//	请求本域为本域编码，则回复本域下注册的所有 设备

	sipd_message_query_or_response  msg;
	memset(&msg, 0 , sizeof(sipd_message_query_or_response));
	strncpy(msg.to_uri, pDeviceInfo->to_uri, MAX_SIPD_URI);
	strncpy(msg.from_uri, pDeviceInfo->from_uri,  MAX_SIPD_URI);

	if(NULL == pDeviceInfo)
	{
		printf("sipd_dev_info_rsp inout err\n");
		return ;
	}

	int len = 0;
	if(pDeviceInfo->Result != SIPD_RESULT_OK)
	{
		snprintf(msg.body,  MAX_SIPD_BODY_LENGTH,
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
		snprintf(msg.body, sizeof(msg.body),
			"<?xml version=\"1.0\"?>\r\n"\
			"<Response>\r\n"\
			"<CmdType>DeviceInfo</CmdType>\r\n"\
			"<SN>%s</SN>\r\n"\
			"<DeviceID>%s</DeviceID>\r\n"\
			"<Result>OK</Result>\r\n",
			pDeviceInfo->SN, pDeviceInfo->device_id);

		if(strlen(pDeviceInfo->DeviceType) > 0)
		{
			len = strlen(msg.body);
			snprintf(msg.body+len, (MAX_XML_BUFFER_LEN-len),"<DeviceType>%s</DeviceType>\r\n", pDeviceInfo->DeviceType);
		}

		if(strlen(pDeviceInfo->manufacturer) > 0)
		{
			len = strlen(msg.body);
			snprintf(msg.body+len, (MAX_XML_BUFFER_LEN-len),"<Manufacturer>%s</Manufacturer>\r\n", pDeviceInfo->manufacturer);
		}


		if(strlen(pDeviceInfo->model) > 0)
		{
			len = strlen(msg.body);
			snprintf(msg.body+len, (MAX_XML_BUFFER_LEN-len),"<Model>%s</Model>\r\n", pDeviceInfo->model);
		}

		if(strlen(pDeviceInfo->firmware) > 0)
		{
			len = strlen(msg.body);
			snprintf(msg.body+len, (MAX_XML_BUFFER_LEN-len),"<Firmware>%s</Firmware>\r\n", pDeviceInfo->firmware);
		}

		if((pDeviceInfo->MaxCamera)  > 0)
		{
			len = strlen(msg.body);
			snprintf(msg.body+len, (MAX_XML_BUFFER_LEN-len),"<MaxCamera>%d</MaxCamera>\r\n", pDeviceInfo->MaxCamera);
		}

		if((pDeviceInfo->MaxAlarm)  > 0)
		{
			len = strlen(msg.body);
			snprintf(msg.body+len, (MAX_XML_BUFFER_LEN-len),"<MaxAlarm>%d</MaxAlarm>\r\n", pDeviceInfo->MaxAlarm);
		}

		snprintf(msg.body+len, (MAX_XML_BUFFER_LEN-len),"%s", "</Response>");
	}
	send_message_response(&msg, NULL);

}



bool CDeviceInfoQuery::handle_request()
{
	if(string(envir().m_cfg.local_domain_id)== string(m_deviceinfo_req.device_id))
	{
		//	要求查找本域的状态
		
	}
	else
	{
		//	要求查找本域下的某个设备，以背靠背方式，代发请求
		CDeviceInfoQuery * pCatalog = new CDeviceInfoQuery(envir());
		if(pCatalog)
		{
			sipd_deviceinfo_req req;
			memset(&req, 0 , sizeof(sipd_deviceinfo_req));
			strncpy(req.device_id, m_deviceinfo_req.device_id, MAX_SIPD_DEVICE_ID_LEN);
			snprintf(req.from_uri, sizeof(req.from_uri), "sip:%s@%s:%d",\
				envir().m_cfg.local_domain_id, envir().m_cfg.local_ip, envir().m_cfg.local_port);
			
			string nextHopUri = envir().GetNextHopUri(m_deviceinfo_req.device_id);
			strncpy(req.to_uri, nextHopUri.c_str(), MAX_SIPD_URI);

			pCatalog->send_request(&req, this);
		}
	}
	return true;
}


bool CDeviceInfoQuery::parse_response(mxml_node_t *tree, mxml_node_t *response_node,  sipd_message_query_or_response  * pReq)
{
	int whitespace;
	mxml_node_t  *node, * item_node;

	string strDeviceId;
	string strSN;
	if(CXmlQuery::parse_deviceId_and_SN(tree, response_node,strDeviceId, strSN)==false)
		return false;

	memset(&m_deviceinfo_rsp, 0, sizeof(sipd_deviceinfo_rsp));
	strncpy(m_deviceinfo_rsp.to_uri, pReq->to_uri, MAX_SIPD_URI);
	strncpy(m_deviceinfo_rsp.from_uri, pReq->from_uri,  MAX_SIPD_URI);
	strncpy(m_deviceinfo_rsp.SN, strSN.c_str(), MAX_SIPD_DEVICE_ID_LEN);
	strncpy(m_deviceinfo_rsp.device_id, strDeviceId.c_str(), MAX_SIPD_DEVICE_ID_LEN);

	node = mxmlFindElement(response_node, tree, "Result", NULL, NULL,MXML_DESCEND);
	if(node)
	{
		char strSumNum[MAX_SIPD_NORMAL_LEN];
		memset(strSumNum, 0 ,MAX_SIPD_NORMAL_LEN);
		memcpy(strSumNum, (void *)mxmlGetText(node, NULL),MAX_SIPD_NORMAL_LEN );
		if(string(strSumNum)=="OK")
			m_deviceinfo_rsp.Result = SIPD_RESULT_OK;
		else
			m_deviceinfo_rsp.Result = SIPD_RESULT_ERROR;
	}

	node = mxmlFindElement(response_node, tree, "Manufacturer", NULL, NULL,MXML_DESCEND);
	if(node)
		memcpy(m_deviceinfo_rsp.manufacturer, (void *)mxmlGetText(node, NULL), \
		sizeof(m_deviceinfo_rsp.manufacturer));

	node = mxmlFindElement(response_node, tree, "Model", NULL, NULL,MXML_DESCEND);
	if(node)
		memcpy(m_deviceinfo_rsp.model, (void *)mxmlGetText(node, NULL), \
		sizeof(m_deviceinfo_rsp.model));

	node = mxmlFindElement(response_node, tree, "Firmware", NULL, NULL,MXML_DESCEND);
	if(node)
		memcpy(m_deviceinfo_rsp.firmware, (void *)mxmlGetText(node, NULL), \
		sizeof(m_deviceinfo_rsp.firmware));

	node = mxmlFindElement(response_node, tree, "DeviceType", NULL, NULL,MXML_DESCEND);
	if(node)
		memcpy(m_deviceinfo_rsp.DeviceType, (void *)mxmlGetText(node, NULL), \
		sizeof(m_deviceinfo_rsp.DeviceType));

	node = mxmlFindElement(response_node, tree, "MaxCamera", NULL, NULL,MXML_DESCEND);
	if(node)
	{
		char strSumNum[MAX_SIPD_NORMAL_LEN];
		memset(strSumNum, 0 ,MAX_SIPD_NORMAL_LEN);
		memcpy(strSumNum, (void *)mxmlGetText(node, NULL),MAX_SIPD_NORMAL_LEN );
		m_deviceinfo_rsp.MaxCamera = atoi(strSumNum);
	}

	node = mxmlFindElement(response_node, tree, "MaxAlarm", NULL, NULL,MXML_DESCEND);
	if(node)
	{
		char strSumNum[MAX_SIPD_NORMAL_LEN];
		memset(strSumNum, 0 ,MAX_SIPD_NORMAL_LEN);
		memcpy(strSumNum, (void *)mxmlGetText(node, NULL),MAX_SIPD_NORMAL_LEN );
		m_deviceinfo_rsp.MaxAlarm = atoi(strSumNum);
	}

	return true;
}

bool CDeviceInfoQuery::handle_response()
{

	if(m_next)
	{
		//	该回复是由于以背靠背方式代发请求 而产生的回复
		m_next->send_response_from(this);
	}
	Medium::close(this);

	return true;
}


void CDeviceInfoQuery::send_response_from(CDeviceInfoQuery * pCatalog)
{
	//	根据 被靠被转发而获取的回复，进行回复
	sipd_deviceinfo_rsp  cataBasicInfo;
	memset(&cataBasicInfo, 0, sizeof(sipd_deviceinfo_rsp));
	memcpy(&cataBasicInfo, &pCatalog->m_deviceinfo_rsp, sizeof(sipd_deviceinfo_rsp));

	strncpy(cataBasicInfo.to_uri , m_deviceinfo_req.from_uri, MAX_SIPD_URI);
	strncpy(cataBasicInfo.from_uri , m_deviceinfo_req.to_uri, MAX_SIPD_URI);
	strncpy(cataBasicInfo.SN , m_deviceinfo_req.SN, MAX_SIPD_DEVICE_ID_LEN);
	strncpy(cataBasicInfo.device_id , m_deviceinfo_req.device_id, MAX_SIPD_DEVICE_ID_LEN);

	string nextHopUri = envir().GetNextHopUri(cataBasicInfo.to_uri);
	strncpy(cataBasicInfo.to_uri ,nextHopUri.c_str(), MAX_SIPD_URI);
	send_response(&cataBasicInfo);

	Medium::close(this);
}