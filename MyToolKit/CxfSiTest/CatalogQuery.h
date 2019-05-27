#pragma once
#include "medium.h"
#include "XmlQuery.h"

class CCatalogQuery : public CXmlQuery
{
public:
	CCatalogQuery(CGbUsageEnvironment& env);
	~CCatalogQuery(void);

	bool parse_request(mxml_node_t *tree, mxml_node_t *query_node,  sipd_message_query_or_response  * pReq);
	bool parse_response(mxml_node_t *tree, mxml_node_t *response_node,  sipd_message_query_or_response  * pReq);
	
	bool handle_request();
	bool handle_response();

	void send_request(sipd_catalog_req  * pReq, CCatalogQuery * pCatalog);
	void send_response_from_enviroment();
	void send_response_from(CCatalogQuery * pCatalog);
	void send_response(sipd_catalog_rsp_basic_info* pCataBasicInfo, sipd_catalog_item* pCataItem);

	int  get_cataloginfo_pair(int start_index, char* pitem_pair, sipd_catalog_item * pCataItem);

public:
	
	sipd_catalog_req  m_catalog_req;

	sipd_catalog_rsp_basic_info		m_basic_info;
	sipd_catalog_item				m_cata_items[100];
	int		m_current_item;

private:
	CCatalogQuery * m_next;
};
