#pragma once
#include "XmlBase.h"

class CXmlExplorer : public CXmlBase
{
public:
	CXmlExplorer(CHttpSvr * pHttpSvr);
	~CXmlExplorer(void);

	void GetXml(CHttpRequest & Request, string & szOption, tostringstream & ssXml, bool & bHandled, string & szInfo, string & szWarning, string & szError, string & szNewUrl);
	void SendJsonTree(CHttpRequest & Request);
	void SendFileIcon(CHttpRequest & Request);
};
