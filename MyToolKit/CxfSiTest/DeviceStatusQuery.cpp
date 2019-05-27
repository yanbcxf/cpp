#include "StdAfx.h"
#include "GbUsageEnvironment.h"
#include "CatalogQuery.h"

#include "DeviceStatusQuery.h"

CDeviceStatusQuery::CDeviceStatusQuery(CGbUsageEnvironment& env)
:CXmlQuery(env)
{
}

CDeviceStatusQuery::~CDeviceStatusQuery(void)
{
}



bool CDeviceStatusQuery::parse_request(mxml_node_t *tree, mxml_node_t *query_node, sipd_message_query_or_response  * pReq)
{
	int whitespace;
	mxml_node_t  *node;

	string strDeviceId;
	string strSN;
	if(CXmlQuery::parse_deviceId_and_SN(tree, query_node, strDeviceId, strSN)==false)
		return false;

	memset(&m_state_req, 0, sizeof(sipd_devicestate_req));
	strncpy(m_state_req.to_uri, pReq->to_uri, MAX_SIPD_URI);
	strncpy(m_state_req.from_uri, pReq->from_uri,  MAX_SIPD_URI);
	strncpy(m_state_req.SN, strSN.c_str(), MAX_SIPD_DEVICE_ID_LEN);
	strncpy(m_state_req.device_id, strDeviceId.c_str(), MAX_SIPD_DEVICE_ID_LEN);

	return true;
}

bool CDeviceStatusQuery::parse_response(mxml_node_t *tree, mxml_node_t *response_node,  sipd_message_query_or_response  * pReq)
{
	int whitespace;
	mxml_node_t  *node, * item_node;

	string strDeviceId;
	string strSN;
	if(CXmlQuery::parse_deviceId_and_SN(tree, response_node,strDeviceId, strSN)==false)
		return false;

	memset(&m_state_rsp, 0, sizeof(sipd_stateinfo_adaptor_rsp));
	strncpy(m_state_rsp.basic_info.to_uri, pReq->to_uri, MAX_SIPD_URI);
	strncpy(m_state_rsp.basic_info.from_uri, pReq->from_uri,  MAX_SIPD_URI);
	strncpy(m_state_rsp.basic_info.SN, strSN.c_str(), MAX_SIPD_DEVICE_ID_LEN);
	strncpy(m_state_rsp.basic_info.device_id, strDeviceId.c_str(), MAX_SIPD_DEVICE_ID_LEN);

	node = mxmlFindElement(response_node, tree, "Result", NULL, NULL,MXML_DESCEND);
	if(node)
	{
		char strSumNum[MAX_SIPD_NORMAL_LEN];
		memset(strSumNum, 0 ,MAX_SIPD_NORMAL_LEN);
		memcpy(strSumNum, (void *)mxmlGetText(node, NULL),MAX_SIPD_NORMAL_LEN );
		if(string(strSumNum)=="OK")
			m_state_rsp.basic_info.Result = SIPD_RESULT_OK;
		else
			m_state_rsp.basic_info.Result = SIPD_RESULT_ERROR;
	}

	node = mxmlFindElement(response_node, tree, "Online", NULL, NULL,MXML_DESCEND);
	if(node)
	{
		char strSumNum[MAX_SIPD_NORMAL_LEN];
		memset(strSumNum, 0 ,MAX_SIPD_NORMAL_LEN);
		memcpy(strSumNum, (void *)mxmlGetText(node, NULL),MAX_SIPD_NORMAL_LEN );
		if(string(strSumNum)=="ONLINE")
			m_state_rsp.basic_info.Online = SIPD_STATUS_ONLINE_TYPE_ONLINE;
		else
			m_state_rsp.basic_info.Online = SIPD_STATUS_ONLINE_TYPE_OFFLINE;
	}

	node = mxmlFindElement(response_node, tree, "Status", NULL, NULL,MXML_DESCEND);
	if(node)
	{
		char strSumNum[MAX_SIPD_NORMAL_LEN];
		memset(strSumNum, 0 ,MAX_SIPD_NORMAL_LEN);
		memcpy(strSumNum, (void *)mxmlGetText(node, NULL),MAX_SIPD_NORMAL_LEN );
		if(string(strSumNum)=="OK")
			m_state_rsp.basic_info.Status = SIPD_RESULT_OK;
		else
			m_state_rsp.basic_info.Status = SIPD_RESULT_ERROR;
	}

	node = mxmlFindElement(response_node, tree, "Encode", NULL, NULL,MXML_DESCEND);
	if(node)
	{
		char strSumNum[MAX_SIPD_NORMAL_LEN];
		memset(strSumNum, 0 ,MAX_SIPD_NORMAL_LEN);
		memcpy(strSumNum, (void *)mxmlGetText(node, NULL),MAX_SIPD_NORMAL_LEN );
		if(string(strSumNum)=="ON")
			m_state_rsp.basic_info.Encode = SIPD_STATUS_ON;
		else
			m_state_rsp.basic_info.Encode = SIPD_STATUS_OFF;
	}

	node = mxmlFindElement(response_node, tree, "Record", NULL, NULL,MXML_DESCEND);
	if(node)
	{
		char strSumNum[MAX_SIPD_NORMAL_LEN];
		memset(strSumNum, 0 ,MAX_SIPD_NORMAL_LEN);
		memcpy(strSumNum, (void *)mxmlGetText(node, NULL),MAX_SIPD_NORMAL_LEN );
		if(string(strSumNum)=="ON")
			m_state_rsp.basic_info.Record = SIPD_STATUS_ON;
		else
			m_state_rsp.basic_info.Record = SIPD_STATUS_OFF;
	}

	node = mxmlFindElement(response_node, tree, "DeviceTime", NULL, NULL, MXML_DESCEND);
	if(NULL != node)
	{	
		// printf("%s line=%d  StartTime=%s\n", __FUNCTION__, __LINE__, mxmlGetText(node, &whitespace));
		strncpy(m_state_rsp.basic_info.DeviceTime, mxmlGetText(node, &whitespace), MAX_SIPD_TIME_LEN);
	}

	node = mxmlFindElement(response_node, tree, "Alarmstatus", "Num", NULL, MXML_DESCEND);
	if(node)
	{
		int k=0;
		for(item_node = mxmlFindElement(node, tree, "Item",NULL, NULL,MXML_DESCEND);
			item_node != NULL;
			item_node = mxmlFindElement(item_node, tree, "Item", NULL, NULL, MXML_DESCEND))
		{
			memset(&m_state_rsp.state_items[k], 0, sizeof(sipd_alarm_status_item));

			node = mxmlFindElement(item_node, tree, "DeviceID", NULL, NULL,MXML_DESCEND);
			if(node)
				memcpy(m_state_rsp.state_items[k].device_id, (void *)mxmlGetText(node, NULL), \
				sizeof(m_state_rsp.state_items[k].device_id));

			node = mxmlFindElement(response_node, tree, "DutyStatus", NULL, NULL,MXML_DESCEND);
			if(node)
			{
				char strSumNum[MAX_SIPD_NORMAL_LEN];
				memset(strSumNum, 0 ,MAX_SIPD_NORMAL_LEN);
				memcpy(strSumNum, (void *)mxmlGetText(node, NULL),MAX_SIPD_NORMAL_LEN );
				if(string(strSumNum)=="ONDUTY")
					m_state_rsp.state_items[k].Status = SIPD_ALARM_STATUS_ONDUTY;
				else if(string(strSumNum)=="OFFDUTY")
					m_state_rsp.state_items[k].Status = SIPD_ALARM_STATUS_OFFDUTY;
				else
					m_state_rsp.state_items[k].Status = SIPD_ALARM_STATUS_ALARM;
			}
			
			k++;
		}

		m_state_rsp.basic_info.AlaramDeviceNum = k;
	}

	return true;
}

