// FfplaySdl2Pure.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
// #include "FfplaySdl2Pure.h"

#include "../ProtocolUtils/activepsparser.h"
#include "../ProtocolUtils/VideoSequence.h"
#include "../ProtocolUtils/ProgramStreamPacketizer.h"


//test different codec
#define TEST_H264  1
#define TEST_HEVC  0

// This is an example of an exported variable
FFPLAYSDL2PURE_API int nFfplaySdl2Pure=0;

// This is an example of an exported function.
FFPLAYSDL2PURE_API int fnFfplaySdl2Pure(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see FfplaySdl2Pure.h for the class definition
CFfplaySdl2Pure::CFfplaySdl2Pure()
{
	return;
}

///////////////////////////////////////////////////////////////////////////////////
//

//	管理所有的视频流
VideoStateList lstVideoState;

///////////////////////////////////////////////////////////////////////////////////
//	接口函数

int do_init();
int do_stream_open(CustomEventData * data1);

int stream_input_data(VideoState *is, int stream_index, const uint8_t *buf, int buf_size, int64_t pts);
int videostate_list_get_by_sessionId(VideoStateList *q, VideoState **pkt, uint32_t sessionId);

FFPLAYSDL2PURE_API int player_init()
{
	do_init();
	return 0;
}

FFPLAYSDL2PURE_API int player_unInit()
{
	return 0;
}


FFPLAYSDL2PURE_API int player_stream_open(stream_media_format * format, HWND hwnd)
{
	CustomEventData data;
	data.allocated = 0;
	memcpy(&data.media_format, format, sizeof(stream_media_format));
	if (hwnd != NULL)
	{
		SDL_Window * sdlWindow = SDL_CreateWindowFrom((void *)hwnd);
		Uint32 windowId = SDL_GetWindowID(sdlWindow);
		SDL_GetWindowSize(sdlWindow, &data.nWidth, &data.nHeight);
		data.window = sdlWindow;

		SDL_Renderer * sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, 0);
		data.renderer = sdlRenderer;
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");  // make the scaled rendering look smoother.
		SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
		//SDL_RenderSetLogicalSize(sdlRenderer, data.nWidth, data.nHeight);
		SDL_SetRenderDrawColor(sdlRenderer, 0xff, 0, 0, 0);

		//	创建边框的填充面
		/*data.sdlTextureBorder = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING,
		640, 480);

		data.sdlSurfaceBorder = SDL_CreateRGBSurface(0, 640, 480, 24,
		0x00FF0000,
		0x0000FF00,
		0x000000FF,
		0xFF000000);*/

	}
	else
	{
		data.window = NULL;
		data.renderer = NULL;
	}

	//	向播放模块发出 命令
	if (!(data.mutex = SDL_CreateMutex()))
		return AVERROR(ENOMEM);
	if (!(data.cond = SDL_CreateCond()))
		return AVERROR(ENOMEM);

	SDL_Event event;
	event.type = FF_STREAM_OPEN_EVENT;
	event.user.data1 = &data;
	{
		SDL_PushEvent(&event);
		SDL_LockMutex(data.mutex);
		while (!data.allocated) {
			SDL_CondWait(data.cond, data.mutex);
		}
	}

	SDL_DestroyMutex(data.mutex);
	SDL_DestroyCond(data.cond);
	return (int)data.sessionId;
}

FFPLAYSDL2PURE_API int player_stream_input_data(unsigned int sessionId, int /*AVMediaType*/ stream_type,
	const unsigned char *buf, int buf_size, long long pts)
{
	CustomEventData data;
	data.allocated = 0;
	data.sessionId = sessionId;
	data.stream_type = (PLAYER_MEDIA_TYPE)stream_type;
	data.buf = buf;
	data.buf_size = buf_size;
	data.pts = pts;
	
	//	向播放模块发出 命令
	if (!(data.mutex = SDL_CreateMutex()))
		return AVERROR(ENOMEM);
	if (!(data.cond = SDL_CreateCond()))
		return AVERROR(ENOMEM);

	SDL_Event event;
	event.type = FF_STREAM_INPUT_DATA_EVENT;
	event.user.data1 = &data;
	{
		SDL_PushEvent(&event);
		SDL_LockMutex(data.mutex);
		while (!data.allocated) {
			SDL_CondWait(data.cond, data.mutex);
		}
	}

	SDL_DestroyMutex(data.mutex);
	SDL_DestroyCond(data.cond);
	return 0;
}

