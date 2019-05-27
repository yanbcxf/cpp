#include "StdAfx.h"

#include "GbUsageEnvironment.h"
#include "CatalogQuery.h"

CCatalogQuery::CCatalogQuery(CGbUsageEnvironment& env)
:CXmlQuery(env)
{
	m_next = NULL;
	m_current_item = 0;
}

CCatalogQuery::~CCatalogQuery(void)
{
}


void CCatalogQuery::send_request(sipd_catalog_req  * pReq, CCatalogQuery * pCatalog)
{
	//	请求本域为本域编码，则回复本域下注册的所有 设备

	memcpy(&m_catalog_req, pReq, sizeof(sipd_catalog_req));
	strncpy(m_catalog_req.SN, name(), MAX_SIPD_DEVICE_ID_LEN );
	Sip_cataloginfo_request(&m_catalog_req);

	m_next = pCatalog;

		/*sipd_catalog_rsp_basic_info  cataBasicInfo;
		strncpy(cataBasicInfo.to_uri , m_catalog_req.to_uri, MAX_SIPD_URI);
		strncpy(cataBasicInfo.from_uri , m_catalog_req.from_uri, MAX_SIPD_URI);
		strncpy(cataBasicInfo.SN , m_catalog_req.SN, MAX_SIPD_DEVICE_ID_LEN);
		strncpy(cataBasicInfo.device_id , m_catalog_req.device_id, MAX_SIPD_DEVICE_ID_LEN);
		Siqd_catalog_query_response(&cataBasicInfo , NULL);*/
}

bool CCatalogQuery::parse_request(mxml_node_t *tree, mxml_node_t *query_node, sipd_message_query_or_response  * pReq)
{
	int whitespace;
	mxml_node_t  *node;
	string strDeviceId;
	string strSN;
	if(CXmlQuery::parse_deviceId_and_SN(tree, query_node,strDeviceId, strSN)==false)
		return false;
	memset(&m_catalog_req, 0, sizeof(sipd_catalog_req));
	strncpy(m_catalog_req.to_uri, pReq->to_uri, MAX_SIPD_URI);
	strncpy(m_catalog_req.from_uri, pReq->from_uri,  MAX_SIPD_URI);
	strncpy(m_catalog_req.SN, strSN.c_str(), MAX_SIPD_DEVICE_ID_LEN);
	strncpy(m_catalog_req.device_id, strDeviceId.c_str(), MAX_SIPD_DEVICE_ID_LEN);

	node = mxmlFindElement(query_node, tree, XML_NAME_StartTime, NULL, NULL, MXML_DESCEND);
	if(NULL != node)
	{	
		strncpy(m_catalog_req.StartTime, mxmlGetText(node, &whitespace), MAX_SIPD_TIME_LEN);
	}

	node = mxmlFindElement(query_node, tree, XML_NAME_EndTime, NULL, NULL, MXML_DESCEND);
	if(NULL != node)
	{	
		strncpy(m_catalog_req.EndTime, mxmlGetText(node, &whitespace), MAX_SIPD_TIME_LEN);
	}

	return true;
}

bool CCatalogQuery::handle_request()
{
	if(string(envir().m_cfg.local_domain_id)== string(m_catalog_req.device_id))
	{
		//	要求查找本域的直接下属
		send_response_from_enviroment();
	}
	else
	{
		//	要求查找本域下的某个设备，以背靠背方式，代发请求
		CCatalogQuery * pCatalog = new CCatalogQuery(envir());
		if(pCatalog)
		{
			sipd_catalog_req req;
			memset(&req, 0 , sizeof(sipd_catalog_req));
			strncpy(req.device_id, m_catalog_req.device_id, MAX_SIPD_DEVICE_ID_LEN);
			snprintf(req.from_uri, sizeof(req.from_uri), "sip:%s@%s:%d",\
				envir().m_cfg.local_domain_id, envir().m_cfg.local_ip, envir().m_cfg.local_port);
			
			string nextHopUri = envir().GetNextHopUri(m_catalog_req.device_id);
			strncpy(req.to_uri, nextHopUri.c_str(), MAX_SIPD_URI);

			pCatalog->send_request(&req, this);
		}
	}
	return true;
}


