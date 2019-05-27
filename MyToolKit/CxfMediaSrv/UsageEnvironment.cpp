#include "StdAfx.h"

#include "UsageEnvironment.h"

UsageEnvironment::UsageEnvironment(void)
:liveMediaPriv(NULL)
{
	m_ThreadHandle  =  CreateThread(NULL,  0 , MessageHandler1, (LPVOID)this ,  0 ,  &m_ThreadId);
	if(NULL == m_ThreadHandle)
	{

	}
	stringstream ss;
	ss << "ThrId-" << m_ThreadId;
	m_Logger = getDynamicLogger(ss.str());

	LOG4CPLUS_DEBUG(m_Logger, "线程开始执行......");
}

UsageEnvironment::~UsageEnvironment(void)
{
	LOG4CPLUS_DEBUG(m_Logger, "线程执行 End.");

	if(m_ThreadHandle)
		CloseHandle(m_ThreadHandle);
}


void UsageEnvironment::SendMessageEx(int srcid,  int msgtype, void *data, int msg_len)
{
	SN_MSG *pmsg = (SN_MSG *)malloc(sizeof(SN_MSG) + msg_len);

	pmsg->thread_id = srcid;
	pmsg->msg_len = msg_len;

	if (!(msg_len == 0 || data == NULL))
	{
		memcpy(pmsg->para, data, msg_len);
	}

	if (!PostThreadMessage(m_ThreadId, msgtype, (WPARAM)pmsg, 0))
	{
		char tmp[1024];
		sprintf(tmp, "sendmessage err %d\n", GetLastError());
		free(pmsg);
		pmsg = NULL;
	}

	return;
}


DWORD WINAPI UsageEnvironment::MessageHandler1(void *data)
{
	UsageEnvironment * pThis = (UsageEnvironment *)data;
	
	MSG msg;
	SN_MSG * tmp = NULL;
	PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
	for(;;)
	{
		if( GetMessage( &msg, 0, 0, 0) )
		{
			tmp = (SN_MSG *)msg.wParam;
			pThis->MessageHandler(&msg);

			tmp = (SN_MSG *)msg.wParam;
			if (tmp != NULL)
			{
				free((void *)msg.wParam);
			}
		}
	}
}

