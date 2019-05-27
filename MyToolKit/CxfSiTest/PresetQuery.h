#pragma once
#include "xmlquery.h"

class CPresetQuery :
	public CXmlQuery
{
public:
	CPresetQuery(CGbUsageEnvironment& env);
	~CPresetQuery(void);

	bool parse_request(mxml_node_t *tree, mxml_node_t *query_node, sipd_message_query_or_response  * pReq);
	bool parse_response(mxml_node_t *tree, mxml_node_t *response_node,  sipd_message_query_or_response  * pReq);

	bool handle_request();
	bool handle_response();

	void send_request(sipd_perset_query_req  * pReq, CPresetQuery * pDeviceInfo);
	void send_response(sipd_deviceinfo_rsp* pDeviceInfo);
	void send_response_from(CPresetQuery * pCatalog);

public:
	sipd_perset_query_req	m_perset_query_req;

private:
	CPresetQuery *	m_next;
};
