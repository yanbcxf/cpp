//#pragma once

#ifndef _USAGEENVIRONMENT_HH
#define _USAGEENVIRONMENT_HH

typedef void TaskFunc(void* clientData);
typedef void* TaskToken;
typedef unsigned int EventTriggerId;


class BaseClient;

class UsageEnvironment 
{
public:
	UsageEnvironment(long sid);
	~UsageEnvironment(void);

	// result message handling:
	typedef char const* MsgString;
	virtual MsgString getResultMsg() const { return NULL; };

	virtual void setResultMsg(MsgString msg) {};
	virtual void setResultMsg(MsgString msg1, MsgString msg2) {};
	virtual void setResultMsg(MsgString msg1, MsgString msg2, MsgString msg3) {};

	BOOL AssociateClientWithCompletionPort(BaseClient * pBaseClient);
	BOOL PostQueuedCompletionStatus(BaseClient * pBaseClient, CIOCPBuffer *pOverlapBuff);

	void AppendLog(CString msg);
	void AppendInfoLog(CString msg);
	// Error Convertion.. 
	CString ErrorCode2Text(DWORD dw);
	BOOL SetupIOWorkers();

	TaskToken scheduleDelayedTask(int64_t microseconds, TaskFunc* proc,	void* clientData);
	void unscheduleDelayedTask(TaskToken& prevTask);
	void rescheduleDelayedTask(TaskToken& task, int64_t microseconds, TaskFunc* proc, void* clientData);

	int GetSeq()
	{
		m_nSeq++;
		if(m_nSeq>2000000000)   // 20 вк
			m_nSeq = 0;
		return m_nSeq;
	}

	// a pointer to additional, optional, client-specific state
	void* liveMediaPriv;

public:
	HANDLE	m_hCompletionPort;

private:
	void ProcessIOMessage(CIOCPBuffer *pOverlapBuff, PVOID pContext, DWORD dwSize);
	static UINT __stdcall IOWorkerThreadProc(LPVOID pParam);
	BOOL CreateCompletionPort();
	

protected:
	DelayQueue fDelayQueue;
	
protected:

	list<HANDLE>	m_IOWorkerList;
	BOOL	m_bShutDown;
	Logger  m_logger;
	long	m_ID;

	int		m_nSeq;

};





#endif