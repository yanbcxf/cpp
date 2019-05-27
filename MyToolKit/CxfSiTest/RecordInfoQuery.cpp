#include "StdAfx.h"
#include "GbUsageEnvironment.h"
#include "CatalogQuery.h"

#include "RecordInfoQuery.h"

CRecordInfoQuery::CRecordInfoQuery(CGbUsageEnvironment& env)
:CXmlQuery(env)
{
}

CRecordInfoQuery::~CRecordInfoQuery(void)
{
}


bool CRecordInfoQuery::parse_request(mxml_node_t *tree, mxml_node_t *query_node, sipd_message_query_or_response  * pReq)
{
	int whitespace;
	mxml_node_t *node;

	string strDeviceId;
	string strSN;
	if(CXmlQuery::parse_deviceId_and_SN(tree, query_node, strDeviceId, strSN)==false)
		return false;

	memset(&m_recordinfo_req, 0, sizeof(sipd_recordinfo_req));
	strncpy(m_recordinfo_req.to_uri, pReq->to_uri, MAX_SIPD_URI);
	strncpy(m_recordinfo_req.from_uri, pReq->from_uri,  MAX_SIPD_URI);
	strncpy(m_recordinfo_req.SN, strSN.c_str(), MAX_SIPD_DEVICE_ID_LEN);
	strncpy(m_recordinfo_req.device_id, strDeviceId.c_str(), MAX_SIPD_DEVICE_ID_LEN);

	node = mxmlFindElement(tree, tree, XML_NAME_StartTime, NULL, NULL, MXML_DESCEND);
	if(NULL != node)
	{	
		// printf("%s line=%d  StartTime=%s\n", __FUNCTION__, __LINE__, mxmlGetText(node, &whitespace));
		strncpy(m_recordinfo_req.StartTime, mxmlGetText(node, &whitespace), MAX_SIPD_TIME_LEN);
	}

	node = mxmlFindElement(tree, tree, XML_NAME_EndTime, NULL, NULL, MXML_DESCEND);
	if(NULL != node)
	{	
		// printf( "%s line=%d  EndTime=%s\n", __FUNCTION__, __LINE__, mxmlGetText(node, &whitespace));
		strncpy(m_recordinfo_req.EndTime, mxmlGetText(node, &whitespace), MAX_SIPD_TIME_LEN);
	}

	node = mxmlFindElement(tree, tree, XML_NAME_FilePath, NULL, NULL, MXML_DESCEND);
	if(NULL != node)
	{	
		//printf("%s line=%d  FilePath=%s\n", __FUNCTION__, __LINE__, mxmlGetText(node, &whitespace));
		strncpy(m_recordinfo_req.FilePath, mxmlGetText(node, &whitespace), MAX_SIPD_URI);
	}

	node = mxmlFindElement(tree, tree, XML_NAME_Address, NULL, NULL, MXML_DESCEND);
	if(NULL != node)
	{	
		//printf("%s line=%d  Address=%s\n", __FUNCTION__, __LINE__, mxmlGetText(node, &whitespace));
		strncpy(m_recordinfo_req.Address, mxmlGetText(node, &whitespace), MAX_SIPD_URI);
	}

	node = mxmlFindElement(tree, tree, XML_NAME_Secrecy, NULL, NULL, MXML_DESCEND);
	if(NULL != node)
	{	
		//printf("%s line=%d  Secrecy=%s\n", __FUNCTION__, __LINE__, mxmlGetText(node, &whitespace));
		m_recordinfo_req.Secrecy = atoi(mxmlGetText(node, &whitespace));
	}

	node = mxmlFindElement(tree, tree, XML_NAME_Type, NULL, NULL, MXML_DESCEND);
	if(NULL != node)
	{	
		// printf("%s line=%d  Type=%s\n", __FUNCTION__, __LINE__, mxmlGetText(node, &whitespace));
		if(strcmp(mxmlGetText(node, &whitespace), XML_Value_Type_time) == 0)
		{
			m_recordinfo_req.Type = SIPD_RECORD_INFO_TYPE_TIME;
		}
		else if(strcmp(mxmlGetText(node, &whitespace), XML_Value_Type_alarm) == 0)
		{
			m_recordinfo_req.Type = SIPD_RECORD_INFO_TYPE_ALARM;
		}
		else if(strcmp(mxmlGetText(node, &whitespace), XML_Value_Type_manual) == 0)
		{
			m_recordinfo_req.Type = SIPD_RECORD_INFO_TYPE_MANUAL;
		}
		else if(strcmp(mxmlGetText(node, &whitespace), XML_Value_Type_all) == 0)
		{
			m_recordinfo_req.Type = SIPD_RECORD_INFO_TYPE_ALL;
		}
	}

	node = mxmlFindElement(tree, tree, XML_NAME_RecorderID, NULL, NULL, MXML_DESCEND);
	if(NULL != node)
	{	
		// printf("%s line=%d  RecorderID=%s\n", __FUNCTION__, __LINE__, mxmlGetText(node, &whitespace));
		strncpy(m_recordinfo_req.RecorderID, mxmlGetText(node, &whitespace), MAX_SIPD_DEVICE_ID_LEN);
	}
	

	return true;
}




