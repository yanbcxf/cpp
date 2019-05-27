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

#define  CATALOG_SESSION_NUM 1
#define  DEVICEINFO_SESSION_NUM 1
#define  STATUSINFO_SESSION_NUM 1

class UsageEnvironment
{
public:
	UsageEnvironment(void);
	~UsageEnvironment(void);

	void SendMessageEx(int srcid,  int msgtype, void *data, int msg_len);
	static DWORD WINAPI MessageHandler1(void *data);
	virtual void MessageHandler(MSG * msg){};

	bool PostCallBack(CCommandCallback * cb);
	static DWORD WINAPI CallbackHandler1(void *data);
	DWORD  CallbackHandler();

	//	数据流的回调队列
	bool PostDataCallBack(CCommandCallback * cb);
	static DWORD WINAPI DataCallbackHandler1(void *data);
	DWORD  DataCallbackHandler();

	int PostCommand(AbstractCommand * cmd);
	static DWORD WINAPI CommandHandler1(void *data);
	DWORD  CommandHandler();

	//	对应 Superior
	int PostRequest(AbstractCommand * cmd);
	static DWORD WINAPI RequestHandler1(void *data);
	DWORD  RequestHandler();

	int	  fetchSerialNumber();
	void  MonitorSession();
	CAbstractObject * SearchSession(string sessionType, int nId);

	static UsageEnvironment& getInstance();

private:
	static UsageEnvironment * instance;

protected:
	static int g_SerialNumber;

	//	对应 Junior 使用的线程
	DWORD m_ThreadId;
	HANDLE m_ThreadHandle;

	DWORD m_ThreadId_Callback;
	HANDLE m_ThreadHandle_Callback;

	DWORD m_ThreadId_DataCallback;
	HANDLE m_ThreadHandle_DataCallback;

	//	对应 Superior 使用的线程
	DWORD m_ThreadId_request;
	HANDLE m_ThreadHandle_request;

	DWORD m_ThreadId_response;
	HANDLE m_ThreadHandle_response;

public:
	JMutex		m_mtxObjects;
	map<int, CAbstractObject *>	m_mapObjects;

	//	对应 Superior
	JMutex		m_mtxRequest;
	vector<AbstractCommand *>   m_vecRequests;
	JMutex		m_mtxRequests;


	//	对应 Junior 
	int m_nDeviceControlSession; 
	int m_nDeviceinfoSession;
	int m_nStateinfoSession;
	int m_nCataloginfoSession;
	int m_nSearchRecordfileSession;
	int m_nMediaSession;

	vector<AbstractCommand *>   m_vecCommandsCataloginfo;
	vector<AbstractCommand *>   m_vecCommandsDeviceinfo;
	vector<AbstractCommand *>   m_vecCommandsStateinfo;
	vector<AbstractCommand *>   m_vecCommands;
	JMutex		m_mtxCommands;

	vector<CCommandCallback *>   m_vecCallbacks;
	JMutex		m_mtxCallbacks;

	vector<CCommandCallback *>   m_vecDataCallbacks;
	JMutex		m_mtxDataCallbacks;

	
	
	// a pointer to additional, optional, client-specific state
	void* liveMediaPriv;
	Logger  m_Logger;

	SIPD_GATEWAY_CFG	m_cfg;

	int					m_iRtpPort;		//	保存当前已经被使用的 最后端口

	unsigned long  m_tvMonitorSession;
	unsigned long  m_tvMonitorSessionLog;

	map<int, int> m_mapDataInput;	//	对数据流的发送情况 进行统计
	map<int, int> m_mapDataOutput;	//	对数据流的发送情况 进行统计
};
