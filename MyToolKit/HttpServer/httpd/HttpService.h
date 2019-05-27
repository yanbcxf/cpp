#pragma once

#include "HttpSocket.h"

namespace ZvLibs
{
	class CHttpSvr;
	class CHttpRequest;

	class CHttpService : public CHttpSocket
	{
	public:
		CHttpService(CHttpSvr * pHttpSvr, SOCKET sClient, sockaddr_in & AddrClient);
		virtual ~CHttpService();

		static void ThreadService(LPVOID pParam);
		bool HandleFilterBefore(CHttpRequest & Request);
		bool HandleFilterAfter(CHttpRequest & Request);

		CHttpSvr * m_pHttpSvr;
		sockaddr_in m_AddrClient;
		SOCKET m_Socket;
	};

}