bool CRecordInfoQuery::parse_response(mxml_node_t *tree, mxml_node_t *response_node,  sipd_message_query_or_response  * pReq)
{
	int whitespace;
	mxml_node_t  *node, * item_node;

	string strDeviceId;
	string strSN;
	if(CXmlQuery::parse_deviceId_and_SN(tree, response_node,strDeviceId, strSN)==false)
		return false;

	memset(&m_recordinfo_rsp, 0, sizeof(sipd_recordinfo_adaptor_rsp));
	strncpy(m_recordinfo_rsp.basic_info.to_uri, pReq->to_uri, MAX_SIPD_URI);
	strncpy(m_recordinfo_rsp.basic_info.from_uri, pReq->from_uri,  MAX_SIPD_URI);
	strncpy(m_recordinfo_rsp.basic_info.SN, strSN.c_str(), MAX_SIPD_DEVICE_ID_LEN);
	strncpy(m_recordinfo_rsp.basic_info.device_id, strDeviceId.c_str(), MAX_SIPD_DEVICE_ID_LEN);

	return true;
}

bool CRecordInfoQuery::handle_request()
{
	if(string(envir().m_cfg.local_domain_id)== string(m_recordinfo_req.device_id))
	{
		//	要求查找本域的状态

	}
	else
	{
		//	要求查找本域下的某个设备，以背靠背方式，代发请求
		CRecordInfoQuery * pCatalog = new CRecordInfoQuery(envir());
		if(pCatalog)
		{
			sipd_recordinfo_req req;
			memset(&req, 0 , sizeof(sipd_recordinfo_req));
			strncpy(req.device_id, m_recordinfo_req.device_id, MAX_SIPD_DEVICE_ID_LEN);
			snprintf(req.from_uri, sizeof(req.from_uri), "sip:%s@%s:%d",\
				envir().m_cfg.local_domain_id, envir().m_cfg.local_ip, envir().m_cfg.local_port);

			string nextHopUri = envir().GetNextHopUri(m_recordinfo_req.device_id);
			strncpy(req.to_uri, nextHopUri.c_str(), MAX_SIPD_URI);

			pCatalog->send_request(&req, this);
		}
	}
	return true;
}

bool CRecordInfoQuery::handle_response()
{

	if(m_next)
	{
		//	该回复是由于以背靠背方式代发请求 而产生的回复
		m_next->send_response_from(this);
	}
	Medium::close(this);

	return true;
}

void CRecordInfoQuery::send_request(sipd_recordinfo_req  * pReq, CRecordInfoQuery * pDeviceInfo)
{
	//	请求本域为本域编码，则回复本域下注册的所有 设备

	memcpy(&m_recordinfo_req, pReq, sizeof(sipd_recordinfo_req));
	strncpy(m_recordinfo_req.SN, name(), MAX_SIPD_DEVICE_ID_LEN );

	sipd_message_query_or_response  msg;
	memset(&msg, 0 , sizeof(sipd_message_query_or_response));
	strncpy(msg.to_uri, m_recordinfo_req.to_uri, MAX_SIPD_URI);
	strncpy(msg.from_uri, m_recordinfo_req.from_uri,  MAX_SIPD_URI);

	snprintf(msg.body, MAX_SIPD_BODY_LENGTH,
		"<?xml version=\"1.0\"?>\r\n"\
		"<Query>\r\n"\
		"<CmdType>DeviceInfo</CmdType>\r\n"\
		"<SN>%s</SN>\r\n"\
		"<DeviceID>%s</DeviceID>\r\n"\
		"</Query>",
		m_recordinfo_req.SN, m_recordinfo_req.device_id);

	send_message_response(&msg, NULL);
	m_next = pDeviceInfo;

}


