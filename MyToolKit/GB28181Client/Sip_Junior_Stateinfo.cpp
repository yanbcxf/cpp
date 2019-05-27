
#include "stdafx.h"
#include "Gb28181_Layer.h"

#include <eXosip.h>
#include <osip_headers.h>
#include <osip_mt.h>

bool parse_deviceId_and_SN(mxml_node_t *tree, mxml_node_t *query_node, string & deviceId, string & SN);


//////////////////////////////////////////////////////////////////////////

Sipd_stateinfo_response_func	pSipd_stateinfo_resp;

void Register_Sipd_stateinfo_response(Sipd_stateinfo_response_func pfunc)
{
	pSipd_stateinfo_resp = pfunc;
}



/*设备（区域、子系统）状态查询*/
int Sip_stateinfo_request(const sipd_devicestate_req * pStateInfoReq)
{
	int ret = OSIP_UNDEFINED_ERROR;

	char buffer[MAX_SIPD_RECORD_INFO_LEN+1] = {0};
	memset(buffer, 0, sizeof(buffer));

	snprintf(buffer,MAX_SIPD_RECORD_INFO_LEN,
		"<?xml version=\"1.0\"?>\r\n"\
		"<Query>\r\n"\
		"<CmdType>DeviceStatus</CmdType>\r\n"\
		"<SN>%s</SN>\r\n"\
		"<DeviceID>%s</DeviceID>\r\n"\
		"</Query>",
		pStateInfoReq->SN, pStateInfoReq->device_id);

	if(/*Get_EvWaitFlg() ==*/ 1)
	{
		osip_message_t *message = NULL;

		eXosip_lock(context_eXosip);
		ret = eXosip_message_build_request(context_eXosip, &message, "MESSAGE",\
			pStateInfoReq->to_uri, pStateInfoReq->from_uri, NULL);
		if(ret != OSIP_SUCCESS)
		{
			eXosip_unlock(context_eXosip);
			LOG4CPLUS_DEBUG(gb_Logger, "Sip_stateinfo_request eXosip_message_build_request failed");
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
			LOG4CPLUS_DEBUG(gb_Logger, "Sip_stateinfo_request osip_message_set_body failed");
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
			LOG4CPLUS_DEBUG(gb_Logger, "Sip_stateinfo_request osip_message_set_content_type failed");
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
	ss << "Sip_stateinfo_request, SN = "<< string(pStateInfoReq->SN) << ", tid = " << ret;
	LOG4CPLUS_DEBUG(gb_Logger,ss.str());

	return ret;
}



bool handle_stateinfo_response(mxml_node_t *tree, mxml_node_t *response_node, sipd_message_query_or_response  * pReq)
{

	sipd_devicestate_rsp_basic_info		basic_info;
	sipd_alarm_status_item				cata_items[200];
	int									current_item = 0;

	mxml_node_t  *node, * item_node, * CmdType_node;

	CmdType_node = mxmlFindElement(response_node, tree, "CmdType", NULL, NULL,MXML_DESCEND);
	if(CmdType_node != NULL && mxmlGetText(CmdType_node, NULL))
	{
		char strbuf[MAX_SIPD_URI];
		memset(strbuf, 0, sizeof(strbuf));
		strncpy_s(strbuf,  (const char *)mxmlGetText(CmdType_node, NULL), sizeof(strbuf));
		if(strcmp(strbuf, "DeviceStatus") != 0)
			return false;
	}

	string strDeviceId;
	string strSN;
	if(parse_deviceId_and_SN(tree, response_node,strDeviceId, strSN)==false)
		return false;

	stringstream ss;
	ss << "handle_stateinfo_response, SN = "<< strSN ;
	LOG4CPLUS_DEBUG(gb_Logger,ss.str());

	memset(&basic_info, 0, sizeof(sipd_catalog_rsp_basic_info));
	strncpy_s(basic_info.to_uri, pReq->to_uri, MAX_SIPD_URI);
	strncpy_s(basic_info.from_uri, pReq->from_uri,  MAX_SIPD_URI);
	strncpy_s(basic_info.SN, strSN.c_str(), MAX_SIPD_DEVICE_ID_LEN);
	strncpy_s(basic_info.device_id, strDeviceId.c_str(), MAX_SIPD_DEVICE_ID_LEN);

	node = mxmlFindElement(response_node, tree, "Result", NULL, NULL,MXML_DESCEND);
	if(node && mxmlGetText(node, NULL))
	{
		char strResult[MAX_SIPD_NORMAL_LEN];
		memset(strResult, 0 ,MAX_SIPD_NORMAL_LEN);
		strncpy_s(strResult, (const char *)mxmlGetText(node, NULL),MAX_SIPD_NORMAL_LEN );
		if(strcmp(strResult, "OK")==0)
			basic_info.Result = SIPD_RESULT_OK;
		else
			basic_info.Result = SIPD_RESULT_ERROR;
	}

	node = mxmlFindElement(response_node, tree, "Online", NULL, NULL,MXML_DESCEND);
	if(node && mxmlGetText(node, NULL))
	{
		char strResult[MAX_SIPD_NORMAL_LEN];
		memset(strResult, 0 ,MAX_SIPD_NORMAL_LEN);
		strncpy_s(strResult, (const char *)mxmlGetText(node, NULL),MAX_SIPD_NORMAL_LEN );
		if(strcmp(strResult, "ONLINE")==0)
			basic_info.Online = SIPD_STATUS_ONLINE_TYPE_ONLINE;
		else
			basic_info.Online = SIPD_STATUS_ONLINE_TYPE_OFFLINE;
	}

	node = mxmlFindElement(response_node, tree, "Status", NULL, NULL,MXML_DESCEND);
	if(node  && mxmlGetText(node, NULL))
	{
		char strResult[MAX_SIPD_NORMAL_LEN];
		memset(strResult, 0 ,MAX_SIPD_NORMAL_LEN);
		strncpy_s(strResult, (const char *)mxmlGetText(node, NULL),MAX_SIPD_NORMAL_LEN );
		if(strcmp(strResult, "OK")==0)
			basic_info.Status = SIPD_RESULT_OK;
		else
			basic_info.Status = SIPD_RESULT_ERROR;
	}

	node = mxmlFindElement(response_node, tree, "Encode", NULL, NULL,MXML_DESCEND);
	if(node  && mxmlGetText(node, NULL))
	{
		char strResult[MAX_SIPD_NORMAL_LEN];
		memset(strResult, 0 ,MAX_SIPD_NORMAL_LEN);
		strncpy_s(strResult, (const char *)mxmlGetText(node, NULL),MAX_SIPD_NORMAL_LEN );
		if(strcmp(strResult, "ON")==0)
			basic_info.Encode = SIPD_STATUS_ON;
		else
			basic_info.Encode = SIPD_STATUS_OFF;
	}

	node = mxmlFindElement(response_node, tree, "Record", NULL, NULL,MXML_DESCEND);
	if(node  && mxmlGetText(node, NULL))
	{
		char strResult[MAX_SIPD_NORMAL_LEN];
		memset(strResult, 0 ,MAX_SIPD_NORMAL_LEN);
		strncpy_s(strResult, (const char *)mxmlGetText(node, NULL),MAX_SIPD_NORMAL_LEN );
		if(strcmp(strResult, "ON")==0)
			basic_info.Record = SIPD_STATUS_ON;
		else
			basic_info.Record = SIPD_STATUS_OFF;
	}

	node = mxmlFindElement(response_node, tree, "DeviceTime", NULL, NULL,MXML_DESCEND);
	if(node  && mxmlGetText(node, NULL))
	{
		memset(basic_info.DeviceTime , 0 ,MAX_SIPD_TIME_LEN + 1);
		strncpy_s(basic_info.DeviceTime, (const char *)mxmlGetText(node, NULL), MAX_SIPD_TIME_LEN );
	}

	node = mxmlFindElement(response_node, tree, "SumNum", NULL, NULL,MXML_DESCEND);
	if(node && mxmlGetText(node, NULL))
	{
		
	}

	node = mxmlFindElement(response_node, tree, "Alarmstatus", "Num", NULL, MXML_DESCEND);
	if(node && mxmlElementGetAttr(node, "Num"))
	{
		char strNum[MAX_SIPD_NORMAL_LEN];
		memset(strNum, 0 ,MAX_SIPD_NORMAL_LEN);
		strncpy_s(strNum, (const char *)mxmlElementGetAttr(node, "Num"),MAX_SIPD_NORMAL_LEN );
		basic_info.AlaramDeviceNum = atoi(strNum);

		//memcpy(m_basic_info.DeviceList_Num, (void *)mxmlGetText(node, NULL), sizeof(m_basic_info.SumNum));
		
		for(item_node = mxmlFindElement(node, tree, "Item",NULL, NULL,MXML_DESCEND);
			item_node != NULL;
			item_node = mxmlFindElement(item_node, tree, "Item", NULL, NULL, MXML_DESCEND))
		{
			memset(&cata_items[current_item], 0, sizeof(sipd_catalog_item));

			node = mxmlFindElement(item_node, tree, "DeviceID", NULL, NULL,MXML_DESCEND);
			if(node && mxmlGetText(node, NULL))
				strncpy_s(cata_items[current_item].device_id, (const char *)mxmlGetText(node, NULL), \
					sizeof(cata_items[current_item].device_id));

			node = mxmlFindElement(item_node, tree, "DutyStatus", NULL, NULL,MXML_DESCEND);
			if(node  && mxmlGetText(node, NULL))
			{
				char strResult[MAX_SIPD_NORMAL_LEN];
				memset(strResult, 0 ,MAX_SIPD_NORMAL_LEN);
				strncpy_s(strResult, (const char *)mxmlGetText(node, NULL),MAX_SIPD_NORMAL_LEN );
				if(strcmp(strResult, "ONDUTY")==0)
					cata_items[current_item].Status = SIPD_ALARM_STATUS_ONDUTY;
				else if(strcmp(strResult, "OFFDUTY")==0)
					cata_items[current_item].Status = SIPD_ALARM_STATUS_OFFDUTY;
				else
					cata_items[current_item].Status = SIPD_ALARM_STATUS_ALARM;
			}

			current_item++;
		}
	}
	else
		basic_info.AlaramDeviceNum = 0;

	if(current_item>0)
	{
		if(pSipd_stateinfo_resp)
			pSipd_stateinfo_resp(&basic_info, cata_items, pReq->tid);
	}
	else
	{
		if(pSipd_stateinfo_resp)
			pSipd_stateinfo_resp(&basic_info, NULL, pReq->tid);
	}

	return true;
}