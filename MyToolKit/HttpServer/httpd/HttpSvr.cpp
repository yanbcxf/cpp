#include "stdafx.h"
#include "HttpSvr.h"
#include "HttpService.h"
#include "HttpHandle.h"
#include "../utils/ZvUtils.h"

namespace ZvLibs
{
	CHttpSvr::CHttpSvr(const tstring & szRootPath, int nPort)
	{
		WSADATA WsaData;
		WSAStartup(MAKEWORD(2,2), &WsaData);
		
		m_szDefaultFile = "Index.xml";

		m_nPort = DEFAULT_HTTP_PORT;

		m_sListen = INVALID_SOCKET;

		InitMimeTypes();

		m_szRoot = szRootPath;

		SetRootPath(m_szRoot);
		Start(nPort);
	}

	CHttpSvr::~CHttpSvr()
	{
		Stop();
	}

	bool CHttpSvr::SetPort(int nPort)
	{
		SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if(s==INVALID_SOCKET)
			return false;

		BOOL b = TRUE;
		setsockopt(s, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (LPCSTR)&b, 4);		

		sockaddr_in saLocal;
		saLocal.sin_family = AF_INET;
		saLocal.sin_port = htons(nPort);
		saLocal.sin_addr.s_addr = INADDR_ANY;

		if(bind(s, (struct sockaddr *)&saLocal, sizeof(sockaddr_in))==SOCKET_ERROR)
			return false;

		if(listen(s, SOMAXCONN)==SOCKET_ERROR)
			return false;

		Stop();

		m_sListen = s;
		m_nPort = nPort;

		_beginthread(ThreadAccept, 0, this);

		return true;
	}

	bool CHttpSvr::Start(int nPort)
	{
		if(m_sListen!=INVALID_SOCKET && nPort==m_nPort) return true;

		if(nPort<0) nPort = m_nPort;

		Stop();

		m_sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		
		if(m_sListen==INVALID_SOCKET)
			return false;

		sockaddr_in saLocal;
		saLocal.sin_family = AF_INET;
		saLocal.sin_port = htons(nPort);
		saLocal.sin_addr.s_addr = INADDR_ANY;

		if(bind(m_sListen, (struct sockaddr *)&saLocal, sizeof(sockaddr_in))==SOCKET_ERROR)
			return false;

		if(listen(m_sListen, SOMAXCONN)==SOCKET_ERROR)
			return false;
		
		_beginthread(ThreadAccept, 0, this);

		m_nPort = nPort;

		return true;
	}

	void CHttpSvr::Stop()
	{
		shutdown(m_sListen, SD_BOTH);
		closesocket(m_sListen);
		m_sListen = INVALID_SOCKET;
	}

	void CHttpSvr::SetDefaultFile(const string szDefaultFile)
	{
		m_szDefaultFile = szDefaultFile;
	}

	void CHttpSvr::ThreadAccept(LPVOID pParam)
	{
		CHttpSvr * pThis = (CHttpSvr*)pParam;

		SOCKET sAccept;
		struct sockaddr_in addr;
		int addrlen = sizeof(addr);

		while((sAccept = accept(pThis->m_sListen, (struct sockaddr*)&addr, &addrlen))!=INVALID_SOCKET)
		{
			new CHttpService(pThis, sAccept, addr);
			addrlen = sizeof(addr);
		}
	}

	void CHttpSvr::RemoveHandle(CHttpHandle & HttpHandle)
	{
		switch(HttpHandle.m_HandleType)
		{
		case HTTP_HANDLE_CONTENT:
			{
				map<string, CHttpHandle*>::iterator it = m_mapHandleContent.find(HttpHandle.m_szPath);
				if(it!=m_mapHandleContent.end())
					m_mapHandleContent.erase(it);
			}
			
			break;
		case HTTP_HANDLE_AFTER:
			{
				for(int i=(int)m_vtHandleFilterAfter.size()-1; i>=0; i--)
				{
					if(m_vtHandleFilterAfter[i]==&HttpHandle)
					{
						m_vtHandleFilterAfter.erase(m_vtHandleFilterAfter.begin()+i);
					}
				}
			}
			break;
		case HTTP_HANDLE_BEFORE:
			{
				for(int i=(int)m_vtHandleFilterBefore.size()-1; i>=0; i--)
				{
					if(m_vtHandleFilterBefore[i]==&HttpHandle)
					{
						m_vtHandleFilterBefore.erase(m_vtHandleFilterBefore.begin()+i);
					}
				}
			}
			break;
		default:
			break;
		}
	}

	void CHttpSvr::AddHandle(CHttpHandle & HttpHandle)
	{
		switch(HttpHandle.m_HandleType)
		{
		case HTTP_HANDLE_CONTENT:
			m_mapHandleContent[HttpHandle.m_szPath] = &HttpHandle;
			break;
		case HTTP_HANDLE_AFTER:
			m_vtHandleFilterAfter.push_back(&HttpHandle);
			break;
		case HTTP_HANDLE_BEFORE:
			m_vtHandleFilterBefore.push_back(&HttpHandle);
			break;
		default:
			break;
		}
	}

	void CHttpSvr::SetRootPath(const string & szRoot)
	{
		m_szRoot = szRoot;
		ToWinPath(m_szRoot);
	}

	string CHttpSvr::GetRootPath()
	{
		return m_szRoot;
	}

