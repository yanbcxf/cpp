#include "stdafx.h"
#include "HttpClient.h"
#include "../utils/ZvUtils.h"
#include "UriCodec.h"
#include "HttpDefine.h"

namespace ZvLibs
{

	CHttpClient::CHttpClient()
	:CHttpSocket(true)
	{
		m_dwLenFiles = 0;
	}

	CHttpClient::~CHttpClient()
	{
	}

	void CHttpClient::SetKeepAlive()
	{
		AddHeader("Connection", "keep-alive");
	}

	bool CHttpClient::IsPost()
	{
		return m_mapParamPost.size()>0 || m_mapFiles.size()>0;
	}

	bool CHttpClient::Connect(const string & szAddress, int nPort)
	{
		AddHeader("Host", szAddress);
		return CHttpSocket::Connect(szAddress, nPort);
	}

	void CHttpClient::Close()
	{
		Clearup();
		CHttpSocket::Close();
	}

	string CHttpClient::GetPostLength()
	{	
		int nLen = 0;
		map<string, string>::iterator it;

		if(!m_bUploadFile)
		{
			for(it=m_mapParamPost.begin(); it!=m_mapParamPost.end(); it++)
			{
				nLen += it->first.length()+it->second.length()+1;
			}
			if(m_mapParamPost.size()>0)
			{
				nLen += m_mapParamPost.size()-1;
			}
		}
		else
		{
			for(it=m_mapParamPost.begin(); it!=m_mapParamPost.end(); it++)
			{
				nLen += 42+41+it->first.length()+it->second.length()+4;
			}

			for(it=m_mapFiles.begin(); it!=m_mapFiles.end(); it++)
			{
				nLen += 42+54+it->first.length()+it->second.length()+4+40;
			}
			if(nLen>0)
				nLen+=m_dwLenFiles+42+2;
		}

		ostringstream ss;
		ss << nLen;
		return ss.str();
	}

	bool CHttpClient::HeaderExists(const string & szHeader)
	{
		return (m_mapHeaderOut.find(szHeader)!=m_mapHeaderOut.end());
	}

	void CHttpClient::AddHeader( const string &  szName, const string &  szValue )
	{
		m_mapHeaderOut[szName] = szValue;
	}

	void CHttpClient::AddParamUrl( const string &  szName, const string &  szValue )
	{
		m_mapParamUrl[szName] = szValue;	
	}

	void CHttpClient::AddParamPost( const string &  szName, const string &  szValue )
	{
		m_mapParamPost[szName] = szValue;	
	}

	bool CHttpClient::AddFile(const string &  szName, const string &  szFilePath)
	{
		WIN32_FIND_DATA fd;

		if(!FileExist(szFilePath, &fd))
		{
			return false;
		}
		m_dwLenFiles += fd.nFileSizeLow;
		m_mapFiles[szName] = szFilePath;

		return true;
	}

