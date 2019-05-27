
#include "stdafx.h"
#include "HttpRequest.h"
#include "HttpSvr.h"
#include "UriCodec.h"
#include "../utils/ZvUtils.h"
#include "HttpDefine.h"
#include "HttpService.h"
#include <time.h>

namespace ZvLibs
{
	CHttpRequest::CHttpRequest(CHttpService * pHttpService)
	:CHttpSocket(false, pHttpService->m_Socket)
	{
		m_bKeepAlive = true;
		m_pHttpService = pHttpService;
		m_pHttpSvr = pHttpService->m_pHttpSvr;
		m_szState = "200 OK";
		m_bHeaderSent = false;
		m_nRecvStep = HTTP_RECV_HEADER;
		m_nBoundaryNum = 0;
		m_bPostFormData = false;
		m_hPostFile = INVALID_HANDLE_VALUE;
		m_nPostRecved = 0;
	}

	CHttpRequest::~CHttpRequest()
	{
		DelTempFile();
	}

	void CHttpRequest::DelTempFile()
	{
		map<string, string>::iterator it;

		for(it = m_mapFilePath.begin(); it!=m_mapFilePath.end(); it++)
		{
			DeleteFile(it->second.c_str());
		}
	}

	bool CHttpRequest::GetLineFromHeader(int & nPosCur, string & szLine)
	{
		if(nPosCur>=(int)m_szHeader.length()) return false;

		int nPos = m_szHeader.find(CRLF, nPosCur);
		if(nPos==string::npos || nPos-nPosCur<=0) return false;
		szLine = m_szHeader.substr(nPosCur, nPos-nPosCur);
		nPosCur = nPos+2;
		return true;
	}

	bool CHttpRequest::ParseRequestLine(int & nPosCur)
	{
		// GET /search?hl=en&q=HTTP&btnG=Google+Search HTTP/1.1 
		string szRequest;

		if(!GetLineFromHeader(nPosCur, szRequest)) return false;

		// query method
		int p = szRequest.find(' ');
		if(p==string::npos)
			return false;

		m_szMethod = szRequest.substr(0, p);

		transform(m_szMethod.begin(), m_szMethod.end(), m_szMethod.begin(), toupper);

		if(m_szMethod.compare("GET")==0)
			m_nMethod = HTTP_QUERY_METHOD_GET;

		else if(m_szMethod.compare("POST")==0)
			m_nMethod = HTTP_QUERY_METHOD_POST;
		else
		{
			return false;
		}

		// uri
		int p1 = szRequest.find(' ', p+1);
		if(p1==string::npos || p1-p-1<=0)
			return false;

		m_szUri = UriDecode(szRequest.substr(p+1, p1-p-1));
		StringReplace(m_szUri, "\\", "/");

		if(m_szUri.length()<=0 || m_szUri[0]!='/')
			m_szUri = '/' + m_szUri;

		// path
		p = m_szUri.find('?');
		if(p==string::npos)
			m_szPath = m_szUri;
		else
		{
			m_szPath = m_szUri.substr(0, p);

			// param
			string szParam = m_szUri.substr(p+1, -1);
			ParseParam(m_mapParam, szParam);
		}
		//
		m_szPath = m_szPath.substr(1, m_szPath.length()-1);

		ParseRealPath();

		return true;
	}

	bool CHttpRequest::ParseHeaderLine(int & nPosCur)
	{	
		string szRequest;

		if(!GetLineFromHeader(nPosCur, szRequest))
			return false;

		int p = szRequest.find(": ", 0);
		if(p>0 && p!=string::npos && p!=szRequest.length()-2)
		{
			string szParam, szValue;
			szParam = szRequest.substr(0, p);
			szValue = szRequest.substr(p+2, -1);
			m_mapHeader[szParam] = szValue;
		}

		return true;
	}

