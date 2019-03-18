#include "StdAfx.h"
#include "OwspClient.h"




////////// COwspClient::RequestRecord implementation //////////

COwspClient::RequestRecord::RequestRecord(unsigned cseq,  responseHandler* handler, u_int16	tlv_type, u_int16 tlv_len, unsigned char * tlv_v)
: fNext(NULL), fCSeq(cseq),  f_tlv_type(tlv_type), f_tlv_len(tlv_len), fHandler(handler) 
{
	f_tlv_v = new unsigned char[tlv_len];
	memcpy(f_tlv_v, tlv_v, tlv_len);
}


COwspClient::RequestRecord::~RequestRecord() {
	// Delete the rest of the list first:
	delete fNext;
	if(f_tlv_v)
		delete[] f_tlv_v;
}


////////// RTSPClient::RequestQueue implementation //////////

COwspClient::RequestQueue::RequestQueue()
: fHead(NULL), fTail(NULL) {
}

COwspClient::RequestQueue::RequestQueue(RequestQueue& origQueue)
: fHead(NULL), fTail(NULL) {
	RequestRecord* request;
	while ((request = origQueue.dequeue()) != NULL) {
		enqueue(request);
	}
}

COwspClient::RequestQueue::~RequestQueue() {
	delete fHead;
}

void COwspClient::RequestQueue::enqueue(RequestRecord* request) {
	if (fTail == NULL) {
		fHead = request;
	} else {
		fTail->next() = request;
	}
	fTail = request;
}

COwspClient::RequestRecord* COwspClient::RequestQueue::dequeue() {
	RequestRecord* request = fHead;
	if (fHead == fTail) {
		fHead = NULL;
		fTail = NULL;
	} else {
		fHead = fHead->next();
	}
	if (request != NULL) request->next() = NULL;
	return request;
}

void COwspClient::RequestQueue::putAtHead(RequestRecord* request) {
	request->next() = fHead;
	fHead = request;
	if (fTail == NULL) {
		fTail = request;
	}
}