	bool CHttpClient::SendData()
	{
		ostringstream ss;
		string str;
		map<string, string>::iterator it;

		if(!m_bUploadFile)
		{
			for(it=m_mapParamPost.begin(); it!=m_mapParamPost.end(); it++)
			{
				ss << it->first << "=" << it->second;
			}

			if(!Send(ss.str())) return false;
		}
		else
		{
			for(it=m_mapParamPost.begin(); it!=m_mapParamPost.end(); it++)
			{
				ss.str("");
				ss << m_szBoundary << CRLF;
				ss << "Content-Disposition: form-data; name=\"" << it->first << "\"" << CRLFCRLF;
				ss << it->second << CRLF;

				if(!Send(ss.str())) return false;
			}

			for(it=m_mapFiles.begin(); it!=m_mapFiles.end(); it++)
			{
				ss.str("");
				ss << m_szBoundary << CRLF;
				ss << "Content-Disposition: form-data; name=\"" << it->first << "\"; filename=\"" << it->second << "\"" << CRLF;
				ss << "Content-Type: application/octet-stream" << CRLFCRLF;
				if(!Send(ss.str())) return false;

				HANDLE hFile;

				hFile = CreateFile(it->second.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, 
					OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

				if(hFile==INVALID_HANDLE_VALUE)
				{
					return false;
				}

				DWORD dwSize = GetFileSize(hFile, NULL);

				if(dwSize==0xFFFFFFFF)
				{
					CloseHandle(hFile);
					return false;
				}

				SendFileContent(hFile);

				CloseHandle(hFile);

				Send(CRLF);
			}

			Send(m_szBoundary+"--\r\n");
		}	

		return true;
	}

	bool CHttpClient::SendHeader()
	{
		ostringstream ss;
		string str;
		map<string, string>::iterator it;

		m_bPost = IsPost();
		m_bUploadFile = m_mapFiles.size()>0;

		// request line
		ss << (m_bPost?"POST":"GET") << " " << m_szUrl;

		if(m_mapParamUrl.size()>0)
		{
			if(m_szUrl.find("?")==string::npos)
				ss << "?";
			else
				ss << "&";

			for(it=m_mapParamUrl.begin(); it!=m_mapParamUrl.end(); it++)
			{
				ss << UriEncode(it->first) << "=" << UriEncode(it->second) << "&";
			}
		}
		ss << " HTTP/1.1";

		// header
		if(m_bPost)
		{
			AddHeader("Content-Length", GetPostLength());
			m_szBoundary = "-------------------------"+GetRandomString(13);

			if(m_bUploadFile)
			{
				str = "multipart/form-data; boundary="+m_szBoundary;
				m_szBoundary.insert(0, "--");
				AddHeader("Content-Type", str);	
			}
			else if(!HeaderExists("Content-Type"))
			{
				AddHeader("Content-Type", "application/x-www-form-urlencoded");
			}
		}

		for(it=m_mapHeaderOut.begin(); it!=m_mapHeaderOut.end(); it++)
		{
			ss << CRLF;
			ss << it->first << ": " << it->second;
		}
		ss << CRLFCRLF;

		return Send(ss.str());
	}

	void CHttpClient::DeleteTmpData()
	{
		DeleteFile(m_szFilePath.c_str());
		m_mapHeaderIn.clear();
		m_szFilePath = "";
		m_szData = "";
	}

	void CHttpClient::Clearup()
	{
		DeleteTmpData();

		m_mapHeaderOut.clear();
		m_mapParamUrl.clear();
		m_mapParamPost.clear();
		m_szFileName = "";
		m_mapFiles.clear();
		m_dwLenFiles = 0;
	}

	bool CHttpClient::RecvHeader()
	{
		// state line
		if(!RecvLine(m_szStateLine))
			return false;
		
		int p, p1;

		p = m_szStateLine.find(" ");
		if(p!=string::npos)
		{
			if(p<(int)m_szStateLine.length()-3)
				m_nStateCode = atoi(m_szStateLine.substr(p+1, 3).c_str());
			else
				m_nStateCode = -1;
		}

		if(m_bSaveToFile)
		{
			if(m_nStateCode!=HTTP_STATUS_OK)
				m_bSaveToFile = false;
		}

		string szRequest;

		// headers
		while(1)
		{
			if(!RecvLine(szRequest))
				return false;

			if(szRequest.length()<=0) break;

			p = szRequest.find(": ", 0);
			if(p>0 && p!=string::npos && p!=szRequest.length()-2)
			{
				string szParam, szValue;
				szParam = szRequest.substr(0, p);
				szValue = szRequest.substr(p+2, -1);
				m_mapHeaderIn[szParam] = szValue;
			}
		}

		// file name
		p = m_mapHeaderIn["Content-Disposition"].find("filename=\"");
		if(p!=string::npos)
		{
			p1 = m_mapHeaderIn["Content-Disposition"].find("\"", p+10);
			if(p1!=string::npos)
				m_szFileName = m_mapHeaderIn["Content-Disposition"].substr(p+10, p1-p-10);
		}

		if(GetStateCode()==HTTP_STATUS_CONTINUE)
		{
			return RecvHeader();
		}

		return true;
	}

	bool CHttpClient::RecvData()
	{
		int  nLength = atoi(m_mapHeaderIn["Content-Length"].c_str());
		bool bChunked = m_mapHeaderIn["Transfer-Encoding"]=="chunked";

		HANDLE hFile = INVALID_HANDLE_VALUE;
		string str;
		int nLen,i;

		if(m_bSaveToFile)
		{
			MakeDirectory("./temp");
			m_szFilePath = "./temp/"+m_szFileName;
			ToWinPath(m_szFilePath);
			hFile = CreateFile(m_szFilePath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, NULL, NULL);
			if(hFile == INVALID_HANDLE_VALUE)
			{
				m_szFilePath = "./temp/"+GetRandomString(32);
				ToWinPath(m_szFilePath);
				hFile = CreateFile(m_szFilePath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, NULL, NULL);
			}

			if(hFile==INVALID_HANDLE_VALUE)
			{
				return false;
			}
		}

		if(bChunked)
		{
			while(1)
			{		
				if(!RecvLine(str))
				{
					CloseHandle(hFile);
					return false;
				}
				
				nLen = strtol(str.c_str(), NULL, 16);
				if(nLen==0) 
				{
					RecvLine(str);
					break;
				}
				
				//---------------------------------
				int nNumRecv = nLen/LEN_RECV_ONCE + ((nLen%LEN_RECV_ONCE==0)?0:1);

				for(i=0; i<nNumRecv; i++)
				{
					int nRecvOnce = (i<nNumRecv-1) ? LEN_RECV_ONCE:((nLen%LEN_RECV_ONCE==0)?LEN_RECV_ONCE:nLen%LEN_RECV_ONCE);

					char buf[LEN_RECV_ONCE+1] = {0};

					if(!Recv(buf, nRecvOnce))
					{
						CloseHandle(hFile);
						return false;
					}

					if(m_bSaveToFile)
					{
						DWORD dwWritten;
						if(!WriteFile(hFile, buf, nRecvOnce, &dwWritten, NULL))
						{
							CloseHandle(hFile);
							return false;
						}
					}
					else
					{	
						if(m_szData.length()<LEN_MAX_RECV_DATA)
							m_szData += buf;
					}		
				}
				//---------------------------------
				RecvLine(str);
			}
		}	

		else
		{
			nLen = nLength;
			
			if(nLen>0)
			{
				//---------------------------------
				int nNumRecv = nLen/LEN_RECV_ONCE + (nLen%LEN_RECV_ONCE==0?0:1);

				for(i=0; i<nNumRecv; i++)
				{
					int nRecvOnce = (i<nNumRecv-1) ? LEN_RECV_ONCE:((nLen%LEN_RECV_ONCE==0)?LEN_RECV_ONCE:nLen%LEN_RECV_ONCE);

					char buf[LEN_RECV_ONCE+1] = {0};

					if(!Recv(buf, nRecvOnce))
					{
						CloseHandle(hFile);
						return false;
					}

					if(m_bSaveToFile)
					{
						DWORD dwWritten;
						if(!WriteFile(hFile, buf, nRecvOnce, &dwWritten, NULL))
						{
							CloseHandle(hFile);
							return false;
						}
					}
					else
					{	
						if(m_szData.length()<LEN_MAX_RECV_DATA)
							m_szData += buf;
					}		
				}
				//---------------------------------
			}
			else
			{
				//---------------------------------

				while(1)
				{
					char buf[LEN_RECV_ONCE+1] = {0};
					int nRecvOnce = LEN_RECV_ONCE;
					int nRecvd = 0;
					if(!Recv(buf, nRecvOnce, &nRecvd))
					{
						CloseHandle(hFile);
						return nRecvd==0;
					}

					if(m_bSaveToFile)
					{
						DWORD dwWritten;
						if(!WriteFile(hFile, buf, nRecvOnce, &dwWritten, NULL))
						{
							CloseHandle(hFile);
							return false;
						}
					}
					else
					{	
						if(m_szData.length()<LEN_MAX_RECV_DATA)
							m_szData += buf;
					}		
				}
				//---------------------------------
			}
		}

		CloseHandle(hFile);

		return true;
	}

	void CHttpClient::SetPost(bool bPost)
	{
		m_bPost = bPost;
	}

	bool CHttpClient::RequestUrl(const string & szUrl, bool bSaveToFile)
	{
		string szAddress;
		int nPort = 80;
		
		int pBegin = szUrl.find("http://");

		if(pBegin==0) pBegin=7;
		else if(pBegin==string::npos) pBegin=0;

		int pEnd = szUrl.find("/", pBegin);

		if(pEnd==string::npos)
		{
			pEnd = szUrl.length();
		}

		int p = szUrl.find(":", pBegin);
		if(p!=string::npos && p<pEnd && p>pBegin)
		{
			nPort = atoi(szUrl.substr(p+1, pEnd).c_str());
			szAddress = szUrl.substr(pBegin, p-pBegin);
		}
		else
		{
			szAddress = szUrl.substr(pBegin, pEnd-pBegin);
		}

		string szPath = szUrl.substr(pEnd, string::npos);
		
		return Connect(szAddress, nPort) && Request(szPath, bSaveToFile);
	}

	bool CHttpClient::Request(const string & szUrl, bool bSaveToFile)
	{	
		m_szUrl = szUrl;

		if(m_szUrl.length()<=0 || (m_szUrl[0]!='/' && m_szUrl[0]!='\\'))
			m_szUrl.insert(0, "/");

		m_bSaveToFile = bSaveToFile;

		DeleteTmpData();

		bool b = (SendHeader() && SendData() && RecvHeader() && RecvData());
		
		return b;
	}

	int CHttpClient::GetStateCode()
	{
		return m_nStateCode;
	}

	string CHttpClient::GetStateLine()
	{
		return m_szStateLine;
	}

	void CHttpClient::SetAuthBasic(const string &  szUser, const string &  szPass)
	{
		string szAuth = szUser + ":" + szPass;
		string szAuthEnc = string("Basic ") + Base64Encode(szAuth);

		AddHeader("Authorization", szAuthEnc);
	}

}