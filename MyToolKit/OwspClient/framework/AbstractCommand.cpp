#include "StdAfx.h"
#include "AbstractCommand.h"

AbstractCommand::AbstractCommand(UsageEnvironment * pEnvir)
{
	m_pEnvir = pEnvir;
	hSempaphore = CreateSemaphore(NULL, 0, 1, NULL );
	hSempaphoreRsp = CreateSemaphore(NULL, 0, 1, NULL );

	m_pRecvBuffer = NULL;
	m_nRecvLength = 0;
}

AbstractCommand::~AbstractCommand(void)
{
	CloseHandle(hSempaphore);
	CloseHandle(hSempaphoreRsp);

	if(m_pRecvBuffer)
	{
		delete[] m_pRecvBuffer;
	}
}


void AbstractCommand::CommandExecute(UsageEnvironment* pThis)
{
	m_nSendResult = Execute(pThis);

	//	命令执行完成后，发送通知
	ReleaseSemaphore(hSempaphore, 1, NULL);
}

void AbstractCommand::ResponseHandler1(void * pUserContext, int nResult, unsigned char * pData , int length)
{
	if(pUserContext)
	{
		AbstractCommand * pCommand = (AbstractCommand *)pUserContext;
		pCommand->ResponseHandler(nResult, pData, length);
	}
}

void AbstractCommand::ResponseHandler(int nResult, unsigned char * pData , int length)
{
	m_nRecvResult = nResult;
	if(length>0)
	{
		m_pRecvBuffer = new unsigned char[length];
		if(m_pRecvBuffer)
		{
			memcpy(m_pRecvBuffer, pData, length);
		}
		m_nRecvLength = length;
	}

	//	收到服务器的回复后，发送通知
	ReleaseSemaphore(hSempaphoreRsp, 1, NULL);
}


int AbstractCommand::SyncCall()
{
	/*上层调用的阻塞式执行*/

	stringstream ss;
	int nResult = RESULT_CODE_LOCAL_ERR;
	CIOCPBuffer * pBuffer = NULL;
	do 
	{
		pBuffer = g_BufferMgr.AllocateBuffer(IOCommand);
		if(pBuffer==NULL)	
			break;
		
		BOOL bSuccess = m_pEnvir->PostQueuedCompletionStatus((BaseClient *)this, pBuffer);
		if ( (!bSuccess && GetLastError( ) != ERROR_IO_PENDING))
			break;
		
		ss << "命令投递完成, result( bSuccess ) = " << bSuccess;
		LOG4CPLUS_DEBUG(g_Logger, ss.str().c_str());

		/*命令投递结束*/
		WaitForSingleObject(hSempaphore, INFINITE);

		if(pBuffer)
			g_BufferMgr.ReleaseBuffer(pBuffer);

		nResult = m_nSendResult;
		ss << "命令发送完成, result( CSeq ) = " << nResult;
		LOG4CPLUS_DEBUG(g_Logger, ss.str().c_str());

		if(m_nSendResult<0)
			break;
		
		/*至此，命令发送已成功；等待服务器返回结果*/
		WaitForSingleObject(hSempaphoreRsp, INFINITE);
		
		ParseResponse();

		ss.str("");
		ss << "回复接收完成, result = " << m_nRecvResult;
		 LOG4CPLUS_DEBUG(g_Logger, ss.str().c_str());

		
		nResult = m_nRecvResult;

	} while (0);

	return nResult;
}