	void CHttpSvr::InitMimeTypes()
	{
		m_mapMimeTypes["doc"]	= "application/msword";
		m_mapMimeTypes["bin"]	= "application/octet-stream";
		m_mapMimeTypes["dll"]	= "application/octet-stream";
		m_mapMimeTypes["exe"]	= "application/octet-stream";
		m_mapMimeTypes["pdf"]	= "application/pdf";
		m_mapMimeTypes["p7c"]	= "application/pkcs7-mime";
		m_mapMimeTypes["ai"]	= "application/postscript";
		m_mapMimeTypes["eps"]	= "application/postscript";
		m_mapMimeTypes["ps"]	= "application/postscript";
		m_mapMimeTypes["rtf"]	= "application/rtf";
		m_mapMimeTypes["fdf"]	= "application/vnd.fdf";
		m_mapMimeTypes["arj"]	= "application/x-arj";
		m_mapMimeTypes["gz"]	= "application/x-gzip";
		m_mapMimeTypes["class"]	= "application/x-java-class";
		m_mapMimeTypes["js"]	= "application/javascript";
		m_mapMimeTypes["lzh"]	= "application/x-lzh";
		m_mapMimeTypes["lnk"]	= "application/x-ms-shortcut";
		m_mapMimeTypes["tar"]	= "application/x-tar";
		m_mapMimeTypes["hlp"]	= "application/x-winhelp";
		m_mapMimeTypes["cert"]  = "application/x-x509-ca-cert";
		m_mapMimeTypes["zip"]	= "application/zip";
		m_mapMimeTypes["cab"]	= "application/x-compressed";
		m_mapMimeTypes["arj"]	= "application/x-compressed";
		m_mapMimeTypes["aif"]	= "audio/aiff";
		m_mapMimeTypes["aifc"]  = "audio/aiff";
		m_mapMimeTypes["aiff"]  = "audio/aiff";
		m_mapMimeTypes["au"]	= "audio/basic";
		m_mapMimeTypes["snd"]	= "audio/basic";
		m_mapMimeTypes["mid"]	= "audio/midi";
		m_mapMimeTypes["rmi"]	= "audio/midi";
		m_mapMimeTypes["mp3"]	= "audio/mpeg";
		m_mapMimeTypes["vox"]	= "audio/voxware";
		m_mapMimeTypes["wav"]	= "audio/x-wav";
		m_mapMimeTypes["ra"]	= "audio/x-pn-realaudio";
		m_mapMimeTypes["ram"]	= "audio/x-pn-realaudio";
		m_mapMimeTypes["bmp"]	= "image/bmp";
		m_mapMimeTypes["gif"]	= "image/gif";
		m_mapMimeTypes["jpeg"]	= "image/jpeg";
		m_mapMimeTypes["jpg"]	= "image/jpeg";
		m_mapMimeTypes["png"]	= "image/png";
		m_mapMimeTypes["tif"]	= "image/tiff";
		m_mapMimeTypes["tiff"]	= "image/tiff";
		m_mapMimeTypes["xbm"]	= "image/xbm";
		m_mapMimeTypes["wrl"]	= "model/vrml";
		m_mapMimeTypes["htm"]	= "text/html";
		m_mapMimeTypes["html"]	= "text/html";
		m_mapMimeTypes["xml"]	= "application/xml";
		m_mapMimeTypes["xsl"]	= "application/xml";
		m_mapMimeTypes["css"]	= "text/css";
		m_mapMimeTypes["c"]		= "text/plain";
		m_mapMimeTypes["cpp"]	= "text/plain";
		m_mapMimeTypes["def"]	= "text/plain";
		m_mapMimeTypes["h"]		= "text/plain";
		m_mapMimeTypes["txt"]	= "text/plain";
		m_mapMimeTypes["ini"]	= "text/plain";
		m_mapMimeTypes["inf"]	= "text/plain";
		m_mapMimeTypes["rtx"]	= "text/richtext";
		m_mapMimeTypes["rtf"]	= "text/richtext";
		m_mapMimeTypes["java"]	= "text/x-java-source";
		m_mapMimeTypes["css"]	= "text/css";
		m_mapMimeTypes["mpeg"]	= "video/mpeg";
		m_mapMimeTypes["mpg"]	= "video/mpeg";
		m_mapMimeTypes["mpe"]	= "video/mpeg";
		m_mapMimeTypes["avi"]	= "video/msvideo";
		m_mapMimeTypes["mov"]	= "video/quicktime";
		m_mapMimeTypes["qt"]	= "video/quicktime";
		m_mapMimeTypes["shtml"]	= "wwwserver/html-ssi";
		m_mapMimeTypes["asa"]	= "wwwserver/isapi";
		m_mapMimeTypes["asp"]	= "wwwserver/isapi";
		m_mapMimeTypes["cfm"]	= "wwwserver/isapi";
		m_mapMimeTypes["dbm"]	= "wwwserver/isapi";
		m_mapMimeTypes["isa"]	= "wwwserver/isapi";
		m_mapMimeTypes["plx"]	= "wwwserver/isapi";
		m_mapMimeTypes["url"]	= "wwwserver/isapi";
		m_mapMimeTypes["cgi"]	= "wwwserver/isapi";
		m_mapMimeTypes["php"]	= "wwwserver/isapi";
		m_mapMimeTypes["wcgi"]	= "wwwserver/isapi";
	}

	string CHttpSvr::GetIp()
	{
		TCHAR szName[MAX_PATH] = {0};
		if(gethostname(szName, sizeof(szName))==0)
		{
			PHOSTENT HostInfo = gethostbyname(szName);
			return inet_ntoa(*(struct in_addr *)HostInfo->h_addr_list[0]);
		}
		else 
			return "127.0.0.1";
	}

	bool CHttpSvr::ContentHandleExist(const string & szPath)
	{
		return m_mapHandleContent.find(szPath)!=m_mapHandleContent.end();
	}

}