bool CDeviceStatusQuery::handle_request()
{
	if(string(envir().m_cfg.local_domain_id)== string(m_state_req.device_id))
	{
		//	要求查找本域的状态

	}
	else
	{
		//	要求查找本域下的某个设备，以背靠背方式，代发请求
		CDeviceStatusQuery * pCatalog = new CDeviceStatusQuery(envir());
		if(pCatalog)
		{
			sipd_devicestate_req req;
			memset(&req, 0 , sizeof(sipd_devicestate_req));
			strncpy(req.device_id, m_state_req.device_id, MAX_SIPD_DEVICE_ID_LEN);
			snprintf(req.from_uri, sizeof(req.from_uri), "sip:%s@%s:%d",\
				envir().m_cfg.local_domain_id, envir().m_cfg.local_ip, envir().m_cfg.local_port);

			string nextHopUri = envir().GetNextHopUri(m_state_req.device_id);
			strncpy(req.to_uri, nextHopUri.c_str(), MAX_SIPD_URI);

			pCatalog->send_request(&req, this);
		}
	}
	return true;
}

bool CDeviceStatusQuery::handle_response()
{

	if(m_next)
	{
		//	该回复是由于以背靠背方式代发请求 而产生的回复
		m_next->send_response_from(this);
	}
	Medium::close(this);

	return true;
}