FFPLAYSDL2PURE_API int player_stream_close(unsigned int sessionId)
{
	CustomEventData data;
	data.allocated = 0;
	data.sessionId = sessionId;

	//	向播放模块发出 命令
	if (!(data.mutex = SDL_CreateMutex()))
		return AVERROR(ENOMEM);
	if (!(data.cond = SDL_CreateCond()))
		return AVERROR(ENOMEM);

	SDL_Event event;
	event.type = FF_STREAM_CLOSE_EVENT;
	event.user.data1 = &data;
	{
		SDL_PushEvent(&event);
		SDL_LockMutex(data.mutex);
		while (!data.allocated) {
			SDL_CondWait(data.cond, data.mutex);
		}
	}

	SDL_DestroyMutex(data.mutex);
	SDL_DestroyCond(data.cond);
	return 0;
}

int player_stream_input_data_thread(void *arg)
{
	uint32_t sessionId = (uint32_t)arg;

	SDL_Delay(40);
	int k = 0;
	for (; k<3 ; k++)
	{
		AVCodec *pCodec;
		AVCodecContext *pCodecCtx = NULL;
		AVCodecParserContext *pCodecParserCtx = NULL;

		FILE *fp_in;

		const int in_buffer_size = 4096;
		uint8_t in_buffer[in_buffer_size + FF_INPUT_BUFFER_PADDING_SIZE] = { 0 };
		uint8_t *cur_ptr;
		int cur_size;
		AVPacket packet;

		AVCodecID codec_id = /*AV_CODEC_ID_HEVC*/ /*AV_CODEC_ID_MPEG2VIDEO*/ AV_CODEC_ID_H264;
		//char filepath_in[] = "bigbuckbunny_480x272.h264";
		char filepath_in[] = "2501659853156_vod_ch0_sess25.h264";
		//char filepath_in[] = "新闻联播.mpg";
		//char filepath_in[] = "bigbuckbunny_480x272.h265";
		//char filepath_in[] = "bigbuckbunny_480x272.m2v";
		//char filepath_in[] = "新闻联播.m2v";

		pCodec = avcodec_find_decoder(codec_id);
		if (!pCodec) {
			printf("Codec not found\n");
			break;
		}
		pCodecCtx = avcodec_alloc_context3(pCodec);
		if (!pCodecCtx){
			printf("Could not allocate video codec context\n");
			break;
		}

		pCodecParserCtx = av_parser_init(codec_id);
		if (!pCodecParserCtx){
			printf("Could not allocate video parser context\n");
			break;
		}

		//if(pCodec->capabilities&CODEC_CAP_TRUNCATED)
		//    pCodecCtx->flags|= CODEC_FLAG_TRUNCATED; 

		if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
			printf("Could not open codec\n");
			break;
		}
		//Input File
		fp_in = fopen(filepath_in, "rb");
		if (!fp_in) {
			printf("Could not open input stream\n");
			break;
		}

		CProgramStreamPacketizer * pPacketizer = new CProgramStreamPacketizer();
		unsigned long lPts = 1;

		av_init_packet(&packet);
		while (1) {

			cur_size = fread(in_buffer, 1, in_buffer_size, fp_in);
			if (cur_size == 0)
				break;
			cur_ptr = in_buffer;

			while (cur_size>0){

				int len = av_parser_parse2(
					pCodecParserCtx, pCodecCtx,
					&packet.data, &packet.size,
					cur_ptr, cur_size,
					AV_NOPTS_VALUE, AV_NOPTS_VALUE, AV_NOPTS_VALUE);

				cur_ptr += len;
				cur_size -= len;

				if (packet.size == 0)
					continue;

				player_stream_input_data(sessionId, PLAYER_MEDIA_TYPE_VIDEO, packet.data, packet.size, AV_NOPTS_VALUE);
				SDL_Delay(40);

				pPacketizer->PackegeH264Data(packet.data, packet.size, lPts, PLAYER_MEDIA_F_VIDEO_TYPE_H264);
				lPts += 1800;
			}

		}
	}
	return 0;
}


