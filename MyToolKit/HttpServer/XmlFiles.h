#pragma once

#include "XmlBase.h"

class CXmlFiles : public CXmlBase
{
public:
	CXmlFiles(CHttpSvr * pHttpSvr);
	~CXmlFiles(void);
	
	void GetXml(CHttpRequest & Request, string & szOption, ostringstream & ssXml, bool & bHandled, string & szInfo, string & szWarning, string & szError, string & szNewUrl);
	bool GetXmlVoice(ostringstream & ssXml);
	bool Move(CHttpRequest & Request);
	bool Download(CHttpRequest & Request, bool bDownload);
};