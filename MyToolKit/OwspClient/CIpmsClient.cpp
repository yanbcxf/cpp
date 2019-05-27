#include "StdAfx.h"
#include "CIpmsClient.h"

//////////////////////////////////////////////////////////////////////////
////////// CIpmsClient::RequestRecord implementation //////////

CIpmsClient::RequestRecord::RequestRecord(unsigned cseq, responseHandler* handler, void * pUserContext, char * url,
	u_int32 body_len, unsigned char * body_data)
	: fNext(NULL), fCSeq(cseq), fHandler(handler), fUserContext(pUserContext)
{
	f_body_len = body_len;
	f_body_data = new unsigned char[f_body_len];
	memcpy(f_body_data, body_data, f_body_len);

	memset(f_url, 0, 256);
	sprintf_s(f_url, 256, "%s", url);

	gettimeofday(&f_tvSubmitTime, NULL);
}


CIpmsClient::RequestRecord::~RequestRecord() {
	// Delete the rest of the list first:
	delete fNext;
	if (f_body_data)
		delete[] f_body_data;
}


////////// CIpmsClient::RequestQueue implementation //////////

CIpmsClient::RequestQueue::RequestQueue()
	: fHead(NULL), fTail(NULL) {
}

CIpmsClient::RequestQueue::RequestQueue(RequestQueue& origQueue)
	: fHead(NULL), fTail(NULL) {
	RequestRecord* request;
	while ((request = origQueue.dequeue()) != NULL) {
		enqueue(request);
	}
}

CIpmsClient::RequestQueue::~RequestQueue() {
	delete fHead;
}

void CIpmsClient::RequestQueue::enqueue(RequestRecord* request) {
	if (fTail == NULL) {
		fHead = request;
	}
	else {
		fTail->next() = request;
	}
	fTail = request;
}

CIpmsClient::RequestRecord* CIpmsClient::RequestQueue::dequeue() {
	RequestRecord* request = fHead;
	if (fHead == fTail) {
		fHead = NULL;
		fTail = NULL;
	}
	else {
		fHead = fHead->next();
	}
	if (request != NULL) request->next() = NULL;
	return request;
}

void CIpmsClient::RequestQueue::putAtHead(RequestRecord* request) {
	request->next() = fHead;
	fHead = request;
	if (fTail == NULL) {
		fTail = request;
	}
}

