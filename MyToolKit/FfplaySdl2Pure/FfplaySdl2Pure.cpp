// FfplaySdl2Pure.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
// #include "FfplaySdl2Pure.h"

//#include "../ProtocolUtils/activepsparser.h"
//#include "../ProtocolUtils/VideoSequence.h"
//#include "../ProtocolUtils/ProgramStreamPacketizer.h"


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

Logger g_player_log;

///////////////////////////////////////////////////////////////////////////////////
//	接口函数

int do_init();
int do_stream_open(CustomEventData * data1);

int stream_input_data(VideoState *is, int stream_index, uint8_t *buf, int buf_size, int64_t pts);
int videostate_list_get_by_sessionId(VideoStateList *q, VideoState **pkt, uint32_t sessionId);

FFPLAYSDL2PURE_API int player_init()
{
	g_player_log =  Logger::getInstance(LOG4CPLUS_TEXT("Player"));
	do_init();
	return 0;
}

FFPLAYSDL2PURE_API int player_unInit()
{
	return 0;
}

//	同步阻塞调用， 成功则 返回播放句柄 > 0 
FFPLAYSDL2PURE_API int player_stream_open(stream_media_format * format, HWND hwnd)
{
	CustomEventData data;
	memcpy(&data.media_format, format, sizeof(stream_media_format));
	if (hwnd != NULL)
	{
		SDL_Window * sdlWindow = SDL_CreateWindowFrom((void *)hwnd);
		if(sdlWindow==NULL)
			return -2;
		Uint32 windowId = SDL_GetWindowID(sdlWindow);
		SDL_GetWindowSize(sdlWindow, &data.nWidth, &data.nHeight);
		data.window = sdlWindow;
		data.renderer = NULL;

		/*
			SDL_RENDERER_SOFTWARE ：使用软件渲染
			SDL_RENDERER_ACCELERATED ：使用硬件加速
			SDL_RENDERER_PRESENTVSYNC：和显示器的刷新率同步
			SDL_RENDERER_TARGETTEXTURE ：不太懂                                                                   
		*/

		SDL_RendererInfo info;
		SDL_GetRenderDriverInfo(0, &info);

		


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
		return -1;
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


//	异步调用
FFPLAYSDL2PURE_API int player_stream_input_data(unsigned int sessionId, int /*AVMediaType*/ stream_type,
	const unsigned char *buf, int buf_size, long long pts)
{
	CustomEventData * data = new CustomEventData;
	data->sessionId = sessionId;
	data->stream_type = (PLAYER_MEDIA_TYPE)stream_type;
	if(buf_size>0)
	{
		data->buf = new unsigned char[buf_size];
		if(data->buf)
			memcpy_s(data->buf, buf_size, buf, buf_size);
		else
		{
			delete data;
			return -1;
		}
		data->buf_size = buf_size;
	}
	else
	{
		delete data;
		return -2;
	}
	data->pts = pts;


	SDL_Event event;
	event.type = FF_STREAM_INPUT_DATA_EVENT;
	event.user.data1 = data;
	SDL_PushEvent(&event);
	
	return 0;
}

// 慢放 或者 快放
FFPLAYSDL2PURE_API int player_stream_speed(unsigned int sessionId, double speed)
{
	CustomEventData data;
	data.sessionId = sessionId;
	data.speed = speed;

	//	向播放模块发出 命令
	if (!(data.mutex = SDL_CreateMutex()))
		return AVERROR(ENOMEM);
	if (!(data.cond = SDL_CreateCond()))
		return AVERROR(ENOMEM);

	SDL_Event event;
	event.type = FF_STREAM_SPEED_EVENT;
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
		char filepath_in[] = "bigbuckbunny_480x272.h264";
		//char filepath_in[] = "2501659853156_vod_ch0_sess25.h264";
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

		//CProgramStreamPacketizer * pPacketizer = new CProgramStreamPacketizer();
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

				//pPacketizer->PackegeH264Data(packet.data, packet.size, lPts, PLAYER_MEDIA_F_VIDEO_TYPE_H264);
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


FFPLAYSDL2PURE_API int player_stream_loop_input_data(unsigned int sessionId)
{
	SDL_CreateThread(player_stream_input_data_thread, "11111", (void *)sessionId);
	return 0;
}

