#pragma once


enum H264_NAL_TYPE{ 
	H264NT_NAL = 0, 
	H264NT_SLICE,                //P帧
	H264NT_SLICE_DPA, 
	H264NT_SLICE_DPB, 
	H264NT_SLICE_DPC, 
	H264NT_SLICE_IDR,			//关键帧
	H264NT_SEI, 
	H264NT_SPS, 
	H264NT_PPS, 
	H264NT_AUD 
}; 


class CDllMediaSession: public CAbstractObject
{
public:
	CDllMediaSession(UsageEnvironment &	environ, int nSerialNumber);
	~CDllMediaSession(void);

	string ObjectType() {  return "CDllMediaSession"; }

	void PreparePsParser();

	void SaveToFile(uint8_t * pPacketData, size_t nPacketLength);

	virtual void ProcessTimeout();
	virtual bool IsTimeout(unsigned long tvNow);

	static int PesPacketParserCallback(PS_PACKET_S *pstPSPacket, APSPARSER_H * phParser, APSPARSER_USERDATA *pUserData);


public:
	sipd_media_session	m_request_sdp;
	sipd_media_session	m_response_sdp;

	int m_did;			/*该实时/回放/下载连接的会话标识*/
	int m_tid;			/*对应连接的sip事务id*/
	int m_cid;			/* call id */

	RTPSession			m_rtp_session;
	CTActivePSParser*	m_pPsParser;


	AVCodec *			m_pCodec[STREAMID_MAP_SIZE];
	AVCodecContext *	m_pCodecCtx[STREAMID_MAP_SIZE];
	AVCodecParserContext *m_pCodecParserCtx[STREAMID_MAP_SIZE];
	AVCodecID			m_codec_id[STREAMID_MAP_SIZE];

	unsigned int		m_allocSize_;
	unsigned char *		m_alloc_;

	FILE *		m_fp;

	int			m_video_idx;
	int			m_audio_idx;
	unsigned long	m_video_pts;
	unsigned long	m_audio_pts;

	bool		m_bPaused;

	unsigned long	m_start_time;		//	开始回放的时间， 精确到 秒
	uint32_t		m_start_rtp;		//	对应的 rtp 时间

	int				m_rtp_seq;
	bool			m_hasMarker;

	bool			m_bFirstVideo;
	bool			m_bFirstAudio;

	int				m_nVideoNum;

};
