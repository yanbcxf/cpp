
#include "stdafx.h"
#include "Gb28181_Layer.h"

#include <eXosip.h>
#include <osip_headers.h>
#include <osip_mt.h>

bool parse_deviceId_and_SN(mxml_node_t *tree, mxml_node_t *query_node, string & deviceId, string & SN);


//////////////////////////////////////////////////////////////////////////

Sipd_deviceinfo_response_func	pSipd_deviceinfo_resp;

void Register_Sipd_deviceinfo_response(Sipd_deviceinfo_response_func pfunc)
{
	pSipd_deviceinfo_resp = pfunc;
}



/*设备（区域、子系统）信息查询*/
int Sip_deviceinfo_request(const sipd_deviceinfo_req * pDeviceinfoReq)
{
	int ret = OSIP_UNDEFINED_ERROR;

	char buffer[MAX_SIPD_RECORD_INFO_LEN+1] = {0};
	memset(buffer, 0, sizeof(buffer));

	snprintf(buffer,MAX_SIPD_RECORD_INFO_LEN,
		"<?xml version=\"1.0\"?>\r\n"\
		"<Query>\r\n"\
		"<CmdType>DeviceInfo</CmdType>\r\n"\
		"<SN>%s</SN>\r\n"\
		"<DeviceID>%s</DeviceID>\r\n"\
		"</Query>",
		pDeviceinfoReq->SN, pDeviceinfoReq->device_id);

	if(/*Get_EvWaitFlg() ==*/ 1)
	{
		osip_message_t *message = NULL;

		eXosip_lock(context_eXosip);
		ret = eXosip_message_build_request(context_eXosip, &message, "MESSAGE",\
			pDeviceinfoReq->to_uri, pDeviceinfoReq->from_uri, NULL);
		if(ret != OSIP_SUCCESS)
		{
			eXosip_unlock(context_eXosip);
			printf("Sip_deviceinfo_request eXosip_message_build_request failed\n");
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
			printf("Sip_deviceinfo_request osip_message_set_body failed\n");
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
			printf("Sip_deviceinfo_request osip_message_set_content_type failed\n");
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
	ss << "Sip_deviceinfo_request, SN = "<< string(pDeviceinfoReq->SN) << ", tid = " << ret;
	LOG4CPLUS_DEBUG(gb_Logger,ss.str());

	return ret;
}




bool handle_deviceinfo_response(mxml_node_t *tree, mxml_node_t *response_node, sipd_message_query_or_response  * pReq)
{

	sipd_deviceinfo_rsp		basic_info;
	

	mxml_node_t  *node, * CmdType_node;

	CmdType_node = mxmlFindElement(response_node, tree, "CmdType", NULL, NULL,MXML_DESCEND);
	if(CmdType_node != NULL && mxmlGetText(CmdType_node, NULL))
	{
		char strbuf[MAX_SIPD_URI];
		memset(strbuf, 0, sizeof(strbuf));
		strncpy_s(strbuf,  (const char *)mxmlGetText(CmdType_node, NULL), MAX_SIPD_URI);
		if(strcmp(strbuf, "DeviceInfo") != 0)
			return false;
	}

	string strDeviceId;
	string strSN;
	if(parse_deviceId_and_SN(tree, response_node,strDeviceId, strSN)==false)
		return false;

	stringstream ss;
	ss << "handle_deviceinfo_response, SN = "<< strSN ;
	LOG4CPLUS_DEBUG(gb_Logger,ss.str());

	memset(&basic_info, 0, sizeof(sipd_deviceinfo_rsp));
	strncpy_s(basic_info.to_uri, pReq->to_uri, MAX_SIPD_URI);
	strncpy_s(basic_info.from_uri, pReq->from_uri,  MAX_SIPD_URI);
	strncpy_s(basic_info.SN, strSN.c_str(), MAX_SIPD_DEVICE_ID_LEN);
	strncpy_s(basic_info.device_id, strDeviceId.c_str(), MAX_SIPD_DEVICE_ID_LEN);

	node = mxmlFindElement(response_node, tree, "Result", NULL, NULL,MXML_DESCEND);
	if(node)
	{
		char strResult[MAX_SIPD_NORMAL_LEN];
		memset(strResult, 0 ,MAX_SIPD_NORMAL_LEN);
		strncpy_s(strResult, (const char *)mxmlGetText(node, NULL),MAX_SIPD_NORMAL_LEN );
		if(strcmp(strResult, "OK")==0)
			basic_info.Result = SIPD_RESULT_OK;
		else
			basic_info.Result = SIPD_RESULT_ERROR;
	}

	node = mxmlFindElement(response_node, tree, "DeviceType", NULL, NULL,MXML_DESCEND);
	if(node)
	{
		memset(basic_info.DeviceType, 0 ,MAX_SIPD_NORMAL_LEN);
		strncpy_s(basic_info.DeviceType, (const char *)mxmlGetText(node, NULL),MAX_SIPD_NORMAL_LEN );
	}

	node = mxmlFindElement(response_node, tree, "Manufacturer", NULL, NULL,MXML_DESCEND);
	if(node)
	{
		memset(basic_info.manufacturer, 0 ,MAX_SIPD_MANUFACTURER_NAME_LEN);
		strncpy_s(basic_info.manufacturer, (const char *)mxmlGetText(node, NULL),MAX_SIPD_MANUFACTURER_NAME_LEN );
	}

	node = mxmlFindElement(response_node, tree, "Model", NULL, NULL,MXML_DESCEND);
	if(node)
	{
		memset(basic_info.model, 0 ,MAX_SIPD_MODEL_NAME_LEN);
		strncpy_s(basic_info.model, (const char *)mxmlGetText(node, NULL),MAX_SIPD_MODEL_NAME_LEN );
	}

	node = mxmlFindElement(response_node, tree, "Firmware", NULL, NULL,MXML_DESCEND);
	if(node)
	{
		memset(basic_info.firmware, 0 ,MAX_SIPD_FIREWARE_LEN);
		strncpy_s(basic_info.firmware, (const char *)mxmlGetText(node, NULL),MAX_SIPD_FIREWARE_LEN );
	}

	node = mxmlFindElement(response_node, tree, "MaxCamera", NULL, NULL,MXML_DESCEND);
	if(node)
	{
		char strResult[MAX_SIPD_NORMAL_LEN];
		memset(strResult, 0 ,MAX_SIPD_NORMAL_LEN);
		strncpy_s(strResult, (const char *)mxmlGetText(node, NULL),MAX_SIPD_NORMAL_LEN );
		basic_info.MaxCamera = atoi(strResult);
	}

	node = mxmlFindElement(response_node, tree, "MaxAlarm", NULL, NULL,MXML_DESCEND);
	if(node)
	{
		char strResult[MAX_SIPD_NORMAL_LEN];
		memset(strResult, 0 ,MAX_SIPD_NORMAL_LEN);
		strncpy_s(strResult, (const char *)mxmlGetText(node, NULL),MAX_SIPD_NORMAL_LEN );
		basic_info.MaxAlarm = atoi(strResult);
	}

	if(pSipd_deviceinfo_resp)
		pSipd_deviceinfo_resp(&basic_info, pReq->tid);

	return true;
}