	bool CHttpRequest::ParseRequestHeader(char * pBuf, int nLenBuf)
	{
		int nPosCur = 0;
		if(!ParseRequestLine(nPosCur)) return false;
		
		while(ParseHeaderLine(nPosCur));

		int p;

		// keep alive
		m_bKeepAlive = m_mapHeader["Connection"]=="close";

		// Authorization: Basic QsdfgWGHffuIcaNlc2FtZQ==
		p = m_mapHeader["Authorization"].find("Basic ");
		if(p!=string::npos)
		{
			string szAuth = m_mapHeader["Authorization"].substr(p+6, string::npos);

			szAuth = Base64Decode(szAuth);

			// user:name
			p = szAuth.find(':');

			m_szUser = szAuth.substr(0, p);
			if(p!=string::npos)
				m_szPass = szAuth.substr(p+1, string::npos);
		}

		// cookies
		if(m_mapHeader.find("Cookie")!=m_mapHeader.end() && m_mapHeader["Cookie"].length()>0)
		{
			vector<string> vtItems;

			StringSplit(m_mapHeader["Cookie"].c_str(), "; ", vtItems);

			for(int i=0; i<(int)vtItems.size(); i++)
			{
				int p=vtItems[i].find('=');
				if(p!=string::npos && p!=vtItems[i].length()-1 && p!=0)
				{
					m_mapCookie[vtItems[i].substr(0, p)] = vtItems[i].substr(p+1, string::npos);
				}
			}
		}

		return true;
	}

	void CHttpRequest::SetRecvStep(int nStep)
	{
		if(m_nRecvStep==HTTP_RECV_POST_CONTENT)
		{
			if(m_bPostFile)
			{
				SetFilePointer(m_hPostFile, 0-m_szBoundary.length(), NULL, FILE_END);
				SetEndOfFile(m_hPostFile);
				CloseHandle(m_hPostFile);
				m_hPostFile = INVALID_HANDLE_VALUE;
			}
			else
			{
				for(int i=0; i<(int)m_szBoundary.length(); i++)
				{
					if(m_mapForm[m_szPostName].length()>0)
						m_mapForm[m_szPostName].erase(m_mapForm[m_szPostName].length()-1);
				}
			}
		}

		m_szHeader = "";
		m_nBoundaryNum = 0;
		m_nRecvStep = nStep;
	}

	bool CHttpRequest::ParseHeader()
	{
		int nPosCur = 0;
		// 1. request line
		string szRequestLine;

		if(!GetLineFromHeader(nPosCur, szRequestLine)) return false;

		int p = szRequestLine.find(' ');
		if(p==string::npos)	return false;

		m_szMethod = m_szHeader.substr(0, p);

		if(m_szMethod.compare("GET")==0)
			m_nMethod = HTTP_QUERY_METHOD_GET;
		else if(m_szMethod.compare("POST")==0)
			m_nMethod = HTTP_QUERY_METHOD_POST;
		else
			return false;

		// uri
		int p1 = szRequestLine.find(' ', p+1);
		if(p1==string::npos || p1-p-1<=0) return false;

		m_szUri = UriDecode(szRequestLine.substr(p+1, p1-p-1));
		StringReplace(m_szUri, "\\", "/");

		if(m_szUri.length()<=0 || m_szUri[0]!='/')
			m_szUri = '/' + m_szUri;

		// path
		p = m_szUri.find('?');
		if(p==string::npos)
			m_szPath = m_szUri;
		else
		{
			m_szPath = m_szUri.substr(0, p);

			// param
			string szParam = m_szUri.substr(p+1, -1);
			ParseParam(m_mapParam, szParam);
		}
		//
		m_szPath = m_szPath.substr(1, m_szPath.length()-1);

		ParseRealPath();

		// 2. headers 
		while(GetLineFromHeader(nPosCur, szRequestLine))
		{
			int p = szRequestLine.find(": ", 0);
			
			if(p>0 && p!=string::npos && p!=szRequestLine.length()-2)
			{
				string szParam, szValue;
				szParam = szRequestLine.substr(0, p);
				szValue = szRequestLine.substr(p+2, -1);
				m_mapHeader[szParam] = szValue;
			}
		}

		// keep alive
		m_bKeepAlive = m_mapHeader["Connection"]=="keep-alive";

		// Authorization: Basic QsdfgWGHffuIcaNlc2FtZQ==
		p = m_mapHeader["Authorization"].find("Basic ");
		if(p!=string::npos)
		{
			string szAuth = m_mapHeader["Authorization"].substr(p+6, string::npos);

			szAuth = Base64Decode(szAuth);

			// user:name
			p = szAuth.find(':');

			m_szUser = szAuth.substr(0, p);
			if(p!=string::npos)
				m_szPass = szAuth.substr(p+1, string::npos);
		}

		// cookies
		if(m_mapHeader.find("Cookie")!=m_mapHeader.end() && m_mapHeader["Cookie"].length()>0)
		{
			vector<string> vtItems;

			StringSplit(m_mapHeader["Cookie"].c_str(), "; ", vtItems);

			for(int i=0; i<(int)vtItems.size(); i++)
			{
				int p=vtItems[i].find('=');
				if(p!=string::npos && p!=vtItems[i].length()-1 && p!=0)
				{
					m_mapCookie[vtItems[i].substr(0, p)] = vtItems[i].substr(p+1, string::npos);
				}
			}
		}

		if(IsPost())
		{
			m_bPostFormData = (m_mapHeader["Content-Type"].find("multipart/form-data")!=string::npos);
			m_nContentLength = atoi(m_mapHeader["Content-Length"].c_str());
		}

		return true;		
	}


