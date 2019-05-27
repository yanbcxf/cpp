#pragma once
#include "xmlquery.h"

class CDeviceInfoQuery :
	public CXmlQuery
{
public:
	CDeviceInfoQuery(CGbUsageEnvironment& env);
	~CDeviceInfoQuery(void);

	bool parse_request(mxml_node_t *tree, mxml_node_t *query_node, sipd_message_query_or_response  * pReq);
	bool parse_response(mxml_node_t *tree, mxml_node_t *response_node,  sipd_message_query_or_response  * pReq);

	bool handle_request();
	bool handle_response();

	void send_request(sipd_deviceinfo_req  * pReq, CDeviceInfoQuery * pDeviceInfo);
	void send_response(sipd_deviceinfo_rsp* pDeviceInfo);
	void send_response_from(CDeviceInfoQuery * pCatalog);

public:
	sipd_deviceinfo_req		m_deviceinfo_req;
	sipd_deviceinfo_rsp		m_deviceinfo_rsp;

private:
	CDeviceInfoQuery *	m_next;

};
