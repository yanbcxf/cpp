#pragma once
#include "xmlquery.h"

class CRecordControl :
	public CXmlQuery
{
public:
	CRecordControl(CGbUsageEnvironment& env);
	~CRecordControl(void);

	bool parse_request(mxml_node_t *tree, mxml_node_t *query_node, sipd_message_query_or_response  * pReq);
	bool parse_response(mxml_node_t *tree, mxml_node_t *response_node,  sipd_message_query_or_response  * pReq);

	bool handle_request();
	bool handle_response();

	void send_request(sipd_record_control  * pReq, CRecordControl * pDeviceInfo);
	void send_response(sipd_control_rsp* pDeviceInfo);
	void send_response_from(CRecordControl * pCatalog);

public:
	sipd_record_control	m_record_control_req;
	sipd_control_rsp	m_record_control_rsp;

private:
	CRecordControl *	m_next;
};
