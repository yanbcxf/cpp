
/*
	https://msdn.microsoft.com/en-us/library/windows/desktop/ms742203(v=vs.85).aspx
*/

#include "StdAfx.h"

#include "Medium.h"
#include "UsageEnvironment.h"
#include "BaseClient.h"

#include "mswsock.h"


unsigned BaseClient::responseBufferSize = 102400; // default value; you can reassign this in your application if you need to

BaseClient::BaseClient(UsageEnvironment& env, char const* strIPAddr, int nPort)
:Medium(env)
{

	stringstream ss;
	ss << " 构建 " << ClientClass();
	CString str1(ss.str().c_str());
	envir().AppendLog(str1);

	m_nNumberOfPendlingIO = 0;
	m_Socket = INVALID_SOCKET;

	fServerPortNum = nPort;
	fServerAddress = string(strIPAddr);

	fIsActive = TRUE;

	fResponseBuffer = new unsigned char[responseBufferSize+1];
	resetResponseBuffer();
}

BaseClient::~BaseClient(void)
{

	stringstream ss;
	ss << " ~销毁 " << ClientClass();
	CString str1(ss.str().c_str());
	envir().AppendLog(str1);

	delete[] fResponseBuffer;
}

void BaseClient::Reclaim(void)
{
	Disconnect();
	handleDestoryed();
	Medium::close(this);
}



/*
* Makes An Asyncorn Send. 
*
*/
BOOL BaseClient::ASend( const unsigned char * buf, int len )
{
	if(m_Socket==INVALID_SOCKET)
		return FALSE;
	
	int left = len;
	int begin = 0;
	while(left>0)
	{
		CIOCPBuffer *pOverlapBuff = g_BufferMgr.AllocateBuffer(IOWrite);
		if(pOverlapBuff)
		{
			len = left>MAXIMUMPACKAGESIZE ? MAXIMUMPACKAGESIZE : left;

			if(pOverlapBuff->AddData(buf + begin, len)==FALSE)
			{
				g_BufferMgr.ReleaseBuffer(pOverlapBuff);
				return FALSE;
			}

			BOOL bSuccess = envir().PostQueuedCompletionStatus( this, pOverlapBuff);
			if ( (!bSuccess && GetLastError( ) != ERROR_IO_PENDING))
			{            
				g_BufferMgr.ReleaseBuffer(pOverlapBuff);
				return FALSE;
			}
			
			CString msg;
			msg.Format(_T("BaseClient::ASend , len = %d, Used = %d "), len, pOverlapBuff->GetUsed());
			envir().AppendLog(msg);

			begin += len;
			left -= len;
		
		}
		else
		{
			return FALSE;
		}
	}
	return TRUE;
	
}


//	向服务器建立连接成功
void BaseClient::OnInitialize(CIOCPBuffer *pOverlapBuff)
{
	g_BufferMgr.ReleaseBuffer(pOverlapBuff);

	ARead();

	handleConnected();
}


/*
* Makes a asynchrony Read by posting a IORead message into completion port 
* who invoces a Onread.
*
* The read is not made directly to distribute CPU power fairly between the connections. 
*/

BOOL BaseClient::ARead(CIOCPBuffer *pOverlapBuff)
{

	if(m_Socket!=INVALID_SOCKET )
	{
		if(pOverlapBuff==NULL)
			pOverlapBuff= g_BufferMgr.AllocateBuffer(IORead);


		if(pOverlapBuff==NULL) 
		{			
			envir().AppendLog(_T("AllocateBuffer(IORead) == NULL."));
			return FALSE;
		}

		pOverlapBuff->SetOperation(IORead);
		BOOL bSuccess = envir().PostQueuedCompletionStatus( this, pOverlapBuff);  	
		if ( (!bSuccess && GetLastError( ) != ERROR_IO_PENDING))
		{            
			g_BufferMgr.ReleaseBuffer(pOverlapBuff);
			return FALSE;
		}
		return TRUE;
	}

	return FALSE;
}


