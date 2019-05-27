#pragma once

class UsageEnvironment;

class AbstractCommand
{
public:
	AbstractCommand(UsageEnvironment * pEnvir);

	virtual ~AbstractCommand(void);

	int SyncCall();

	void CommandExecute(UsageEnvironment* pThis); 

	static void ResponseHandler1(void * pUserContext, int nResult, unsigned char * pData , int length);
	void ResponseHandler(int nResult, unsigned char * pData , int length);

	virtual int Execute(UsageEnvironment* pThis) = 0;
	virtual void ParseResponse() {}

public:
	int		m_nSendResult;			//	命令发送的结果

	int		m_nRecvResult;
	unsigned char * m_pRecvBuffer;	//  服务器返回的结果
	int		m_nRecvLength;

protected:
	UsageEnvironment * m_pEnvir;
public:
	HANDLE hSempaphore;

	HANDLE hSempaphoreRsp;
	
};