class SavedData {
public:
	SavedData(unsigned char* buf, unsigned size)
		: next(NULL), data(buf), dataSize(size)
	{
		numOfSavedData++;
	}
	virtual ~SavedData() {
		delete[] data;
		delete next;
		numOfSavedData--;
	}

	SavedData* next;
	unsigned char* data;
	unsigned dataSize;
	
public:
	static int numOfSavedData;
};

int SavedData::numOfSavedData = 0;

SavedData* savedDataHead;
SavedData* savedDataTail;


int PesPacketParserCB_general(PS_PACKET_S *pstPSPacket, APSPARSER_H * phParser, APSPARSER_USERDATA *pUserData)
{
	static AVCodec *pCodec = NULL;
	static AVCodecContext *pCodecCtx = NULL;
	static AVCodecParserContext *pCodecParserCtx = NULL;

	AVCodecID codec_id = /*AV_CODEC_ID_HEVC*/ /*AV_CODEC_ID_MPEG2VIDEO*/ AV_CODEC_ID_H264;
	if (pCodec == NULL)
	{
		pCodec = avcodec_find_decoder(codec_id);
		if (!pCodec) {
			printf("Codec not found\n");
		}
		pCodecCtx = avcodec_alloc_context3(pCodec);
		if (!pCodecCtx){
			printf("Could not allocate video codec context\n");
		}

		pCodecParserCtx = av_parser_init(codec_id);
		if (!pCodecParserCtx){
			printf("Could not allocate video parser context\n");
		}
	}

	CTActivePSParser * pParser = (CTActivePSParser *)phParser;
	uint32_t sessionId = (uint32_t)pUserData;

	unsigned char es_id = pstPSPacket->pstPESPacket->es_id & 0xE0;
	if (es_id == 224)
	{
		AVPacket packet;
		av_init_packet(&packet);

		uint8_t *cur_ptr;
		int cur_size;
		cur_ptr = pstPSPacket->pstPESPacket->es_data;
		cur_size = pstPSPacket->pstPESPacket->es_data_length;

		while (cur_size>0){

			int len = av_parser_parse2(
				pCodecParserCtx, pCodecCtx,
				&packet.data, &packet.size,
				cur_ptr, cur_size,
				AV_NOPTS_VALUE, AV_NOPTS_VALUE, AV_NOPTS_VALUE);

			cur_ptr += len;
			cur_size -= len;

			if (packet.size == 0)
				continue;

			player_stream_input_data(sessionId, PLAYER_MEDIA_TYPE_VIDEO, packet.data, packet.size, AV_NOPTS_VALUE);
			//SDL_Delay(40);
		}
	}
	return 0;
}