bool CCatalogQuery::parse_response(mxml_node_t *tree, mxml_node_t *response_node,  sipd_message_query_or_response  * pReq)
{
	int whitespace;
	mxml_node_t  *node, * item_node;

	string strDeviceId;
	string strSN;
	if(CXmlQuery::parse_deviceId_and_SN(tree, response_node,strDeviceId, strSN)==false)
		return false;

	stringstream ss;
	/*ss << "CCatalogQuery::parse_response m_current_item = " << m_current_item;
	LOG4CPLUS_DEBUG(envir().m_Logger, ss.str().c_str());*/

	memset(&m_basic_info, 0, sizeof(sipd_catalog_rsp_basic_info));
	strncpy(m_basic_info.to_uri, pReq->to_uri, MAX_SIPD_URI);
	strncpy(m_basic_info.from_uri, pReq->from_uri,  MAX_SIPD_URI);
	strncpy(m_basic_info.SN, strSN.c_str(), MAX_SIPD_DEVICE_ID_LEN);
	strncpy(m_basic_info.device_id, strDeviceId.c_str(), MAX_SIPD_DEVICE_ID_LEN);

	node = mxmlFindElement(response_node, tree, "SumNum", NULL, NULL,MXML_DESCEND);
	if(node)
	{
		char strSumNum[MAX_SIPD_NORMAL_LEN];
		memset(strSumNum, 0 ,MAX_SIPD_NORMAL_LEN);
		strncpy(strSumNum, (const char *)mxmlGetText(node, NULL),MAX_SIPD_NORMAL_LEN );
		m_basic_info.SumNum = atoi(strSumNum);
	}

	node = mxmlFindElement(response_node, tree, "DeviceList", "Num", NULL, MXML_DESCEND);
	if(node)
	{
		//memcpy(m_basic_info.DeviceList_Num, (void *)mxmlGetText(node, NULL), sizeof(m_basic_info.SumNum));
		
		for(item_node = mxmlFindElement(node, tree, "Item",NULL, NULL,MXML_DESCEND);
			item_node != NULL;
			item_node = mxmlFindElement(item_node, tree, "Item", NULL, NULL, MXML_DESCEND))
		{
			memset(&m_cata_items[m_current_item], 0, sizeof(sipd_catalog_item));

			node = mxmlFindElement(item_node, tree, "DeviceID", NULL, NULL,MXML_DESCEND);
			if(node)
				strncpy(m_cata_items[m_current_item].device_id, (const char *)mxmlGetText(node, NULL), \
					sizeof(m_cata_items[m_current_item].device_id));

			node = mxmlFindElement(item_node, tree, "Name", NULL, NULL,MXML_DESCEND);
			if(node)
				strncpy(m_cata_items[m_current_item].Name, (const char  *)mxmlGetText(node, NULL), \
				sizeof(m_cata_items[m_current_item].Name));

			node = mxmlFindElement(item_node, tree, "Manufacturer", NULL, NULL,MXML_DESCEND);
			if(node)
				strncpy(m_cata_items[m_current_item].Manufacturer, (const char  *)mxmlGetText(node, NULL), \
				sizeof(m_cata_items[m_current_item].Manufacturer));

			node = mxmlFindElement(item_node, tree, "Model", NULL, NULL,MXML_DESCEND);
			if(node)
				strncpy(m_cata_items[m_current_item].Model, (const char  *)mxmlGetText(node, NULL), \
				sizeof(m_cata_items[m_current_item].Model));

			
			node = mxmlFindElement(item_node, tree, "Owner", NULL, NULL,MXML_DESCEND);
			if(node)
				strncpy(m_cata_items[m_current_item].Owner, (const char  *)mxmlGetText(node, NULL), \
				sizeof(m_cata_items[m_current_item].Owner));

			node = mxmlFindElement(item_node, tree, "CivilCode", NULL, NULL,MXML_DESCEND);
			if(node)
				strncpy(m_cata_items[m_current_item].CivilCode, (const char  *)mxmlGetText(node, NULL), \
				sizeof(m_cata_items[m_current_item].CivilCode));

			node = mxmlFindElement(item_node, tree, "Block", NULL, NULL,MXML_DESCEND);
			if(node)
				strncpy(m_cata_items[m_current_item].Block, (const char  *)mxmlGetText(node, NULL), \
				sizeof(m_cata_items[m_current_item].Block));

			node = mxmlFindElement(item_node, tree, "Address", NULL, NULL,MXML_DESCEND);
			if(node)
				strncpy(m_cata_items[m_current_item].Address, (const char  *)mxmlGetText(node, NULL), \
				sizeof(m_cata_items[m_current_item].Address));

			/*node = mxmlFindElement(item_node, tree, "Parental", NULL, NULL,MXML_DESCEND);
			if(node)
				memcpy(m_cata_items[m_current_item].Address, (void *)mxmlGetText(node, NULL), \
				sizeof(m_cata_items[m_current_item].Address));*/

			node = mxmlFindElement(item_node, tree, "ParentID", NULL, NULL,MXML_DESCEND);
			if(node)
				strncpy(m_cata_items[m_current_item].ParentID, (const char  *)mxmlGetText(node, NULL), \
				sizeof(m_cata_items[m_current_item].ParentID));

			m_current_item++;
		}
	}
	return true;
}

