#include "ParseXml.hpp"
#include "XmlUtils.hpp"

int g_nNode=0;
string g_strXML;
string g_strNodeName;

xmlChar* GetNodeProp(xmlNodePtr data,const char* name)
{
	char *c1;
	c1=XmlUtils::ConvertEnc("gbk","utf-8",name);
	xmlChar *pXml = xmlGetProp(data,BAD_CAST c1);
	free(c1);
	if (pXml != NULL)
	{
		c1=XmlUtils::ConvertEnc("utf-8","gbk",(char *)pXml);
		int t = strlen(c1);
		memset(pXml,0x00,strlen((char *)pXml));
		memcpy(pXml,c1,strlen(c1));
		//memcpy(pXml,"C:\存储B",8);
		free(c1);
	}

	return pXml;
}

string GetXMLFieldValue(string strXML,string strName,char *cXmlNode)
{
	xmlDocPtr xmlDoc = XmlUtils::StringToDoc(strXML);
	if (xmlDoc == NULL)
	{
		xmlFreeDoc(xmlDoc);
		return "";
	}
	xmlXPathObjectPtr pXmlPathObj = XmlUtils::QureyNode(xmlDoc,cXmlNode);
	if (NULL == pXmlPathObj)
	{
		xmlFreeDoc(xmlDoc);
		return "";
	}

	xmlNodePtr pCurNode = NULL;
	xmlNodeSetPtr pNodeSet = NULL;
	if(((pNodeSet = pXmlPathObj->nodesetval) == NULL) || ((pCurNode = pNodeSet->nodeTab[0]) == NULL))
	{
		xmlXPathFreeObject (pXmlPathObj);	
		xmlFreeDoc(xmlDoc);
		return "";
	}
	string strValue;
	xmlChar *xValue = GetNodeProp(pCurNode,strName.c_str());
	if (NULL != xValue)
	{
		strValue = (char *)xValue;
		xmlFree(xValue);
	}

	xmlXPathFreeObject(pXmlPathObj);
	xmlFreeDoc(xmlDoc);
	return strValue;
}

xmlDocPtr g_xmlDoc = NULL;
xmlXPathObjectPtr g_pXmlPathObj = NULL;

//查看是否有第i个节点
bool GetNstNode(string strName,int i)
{
	if (i < 1)
		return false;

	if (g_pXmlPathObj == NULL)
		return false;

	xmlNodeSetPtr pNodeSet = NULL;
	if(( pNodeSet = g_pXmlPathObj->nodesetval) == NULL)
	{
		return false;
	}

	int nNum= pNodeSet->nodeNr;

	if (nNum >= i)
		return true;
	else 
		return false;
}

bool FindFirstNode(string strXML,string strName)
{
	if (g_pXmlPathObj != NULL)
	{
		xmlXPathFreeObject(g_pXmlPathObj);
		g_pXmlPathObj = NULL;
	}

	if (g_xmlDoc != NULL)
	{
		xmlFreeDoc(g_xmlDoc);
		g_xmlDoc = NULL;
	}
	//打开XML文件
	g_xmlDoc = XmlUtils::StringToDoc(strXML);
	if (g_xmlDoc == NULL)
	{
		return false;
	}

	g_pXmlPathObj = XmlUtils::QureyNode(g_xmlDoc,"/root/data");
	if (NULL == g_pXmlPathObj)
	{
		xmlFreeDoc(g_xmlDoc);
		g_xmlDoc = NULL;
		return false;
	}

	if ( !GetNstNode(strName,1) )
	{
		if (g_pXmlPathObj != NULL)
		{
			xmlXPathFreeObject(g_pXmlPathObj);
			g_pXmlPathObj = NULL;
		}

		if (g_xmlDoc != NULL)
		{
			xmlFreeDoc(g_xmlDoc);
			g_xmlDoc = NULL;
		}
		return false;
	}

	g_strXML = strXML;
	g_strNodeName = strName;
	g_nNode=1;

	return true;
}

bool FindNextNode1()
{
	g_nNode = (g_nNode>0 ? g_nNode+1 : 1);
	bool bRes = GetNstNode(g_strNodeName,g_nNode);
	if ( !bRes )
	{
		if (g_pXmlPathObj != NULL)
		{
			xmlXPathFreeObject(g_pXmlPathObj);
			g_pXmlPathObj = NULL;
		}

		if (g_xmlDoc != NULL)
		{
			xmlFreeDoc(g_xmlDoc);
			g_xmlDoc = NULL;
		}
		return false;
	}
	return true;
}


string GetXMLNodePropValue(string strName)
{
	xmlNodePtr pCurNode = NULL;
	xmlNodeSetPtr pNodeSet = NULL;
	if(((pNodeSet = g_pXmlPathObj->nodesetval) == NULL) || ((pCurNode = pNodeSet->nodeTab[g_nNode-1]) == NULL))
	{
		return "";
	}

	string strValue;
	xmlChar *xValue = GetNodeProp(pCurNode,strName.c_str());
	if (NULL != xValue)
	{
		strValue = (char *)xValue;
		xmlFree(xValue);
	}

	return strValue;
}

bool GetXMLAllNodePropValues( string strXML,list<string> &lstString,list<map<string,string > > &lstPropMap )
{

	lstPropMap.clear();
	xmlDocPtr xmlDoc = XmlUtils::StringToDoc(strXML);
	if (xmlDoc == NULL)
	{
		return false;
	}
	xmlXPathObjectPtr pXmlPathObj = XmlUtils::QureyNode(xmlDoc,"/root");
	if (NULL == pXmlPathObj)
	{
		xmlFreeDoc(xmlDoc);
		return true;
	}

	xmlNodePtr pCurNode = NULL;
	xmlNodeSetPtr pNodeSet = NULL;
	if((pNodeSet = pXmlPathObj->nodesetval) == NULL)
	{
		xmlXPathFreeObject (pXmlPathObj);	
		xmlFreeDoc(xmlDoc);
		return true;
	}

	pNodeSet = pXmlPathObj->nodesetval;
	for (int i=0; i < pNodeSet->nodeNr; i++)
	{
		pCurNode = pNodeSet->nodeTab[i];
		map<string,string > mapProp;
		for (list<string >::iterator it=lstString.begin(); it!=lstString.end(); it++)
		{
			string strValue;
			xmlChar *xValue = GetNodeProp(pCurNode,it->c_str());
			if(xValue!=NULL)
				mapProp.insert(pair<string,string>(it->c_str(),strValue));

			if (NULL != xValue)
			{
				strValue = (char *)xValue;
				xmlFree(xValue);
			}
			
			
		}
		
		lstPropMap.push_back(mapProp);
	}

	xmlXPathFreeObject(pXmlPathObj);
	xmlFreeDoc(xmlDoc);
	return true;
}
