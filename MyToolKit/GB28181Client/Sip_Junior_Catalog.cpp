
#include "stdafx.h"
#include "Gb28181_Layer.h"

#include <eXosip.h>
#include <osip_headers.h>
#include <osip_mt.h>

bool parse_deviceId_and_SN(mxml_node_t *tree, mxml_node_t *query_node, string & deviceId, string & SN);



//////////////////////////////////////////////////////////////////////////

Sipd_cataloginfo_response_func	pSipd_catalog_resp;

void Register_Sipd_cataloginfo_response(Sipd_cataloginfo_response_func pfunc)
{
	pSipd_catalog_resp = pfunc;
}


/*设备（区域、子系统）目录查询*/
int Sip_cataloginfo_request(const sipd_catalog_req * pCatalogReq)
{
	int ret = OSIP_UNDEFINED_ERROR;

	char buffer[MAX_SIPD_RECORD_INFO_LEN+1] = {0};
	memset(buffer, 0, sizeof(buffer));

	snprintf(buffer,MAX_SIPD_RECORD_INFO_LEN,
		"<?xml version=\"1.0\"?>\r\n"\
		"<Query>\r\n"\
		"<CmdType>Catalog</CmdType>\r\n"\
		"<SN>%s</SN>\r\n"\
		"<DeviceID>%s</DeviceID>\r\n"\
		"</Query>",
		pCatalogReq->SN, pCatalogReq->device_id);

	if(/*Get_EvWaitFlg() ==*/ 1)
	{
		osip_message_t *message = NULL;

		eXosip_lock(context_eXosip);
		ret = eXosip_message_build_request(context_eXosip, &message, "MESSAGE",\
			pCatalogReq->to_uri, pCatalogReq->from_uri, NULL);
		if(ret != OSIP_SUCCESS)
		{
			eXosip_unlock(context_eXosip);

			char tmp[128] = { 0 };
			sprintf_s(tmp, 128 , "Sip_cataloginfo_request eXosip_message_build_request failed, ret = %d", ret);
			LOG4CPLUS_DEBUG(gb_Logger, string(tmp));

			stringstream ss;
			ss << "发送的信息 ：" << string(buffer);
			LOG4CPLUS_DEBUG(gb_Logger, ss.str());

			ss.str("");
			ss << "pCatalogReq->to_uri ：" << string(pCatalogReq->to_uri);
			ss << ", pCatalogReq->from_uri : " << string(pCatalogReq->from_uri);
			LOG4CPLUS_DEBUG(gb_Logger, ss.str());

			if(message != NULL)
			{
				osip_message_free(message);
			}
			//context_eXosip_unlock();
			return ret;
		}

		ret = osip_message_set_body(message, buffer, strlen(buffer));
		if(ret != OSIP_SUCCESS)
		{
			eXosip_unlock(context_eXosip);
			LOG4CPLUS_DEBUG(gb_Logger, "Sip_cataloginfo_request osip_message_set_body failed");
			if(message != NULL)
			{
				osip_message_free(message);
			}
			//context_eXosip_unlock();
			return ret;
		}

		ret = osip_message_set_content_type (message, "Application/MANSCDP+xml");
		if(ret != OSIP_SUCCESS)
		{
			eXosip_unlock(context_eXosip);
			LOG4CPLUS_DEBUG(gb_Logger, "Sip_cataloginfo_request osip_message_set_content_type failed");
			if(message != NULL)
			{
				osip_message_free(message);
			}
			//context_eXosip_unlock();
			return ret;
		}

		ret = eXosip_message_send_request (context_eXosip, message);
		eXosip_unlock(context_eXosip);
	}

	/*返回 tid >0*/

	stringstream ss;
	ss << "Sip_cataloginfo_request, SN = "<< string(pCatalogReq->SN) << ", tid = " << ret;
	LOG4CPLUS_DEBUG(gb_Logger,ss.str());

	return ret;
}


