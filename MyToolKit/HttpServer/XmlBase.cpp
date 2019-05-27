#include "stdafx.h"
#include "XmlBase.h"

CXmlBase::CXmlBase(CHttpSvr * pHttpSvr, const char * szPath)
:CHttpHandle(pHttpSvr, HTTP_HANDLE_CONTENT, szPath)
{

}

CXmlBase::~CXmlBase()
{

}

bool CXmlBase::Handle(CHttpRequest & Request)
{
	DWORD dwTickBegin = GetTickCount();

	Request.UriUtf8ToAnsi();
	Request.ParamUtf8ToAnsi();
	Request.FileNameUtf8ToAnsi();
	Request.FormUtf8ToAnsi();

	ostringstream ssXml;
	string szXslFile, szRoot;
	string szInfo, szError, szWarning, szNewUrl;
	bool bHandled = false;

	int p = Request.m_szPath.rfind('.');
	if(p!=string::npos)
		szXslFile = Request.m_szPath.substr(0, p);

	p = szXslFile.rfind('/');
	if(p!=string::npos && p!=szXslFile.length()-1)
		szRoot = szXslFile.substr(p+1, string::npos);
	else
		szRoot = szXslFile;

	Request.SetParam("Content-Type", "text/xml; charset=utf-8");

	ssXml << "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?>\r\n";
	ssXml << "<?xml-stylesheet type=\"text/xsl\" href=\"" << szRoot << ".xsl\"?>\r\n";
	ssXml << "<" << szRoot << ">\r\n";

	string szOption = Request.m_mapParam["Option"];

	GetXml(Request, szOption, ssXml, bHandled, szInfo, szWarning, szError, szNewUrl);
	
	if(bHandled)	
		return true;

	if(szNewUrl.length()>0)
	{
		Request.Location(szNewUrl);
		return true;
	}

	// System
	ssXml << "\r\n<System>\r\n";
	
	if(Request.ParamExist("Info"))
		szInfo = Request.m_mapParam["Info"];

	if(szInfo.length()>0)
	{
		ssXml << "\t<Info>" << EscapeHtmlCharacter(szInfo) << "</Info>\r\n";
	}
	else if(szWarning.length()>0)
	{
		ssXml << "\t<Warning>" << EscapeHtmlCharacter(szWarning) << "</Warning>\r\n";
	}

	else if(szError.length()>0)
	{
		ssXml << "\t<Error>" << EscapeHtmlCharacter(szError) << "</Error>\r\n";
	}

	ssXml << "\t<Uri>" << EscapeHtmlCharacter(Request.m_szUri) << "</Uri>\r\n";
	ssXml << "\t<TimeConsuming>" << (GetTickCount()-dwTickBegin) << "</TimeConsuming>\r\n";
	ssXml << "\t<Option>" << Request.m_mapParam["Option"] << "</Option>\r\n";
	ssXml << "\t<IsPost>" << (Request.IsPost()?"true":"false") << "</IsPost>\r\n";
	ssXml << "</System>\r\n";

	// Form
	Request.FormToXml(ssXml);

	ssXml << "</" << szRoot << ">";

	Request.SetCacheControl(0);

 	bool b = Request.SendUtf8Body(ssXml.str());

	return b;
}