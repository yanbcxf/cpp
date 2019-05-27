#pragma once
#include "xmlquery.h"

class CRecordInfoQuery :
	public CXmlQuery
{
public:
	CRecordInfoQuery(CGbUsageEnvironment& env);
	~CRecordInfoQuery(void);

	bool parse_request(mxml_node_t *tree, mxml_node_t *query_node, sipd_message_query_or_response  * pReq);
	bool parse_response(mxml_node_t *tree, mxml_node_t *response_node,  sipd_message_query_or_response  * pReq);

	bool handle_request();
	bool handle_response();

	void send_request(sipd_recordinfo_req  * pReq, CRecordInfoQuery * pDeviceInfo);
	void send_response(sipd_recordinfo_adaptor_rsp* pDeviceInfo);
	void send_response_from(CRecordInfoQuery * pCatalog);
	
public:
	sipd_recordinfo_req			m_recordinfo_req;
	sipd_recordinfo_adaptor_rsp	m_recordinfo_rsp;
private:
	CRecordInfoQuery * m_next;

private:
	int get_recordinfo_pair(int start_index, char* pitem_pair, sipd_recordinfo_adaptor_rsp *prsp);
};