bool handle_catalog_response(mxml_node_t *tree, mxml_node_t *response_node, sipd_message_query_or_response  * pReq)
{

	sipd_catalog_rsp_basic_info		basic_info;
	sipd_catalog_item				cata_items[100];
	int								current_item = 0;

	mxml_node_t  *node, * item_node, * CmdType_node;

	CmdType_node = mxmlFindElement(response_node, tree, "CmdType", NULL, NULL,MXML_DESCEND);
	if(CmdType_node != NULL)
	{
		char strbuf[MAX_SIPD_URI];
		memset(strbuf, 0, sizeof(strbuf));
		sprintf_s(strbuf, MAX_SIPD_URI, "%s" , mxmlGetText(CmdType_node, NULL));
		//memcpy(strbuf,  (const char *)mxmlGetText(CmdType_node, NULL), sizeof(strbuf));
		if(strcmp(strbuf, "Catalog") != 0)
			return false;
	}

	string strDeviceId;
	string strSN;
	if(parse_deviceId_and_SN(tree, response_node,strDeviceId, strSN)==false)
		return false;

	stringstream ss;
	/*ss << "CCatalogQuery::parse_response m_current_item = " << m_current_item;
	LOG4CPLUS_DEBUG(envir().m_Logger, ss.str().c_str());*/

	memset(&basic_info, 0, sizeof(sipd_catalog_rsp_basic_info));
	strncpy_s(basic_info.to_uri, pReq->to_uri, MAX_SIPD_URI);
	strncpy_s(basic_info.from_uri, pReq->from_uri,  MAX_SIPD_URI);
	strncpy_s(basic_info.SN, strSN.c_str(), MAX_SIPD_DEVICE_ID_LEN);
	strncpy_s(basic_info.device_id, strDeviceId.c_str(), MAX_SIPD_DEVICE_ID_LEN);

	node = mxmlFindElement(response_node, tree, "SumNum", NULL, NULL,MXML_DESCEND);
	if(node)
	{
		char strSumNum[MAX_SIPD_NORMAL_LEN];
		memset(strSumNum, 0 ,MAX_SIPD_NORMAL_LEN);
		strncpy_s(strSumNum, (const char *)mxmlGetText(node, NULL),MAX_SIPD_NORMAL_LEN );
		basic_info.SumNum = atoi(strSumNum);
	}

	node = mxmlFindElement(response_node, tree, "DeviceList", "Num", NULL, MXML_DESCEND);
	if(node && mxmlElementGetAttr(node, "Num"))
	{
		char strDeviceListNum[MAX_SIPD_NORMAL_LEN];
		memset(strDeviceListNum, 0 ,MAX_SIPD_NORMAL_LEN);
		strncpy_s(strDeviceListNum, (const char *)mxmlElementGetAttr(node, "Num"), MAX_SIPD_NORMAL_LEN );
		basic_info.DeviceList_Num = atoi(strDeviceListNum);
		//memcpy(m_basic_info.DeviceList_Num, (void *)mxmlGetText(node, NULL), sizeof(m_basic_info.SumNum));

		if(basic_info.DeviceList_Num >100)
		{
			LOG4CPLUS_DEBUG(gb_Logger, "handle_catalog_response, DeviceList_Num is too big" );
		}
		
		for(item_node = mxmlFindElement(node, tree, "Item",NULL, NULL,MXML_DESCEND);
			item_node != NULL;
			item_node = mxmlFindElement(item_node, tree, "Item", NULL, NULL, MXML_DESCEND))
		{
			memset(&cata_items[current_item], 0, sizeof(sipd_catalog_item));

			node = mxmlFindElement(item_node, tree, "DeviceID", NULL, NULL,MXML_DESCEND);
			if(node && mxmlGetText(node, NULL))
				strncpy_s(cata_items[current_item].device_id, (const char *)mxmlGetText(node, NULL), \
					sizeof(cata_items[current_item].device_id));

			node = mxmlFindElement(item_node, tree, "Name", NULL, NULL,MXML_DESCEND);
			if(node && mxmlGetText(node, NULL))
			{
				string strName = "";
				node = mxmlGetFirstChild(node);
				while(node)
				{
					if(mxmlGetText(node, NULL))
					{
						if(strName.empty()==false)
							strName += " ";
						strName += string(mxmlGetText(node, NULL));
					}
					node = mxmlGetNextSibling(node);
				}
				/*int whitespace;
				const char * p = (const char *)mxmlGetText(node, &whitespace);
				node = mxmlGetNextSibling(node);*/

				strncpy_s(cata_items[current_item].Name, Utf8_GBK(strName).c_str(), \
				sizeof(cata_items[current_item].Name));
			}

			node = mxmlFindElement(item_node, tree, "Manufacturer", NULL, NULL,MXML_DESCEND);
			if(node && mxmlGetText(node, NULL))
				strncpy_s(cata_items[current_item].Manufacturer, Utf8_GBK((const char  *)mxmlGetText(node, NULL)).c_str(), \
				sizeof(cata_items[current_item].Manufacturer));

			node = mxmlFindElement(item_node, tree, "Model", NULL, NULL,MXML_DESCEND);
			if(node && mxmlGetText(node, NULL))
				strncpy_s(cata_items[current_item].Model, (const char  *)mxmlGetText(node, NULL), \
				sizeof(cata_items[current_item].Model));

			
			node = mxmlFindElement(item_node, tree, "Owner", NULL, NULL,MXML_DESCEND);
			if(node && mxmlGetText(node, NULL))
				strncpy_s(cata_items[current_item].Owner, Utf8_GBK((const char  *)mxmlGetText(node, NULL)).c_str(), \
				sizeof(cata_items[current_item].Owner));

			node = mxmlFindElement(item_node, tree, "CivilCode", NULL, NULL,MXML_DESCEND);
			if(node && mxmlGetText(node, NULL))
				strncpy_s(cata_items[current_item].CivilCode, (const char  *)mxmlGetText(node, NULL), \
				sizeof(cata_items[current_item].CivilCode));

			node = mxmlFindElement(item_node, tree, "Block", NULL, NULL,MXML_DESCEND);
			if(node && mxmlGetText(node, NULL))
				strncpy_s(cata_items[current_item].Block, (const char  *)mxmlGetText(node, NULL), \
				sizeof(cata_items[current_item].Block));

			node = mxmlFindElement(item_node, tree, "Address", NULL, NULL,MXML_DESCEND);
			if(node && mxmlGetText(node, NULL))
				strncpy_s(cata_items[current_item].Address, (const char  *)mxmlGetText(node, NULL), \
				sizeof(cata_items[current_item].Address));

			node = mxmlFindElement(item_node, tree, "Parental", NULL, NULL,MXML_DESCEND);
			if(node && mxmlGetText(node, NULL))
			{
				char strDeviceListNum[MAX_SIPD_NORMAL_LEN];
				memset(strDeviceListNum, 0 ,MAX_SIPD_NORMAL_LEN);
				strncpy_s(strDeviceListNum, (const char *)mxmlGetText(node, NULL), MAX_SIPD_NORMAL_LEN );
				cata_items[current_item].Parental = atoi(strDeviceListNum);
			}

			node = mxmlFindElement(item_node, tree, "ParentID", NULL, NULL,MXML_DESCEND);
			if(node && mxmlGetText(node, NULL))
				strncpy_s(cata_items[current_item].ParentID, (const char  *)mxmlGetText(node, NULL), \
				sizeof(cata_items[current_item].ParentID));

			node = mxmlFindElement(item_node, tree, "IPAddress", NULL, NULL,MXML_DESCEND);
			if(node && mxmlGetText(node, NULL))
				strncpy_s(cata_items[current_item].IPAddress, (const char  *)mxmlGetText(node, NULL), \
				sizeof(cata_items[current_item].IPAddress));

			node = mxmlFindElement(item_node, tree, "Status", NULL, NULL,MXML_DESCEND);
			if(node && mxmlGetText(node, NULL))
			{
				char strDeviceListNum[MAX_SIPD_NORMAL_LEN];
				memset(strDeviceListNum, 0 ,MAX_SIPD_NORMAL_LEN);
				strncpy_s(strDeviceListNum, (const char *)mxmlGetText(node, NULL), MAX_SIPD_NORMAL_LEN );
				if(strcmp(strDeviceListNum, "OFF") ==0)
					cata_items[current_item].Status = SIPD_STATUS_OFF;
				else
					cata_items[current_item].Status = SIPD_STATUS_ON;
			}


			current_item++;
		}
	}
	else
		basic_info.DeviceList_Num = 0;

	if(current_item>0)
	{
		if(pSipd_catalog_resp)
			pSipd_catalog_resp(&basic_info, cata_items, pReq->tid);
	}
	else
	{
		if(pSipd_catalog_resp)
			pSipd_catalog_resp(&basic_info, NULL, pReq->tid);
	}

	return true;
}