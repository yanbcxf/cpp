#include "StdAfx.h"

//#include "Medium.h"
//#include "DelayQueue.h"
//#include "UsageEnvironment.h"
//#include "BaseClient.h"
//#include "AbstractCommand.h"

////////// A subclass of DelayQueueEntry,
//////////     used to implement BasicTaskScheduler0::scheduleDelayedTask()

class AlarmHandler: public DelayQueueEntry {
public:
	AlarmHandler(TaskFunc* proc, void* clientData, DelayInterval timeToDelay)
		: DelayQueueEntry(timeToDelay), fProc(proc), fClientData(clientData) {
	}

private: // redefined virtual functions
	virtual void handleTimeout() {
		(*fProc)(fClientData);
		DelayQueueEntry::handleTimeout();
	}

private:
	TaskFunc* fProc;
	void* fClientData;
};

//////////////////////////////////////////////////////////////////////////

UsageEnvironment::UsageEnvironment(long sid)
: liveMediaPriv(NULL)
{
	m_bShutDown = FALSE;
	m_ID = sid;

	m_nSeq = 0;

	stringstream ss;
	ss << "Envir-" << m_ID;

	string loggerName = ss.str();
	m_logger = getDynamicLogger(loggerName);

}

UsageEnvironment::~UsageEnvironment(void)
{

}



CString UsageEnvironment::ErrorCode2Text(DWORD dw)
{
	CString error="";
	// Put your own common error text here (give more explaination etc..) 
	switch(dw)
	{
	case WSAEFAULT:
		error="WSAEFAULT	The buf parameter is not completely contained in a valid part of the user address space.";
		break; 
	case WSAENOTCONN:
		error="WSAENOTCONN	The socket is not connected."; 
		break;
	case WSAEINTR:
		error="WSAEINTR	The (blocking) call was canceled through WSACancelBlockingCall.	"; 
		break;
	case WSAENOTSOCK:
		error=" WSAENOTSOCK	The descriptor s is not a socket."; 
		break; 
	case WSANOTINITIALISED:
		error="WSANOTINITIALISED: A successful WSAStartup call must occur before using this function.";
		break; 
	case WSAENETDOWN:
		error="WSAENETDOWN	The network subsystem has failed."; 
		break;
	case WSAEINPROGRESS:
		error="WSAEINPROGRESS	A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function."; 
		break;
	case WSAENETRESET:
		error=" WSAENETRESET	The connection has been broken due to the keep-alive activity detecting a failure while the operation was in progress."; 
		break; 
	case WSAEOPNOTSUPP:
		error="WSAEOPNOTSUPP	MSG_OOB was specified, but the socket is not stream-style such as type SOCK_STREAM, OOB data is not supported in the communication domain associated with this socket, or the socket is unidirectional and supports only send operations.	"; 
		break; 
	case WSAESHUTDOWN:
		error="WSAESHUTDOWN	The socket has been shut down; it is not possible to receive on a socket after shutdown has been invoked with how set to SD_RECEIVE or SD_BOTH."; 
		break;
	case WSAEWOULDBLOCK:
		error=" WSAEWOULDBLOCK	The socket is marked as nonblocking and the receive operation would block.	"; 
		break; 
	case WSAEMSGSIZE:
		error=" WSAENOTSOCK		The message was too large to fit into the specified buffer and was truncated."; 
		break;
	case WSAEINVAL:
		error="WSAEINVAL	The socket has not been bound with bind, or an unknown flag was specified, or MSG_OOB was specified for a socket with SO_OOBINLINE enabled or (for byte stream sockets only) len was zero or negative.	"; 
	case WSAECONNABORTED:
		error=" 	WSAECONNABORTED	The virtual circuit was terminated due to a time-out or other failure. The application should close the socket as it is no longer usable."; 
		break; 
	case WSAETIMEDOUT:
		error="WSAETIMEDOUT	The connection has been dropped because of a network failure or because the peer system failed to respond.	"; 
		break; 
	case WSAECONNRESET:
		//error="WSAECONNRESET	The virtual circuit was reset by the remote side executing a hard or abortive close."; 
		error="WSAECONNRESET Connection dropped..";
		break;

	default:
		error="";  
		break;
	}

	// Use system format..  
	if(error.IsEmpty())
	{
		LPVOID lpMsgBuf;
		error="";
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &lpMsgBuf,
			0, NULL );
		error.Format("%s",lpMsgBuf);
		LocalFree(lpMsgBuf);
	}
	return error;
}

void UsageEnvironment::AppendInfoLog(CString msg)
{
	LOG4CPLUS_INFO(m_logger, msg);
}

void UsageEnvironment::AppendLog(CString msg)
{
	//LOG4CPLUS_DEBUG(m_logger, msg);
}