void BaseClient::OnWrite(DWORD dwIoSize,CIOCPBuffer *pOverlapBuff)
{

	if(m_Socket!=INVALID_SOCKET && pOverlapBuff)
	{
		CString msg;
		msg.Format(_T(" BaseClient::OnWrite , dwIoSize = %d, Used = %d"), dwIoSize, pOverlapBuff->GetUsed());
		envir().AppendLog(msg);

		pOverlapBuff->SetOperation(IOWriteCompleted);
		pOverlapBuff->SetupWrite();		//	设置 m_wsabuf 做好发送的准备
		ULONG ulFlags = 0;				//	MSG_PARTIAL;
		DWORD dwSendNumBytes = 0;

		int nRetVal = WSASend(m_Socket, 
			pOverlapBuff->GetWSABuffer(),
			1,
			NULL,	//	&dwSendNumBytes, 
			ulFlags,
			&pOverlapBuff->m_ol, 
			NULL);

		if ( nRetVal == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING ) 
		{
			if(WSAGetLastError()!=WSAENOTSOCK)
			{

				// Remove Unnecessary disconnect messages in release mode.. 
				if(WSAGetLastError()!=WSAECONNRESET&&WSAGetLastError()!=WSAECONNABORTED)

				{	
					CString msg;
					msg.Format(_T("Error in OnWrite..: %s"),envir().ErrorCode2Text(WSAGetLastError()));
					envir().AppendLog(msg);
				}
			}
			Disconnect();
			g_BufferMgr.ReleaseBuffer(pOverlapBuff);
		}
		else if(nRetVal == SOCKET_ERROR)
		{
			CString msg;
			msg.Format(_T("发送被推迟 in OnWrite..: %s"),envir().ErrorCode2Text(WSAGetLastError()));
			envir().AppendLog(msg);
		}
		else
		{
			//	异步发送数据成功
			CString msg;
			msg.Format(_T("发送成功 in OnWrite..: 共发送 %d 字节"), dwSendNumBytes);
			envir().AppendLog(msg);
		}
	}
	
}



void BaseClient::OnWriteCompleted( DWORD dwIoSize,CIOCPBuffer *pOverlapBuff)
{
	if (pOverlapBuff!=NULL)
	{
		if(pOverlapBuff->GetUsed()!=dwIoSize)
		{
			CString msg;
			msg.Format(_T("The whole message was not Sent.: %s"),envir().ErrorCode2Text(WSAGetLastError()));
			envir().AppendLog(msg);
		}
		else
		{
			
		}
		//	发送完成后，回收缓存

		CString msg;
		msg.Format(_T(" BaseClient::OnWriteCompleted, 发送完成后，回收缓存，dwIoSize = %d , Used = %d"), dwIoSize, \
			pOverlapBuff->GetUsed());
		envir().AppendLog(msg);

		g_BufferMgr.ReleaseBuffer(pOverlapBuff);
	}
}



void BaseClient::OnRead(CIOCPBuffer *pOverlapBuff)
{
	// issue a read request 
	if(m_Socket!=INVALID_SOCKET && pOverlapBuff)
	{
		pOverlapBuff->SetOperation(IOReadCompleted);
		pOverlapBuff->SetupRead();

		DWORD dwIoSize=0;
		ULONG	ulFlags = MSG_PARTIAL;
		UINT nRetVal = WSARecv(m_Socket, 
			pOverlapBuff->GetWSABuffer(),
			1,
			&dwIoSize, 
			&ulFlags,
			&pOverlapBuff->m_ol, 
			NULL);
		if ( nRetVal == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) 
		{
			if(WSAGetLastError()!=WSAENOTSOCK)
			{

				// Remove Unnecessary disconnect messages in release mode.. 
				if(WSAGetLastError()!=WSAECONNRESET&&WSAGetLastError()!=WSAECONNABORTED)
				{
					CString msg;
					msg.Format(_T("Disconnect in Onread Possible Socket Error: %s"),envir().ErrorCode2Text(WSAGetLastError()));
					envir().AppendLog(msg);
				}
			}
			g_BufferMgr.ReleaseBuffer(pOverlapBuff);
			
			Disconnect();
		}
	}
}



