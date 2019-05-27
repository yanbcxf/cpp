// OwspClient.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "OwspLoginCommand.h"
#include "CIpmsRegisterCmd.h"

#include<iostream>  
#include<fstream> 

typedef struct  _OwspAVdata
{	
	int sid;
	u_int16 tlv_type;
	u_int16 tlv_len;
	unsigned char * tlv_v;
	int frmType;
	u_int32 time;
	bool isPacketEnd;
}OwspAVdata;				//音视频数据

JMutex				  frmBufMutx_;   //缓冲数据的所
vector<OwspAVdata>	  frmBuf_;		//缓存客户端送过来的数据

void OwspCallback(string strClientName, unsigned char * pData, int dataLength)
{
	JMutexAutoLock lock(frmBufMutx_);

	OwspAVdata dataInfo;
	dataInfo.tlv_v = (unsigned char *)malloc(dataLength);
	if(dataInfo.tlv_v)
	{
		dataInfo.tlv_len = dataLength;
		memcpy(dataInfo.tlv_v, pData, dataLength);
		frmBuf_.push_back(dataInfo);
	}
}

//Bind端口
SOCKET UdpBindPort(unsigned short unBindPort)
{
	//设置绑定方式
	SOCKET m_Socket;
	struct sockaddr_in ServerAddr;
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(unBindPort); ///监听端口
	ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY); ///本机

	m_Socket = socket(AF_INET, SOCK_DGRAM, 0);
	BOOL bBroadcast = TRUE;
	setsockopt(m_Socket, SOL_SOCKET, SO_BROADCAST, (const char*)&bBroadcast, sizeof(BOOL));
	if (SOCKET_ERROR == bind(m_Socket, (struct sockaddr*)&ServerAddr, sizeof(ServerAddr)))
	{
		return NULL;
	}
	return m_Socket;
}


bool UdpReceive(SOCKET m_Socket, char* szReceive, DWORD dwReceiveSize)
{
	//非阻塞消息select使用
	fd_set fdread;
	//初始化fd_set
	FD_ZERO(&fdread);
	//分配套接字句柄到相应的fd_set
	FD_SET(m_Socket, &fdread);
	select(0, &fdread, NULL, NULL, NULL);
	if (FD_ISSET(m_Socket, &fdread))
	{
		//int len = sizeof(m_ServerAddr);
		if (SOCKET_ERROR == recvfrom(m_Socket, szReceive, dwReceiveSize, 0, 0, 0
			/*(struct sockaddr*)&m_ServerAddr, &len*/ ))
		{
			return false;
		}
		unsigned short* pUnBuff = (unsigned short*)szReceive;
		//cout << "CUDPClient::Receive:" << inet_ntoa(m_ServerAddr.sin_addr) << "-" << pUnBuff[0] << "-" << pUnBuff[1] << endl;
	}
	return true;
}

void testJson()
{
	string  strData = "{"
		"\"CmdType\": \"DevConfigIssue\", "
		"\"UploadCycle\" : {"
		"\"Upload\": 10,"
		"\"Fre\" : 20,"
		"\"ACEnable\" : 0"
		"},"
		"\"ETH\" : {"
		"\"DHCP\": 0,"
		"\"IpAddr\" : \"192.168.1.100\","
		"\"GateWay\" : \"192.168.1.1\","
		"\"NetMask\" : \"255.255.255.0\","
		"\"DNS1\" : \"8.8.8.8\","
		"\"DNS2\" : \"8.8.4.4\" "
		"},"
		"\"PortEle\": ["
		"{"
		"\"PortNum\": 1,"
		"\"CollectEnable\" : 0"
		"},"
		"{"
		"\"PortNum\": 2,"
		"\"CollectEnable\" : 1"
		"}"
		"],"
		"\"Dev\": \"121\" "
		"}";

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

				}
			}

		}

		json_result = jsonRoot["UploadCycle"];
		if (json_result.type() == Json::objectValue)
		{
			int nUpload = json_result["Upload"].asInt();
			int nFre = json_result["Fre"].asInt();
			int nACEnable = json_result["ACEnable"].asInt();
		}

		json_result = jsonRoot["ETH"];
		if (json_result.type() == Json::objectValue)
		{
			int nRelay1State = json_result["DHCP"].asInt();
			string IpAddr = json_result["IpAddr"].asString();
			string GateWay = json_result["GateWay"].asString();
			string NetMask = json_result["NetMask"].asString();
			string DNS1 = json_result["DNS1"].asString();
			string DNS2 = json_result["DNS2"].asString();

		}

		string Dev = jsonRoot["Dev"].asString();
	}
}