int CRecordInfoQuery::get_recordinfo_pair(int start_index, char* pitem_pair, sipd_recordinfo_adaptor_rsp *prsp)
{
	int len=0;
	if(NULL == pitem_pair  || NULL == prsp)
	{
		printf("get_recordinfo_pair input param error!\n");
		return -1;
	}

	if(start_index > (prsp->basic_info.RecordListNum-1))
	{
		printf("get_recordinfo_pair already get last item. Warning! RecordListNum=%d\n", prsp->basic_info.RecordListNum);
		return -1;
	}

	/*item1*/
	snprintf(pitem_pair,MAX_SIPD_FILEITEM_LEN,
		"<Item>\r\n"\
		"<DeviceID>%s</DeviceID>\r\n"\
		"<Name>%s</Name>\r\n",
		prsp->file_items[start_index].device_id, prsp->file_items[start_index].Name);


	if(strlen(prsp->file_items[start_index].FilePath) > 0)
	{
		len = strlen(pitem_pair);
		snprintf(pitem_pair+len, (MAX_SIPD_FILEITEM_LEN-len),"<FilePath>%s</FilePath>\r\n", prsp->file_items[start_index].FilePath);
	}

	if(strlen(prsp->file_items[start_index].Address) > 0)
	{
		len = strlen(pitem_pair);
		snprintf(pitem_pair+len, (MAX_SIPD_FILEITEM_LEN-len),"<Address>%s</Address>\r\n", prsp->file_items[start_index].Address);
	}

	if(strlen(prsp->file_items[start_index].StartTime) > 0)
	{
		len = strlen(pitem_pair);
		snprintf(pitem_pair+len, (MAX_SIPD_FILEITEM_LEN-len),"<StartTime>%s</StartTime>\r\n", prsp->file_items[start_index].StartTime);
	}

	if(strlen(prsp->file_items[start_index].EndTime) > 0)
	{
		len = strlen(pitem_pair);
		snprintf(pitem_pair+len, (MAX_SIPD_FILEITEM_LEN-len),"<EndTime>%s</EndTime>\r\n", prsp->file_items[start_index].EndTime);
	}

	len = strlen(pitem_pair);
	snprintf(pitem_pair+len, (MAX_SIPD_FILEITEM_LEN-len),"<Secrecy>%d</Secrecy>\r\n", prsp->file_items[start_index].Secrecy);


	if(SIPD_RECORD_INFO_TYPE_TIME == prsp->file_items[start_index].Type )
	{
		len = strlen(pitem_pair);
		snprintf(pitem_pair+len, (MAX_SIPD_FILEITEM_LEN-len),"<Type>%s</Type>\r\n", XML_Value_Type_time);
	}
	else if(SIPD_RECORD_INFO_TYPE_ALARM == prsp->file_items[start_index].Type )
	{
		len = strlen(pitem_pair);
		snprintf(pitem_pair+len, (MAX_SIPD_FILEITEM_LEN-len),"<Type>%s</Type>\r\n", XML_Value_Type_alarm);
	}
	else if(SIPD_RECORD_INFO_TYPE_MANUAL == prsp->file_items[start_index].Type )
	{
		len = strlen(pitem_pair);
		snprintf(pitem_pair+len, (MAX_SIPD_FILEITEM_LEN-len),"<Type>%s</Type>\r\n", XML_Value_Type_manual);
	}
	else
	{
		len = strlen(pitem_pair);
		snprintf(pitem_pair+len, (MAX_SIPD_FILEITEM_LEN-len),"<Type>%s</Type>\r\n", XML_Value_Type_all);
	}

	if(strlen(prsp->file_items[start_index].RecorderID) > 0)
	{
		len = strlen(pitem_pair);
		snprintf(pitem_pair+len, (MAX_SIPD_FILEITEM_LEN-len),"<RecorderID>%s</RecorderID>\r\n", prsp->file_items[start_index].RecorderID);
	}

	len = strlen(pitem_pair);
	snprintf(pitem_pair+len, (MAX_SIPD_FILEITEM_LEN-len),"</Item>\r\n");

	return 0;
}