	bool CHttpRequest::ParsePostHeader()
	{	
		int p, p1;

		// name
		p = m_szHeader.find("name=\"");
		if(p==string::npos) return false;
		p1 = m_szHeader.find("\"", p+6);
		if(p1<=p+6) return false;
		m_szPostName = m_szHeader.substr(p+6, p1-p-6);

		m_bPostFile = false;

		// filename
		p = m_szHeader.find("filename=\"");
		if(p!=string::npos)
		{
			p1 = m_szHeader.find("\"", p+10);

			if(p1>p+10)
			{
				m_bPostFile = true;
				m_szPostFileName = m_szHeader.substr(p+10, p1-p-10);
			}
		}

		int nPosCur = 0;

		// boundary
		if(m_szBoundary.length()<=0)
		{
			if(!GetLineFromHeader(nPosCur, m_szBoundary) || m_szBoundary.length()<=0) return false;

			m_szBoundary.insert(0, CRLF);
			m_szBoundary += CRLF;
		}

		if(m_bPostFile)
		{
			if(m_szPostName.length()>0 && m_szPostFileName.length()>0)
			{
				MakeDirectory(".\\temp");

				m_mapFileName[m_szPostName] = m_szPostFileName;
				m_szPostFileName = GetAppPath()+"\\temp\\"+GetRandomString(8);
				m_mapFilePath[m_szPostName] = m_szPostFileName;
			}

			m_hPostFile = CreateFile(m_szPostFileName.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, NULL, NULL);

			if(m_hPostFile==INVALID_HANDLE_VALUE) return false;
		}		

		return true;
	}

	bool CHttpRequest::ParsePostContent(char * pBuf, int nLen)
	{
		if(m_bPostFile)
		{
			DWORD dwWritten;

			if(!WriteFile(m_hPostFile, pBuf, nLen, &dwWritten, NULL) || (int)dwWritten!=nLen)
				return false;
		}
		else
		{
			for(int i=0; i<nLen; i++)
			m_mapForm[m_szPostName] += pBuf[i];
			if(m_mapForm[m_szPostName].length()>LEN_MAX_HEADER) return false;
		}

		return true;
	}

	bool CHttpRequest::ParsePostUrlEncode()
	{
		ParseParam(m_mapForm, UriDecode(m_szHeader));

		return true;
	}

	bool CHttpRequest::CheckRecvStep(char * pBuf, int nLen, int & m_nRecvStep, int & nHandled, bool & bEnd)
	{
		int nStepOld = m_nRecvStep;
		bEnd = false;
		static string szCrLf = CRLFCRLF;

		string & szBoundary = (m_nRecvStep==HTTP_RECV_POST_CONTENT)?m_szBoundary:szCrLf;

		int i=0;

		for(i=0; i<nLen; i++)
		{
			if(szBoundary[m_nBoundaryNum]==pBuf[i])
			{
				if(++m_nBoundaryNum>=(int)szBoundary.length())
				{
					bEnd = true;
					i++;
					break;
				}
			}
			else
			{
				m_nBoundaryNum = (pBuf[i]==szBoundary[0])?1:0;
			}
		}

		nHandled += i;

		if(nStepOld!=HTTP_RECV_HEADER)
		{
			m_nPostRecved += nHandled;
			if(m_nPostRecved>=m_nContentLength)
			{
				bEnd = true;
			}
		}

		return true;
	}

