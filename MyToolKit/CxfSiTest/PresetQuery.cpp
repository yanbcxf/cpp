#include "StdAfx.h"
#include "GbUsageEnvironment.h"
#include "CatalogQuery.h"

#include "PresetQuery.h"

CPresetQuery::CPresetQuery(CGbUsageEnvironment& env)
:CXmlQuery(env)
{
}

CPresetQuery::~CPresetQuery(void)
{
}


bool CPresetQuery::parse_request(mxml_node_t *tree, mxml_node_t *query_node, sipd_message_query_or_response  * pReq)
{
	int whitespace;
	mxml_node_t  *node;
	
	string strDeviceId;
	string strSN;
	if(CXmlQuery::parse_deviceId_and_SN(tree, query_node, strDeviceId, strSN)==false)
		return false;

	memset(&m_perset_query_req, 0, sizeof(sipd_perset_query_req));
	strncpy(m_perset_query_req.to_uri, pReq->to_uri, MAX_SIPD_URI);
	strncpy(m_perset_query_req.from_uri, pReq->from_uri,  MAX_SIPD_URI);
	strncpy(m_perset_query_req.SN, strSN.c_str(), MAX_SIPD_DEVICE_ID_LEN);
	strncpy(m_perset_query_req.device_id, strDeviceId.c_str(), MAX_SIPD_DEVICE_ID_LEN);

	node = mxmlFindElement(tree, tree, XML_NAME_StartTime, NULL, NULL, MXML_DESCEND);
	if(NULL != node)
	{	
		//printf("%s line=%d  StartTime=%s\n", __FUNCTION__, __LINE__, mxmlGetText(node, &whitespace));
		strncpy(m_perset_query_req.StartTime, mxmlGetText(node, &whitespace), MAX_SIPD_TIME_LEN);
	}

	node = mxmlFindElement(tree, tree, XML_NAME_EndTime, NULL, NULL, MXML_DESCEND);
	if(NULL != node)
	{	
		//printf( "%s line=%d  EndTime=%s\n", __FUNCTION__, __LINE__, mxmlGetText(node, &whitespace));
		strncpy(m_perset_query_req.EndTime, mxmlGetText(node, &whitespace), MAX_SIPD_TIME_LEN);
	}

	return true;
}



bool CPresetQuery::parse_response(mxml_node_t *tree, mxml_node_t *response_node,  sipd_message_query_or_response  * pReq)
{
	int whitespace;
	mxml_node_t  *node, * item_node;

	string strDeviceId;
	string strSN;
	if(CXmlQuery::parse_deviceId_and_SN(tree, response_node,strDeviceId, strSN)==false)
		return false;

	/*memset(&m_deviceinfo_rsp, 0, sizeof(sipd_deviceinfo_rsp));
	strncpy(m_deviceinfo_rsp.to_uri, pReq->to_uri, MAX_SIPD_URI);
	strncpy(m_deviceinfo_rsp.from_uri, pReq->from_uri,  MAX_SIPD_URI);
	strncpy(m_deviceinfo_rsp.SN, strSN.c_str(), MAX_SIPD_DEVICE_ID_LEN);
	strncpy(m_deviceinfo_rsp.device_id, strDeviceId.c_str(), MAX_SIPD_DEVICE_ID_LEN);*/

	return true;
}

bool CPresetQuery::handle_request()
{
	if(string(envir().m_cfg.local_domain_id)== string(m_perset_query_req.device_id))
	{
		//	要求查找本域的状态

	}
	else
	{
		//	要求查找本域下的某个设备，以背靠背方式，代发请求
		CPresetQuery * pCatalog = new CPresetQuery(envir());
		if(pCatalog)
		{
			sipd_perset_query_req req;
			memset(&req, 0 , sizeof(sipd_perset_query_req));
			strncpy(req.device_id, m_perset_query_req.device_id, MAX_SIPD_DEVICE_ID_LEN);
			snprintf(req.from_uri, sizeof(req.from_uri), "sip:%s@%s:%d",\
				envir().m_cfg.local_domain_id, envir().m_cfg.local_ip, envir().m_cfg.local_port);

			string nextHopUri = envir().GetNextHopUri(m_perset_query_req.device_id);
			strncpy(req.to_uri, nextHopUri.c_str(), MAX_SIPD_URI);

			pCatalog->send_request(&req, this);
		}
	}
	return true;
}

bool CPresetQuery::handle_response()
{

	if(m_next)
	{
		//	该回复是由于以背靠背方式代发请求 而产生的回复
		m_next->send_response_from(this);
	}
	Medium::close(this);

	return true;
}

void CPresetQuery::send_request(sipd_perset_query_req  * pReq, CPresetQuery * pDeviceInfo)
{
	//	请求本域为本域编码，则回复本域下注册的所有 设备

	memcpy(&m_perset_query_req, pReq, sizeof(sipd_perset_query_req));
	strncpy(m_perset_query_req.SN, name(), MAX_SIPD_DEVICE_ID_LEN );

	sipd_message_query_or_response  msg;
	memset(&msg, 0 , sizeof(sipd_message_query_or_response));
	strncpy(msg.to_uri, m_perset_query_req.to_uri, MAX_SIPD_URI);
	strncpy(msg.from_uri, m_perset_query_req.from_uri,  MAX_SIPD_URI);

	snprintf(msg.body, MAX_SIPD_BODY_LENGTH,
		"<?xml version=\"1.0\"?>\r\n"\
		"<Query>\r\n"\
		"<CmdType>DeviceInfo</CmdType>\r\n"\
		"<SN>%s</SN>\r\n"\
		"<DeviceID>%s</DeviceID>\r\n"\
		"</Query>",
		m_perset_query_req.SN, m_perset_query_req.device_id);

	send_message_response(&msg, NULL);
	m_next = pDeviceInfo;

}


void CPresetQuery::send_response( sipd_deviceinfo_rsp* pDeviceInfo)
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

	send_message_response(&msg, NULL);
}


void CPresetQuery::send_response_from(CPresetQuery * pCatalog)
{
	//	根据 被靠被转发而获取的回复，进行回复
	sipd_deviceinfo_rsp  cataBasicInfo;
	memset(&cataBasicInfo, 0, sizeof(sipd_deviceinfo_rsp));
	//memcpy(&cataBasicInfo, &pCatalog->m_deviceinfo_rsp, sizeof(sipd_deviceinfo_rsp));

	strncpy(cataBasicInfo.to_uri , m_perset_query_req.from_uri, MAX_SIPD_URI);
	strncpy(cataBasicInfo.from_uri , m_perset_query_req.to_uri, MAX_SIPD_URI);
	strncpy(cataBasicInfo.SN , m_perset_query_req.SN, MAX_SIPD_DEVICE_ID_LEN);
	strncpy(cataBasicInfo.device_id , m_perset_query_req.device_id, MAX_SIPD_DEVICE_ID_LEN);

	string nextHopUri = envir().GetNextHopUri(cataBasicInfo.to_uri);
	strncpy(cataBasicInfo.to_uri ,nextHopUri.c_str(), MAX_SIPD_URI);
	send_response(&cataBasicInfo);

	Medium::close(this);
}