void BaseClient::OnReadCompleted(DWORD dwIoSize,CIOCPBuffer *pOverlapBuff)
{

	if (dwIoSize == 0|| pOverlapBuff==NULL)
	{
		CString msg;
		msg.Format(_T("Disconnection in OnReadCompleted (dwIoSize == 0||pOverlapBuff==NULL) errcode: %s"),
			envir().ErrorCode2Text(WSAGetLastError()));
		envir().AppendLog(msg);
		Disconnect();
		g_BufferMgr.ReleaseBuffer(pOverlapBuff);
		return;
	}

	while(fIsActive)
	{
		if(dwIoSize <= fResponseBufferBytesLeft)
		{
			memmove(&fResponseBuffer[fResponseBytesAlreadySeen], pOverlapBuff->GetBuffer(),dwIoSize);
			fResponseBufferBytesLeft -= dwIoSize;
			fResponseBytesAlreadySeen += dwIoSize;
			handleResponseBytes(dwIoSize);
			
			break;
		}
		else
		{
			int newBytes = fResponseBufferBytesLeft;
			memmove(&fResponseBuffer[fResponseBytesAlreadySeen], pOverlapBuff->GetBuffer(),newBytes);
			fResponseBufferBytesLeft -= newBytes;
			fResponseBytesAlreadySeen += newBytes;
			handleResponseBytes(newBytes);
			dwIoSize = dwIoSize - newBytes;
		}

	}
	if(fIsActive)
	{
		if(ARead(pOverlapBuff))
		{
			//envir().AppendLog("启动 Aread 成功");
		}
		else
			envir().AppendLog(_T("启动 Aread 失败"));
	}
	else
		g_BufferMgr.ReleaseBuffer(pOverlapBuff);

}


int BaseClient::connectToServer_Old(SOCKET	socketNum, int remotePortNum) 
{
	SOCKADDR_IN	SockAddr;
	int			nRet=-1;

	// Clear the SockAddr. 
	memset(&SockAddr,0,sizeof(SockAddr));
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_addr.s_addr = inet_addr(fServerAddress.c_str());
	SockAddr.sin_port = htons(remotePortNum);
	nRet = connect(socketNum,(sockaddr*)&SockAddr, sizeof(SockAddr));

	if (nRet == SOCKET_ERROR)
	{
		if( WSAGetLastError() != WSAEWOULDBLOCK)
		{
			CString msg;
			msg.Format(_T("Connection Error: %s"),envir().ErrorCode2Text(WSAGetLastError()));
			envir().AppendLog(msg);
			return -1;
		}
		else
		{
			CString msg;
			msg.Format(_T("Connection Error: %s"),envir().ErrorCode2Text(WSAGetLastError()));
			envir().AppendLog(msg);

			return 1;		//	return 0 原本需要等待 链接成功
		}
	}
	else
	{
		return 1;
		
	}
}


int BaseClient::connectToServer(SOCKET	socketNum, int remotePortNum) 
{
	int			nRet=-1;
	
	CIOCPBuffer * pOverlapBuff= g_BufferMgr.AllocateBuffer(IOInitialize);
	if(pOverlapBuff==NULL) 
	{			
		envir().AppendLog(_T("AllocateBuffer(IOConnect) == NULL."));
		return -1;
	}

	//   重点  
	LPFN_CONNECTEX lpfnConnectEx = NULL ; 
	DWORD dwBytes = 0; 
	GUID GuidConnectEx = WSAID_CONNECTEX ; 

	//   重点，获得ConnectEx 函数的指针
	if (SOCKET_ERROR == WSAIoctl (socketNum , SIO_GET_EXTENSION_FUNCTION_POINTER , 
		&GuidConnectEx , sizeof (GuidConnectEx ), 
		&lpfnConnectEx , sizeof (lpfnConnectEx ), &dwBytes , 0, 0)) 
	{ 
		//TRACE ( "WSAIoctl is failed. Error code = %d" , WSAGetLastError ()); 
		return -1 ; 
	} 

	//   设置连接目标地址  
	SOCKADDR_IN	SockAddr;
	memset(&SockAddr,0,sizeof(SockAddr));
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_addr.s_addr = inet_addr(fServerAddress.c_str());
	SockAddr.sin_port = htons(remotePortNum);
	
	int nLen = sizeof (SockAddr ); 
	DWORD dwBytesSent = 0; 

	BOOL bResult = lpfnConnectEx(socketNum , 
		(sockaddr *)&SockAddr ,				// [in]   对方地址 
		nLen ,								// [in]   对方地址长度 
		NULL ,								// [in]   连接后要发送的内容，这里不用 
		0 ,									// [in]   发送内容的字节数 ，这里不用 
		&dwBytesSent ,						// [out]   发送了多少个字节，这里不用  
		(OVERLAPPED *)&pOverlapBuff->m_ol ); // [in]   这东西复杂，下一篇有详解 

	if (!bResult )      //   返回值处理 
	{ 
		if ( WSAGetLastError () != ERROR_IO_PENDING )   //   调用失败  
		{ 
			envir().AppendLog(_T("ConnextEx error "));
			return -1 ; 
		} 
		else //  操作未决（正在进行中 … )
		{ 
			return 0;
		} 
	} 

	return 1;
}