	bool CHttpRequest::HandleRecvedData(char * pBuf, int nLen, int & nHandled)
	{
		if(nLen<=0) return true;

		int i = 0;
		bool bEnd;

		int nStepOld = m_nRecvStep;

		CheckRecvStep(pBuf, nLen, m_nRecvStep, nHandled, bEnd);
		
		switch(nStepOld)
		{
		case HTTP_RECV_HEADER:
		case HTTP_RECV_POST_URL_ENCODE:
		case HTTP_RECV_POST_HEADER:

			if(m_szHeader.length()>LEN_MAX_HEADER) return false;

			for(i=0; i<nHandled; i++)
				m_szHeader += pBuf[i];

			break;

		case HTTP_RECV_POST_CONTENT:

			if(!ParsePostContent(pBuf, nHandled)) return false;

			break;
		}

		if(bEnd)
		{
			switch(nStepOld)
			{
			case HTTP_RECV_HEADER:
				if(!ParseHeader()) return false;
				if(!IsPost() || m_nContentLength<=0)
					SetRecvStep(HTTP_RECV_DONE);
				else if(IsPostFormData())
					SetRecvStep(HTTP_RECV_POST_HEADER);
				else 
					SetRecvStep(HTTP_RECV_POST_URL_ENCODE);
				break;

			case HTTP_RECV_POST_URL_ENCODE:
				if(!ParsePostUrlEncode()) 
					return false;
				SetRecvStep(HTTP_RECV_DONE);
				break;

			case HTTP_RECV_POST_HEADER:
				if(!ParsePostHeader()) return false;

				SetRecvStep(HTTP_RECV_POST_CONTENT);
				break;

			case HTTP_RECV_POST_CONTENT:
				SetRecvStep(HTTP_RECV_POST_HEADER);
				break;
			}
		}

		if(nStepOld!=HTTP_RECV_HEADER)
		{
			if(m_nPostRecved>=m_nContentLength)
			{
				SetRecvStep(HTTP_RECV_DONE);
			}
		}

		return true;
	}

	bool CHttpRequest::RecvRequest()
	{
		char buf[LEN_RECV_ONCE];
		int nRecved;
		
		while(m_nRecvStep!=HTTP_RECV_DONE)
		{
			if(!Recv(buf, LEN_RECV_ONCE, &nRecved))
				return false;
			
			int nHandledTotal = 0;

			while(nHandledTotal<nRecved)
			{
				int nHandled = 0;
				if(!HandleRecvedData(buf+nHandledTotal, nRecved-nHandledTotal, nHandled))
					return false;
				nHandledTotal += nHandled;
			}
		}

		return true;
	}

	void CHttpRequest::ParseParam(map<string, string> & mapParam, string & str)
	{
		//n=xxx&name=value&name1=value2&name2=value3

		string & szDecode = str; //UriDecode(str);

		if(szDecode.length()<=0) return;

		vector<string> vtItems;

		StringSplit(szDecode.c_str(), "&", vtItems);

		for(int i=0; i<(int)vtItems.size(); i++)
		{
			int p=vtItems[i].find('=');

			if(p!=0)
			{	
				string szName, szValue;

				szName = szValue = "";

				szName = vtItems[i].substr(0, p);
				
				if(p!=string::npos && p!=vtItems[i].length()-1)
				{
					szValue = vtItems[i].substr(p+1, string::npos);
				}
				mapParam[szName] = szValue;
			}
		}
	}