COwspClient::RequestRecord* COwspClient::RequestQueue::findByCSeq(unsigned cseq) {
	RequestRecord* request;
	for (request = fHead; request != NULL; request = request->next()) {
		if (request->cseq() == cseq) return request;
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////

COwspClient::COwspClient(UsageEnvironment& env, char const* strIPAddr, int nPort, char const * strDevSn)
:BaseClient(env, strIPAddr, nPort)
{
	m_vPkgLen = 0;
	m_nStatus = COwspClient::PACKET_0;
}

COwspClient::~COwspClient(void)
{
}


unsigned COwspClient::sendRequest(RequestRecord* request)
{
	char* cmd = NULL;
	do {
		
		Boolean connectionIsPending = False;
		if (!fRequestsAwaitingConnection.isEmpty()) {
			// A connection is currently pending (with at least one enqueued request).  Enqueue this request also:
			connectionIsPending = True;
		} else if (m_Socket == INVALID_SOCKET) { // we need to open a connection
			int connectResult = openConnection();
			if (connectResult < 0) break; // an error occurred
			else if (connectResult == 0) {
				// A connection is pending
				 connectionIsPending = True;
			} // else the connection succeeded.  Continue sending the command.
		}

		if (connectionIsPending) {
			fRequestsAwaitingConnection.enqueue(request);
			return request->cseq();
		}

		/*产生并且发送命令*/

		unsigned char * pData = NULL;

		m_pHandler = request->handler();

		unsigned int packSize = 4;
		packSize += sizeof(TLV_HEADER);
		packSize += request->f_tlv_len;

		OwspPacketHeader oph;
		oph.packet_seq = request->cseq();
		pData = (unsigned char *)&oph;
		pData[0] = (packSize & 0xff000000)>>24;
		pData[1] = (packSize & 0x00ff0000)>>16;
		pData[2] = (packSize & 0x0000ff00)>>8;
		pData[3] = (packSize & 0x000000ff);
		SendDataByBuffer(pData, sizeof(OwspPacketHeader) ,false);

		TLV_HEADER  th;
		th.tlv_len = request->f_tlv_len;
		th.tlv_type = request->f_tlv_type;
		pData = (unsigned char *)&th;
		SendDataByBuffer(pData,  sizeof(TLV_HEADER) ,false);

		SendDataByBuffer(request->f_tlv_v, request->f_tlv_len ,true);

		return request->cseq();
	} while (0);

	// An error occurred, so call the response handler immediately (indicating the error):
	/*delete[] cmd;
	handleRequestError(request);
	delete request;*/
	return 0;
}


void COwspClient::handleResponseBytes(int newBytesRead)
{
	

	CString str1;
	str1.Format(_T("收到数据，共 %d 字节"), newBytesRead);
	envir().AppendLog(str1);

	bool isContinue = true;

	do {

		unsigned requestSize = 0;
		switch(m_nStatus)
		{
		case COwspClient::PACKET_0:
			if(fResponseBytesAlreadySeen>=sizeof(OwspPacketHeader))
			{

				m_nPacketLeft = fResponseBuffer[0]<<24;
				m_nPacketLeft += fResponseBuffer[1]<<16;
				m_nPacketLeft += fResponseBuffer[2]<<8;
				m_nPacketLeft += fResponseBuffer[3];
				m_nPacketLeft -= 4;

				m_nPacketSeq = fResponseBuffer[7]<<24;
				m_nPacketSeq += fResponseBuffer[6]<<16;
				m_nPacketSeq += fResponseBuffer[5]<<8;
				m_nPacketSeq += fResponseBuffer[4];

				requestSize = sizeof(OwspPacketHeader);
				m_nStatus = COwspClient::TLV_0;

				stringstream ss;
				ss.str("");
				ss << "数据包 【"  <<  m_nPacketSeq << " 】处理开始，";
				ss << "随后所有 TLV 包总长 = " << ( m_nPacketLeft);
				CString str1(ss.str().c_str());
				envir().AppendLog(str1);
			
			}
			else
				isContinue = false;
			break;
		case COwspClient::TLV_0:
			if(fResponseBytesAlreadySeen>=sizeof(TLV_HEADER))
			{
				m_nTlvType  = fResponseBuffer[0];
				m_nTlvType += fResponseBuffer[1]<<8;

				m_nTlvLen  = fResponseBuffer[2];
				m_nTlvLen += fResponseBuffer[3]<<8;

				requestSize = sizeof(TLV_HEADER);
				m_nPacketLeft -= sizeof(TLV_HEADER) ;
				m_nStatus = COwspClient::TLV_1;

				string tlv = TLV_info(m_nTlvType, m_nTlvLen + sizeof(TLV_HEADER));
				CString str1(tlv.c_str());
				envir().AppendLog(str1);
			}
			else
				isContinue = false;
			break;
		case COwspClient::TLV_1:
			if(fResponseBytesAlreadySeen>=m_nTlvLen)
			{
				stringstream ss;

				//	处理 TLV_V 中的数据,  
				if(m_nTlvType ==TLV_T_VIDEO_IFRAME_DATA  )
				{
					//	仅处理 I 帧或 P 帧
					/*if(NULL == fp_video)
					{
						fp_video=fopen("to_video.h264","ab");
					}
					unsigned char * pp = cbTmp.linearize();
					fwrite(pp,1,tlv_len ,fp_video);*/
					if(m_pHandler)
					{
						(*m_pHandler)(name(), fResponseBuffer, m_nTlvLen);
					}
					int www = 2;
				}
				else if(m_nTlvType ==TLV_T_VIDEO_PFRAME_DATA  )
				{
					int www = 1;
					if(m_pHandler)
					{
						(*m_pHandler)(name(), fResponseBuffer, m_nTlvLen);
					}
				}
				else if(m_nTlvType==TLV_T_LOGIN_REQUEST)
				{
					print_TLV_V_LoginRequest( (TLV_V_LoginRequest *)fResponseBuffer);

				}
				else if(m_nTlvType==TLV_T_PHONE_INFO_REQUEST)
				{
					print_TLV_V_PhoneRequest( (TLV_V_PhoneInfoRequest *)fResponseBuffer);

				}
				else if(m_nTlvType ==TLV_T_STREAM_FORMAT_INFO )
				{
					print__TLV_V_StreamDataFormat((TLV_V_StreamDataFormat *)  fResponseBuffer);
				}
				else if(m_nTlvType ==TLV_T_VIDEO_FRAME_INFO )
				{
					print_TLV_V_VideoFrameInfo((TLV_V_VideoFrameInfo * )fResponseBuffer);
				}
				else if(m_nTlvType ==TLV_T_VERSION_INFO_REQUEST  )
				{
					print_TLV_V_VersionInfoRequest((TLV_V_VersionInfoRequest * )fResponseBuffer);
				}
				else if(m_nTlvType ==TLV_T_DVS_INFO_REQUEST  )
				{
					print_TLV_V_DVSInfoRequest((TLV_V_DVSInfoRequest * )fResponseBuffer);
				}


				else if(m_nTlvType ==TLV_T_CONTROL_REQUEST  )
				{
					print_TLV_V_ControlRequest((TLV_V_ControlRequest * )fResponseBuffer);
				}
				else if(m_nTlvType == TLV_T_CONTROL_ANSWER )
				{
					print_TLV_V_ControlResponse((TLV_V_ControlResponse * )fResponseBuffer);
				}
				else if(m_nTlvType == TLV_T_CHANNLE_REQUEST)
				{
					print_TLV_V_ChannelRequest((TLV_V_ChannelRequest * )fResponseBuffer);
				}
				else if(m_nTlvType ==TLV_T_CHANNLE_ANSWER  )
				{
					print_TLV_V_ChannelResponse((TLV_V_ChannelResponse * )fResponseBuffer);
				}


				/*删除已处理数据*/
				requestSize = m_nTlvLen;
				m_nPacketLeft -= m_nTlvLen ;

				if(m_nPacketLeft==0)
				{
					m_nStatus = COwspClient::PACKET_0;
					
					ss.str("");
					ss << "数据包 【"  <<  m_nPacketSeq << " 】处理完成";
					CString str1(ss.str().c_str());
					envir().AppendLog(str1);
				}
				else if(m_nPacketLeft>0)
				{
					m_nStatus = COwspClient::TLV_0;
				}
				else 
				{
					ss.str("");
					ss << "数据包 【"  <<  m_nPacketSeq << " 】error";
					CString str1(ss.str().c_str());
					envir().AppendLog(str1);
				}

			}
			else
				isContinue = false;
			break;

		}
	
		// Check whether there are extra bytes remaining in the buffer, after the end of the request (a rare case).
		// If so, move them to the front of our buffer, and keep processing it, because it might be a following, pipelined request.
		
		int numBytesRemaining  =  fResponseBytesAlreadySeen - requestSize;
		resetResponseBuffer(); // to prepare for any subsequent request

		if (numBytesRemaining > 0) {
			memmove(fResponseBuffer, &fResponseBuffer[requestSize], numBytesRemaining);
			fResponseBytesAlreadySeen = numBytesRemaining;
			fResponseBufferBytesLeft = responseBufferSize - numBytesRemaining;
		}

	}while (isContinue);

}

void COwspClient::handleDestoryed()
{
	//	该客户端被销毁时，发出的通知
	CString msg;
	msg.Format(_T("clientName（%s）,设备（%s）"), name(), m_strDevSn.c_str());
	envir().AppendLog(msg);
}

void COwspClient::handleConnected()
{
	/*m_bOnline = true;*/

	RequestQueue tmpRequestQueue(fRequestsAwaitingConnection);
	RequestRecord* request;

	// Find out whether the connection succeeded or failed:
	do {
		
		// Resume sending all pending requests:
		while ((request = tmpRequestQueue.dequeue()) != NULL) {
			sendRequest(request);
		}
		return;
	} while (0);

	//	已经建立连接的， 1 秒后断开
	// envir().scheduleDelayedTask(1000 * 1000, COwspClient::TestTaskFunc, this);
}


void COwspClient::TestTaskFunc(void* clientData)
{
	COwspClient * pClient = (COwspClient * )clientData;
	pClient->Disconnect();

}

void COwspClient::_memcpy(unsigned char *  mm_vPkgData, int mm_vPkgLen, unsigned char * vPkgData,   int curPos, int  vPkgLen)
{
	memcpy(mm_vPkgData + mm_vPkgLen , vPkgData + curPos,  vPkgLen );
}


void COwspClient::SendDataByBuffer(unsigned char * vPkgData, int vPkgLen, bool bMark)
{
	const int TCP_PKG_LEN = 1460;   /*1460;*/
	stringstream ss("");
	string vMsg;
	int	curPos ;

	curPos = 0;
	while(m_vPkgLen + vPkgLen >TCP_PKG_LEN)
	{
		memcpy(m_vPkgData + m_vPkgLen , vPkgData + curPos, TCP_PKG_LEN -m_vPkgLen );
		
		ASend(m_vPkgData, TCP_PKG_LEN);

		vPkgLen = vPkgLen - (TCP_PKG_LEN -m_vPkgLen);
		curPos += TCP_PKG_LEN -m_vPkgLen;
		m_vPkgLen = 0;
	}
	//	帧边界，全部发送
	if(bMark)
	{
		if(m_vPkgLen + vPkgLen > 0)
		{
			memcpy(m_vPkgData + m_vPkgLen , vPkgData + curPos,  vPkgLen );
			ASend(m_vPkgData, m_vPkgLen + vPkgLen);
			m_vPkgLen = 0;
		}
	}
	else
	{
		//memcpy(m_vPkgData + m_vPkgLen , vPkgData + curPos,  vPkgLen );
		_memcpy(m_vPkgData, m_vPkgLen, vPkgData , curPos, vPkgLen);
		m_vPkgLen += vPkgLen;
	}
}

extern void OwspCallback(string strClientName, unsigned char * pData, int dataLength);

unsigned COwspClient::sendLoginCommand() 
{
	TLV_V_LoginRequest  login;
	memset(&login, 0 ,sizeof(TLV_V_LoginRequest));
	login.dataType = 2;
	memcpy(login.userName,/* "8490E91150396"*//*"8416EC1910059"*/  g_strDevsn.c_str(), STR_LEN_32 );			//	设备序列号
	memcpy(login.password, "123456", STR_LEN_16);
	login.channel = g_nChannel;
	login.streamMode = OWSP_STREAM_SUB;

	return sendRequest(new RequestRecord(++envir().m_nSeq, OwspCallback, TLV_T_LOGIN_REQUEST, 
		sizeof(TLV_V_LoginRequest), (unsigned char *)&login));
}


//////////////////////////////////////////////////////////////////////////
//	打印包信息



string COwspClient::TLV_info(unsigned int tlv_type , unsigned int tlv_len)
{
	string vRslt="";
	stringstream ss("");
	ss<<"---------- TLV_info 【 ";
	switch(tlv_type)
	{
	case 39 : ss << " TLV_T_VERSION_INFO_ANSWER ";			break;
	case 40 : ss << " TLV_T_VERSION_INFO_REQUEST ";		break;
	case 41 : ss << " TLV_T_LOGIN_REQUEST ";					  break;
	case 42 : ss << " TLV_T_LOGIN_ANSWER	 ";					break;
	case 43 : ss << " TLV_T_TOTAL_CHANNEL ";						break;
	case 44 : ss << " TLV_T_SENDDATA_REQUEST ";				break;
	case 45 : ss << " TLV_T_SENDDATA_ANSWER	 ";				break;
	case 46 : ss << " TLV_T_TOTAL_CHANEL_ANSWER ";			break;
	case 47 : ss << " TLV_T_SUSPENDSENDDATA_REQUEST ";	break;
	case 48 : ss << " TLV_T_SUSPENDSENDDATA_ANSWER ";	break;
	case 49 : ss << " TLV_T_DEVICE_KEEP_ALIVE ";				break;
	case 50 : ss << " TLV_T_DEVICE_FORCE_EXIT ";				break;
	case 51 : ss << " TLV_T_CONTROL_REQUEST ";					break;
	case 52 : ss << " TLV_T_CONTROL_ANSWER ";					break;
	case 53 : ss << " TLV_T_RECORD_REQUEST ";				  break;
	case 54 : ss << " TLV_T_RECORD_ANSWER ";						break;
	case 55 : ss << " TLV_T_DEVICE_SETTING_REQUEST ";	break;
	case 56 : ss << " TLV_T_DEVICE_SETTING_ANSWER ";		break;
	case 57 : ss << " TLV_T_KEEP_ALIVE_ANSWER ";				break;
	case 58 : ss << " TLV_T_DEVICE_RESET ";						break;
	case 59 : ss << " TLV_T_DEVICE_RESET_ANSWER ";			break;
	case 60 : ss << " TLV_T_ALERT_REQUEST ";     			break;
	case 61 : ss << " TLV_T_ALERT_ANSWER  ";     			break;
	case 62 : ss << " TLV_T_ALERT_SEND_PHOTO ";    		break;
	case 63 : ss << " TLV_T_ALERT_SEND_PHOTO_ANSWER "; break;
	case 64 : ss << " TLV_T_CHANNLE_REQUEST ";		    	break;
	case 65 : ss << " TLV_T_CHANNLE_ANSWER ";					break;
	case 66 : ss << " TLV_T_SUSPEND_CHANNLE_REQUEST ";	break;
	case 67 : ss << " TLV_T_SUSPEND_CHANNLE_ANSWER ";	break;
	case 68 : ss << " TLV_T_VALIDATE_REQUEST ";				break;
	case 69 : ss << " TLV_T_VALIDATE_ANSWER ";					break;
	case 70 : ss << " TLV_T_DVS_INFO_REQUEST ";				break;
	case 71 : ss << " TLV_T_DVS_INFO_ANSWER ";					break;
	case 72 : ss << " TLV_T_PHONE_INFO_REQUEST	 ";		break;
	case 73 : ss << " TLV_T_PHONE_INFO_ANSWER ";				break;

	case 0x61 : ss << " TLV_T_AUDIO_INFO ";	break;
	case 0x62 : ss << " TLV_T_AUDIO_DATA ";				break;
	case 0x63 : ss << " TLV_T_VIDEO_FRAME_INFO ";					break;
	case 0x64 : ss << " TLV_T_VIDEO_IFRAME_DATA ";				break;
	case 0x66 : ss << " TLV_T_VIDEO_PFRAME_DATA ";					break;
	case 0x65 : ss << " TLV_T_VIDEO_FRAME_INFO_EX	 ";		break;
	case 200 :	ss << " TLV_T_STREAM_FORMAT_INFO ";				break;
	default :  ss << "Unknown Error";									break;
	}
	ss<<", "<<	tlv_len;
	ss<<" 】";
	vRslt=ss.str();
	return vRslt;
}


void  COwspClient::print_TLV_V_LoginRequest(TLV_V_LoginRequest * p)
{
	stringstream ss("");
	ss << "-------------------- userName : " << p->userName;
	envir().AppendLog(CString(ss.str().c_str()));

	ss.str("");
	ss << "-------------------- password : " << p->password ;
	envir().AppendLog(CString(ss.str().c_str()));

	ss.str("");
	ss<< "-------------------- deviceId : " << p->deviceId ;
	envir().AppendLog(CString(ss.str().c_str()));

	ss.str("");
	ss<< "-------------------- channel : " << (unsigned short)p->channel ;
	envir().AppendLog(CString(ss.str().c_str()));

}

void COwspClient::print_TLV_V_PhoneRequest(TLV_V_PhoneInfoRequest * p)
{
	stringstream ss("");
	ss << "-------------------- equipmentIdentity : " << p->equipmentIdentity;
	envir().AppendLog(CString(ss.str().c_str()));

	ss.str("");
	ss << "-------------------- equipmentOS : " << p->equipmentOS ;
	envir().AppendLog(CString(ss.str().c_str()));

}

void  COwspClient::print__TLV_V_StreamDataFormat(TLV_V_StreamDataFormat * p)
{
	stringstream ss("");
	ss << "-------------------- videoChannel : " << (unsigned short)p->videoChannel;
	envir().AppendLog(CString(ss.str().c_str()));

	ss.str("");
	ss << "-------------------- audioChannel : " << (unsigned short)p->audioChannel ;
	envir().AppendLog(CString(ss.str().c_str()));

	ss.str("");
	ss<< "-------------------- dataType : " << (unsigned short)p->dataType ;
	envir().AppendLog(CString(ss.str().c_str()));


	ss.str("");
	ss<< "-------------------- videoFormat.codecId : " << p->videoFormat.codecId ;
	envir().AppendLog(CString(ss.str().c_str()));

	ss.str("");
	ss<< "-------------------- videoFormat.bitrate : " << p->videoFormat.bitrate ;
	envir().AppendLog(CString(ss.str().c_str()));

	ss.str("");
	ss<< "-------------------- videoFormat.width : " << p->videoFormat.width ;
	envir().AppendLog(CString(ss.str().c_str()));

	ss.str("");
	ss<< "-------------------- videoFormat.height : " << p->videoFormat.height ;
	envir().AppendLog(CString(ss.str().c_str()));

	ss.str("");
	ss<< "-------------------- videoFormat.framerate : " << (unsigned short)p->videoFormat.framerate ;
	envir().AppendLog(CString(ss.str().c_str()));

	ss.str("");
	ss<< "-------------------- videoFormat.colorDepth : " << (unsigned short)p->videoFormat.colorDepth ;
	envir().AppendLog(CString(ss.str().c_str()));

	ss.str("");

	ss.str("");
	ss<< "-------------------- audioFormat.samplesPerSecond : " << p->audioFormat.samplesPerSecond ;
	envir().AppendLog(CString(ss.str().c_str()));

	ss.str("");
	ss<< "-------------------- audioFormat.bitrate : " << p->audioFormat.bitrate ;
	envir().AppendLog(CString(ss.str().c_str()));

	ss.str("");
	ss<< "-------------------- audioFormat.waveFormat : " << p->audioFormat.waveFormat ;
	envir().AppendLog(CString(ss.str().c_str()));

	ss.str("");
	ss<< "-------------------- audioFormat.channelNumber : " << p->audioFormat.channelNumber ;
	envir().AppendLog(CString(ss.str().c_str()));

	ss.str("");
	ss<< "-------------------- audioFormat.blockAlign : " << p->audioFormat.blockAlign ;
	envir().AppendLog(CString(ss.str().c_str()));

	ss.str("");
	ss<< "-------------------- audioFormat.bitsPerSample : " << p->audioFormat.bitsPerSample ;
	envir().AppendLog(CString(ss.str().c_str()));

	ss.str("");
	ss<< "-------------------- audioFormat.frameInterval : " << p->audioFormat.frameInterval ;
	envir().AppendLog(CString(ss.str().c_str()));

}

void COwspClient::print_TLV_V_VideoFrameInfo(TLV_V_VideoFrameInfo * p)
{
	stringstream ss("");
	ss << "-------------------- channelId : " << (unsigned short)p->channelId;
	envir().AppendLog(CString(ss.str().c_str()));

	ss.str("");
	ss << "-------------------- checksum : " << p->checksum ;
	envir().AppendLog(CString(ss.str().c_str()));

	ss.str("");
	ss<< "-------------------- frameIndex : " << p->frameIndex ;
	envir().AppendLog(CString(ss.str().c_str()));

	ss.str("");
	ss<< "-------------------- time : " << p->time ;
	envir().AppendLog(CString(ss.str().c_str()));

}

void  COwspClient::print_TLV_V_DVSInfoRequest(TLV_V_DVSInfoRequest * p)
{
	stringstream ss("");
	ss << "-------------------- companyIdentity : " << p->companyIdentity;
	envir().AppendLog(CString(ss.str().c_str()));

	ss.str("");
	ss << "-------------------- equipmentIdentity : " << p->equipmentIdentity ;
	envir().AppendLog(CString(ss.str().c_str()));

	ss.str("");
	ss<< "-------------------- equipmentName : " << p->equipmentName ;
	envir().AppendLog(CString(ss.str().c_str()));

	ss.str("");
	ss<< "-------------------- equipmentVersion : " << p->equipmentVersion ;
	envir().AppendLog(CString(ss.str().c_str()));

	ss.str("");
	ss << "-------------------- channleNumber : " << (unsigned short)p->channleNumber;
	envir().AppendLog(CString(ss.str().c_str()));

	ss.str("");
	ss << "-------------------- equipmentDate : " << (unsigned short)p->equipmentDate.m_year ;
	ss << "-" <<  (unsigned short)p->equipmentDate.m_month << "-" << (unsigned short)p->equipmentDate.m_day;
	envir().AppendLog(CString(ss.str().c_str()));

}

void  COwspClient::print_TLV_V_VersionInfoRequest(TLV_V_VersionInfoRequest * p)
{
	stringstream ss("");
	ss << "-------------------- versionMajor : " << p->versionMajor;
	envir().AppendLog(CString(ss.str().c_str()));

	ss.str("");
	ss << "-------------------- versionMinor : " << p->versionMinor ;
	envir().AppendLog(CString(ss.str().c_str()));

}

void  COwspClient::print_TLV_V_ChannelResponse(TLV_V_ChannelResponse * p)
{
	stringstream ss("");
	ss << "-------------------- result : " << p->result;
	envir().AppendLog(CString(ss.str().c_str()));

	ss.str("");
	ss << "-------------------- currentChannel : " << (unsigned short)p->currentChannel ;
	envir().AppendLog(CString(ss.str().c_str()));

}

string COwspClient::format_OWSP_ACTIONCode(unsigned short  cmdCode)
{
	stringstream ss;
	string vRslt;
	ss<< " ( ";
	switch(cmdCode)
	{
	case OWSP_ACTION_MD_STOP :   ss <<"OWSP_ACTION_MD_STOP" ;  		break;
	case OWSP_ACTION_ZOOMReduce :   ss <<"OWSP_ACTION_ZOOMReduce" ;  		break;
	case OWSP_ACTION_ZOOMADD :   ss <<"OWSP_ACTION_ZOOMADD" ;  		break;
	case OWSP_ACTION_FOCUSADD :   ss <<"OWSP_ACTION_FOCUSADD" ;  		break;
	case OWSP_ACTION_FOCUSReduce :   ss <<"OWSP_ACTION_FOCUSReduce" ;  		break;
	case OWSP_ACTION_MD_UP :   ss <<"OWSP_ACTION_MD_UP" ;  		break;
	case OWSP_ACTION_MD_DOWN :   ss <<"OWSP_ACTION_MD_DOWN" ;  		break;
	case OWSP_ACTION_MD_LEFT :   ss <<"OWSP_ACTION_MD_LEFT" ;  		break;
	case OWSP_ACTION_MD_RIGHT :   ss <<"OWSP_ACTION_MD_RIGHT" ;  		break;
	case OWSP_ACTION_Circle_Add :   ss <<"OWSP_ACTION_Circle_Add" ;  		break;
	case OWSP_ACTION_Circle_Reduce  :   ss <<"OWSP_ACTION_Circle_Reduce" ;  		break;
	case OWSP_ACTION_AUTO_CRUISE  :   ss <<"OWSP_ACTION_AUTO_CRUISE" ;  		break;
	case OWSP_ACTION_GOTO_PRESET_POSITION  :   ss <<"OWSP_ACTION_GOTO_PRESET_POSITION" ;  		break;
	case OWSP_ACTION_SET_PRESET_POSITION  :   ss <<"OWSP_ACTION_SET_PRESET_POSITION" ;  		break;
	case OWSP_ACTION_CLEAR_PRESET_POSITION  :   ss <<"OWSP_ACTION_CLEAR_PRESET_POSITION" ;  		break;
	case OWSP_ACTION_ACTION_RESET  :   ss <<"OWSP_ACTION_ACTION_RESET" ;  		break;

	case OWSP_ACTION_TV_SWITCH  :   ss <<"OWSP_ACTION_TV_SWITCH" ;  		break; 
	case OWSP_ACTION_TV_TUNER  :   ss <<"OWSP_ACTION_TV_TUNER" ;  		break;
	case OWSP_ACTION_TV_SET_QUALITY  :   ss <<"OWSP_ACTION_TV_SET_QUALITY" ;  		break;
	default: ss << "未知操作" ; break;
	}

	ss<< " ) ";
	vRslt=ss.str();
	return vRslt;
}

void  COwspClient::print_TLV_V_ControlRequest(TLV_V_ControlRequest * p)
{
	stringstream ss("");
	ss << "-------------------- deviceId : " << p->deviceId;
	envir().AppendLog(CString(ss.str().c_str()));

	ss.str("");
	ss << "-------------------- channel : " << (unsigned short)p->channel ;
	envir().AppendLog(CString(ss.str().c_str()));

	ss.str("");
	ss << "-------------------- cmdCode : " << (unsigned short)p->cmdCode ;
	ss << format_OWSP_ACTIONCode(p->cmdCode);
	envir().AppendLog(CString(ss.str().c_str()));

	ss.str("");
	ss << "-------------------- size : " << (unsigned short)p->size ;
	envir().AppendLog(CString(ss.str().c_str()));

}

void  COwspClient::print_TLV_V_ControlResponse(TLV_V_ControlResponse * p)
{
	stringstream ss("");
	ss << "-------------------- result : " << p->result;
	envir().AppendLog(CString(ss.str().c_str()));
}

void  COwspClient::print_TLV_V_ChannelRequest(TLV_V_ChannelRequest * p)
{
	stringstream ss("");
	ss << "-------------------- deviceId : " << p->deviceId;
	envir().AppendLog(CString(ss.str().c_str()));
	ss.str("");
	ss << "-------------------- sourceChannel : " << (unsigned short)p->sourceChannel ;
	envir().AppendLog(CString(ss.str().c_str()));

	ss.str("");
	ss << "-------------------- destChannel : " << (unsigned short)p->destChannel ;
	envir().AppendLog(CString(ss.str().c_str()));

}