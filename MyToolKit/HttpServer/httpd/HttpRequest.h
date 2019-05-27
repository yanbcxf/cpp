#pragma once

#include "HttpSocket.h"

namespace ZvLibs
{
	class CHttpSvr;
	class CHttpService;

	class CHttpRequest : public CHttpSocket
	{
	public:
		CHttpRequest(CHttpService * pHttpService);
		virtual ~CHttpRequest();
		
		CHttpSvr * m_pHttpSvr;
		CHttpService * m_pHttpService;

		// Request
		int m_nMethod;
		int m_nContentLength;
		string m_szMethod;
		string m_szUri;
		string m_szPath;
		string m_szScriptName;
		string m_szRealPath;
		bool m_bKeepAlive;

		// post form data
		int m_nRecvStep;
		int m_nPostRecved;
		string m_szPostHeader;
		string m_szHeader;
		int m_nBoundaryNum;
		string m_szPostBoundary;
		HANDLE m_hPostFile;
		bool m_bPostFormData;
		bool m_bPostFile;
		string m_szPostName, m_szPostFileName;

		map<string, string> m_mapHeader;
		map<string, string> m_mapParam;
		map<string, string> m_mapForm;			// yangbin ½ÓÊÕµÄ json 
		map<string, string> m_mapFilePath;
		map<string, string> m_mapFileName;
		map<string, string> m_mapCookie;
		
		// Authorization
		string m_szUser;
		string m_szPass;

		// Response
		string m_szState;
		map<string, string> m_mapHeaderOut;
		map<string, string> m_mapCookieOut;

		bool m_bHeaderSent;
		string m_szBoundary;

		string GetIp();
		string GetContentType(const string & szFileExt);
		void ParseRealPath();
		bool IsPost();
		void FindForm(const string & szBegin, vector<string> * pvtValue = NULL, vector<string> * pvtFormName = NULL);

		void SetState(const string & szState);
		void SetCharset(const string & szCharset);
		void SetParam(const string & szParam, const string & szValue);
		void SetContentLength(int nLen);
		void SetContentType(const string & szType);
		void SetCookie(const string & szName, const string & szValue);
		void DelParam(const string & szParam);

		bool SendDir(const string & szPath);
		bool SendFile(const string & szPath, bool bDownload=false);
		bool SendHeader();
		bool SendError(const string & szError, const string & szInfo = "");
		bool SendBody(const string & szData);	
		bool SendUtf8Body(string & szData);
		bool AuthenticateBasic(const string &szRealm, bool bSendBody = true);
		bool Location(const string & szUrl="");

		bool RecvRequest();
		bool RecvHeader();
		bool RecvPostData();
		bool RecvFormData();

		void FormUtf8ToAnsi();
		bool FormExist(const string & szFormName);

		void ParseParam(map<string, string> & mapParam, string & str);
		void ParseFormDataHeader(string & szHeader, bool & bFile, string & szName, string & szFileName, string & szBoundary);
		
		void DelTempFile();
		bool ParamExist(const string & szParamName);
		bool FormFileExist(const string & szFileName);
		void SetCacheControl(int nCacheTimeSeconds);
		void ParamUtf8ToAnsi();
		void FileNameUtf8ToAnsi();
		void UriUtf8ToAnsi();

		void FormToXml(ostringstream & ssXml);
		bool IsChunked();
		bool RecvHeaders(char * pHeader, int * pLenHeader, char ** ppExtra, int * pLenExtra);
		bool ParseRequestHeader(char * pBuf, int nLen);
		bool GetLineFromHeader(int & nPosCur, string & szLine);
		bool ParseRequestLine(int & nPosCur);
		bool ParseHeaderLine(int & nPosCur);
		bool HandleRecvedData(char * pBuf, int nLen, int & nHandled);
		bool RecvUrlEncode(char * pExtraData, int nExtraLen);
		void SetRecvStep(int nStep);
		bool ParseHeader();
		bool IsPostFormData();
		bool CheckRecvStep(char * pBuf, int nLen, int & m_nRecvStep, int & nHandled, bool & bEnd);
		bool ParsePostHeader();
		bool ParsePostUrlEncode();
		bool ParsePostContent(char * pBuf, int nLen);
		void SetDate(const string & szDate="");
		void SetLastModified(const string & szDate="");
		string GetFileGmtTime(HANDLE hFile);
		void SetServerName(const string & szServer = "");
	};
}