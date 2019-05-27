
#include "stdafx.h"
#include "Gb28181_Layer.h"

#include <eXosip.h>
#include <osip_headers.h>
#include <osip_mt.h>

bool parse_deviceId_and_SN(mxml_node_t *tree, mxml_node_t *query_node, string & deviceId, string & SN);


//////////////////////////////////////////////////////////////////////////

Sipd_Alarm_Notify_func	pSipd_Alarm_Notify;

void Register_Sipd_Alarm_Notify(Sipd_Alarm_Notify_func pfunc)
{
	pSipd_Alarm_Notify = pfunc;
}


bool handle_alarm_notify(mxml_node_t *tree, mxml_node_t *notify_node, sipd_message_query_or_response  * pReq)
{

	sipd_alarm_notify		basic_info;
	mxml_node_t  *node,  * CmdType_node;

	CmdType_node = mxmlFindElement(notify_node, tree, "CmdType", NULL, NULL,MXML_DESCEND);
	if(CmdType_node != NULL && mxmlGetText(CmdType_node, NULL))
	{
		char strbuf[MAX_SIPD_URI];
		memset(strbuf, 0, sizeof(strbuf));
		strncpy_s(strbuf,  (const char *)mxmlGetText(CmdType_node, NULL), sizeof(strbuf));
		if(strcmp(strbuf, "Alarm") != 0)
			return false;
	}

	string strDeviceId;
	string strSN;
	if(parse_deviceId_and_SN(tree, notify_node,strDeviceId, strSN)==false)
		return false;

	stringstream ss;
	/*ss << "CCatalogQuery::parse_response m_current_item = " << m_current_item;
	LOG4CPLUS_DEBUG(envir().m_Logger, ss.str().c_str());*/

	memset(&basic_info, 0, sizeof(sipd_catalog_rsp_basic_info));
	strncpy_s(basic_info.to_uri, pReq->to_uri, MAX_SIPD_URI);
	strncpy_s(basic_info.from_uri, pReq->from_uri,  MAX_SIPD_URI);
	strncpy_s(basic_info.SN, strSN.c_str(), MAX_SIPD_DEVICE_ID_LEN);
	strncpy_s(basic_info.device_id, strDeviceId.c_str(), MAX_SIPD_DEVICE_ID_LEN);


	node = mxmlFindElement(notify_node, tree, "AlarmTime", NULL, NULL,MXML_DESCEND);
	if(node)
	{
		memset(basic_info.AlarmTime , 0 ,MAX_SIPD_TIME_LEN + 1);
		strncpy_s(basic_info.AlarmTime, (const char *)mxmlGetText(node, NULL), MAX_SIPD_TIME_LEN );
	}

	node = mxmlFindElement(notify_node, tree, "AlarmMethod", NULL, NULL,MXML_DESCEND);
	if(node)
	{
		char strResult[MAX_SIPD_NORMAL_LEN];
		memset(strResult, 0 ,MAX_SIPD_NORMAL_LEN);
		strncpy_s(strResult, (const char *)mxmlGetText(node, NULL),MAX_SIPD_NORMAL_LEN );
		basic_info.AlarmMethod = (enum sipd_alarm_condition)atoi(strResult);
	}

	node = mxmlFindElement(notify_node, tree, "AlarmPriority", NULL, NULL,MXML_DESCEND);
	if(node)
	{
		char strResult[MAX_SIPD_NORMAL_LEN];
		memset(strResult, 0 ,MAX_SIPD_NORMAL_LEN);
		strncpy_s(strResult, (const char *)mxmlGetText(node, NULL),MAX_SIPD_NORMAL_LEN );
		basic_info.AlarmPriority = (enum sipd_alarm_level)atoi(strResult);
	}


	if(pSipd_Alarm_Notify)
		pSipd_Alarm_Notify(&basic_info);

	return true;
}