int PesPacketParserCB_h264(PS_PACKET_S *pstPSPacket, APSPARSER_H * phParser, APSPARSER_USERDATA *pUserData)
{
	static FILE *fp_out = NULL;
	/*static unsigned char * pIdr = NULL;
	static int Idr_len = 0;*/

	static int last_pack_id = 0;
	static unsigned char * last_pack_data = NULL;
	static int last_pack_len = 0;
	static unsigned char last_es_id = 0;

	CTActivePSParser * pParser = (CTActivePSParser *)phParser;
	uint32_t sessionId = (uint32_t)pUserData;

	char filepath_out[] = "ipc133.h264";



	//Input File
	/*if (!fp_out)
	{
		fp_out = fopen(filepath_out, "wb");
		if (!fp_out) {
			printf("Could not open input stream\n");
		}
	}*/
	

	unsigned char es_id = pstPSPacket->pstPESPacket->es_id & 0xE0;
	unsigned int data_length = pstPSPacket->pstPESPacket->es_data_length;

	if (last_pack_id == pstPSPacket->pack_id)
	{
		unsigned char * pTmp = new unsigned char[pstPSPacket->pstPESPacket->es_data_length + last_pack_len];
		memcpy(pTmp, last_pack_data, last_pack_len);
		memcpy(pTmp + last_pack_len, pstPSPacket->pstPESPacket->es_data, pstPSPacket->pstPESPacket->es_data_length);
		if (last_pack_data)
			delete[] last_pack_data;
		last_pack_data = pTmp;
		last_pack_len = pstPSPacket->pstPESPacket->es_data_length + last_pack_len;
		return 0;
	}

	//	一个完整的 PS 包接收成功，输出到播放器
	if (last_es_id == 224)
	{
		player_stream_input_data(sessionId, PLAYER_MEDIA_TYPE_VIDEO, last_pack_data,
			last_pack_len, pstPSPacket->pstPESPacket->pts);
	}
	else
	{
		player_stream_input_data(sessionId, PLAYER_MEDIA_TYPE_AUDIO, last_pack_data,
			last_pack_len, pstPSPacket->pstPESPacket->pts);
	}

	//	保存刚收到的部分 PS 包
	last_pack_id = pstPSPacket->pack_id;
	last_es_id = es_id;
	if (last_pack_data)
		delete[] last_pack_data;

	last_pack_data = new unsigned char[pstPSPacket->pstPESPacket->es_data_length];
	last_pack_len = pstPSPacket->pstPESPacket->es_data_length;
	memcpy(last_pack_data, pstPSPacket->pstPESPacket->es_data, last_pack_len);

	/*
	if (es_id == 224)
	{
		int nalType = getNalType(pstPSPacket->pstPESPacket->es_data);
		if (nalType == H264NT_SPS)
		{
			if (pIdr)
				delete[] pIdr;
			pIdr = new unsigned char[pstPSPacket->pstPESPacket->es_data_length];
			Idr_len = pstPSPacket->pstPESPacket->es_data_length;
			memcpy(pIdr, pstPSPacket->pstPESPacket->es_data, Idr_len);
		}
		else if (nalType != H264NT_SLICE && pIdr )
		{
			
			unsigned char * pTmp = new unsigned char[pstPSPacket->pstPESPacket->es_data_length + Idr_len];
			memcpy(pTmp, pIdr, Idr_len);
			memcpy(pTmp + Idr_len, pstPSPacket->pstPESPacket->es_data, pstPSPacket->pstPESPacket->es_data_length);
			if (pIdr)
				delete[] pIdr;
			pIdr = pTmp;
			Idr_len = pstPSPacket->pstPESPacket->es_data_length + Idr_len;
		
		}
		else if (getNalType(pstPSPacket->pstPESPacket->es_data) == H264NT_SLICE)
		{
			if (pIdr)
			{
				player_stream_input_data(sessionId, AVMEDIA_TYPE_VIDEO, pIdr,
					Idr_len, pstPSPacket->pstPESPacket->pts);
				delete[] pIdr;
				pIdr = NULL;
			}

			
			player_stream_input_data(sessionId, AVMEDIA_TYPE_VIDEO, pstPSPacket->pstPESPacket->es_data,
				pstPSPacket->pstPESPacket->es_data_length, pstPSPacket->pstPESPacket->pts);
		}


		
		if (fp_out)
		{
			fwrite(pstPSPacket->pstPESPacket->es_data, 1, pstPSPacket->pstPESPacket->es_data_length, fp_out);
		}
	}
	else
		player_stream_input_data(sessionId, AVMEDIA_TYPE_AUDIO, pstPSPacket->pstPESPacket->es_data,
		pstPSPacket->pstPESPacket->es_data_length, pstPSPacket->pstPESPacket->pts);
*/
	return 0;
}