bool CCatalogQuery::handle_response()
{
	if(m_basic_info.SumNum<=m_current_item)
	{
		//	目录项接受完全，开始处理
		m_basic_info.DeviceList_Num = m_current_item;
		if(m_next)
		{
			//	该回复是由于以背靠背方式代发请求 而产生的回复
			m_next->send_response_from(this);
		}
		else
		{
			//	该回复系由于自身维护目录树的需求而发出的请求
			envir().update_catalog_from(this);
		}
		Medium::close(this);
	}
	return true;
}


void CCatalogQuery::send_response_from_enviroment()
{
	//	回复本域下注册的所有设备或者 子域
	sipd_catalog_rsp_basic_info  cataBasicInfo;
	memset(&cataBasicInfo, 0 , sizeof(sipd_catalog_rsp_basic_info));
	strncpy(cataBasicInfo.to_uri , m_catalog_req.from_uri, MAX_SIPD_URI);
	strncpy(cataBasicInfo.from_uri , m_catalog_req.to_uri, MAX_SIPD_URI);
	strncpy(cataBasicInfo.SN , m_catalog_req.SN, MAX_SIPD_DEVICE_ID_LEN);
	strncpy(cataBasicInfo.device_id , m_catalog_req.device_id, MAX_SIPD_DEVICE_ID_LEN);

	string nextHopUri = envir().GetNextHopUri(cataBasicInfo.to_uri);
	if(nextHopUri.empty()==false)
	{
		strncpy(cataBasicInfo.to_uri , nextHopUri.c_str(), MAX_SIPD_URI);
	}

	if(envir().m_catalog_tree.size()==0)
	{
		cataBasicInfo.SumNum = 0;
		send_response(&cataBasicInfo , NULL);
	}
	else 
	{
		sipd_catalog_item* pCataItem = new sipd_catalog_item[envir().m_catalog_tree.size()];
		
		
		int index = 0;
		map<string, CCatalogTree *>::iterator it = envir().m_catalog_tree.begin();
		for(;  it!=envir().m_catalog_tree.end(); it++)
		{
			CCatalogTree * pTree = it->second;
			sipd_catalog_item * pCataItemTmp = pTree->FindCatalogItem(pTree->m_username);
			if(pCataItemTmp)
			{
				memcpy(&pCataItem[index], pCataItemTmp, sizeof(sipd_catalog_item));
				index++;
			}
		}
		cataBasicInfo.SumNum = index;
		cataBasicInfo.DeviceList_Num = index;

		send_response(&cataBasicInfo , pCataItem);
		delete[] pCataItem;
	}

	
}

void CCatalogQuery::send_response_from(CCatalogQuery * pCatalog)
{
	//	根据 被靠被转发而获取的回复，进行回复
	sipd_catalog_rsp_basic_info  cataBasicInfo;
	memset(&cataBasicInfo, 0, sizeof(sipd_catalog_rsp_basic_info));
	strncpy(cataBasicInfo.to_uri , m_catalog_req.from_uri, MAX_SIPD_URI);
	strncpy(cataBasicInfo.from_uri , m_catalog_req.to_uri, MAX_SIPD_URI);
	strncpy(cataBasicInfo.SN , m_catalog_req.SN, MAX_SIPD_DEVICE_ID_LEN);
	strncpy(cataBasicInfo.device_id , m_catalog_req.device_id, MAX_SIPD_DEVICE_ID_LEN);

	string nextHopUri = envir().GetNextHopUri(cataBasicInfo.to_uri);
	strncpy(cataBasicInfo.to_uri ,nextHopUri.c_str(), MAX_SIPD_URI);
	send_response(&cataBasicInfo , pCatalog->m_cata_items);

	Medium::close(this);

}