	void CHttpRequest::ParseFormDataHeader(string & szHeader, bool & bFile, string & szName, string & szFileName, string & szBoundary)
	{
		int p, p1;

		bFile = false;
		szName = szFileName = "";

		// name
		p = szHeader.find("name=\"");
		if(p!=string::npos)
		{
			p1 = szHeader.find("\"", p+6);

			if(p1>p+6)
			{
				szName = szHeader.substr(p+6, p1-p-6);
				// szName = Utf8ToAnsi(szName);
			}
		}

		// filename
		p = szHeader.find("filename=\"");
		if(p!=string::npos)
		{
			bFile = true;
			p1 = szHeader.find("\"", p+10);

			if(p1>p+10)
			{
				szFileName = szHeader.substr(p+10, p1-p-10);
				// szFileName = Utf8ToAnsi(szFileName);
			}
		}

		// boundary
		p = szHeader.find("boundary=\"");
		if(p!=string::npos)
		{
			bFile = true;
			p1 = szHeader.find("\"", p+9);

			if(p1>p+9)
			{
				szBoundary = CRLF+szHeader.substr(p+9, p1-p-9);
			}
		}
	}

	void CHttpRequest::SetState(const string & szState)
	{
		m_szState = szState;
	}

	void CHttpRequest::SetParam(const string & szParam, const string & szValue)
	{
		m_mapHeaderOut[szParam] = szValue;
	}

	void CHttpRequest::DelParam(const string & szParam)
	{
		map<string, string>::iterator it;

		it = m_mapHeaderOut.find(szParam);
		if(it!=m_mapHeaderOut.end())
		{
			m_mapHeaderOut.erase(it);
		}
	}

	void CHttpRequest::SetContentLength(int nLen)
	{
		if(nLen<=0)
		{
			DelParam("Content-Length");
			SetParam("Transfer-Encoding", "chunked");
		}
		else
		{
			string szValue;
			stringstream ss;  
			ss << nLen;  
			ss >> szValue;
			DelParam("Transfer-Encoding");
			SetParam("Content-Length", szValue.c_str());
		}
	}

	void CHttpRequest::FileNameUtf8ToAnsi()
	{
		map<string, string>::iterator it;
		for(it=m_mapFileName.begin(); it!=m_mapFileName.end(); it++)
		{
			m_mapFileName[it->first] = Utf8ToTstring(it->second);
		}
	}

	void CHttpRequest::ParamUtf8ToAnsi()
	{
		map<string, string>::iterator it;
		for(it=m_mapParam.begin(); it!=m_mapParam.end(); it++)
		{
			m_mapParam[it->first] = Utf8ToTstring(it->second);
		}
	}

	void CHttpRequest::UriUtf8ToAnsi()
	{
		m_szUri = Utf8ToTstring(m_szUri);
	}

	void CHttpRequest::FormUtf8ToAnsi()
	{
		map<string, string>::iterator it;
		for(it=m_mapForm.begin(); it!=m_mapForm.end(); it++)
		{
			m_mapForm[it->first] = Utf8ToTstring(it->second);
		}
	}

	bool CHttpRequest::AuthenticateBasic(const string & szRealm, bool bSendBody)
	{
		string str = string("Basic realm=\"")+szRealm+"\"";

		SetParam("Content-Type", "text/xml");
		SetParam("WWW-Authenticate", str.c_str());
		SetState("401 Unauthorized");
		SendHeader();
		if(bSendBody)
		{
			SendBody("401 Unauthorized");
		}
		return true;
	}

	void CHttpRequest::SetCookie(const string & szName, const string & szValue)
	{
		m_mapCookieOut[szName] = szValue;

		m_mapHeaderOut["Set-Cookie"] = "";

		map<string, string>::iterator it;
		int i=0;

		for(it=m_mapCookieOut.begin(); it!=m_mapCookieOut.end(); it++)
		{
			if(i>0)
			{
				m_mapHeaderOut["Set-Cookie"] += "\r\nSet-Cookie: ";
			}
			m_mapHeaderOut["Set-Cookie"]+=it->first;
			m_mapHeaderOut["Set-Cookie"]+="=";
			m_mapHeaderOut["Set-Cookie"]+=it->second;
			i++;
		}
	}