int _tmain(int argc, _TCHAR* argv[])
{
	WORD ver;
	WSADATA data;

	ver = MAKEWORD(2, 0);
	int ret = WSAStartup(ver, &data);
	if (ret < 0)
	{
		fprintf(stderr, "Init socket failed!n");
		getchar();
		return -1;
	}

	PropertyConfigurator::doConfigure("log4cplus.properties");
	g_Logger = Logger::getInstance("Global");

	UsageEnvironment * pUsageEnvironment1 = new UsageEnvironment(1);
	if (pUsageEnvironment1)
		pUsageEnvironment1->SetupIOWorkers();

	UsageEnvironment * pUsageEnvironment2 = new UsageEnvironment(2);
	if (pUsageEnvironment2)
		pUsageEnvironment2->SetupIOWorkers();

	

	/*睡眠等待完成端口初始化结束*/
	Sleep(3000);

	string buff;
	ifstream infile;
	infile.open("ipms.ini");

	if (!infile)
		return -1;

	int loop = 1;
	while (getline(infile, buff))
	{
		int nResult = -1;

		vector<string>	vecStr;
		splitString(buff, ",", vecStr);
		if (vecStr.size() < 2)
			continue;

		UsageEnvironment * pEnvir = NULL;
		if (loop % 2)
			pEnvir = pUsageEnvironment1;
		else
			pEnvir = pUsageEnvironment2;

		AbstractCommand * pCommand = new CIpmsRegisterCmd(pEnvir, "10.18.73.236", 8002, vecStr[0], atoi(vecStr[1].c_str()));
		if (pCommand)
		{
			nResult = pCommand->SyncCall();
			delete pCommand;
		}

		loop++;
	}

	infile.close();

	while (1)
		Sleep(1);
}

int _tmain_old(int argc, _TCHAR* argv[])
{

	WORD ver;
	WSADATA data;

	ver = MAKEWORD( 2, 0 );
	int ret = WSAStartup( ver, &data );
	if ( ret < 0 )
	{
		fprintf( stderr, "Init socket failed!n" );
		getchar();
		return -1;
	}

	//testJson();
	
	PropertyConfigurator::doConfigure("log4cplus.properties");

	g_Logger = Logger::getInstance("Global");

	ReadCfg();

	g_pUsageEnvironment = new UsageEnvironment(123456);
	if(g_pUsageEnvironment)
		g_pUsageEnvironment->SetupIOWorkers();


	frmBufMutx_.Init();

	/*睡眠等待完成端口初始化结束*/
	Sleep(1000);

	CIOCPBuffer * pBuffer = NULL;
	COwspLoginCommand * pCommand = NULL;
	do 
	{
		pBuffer = g_BufferMgr.AllocateBuffer(IOCommand);
		if(pBuffer==NULL)	
			break;

		pCommand = new COwspLoginCommand(g_pUsageEnvironment, g_strServerIP, g_nServerPort);
		if(pCommand==NULL)
			break;

		BOOL bSuccess = g_pUsageEnvironment->PostQueuedCompletionStatus((BaseClient *)pCommand, pBuffer);
		if ( (!bSuccess && GetLastError( ) != ERROR_IO_PENDING))
			break;

		/*命令投递结束  yangbin */
		// pCommand->CommandWaitingForEnd();


	} while (0);

	if(pCommand)
		delete pCommand;

	if(pBuffer)
		g_BufferMgr.ReleaseBuffer(pBuffer);

	/*保存文件*/

	FILE * fp_video=fopen("to_video.h264","wb");


	vector<OwspAVdata>	frmBuf_tmp;
	OwspAVdata vWrBuf;
	while(1)
	{
		frmBuf_tmp.clear();
		{
			JMutexAutoLock lock(frmBufMutx_);

			while(0 != (frmBuf_.size()))
			{
				vWrBuf = *(frmBuf_.begin());
				frmBuf_.erase(frmBuf_.begin());
				frmBuf_tmp.push_back(vWrBuf);

			}
		}

		vector<OwspAVdata>::iterator  ito = frmBuf_tmp.begin();
		for(; ito != frmBuf_tmp.end(); ito++)
		{
			if(NULL != ito->tlv_v)
			{		
				fwrite(ito->tlv_v,1,ito->tlv_len ,fp_video);

				free(ito->tlv_v);
				ito->tlv_v = NULL;
			}
		}

	}
	

	getchar();
	return 0;
}

