#include "StdAfx.h"
#include "DllMediaSession.h"
#include "CallbackDllCallDataResponse.h"

// 寻找 00 00 00 01 字节串开始位置
int SearchTag2(BYTE* pBuffer, int begin, int dwBufSize, int & next)
{
	int i=0;
	for(; i<dwBufSize-3; i++)
	{
		//	先找 00 00 00 01
		if(0x00==pBuffer[i + begin] && 0x00==pBuffer[i+begin+1] && 
			0x00==pBuffer[i+begin+2] && 0x01 == pBuffer[i+begin+3] )
		{
			next = begin + i + 4;
			break;
		}

		//	再找 00 00 01
		if(0x00==pBuffer[i + begin] && 0x00==pBuffer[i+begin+1] && 
			0x01==pBuffer[i+begin+2]  )
		{
			next = begin + i + 3;
			break;
		}
	}
	if(i>=dwBufSize-3) return -1;
	return i + begin;
}


string ScanBuffer(BYTE* pBuffer, DWORD dwBufSize)
{
	int begin = 0;
	int next = 0;
	stringstream ss;

	ss << "Scan 新视频包......";
	ss << ", 包含如下 ";
	/*pBuffer += 72;
	dwBufSize -= 72;*/
	while((begin = SearchTag2(pBuffer, next, dwBufSize - next, next))>=0)
	{
		int frmType=(pBuffer[next]&0x1f);
		if(frmType==H264NT_SPS)
		{
			ss << "SPS ";
		}
		else if(frmType==H264NT_PPS)
		{
			ss << "PPS ";
		}
		else if(frmType==H264NT_PPS)
		{
			ss << "PPS ";
		}
		else if(frmType==H264NT_SEI)
		{
			ss << "SEI ";
		}
		else if(frmType==H264NT_SLICE_IDR)
		{
			ss << "IDR ";
		}
		else if(frmType==H264NT_SLICE)
		{
			ss << "SLICE ";
		}
		else if(frmType==H264NT_AUD)
		{
			ss << "AUD ";
		}
		else
		{
			ss << "未知类型 ";
		}
	}
	return ss.str();
}

void clearAudFrame(uint8_t * & pPacketData, int & pPacketSize)
{
	// 清除开始的 辅助帧

	/*if(pPacketData[0] == 0x0 && pPacketData[1]== 0x0 && pPacketData[2] == 0x0 && pPacketData[3] == 0x01
		&& pPacketData[4] == 0x09 && pPacketData[5] == 0xf0)
	{
		pPacketData += 6;
		pPacketSize -= 6;
	}*/

	int begin = 0;
	int next = 0;

	while((begin = SearchTag2(pPacketData, next, pPacketSize - next, next))>=0)
	{
		int frmType=(pPacketData[next]&0x1f);
		if(frmType!=H264NT_SEI && frmType != H264NT_AUD)
			break;
	}

	if(begin>=0)
	{
		pPacketData += begin;
		pPacketSize -= begin;
	}

}


