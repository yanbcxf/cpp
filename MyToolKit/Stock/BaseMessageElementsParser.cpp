#include "StdAfx.h"
#include "BaseMessageElementsParser.h"

CBaseMessageElementsParser::CBaseMessageElementsParser(LPVOID pContext)
:XmlParser(pContext)
{
	bInMsgType = false;
	msg_type = "";
	code = "";
	model = "";
	param1 = "";
	param2 = "";
	param3 = "";
}

CBaseMessageElementsParser::~CBaseMessageElementsParser(void)
{
}


void CBaseMessageElementsParser::characterData(const XML_Char *s, int len)
{
	if(bInMsgType)
	{
		msg_type = string(s, len);
	}
}

void CBaseMessageElementsParser::startElement(const char *name, const char **atts)
{
	if(string(name)=="msg_type")
	{
		bInMsgType = true;
		Parameter(atts);
	}
	else if(string(name)=="data")
	{
		Parameter(atts);
	}
}

void CBaseMessageElementsParser::endElement(const char *name)
{
	if(bInMsgType)
		bInMsgType = false;
}

void CBaseMessageElementsParser::Parameter(const char **atts)
{
	const char ** pStr = atts;
	int column = 0;
	string	columnName;
	while(pStr && (*pStr))
	{
		if(column%2==0)
		{
			columnName = string((char *)*pStr);
		}
		else if(column%2==1)
		{
			if(columnName=="Param1" || columnName=="param1")
				param1 = (char *)*pStr;
			if(columnName=="Param2" || columnName=="param2")
				param2 = (char *)*pStr;
			if(columnName=="Param3" || columnName=="param3")
				param3 = (char *)*pStr;

			if(columnName=="Code" || columnName=="code")
				code = (char *)*pStr;
			if(columnName=="model" || columnName=="Model")
				model = (char *)*pStr;
		}
		column++;
		pStr++;
	}
}