CIpmsClient::RequestRecord* CIpmsClient::RequestQueue::findByCSeq(unsigned cseq) {
	RequestRecord* request;
	for (request = fHead; request != NULL; request = request->next()) {
		if (request->cseq() == cseq) return request;
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////

CIpmsClient::CIpmsClient(UsageEnvironment& env, string strIPAddr, int nPort, string strDeviceSN, bool bControlBox)
	:BaseClient(env, strIPAddr.c_str(), nPort)
{
	m_vPkgLen = 0;
	m_nStatus = CIpmsClient::PACKET_0;

	DeviceSN = strDeviceSN;
	isControlBox = bControlBox;

	DeviceType = 1;
	DeviceName = "SN_" + strDeviceSN;
	DeviceMac = strDeviceSN;

	SessionID = "";

	Upload = 5;
	Fre = 5;
	ACEnable = 1;
	DHCP = 0;
	IpAddr = "192.168.1.100";
	GateWay = "192.168.1.1";
	NetMask = "255.255.255.0";
	DNS1 = "8.8.8.8";
	DNS2 = "8.8.8.4";
	PortNum = 16;
	
	for (int i = 0; i < PortNum; i++)
	{
		CollectEnable[i] = 1;
		PortState[i] = 1;
		PortAC[i] = 200;
	}
	Dev = "000";		//	负载设备属性

	FanLevel = 2;
	Open1 = 1;
	Open2 = 1;
	Relay1State = 1;
	Relay2State = 1;

	AC = 500;
	
	/*启动下一轮的超时检查*/
	//m_TimeoutTask = envir().scheduleDelayedTask(10 * 1000 * 1000, CIpmsClient::CheckTimeOuntFunc, this);
	m_TimeoutTask = NULL;
	m_HeartbeatTask = NULL;
}


CIpmsClient::~CIpmsClient()
{
	envir().unscheduleDelayedTask(m_TimeoutTask);
	envir().unscheduleDelayedTask(m_HeartbeatTask);

	/* 服务器连接中断 */
	RequestRecord* request;
	if (fRequestsAwaitingResponse.isEmpty() == false)
	{
		while ((request = fRequestsAwaitingResponse.dequeue()) != NULL)
		{
			if (request->handler() != NULL)
			{
				int resultCode = RESULT_CODE_SERVER_CONNECTION_ERR;
				(*request->handler())(request->userContext(), resultCode, NULL, 0);
			}
			delete request;
		}
	}

	/*服务器连接不成功*/
	if (fRequestsAwaitingConnection.isEmpty() == false)
	{
		while ((request = fRequestsAwaitingConnection.dequeue()) != NULL)
		{
			if (request->handler() != NULL)
			{
				int resultCode = RESULT_CODE_SERVER_CONNECTION_ERR;
				(*request->handler())(request->userContext(), resultCode, NULL, 0);
			}
			delete request;
		}
	}
}


void CIpmsClient::_memcpy(unsigned char *  mm_vPkgData, int mm_vPkgLen, unsigned char * vPkgData, int curPos, int  vPkgLen)
{
	memcpy(mm_vPkgData + mm_vPkgLen, vPkgData + curPos, vPkgLen);
}


void CIpmsClient::SendDataByBuffer(unsigned char * vPkgData, int vPkgLen, bool bMark)
{
	const int TCP_PKG_LEN = 1460;   /*1460;*/
	stringstream ss("");
	string vMsg;
	int	curPos;

	curPos = 0;
	while (m_vPkgLen + vPkgLen >TCP_PKG_LEN)
	{
		memcpy(m_vPkgData + m_vPkgLen, vPkgData + curPos, TCP_PKG_LEN - m_vPkgLen);

		ASend(m_vPkgData, TCP_PKG_LEN);

		vPkgLen = vPkgLen - (TCP_PKG_LEN - m_vPkgLen);
		curPos += TCP_PKG_LEN - m_vPkgLen;
		m_vPkgLen = 0;
	}
	//	帧边界，全部发送
	if (bMark)
	{
		if (m_vPkgLen + vPkgLen > 0)
		{
			memcpy(m_vPkgData + m_vPkgLen, vPkgData + curPos, vPkgLen);
			ASend(m_vPkgData, m_vPkgLen + vPkgLen);
			m_vPkgLen = 0;
		}
	}
	else
	{
		//memcpy(m_vPkgData + m_vPkgLen , vPkgData + curPos,  vPkgLen );
		_memcpy(m_vPkgData, m_vPkgLen, vPkgData, curPos, vPkgLen);
		m_vPkgLen += vPkgLen;
	}
}

void CIpmsClient::handleResponseBytes(int newBytesRead)
{

	CString str1;
	str1.Format("收到数据，共 %d 字节", newBytesRead);
	envir().AppendLog(str1);


	bool isContinue = true;

	do {

		unsigned requestSize = 0;

		switch (m_nStatus)
		{
		case CIpmsClient::PACKET_0:
		{
			// Data was read OK.  Look through the data that we've read so far, to see if it contains <CR><LF><CR><LF>.
			// (If not, wait for more data to arrive.)
			Boolean endOfHeaders = False;
			unsigned char * ptr = fResponseBuffer;
			if (fResponseBytesAlreadySeen > 3) {
				unsigned char * const ptrEnd = &fResponseBuffer[fResponseBytesAlreadySeen - 3];
				while (ptr < ptrEnd) {
					if (*ptr++ == '\r' && *ptr++ == '\n' && *ptr++ == '\r' && *ptr++ == '\n') {
						// This is it
						endOfHeaders = True;
						break;
					}
				}
			}

			if (endOfHeaders)
			{
				requestSize = ptr - fResponseBuffer;
				m_strHeader = string(fResponseBuffer, ptr);

				//	获取 Cookie
				stringstream ss;
				ss << m_strHeader;
				char buffer[100];
				m_strCookie = "";
				while (ss.getline(buffer, sizeof(buffer)))
				{
					string str = string(buffer);
					if (str.find("Cookie") != string::npos)
						m_strCookie = string(buffer);
				}

				//	确定后面实体的长度
				string::size_type pos = m_strHeader.find("GET");
				if (pos != string::npos)
				{
					m_nContenLength = 0;
					m_nStatus = CIpmsClient::PACKET_DATA;
				}
				else
				{
					string::size_type pos1 = m_strHeader.find("Content-Length");
					if (pos1 != string::npos)
					{
						string strTmp = m_strHeader.substr(pos1);
						sscanf_s(strTmp.c_str(), "Content-Length:%d", &m_nContenLength);
						m_nStatus = CIpmsClient::PACKET_DATA;
					}
					else
						isContinue = false;
				}				
			}
			else
				isContinue = false;
		}
		break;

		case CIpmsClient::PACKET_DATA:
		{
			if (fResponseBytesAlreadySeen >= m_nContenLength)
			{
				m_nStatus = CIpmsClient::PACKET_0;
				requestSize = m_nContenLength;

				char strTmp[512 + 1] = { 0 };
				memcpy(strTmp, fResponseBuffer, m_nContenLength>512 ? 512 : m_nContenLength);

				string strData = string(strTmp);
				/*vector<string> vecResult;
				splitString(strData, "&", vecResult);*/

				int nSeq; // = ParseIntProperty(vecResult, "seq");

				if (m_strHeader.find("POST") != string::npos || m_strHeader.find("GET") != string::npos)
				{
					if (m_strHeader.find("/DevConfigReq") != string::npos)
					{
						//	配置信息请求
						char json[MAX_JSON] = { 0 };
						sprintf_s(json, 
							"{ \"CmdType\": \"DevConfigReq\","
							"\"SessionID\": \"%s\","
							"\"Status\": 0,"
							"\"UploadCycle\": {"
							"\"Upload\": %d,"
							"\"Fre\": %d,"
							"\"ACEnable\": %d"
							"},"
							" \"ETH\": {"
							"\"DHCP\": %d,"
							"\"IpAddr\": \"%s\","
							"\"GateWay\": \"%s\","
							"\"NetMask\": \"%s\","
							"\"DNS1\": \"%s\","
							"\"DNS2\": \"%s\""
							" },"
							"\"PortEle\": [ ",
							SessionID.c_str(), Upload, Fre, ACEnable, DHCP, IpAddr.c_str(), 
							GateWay.c_str(), NetMask.c_str(), DNS1.c_str(), DNS2.c_str()
						);

						for (int i = 0; i < PortNum; i++)
						{
							char tmp[256] = { 0 };
							if(i+1 == PortNum)
								sprintf_s(tmp, "{ \"PortNum\": %d, \"CollectEnable\": %d }", i + 1, CollectEnable[i]);
							else
								sprintf_s(tmp, "{ \"PortNum\": %d, \"CollectEnable\": %d }, ", i + 1, CollectEnable[i]);
							
							int len = strlen(json);
							sprintf_s(json +len, MAX_JSON - len, "%s", tmp);
						}

						int len = strlen(json);
						sprintf_s(json + len, MAX_JSON - len, "], \"Dev\": \"%s\" ", Dev.c_str());


						unsigned char header[256] = { 0 };
						sprintf_s((char *)header, 256, "HTTP/1.1 200 OK\r\n"
							"Content-Type: Application/json\r\n"
							"Content-Length: %d\r\n\r\n", strlen((char *)json));

						SendDataByBuffer(header, strlen((const char*)header), false);
						SendDataByBuffer((unsigned char *)json, strlen((char *)json), true);
					}
					else if (m_strHeader.find("/DevConfigIssue") != string::npos)
					{
						// 配置信息下发
						Json::Value jsonRoot;
						Json::Reader reader(Json::Features::strictMode());
						bool parsingSuccessful = reader.parse(strData, jsonRoot);
						if (!parsingSuccessful) {

						}
						else
						{
							Json::Value json_result = jsonRoot["PortEle"];
							if (json_result.type() == Json::arrayValue)
							{
								int size = json_result.size();
								if (size>0)
								{
									for (int index = 0; index< size; index++)
									{
										Json::Value  struction = json_result[index];

										int nPortNum = struction["PortNum"].asInt();
										int nCollectEnable = struction["CollectEnable"].asInt();

										if (nPortNum > 1 && nPortNum < PortNum)
											CollectEnable[nPortNum - 1] = nCollectEnable;
									}
								}

							}

							json_result = jsonRoot["UploadCycle"];
							if (json_result.type() == Json::objectValue)
							{
								Upload = json_result["Upload"].asInt();
								Fre = json_result["Fre"].asInt();
								ACEnable = json_result["ACEnable"].asInt();
							}

							json_result = jsonRoot["ETH"];
							if (json_result.type() == Json::objectValue)
							{
								DHCP = json_result["DHCP"].asInt();
								IpAddr = json_result["IpAddr"].asString();
								GateWay = json_result["GateWay"].asString();
								NetMask = json_result["NetMask"].asString();
								DNS1 = json_result["DNS1"].asString();
								DNS2 = json_result["DNS2"].asString();

							}

							Dev = jsonRoot["Dev"].asString();
						}

						//	回复
						char json[MAX_JSON] = { 0 };
						sprintf_s(json, "{ \"CmdType\": \"DevConfigIssue\",  \"SessionID\": \"%s\", \"Status\":0 }", SessionID.c_str());

						unsigned char header[256] = { 0 };
						sprintf_s((char *)header, 256, "HTTP/1.1 200 OK\r\n"
							"Content-Type: Application/json\r\n"
							"Content-Length: %d\r\n\r\n", strlen((char *)json));

						SendDataByBuffer(header, strlen((const char*)header), false);
						SendDataByBuffer((unsigned char *)json, strlen((char *)json), true);
					}
					else if (m_strHeader.find("/DevStateReq") != string::npos)
					{
						// 状态信息请求
						char json[MAX_JSON] = { 0 };
						sprintf_s(json,
							"{ \"CmdType\": \"DevStateReq\","
							"\"SessionID\": \"%s\","
							"\"Status\": 0,",
							SessionID.c_str()
						);

						if (isControlBox == false)
						{
							//	电箱
							if (PortNum > 0)
							{
								int len = strlen(json);
								sprintf_s(json + len, MAX_JSON - len, "%s", "\"POEPort\": [ ");
								for (int i = 0; i < PortNum; i++)
								{
									char tmp[256] = { 0 };
									if (i + 1 == PortNum)
										sprintf_s(tmp, "{ \"PortNum\": %d, \"PortState\": %d }", i + 1, PortState[i]);
									else
										sprintf_s(tmp, "{ \"PortNum\": %d, \"PortState\": %d }, ", i + 1, PortState[i]);

									int len = strlen(json);
									sprintf_s(json + len, MAX_JSON - len, "%s", tmp);
								}

								len = strlen(json);
								sprintf_s(json + len, MAX_JSON - len, "%s", "] }\r\n");
							}

						}
						else
						{
							/*
								"\"PowerBox\" : { "
								"\"Relay1State\": %d, "
								"\"Relay2State\" : %d } } ",
							*/

							// 控制盒
							int len = strlen(json);
							sprintf_s(json + len, MAX_JSON - len, "\"ControlBox\": { "
								"\"FanLevel\": %d, "
								"\"Open1\" : %d, "
								"\"Open2\" : %d } } \r\n",
								FanLevel, Open1, Open2);
						}

						unsigned char header[256] = { 0 };
						sprintf_s((char *)header, 256, "HTTP/1.1 200 OK\r\n"
							"Content-Type: Application/json\r\n"
							"%s\n"
							"Content-Length: %d\r\n\r\n", m_strCookie.c_str(), strlen((char *)json));

						SendDataByBuffer(header, strlen((const char*)header), false);
						SendDataByBuffer((unsigned char *)json, strlen((char *)json), true);

					}
					else if (m_strHeader.find("/DevControl") != string::npos)
					{
						//	设备控制 S->C
						Json::Value jsonRoot;
						Json::Reader reader(Json::Features::strictMode());
						bool parsingSuccessful = reader.parse(strData, jsonRoot);
						if (!parsingSuccessful) {

						}
						else
						{
							Json::Value json_result = jsonRoot["POEPort"];
							if (json_result.type() == Json::arrayValue)
							{
								int size = json_result.size();
								if (size>0)
								{
									for (int index = 0; index< size; index++)
									{
										Json::Value  struction = json_result[index];

										int nPortNum = struction["PortNum"].asInt();
										int nPortState = struction["PortState"].asInt();

										if (nPortNum > 1 && nPortNum < PortNum)
											PortState[nPortNum - 1] = nPortState;
									}
								}
								
							}

							json_result = jsonRoot["ControlBox"];
							if (json_result.type() == Json::objectValue)
							{
								FanLevel = json_result["FanLevel"].asInt();
								Open1 = json_result["Open1"].asInt();
								Open2 = json_result["Open2"].asInt();
							}

							json_result = jsonRoot["PowerBox"];
							if (json_result.type() == Json::objectValue)
							{
								Relay1State = json_result["Relay1State"].asInt();
								Relay2State = json_result["Relay2State"].asInt();
							}
						}

						// 回复
						char json[MAX_JSON] = { 0 };
						sprintf_s(json, "{ \"CmdType\": \"DevControl\",  \"SessionID\": \"%s\", \"Status\":0 }", SessionID.c_str());

						unsigned char header[256] = { 0 };
						sprintf_s((char *)header, 256, "HTTP/1.1 200 OK\r\n"
							"Content-Type: Application/json\r\n"
							"%s\r\n"
							"Content-Length: %d\r\n\r\n", m_strCookie.c_str(), strlen((char *)json));

						SendDataByBuffer(header, strlen((const char*)header), false);
						SendDataByBuffer((unsigned char *)json, strlen((char *)json), true);
					}
				}
				else if (m_strHeader.find("200 OK") != string::npos || m_strHeader.find("400 Bad") != string::npos)
				{
					string strCmdType = "";
					string strSessionID = "";
					int	   nStatus = -1;

					Json::Value jsonRoot;
					Json::Reader reader(Json::Features::strictMode());
					bool parsingSuccessful = reader.parse(strData, jsonRoot);
					if (!parsingSuccessful) {

					}
					else
					{
						strCmdType = jsonRoot["CmdType"].asString();
						strSessionID = jsonRoot["SessionID"].asString();
						nStatus = jsonRoot["Status"].asInt();
					}

					RequestRecord* foundRequest = NULL;
					/*查找匹配的请求*/
					if (strCmdType=="Register")
					{
						RequestRecord* request;
						RequestQueue tmpRequestQueue;
						while ((request = fRequestsAwaitingResponse.dequeue()) != NULL) {
							foundRequest = request;
						}

						CString str;
						str.Format("Register %s 200 OK", DeviceSN.c_str());
						envir().AppendInfoLog(str);

						/*先前弹出的请求再插入等待队列*/
						while ((request = tmpRequestQueue.dequeue()) != NULL)
						{
							fRequestsAwaitingResponse.enqueue(request);
						}
					}
					else if (strCmdType == "KeepAlive")
					{
						/*若为心跳的回复，则定时开启后续心跳*/
						// yangbin 以下语句使用错误，会导致 DelayQueue 队列不断增长，应该采用 rescheduleDelayedTask
						//m_HeartbeatTask = envir().scheduleDelayedTask(5000 * 1000, CIpmsClient::TestHeartBeatFunc, this);
						envir().rescheduleDelayedTask(m_HeartbeatTask, 5000 * 1000, CIpmsClient::TestHeartBeatFunc, this);
					}

					if (foundRequest)
					{
						// 命令请求的回调处理
						if (foundRequest->handler() != NULL)
						{
							int resultCode;
							if (m_strHeader.find("OK") != string::npos) {

								SessionID = strSessionID;

								resultCode = RESULT_CODE_OK;
								(*foundRequest->handler())(foundRequest->userContext(), resultCode, fResponseBuffer, requestSize);
							}
							else {
								// An error occurred parsing the response, so call the handler, indicating an error:
								resultCode = RESULT_CODE_SERVER_ERR;
								(*foundRequest->handler())(foundRequest->userContext(), resultCode, fResponseBuffer, requestSize);
							}
						}

						if (1)
						{
							/*若为登录的回复，则定时开启后续心跳*/
							m_HeartbeatTask = envir().scheduleDelayedTask(5000 * 1000, CIpmsClient::TestHeartBeatFunc, this);

						}

						delete foundRequest;
					}
				}

			}
			else
				isContinue = false;
		}
		break;

		}


		// Check whether there are extra bytes remaining in the buffer, after the end of the request (a rare case).
		// If so, move them to the front of our buffer, and keep processing it, because it might be a following, pipelined request.

		int numBytesRemaining = fResponseBytesAlreadySeen - requestSize;
		resetResponseBuffer(); // to prepare for any subsequent request

		if (numBytesRemaining > 0) {
			memmove(fResponseBuffer, &fResponseBuffer[requestSize], numBytesRemaining);
			fResponseBytesAlreadySeen = numBytesRemaining;
			fResponseBufferBytesLeft = responseBufferSize - numBytesRemaining;
		}

	} while (isContinue);

}


void CIpmsClient::handleConnected()
{
	/*m_bOnline = true;*/

	CString str;
	str.Format("Register %s handleConnected", DeviceSN.c_str());
	envir().AppendInfoLog(str);

	RequestQueue tmpRequestQueue(fRequestsAwaitingConnection);
	RequestRecord* request;

	// Find out whether the connection succeeded or failed:
	do {

		// Resume sending all pending requests:
		while ((request = tmpRequestQueue.dequeue()) != NULL) {
			sendRequest(request);
		}
	} while (0);

	//	已经建立连接的， 1 秒后断开
	// envir().scheduleDelayedTask(1000 * 1000, COwspClient::TestTaskFunc, this);

}


void CIpmsClient::handleDestoryed()
{
	//	该客户端被销毁时，发出的通知


	CString msg;
	msg.Format("clientName（%s）,设备（%s）", name(), DeviceSN.c_str());
	envir().AppendLog(msg);

}



unsigned CIpmsClient::sendRequest(RequestRecord* request)
{
	char* cmd = NULL;
	do {
		Boolean connectionIsPending = False;
		if (!fRequestsAwaitingConnection.isEmpty()) {
			// A connection is currently pending (with at least one enqueued request).  Enqueue this request also:
			connectionIsPending = True;
		}
		else if (m_Socket == INVALID_SOCKET) { // we need to open a connection
			int connectResult = openConnection();
			if (connectResult < 0) break; // an error occurred
			else if (connectResult == 0) {
				// A connection is pending
				connectionIsPending = True;
			} // else the connection succeeded.  Continue sending the command.
		}

		if (connectionIsPending) {
			fRequestsAwaitingConnection.enqueue(request);

			CString str;
			str.Format("Register %s Pending", DeviceSN.c_str());
			envir().AppendInfoLog(str);
			return request->cseq();
		}

		CString str;
		str.Format("Register %s connection succeeded", DeviceSN.c_str());
		envir().AppendInfoLog(str);

		/*产生并且发送命令*/
		m_pHandler = request->handler();

		unsigned char header[1024] = { 0 };
		sprintf_s((char *)header, 1024, "%s HTTP/1.1\r\n"
			"Content-Type: application/json\r\n"
			"Content-Length: %d\r\n\r\n", request->f_url, request->f_body_len);


		SendDataByBuffer(header, strlen((const char*)header), false);
		SendDataByBuffer(request->f_body_data, request->f_body_len, true);

		/*放置到等待回复队列*/
		fRequestsAwaitingResponse.enqueue(request);

		return request->cseq();
	} while (0);

	// An error occurred, so call the response handler immediately (indicating the error):
	/*delete[] cmd;
	handleRequestError(request);
	delete request;*/
	return RESULT_CODE_LOCAL_ERR;
}

void CIpmsClient::TestTaskFunc(void* clientData)
{
	CIpmsClient * pClient = (CIpmsClient *)clientData;
	pClient->Disconnect();
}

void CIpmsClient::TestHeartBeatFunc(void* clientData)
{
	CIpmsClient * pClient = (CIpmsClient *)clientData;
	pClient->sendKeepAlive();
	pClient->sendEleReport();
}

void CIpmsClient::CheckTimeOuntFunc(void* clientData)
{
	CIpmsClient * pClient = (CIpmsClient *)clientData;
	pClient->TimeoutProcess();

}

void CIpmsClient::TimeoutProcess()
{
	RequestRecord* request;
	RequestQueue tmpRequestQueue;
	if (fRequestsAwaitingResponse.isEmpty() == false)
	{
		while ((request = fRequestsAwaitingResponse.dequeue()) != NULL)
		{
			struct timeval tvNow;
			gettimeofday(&tvNow, NULL);

			long delay = (tvNow.tv_sec * 1000 + tvNow.tv_usec / 1000) -
				(request->f_tvSubmitTime.tv_sec * 1000 + request->f_tvSubmitTime.tv_usec / 1000);

			if (delay> 3600 * 1000)
			{
				/*该请求超时，则终止命令执行，直接回复该客户端上层*/

				if (request->handler() != NULL)
				{
					int resultCode = RESULT_CODE_SERVER_TIMEOUT;
					(*request->handler())(request->userContext(), resultCode, NULL, 0);
				}
				delete request;
			}
			else
			{
				tmpRequestQueue.enqueue(request);
			}
		}

		/*先前弹出的请求再插入等待队列*/
		while ((request = tmpRequestQueue.dequeue()) != NULL)
		{
			fRequestsAwaitingResponse.enqueue(request);
		}
	}

	/*  yangbin 2017-11-14 ,启动下一轮的超时检查， 使用错误会导致 DelayQueue 不断增长，应该采用  */
	// m_TimeoutTask = envir().scheduleDelayedTask(1000 * 1000, CIpmsClient::CheckTimeOuntFunc, this);
	envir().rescheduleDelayedTask(m_TimeoutTask, 30 * 1000 * 1000, CIpmsClient::CheckTimeOuntFunc, this);
}

//////////////////////////////////////////////////////////////////////////////

unsigned CIpmsClient::sendRegisterCommand(void * pUserContext)
{
	int nSeq = envir().GetSeq();

	CString url("POST /Register");

	char json[MAX_JSON] = { 0 };
	sprintf_s(json, "{"
		"\"CmdType\": \"Register\","
		"\"DeviceSN\" : \"%s\", "
		"\"DeviceType\": %d,"
		"\"DeviceName\": \"%s\", "
		"\"DeviceMac\": \"%s\", "
		"\"SoftVersion\": \"1.0.0.0\", "
		"\"HardVersion\": \"1.0.0.0\" }", DeviceSN.c_str(), (isControlBox? 1 : 0),
		DeviceName.c_str(), DeviceMac.c_str());

	return sendRequest(new RequestRecord(nSeq, AbstractCommand::ResponseHandler1, pUserContext,
		url.GetBuffer(), strlen(json), (unsigned char *)json));
}

// 心跳 C->S
unsigned CIpmsClient::sendKeepAlive()
{
	int nSeq = envir().GetSeq();

	char json[MAX_JSON] = { 0 };
	sprintf_s(json,
		"{ \"CmdType\": \"KeepAlive\","
		"\"Status\": 0 }");

	unsigned char header[256] = { 0 };
	sprintf_s((char *)header, 256, "POST /KeepAlive HTTP/1.1\r\n"
		"Content-Type: Application/json\r\n"
		"Content-Length: %d\r\n\r\n", strlen((char *)json));

	SendDataByBuffer(header, strlen((const char*)header), false);
	SendDataByBuffer((unsigned char *)json, strlen((char *)json), true);

	return nSeq;
}

// 状态信息推送 C->S
unsigned CIpmsClient::sendDevStatePush()
{
	int nSeq = envir().GetSeq();

	char json[MAX_JSON] = { 0 };
	sprintf_s(json,
		"{ \"CmdType\": \"DevStateReq\","
		"\"SessionID\": \"%s\","
		"\"Status\": 0,",
		SessionID.c_str()
	);

	if (isControlBox == false)
	{
		//	电箱
		if (PortNum > 0)
		{
			int len = strlen(json);
			sprintf_s(json + len, MAX_JSON - len, "%s", "\"POEPort\": [ ");
			for (int i = 0; i < PortNum; i++)
			{
				char tmp[256] = { 0 };
				if (i + 1 == PortNum)
					sprintf_s(tmp, "{ \"PortNum\": %d, \"PortState\": %d }", i + 1, PortState[i]);
				else
					sprintf_s(tmp, "{ \"PortNum\": %d, \"PortState\": %d }, ", i + 1, PortState[i]);

				int len = strlen(json);
				sprintf_s(json + len, MAX_JSON - len, "%s", tmp);
			}

			len = strlen(json);
			sprintf_s(json + len, MAX_JSON - len, "%s", "] }");
		}

	}
	else
	{
		/*
		"\"PowerBox\" : { "
		"\"Relay1State\": %d, "
		"\"Relay2State\" : %d } } ",
		*/

		// 控制盒
		int len = strlen(json);
		sprintf_s(json + len, MAX_JSON - len, "\"ControlBox\": { "
			"\"FanLevel\": %d, "
			"\"Open1\" : %d, "
			"\"Open2\" : %d } } ",
			FanLevel, Open1, Open2);
	}


	unsigned char header[256] = { 0 };
	sprintf_s((char *)header, 256, "POST /DevStatePush HTTP/1.1\r\n"
		"Content-Type: Application/json\r\n"
		"Content-Length: %d\r\n\r\n", strlen((char *)json));

	SendDataByBuffer(header, strlen((const char*)header), false);
	SendDataByBuffer((unsigned char *)json, strlen((char *)json), true);

	return nSeq;
}

// 电量上报 C->S
unsigned CIpmsClient::sendEleReport()
{
	int nSeq = envir().GetSeq();

	char json[MAX_JSON] = { 0 };
	sprintf_s(json,
		"{ \"CmdType\": \"EleReport\","
		"\"SessionID\": \"%s\",",
		SessionID.c_str()
	);

	if (isControlBox == false)
	{
		if (PortNum > 0)
		{
			int len = strlen(json);
			sprintf_s(json + len, MAX_JSON - len, "%s", "\"POEPort\": [ ");
			for (int i = 0; i < PortNum; i++)
			{
				char tmp[256] = { 0 };
				if (i + 1 == PortNum)
					sprintf_s(tmp, "{ \"PortNum\": %d, \"PortState\": %d }", i + 1, PortAC[i]);
				else
					sprintf_s(tmp, "{ \"PortNum\": %d, \"PortState\": %d }, ", i + 1, PortAC[i]);

				int len = strlen(json);
				sprintf_s(json + len, MAX_JSON - len, "%s", tmp);
			}

			len = strlen(json);
			sprintf_s(json + len, MAX_JSON - len, "], \"Time\": \"%s\" }", NowTime().c_str());
		}
	}
	else
	{
		int len = strlen(json);
		sprintf_s(json + len, MAX_JSON - len, "\"AC\": %d, \"Time\": \"%s\" }", AC, NowTime().c_str());
	}

	

	unsigned char header[256] = { 0 };
	sprintf_s((char *)header, 256, "POST /EleReport HTTP/1.1\r\n"
		"Content-Type: Application/json\r\n"
		"Content-Length: %d\r\n\r\n", strlen((char *)json));

	SendDataByBuffer(header, strlen((const char*)header), false);
	SendDataByBuffer((unsigned char *)json, strlen((char *)json), true);

	return nSeq;
}


// 离线电量上报 C->S
unsigned CIpmsClient::sendOfflineEleReport()
{
	int nSeq = envir().GetSeq();

	char json[MAX_JSON] = { 0 };
	sprintf_s(json,
		"{ \"CmdType\": \"OfflineEleReport \","
		"\"SessionID\": \"%s\","
		"\"POEPort\": [ ",
		SessionID.c_str()
	);

	for (int i = 0; i < PortNum; i++)
	{
		char tmp[256] = { 0 };
		if (i + 1 == PortNum)
			sprintf_s(tmp, "{ \"PortNum\": %d, \"PortState\": %d }", i + 1, PortAC[i]);
		else
			sprintf_s(tmp, "{ \"PortNum\": %d, \"PortState\": %d }, ", i + 1, PortAC[i]);

		int len = strlen(json);
		sprintf_s(json + len, MAX_JSON - len, "%s", tmp);
	}

	int len = strlen(json);
	sprintf_s(json + len, MAX_JSON - len, "],\"AC\": %d, \"StartTime\": %s, \"EndTime\": %s }", AC, Time, Time);

	unsigned char header[256] = { 0 };
	sprintf_s((char *)header, 256, "POST /OfflineEleReport HTTP/1.1\r\n"
		"Content-Type: Application/json\r\n"
		"Content-Length: %d\r\n\r\n", strlen((char *)json));

	SendDataByBuffer(header, strlen((const char*)header), false);
	SendDataByBuffer((unsigned char *)json, strlen((char *)json), true);

	return nSeq;
}


// 报警上报 C->S
unsigned CIpmsClient::sendDeviceAlarm()
{
	int nSeq = envir().GetSeq();

	char json[MAX_JSON] = { 0 };
	sprintf_s(json, "{"
		"\"CmdType\": \"DeviceAlarm\","
		"\"SessionID\" : \"%s\", "
		"\"AlarmNum\" : 1, "
		"\"AlarmDescription\" : \"描述报警内容的字符串\", "
		"\"AlarmTime\" : \"%s\", "
		"\"AlarmPort\" : 1  }", SessionID.c_str(), NowTime().c_str());

	unsigned char header[256] = { 0 };
	sprintf_s((char *)header, 256, "POST /DeviceAlarm /%s HTTP/1.1\r\n"
		"Content-Type: Application/json\r\n"
		"Content-Length: %d\r\n\r\n", SessionID.c_str(), strlen((char *)json));

	SendDataByBuffer(header, strlen((const char*)header), false);
	SendDataByBuffer((unsigned char *)json, strlen((char *)json), true);

	return nSeq;
}