int CDllMediaSession::PesPacketParserCallback(PS_PACKET_S *pstPSPacket, APSPARSER_H * phParser, APSPARSER_USERDATA *pUserData)
{

	CTActivePSParser * pParser = (CTActivePSParser *)phParser;
	CDllMediaSession * pSession = (CDllMediaSession*)pUserData;

	

	/*ps 流中可以包含多个视频流，多个的音频流*/
	for (int i = 0; i < STREAMID_MAP_SIZE; i++)
	{
		if (pParser->m_StreamIDMap[i] > 0 && pSession->m_pCodec[i] == NULL)
		{
			/*	stream_type 含义：
			a)  MPEG - 4视频流：  0x10；
			b)  H.264视频流：   0x1B；
			c)  SVAC视频流：    0x80；
			d)  G.711音频流：   0x90；
			e)  G.722.1音频流： 0x92；
			f)  G.723.1音频流： 0x93；
			g)  G.729音频流：   0x99；
			h)  SVAC音频流：    0x9B。*/
			switch (pParser->m_StreamIDMap[i])
			{
			case 0x10:
				pSession->m_codec_id[i] = AV_CODEC_ID_MPEG4;
				break;
			case 0x1B:
				pSession->m_codec_id[i] = AV_CODEC_ID_H264;
				break;
			case 0x80:
				pSession->m_codec_id[i] = AV_CODEC_ID_PCM_ALAW;
				break;
			case 0x90:
				pSession->m_codec_id[i] = AV_CODEC_ID_PCM_ALAW;
				break;
			case 0x92:
				pSession->m_codec_id[i] = AV_CODEC_ID_PCM_ALAW;
				break;
			case 0x93:
				pSession->m_codec_id[i] = AV_CODEC_ID_PCM_ALAW;
				break;
			case 0x99:
				pSession->m_codec_id[i] = AV_CODEC_ID_PCM_ALAW;
				break;
			case 0x9B:
				pSession->m_codec_id[i] = AV_CODEC_ID_PCM_ALAW;
				break;
			default:
				pSession->m_codec_id[i] = AV_CODEC_ID_PCM_ALAW;
			}


			pSession->m_pCodec[i] = avcodec_find_decoder(pSession->m_codec_id[i]);
			if (!pSession->m_pCodec[i]) {
				//printf("Codec not found\n");
			}
			pSession->m_pCodecCtx[i] = avcodec_alloc_context3(pSession->m_pCodec[i]);
			if (!pSession->m_pCodecCtx[i]){
				//printf("Could not allocate video codec context\n");
			}

			pSession->m_pCodecParserCtx[i] = av_parser_init(pSession->m_codec_id[i]);
			if (!pSession->m_pCodecParserCtx[i]){
				//printf("Could not allocate video parser context\n");
			}
		}
	}

	/*char tttmp[256] = { 1 };
	CCallbackDllCallDataResponse * pcb = new CCallbackDllCallDataResponse(pSession->SerailNumber(), 
		(unsigned char *)tttmp, 256);
	pSession->m_Environ.PostDataCallBack(pcb);
	return 0;*/
	

	unsigned char es_id = pstPSPacket->pstPESPacket->es_id;

	if(es_id == 192)
	{
		int wwww =1;
	}

	if (pSession->m_pCodec[es_id])
	{
		uint8_t *cur_ptr;
		int cur_size;
		cur_ptr = pstPSPacket->pstPESPacket->es_data;
		cur_size = pstPSPacket->pstPESPacket->es_data_length;

		while (cur_size>0)
		{
			uint8_t * pPacketData = NULL;
			int pPacketSize = 0;

			AVPacket packet;
			av_init_packet(&packet);

			if(pParser->m_StreamIDMap[es_id]==0x1B)
			{
				//	视频包处理
				
				int len = av_parser_parse2(
					pSession->m_pCodecParserCtx[es_id],
					pSession->m_pCodecCtx[es_id],
					&packet.data, &packet.size,
					cur_ptr, cur_size,
					pstPSPacket->pstPESPacket->pts, pstPSPacket->pstPESPacket->pts, AV_NOPTS_VALUE);

				cur_ptr += len;
				cur_size -= len;

				if (packet.size == 0)
					continue;

				//	清除 h264 AUD 
				pPacketData = packet.data;
				pPacketSize = packet.size;

				/*string strScan = ScanBuffer(pPacketData, pPacketSize);
				LOG4CPLUS_DEBUG(pSession->m_Environ.m_Logger, strScan );*/

				clearAudFrame(pPacketData, pPacketSize);

			}
			else
			{
				//	音频包处理

				pPacketData = cur_ptr;
				pPacketSize = cur_size;
				
				cur_size = 0;
			}
			
			//	增加 pes 头

			H264_PARAM snHead;
			snHead.version = 2;
			
			//0=H264视频，=G711.A(1ch,8k,16bit)	 2=unsigned pcm(1ch,8k,8bit)  3=G711 ulaw(1ch,8k,16bit)	
			if(pParser->m_StreamIDMap[es_id]==0x1B)
			{
				snHead.knd = 0;
				snHead.idx = pSession->m_video_idx;				//索引	
				pSession->m_video_idx++;
			}
			else if(pParser->m_StreamIDMap[es_id]==0x90)
			{
				snHead.knd = 1;
				snHead.idx = pSession->m_audio_idx;				
				pSession->m_audio_idx++; 
			}
			else 
				snHead.knd = 2;

			// unsigned long cur_pts = pstPSPacket->pstPESPacket->pts / 90000.0 *  1000000;

		
			snHead.fbs = 25;			//帧率或采样率		 4		
			if(pSession->m_start_time ==0)
				snHead.curtime = time(NULL);		//？？码流带时间		 8  pts换算成秒  
			else
			{
				if(pSession->m_start_rtp==0)
					pSession->m_start_rtp = pParser->m_rtptime ;
				
				//	curtime 单位是 s 
				snHead.curtime = pSession->m_start_time + (pParser->m_rtptime - pSession->m_start_rtp)/1000000.0;
			}
			snHead.ptsSetup = 0;				//pts步进值			 4	
			snHead.dtsSetup = 0;				//dtsSetup			 4
			snHead.pts =   pParser->m_rtptime ;	//	单位时 us 
			snHead.dts = 0;						//dts信息			 8	
			snHead.nal_len = pPacketSize;			//码流带长度		 4    pkt_.size;

			if(pSession->m_allocSize_ < (unsigned int)(pPacketSize + 76))
			{
				delete[] pSession->m_alloc_;
				pSession->m_alloc_ = NULL;
				pSession->m_alloc_ = new unsigned char[pPacketSize + 76];
				pSession->m_allocSize_ = pPacketSize + 76;
			}
			memset(pSession->m_alloc_, 0, sizeof(pSession->m_allocSize_));
			memcpy(pSession->m_alloc_, &snHead, sizeof(H264_PARAM));
			memcpy(pSession->m_alloc_ + sizeof(H264_PARAM), pPacketData, pPacketSize);
			memset(pSession->m_alloc_ + sizeof(H264_PARAM) + pPacketSize, 0xFF, 4);	//0xFFFFFFFF

			if(g_Dll_Call_Response_func)
			{

				if(snHead.knd ==0)
					pSession->m_nVideoNum++;

				bool bLog = false;
				if(pSession->m_bFirstAudio && snHead.knd >0 )
				{
					bLog = true;
					pSession->m_bFirstAudio = false;
				}
				if(pSession->m_bFirstVideo && snHead.knd == 0 )
				{
					bLog = true;
					pSession->m_bFirstVideo = false;
				}

				if(bLog)
				{
					char tmp[256] = { 0 };
					sprintf_s(tmp, 256, "[%08d] knd = %d, pts = %u, datasize = %6d, curtime = %u", 
						pSession->SerailNumber(), snHead.knd, pParser->m_rtptime, pPacketSize,  snHead.curtime );
					LOG4CPLUS_DEBUG(pSession->m_Environ.m_Logger, string(tmp) );
				}
				
				//g_Dll_Call_Response_func(pSession->SerailNumber(), 1, 0, pSession->m_alloc_,  pPacketSize + 76);

				unsigned long tvBegin = GetTickCount();
				
				CCallbackDllCallDataResponse * pcb = new CCallbackDllCallDataResponse(pSession->SerailNumber(), 
					pSession->m_alloc_,  pPacketSize + 76);
				pSession->m_Environ.PostDataCallBack(pcb);

				unsigned long tvNow = GetTickCount();

				long delay = tvNow - tvBegin;
				if(delay > 16)
				{
					char tmp[256] = { 0 };
					sprintf_s(tmp, 256, "CCallbackDllCallDataResponse，用时 %d ms", delay);
					LOG4CPLUS_DEBUG(gb_Logger, string(tmp) );
				}
			}

			av_free_packet(&packet);
		}
	}

	return 0;
}


