#include "StdAfx.h"
#include "AbstractCommand.h"

AbstractCommand::AbstractCommand(bool bSnyc)
{
	m_bSnyc = bSnyc;
	if(m_bSnyc)
		hSempaphore = CreateSemaphore(NULL, 0, 1, NULL );
}

AbstractCommand::~AbstractCommand(void)
{
	if(m_bSnyc)
		CloseHandle(hSempaphore);
	
}


void AbstractCommand::CommandExecute(UsageEnvironment* pThis)
{
	m_nResult = Execute(pThis);

	stringstream ss;
	char tmp[32];
	sprintf_s(tmp, 32, "%08d", m_nResult);
	ss <<  CommandType() << ", 执行完成， 结果为[" << string(tmp) << "]";

	bool bLog = true;
	if(CommandType()=="CSipdCataloginfoResponse")
		bLog = false;

	if(bLog)
		LOG4CPLUS_DEBUG(pThis->m_Logger, ss.str());

	if(m_bSnyc==true)
	{
		//	命令执行完成后，发送通知
		ReleaseSemaphore(hSempaphore, 1, NULL);
	}
	else
	{
		//  异步命令，执行完成直接删除
		delete this;
	}
}



int AbstractCommand::Call(UsageEnvironment & environ )
{
	stringstream ss;
	int nResult ;
	do 
	{
		nResult = environ.PostCommand(this);
		

		if(nResult < 0)
		{
			/* 由于命令重入，故投递失败 */
			nResult = -2;
			break;
		}		

		break;
		// 以下不用


		/*命令投递结束*/
		if(m_bSnyc)
		{
			/*上层调用的阻塞式执行*/
			WaitForSingleObject(hSempaphore, INFINITE);

			nResult = m_nResult;
		}
		/*else
		{
			nResult = 0;
		}*/

	} while (0);

	if( 0 /*m_bSnyc*/)
	{
		//	同步命令在用户的调用线程中 delete 
		delete this;
	}

	return nResult;
}