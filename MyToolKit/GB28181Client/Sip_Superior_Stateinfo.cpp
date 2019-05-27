
#include "stdafx.h"
#include "Gb28181_Layer.h"

#include <eXosip.h>
#include <osip_headers.h>
#include <osip_mt.h>



bool parse_deviceId_and_SN(mxml_node_t *tree, mxml_node_t *query_node, string & deviceId, string & SN);

//////////////////////////////////////////////////////////////////////////

bool handle_stateinfo_request(mxml_node_t *tree, mxml_node_t *response_node, sipd_message_query_or_response  * pReq)
{
	mxml_node_t  *node, * item_node, * CmdType_node;

	CmdType_node = mxmlFindElement(response_node, tree, "CmdType", NULL, NULL,MXML_DESCEND);
	if(CmdType_node != NULL)
	{
		char strbuf[MAX_SIPD_URI];
		memset(strbuf, 0, sizeof(strbuf));
		sprintf_s(strbuf, MAX_SIPD_URI, "%s" , mxmlGetText(CmdType_node, NULL));
		//memcpy(strbuf,  (const char *)mxmlGetText(CmdType_node, NULL), sizeof(strbuf));
		if(strcmp(strbuf, "DeviceStatus") != 0)
			return false;
	}

	string strDeviceId;
	string strSN;
	if(parse_deviceId_and_SN(tree, response_node,strDeviceId, strSN)==false)
		return false;

	stringstream ss;
	/*ss << "CCatalogQuery::parse_response m_current_item = " << m_current_item;
	LOG4CPLUS_DEBUG(envir().m_Logger, ss.str().c_str());*/

	

	/*if(current_item>0)
	{
		if(pSipd_catalog_resp)
			pSipd_catalog_resp(&basic_info, cata_items, pReq->tid);
	}
	else
	{
		if(pSipd_catalog_resp)
			pSipd_catalog_resp(&basic_info, NULL, pReq->tid);
	}*/

	return true;
}