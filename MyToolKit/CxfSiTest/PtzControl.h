#pragma once
#include "xmlquery.h"

class CPtzControl :
	public CXmlQuery
{
public:
	CPtzControl(CGbUsageEnvironment& env);
	~CPtzControl(void);

	bool parse_request(mxml_node_t *tree, mxml_node_t *query_node, sipd_message_query_or_response  * pReq);
	
	bool handle_request();
	bool handle_response();

	void send_request(struct sipd_device_control  * pReq, CPtzControl * pDeviceInfo);


public:
	struct sipd_device_control	m_device_control_req;
};
