#include "StdAfx.h"
#include "GbUsageEnvironment.h"
#include "CatalogQuery.h"

#include "RecordControl.h"

CRecordControl::CRecordControl(CGbUsageEnvironment& env)
:CXmlQuery(env)
{
}

CRecordControl::~CRecordControl(void)
{
}



bool CRecordControl::parse_request(mxml_node_t *tree, mxml_node_t *query_node, sipd_message_query_or_response  * pReq)
{
	int whitespace;
	mxml_node_t  *node;
	
	string strDeviceId;
	string strSN;
	if(CXmlQuery::parse_deviceId_and_SN(tree, query_node, strDeviceId, strSN)==false)
		return false;

	memset(&m_record_control_req, 0, sizeof(sipd_record_control));
	strncpy(m_record_control_req.to_uri, pReq->to_uri, MAX_SIPD_URI);
	strncpy(m_record_control_req.from_uri, pReq->from_uri,  MAX_SIPD_URI);
	strncpy(m_record_control_req.SN, strSN.c_str(), MAX_SIPD_DEVICE_ID_LEN);
	strncpy(m_record_control_req.device_id, strDeviceId.c_str(), MAX_SIPD_DEVICE_ID_LEN);

	node = mxmlFindElement(query_node, tree, "RecordCmd", NULL, NULL,MXML_DESCEND);
	if(node)
	{
		char strSumNum[MAX_SIPD_NORMAL_LEN];
		memset(strSumNum, 0 ,MAX_SIPD_NORMAL_LEN);
		memcpy(strSumNum, (void *)mxmlGetText(node, NULL),MAX_SIPD_NORMAL_LEN );
		if(string(strSumNum)=="Record")
			m_record_control_req.cmd_type = SIPD_RECORD_CONTROL_START;
		else
			m_record_control_req.cmd_type = SIPD_RECORD_CONTROL_STOP;
	}
	return true;
}



bool CRecordControl::parse_response(mxml_node_t *tree, mxml_node_t *response_node,  sipd_message_query_or_response  * pReq)
{
	int whitespace;
	mxml_node_t  *node, * item_node;

	string strDeviceId;
	string strSN;
	if(CXmlQuery::parse_deviceId_and_SN(tree, response_node,strDeviceId, strSN)==false)
		return false;

	memset(&m_record_control_rsp, 0, sizeof(sipd_control_rsp));
	strncpy(m_record_control_rsp.to_uri, pReq->to_uri, MAX_SIPD_URI);
	strncpy(m_record_control_rsp.from_uri, pReq->from_uri,  MAX_SIPD_URI);
	strncpy(m_record_control_rsp.SN, strSN.c_str(), MAX_SIPD_DEVICE_ID_LEN);
	strncpy(m_record_control_rsp.device_id, strDeviceId.c_str(), MAX_SIPD_DEVICE_ID_LEN);


	node = mxmlFindElement(response_node, tree, "Result", NULL, NULL,MXML_DESCEND);
	if(node)
	{
		char strSumNum[MAX_SIPD_NORMAL_LEN];
		memset(strSumNum, 0 ,MAX_SIPD_NORMAL_LEN);
		memcpy(strSumNum, (void *)mxmlGetText(node, NULL),MAX_SIPD_NORMAL_LEN );
		if(string(strSumNum)=="OK")
			m_record_control_rsp.result = SIPD_RESULT_OK;
		else
			m_record_control_rsp.result = SIPD_RESULT_ERROR;
	}
	return true;
}

