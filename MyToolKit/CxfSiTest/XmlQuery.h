#pragma once
#include "medium.h"


#define CONTENT_TYPE_APP				("Application")
#define CONTENT_TYPE_SUB_MANSCMD		("MANSCDP+xml")
#define CONTENT_TYPE_SUB_MANSRTSP	("MANSRTSP")
#define CONTENT_TYPE_SUB_RTSP	("RTSP")
#define MAX_XML_BUFFER_LEN			(1024)
#define XML_NAME_CmdType			("CmdType")
#define XML_VALUE_CmdType_DeviceControl			("DeviceControl")
#define XML_VALUE_CmdType_Alarm					("Alarm")
#define XML_NAME_Result			("Result")
#define XML_NAME_SN			("SN")
#define XML_NAME_DeviceID			("DeviceID")
#define XML_NAME_PTZCmd			("PTZCmd")
#define XML_NAME_TeleBoot			("TeleBoot")
#define XML_VALUE_TeleBoot_Boot			("Boot")
#define XML_NAME_RecordCmd			("RecordCmd")
#define XML_VALUE_RecordCmd_Record			("Record")
#define XML_VALUE_RecordCmd_StopRecord			("StopRecord")
#define XML_NAME_GuardCmd			("GuardCmd")
#define XML_VALUE_GuardCmd_SetGuard			("SetGuard")
#define XML_VALUE_GuardCmd_ResetGuard			("ResetGuard")
#define XML_NAME_AlarmCmd			("AlarmCmd")
#define XML_VALUE_AlarmCmd_ResetAlarm			("ResetAlarm")
#define XML_NAME_ControlPriority			("ControlPriority")
#define XML_NAME_StartTime				("StartTime")
#define XML_NAME_EndTime				("EndTime")
#define XML_NAME_FilePath				("FilePath")
#define XML_NAME_Address				("Address")
#define XML_NAME_Secrecy				("Secrecy")
#define XML_NAME_Type				("Type")
#define XML_Value_Type_time				("time")
#define XML_Value_Type_alarm			("alarm")
#define XML_Value_Type_manual			("manual")
#define XML_Value_Type_all			("all")
#define XML_NAME_RecorderID				("RecorderID")
#define XML_NAME_StartAlarmPriority			("StartAlarmPriority")
#define XML_NAME_EndAlarmPriority			("EndAlarmPriority")
#define XML_NAME_AlarmMethod			("AlarmMethod")

#define MANSCDP_XML 	("Application/MANSCDP+xml")
#define QUERY 			("Query")
#define CMDTYPE 			("CmdType")
#define DEVICEINFO 		("DeviceInfo")
#define DEVICESTATUS 		("DeviceStatus")
#define DEVICECATALOG		("Catalog")
#define SIPD_RECORDINFO		("RecordInfo")
#define PERSETQUERY		("PersetQuery")
#define RESPONSE 			("Response")

class CXmlQuery : public Medium
{
public:
	CXmlQuery(CGbUsageEnvironment& env);
	~CXmlQuery(void);

	static CXmlQuery * manscdp_xml_deal(CGbUsageEnvironment& env, sipd_message_query_or_response  * pReq);
	static CXmlQuery * manscdp_xml_deal1(CGbUsageEnvironment& env, sipd_message_query_or_response  * pReq);

	virtual bool parse_request(mxml_node_t *tree, mxml_node_t *query_node, sipd_message_query_or_response  * pReq) = 0;
	virtual bool parse_response(mxml_node_t *tree, mxml_node_t *response_node, sipd_message_query_or_response  * pReq);
	virtual bool parse_deviceId_and_SN(mxml_node_t *tree, mxml_node_t *query_node, string & deviceId, string & SN);
	virtual bool handle_request(){ return false; };
	virtual bool handle_response(){ return false; };

	CGbUsageEnvironment& envir() const { return (CGbUsageEnvironment&)fEnviron; }


public:
	/*string m_to_username;
	string m_to_host;
	string m_device_id;
	string m_SN;*/
};