void CRecordInfoQuery::send_response( sipd_recordinfo_adaptor_rsp* prsp)
{
	//	请求本域为本域编码，则回复本域下注册的所有 设备

	sipd_message_query_or_response  msg;
	memset(&msg, 0 , sizeof(sipd_message_query_or_response));
	strncpy(msg.to_uri, prsp->basic_info.to_uri, MAX_SIPD_URI);
	strncpy(msg.from_uri, prsp->basic_info.from_uri,  MAX_SIPD_URI);

	if(NULL == prsp)
	{
		printf("sipd_dev_info_rsp inout err\n");
		return ;
	}

	int len = 0;
	if(0 == prsp->basic_info.SumNum)
	{
		snprintf(msg.body , MAX_SIPD_RECORD_INFO_LEN,
			"<?xml version=\"1.0\"?>\r\n"\
			"<Response>\r\n"\
			"<CmdType>RecordInfo</CmdType>\r\n"\
			"<SN>%s</SN>\r\n"\
			"<DeviceID>%s</DeviceID>\r\n"\
			"<Name>%s</Name>\r\n"\
			"<SumNum>%d</SumNum>\r\n"\
			"</Response>",
			prsp->basic_info.SN, prsp->basic_info.device_id,
			prsp->basic_info.Name, prsp->basic_info.SumNum);

	}
	else
	{
		snprintf(msg.body, MAX_SIPD_RECORD_INFO_LEN,
			"<?xml version=\"1.0\"?>\r\n"\
			"<Response>\r\n"\
			"<CmdType>RecordInfo</CmdType>\r\n"\
			"<SN>%s</SN>\r\n"\
			"<DeviceID>%s</DeviceID>\r\n"\
			"<Name>%s</Name>\r\n"\
			"<SumNum>%d</SumNum>\r\n",
			prsp->basic_info.SN, prsp->basic_info.device_id, prsp->basic_info.Name, prsp->basic_info.SumNum);

		len = strlen(msg.body);
		snprintf(msg.body+len, (MAX_SIPD_RECORD_INFO_LEN-len),"<RecordList Num=\"%d\">\r\n", prsp->basic_info.RecordListNum);

		for(int index = 0; index < (prsp->basic_info.RecordListNum); index++)
		{
			char item_pair[MAX_SIPD_FILEITEM_LEN+1];
			memset(&item_pair, 0, sizeof(item_pair));
			int ret = get_recordinfo_pair(index, item_pair, prsp);
			if(ret != 0)
			{
				printf("build_and_send_recordinfo_rsp get_recordinfo_pair error! ret=%d \n", ret);
				break;
			}
			len = strlen(msg.body);
			snprintf(msg.body+len, (MAX_SIPD_RECORD_INFO_LEN-len), item_pair);
		}

		len = strlen(msg.body);
		snprintf(msg.body+len, (MAX_SIPD_RECORD_INFO_LEN-len),"</RecordList>\r\n</Response>");

	}
	send_message_response(&msg, NULL);
}


void CRecordInfoQuery::send_response_from(CRecordInfoQuery * pCatalog)
{
	//	根据 被靠被转发而获取的回复，进行回复
	sipd_recordinfo_adaptor_rsp  cataBasicInfo;
	memset(&cataBasicInfo, 0, sizeof(sipd_recordinfo_adaptor_rsp));
	memcpy(&cataBasicInfo, &pCatalog->m_recordinfo_rsp, sizeof(sipd_recordinfo_adaptor_rsp));

	strncpy(cataBasicInfo.basic_info.to_uri , m_recordinfo_req.from_uri, MAX_SIPD_URI);
	strncpy(cataBasicInfo.basic_info.from_uri , m_recordinfo_req.to_uri, MAX_SIPD_URI);
	strncpy(cataBasicInfo.basic_info.SN , m_recordinfo_req.SN, MAX_SIPD_DEVICE_ID_LEN);
	strncpy(cataBasicInfo.basic_info.device_id , m_recordinfo_req.device_id, MAX_SIPD_DEVICE_ID_LEN);

	string nextHopUri = envir().GetNextHopUri(cataBasicInfo.basic_info.to_uri);
	strncpy(cataBasicInfo.basic_info.to_uri ,nextHopUri.c_str(), MAX_SIPD_URI);
	send_response(&cataBasicInfo);

	Medium::close(this);
}