
#include "stdafx.h"
#include "Gb28181_Layer.h"

#include <eXosip.h>
#include <osip_headers.h>
#include <osip_mt.h>

bool parse_deviceId_and_SN(mxml_node_t *tree, mxml_node_t *query_node, string & deviceId, string & SN);



//////////////////////////////////////////////////////////////////////////

Sipd_search_recordfile_response_func	pSipd_search_recordfile_resp;

void Register_Sipd_search_recordfile_response(Sipd_search_recordfile_response_func pfunc)
{
	pSipd_search_recordfile_resp = pfunc;
}


/*录像文件查询*/
int Sip_search_recordfile_request(sipd_recordinfo_req * pRecordFileReq)
{
	int ret = OSIP_UNDEFINED_ERROR;

	char buffer[MAX_SIPD_RECORD_INFO_LEN+1] = {0};
	memset(buffer, 0, sizeof(buffer));

	char recordType[32] = { 0 };
	switch(pRecordFileReq->Type)
	{
	case SIPD_RECORD_INFO_TYPE_ALL:
		sprintf_s(recordType, 32, "%s", "all");
		break;
	case SIPD_RECORD_INFO_TYPE_TIME:
		sprintf_s(recordType, 32, "%s", "time");
		break;		
	case SIPD_RECORD_INFO_TYPE_ALARM:
		sprintf_s(recordType, 32, "%s", "alarm");
		break;
	case SIPD_RECORD_INFO_TYPE_MANUAL:
		sprintf_s(recordType, 32, "%s", "manual");
		break;		
	}

	snprintf(buffer,MAX_SIPD_RECORD_INFO_LEN,
		"<?xml version=\"1.0\"?>\r\n"\
		"<Query>\r\n"\
		"<CmdType>RecordInfo</CmdType>\r\n"\
		"<SN>%s</SN>\r\n"\
		"<DeviceID>%s</DeviceID>\r\n"\
		"<StartTime>%s</StartTime>\r\n"\
		"<EndTime>%s</EndTime>\r\n"\
		"<FilePath>%s</FilePath>\r\n"\
		"<Address>%s</Address>\r\n"\
		"<Secrecy>%d</Secrecy>\r\n"\
		"<Type>%s</Type>\r\n"\
		"<RecorderID>%s</RecorderID>\r\n"\
		"</Query>",
		pRecordFileReq->SN, pRecordFileReq->device_id, 
		pRecordFileReq->StartTime, pRecordFileReq->EndTime, pRecordFileReq->FilePath, pRecordFileReq->Address,
		pRecordFileReq->Secrecy, recordType, pRecordFileReq->RecorderID);

	if(/*Get_EvWaitFlg() ==*/ 1)
	{
		osip_message_t *message = NULL;

		eXosip_lock(context_eXosip);
		ret = eXosip_message_build_request(context_eXosip, &message, "MESSAGE",\
			pRecordFileReq->to_uri, pRecordFileReq->from_uri, NULL);
		if(ret != OSIP_SUCCESS)
		{
			eXosip_unlock(context_eXosip);
			LOG4CPLUS_DEBUG(gb_Logger, "Sip_search_recordfile_request eXosip_message_build_request failed");
			if(message != NULL)
			{
				osip_message_free(message);
			}
			//context_eXosip_unlock();
			return -1;
		}

		ret = osip_message_set_body(message, buffer, strlen(buffer));
		if(ret != OSIP_SUCCESS)
		{
			eXosip_unlock(context_eXosip);
			LOG4CPLUS_DEBUG(gb_Logger, "Sip_search_recordfile_request osip_message_set_body failed");
			if(message != NULL)
			{
				osip_message_free(message);
			}
			//context_eXosip_unlock();
			return -1;
		}

		ret = osip_message_set_content_type (message, "Application/MANSCDP+xml");
		if(ret != OSIP_SUCCESS)
		{
			eXosip_unlock(context_eXosip);
			LOG4CPLUS_DEBUG(gb_Logger, "Sip_search_recordfile_request osip_message_set_content_type failed");
			if(message != NULL)
			{
				osip_message_free(message);
			}
			//context_eXosip_unlock();
			return -1;
		}

		ret = eXosip_message_send_request (context_eXosip, message);
		eXosip_unlock(context_eXosip);
	}

	/*返回 tid >0*/
	stringstream ss;
	ss << "Sip_search_recordfile_request, SN = "<< string(pRecordFileReq->SN) << ", tid = " << ret;
	LOG4CPLUS_DEBUG(gb_Logger,ss.str());

	return ret;
}