void CDeviceStatusQuery::send_request(sipd_devicestate_req  * pReq, CDeviceStatusQuery * pDeviceInfo)
{
	//	请求本域为本域编码，则回复本域下注册的所有 设备

	memcpy(&m_state_req, pReq, sizeof(sipd_devicestate_req));
	strncpy(m_state_req.SN, name(), MAX_SIPD_DEVICE_ID_LEN );

	sipd_message_query_or_response  msg;
	memset(&msg, 0 , sizeof(sipd_message_query_or_response));
	strncpy(msg.to_uri, m_state_req.to_uri, MAX_SIPD_URI);
	strncpy(msg.from_uri, m_state_req.from_uri,  MAX_SIPD_URI);

	snprintf(msg.body, MAX_SIPD_BODY_LENGTH,
		"<?xml version=\"1.0\"?>\r\n"\
		"<Query>\r\n"\
		"<CmdType>DeviceStatus</CmdType>\r\n"\
		"<SN>%s</SN>\r\n"\
		"<DeviceID>%s</DeviceID>\r\n"\
		"</Query>",
		m_state_req.SN, m_state_req.device_id);

	send_message_response(&msg, NULL);
	m_next = pDeviceInfo;

}


void CDeviceStatusQuery::send_response( sipd_stateinfo_adaptor_rsp* pStateinfoRsp)
{
	//	请求本域为本域编码，则回复本域下注册的所有 设备

	sipd_message_query_or_response  msg;
	memset(&msg, 0 , sizeof(sipd_message_query_or_response));
	strncpy(msg.to_uri, pStateinfoRsp->basic_info.to_uri, MAX_SIPD_URI);
	strncpy(msg.from_uri, pStateinfoRsp->basic_info.from_uri,  MAX_SIPD_URI);

	if(NULL == pStateinfoRsp)
	{
		printf("sipd_dev_info_rsp inout err\n");
		return ;
	}

	int len = 0;
	memset(msg.body, 0, sizeof(msg.body));
	if(pStateinfoRsp->basic_info.Result != SIPD_RESULT_OK)
	{
		snprintf(msg.body, sizeof(msg.body),
			"<?xml version=\"1.0\"?>\r\n"\
			"<Response>\r\n"\
			"<CmdType>DeviceStatus</CmdType>\r\n"\
			"<SN>%s</SN>\r\n"\
			"<DeviceID>%s</DeviceID>\r\n"\
			"<Result>ERROR</Result>\r\n"\
			"</Response>",
			pStateinfoRsp->basic_info.SN, pStateinfoRsp->basic_info.device_id,
			(pStateinfoRsp->basic_info.Encode== SIPD_STATUS_ON)?"ON":"OFF", 
			(pStateinfoRsp->basic_info.Record== SIPD_STATUS_OFF)?"OFF":"ON");  

	}
	else
	{
		snprintf(msg.body, sizeof(msg.body),
			"<?xml version=\"1.0\"?>\r\n"\
			"<Response>\r\n"\
			"<CmdType>DeviceStatus</CmdType>\r\n"\
			"<SN>%s</SN>\r\n"\
			"<DeviceID>%s</DeviceID>\r\n"\
			"<Result>OK</Result>\r\n",
			pStateinfoRsp->basic_info.SN, pStateinfoRsp->basic_info.device_id); 

		if((pStateinfoRsp->basic_info.Online== SIPD_STATUS_ONLINE_TYPE_ONLINE))
		{
			len = strlen(msg.body);
			snprintf(msg.body+len, (MAX_XML_BUFFER_LEN-len),"<Online>%s</Online>\r\n", "ONLINE");
		}
		else
		{
			len = strlen(msg.body);
			snprintf(msg.body+len, (MAX_XML_BUFFER_LEN-len),"<Online>%s</Online>\r\n", "OFFLINE");
		}

		if(pStateinfoRsp->basic_info.Status == SIPD_RESULT_OK)
		{
			len = strlen(msg.body);
			snprintf(msg.body+len, (MAX_XML_BUFFER_LEN-len),"<Status>%s</Status>\r\n", "OK");
		}
		else
		{
			len = strlen(msg.body);
			snprintf(msg.body+len, (MAX_XML_BUFFER_LEN-len),"<Status>%s</Status>\r\n", "ERROR");

			len = strlen(msg.body);
			snprintf(msg.body+len, (MAX_XML_BUFFER_LEN-len),"<Reason>%s</Reason>\r\n", pStateinfoRsp->basic_info.Reason);
		}


		if(pStateinfoRsp->basic_info.Encode == SIPD_STATUS_ON)
		{
			len = strlen(msg.body);
			snprintf(msg.body+len, (MAX_XML_BUFFER_LEN-len),"<Encode>%s</Encode>\r\n", "ON");
		}
		else
		{
			len = strlen(msg.body);
			snprintf(msg.body+len, (MAX_XML_BUFFER_LEN-len),"<Encode>%s</Encode>\r\n", "OFF");
		}

		if(pStateinfoRsp->basic_info.Record== SIPD_STATUS_ON)
		{
			len = strlen(msg.body);
			snprintf(msg.body+len, (MAX_XML_BUFFER_LEN-len),"<Record>%s</Record>\r\n", "ON");
		}
		else
		{
			len = strlen(msg.body);
			snprintf(msg.body+len, (MAX_XML_BUFFER_LEN-len),"<Record>%s</Record>\r\n", "OFF");
		}

		len = strlen(msg.body);
		snprintf(msg.body+len, (MAX_XML_BUFFER_LEN-len),"<DeviceTime>%s</DeviceTime>\r\n", pStateinfoRsp->basic_info.DeviceTime);

		int almnum = pStateinfoRsp->basic_info.AlaramDeviceNum;
		if(almnum > 0)
		{
			len = strlen(msg.body);
			snprintf(msg.body+len, (MAX_XML_BUFFER_LEN-len),"<Alarmstatus Num=\"%d\">\r\n", pStateinfoRsp->basic_info.AlaramDeviceNum);

			for(int i=0; i<almnum; i++)
			{
				len = strlen(msg.body);
				snprintf(msg.body+len, (MAX_XML_BUFFER_LEN-len),"<Item>\r\n");

				len = strlen(msg.body);
				snprintf(msg.body+len, (MAX_XML_BUFFER_LEN-len),"<DeviceID>%s</DeviceID>\r\n", pStateinfoRsp->state_items[i].device_id);

				if(SIPD_ALARM_STATUS_ONDUTY == pStateinfoRsp->state_items[i].Status)
				{
					len = strlen(msg.body);
					snprintf(msg.body+len, (MAX_XML_BUFFER_LEN-len),"<DutyStatus>%s</DutyStatus>\r\n", "ONDUTY");
				}
				else if(SIPD_ALARM_STATUS_OFFDUTY == pStateinfoRsp->state_items[i].Status)
				{
					len = strlen(msg.body);
					snprintf(msg.body+len, (MAX_XML_BUFFER_LEN-len),"<DutyStatus>%s</DutyStatus>\r\n", "OFFDUTY");
				}
				else if(SIPD_ALARM_STATUS_ALARM== pStateinfoRsp->state_items[i].Status)
				{
					len = strlen(msg.body);
					snprintf(msg.body+len, (MAX_XML_BUFFER_LEN-len),"<DutyStatus>%s</DutyStatus>\r\n", "ALARM");
				}
				else
				{
					len = strlen(msg.body);
					snprintf(msg.body+len, (MAX_XML_BUFFER_LEN-len),"<DutyStatus>%s</DutyStatus>\r\n", "ONDUTY");
				}

				len = strlen(msg.body);
				snprintf(msg.body+len, (MAX_XML_BUFFER_LEN-len),"</Item>\r\n");

			}
			len = strlen(msg.body);
			snprintf(msg.body+len, (MAX_XML_BUFFER_LEN-len),"</Alarmstatus>\r\n");
		}

		len = strlen(msg.body);
		snprintf(msg.body+len, (MAX_XML_BUFFER_LEN-len),"</Response>");
	}

	send_message_response(&msg, NULL);
}