void CCatalogQuery::send_response(sipd_catalog_rsp_basic_info* pCataBasicInfo, sipd_catalog_item* pCataItem)
{

	sipd_message_query_or_response  msg;
	memset(&msg, 0 , sizeof(sipd_message_query_or_response));
	strncpy(msg.to_uri, pCataBasicInfo->to_uri, MAX_SIPD_URI);
	strncpy(msg.from_uri, pCataBasicInfo->from_uri,  MAX_SIPD_URI);
	
	int len =0;
	if(0 == pCataBasicInfo->SumNum)
	{
		snprintf(msg.body, MAX_SIPD_RECORD_INFO_LEN,
			"<?xml version=\"1.0\"?>\r\n"\
			"<Response>\r\n"\
			"<CmdType>Catalog</CmdType>\r\n"\
			"<SN>%s</SN>\r\n"\
			"<DeviceID>%s</DeviceID>\r\n"\
			"<SumNum>%d</SumNum>\r\n"\
			"</Response>",
			pCataBasicInfo->SN, pCataBasicInfo->device_id, pCataBasicInfo->SumNum);

	}
	else
	{
		snprintf(msg.body, MAX_SIPD_RECORD_INFO_LEN,
			"<?xml version=\"1.0\"?>\r\n"\
			"<Response>\r\n"\
			"<CmdType>Catalog</CmdType>\r\n"\
			"<SN>%s</SN>\r\n"\
			"<DeviceID>%s</DeviceID>\r\n"\
			"<SumNum>%d</SumNum>\r\n",
			pCataBasicInfo->SN, pCataBasicInfo->device_id, pCataBasicInfo->SumNum);

		len = strlen(msg.body);
		snprintf(msg.body+len, (MAX_SIPD_RECORD_INFO_LEN-len),"<DeviceList Num=\"%d\">\r\n",pCataBasicInfo->DeviceList_Num);

		for(int index = 0; index < (pCataBasicInfo->DeviceList_Num); index++)
		{
			char item_pair[MAX_SIPD_RECORD_INFO_LEN+1];
			memset(item_pair, 0, sizeof(item_pair));
			int ret = get_cataloginfo_pair(index, item_pair, pCataItem );
			if(ret != 0)
			{
				printf("sipd_dev_catalog_rsp get_cataloginfo_pair error! ret=%d \n", ret);
				break;
			}

			len = strlen(msg.body);
			snprintf(msg.body+len, (MAX_SIPD_RECORD_INFO_LEN-len), item_pair);
		}

		len = strlen(msg.body);
		snprintf(msg.body+len, (MAX_SIPD_RECORD_INFO_LEN-len),"</DeviceList>\r\n</Response>");

	}
	send_message_response(&msg, NULL);
}



