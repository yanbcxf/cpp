#pragma once

class COwspClient : public BaseClient
{
public:
	typedef void (responseHandler)(string  strClientName, unsigned char * pData,  int dataLength);

	COwspClient(UsageEnvironment& env, char const* strIPAddr, int nPort, char const * strDevSn);
	~COwspClient(void);

	enum { PACKET_0 = 0,  TLV_0, TLV_1 };

public:
	class RequestRecord {
	public:
		RequestRecord(unsigned cseq,  responseHandler* handler,	u_int16	 tlv_type, u_int16 tlv_len, unsigned char * tlv_v );
		
		// alternative constructor for creating "PLAY" requests that include 'absolute' time values
		virtual ~RequestRecord();

		RequestRecord*& next() { return fNext; }
		unsigned& cseq() { return fCSeq; }
		//char const* commandName() const { return fCommandName; }
		
		
		responseHandler*& handler() { return fHandler; }

		u_int16		f_tlv_type;
		u_int16		f_tlv_len;
		unsigned char * f_tlv_v;

	private:
		RequestRecord* fNext;
		unsigned fCSeq;
		//char const* fCommandName;
	
		responseHandler* fHandler;
	};

	//////////////////////////////////////////////////////////////////////////

	void _memcpy(unsigned char *  mm_vPkgData, int mm_vPkgLen, unsigned char * vPkgData,   int curPos, int  vPkgLen);
	void SendDataByBuffer(unsigned char * vPkgData, int vPkgLen, bool bMark);
	void handleResponseBytes(int newBytesRead);
	virtual void handleConnected() ;
	virtual void handleDestoryed();
	virtual unsigned sendRequest(RequestRecord* request);
	static void TestTaskFunc(void* clientData);

	unsigned sendLoginCommand();

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

	//////////////////////////////////////////////////////////////////////////

	string format_OWSP_ACTIONCode(unsigned short  cmdCode);
	string TLV_info(unsigned int tlv_type , unsigned int tlv_len);
	void print_TLV_V_LoginRequest(TLV_V_LoginRequest * p);
	void  print_TLV_V_PhoneRequest(TLV_V_PhoneInfoRequest * p);
	void  print__TLV_V_StreamDataFormat(TLV_V_StreamDataFormat * p);
	void  print_TLV_V_VideoFrameInfo(TLV_V_VideoFrameInfo * p);
	void  print_TLV_V_DVSInfoRequest(TLV_V_DVSInfoRequest * p);
	void  print_TLV_V_VersionInfoRequest(TLV_V_VersionInfoRequest * p);
	void  print_TLV_V_ChannelResponse(TLV_V_ChannelResponse * pp);
	void  print_TLV_V_ControlRequest(TLV_V_ControlRequest * pp);
	void  print_TLV_V_ControlResponse(TLV_V_ControlResponse * pp);
	void  print_TLV_V_ChannelRequest(TLV_V_ChannelRequest * pp);

	//////////////////////////////////////////////////////////////////////////
	RequestQueue fRequestsAwaitingConnection;

private:
	string m_strDevSn;

	//发送缓冲区
	unsigned char  m_vPkgData[1500];		//MTU 1500  1440
	int	m_vPkgLen;

	/* 解析来自服务器的 OWSP 数据包 */
	unsigned int  m_nStatus;	
	unsigned int  m_nPacketSeq;
	unsigned int  m_nPacketLeft;
	unsigned int  m_nTlvType;
	unsigned int  m_nTlvLen;

	responseHandler*  m_pHandler;

};