void CDeviceStatusQuery::send_response_from(CDeviceStatusQuery * pCatalog)
{
	//	根据 被靠被转发而获取的回复，进行回复
	sipd_stateinfo_adaptor_rsp  cataBasicInfo;
	memset(&cataBasicInfo, 0, sizeof(sipd_stateinfo_adaptor_rsp));
	memcpy(&cataBasicInfo, &pCatalog->m_state_rsp, sizeof(sipd_stateinfo_adaptor_rsp));

	strncpy(cataBasicInfo.basic_info.to_uri , m_state_req.from_uri, MAX_SIPD_URI);
	strncpy(cataBasicInfo.basic_info.from_uri , m_state_req.to_uri, MAX_SIPD_URI);
	strncpy(cataBasicInfo.basic_info.SN , m_state_req.SN, MAX_SIPD_DEVICE_ID_LEN);
	strncpy(cataBasicInfo.basic_info.device_id , m_state_req.device_id, MAX_SIPD_DEVICE_ID_LEN);

	string nextHopUri = envir().GetNextHopUri(cataBasicInfo.basic_info.to_uri);
	strncpy(cataBasicInfo.basic_info.to_uri ,nextHopUri.c_str(), MAX_SIPD_URI);
	send_response(&cataBasicInfo);

	Medium::close(this);
}