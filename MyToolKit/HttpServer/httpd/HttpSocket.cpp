
#include "stdafx.h"
#include "HttpSocket.h"
#include "HttpDefine.h"

#pragma comment(lib, "Ws2_32")
#pragma comment(lib, "mswsock")

namespace ZvLibs
{
	CHttpSocket::CHttpSocket(bool bOwner, SOCKET s)
	{
		m_Socket = s;
		m_nTmoutConn = 10;
		m_nTmoutRecv = 10;
		m_nTmoutSend = 10;
		
		m_bOwner = bOwner;

		if(bOwner)
			InitSocket();
	}

	CHttpSocket::~CHttpSocket()
	{
		if(m_bOwner)
		{
			Close();
		}
	}

	bool CHttpSocket::Connect(const string & szAddress, int nPort)
	{
		Close();

		if(!InitSocket())
			return false;

		struct hostent * stHost;
		sockaddr_in addr;
	
		if(inet_addr(szAddress.c_str()) == INADDR_NONE) 
		{
			stHost = gethostbyname(szAddress.c_str());
			if(stHost==NULL)
			{
				return false;
			}
			addr.sin_addr = *((struct in_addr *)stHost->h_addr);
		} 
		else
		{
			addr.sin_addr.s_addr=inet_addr(szAddress.c_str());
		}

		addr.sin_family=AF_INET;
		addr.sin_port=htons(nPort);
		
		if(connect(m_Socket, (LPSOCKADDR)&addr,sizeof(addr))==0)
			return true;

		if(WSAGetLastError()!=WSAEWOULDBLOCK) 
		{
			return false;
		}

		FD_SET fd = {1, m_Socket};
		TIMEVAL tv = {m_nTmoutConn,  0};  

		int nSel = select(0, 0, &fd, 0, &tv);

		if(nSel<= 0)
		{
			if(nSel==0)
			{
				WSASetLastError(WSAETIMEDOUT);
			}
			
			return false;
		}

		return true;
	}

	bool CHttpSocket::Recv(char * buf, int nLen, int * pnRecved)
	{
		int ret = 0;
		int r   = 0;

		FD_SET  fd = {1, m_Socket};
		TIMEVAL tv = {m_nTmoutRecv, 0};

		int nSel = select(0, &fd, NULL, NULL, &tv);

		if(nSel > 0)   
		{  
			r = recv(m_Socket, buf, nLen, 0);

			if(r<0) return false;
	
			else if(r==0)
			{
				if(pnRecved!=NULL) *pnRecved = 0;
				WSASetLastError(WSAECONNRESET);
				return false;
			}

			if(pnRecved!=NULL)
				*pnRecved = r;

			return true;
		}

		else if(nSel == 0)   
		{  
			WSASetLastError(WSAETIMEDOUT);
		}

		return false;
	}

	bool CHttpSocket::Recv(char * buf, int nLen)
	{
		int ret = 0;
		int r = 0;

		FD_SET fd = {1, m_Socket};
		TIMEVAL tv = {m_nTmoutRecv,  0};  

		while(ret<nLen)
		{
			int nSel = select(0, &fd, NULL, NULL, &tv);
			if( nSel > 0)   
			{  
				r = recv(m_Socket, buf+ret, nLen-ret, 0);
				if(r<0) return false;

				else if(r==0)
				{
					WSASetLastError(WSAECONNRESET);
					return false;
				}
				ret += r;
				continue;
			}

			if(nSel == 0)   
			{  
				WSASetLastError(WSAETIMEDOUT);
			}

			return false;
		}

		return true;
	}

	bool CHttpSocket::Recv(string & szStr, int nLen)
	{
		szStr.resize(nLen);

		return Recv(&szStr[0], nLen);
	}

	bool CHttpSocket::Send(const char * szData, int nLen)
	{
		int ret = 0;
		int r = 0;
		if(nLen<=0)
			nLen = strlen(szData);

		FD_SET fd = {1, m_Socket};
		TIMEVAL tv = {m_nTmoutSend, 0};  

		while(ret < nLen)
		{
			int nSel = select(0, NULL, &fd, NULL, &tv);
			if(nSel>0)
			{
				r = send(m_Socket, szData+ret, nLen-ret, 0);
				if(r<0)
				{
					return false;
				}

				ret += r;
				continue;
			}

			if(nSel == 0)   
			{  
				WSASetLastError(WSAETIMEDOUT);
			}

			return false;
		}

		return true;
	}

	bool CHttpSocket::RecvLine(string & szLine, int * pLen)
	{
		char c;
		int nLen = 0;
		int nExtra = 0;
		szLine = "";

		while(Recv(&c, 1))
		{
			switch(c)
			{
			case CR:
				nExtra++;
				break;
			case LF:
				nExtra++;
				if(pLen!=NULL)
				{
					* pLen = nExtra + nLen;
				}
				return true;
			default:
				nLen++;
				szLine += c;
			}

			if(nLen>LEN_MAX_HEADER_LINE)
			{
				return false;
			}
		}

		return false;
	}

	bool CHttpSocket::Send(const string & szData)
	{
		return Send(szData.c_str(), szData.length());
	}

	void CHttpSocket::Close()
	{
		shutdown(m_Socket, SD_BOTH);
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
	}

	bool CHttpSocket::InitSocket()
	{
		if(m_Socket==INVALID_SOCKET)
		{
			m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if(m_Socket == INVALID_SOCKET)
			{
				return false;
			}
		}

		unsigned long ul;
		ul = 1;
		ioctlsocket(m_Socket, FIONBIO, (unsigned long*)&ul);
		return true;
	}

	bool CHttpSocket::SendFileContent(HANDLE hFile)
	{	
		TCHAR buf[4096];
		DWORD dwRead;

		while(ReadFile(hFile, buf, 4096, &dwRead, 0) && dwRead>0 && Send(buf, dwRead));

		return true;
	}
}