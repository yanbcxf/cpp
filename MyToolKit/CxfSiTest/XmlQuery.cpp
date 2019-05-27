#include "StdAfx.h"

#include "GbUsageEnvironment.h"
#include "CatalogQuery.h"

#include "XmlQuery.h"

#include "DeviceInfoQuery.h"
#include "DeviceStatusQuery.h"
#include "RecordInfoQuery.h"
#include "PresetQuery.h"
#include "CatalogQuery.h"
#include "RecordControl.h"
#include "PtzControl.h"

CXmlQuery::CXmlQuery(CGbUsageEnvironment& env)
:Medium(env)
{
}

CXmlQuery::~CXmlQuery(void)
{
}

bool CXmlQuery::parse_deviceId_and_SN(mxml_node_t *tree, mxml_node_t *query_node, string & deviceId, string & SN)
{
	mxml_node_t *DeviceID_node , *sn_node;
	char id[MAX_SIPD_DEVICE_ID_LEN+1];
	char snbuf[MAX_SIPD_DEVICE_ID_LEN+1];

	DeviceID_node = mxmlFindElement(query_node, tree, "DeviceID", NULL, NULL,MXML_DESCEND);
	sn_node = mxmlFindElement(query_node, tree, "SN", NULL, NULL, MXML_DESCEND);

	if(DeviceID_node!=NULL)
	{
		memset(id, 0, sizeof(id));
		memcpy(id, (void *)mxmlGetText(DeviceID_node, NULL), sizeof(id));
		deviceId = string(id);
	}
	else
		return false;

	if(sn_node!=NULL)
	{
		memset(snbuf, 0, sizeof(snbuf));
		memcpy(snbuf, (void *)mxmlGetText(sn_node, NULL), sizeof(snbuf));
		SN = string(snbuf);
	}
	else 
		return false;

	return true;
}


bool CXmlQuery::parse_response(mxml_node_t *tree, mxml_node_t *response_node, sipd_message_query_or_response  * pReq)
{
	return false;
}

CXmlQuery * CXmlQuery::manscdp_xml_deal1(CGbUsageEnvironment& env, sipd_message_query_or_response  * pReq)
{

	return NULL;
}

