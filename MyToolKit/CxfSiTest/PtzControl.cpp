#include "StdAfx.h"
#include "GbUsageEnvironment.h"
#include "CatalogQuery.h"

#include "PtzControl.h"

CPtzControl::CPtzControl(CGbUsageEnvironment& env)
:CXmlQuery(env)
{
}

CPtzControl::~CPtzControl(void)
{
}



bool CPtzControl::parse_request(mxml_node_t *tree, mxml_node_t *query_node, sipd_message_query_or_response  * pReq)
{
	int whitespace;
	mxml_node_t  *node;
	
	string strDeviceId;
	string strSN;
	if(CXmlQuery::parse_deviceId_and_SN(tree, query_node, strDeviceId, strSN)==false)
		return false;

	memset(&m_device_control_req, 0, sizeof(struct sipd_device_control));
	strncpy(m_device_control_req.to_uri, pReq->to_uri, MAX_SIPD_URI);
	strncpy(m_device_control_req.from_uri, pReq->from_uri,  MAX_SIPD_URI);
	strncpy(m_device_control_req.SN, strSN.c_str(), MAX_SIPD_DEVICE_ID_LEN);
	strncpy(m_device_control_req.device_id, strDeviceId.c_str(), MAX_SIPD_DEVICE_ID_LEN);

	
	node = mxmlFindElement(query_node, tree, XML_NAME_PTZCmd, NULL, NULL, MXML_DESCEND);
	if(NULL != node)
	{	
		//printf("%s line=%d  PTZCmd=%s\n", __FUNCTION__, __LINE__, mxmlGetText(node, &whitespace));
		strncpy(m_device_control_req.ptz_cmd, mxmlGetText(node, &whitespace), PTZ_CMD_LEN);
	}

	
	
	node = mxmlFindElement(query_node, tree, XML_NAME_ControlPriority, NULL, NULL, MXML_DESCEND);
	if(NULL != node)
	{	
		//printf("%s line=%d  ControlPriority=%s\n", __FUNCTION__, __LINE__, mxmlGetText(node, &whitespace));
		m_device_control_req.ControlPriority = atoi(mxmlGetText(node, &whitespace));
	}

	return true;
}



bool CPtzControl::handle_request()
{
	if(string(envir().m_cfg.local_domain_id)== string(m_device_control_req.device_id))
	{
		//	要求查找本域的状态

	}
	else
	{
		//	要求查找本域下的某个设备，以背靠背方式，代发请求
		CPtzControl * pCatalog = new CPtzControl(envir());
		if(pCatalog)
		{
			
			struct sipd_device_control req;
			memset(&req, 0 , sizeof(struct sipd_device_control));
			memcpy(&req, &m_device_control_req, sizeof(struct sipd_device_control));
			snprintf(req.from_uri, sizeof(req.from_uri), "sip:%s@%s:%d",\
				envir().m_cfg.local_domain_id, envir().m_cfg.local_ip, envir().m_cfg.local_port);

			string nextHopUri = envir().GetNextHopUri(m_device_control_req.device_id);
			strncpy(req.to_uri, nextHopUri.c_str(), MAX_SIPD_URI);

			pCatalog->send_request(&req, NULL);

			//	 不需要等待回复
			delete pCatalog;
		}
	}
	return true;
}

bool CPtzControl::handle_response()
{

	return true;
}

void CPtzControl::send_request(struct sipd_device_control  * pReq, CPtzControl * pDeviceInfo)
{
	//	请求本域为本域编码，则回复本域下注册的所有 设备

	memcpy(&m_device_control_req, pReq, sizeof(sipd_perset_query_req));
	//strncpy(m_device_control_req.SN, name(), MAX_SIPD_DEVICE_ID_LEN );

	sipd_message_query_or_response  msg;
	memset(&msg, 0 , sizeof(sipd_message_query_or_response));
	strncpy(msg.to_uri, m_device_control_req.to_uri, MAX_SIPD_URI);
	strncpy(msg.from_uri, m_device_control_req.from_uri,  MAX_SIPD_URI);

	snprintf(msg.body, MAX_SIPD_BODY_LENGTH,
		"<?xml version=\"1.0\"?>\r\n"\
		"<Control>\r\n"\
		"<CmdType>DeviceControl</CmdType>\r\n"\
		"<SN>%s</SN>\r\n"\
		"<DeviceID>%s</DeviceID>\r\n"\
		"<PTZCmd>%s</PTZCmd>\r\n"\
		"<Info><ControlPriority>%d</ControlPriority></Info>\r\n"\
		"</Control>",
		m_device_control_req.SN, 
		m_device_control_req.device_id,
		m_device_control_req.ptz_cmd,
		m_device_control_req.ControlPriority
		);

	send_message_response(&msg, NULL);
	
}