int CCatalogQuery::get_cataloginfo_pair(int start_index, char* pitem_pair, sipd_catalog_item * pCataItem)
{
	int len=0;
	if(NULL == pitem_pair  || NULL == pCataItem)
	{
		printf("get_recordinfo_pair input param error!\n");
		return -1;
	}

	
	/*item1*/
	snprintf(pitem_pair,MAX_SIPD_FILEITEM_LEN,
		"<Item>\r\n"\
		"<DeviceID>%s</DeviceID>\r\n",
		pCataItem[start_index].device_id);

	if(strlen(pCataItem[start_index].Name) > 0)
	{
		len = strlen(pitem_pair);
		snprintf(pitem_pair+len, (MAX_SIPD_FILEITEM_LEN-len),"<Name>%s</Name>\r\n", pCataItem[start_index].Name);
	}

	if(strlen(pCataItem[start_index].Manufacturer) > 0)
	{
		len = strlen(pitem_pair);
		snprintf(pitem_pair+len, (MAX_SIPD_FILEITEM_LEN-len),"<Manufacturer>%s</Manufacturer>\r\n", pCataItem[start_index].Manufacturer);
	}

	if(strlen(pCataItem[start_index].Model) > 0)
	{
		len = strlen(pitem_pair);
		snprintf(pitem_pair+len, (MAX_SIPD_FILEITEM_LEN-len),"<Model>%s</Model>\r\n", pCataItem[start_index].Model);
	}

	if(strlen(pCataItem[start_index].Owner) > 0)
	{
		len = strlen(pitem_pair);
		snprintf(pitem_pair+len, (MAX_SIPD_FILEITEM_LEN-len),"<Owner>%s</Owner>\r\n", pCataItem[start_index].Owner);
	}

	if(strlen(pCataItem[start_index].CivilCode) > 0)
	{
		len = strlen(pitem_pair);
		snprintf(pitem_pair+len, (MAX_SIPD_FILEITEM_LEN-len),"<CivilCode>%s</CivilCode>\r\n", pCataItem[start_index].CivilCode);
	}

	if(strlen(pCataItem[start_index].Block) > 0)
	{
		len = strlen(pitem_pair);
		snprintf(pitem_pair+len, (MAX_SIPD_FILEITEM_LEN-len),"<Block>%s</Block>\r\n", pCataItem[start_index].Block);
	}

	if(strlen(pCataItem[start_index].Address) > 0)
	{
		len = strlen(pitem_pair);
		snprintf(pitem_pair+len, (MAX_SIPD_FILEITEM_LEN-len),"<Address>%s</Address>\r\n", pCataItem[start_index].Address);
	}

	len = strlen(pitem_pair);
	snprintf(pitem_pair+len, (MAX_SIPD_FILEITEM_LEN-len),"<Parental>%d</Parental>\r\n", pCataItem[start_index].Parental);

	if(strlen(pCataItem[start_index].ParentID) > 0)
	{
		len = strlen(pitem_pair);
		snprintf(pitem_pair+len, (MAX_SIPD_FILEITEM_LEN-len),"<ParentID>%s</ParentID>\r\n", pCataItem[start_index].ParentID);
	}

	len = strlen(pitem_pair);
	snprintf(pitem_pair+len, (MAX_SIPD_FILEITEM_LEN-len),"<SafetyWay>%d</SafetyWay>\r\n", pCataItem[start_index].SafteyWay);

	len = strlen(pitem_pair);
	snprintf(pitem_pair+len, (MAX_SIPD_FILEITEM_LEN-len),"<RegisterWay>%d</RegisterWay>\r\n", pCataItem[start_index].RegisterWay);

	if(strlen(pCataItem[start_index].CertNum) > 0)
	{
		len = strlen(pitem_pair);
		snprintf(pitem_pair+len, (MAX_SIPD_FILEITEM_LEN-len),"<CertNum>%s</CertNum>\r\n", pCataItem[start_index].CertNum);
	}

	len = strlen(pitem_pair);
	snprintf(pitem_pair+len, (MAX_SIPD_FILEITEM_LEN-len),"<Certifiable>%d</Certifiable>\r\n", pCataItem[start_index].Certifiable);

	len = strlen(pitem_pair);
	snprintf(pitem_pair+len, (MAX_SIPD_FILEITEM_LEN-len),"<ErrCode>%d</ErrCode>\r\n", pCataItem[start_index].ErrCode);

	if(strlen(pCataItem[start_index].EndTime) > 0)
	{
		len = strlen(pitem_pair);
		snprintf(pitem_pair+len, (MAX_SIPD_FILEITEM_LEN-len),"<EndTime>%s</EndTime>\r\n", pCataItem[start_index].EndTime);
	}

	len = strlen(pitem_pair);
	snprintf(pitem_pair+len, (MAX_SIPD_FILEITEM_LEN-len),"<Secrecy>%d</Secrecy>\r\n", pCataItem[start_index].Secrecy);

	if(strlen(pCataItem[start_index].IPAddress) > 0)
	{
		len = strlen(pitem_pair);
		snprintf(pitem_pair+len, (MAX_SIPD_FILEITEM_LEN-len),"<IPAddress>%s</IPAddress>\r\n", pCataItem[start_index].IPAddress);
	}

	if(pCataItem[start_index].Port != 0)
	{
		len = strlen(pitem_pair);
		snprintf(pitem_pair+len, (MAX_SIPD_FILEITEM_LEN-len),"<Port>%d</Port>\r\n", pCataItem[start_index].Port);
	}

	if(strlen(pCataItem[start_index].Password) > 0)
	{
		len = strlen(pitem_pair);
		snprintf(pitem_pair+len, (MAX_SIPD_FILEITEM_LEN-len),"<Password>%s</Password>\r\n", pCataItem[start_index].Password);
	}

	if(SIPD_STATUS_ON == pCataItem[start_index].Status)
	{
		len = strlen(pitem_pair);
		snprintf(pitem_pair+len, (MAX_SIPD_FILEITEM_LEN-len),"<Status>%s</Status>\r\n", "ON");
	}
	else if(SIPD_STATUS_OFF == pCataItem[start_index].Status)
	{
		len = strlen(pitem_pair);
		snprintf(pitem_pair+len, (MAX_SIPD_FILEITEM_LEN-len),"<Status>%s</Status>\r\n", "OFF");
	}

	if(strlen(pCataItem[start_index].Longitude) > 0)
	{
		len = strlen(pitem_pair);
		snprintf(pitem_pair+len, (MAX_SIPD_FILEITEM_LEN-len),"<Longitude>%s</Longitude>\r\n", pCataItem[start_index].Longitude);
	}

	if(strlen(pCataItem[start_index].Latitude) > 0)
	{
		len = strlen(pitem_pair);
		snprintf(pitem_pair+len, (MAX_SIPD_FILEITEM_LEN-len),"<Latitude>%s</Latitude>\r\n", pCataItem[start_index].Latitude);
	}
	
	len = strlen(pitem_pair);
	snprintf(pitem_pair+len, (MAX_SIPD_FILEITEM_LEN-len),"</Item>\r\n");

	return 0;
}