	bool CHttpRequest::IsChunked()
	{
		return (m_mapHeaderOut.find("Content-Length")==m_mapHeaderOut.end());
	}

	void CHttpRequest::SetServerName(const string & szServer)
	{
		SetParam("Server", szServer.length()>0?szServer:"zhengv@gmail.com");
	}

	void CHttpRequest::SetDate(const string & szDate)
	{
		if(szDate.length()<=0)
		{
			char szDT[128];
			struct tm *newtime;
			time_t ltime;

			time(&ltime);
			newtime = gmtime(&ltime);
			strftime(szDT, 128,	"%a, %d %b %Y %H:%M:%S GMT", newtime);

			SetParam("Date", szDT);
		}
		else
			SetParam("Date", szDate);
	}

	bool CHttpRequest::SendHeader()
	{
		string szHeader = "HTTP/1.1 ";
		szHeader+=m_szState+CRLF;
		
		SetServerName();
		SetDate();
		
		if(IsChunked())
			SetContentLength(-1);

		map<string, string>::iterator it;
		for(it=m_mapHeaderOut.begin(); it!=m_mapHeaderOut.end(); it++)
		{
			if(it->second!="")
			{
				szHeader+=it->first+": ";
				szHeader+=it->second+CRLF;
			}
		}

		szHeader+=CRLF;

		m_bHeaderSent = true;
		
		return Send(szHeader.c_str(), szHeader.length());
	}

	bool CHttpRequest::SendUtf8Body(string & szData)
	{
		return SendBody(TstringToUtf8(szData));
	}

	bool CHttpRequest::SendBody(const string & szData)
	{
		if(!m_bHeaderSent)
		{
			m_bHeaderSent = true;
			if(!SendHeader())
				return false;
		}

		if(IsChunked())
		{
			if(szData=="")
			{
				return (Send("0") && Send(CRLFCRLF));
			}

			char buf[10] = {0};
			sprintf(buf, "%x%s", szData.length(), CRLF);

			return Send(buf) && Send(szData) && Send(CRLF);
		}
		else
		{
			return Send(szData);
		}
	}

	bool CHttpRequest::Location(const string & szUrl)
	{
		if(szUrl=="")
			SetParam("Location", m_szUri);
		else 
			SetParam("Location", szUrl);
		return SendError("302 Found");
	}

	bool CHttpRequest::SendError(const string & szError, const string & szInfo)
	{
		string str = szError;
		if(szInfo.length()>0)
			str += " , " + szInfo;

		SetState(str);

		return SendHeader()&&SendBody(str);
	}

	void CHttpRequest::SetContentType(const string & szType)
	{
		if(szType.length()<=0) return;

		string str = m_mapHeaderOut["Content-Type"];

		int p;
		string szCharset="";
		if((p=str.find("; charset="))!=string::npos)
		{
			szCharset = str.substr(p);
		}

		m_mapHeaderOut["Content-Type"] = szType + szCharset;
	}

	void CHttpRequest::SetCharset(const string & szCharset)
	{
		string str = m_mapHeaderOut["Content-Type"];
		int p;

		if(str.length()<=0)
		{
			m_mapHeaderOut["Content-Type"] = "text/html; charset=";
			m_mapHeaderOut["Content-Type"] += szCharset;
		}
		else
		{	
			if((p=str.find("; charset="))!=string::npos)
			{
				m_mapHeaderOut["Content-Type"] = str.substr(0, p);	
			}
			m_mapHeaderOut["Content-Type"] += "; charset=";
			m_mapHeaderOut["Content-Type"] += szCharset;
		}
	}

