#pragma once

#define MAX_JSON 2048

class CIpmsClient : public BaseClient
{
public:
	typedef void (responseHandler)(void  * pUserContext, int nResult, unsigned char * pData, int dataLength);

	CIpmsClient(UsageEnvironment& env, string strIPAddr, int nPort, string strDeviceSN, bool bControlBox);
	~CIpmsClient();

	enum { PACKET_0 = 0, PACKET_DATA };

	class RequestRecord {
	public:
		RequestRecord(unsigned cseq, responseHandler* handler, void * pUserContext,
			char* url, u_int32 body_len, unsigned char * body_data);

		// alternative constructor for creating "PLAY" requests that include 'absolute' time values
		virtual ~RequestRecord();

		RequestRecord*& next() { return fNext; }
		unsigned& cseq() { return fCSeq; }
		//char const* commandName() const { return fCommandName; }


		responseHandler*& handler() { return fHandler; }
		void * userContext() { return fUserContext; }

		char			f_url[256];
		u_int32			f_body_len;
		unsigned char * f_body_data;

		struct timeval	f_tvSubmitTime;

	private:
		RequestRecord* fNext;
		unsigned fCSeq;
		//char const* fCommandName;
		responseHandler* fHandler;
		void * fUserContext;
	};

	//////////////////////////////////////////////////////////////////////////

	void _memcpy(unsigned char *  mm_vPkgData, int mm_vPkgLen, unsigned char * vPkgData, int curPos, int  vPkgLen);
	void SendDataByBuffer(unsigned char * vPkgData, int vPkgLen, bool bMark);
	void handleResponseBytes(int newBytesRead);
	virtual void handleConnected();
	virtual void handleDestoryed();
	virtual unsigned sendRequest(RequestRecord* request);

	static void TestTaskFunc(void* clientData);
	static void TestHeartBeatFunc(void * clientData);
	static void CheckTimeOuntFunc(void* clientData);

	void TimeoutProcess();

	unsigned sendRegisterCommand(void * pUserContext);
	unsigned sendKeepAlive();
	unsigned sendDevStatePush();
	unsigned sendEleReport();
	unsigned sendOfflineEleReport();
	unsigned sendDeviceAlarm();

private:
	//////////////////////////////////////////////////////////////////////////
	class RequestQueue {
	public:
		RequestQueue();
		RequestQueue(RequestQueue& origQueue); // moves the queue contents to the new queue
		virtual ~RequestQueue();

		void enqueue(RequestRecord* request); // "request" must not be NULL
		RequestRecord* dequeue();
		void putAtHead(RequestRecord* request); // "request" must not be NULL
		RequestRecord* findByCSeq(unsigned cseq);
		Boolean isEmpty() const { return fHead == NULL; }

	private:
		RequestRecord* fHead;
		RequestRecord* fTail;
	};


	RequestQueue fRequestsAwaitingConnection, fRequestsAwaitingResponse;

private:
	string	m_strServerIp;
	int		m_nPort;

	// 协议解析
	unsigned int  m_nStatus;
	unsigned int  m_nContenLength;
	string			m_strHeader;
	string			m_strCookie;

	//发送缓冲区
	unsigned char  m_vPkgData[1500];		//MTU 1500  1440
	int	m_vPkgLen;

	responseHandler*  m_pHandler;

	TaskToken	m_TimeoutTask;
	TaskToken	m_HeartbeatTask;

public:
	string	DeviceSN;
	bool	isControlBox;
	int		DeviceType;
	string	DeviceName;
	string	DeviceMac;

	string	SessionID;

	int		Upload;
	int		Fre;
	int		ACEnable;
	int		DHCP;
	string	IpAddr;
	string	GateWay;
	string	NetMask;
	string	DNS1;
	string	DNS2;
	
	int		PortNum;
	int		CollectEnable[64];
	int		PortState[64];
	int		PortAC[64];
	string	Dev;

	int		FanLevel;
	int		Open1;
	int		Open2;
	int		Relay1State;
	int		Relay2State;

	int		AC;
	string	Time;

	int		AlarmNum;
	string	AlarmDescription;
	string	AlarmTime;
	int		AlarmPort;
};