bool handle_search_recordfile_response(mxml_node_t *tree, mxml_node_t *response_node, sipd_message_query_or_response  * pReq)
{

	sipd_recordinfo_rsp_basic_info		basic_info;
	sipd_file_item					cata_items[100];
	int								current_item = 0;

	mxml_node_t  *node, * item_node, * CmdType_node;

	CmdType_node = mxmlFindElement(response_node, tree, "CmdType", NULL, NULL,MXML_DESCEND);
	if(CmdType_node != NULL && mxmlGetText(CmdType_node, NULL))
	{
		char strbuf[MAX_SIPD_URI];
		memset(strbuf, 0, sizeof(strbuf));
		strncpy_s(strbuf,  (const char *)mxmlGetText(CmdType_node, NULL), sizeof(strbuf));
		if(strcmp(strbuf, "RecordInfo") != 0)
			return false;
	}

	string strDeviceId;
	string strSN;
	if(parse_deviceId_and_SN(tree, response_node,strDeviceId, strSN)==false)
		return false;

	stringstream ss;
	/*ss << "CCatalogQuery::parse_response m_current_item = " << m_current_item;
	LOG4CPLUS_DEBUG(envir().m_Logger, ss.str().c_str());*/

	memset(&basic_info, 0, sizeof(sipd_recordinfo_rsp_basic_info));
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

	node = mxmlFindElement(response_node, tree, "Name", NULL, NULL,MXML_DESCEND);
	if(node && mxmlGetText(node, NULL))
		strncpy_s(basic_info.Name, Utf8_GBK((const char  *)mxmlGetText(node, NULL)).c_str(), \
			sizeof(basic_info.Name));

	node = mxmlFindElement(response_node, tree, "RecordList", "Num", NULL, MXML_DESCEND);
	if(node && mxmlElementGetAttr(node, "Num"))
	{
		char strNum[MAX_SIPD_NORMAL_LEN];
		memset(strNum, 0 ,MAX_SIPD_NORMAL_LEN);
		strncpy_s(strNum, (const char *)mxmlElementGetAttr(node, "Num"),MAX_SIPD_NORMAL_LEN );
		basic_info.RecordListNum = atoi(strNum);

		//memcpy(m_basic_info.DeviceList_Num, (void *)mxmlGetText(node, NULL), sizeof(m_basic_info.SumNum));
		
		for(item_node = mxmlFindElement(node, tree, "Item",NULL, NULL,MXML_DESCEND);
			item_node != NULL;
			item_node = mxmlFindElement(item_node, tree, "Item", NULL, NULL, MXML_DESCEND))
		{
			memset(&cata_items[current_item], 0, sizeof(sipd_catalog_item));

			node = mxmlFindElement(item_node, tree, "DeviceID", NULL, NULL,MXML_DESCEND);
			if(node  && mxmlGetText(node, NULL))
				strncpy_s(cata_items[current_item].device_id, (const char *)mxmlGetText(node, NULL), \
					sizeof(cata_items[current_item].device_id));

			node = mxmlFindElement(item_node, tree, "Name", NULL, NULL,MXML_DESCEND);
			if(node && mxmlGetText(node, NULL))
				strncpy_s(cata_items[current_item].Name, Utf8_GBK((const char  *)mxmlGetText(node, NULL)).c_str(), \
				sizeof(cata_items[current_item].Name));

			node = mxmlFindElement(item_node, tree, "FilePath", NULL, NULL,MXML_DESCEND);
			if(node && mxmlGetText(node, NULL))
				strncpy_s(cata_items[current_item].FilePath, (const char  *)mxmlGetText(node, NULL), \
				sizeof(cata_items[current_item].FilePath));

			node = mxmlFindElement(item_node, tree, "Address", NULL, NULL,MXML_DESCEND);
			if(node && mxmlGetText(node, NULL))
				strncpy_s(cata_items[current_item].Address, (const char  *)mxmlGetText(node, NULL), \
				sizeof(cata_items[current_item].Address));

			
			node = mxmlFindElement(item_node, tree, "StartTime", NULL, NULL,MXML_DESCEND);
			if(node && mxmlGetText(node, NULL))
				strncpy_s(cata_items[current_item].StartTime, (const char  *)mxmlGetText(node, NULL), \
				sizeof(cata_items[current_item].StartTime));

			node = mxmlFindElement(item_node, tree, "EndTime", NULL, NULL,MXML_DESCEND);
			if(node && mxmlGetText(node, NULL))
				strncpy_s(cata_items[current_item].EndTime, (const char  *)mxmlGetText(node, NULL), \
				sizeof(cata_items[current_item].EndTime));

			node = mxmlFindElement(item_node, tree, "Secrecy", NULL, NULL,MXML_DESCEND);
			if(node && mxmlGetText(node, NULL))
			{
				char strSecrecy[MAX_SIPD_NORMAL_LEN];
				memset(strSecrecy, 0 ,MAX_SIPD_NORMAL_LEN);
				strncpy_s(strSecrecy, (const char *)mxmlGetText(node, NULL),MAX_SIPD_NORMAL_LEN );
				cata_items[current_item].Secrecy = atoi(strSecrecy);
			}

			node = mxmlFindElement(item_node, tree, "Type", NULL, NULL,MXML_DESCEND);
			if(node && mxmlGetText(node, NULL))
			{
				char strResult[MAX_SIPD_NORMAL_LEN];
				memset(strResult, 0 ,MAX_SIPD_NORMAL_LEN);
				strncpy_s(strResult, (const char *)mxmlGetText(node, NULL),MAX_SIPD_NORMAL_LEN );
				if(strcmp(strResult, "time")==0)
					cata_items[current_item].Type = SIPD_RECORD_INFO_TYPE_TIME;
				else if(strcmp(strResult, "alarm")==0)
					cata_items[current_item].Type = SIPD_RECORD_INFO_TYPE_ALARM;
				else if(strcmp(strResult, "manual")==0)
					cata_items[current_item].Type = SIPD_RECORD_INFO_TYPE_MANUAL;
				else
					cata_items[current_item].Type = SIPD_RECORD_INFO_TYPE_ALL;
			}

			node = mxmlFindElement(item_node, tree, "RecorderID", NULL, NULL,MXML_DESCEND);
			if(node && mxmlGetText(node, NULL))
				strncpy_s(cata_items[current_item].RecorderID, (const char  *)mxmlGetText(node, NULL), \
				sizeof(cata_items[current_item].RecorderID));

			current_item++;
		}
	}

	if(current_item>0)
	{
		if(pSipd_search_recordfile_resp)
			pSipd_search_recordfile_resp(&basic_info, cata_items, pReq->tid);
	}

	return true;
}