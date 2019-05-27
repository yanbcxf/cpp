#pragma once

class CXmlBase : public CHttpHandle
{
public:
	CXmlBase(CHttpSvr * pHttpSvr, const char * szPath = "");
	virtual ~CXmlBase();
	
	virtual bool Handle(CHttpRequest & Request);
	virtual void GetXml(CHttpRequest & Request, string & szOption, ostringstream & ssXml, bool & bHandled, string & szInfo, string & szWarning, string & szError, string & szNewUrl) = 0;
};
