#pragma once


#pragma pack(push) //保存对齐状态
#pragma pack (1)     /*指定按1字节对齐*/

typedef struct
{
	int thread_id;		//发送的thead ID专用
	int msg_len;
	char para[1];	//消息参数，不同的消息对应不同的消息参数
} SN_MSG;

#pragma pack(pop)//恢复对齐状态

class UsageEnvironment
{
public:
	UsageEnvironment(void);
	~UsageEnvironment(void);

	void SendMessageEx(int srcid,  int msgtype, void *data, int msg_len);

	static DWORD WINAPI MessageHandler1(void *data);

	virtual void MessageHandler(MSG * msg) = 0;

	

protected:
	DWORD m_ThreadId;
	HANDLE m_ThreadHandle;

	

public:
	//	设备 uri ： 设备注册地址

	
	// a pointer to additional, optional, client-specific state
	void* liveMediaPriv;
	Logger  m_Logger;
};