//////////////////////////////////////////////////////////////////////////

CDllMediaSession::CDllMediaSession(UsageEnvironment & environ, int nSerialNumber)
:CAbstractObject(environ, nSerialNumber)
{
	memset(&m_request_sdp, 0, sizeof(sipd_media_session));
	memset(&m_response_sdp, 0, sizeof(sipd_media_session));

	m_tid = -1;
	m_did = -1;
	m_cid = -1;

	m_pPsParser = NULL;

	m_fp = NULL;

	for(int i = 0 ; i< STREAMID_MAP_SIZE; i++)
	{
		m_pCodec[i] = NULL;
		m_pCodecCtx[i] = NULL;
		m_pCodecParserCtx[i] = NULL;
	}

	m_alloc_ = NULL;
	m_allocSize_ = 0;

	m_video_idx = 0;
	m_audio_idx = 0;

	m_video_pts = 0;
	m_audio_pts = 0;

	m_bPaused = false;

	m_bFirstVideo = true;
	m_bFirstAudio = true;

	m_Environ.m_nMediaSession++;

	m_nVideoNum = 0;
}

CDllMediaSession::~CDllMediaSession(void)
{
	if(m_pPsParser)
		delete m_pPsParser;

	if(m_alloc_)
		delete[] m_alloc_;

	if(m_fp)
		fclose(m_fp);

	for(int i = 0 ; i< STREAMID_MAP_SIZE; i++)
	{
		bool bfree = false;
		if(m_pCodecParserCtx[i])
		{
			bfree = true;
			av_parser_close(m_pCodecParserCtx[i]);
			//av_free(m_pCodecParserCtx[i]);
		}

		if(m_pCodecCtx[i])
		{
			if(bfree)
				avcodec_close(m_pCodecCtx[i]);
			av_free(m_pCodecCtx[i]);
		}

		/*if(m_pCodec[i])
		{
			av_free(m_pCodec[i]);
		}*/
	}

	if(m_rtp_session.IsActive())
	{
		m_rtp_session.Destroy();
	}

	m_Environ.m_nMediaSession--;

}


void CDllMediaSession::PreparePsParser()
{
	if(m_pPsParser==NULL)
	{
		m_pPsParser = new CTActivePSParser(PesPacketParserCallback, this);
	}
}

void CDllMediaSession::SaveToFile(uint8_t * pPacketData, size_t nPacketLength)
{
	if(m_fp==NULL)
	{
		char tmp[256] = { 0 };
		sprintf_s(tmp, 256, "gb_28181_%06d.ps", m_SerialNumber);
		m_fp = fopen(tmp, "wb");
	}
	if(m_fp)
		fwrite(pPacketData, 1, nPacketLength, m_fp);
}


void CDllMediaSession::ProcessTimeout()
{
	char tmp[256] = { 0};
	sprintf_s(tmp, 256, "%s[%08d], tid = %d, Timeout", ObjectType().c_str(),  SerailNumber(), m_tid);
	LOG4CPLUS_DEBUG(m_Environ.m_Logger, string(tmp));

	CCallbackDllCallResponse *pcb = new CCallbackDllCallResponse(SerailNumber(), 0, -1);
	m_Environ.PostCallBack(pcb);
}



bool  CDllMediaSession::IsTimeout(unsigned long tvNow)
{
	long  delay = tvNow - m_KeepaliveTime; 

	if( m_did < 0 && delay > 10 * 1000)
		return true;

	return false;
}