	bool CHttpRequest::SendDir(const string & szPath)
	{
		WIN32_FIND_DATA fd;

		string str = szPath + "\\*.*";

		HANDLE hFind = FindFirstFile(str.c_str(), &fd);

		if(hFind==INVALID_HANDLE_VALUE)
		{
			SendError("404 Not Found");
			return true;
		}
		
		ostringstream ss;

		ss << ""
			"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\r\n"
			"<html xmlns=\"http://www.w3.org/1999/xhtml\">\r\n"
			"<head>\r\n"
			"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\r\n"
			"<title></title>\r\n"
			"</head>\r\n"

			"<body>\r\n/" << m_szPath <<
			
			"<hr />\r\n"
			"<table width=\"100%\" border=\"0\">\r\n";

		while(hFind!=INVALID_HANDLE_VALUE)
		{	
			if(strcmp(fd.cFileName, ".")!=0)
			{
				ss << "<tr><td><a href=\"/" << m_szPath << (m_szPath.length()>0?"/":"") << ((strcmp(fd.cFileName, "..")==0)?"..":UriEncode(fd.cFileName)) << ((fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)?"/":"") << "\">" << fd.cFileName << "</a></tr></td>";
			}
			
			if(!FindNextFile(hFind, &fd))
				break;
		}

		ss << "</table>\r\n"
			
			"<hr />\r\n"
			"</body>\r\n"
			"</html>\r\n";

		FindClose(hFind);

		SendUtf8Body(ss.str());
		
		return true;
	}

	void CHttpRequest::SetLastModified(const string & szDate)
	{
		if(szDate.length()>0)
			SetParam("Last-Modified", szDate);
	}

	string CHttpRequest::GetFileGmtTime(HANDLE hFile)
	{
		char szDT[128];
		SYSTEMTIME st;
		FILETIME LastWriteTime;
		GetFileTime(hFile, NULL, NULL, &LastWriteTime);
		FileTimeToSystemTime(&LastWriteTime, &st);
		struct tm gm = { st.wSecond, st.wMinute, st.wHour, st.wDay, st.wMonth-1, st.wYear-1900, st.wDayOfWeek, 0, 0 };
		strftime(szDT, 128,	"%a, %d %b %Y %H:%M:%S GMT", &gm);
		return szDT;
	}

	bool CHttpRequest::SendFile(const string & szPath, bool bDownload)
	{
		// Dir
		WIN32_FIND_DATA fd;

		string str = (szPath.length()>0 && szPath[szPath.length()-1]=='\\')?szPath.substr(0, szPath.length()-1):szPath;

		if(!FileExist(str, &fd))
		{
			SendError("404 Not Found");
			return false;			
		}

		if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			return SendDir(szPath);
		}

		// Open File
		HANDLE hFile;
		hFile = CreateFile(szPath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, 
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if(hFile==INVALID_HANDLE_VALUE)
		{
			SendError("404 Not Found");
			return false;
		}

		DWORD dwSize = GetFileSize(hFile, NULL);

		if(dwSize==0xFFFFFFFF)
		{
			CloseHandle(hFile);
			SendError("500 Internal Server Error");
			return false;
		}

		SetContentLength(dwSize);

		// last modified
		SetLastModified(GetFileGmtTime(hFile));

		int p = szPath.rfind('.');

		string szFileExt = "";
		if(p!=string::npos && p!=szPath.length()-1)
		{
			szFileExt = szPath.substr(p+1, szPath.length()-p);
		}
		SetContentType(GetContentType(szFileExt));
		
		// Content-Disposition: attachment(inline); filename="example.pdf" 

		if(bDownload)
		{
			ostringstream ss;
			
			ss << (bDownload?"attachment":"inline");
			ss << "; filename=\"";
			ss << GetFileName(szPath.c_str());
			ss << "\"";
			SetParam("Content-Disposition", ss.str().c_str());
		}

		if(!SendHeader())
			return false;

		// TransmitFile(m_sClient, hFile, dwSize, 0, NULL, NULL, 0);

		SendFileContent(hFile);

		CloseHandle(hFile);

		return true;
	}