int PesPacketParserCB_mpeg(PS_PACKET_S *pstPSPacket, APSPARSER_H * phParser, APSPARSER_USERDATA *pUserData)
{
	static FILE *fp_out = NULL;

	CTActivePSParser * pParser = (CTActivePSParser *)phParser;
	uint32_t sessionId = (uint32_t)pUserData;

	char filepath_out[] = "新闻联播.m2v";

	static int last_pack_id = 0;
	static unsigned char * last_pack_data = NULL;
	static int last_pack_len = 0;
	static unsigned char last_es_id = 0;


	//Input File
	if (!fp_out)
	{
		fp_out = fopen(filepath_out, "wb");
		if (!fp_out) {
			printf("Could not open input stream\n");
		}
	}


	unsigned char es_id = pstPSPacket->pstPESPacket->es_id & 0xE0;
	unsigned int data_length = pstPSPacket->pstPESPacket->es_data_length;

	/*if (es_id == 224)
	{
		if (fp_out)
		{
			fwrite(pstPSPacket->pstPESPacket->es_data, 1, pstPSPacket->pstPESPacket->es_data_length, fp_out);
		}
	}
	else
	{
		int www = 1;
	}
*/

	if (last_pack_id == pstPSPacket->pack_id)
	{
		unsigned char * pTmp = new unsigned char[pstPSPacket->pstPESPacket->es_data_length + last_pack_len];
		memcpy(pTmp, last_pack_data, last_pack_len);
		memcpy(pTmp + last_pack_len, pstPSPacket->pstPESPacket->es_data, pstPSPacket->pstPESPacket->es_data_length);
		if (last_pack_data)
			delete[] last_pack_data;
		last_pack_data = pTmp;
		last_pack_len = pstPSPacket->pstPESPacket->es_data_length + last_pack_len;
		return 0;
	}

	//	一个完整的 PS 包接收成功，输出到播放器
	if (last_es_id == 224)
	{
		player_stream_input_data(sessionId, PLAYER_MEDIA_TYPE_VIDEO, last_pack_data,
			last_pack_len, pstPSPacket->pstPESPacket->pts);

		if (fp_out)
		{
			fwrite(last_pack_data, 1, last_pack_len, fp_out);
		}
	}
	else
	{
		player_stream_input_data(sessionId, PLAYER_MEDIA_TYPE_AUDIO, last_pack_data,
			last_pack_len, pstPSPacket->pstPESPacket->pts);

		if (fp_out)
		{
			fwrite(last_pack_data, 1, last_pack_len, fp_out);
		}
	}

	//	保存刚收到的部分 PS 包
	last_pack_id = pstPSPacket->pack_id;
	last_es_id = es_id;
	if (last_pack_data)
		delete[] last_pack_data;

	last_pack_data = new unsigned char[pstPSPacket->pstPESPacket->es_data_length];
	last_pack_len = pstPSPacket->pstPESPacket->es_data_length;
	memcpy(last_pack_data, pstPSPacket->pstPESPacket->es_data, last_pack_len);

	return 0;
}

int player_stream_input_data_thread_2(void *arg)
{
	uint32_t sessionId = (uint32_t)arg;

	SDL_Delay(40);
	int k = 0;
	for (; k<3; k++)
	{

		FILE *fp_in;

		const int in_buffer_size = 4096;
		uint8_t in_buffer[in_buffer_size + FF_INPUT_BUFFER_PADDING_SIZE] = { 0 };
		uint8_t *cur_ptr;
		int cur_size;
						
		//char filepath_in[] = "bigbuckbunny_480x272.h264";
		//char filepath_in[] = "新闻联播.mpg";
		char filepath_in[] = "ipc133.ps";


		
		//Input File
		fp_in = fopen(filepath_in, "rb");
		if (!fp_in) {
			printf("Could not open input stream\n");
			break;
		}

		CTActivePSParser * pParser = new CTActivePSParser(PesPacketParserCB_general, (APSPARSER_USERDATA *)sessionId);

		while (1) {

			cur_size = fread(in_buffer, 1, in_buffer_size, fp_in);
			if (cur_size == 0)
				break;
			cur_ptr = in_buffer;

			pParser->ParsePSData(in_buffer, cur_size);

			//player_stream_input_data(sessionId, AVMEDIA_TYPE_VIDEO, packet.data, packet.size, AV_NOPTS_VALUE);
			SDL_Delay(5);

		}
	}
	return 0;
}

FFPLAYSDL2PURE_API int player_stream_loop_input_data(unsigned int sessionId)
{
	SDL_CreateThread(player_stream_input_data_thread, "11111", (void *)sessionId);
	return 0;
}