#ifndef MILLION
#define MILLION 1000000
#endif

UINT __stdcall UsageEnvironment::IOWorkerThreadProc(LPVOID pParam)
{
	UsageEnvironment* pThis = reinterpret_cast<UsageEnvironment*>(pParam);
	if ( pThis )
	{

		if(pThis->CreateCompletionPort())
		{
			pThis->AppendLog("===================== IOWorker Start!");
		}
		else
		{
			pThis->AppendLog("===================== IOWorker CreateCompletionPort 失败!");
			return 0xdead;
		}
		

		DWORD dwIoSize;
		PVOID lpClientContext;		// 可能指向 BaseClient 或者 AbstractCommand 对象

		CIOCPBuffer *pOverlapBuff;
		bool bError=false;
		HANDLE hCompletionPort = pThis->m_hCompletionPort;
		LPOVERLAPPED lpOverlapped;
		//pOverlapPlus = CONTAINING_RECORD(lpOverlapped, MYOVERLAPPEDPLUS, m_ol);
		while ( !bError )
		{

			// 计算离当前最近需要处理的 定时任务时间
			DelayInterval const& timeToDelay = pThis->fDelayQueue.timeToNextAlarm();
			struct timeval tv_timeToDelay;
			tv_timeToDelay.tv_sec = timeToDelay.seconds();
			tv_timeToDelay.tv_usec = timeToDelay.useconds();
			// Very large "tv_sec" values cause select() to fail.
			// Don't make it any larger than 1 million seconds (11.5 days)
			const long MAX_TV_SEC = MILLION;
			if (tv_timeToDelay.tv_sec > MAX_TV_SEC) {
				tv_timeToDelay.tv_sec = MAX_TV_SEC;
			}

			long waitTime = tv_timeToDelay.tv_sec * 1000 + tv_timeToDelay.tv_usec /1000 ;
			CString msg;
			msg.Format("GetQueuedCompletionStatus 开始执行 ,允许 Timeout = %d", waitTime);
			//pThis->AppendLog(msg);

			if(waitTime==0)
			{
				//pThis->AppendLog("=== 开始执行定时任务 ===");
				pThis->fDelayQueue.handleAlarm();
			}
			
			pOverlapBuff=NULL;
			lpClientContext=NULL;
			// Get a completed IO request.
			BOOL bIORet = GetQueuedCompletionStatus(
				hCompletionPort,
				&dwIoSize,
				(LPDWORD) &lpClientContext,
				&lpOverlapped, waitTime /*INFINITE*/);

			
			// If Something whent wrong..
			if (!bIORet)  
			{
				DWORD dwIOError = GetLastError();
				if(dwIOError != WAIT_TIMEOUT) // It was not an Time out event we wait for ever (INFINITE) 
				{	
					CString msg;
					msg.Format(_T("GetQueuedCompletionStatus errorCode: %i, %s\n"),WSAGetLastError(),pThis->ErrorCode2Text(dwIOError));
					pThis->AppendLog(msg);
					// if we have a pointer & This is not an shut down.. 
					//if (lpClientContext!=NULL && pThis->m_bShutDown == false)
					if (lpClientContext!=NULL)
					{
						/*
						* ERROR_NETNAME_DELETED Happens when the communication socket
						* is cancelled and you have pendling WSASend/WSARead that are not finished. 
						* Then the Pendling I/O (WSASend/WSARead etc..) is cancelled and we return with 
						* ERROR_NETNAME_DELETED.. 
						*/
						if(dwIOError==ERROR_NETNAME_DELETED) 
						{

							pOverlapBuff=CONTAINING_RECORD(lpOverlapped, CIOCPBuffer, m_ol);
							int nOper = pOverlapBuff->GetOperation();

							/*stringstream ss;
							ss << "ERROR_NETNAME_DELETED , GetOperation = " << nOper;
							ss << "BaseClient Name = " << string((char const *)lpClientContext);
							LOG4CPLUS_DEBUG(g_Logger, ss.str());*/

							Medium * pBaseClient ;
							if(Medium::lookupByName(*pThis, (char const *)lpClientContext, pBaseClient))
							{
								((BaseClient *)pBaseClient)->Reclaim(); 
							}

							
							//TRACE("ERROR_NETNAME_DELETED\r\n");
							

						}
						else
						{ // Should not get here if we do: disconnect the client and cleanup & report. 

							pThis->AppendLog(pThis->ErrorCode2Text(dwIOError)); 

							Medium * pBaseClient ;
							if(Medium::lookupByName(*pThis, (char const *)lpClientContext, pBaseClient))
							{
								((BaseClient *)pBaseClient)->Reclaim(); 
							}

							/*((BaseClient *)lpClientContext)->Reclaim(); 
							CString msg;
							msg.Format(">IOWORKER2 (%x)\r\n",lpClientContext);
							pThis->AppendLog(msg);*/
						}
						// Clear the buffer if returned. 
						pOverlapBuff=NULL;
						if(lpOverlapped!=NULL)
							pOverlapBuff=CONTAINING_RECORD(lpOverlapped, CIOCPBuffer, m_ol);
						if(pOverlapBuff!=NULL)
							g_BufferMgr.ReleaseBuffer(pOverlapBuff);			
						continue;
					}
					// We shall never come here  
					// anyway this was an error and we should exit the worker thread
					bError = true;
					pThis->AppendLog(pThis->ErrorCode2Text(dwIOError)); 
					pThis->AppendLog("IOWORKER KILLED BECAUSE OF ERROR IN GetQueuedCompletionStatus");

					pOverlapBuff=NULL;
					if(lpOverlapped!=NULL)
						pOverlapBuff=CONTAINING_RECORD(lpOverlapped, CIOCPBuffer, m_ol);
					if(pOverlapBuff!=NULL)
						g_BufferMgr.ReleaseBuffer(pOverlapBuff);
					continue;
				}
				else
				{
					//pThis->AppendLog("=== 开始执行定时任务 ===");
					pThis->fDelayQueue.handleAlarm();
					continue;
				}
			}

			if(bIORet && lpOverlapped && lpClientContext) 
			{
				pOverlapBuff=CONTAINING_RECORD(lpOverlapped, CIOCPBuffer, m_ol);
				if(pOverlapBuff!=NULL)
					pThis->ProcessIOMessage(pOverlapBuff, lpClientContext, dwIoSize);
			}	

			if(lpClientContext==NULL && pOverlapBuff==NULL && pThis->m_bShutDown)
			{
				pThis->AppendLog("lpClientContext==NULL \r\n"); 
				bError=true;
			}
			
		} 

	}

	pThis->AppendLog("================================= IOWorker Died!");
	return 0xdead;
}



