#pragma once
#include "xmlquery.h"

class CDeviceStatusQuery :
	public CXmlQuery
{
public:
	CDeviceStatusQuery(CGbUsageEnvironment& env);
	~CDeviceStatusQuery(void);

	bool parse_request(mxml_node_t *tree, mxml_node_t *query_node, sipd_message_query_or_response  * pReq);
	bool parse_response(mxml_node_t *tree, mxml_node_t *response_node,  sipd_message_query_or_response  * pReq);

	bool handle_request();
	bool handle_response();

	void send_request(sipd_devicestate_req  * pReq, CDeviceStatusQuery * pDeviceInfo);
	void send_response(sipd_stateinfo_adaptor_rsp* pDeviceInfo);
	void send_response_from(CDeviceStatusQuery * pCatalog);

public:
	sipd_devicestate_req		m_state_req;	
	sipd_stateinfo_adaptor_rsp	m_state_rsp;

private:
	CDeviceStatusQuery *	m_next;
};
