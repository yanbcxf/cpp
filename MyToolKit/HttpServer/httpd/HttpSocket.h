#pragma once

namespace ZvLibs
{
	class CHttpSocket
	{
	public:

		CHttpSocket(bool m_bOwner = true, SOCKET s = INVALID_SOCKET);
		virtual ~CHttpSocket();
		
		SOCKET m_Socket;
		bool m_bOwner;
		long m_nTmoutRecv;
		long m_nTmoutSend;
		long m_nTmoutConn;

		bool Connect(const string & szAddress, int nPort);
		bool Send(const char * szData, int nLen = -1);
		bool Send(const string & szData);
		
		bool Recv(string & szStr, int nLen);
		bool Recv(char * buf, int nLen);
		bool Recv(char * buf, int nLen, int * pnRecved);

		bool RecvRequest(char * buf, int * nRecved);
		bool RecvLine(string & szLine, int * pLen = NULL);

		void Close();
		bool SendFileContent(HANDLE hFile);

	private:

		bool InitSocket();

	};
}