int BaseClient::openConnection()
{
	do{
		m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_Socket == INVALID_SOCKET) break;

		//  以下的绑定很重要，也是容易漏掉的。（如果少了绑定，在 ConnextEx  时将得到错误代码：10022  — 提供了一个无效的参数 
		sockaddr_in local_addr ; 
		ZeroMemory (&local_addr , sizeof (sockaddr_in )); 
		local_addr .sin_family = AF_INET ; 
		int irt = ::bind (m_Socket , (sockaddr *)(&local_addr ), sizeof (sockaddr_in )); 


		//  将 Socket 关联到 IOCP 
		if (!envir().AssociateClientWithCompletionPort(this))
		{
			CString msg;
			msg.Format(_T("AssociateClientWithCompletionPort Failed: %s"),envir().ErrorCode2Text(WSAGetLastError()));
			envir().AppendLog(msg);

			break;
		}

		int connectResult = connectToServer(m_Socket, fServerPortNum);
		if (connectResult < 0) break;
		
		return connectResult;

	}while(0);

	::closesocket(m_Socket);
	m_Socket = INVALID_SOCKET;
	return -1;
}


int BaseClient::openConnection_Old()
{
	do{
		m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_Socket == INVALID_SOCKET) break;

		const char chOpt = 1;
		int nErr = setsockopt(m_Socket, IPPROTO_TCP, TCP_NODELAY, &chOpt, sizeof(char));
		if (nErr == -1)
		{
			CString msg;
			msg.Format(_T("setsockopt() error: %s"),envir().ErrorCode2Text(WSAGetLastError()));
			envir().AppendLog(msg);
			break;
		}

		int connectResult = connectToServer(m_Socket, fServerPortNum);
		if (connectResult < 0) break;
		else if (connectResult > 0) {
			// The connection succeeded.  Arrange to handle responses to requests sent on it:

			if (!envir().AssociateClientWithCompletionPort(this))
			{
				CString msg;
				msg.Format(_T("AssociateClientWithCompletionPort Failed: %s"),envir().ErrorCode2Text(WSAGetLastError()));
				envir().AppendLog(msg);
				
				break;
			}

						
			//	说明连接成功，启动 读操作
			CIOCPBuffer *pOverlapBuff = g_BufferMgr.AllocateBuffer(IOInitialize);
			envir().PostQueuedCompletionStatus(this, pOverlapBuff);

		}
		return connectResult;

	}while(0);

	::closesocket(m_Socket);
	m_Socket = INVALID_SOCKET;
	return -1;
}


void BaseClient::resetResponseBuffer() 
{
	fResponseBytesAlreadySeen = 0;
	fResponseBufferBytesLeft = responseBufferSize;
}

void BaseClient::Disconnect()
{
	if(m_Socket!=INVALID_SOCKET)
	{
		::CancelIo((HANDLE)m_Socket);
		::closesocket(m_Socket );
		m_Socket = INVALID_SOCKET;
	}

}