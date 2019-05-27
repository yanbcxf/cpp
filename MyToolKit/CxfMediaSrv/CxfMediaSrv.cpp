// CxfMediaSrv.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "rtpsession.h"
#include "rtpsessionparams.h"
#include "rtpudpv4transmitter.h"
#include "rtpipv4address.h"
#include "rtptimeutilities.h"
#include "rtppacket.h"

using namespace jrtplib;


#include "UsageEnvironment.h"
#include "GbUsageEnvironment.h"




int _tmain(int argc, _TCHAR* argv[])
{

	// 初始化log4cplus
	PropertyConfigurator::doConfigure("log4cplus.properties");
	g_logger = getDynamicLogger("CxfMediaSrv");

	//////////////////////////////////////////////////////////////////////////
	//	读入 INI 文件

	stringstream ss;
	CHAR theAppPath[MAX_PATH];
	GetModuleFileName(NULL, theAppPath, MAX_PATH );

	string fullpath = theAppPath;
	int ipos = fullpath.find_last_of("\\");

	// 获取上级路径
	fullpath = fullpath.substr(0,ipos);
	fullpath += "\\CxfMediaSrv.ini";

	ss << "fullpath = " << fullpath ;
	//LOG4CPLUS_DEBUG(g_logger, ss.str());
	ReadCfg(fullpath);

	CHAR theCurrPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, theCurrPath); 
	g_strCurrentDir = string(theCurrPath);

	//	启动 Sip 服务
	SIPD_GATEWAY_CFG cfg;
	memset(&cfg, 0, sizeof(SIPD_GATEWAY_CFG));
	strncpy(cfg.local_domain_name, "35010400", MAX_SIPD_DEVICE_ID_LEN);
	strncpy(cfg.local_domain_id, "35010400002020000055", MAX_SIPD_DEVICE_ID_LEN);		//	202 媒体分发服务器
	cfg.local_port = 5066;
	/*strncpy(cfg.local_ip, "192.168.1.105", MAX_SIPD_NORMAL_LEN);
	strncpy(cfg.local_gateway, "192.168.1.1", MAX_SIPD_NORMAL_LEN);*/
	strncpy(cfg.local_ip, g_strLocalIP.c_str(), MAX_SIPD_NORMAL_LEN);
	strncpy(cfg.local_gateway, "10.18.72.254", MAX_SIPD_NORMAL_LEN);

	strncpy(cfg.upper_server_name, "35010400", MAX_SIPD_DEVICE_ID_LEN);
	strncpy(cfg.upper_server_id, "35010400002000000005", MAX_SIPD_DEVICE_ID_LEN);		//	200 中小信令控制服务器
	strncpy(cfg.upper_server_ip, "10.18.72.35", MAX_SIPD_NORMAL_LEN);
	//strncpy(cfg.upper_server_ip, "192.168.1.109", MAX_SIPD_NORMAL_LEN);
	cfg.upper_server_port = 5068;
	strncpy(cfg.register_upper_server_passwd, "12345678", MAX_SIPD_NORMAL_LEN);
	cfg.register_keepalive = 3600;
	cfg.keepalive_internal = 60;

	//	启动 sip 消息处理
	CGbUsageEnvironment::getInstance()->init(cfg);

	sipd_init(&cfg);

	//////////////////////////////////////////////////////////////////////////
	// 启动 RTP 流的转发

	RTPSession session;

	RTPSessionParams sessionparams;
	sessionparams.SetOwnTimestampUnit(1.0/90000.0);

	RTPUDPv4TransmissionParams transparams;
	transparams.SetPortbase( g_iRtpPort /*8000*/);

	int status = session.Create(sessionparams,&transparams);
	if (status < 0)
	{
		//std::cerr << RTPGetErrorString(status) << std::endl;
		exit(-1);
	}

	/*uint8_t localip[]={127,0,0,1};
	RTPIPv4Address addr(localip,9000);

	status = session.AddDestination(addr);
	if (status < 0)
	{
		exit(-1);
	}*/

	session.SetDefaultPayloadType(96);
	session.SetDefaultMark(false);
	session.SetDefaultTimestampIncrement(90000.0 /25.0);

	uint8_t silencebuffer[160];
	for (int i = 0 ; i < 160 ; i++)
		silencebuffer[i] = 128;

	RTPTime delay(0.020);
	RTPTime starttime = RTPTime::CurrentTime();

	//getchar();

	FILE * fp_wlz1 = NULL;
	
	bool done = false;
	while (!done)
	{
		/*status = session.SendPacket(silencebuffer,160);
		if (status < 0)
		{
			exit(-1);
		}*/

		session.BeginDataAccess();
		if (session.GotoFirstSource())
		{
			do
			{
				RTPPacket *packet;
				while ((packet = session.GetNextPacket()) != 0)
				{
					/*if(NULL == fp_wlz1)
						fp_wlz1=fopen("C:/ipc133.ps","ab");*/

					size_t lengh = packet->GetPayloadLength();
					uint8_t * RawData = packet->GetPayloadData();   //得到数据

					//fwrite(RawData,1,lengh ,fp_wlz1);

					/*std::cout << "Got packet with " 
						<< "extended sequence number " 
						<< packet->GetExtendedSequenceNumber() 
						<< " from SSRC " << packet->GetSSRC() 
						<< std::endl;*/

					/*stringstream ss;
					ss << "Got packet with " 
						<< "extended sequence number " 
						<< packet->GetExtendedSequenceNumber() 
						<< " from SSRC " << packet->GetSSRC() ;
					LOG4CPLUS_DEBUG(g_logger, ss.str().c_str());*/

					char tmp[20];
					memset(tmp, 0 ,20);
					snprintf(tmp, 20, "%010d", packet->GetSSRC());

					list<sipd_sdp_info> lst = CGbUsageEnvironment::getInstance()->GetTransferList(tmp);
					if(lst.size()>0)
					{
						session.ClearDestinations();
					

						list<sipd_sdp_info>::iterator it = lst.begin();
						for(; it!=lst.end(); it++)
						{
							sipd_sdp_info sdp = *it;

						}

						session.SendPacket()
						//session.SendPacketEx()
					}
					else
					{
						stringstream ss;
						ss << "未找到发送的目的 SDP，  SSRC = " << string(tmp);
						LOG4CPLUS_DEBUG(g_logger, ss.str().c_str());
					}

					session.DeletePacket(packet);
				}
			} while (session.GotoNextSource());
		}
		session.EndDataAccess();

		RTPTime::Wait(delay);

		RTPTime t = RTPTime::CurrentTime();
		t -= starttime;
		/*if (t > RTPTime(360.0))
			done = true;*/
	}

	delay = RTPTime(10.0);
	session.BYEDestroy(delay,"Time's up",9);
	return 0;
}