BOOL UsageEnvironment::AssociateClientWithCompletionPort(BaseClient * pBaseClient)
{
	HANDLE h = CreateIoCompletionPort((HANDLE) pBaseClient->m_Socket, m_hCompletionPort, (DWORD)pBaseClient->name(), 0);
	return h == m_hCompletionPort;
}

BOOL UsageEnvironment::PostQueuedCompletionStatus(BaseClient * pBaseClient, CIOCPBuffer *pOverlapBuff)
{
	if(pOverlapBuff!=NULL)
	{
		BOOL bSuccess = ::PostQueuedCompletionStatus(m_hCompletionPort, pOverlapBuff->GetUsed(), (DWORD) pBaseClient, &pOverlapBuff->m_ol);

		if ( (!bSuccess && GetLastError( ) != ERROR_IO_PENDING))
		{            
			CString msg;
			msg.Format("Error in UsageEnvironment::PostQueuedCompletionStatus: %s",ErrorCode2Text(WSAGetLastError()));
			AppendLog(msg);
			return FALSE;
		}
		return TRUE;
	}
	else
	{
		CString msg;
		msg.Format("ERROR Could not allocate memory for buffer in Acceptincoming: %s",ErrorCode2Text(WSAGetLastError()));
		AppendLog(msg);
		return FALSE;
	}
}


/*
* Creates the  Completion Port m_hCompletionPort used by 
* IO worker Threads. 
*/
BOOL UsageEnvironment::CreateCompletionPort()
{
	SOCKET s;	

	//
	// First open a temporary socket that we will use to create the
	// completion port.  In NT 3.51 it will not be necessary to specify
	// the FileHandle parameter of CreateIoCompletionPort()--it will
	// be legal to specify FileHandle as NULL.  However, for NT 3.5
	// we need an overlapped file handle.
	//

	s = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if ( s == INVALID_SOCKET ) 
	{	
		CString msg;
		msg.Format("Error Could not start Create Socket in CreateCompletionPort: %s",ErrorCode2Text(WSAGetLastError()));
		AppendLog(msg);
		return FALSE;
	}


	// Create the completion port that will be used by all the workers
	// threads.
	m_hCompletionPort = CreateIoCompletionPort( (HANDLE)s, NULL, 0, 0 );
	if ( m_hCompletionPort == NULL ) 
	{	
		CString msg;
		msg.Format("CreateIoCompletionPort Failed: %s",ErrorCode2Text(WSAGetLastError()));
		AppendLog(msg);
		closesocket( s );
		return FALSE;
	}

	closesocket( s );
	return TRUE;
}


