#pragma once

namespace ZvLibs
{	
	class CHttpRequest;

	enum HTTP_HANDLE_TYPE
	{
		HTTP_HANDLE_IMM,
		HTTP_HANDLE_BEFORE,
		HTTP_HANDLE_CONTENT,
		HTTP_HANDLE_AFTER,
	};

	class CHttpHandle  
	{
	public:
		CHttpHandle(CHttpSvr * pHttpSvr, HTTP_HANDLE_TYPE HandleType = HTTP_HANDLE_IMM, const string & szPath = "");
		virtual ~CHttpHandle();

		virtual bool Handle(CHttpRequest & Request);
		string m_szPath;
		CHttpSvr * m_pHttpSvr;
		HTTP_HANDLE_TYPE m_HandleType;
	};
}