	void CHttpRequest::ParseRealPath()
	{
		if(m_szPath.length()>0 && m_szPath[m_szPath.length()-1]=='/')
			m_szPath = m_szPath.substr(0, m_szPath.length()-1);

		m_szRealPath = m_pHttpSvr->m_szRoot;
		if(m_szPath.length()>0)
			m_szRealPath += "\\"+m_szPath;

		StringReplace(m_szRealPath, "/", "\\");

		if(m_szRealPath.find("..\\")!=string::npos || m_szRealPath.find("..")==m_szRealPath.length()-2)
		{
			m_szRealPath = m_pHttpSvr->m_szRoot;
			m_szPath = "/";
			m_szScriptName = m_pHttpSvr->m_szDefaultFile;
			return;
		}

		WIN32_FIND_DATA fd;
		bool bFileExist = FileExist(m_szRealPath, &fd);

		if(m_pHttpSvr->ContentHandleExist(m_szPath))
		{
			m_szScriptName = m_szPath;
		}
		else if(bFileExist && (fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) && m_pHttpSvr->ContentHandleExist(m_pHttpSvr->m_szDefaultFile))
		{
			m_szScriptName = m_pHttpSvr->m_szDefaultFile;
			m_szRealPath += "\\"+m_pHttpSvr->m_szDefaultFile;
			m_szPath += "/"+m_pHttpSvr->m_szDefaultFile;
		}
		else if(FileExist(m_szRealPath+"\\"+m_pHttpSvr->m_szDefaultFile))
		{
			m_szScriptName = m_pHttpSvr->m_szDefaultFile;
			m_szRealPath += "\\"+m_pHttpSvr->m_szDefaultFile;
			m_szPath += "/"+m_pHttpSvr->m_szDefaultFile;
		}
		else
		{
			int p = m_szRealPath.rfind("\\");
			if(p!=string::npos && p!=m_szRealPath.length()-1)
			{
				m_szScriptName = m_szRealPath.substr(p+1);
			}
		}
	}


	string CHttpRequest::GetContentType(const string & szFileExt)
	{	
		string str = m_pHttpSvr->m_mapMimeTypes[szFileExt];

		if(szFileExt.length()<=0) str = "application/octet-stream";

		return str;
	}

	bool CHttpRequest::IsPostFormData()
	{
		return m_bPostFormData;
	}

	bool CHttpRequest::IsPost()
	{
		return m_nMethod==HTTP_QUERY_METHOD_POST;
	}

	void CHttpRequest::FindForm(const string & szBegin, vector<string> * pvtValue, vector<string> * pvtFormName)
	{	
		if(pvtValue!=NULL) pvtValue->clear();
		if(pvtFormName!=NULL) pvtFormName->clear();

		map<string, string>::iterator it;
		for(it=m_mapForm.begin(); it!=m_mapForm.end(); it++)
		{
			if(it->first.find(szBegin, 0)!=string::npos && it->second.length()>0)
			{
				if(pvtValue!=NULL) pvtValue->push_back(it->second);
				if(pvtFormName!=NULL) pvtFormName->push_back(it->first);
			}
		}
	}

	string CHttpRequest::GetIp()
	{
		return inet_ntoa(m_pHttpService->m_AddrClient.sin_addr);
	}

	bool CHttpRequest::FormExist(const string & szFormName)
	{
		return m_mapForm.find(szFormName)!=m_mapForm.end();
	}

	bool CHttpRequest::ParamExist(const string & szParamName)
	{
		return m_mapParam.find(szParamName)!=m_mapParam.end();
	}

	void CHttpRequest::SetCacheControl(int nCacheTimeSeconds)
	{
		ostringstream ss;
		if(nCacheTimeSeconds>0)
			ss << "max-age=" << nCacheTimeSeconds;//<< ", no-cache=\"Set-Cookie\"";
		else
			ss << "no-cache, must-revalidate";
		SetParam("Cache-Control", ss.str()); 
	}

	bool CHttpRequest::FormFileExist(const string & szFileName)
	{
		return m_mapFilePath.find(szFileName)!=m_mapFilePath.end();
	}

	void CHttpRequest::FormToXml(ostringstream & ssXml)
	{
		map<string, string>::iterator it;

		ssXml << "<Form>";

		for(it=m_mapForm.begin(); it!=m_mapForm.end(); it++)
		{
			string szName, szValue;
			szName = it->first;
			szValue = it->second;

			ssXml << "<" << ZvLibs::EscapeHtmlCharacter(szName) << ">";
			ssXml << ZvLibs::EscapeHtmlCharacter(szValue);
			ssXml << "</" << ZvLibs::EscapeHtmlCharacter(szName) << ">";
		}

		ssXml << "</Form>\r\n";
	}
}