/*
* Starts the IOWorkers. 
*
*/

BOOL UsageEnvironment::SetupIOWorkers()
{
	UINT threadid;
	//for(int i=0; i<m_iMaxIOWorkers;i++)
	// 每个的 UsageEnvironment 对应一个的 线程
	{
		HANDLE threadhandle = (HANDLE)_beginthreadex(NULL,0,UsageEnvironment::IOWorkerThreadProc,this,0,&threadid);
		
		if(threadhandle)
			m_IOWorkerList.push_back(threadhandle);
		else
		{

			CString msg;
			msg.Format("Error Couldnot start worker: %s",ErrorCode2Text(WSAGetLastError()));
			AppendLog(msg);

			return FALSE;
		}
	}
	return TRUE; 
}



void UsageEnvironment::ProcessIOMessage(CIOCPBuffer *pOverlapBuff, PVOID pContext, DWORD dwSize)
{

	if(pOverlapBuff==NULL)
		return;

	//	Sleep(rand()%50);
	switch (pOverlapBuff->GetOperation())
	{

	case IOInitialize :
		AppendLog("UsageEnvironment::ProcessIOMessage ---> IOInitialize");
		{
			Medium  *pBaseClient ;
			if(Medium::lookupByName(*this, (char const *)pContext, pBaseClient))
			{
				((BaseClient *)pBaseClient)->OnInitialize(pOverlapBuff);
			}
		}
		//((BaseClient *)pContext)->OnInitialize(pOverlapBuff);
		break;

	case IORead :
		AppendLog("UsageEnvironment::ProcessIOMessage ---> IORead");
		((BaseClient *)pContext)->OnRead(pOverlapBuff);
		break;	


	case IOReadCompleted :
		AppendLog("UsageEnvironment::ProcessIOMessage ---> IOReadCompleted");
		{
			Medium  *pBaseClient ;
			if(Medium::lookupByName(*this, (char const *)pContext, pBaseClient))
			{
				((BaseClient *)pBaseClient)->OnReadCompleted(dwSize,pOverlapBuff);
			}
		}
		//((BaseClient *)pContext)->OnReadCompleted(dwSize,pOverlapBuff);
		break;


	case IOWrite :
		AppendLog("UsageEnvironment::ProcessIOMessage ---> IOWrite");
		((BaseClient *)pContext)->OnWrite(dwSize,pOverlapBuff);
		break;

	case IOWriteCompleted :
		AppendLog("UsageEnvironment::ProcessIOMessage ---> IOWriteCompleted");
		{
			Medium * pBaseClient ;
			if(Medium::lookupByName(*this, (char const *)pContext, pBaseClient))
			{
				((BaseClient *)pBaseClient)->OnWriteCompleted(dwSize,pOverlapBuff);
			}
		}
		//((BaseClient *)pContext)->OnWriteCompleted(dwSize,pOverlapBuff);
		break;

	case IOZeroByteRead : //  Workaround the the WSAENOBUFS error problem..See OnZeroByteRead. 
		//((BaseClient *)pContext)->OnZeroByteRead(pOverlapBuff);
		break;

	case IOZeroReadCompleted : //  Workaround the the WSAENOBUFS error problem.. 
		//((BaseClient *)pContext)->OnZeroByteReadCompleted(dwSize,pOverlapBuff);
		break;

	case IOCommand : //  执行 SDK 接口投递的命令.. 
		AppendLog("UsageEnvironment::ProcessIOMessage ---> IOCommand");
		((AbstractCommand *)pContext)->CommandExecute(this);
		break;

	default:
		g_BufferMgr.ReleaseBuffer(pOverlapBuff);
		break;

	} 
}



TaskToken UsageEnvironment::scheduleDelayedTask(int64_t microseconds,
												   TaskFunc* proc,
												   void* clientData) 
{
	if (microseconds < 0) microseconds = 0;
	DelayInterval timeToDelay((long)(microseconds/1000000), (long)(microseconds%1000000));
	AlarmHandler* alarmHandler = new AlarmHandler(proc, clientData, timeToDelay);
	fDelayQueue.addEntry(alarmHandler);

	return (void*)(alarmHandler->token());												  
}

void UsageEnvironment::unscheduleDelayedTask(TaskToken& prevTask) {
	DelayQueueEntry* alarmHandler = fDelayQueue.removeEntry((intptr_t)prevTask);
	prevTask = NULL;
	delete alarmHandler;
}


void UsageEnvironment::rescheduleDelayedTask(TaskToken& task,
	int64_t microseconds, TaskFunc* proc,
	void* clientData) {
	unscheduleDelayedTask(task);
	task = scheduleDelayedTask(microseconds, proc, clientData);
}