bool CRecordControl::handle_request()
{
	if(string(envir().m_cfg.local_domain_id)== string(m_record_control_req.device_id))
	{
		//	要求查找本域的状态

	}
	else
	{
		//	要求查找本域下的某个设备，以背靠背方式，代发请求
		CRecordControl * pCatalog = new CRecordControl(envir());
		if(pCatalog)
		{
			sipd_record_control req;
			memset(&req, 0 , sizeof(sipd_record_control));
			strncpy(req.device_id, m_record_control_req.device_id, MAX_SIPD_DEVICE_ID_LEN);
			snprintf(req.from_uri, sizeof(req.from_uri), "sip:%s@%s:%d",\
				envir().m_cfg.local_domain_id, envir().m_cfg.local_ip, envir().m_cfg.local_port);

			string nextHopUri = envir().GetNextHopUri(m_record_control_req.device_id);
			strncpy(req.to_uri, nextHopUri.c_str(), MAX_SIPD_URI);

			pCatalog->send_request(&req, this);
		}
	}
	return true;
}

bool CRecordControl::handle_response()
{

	if(m_next)
	{
		//	该回复是由于以背靠背方式代发请求 而产生的回复
		m_next->send_response_from(this);
	}
	Medium::close(this);

	return true;
}

void CRecordControl::send_request(sipd_record_control  * pReq, CRecordControl * pDeviceInfo)
{
	//	请求本域为本域编码，则回复本域下注册的所有 设备

	memcpy(&m_record_control_req, pReq, sizeof(sipd_record_control));
	strncpy(m_record_control_req.SN, name(), MAX_SIPD_DEVICE_ID_LEN );

	sipd_message_query_or_response  msg;
	memset(&msg, 0 , sizeof(sipd_message_query_or_response));
	strncpy(msg.to_uri, m_record_control_req.to_uri, MAX_SIPD_URI);
	strncpy(msg.from_uri, m_record_control_req.from_uri,  MAX_SIPD_URI);

	snprintf(msg.body, MAX_SIPD_BODY_LENGTH,
		"<?xml version=\"1.0\"?>\r\n"\
		"<Control>\r\n"\
		"<CmdType>DeviceInfo</CmdType>\r\n"\
		"<SN>%s</SN>\r\n"\
		"<DeviceID>%s</DeviceID>\r\n"\
		"<RecordCmd>%s</RecordCmd>\r\n"\
		"</Control>",
		m_record_control_req.SN, m_record_control_req.device_id,
		m_record_control_req.cmd_type == SIPD_RECORD_CONTROL_START ? "Record":"StopRecord");

	send_message_response(&msg, NULL);
	m_next = pDeviceInfo;

}


void CRecordControl::send_response( sipd_control_rsp* pDeviceInfo)
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

	snprintf(msg.body,MAX_XML_BUFFER_LEN,
		"<?xml version=\"1.0\"?>\r\n"\
		"<Response>\r\n"\
		"<CmdType>DeviceControl</CmdType>\r\n"\
		"<SN>%d</SN>\r\n"\
		"<DeviceID>%s</DeviceID>\r\n"\
		"<Result>%s</Result>\r\n"\
		"</Response>",
		pDeviceInfo->SN, pDeviceInfo->device_id, (pDeviceInfo->result ==0)?"OK":"ERROR");

	send_message_response(&msg, NULL);
}


void CRecordControl::send_response_from(CRecordControl * pCatalog)
{
	//	根据 被靠被转发而获取的回复，进行回复
	sipd_control_rsp  cataBasicInfo;
	memset(&cataBasicInfo, 0, sizeof(sipd_control_rsp));
	memcpy(&cataBasicInfo, &pCatalog->m_record_control_rsp, sizeof(sipd_control_rsp));

	strncpy(cataBasicInfo.to_uri , m_record_control_req.from_uri, MAX_SIPD_URI);
	strncpy(cataBasicInfo.from_uri , m_record_control_req.to_uri, MAX_SIPD_URI);
	strncpy(cataBasicInfo.SN , m_record_control_req.SN, MAX_SIPD_DEVICE_ID_LEN);
	strncpy(cataBasicInfo.device_id , m_record_control_req.device_id, MAX_SIPD_DEVICE_ID_LEN);

	string nextHopUri = envir().GetNextHopUri(cataBasicInfo.to_uri);
	strncpy(cataBasicInfo.to_uri ,nextHopUri.c_str(), MAX_SIPD_URI);
	send_response(&cataBasicInfo);

	Medium::close(this);
}