CXmlQuery * CXmlQuery::manscdp_xml_deal(CGbUsageEnvironment& env, sipd_message_query_or_response  * pReq)
{

	//LOG4CPLUS_DEBUG(env.m_Logger, "sipd_manscdp_xml_deal Begin...");
	mxml_node_t *tree, *query_node, *CmdType_node, *sn_node, * response_node, * control_node;
	char strbuf[MAX_SIPD_URI];
	mxml_node_t *node;
	int whitespace;
	SIPD_GATEWAY_CFG sipd_cfg;
	memset(strbuf, 0, sizeof(strbuf));

	CXmlQuery * pXmlQuery = NULL;
	if(pReq->body != NULL)
	{
		tree = mxmlLoadString(NULL, pReq->body, MXML_TEXT_CALLBACK);
		if(tree != NULL)
		{
			query_node = mxmlFindElement(tree, tree, QUERY, NULL, NULL,MXML_DESCEND);
			response_node = mxmlFindElement(tree, tree, RESPONSE, NULL, NULL,MXML_DESCEND);
			control_node = mxmlFindElement(tree, tree, "Control", NULL, NULL,MXML_DESCEND);
			
			if(query_node != NULL)
			{
				CmdType_node = mxmlFindElement(query_node, tree, CMDTYPE, NULL, NULL,MXML_DESCEND);
				if(CmdType_node != NULL)
				{
					memcpy(strbuf,  (void *)mxmlGetText(CmdType_node, NULL), sizeof(strbuf));
					if(strcmp(strbuf, DEVICEINFO) == 0) 
					{
						pXmlQuery = new CDeviceInfoQuery(env);
					}
					else if(strcmp(strbuf, DEVICESTATUS) == 0) 
					{
						pXmlQuery = new CDeviceStatusQuery(env);
					}
					else if(strcmp(strbuf, DEVICECATALOG) == 0) 
					{
						pXmlQuery = new CCatalogQuery(env);
					}
					else if(strcmp(strbuf, SIPD_RECORDINFO) == 0) 
					{
						pXmlQuery = new CRecordInfoQuery(env);
					}
					else if(strcmp(strbuf, PERSETQUERY)==0)
					{
						pXmlQuery = new CPresetQuery(env);
					}
					
					if(pXmlQuery)
					{
						//pXmlQuery->parse_uri(pReq);
						pXmlQuery->parse_request(tree, query_node, pReq);
					}
				}
			}
			if(control_node != NULL)
			{
				CmdType_node = mxmlFindElement(control_node, tree, CMDTYPE, NULL, NULL,MXML_DESCEND);
				if(CmdType_node != NULL)
				{
					memcpy(strbuf,  (void *)mxmlGetText(CmdType_node, NULL), sizeof(strbuf));
					if(strcmp(strbuf, "DeviceControl") == 0) 
					{
						mxml_node_t * node;
						node = mxmlFindElement(control_node, tree, "RecordCmd", NULL, NULL,MXML_DESCEND);
						if(node!=NULL)
						{
							pXmlQuery = new CRecordControl(env);
						}

						node = mxmlFindElement(control_node, tree, "PTZCmd", NULL, NULL,MXML_DESCEND);
						if(node!=NULL)
						{
							pXmlQuery = new CPtzControl(env);
						}

						node = mxmlFindElement(control_node, tree, "AlarmCmd", NULL, NULL,MXML_DESCEND);
						if(node!=NULL)
						{
							// 报警复位
						}

						node = mxmlFindElement(control_node, tree, "GuardCmd", NULL, NULL,MXML_DESCEND);
						if(node!=NULL)
						{
							//	布撤防

						}

						node = mxmlFindElement(control_node, tree, "TeleBoot", NULL, NULL,MXML_DESCEND);
						if(node!=NULL)
						{
							//	远程启动

						}
					}

					if(pXmlQuery)
					{
						//pXmlQuery->parse_uri(pReq);
						pXmlQuery->parse_request(tree, control_node, pReq);
					}
				}
			}
			else if(response_node != NULL)
			{
				//	根据 SN 查找本域发出“query”请求的对象
				mxml_node_t   *sn_node;
				string str_SN;

				sn_node = mxmlFindElement(response_node, tree, "SN", NULL, NULL, MXML_DESCEND);
				if(sn_node!=NULL)
				{
					char snbuf[MAX_SIPD_DEVICE_ID_LEN+1];
					memset(snbuf, 0, sizeof(snbuf));
					memcpy(snbuf, (void *)mxmlGetText(sn_node, NULL), sizeof(snbuf));
					str_SN = string(snbuf);
					/*string test = "sipd_manscdp_xml_deal SN = " + str_SN;				
					LOG4CPLUS_DEBUG(env.m_Logger, test.c_str());*/
					Medium::lookupByName(env,str_SN.c_str(), (Medium *&)pXmlQuery);
					if(pXmlQuery)
					{
						pXmlQuery->parse_response(tree, response_node, pReq);
					}
				}
			}
			else
			{
				printf("sipd_manscdp_xml_deal que_node is NULL\n");
			}

			mxmlDelete(tree);
		}
		else
		{
			printf("sipd_manscdp_xml_deal tree is NULL\n");
		}
		//LOG4CPLUS_DEBUG(env.m_Logger, "sipd_manscdp_xml_deal ok");
	}
	else
	{
		printf("sipd_manscdp_xml_deal body is NULL\n");
		LOG4CPLUS_DEBUG(env.m_Logger, "sipd_manscdp_xml_deal body is NULL");
	}

	return pXmlQuery;
}
