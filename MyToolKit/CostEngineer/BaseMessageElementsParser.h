#pragma once
#include "xmlparser.h"

class CBaseMessageElementsParser :
	public XmlParser
{
public:
	CBaseMessageElementsParser(LPVOID pContext);
	~CBaseMessageElementsParser(void);

	virtual void characterData(const XML_Char *s, int len);
	virtual void startElement(const char *name, const char **atts);
	virtual void endElement(const char *name);
	void Parameter(const char **atts);

private: 
	bool bInMsgType;
public:
	string msg_type;
	string code;
	string model;
	string param1, param2, param3;
};
