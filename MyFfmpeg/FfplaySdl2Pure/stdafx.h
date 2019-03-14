// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

//最多存储的帧信息
#define MAX_FRAME_NUM 10000
//最多存储的Packet信息
#define MAX_PACKET_NUM 10000
//URL长度
#define MAX_URL_LENGTH 500


#define UINT64_C unsigned __int64

//补充定义
#define INT64_MIN       (-9223372036854775807i64 - 1)
#define INT64_MAX       9223372036854775807i64

#include "cmdutils.h"			//	其中包含了 include "stdint.h"
// #include <stdint.h>

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavfilter/avfilter.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
}


#include <assert.h>

//--------------
#include "config.h"
#include <inttypes.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <signal.h>

#define isnan _isnan

extern "C"
{
#include "libavutil/avstring.h"
#include "libavutil/colorspace.h"
#include "libavutil/eval.h"
#include "libavutil/mathematics.h"
#include "libavutil/pixdesc.h"
#include "libavutil/imgutils.h"
#include "libavutil/dict.h"
#include "libavutil/parseutils.h"
#include "libavutil/samplefmt.h"
#include "libavutil/avassert.h"
#include "libavutil/time.h"
#include "libavformat/avformat.h"
#include "libavdevice/avdevice.h"
#include "libswscale/swscale.h"
#include "libavutil/opt.h"
#include "libavcodec/avfft.h"
#include "libswresample/swresample.h"
}

#if CONFIG_AVFILTER
# include "libavfilter/avcodec.h"
# include "libavfilter/avfilter.h"
# include "libavfilter/avfiltergraph.h"
# include "libavfilter/buffersink.h"
# include "libavfilter/buffersrc.h"
#endif

#include "SDL.h"
#include "SDL_thread.h"
#include "SDL_events.h"


#define FF_ALLOC_EVENT   (SDL_USEREVENT)
#define FF_REFRESH_EVENT (SDL_USEREVENT + 1)
#define FF_QUIT_EVENT    (SDL_USEREVENT + 2)
#define FF_STREAM_OPEN_EVENT    (SDL_USEREVENT + 3)
#define FF_STREAM_CLOSE_EVENT    (SDL_USEREVENT + 4)
#define FF_STREAM_INPUT_DATA_EVENT    (SDL_USEREVENT + 5)
// TODO: reference additional headers your program requires here

///////////////////////////////////////////////////////////////////////////

#define MAX_QUEUE_SIZE (15 * 1024 * 1024)
#define MIN_FRAMES 5

/* Minimum SDL audio buffer size, in samples. */
#define SDL_AUDIO_MIN_BUFFER_SIZE 512
/* Calculate actual buffer size keeping in mind not cause too frequent audio callbacks */
#define SDL_AUDIO_MAX_CALLBACKS_PER_SEC 30

/* no AV sync correction is done if below the minimum AV sync threshold */
#define AV_SYNC_THRESHOLD_MIN 0.04
/* AV sync correction is done if above the maximum AV sync threshold */
#define AV_SYNC_THRESHOLD_MAX 0.1
/* If a frame duration is longer than this, it will not be duplicated to compensate AV sync */
#define AV_SYNC_FRAMEDUP_THRESHOLD 0.1
/* no AV correction is done if too big error */
#define AV_NOSYNC_THRESHOLD 10.0

/* maximum audio speed change to get correct sync */
#define SAMPLE_CORRECTION_PERCENT_MAX 10

/* external clock speed adjustment constants for realtime sources based on buffer fullness */
#define EXTERNAL_CLOCK_SPEED_MIN  0.900
#define EXTERNAL_CLOCK_SPEED_MAX  1.010
#define EXTERNAL_CLOCK_SPEED_STEP 0.001

/* we use about AUDIO_DIFF_AVG_NB A-V differences to make the average */
#define AUDIO_DIFF_AVG_NB   20

/* polls for possible required screen refresh at least this often, should be less than 1/fps */
#define REFRESH_RATE 0.01

/* NOTE: the size must be big enough to compensate the hardware audio buffersize size */
/* TODO: We assume that a decoded and resampled frame fits into this buffer */
#define SAMPLE_ARRAY_SIZE (8 * 65536)

#define CURSOR_HIDE_DELAY 1000000


typedef struct MyAVPacketList {
	AVPacket pkt;
	struct MyAVPacketList *next;
	int serial;
} MyAVPacketList;

typedef struct PacketQueue {
	MyAVPacketList *first_pkt, *last_pkt;
	int nb_packets;
	int size;
	int abort_request;
	int serial;
	SDL_mutex *mutex;
	SDL_cond *cond;
} PacketQueue;

#define VIDEO_PICTURE_QUEUE_SIZE 3
#define SUBPICTURE_QUEUE_SIZE 16
#define SAMPLE_QUEUE_SIZE 9
#define FRAME_QUEUE_SIZE FFMAX(SAMPLE_QUEUE_SIZE, FFMAX(VIDEO_PICTURE_QUEUE_SIZE, SUBPICTURE_QUEUE_SIZE))

typedef struct AudioParams {
	int freq;
	int channels;
	int64_t channel_layout;
	enum AVSampleFormat fmt;
	int frame_size;
	int bytes_per_sec;
} AudioParams;

// 保存了 pts 和 系统时钟 （get_time_relative() ）的对应关系 
typedef struct Clock {
	double pts;           /* clock base */
	double pts_drift;     /* clock base minus time at which we updated the clock */
	double last_updated;	/* 该值与 pts 应该是对应的 */
	double speed;			/*快进和慢放有关*/
	int serial;           /* clock is based on a packet with this serial */
	int paused;
	int *queue_serial;    /* pointer to the current packet queue serial, used for obsolete clock detection */
} Clock;

/* Common struct for handling all types of decoded data and allocated render buffers. */
typedef struct Frame {
	AVFrame *frame;
	AVSubtitle sub;
	int serial;
	double pts;           /* presentation timestamp for the frame */
	double duration;      /* estimated duration of the frame */
	int64_t pos;          /* byte position of the frame in the input file */
	SDL_Texture *bmp;
	int allocated;
	int reallocate;
	int width;
	int height;
	AVRational sar;
} Frame;

typedef struct FrameQueue {
	Frame queue[FRAME_QUEUE_SIZE];
	int rindex;		//	队列的读索引
	int windex;		//	队列的写索引
	int size;
	int max_size;
	int keep_last;		//	keep_last = 1, 说明要求保存已经显示过的帧，该标志控制 frame_queue_next 函数的动作
	int rindex_shown;	//	rindex_shown = 1 ，说明 rindex 指向的帧已经被显示了
	SDL_mutex *mutex;
	SDL_cond *cond;
	PacketQueue *pktq;
} FrameQueue;

enum {
	AV_SYNC_AUDIO_MASTER, /* default choice */
	AV_SYNC_VIDEO_MASTER,
	AV_SYNC_EXTERNAL_CLOCK, /* synchronize to an external clock */
};

typedef struct Decoder {
	AVPacket pkt;
	AVPacket pkt_temp;
	PacketQueue *queue;
	AVCodecContext *avctx;
	int pkt_serial;
	int finished;
	int packet_pending;
	SDL_cond *empty_queue_cond;
	int64_t start_pts;
	AVRational start_pts_tb;
	int64_t next_pts;
	AVRational next_pts_tb;
	SDL_Thread *decoder_tid;
} Decoder;

//////////////////////////////////////////////////////////////////////////

enum ShowMode {
	SHOW_MODE_NONE = -1, SHOW_MODE_VIDEO = 0, SHOW_MODE_WAVES, SHOW_MODE_RDFT, SHOW_MODE_NB
};

//////////////////////////////////////////////////////////////////////////

#include "FfplaySdl2Pure.h"

typedef struct VideoState {
	SDL_Thread *read_tid;
	// AVInputFormat *iformat;
	int abort_request;
	int force_refresh;
	int paused;
	int last_paused;
	int queue_attachments_req;
	int seek_req;
	int seek_flags;
	int64_t seek_pos;
	int64_t seek_rel;
	int read_pause_return;
	AVFormatContext *ic;		//	原先用来从网络或者文件中获取流， 现在作用仅保存 流数据的格式信息
	int realtime;

	Clock audclk;
	Clock vidclk;
	Clock extclk;

	FrameQueue pictq;
	// FrameQueue subpq;
	FrameQueue sampq;

	Decoder auddec;
	Decoder viddec;
	//Decoder subdec;

	int audio_stream;

	int av_sync_type;

	double audio_clock;
	int audio_clock_serial;
	double audio_diff_cum; /* used for AV difference average computation */
	double audio_diff_avg_coef;
	double audio_diff_threshold;
	int audio_diff_avg_count;
	AVStream *audio_st;
	// BOOL	audio_st;
	PacketQueue audioq;
	int audio_hw_buf_size;
	uint8_t silence_buf[SDL_AUDIO_MIN_BUFFER_SIZE];
	uint8_t *audio_buf;
	uint8_t *audio_buf1;
	unsigned int audio_buf_size; /* in bytes */
	unsigned int audio_buf1_size;
	int audio_buf_index; /* in bytes */
	int audio_write_buf_size;
	struct AudioParams audio_src;
#if CONFIG_AVFILTER
	struct AudioParams audio_filter_src;
#endif
	struct AudioParams audio_tgt;
	struct SwrContext *swr_ctx;
	int frame_drops_early;
	int frame_drops_late;

	ShowMode show_mode;
	int16_t sample_array[SAMPLE_ARRAY_SIZE];
	int sample_array_index;
	int last_i_start;
	RDFTContext *rdft;
	int rdft_bits;
	FFTSample *rdft_data;
	int xpos;
	double last_vis_time;

	/*int subtitle_stream;
	AVStream *subtitle_st;
	PacketQueue subtitleq;*/

	double frame_timer;
	double frame_last_returned_time;
	double frame_last_filter_delay;

	int video_stream;
	AVStream *video_st;
	//BOOL	video_st;
	PacketQueue videoq;
	double max_frame_duration;      // maximum duration of a frame - above this, we consider the jump a timestamp discontinuity
#if !CONFIG_AVFILTER
	struct SwsContext *img_convert_ctx;
#endif
	SDL_Rect last_display_rect;
	int eof;

	char filename[1024];
	int width, height, xleft, ytop;
	int step;

#if CONFIG_AVFILTER
	int vfilter_idx;
	AVFilterContext *in_video_filter;   // the first filter in the video chain
	AVFilterContext *out_video_filter;  // the last filter in the video chain
	AVFilterContext *in_audio_filter;   // the first filter in the audio chain
	AVFilterContext *out_audio_filter;  // the last filter in the audio chain
	AVFilterGraph *agraph;              // audio filter graph
#endif

	int last_video_stream, last_audio_stream /*, last_subtitle_stream*/;

	SDL_cond *continue_read_thread;

	SDL_Window * sdlWindow;				//	该流对应的显示窗口
	SDL_Renderer * sdlRenderer;			//	显示窗口对应的渲染器

	/*SDL_Texture * sdlTextureBorder;
	SDL_Surface * sdlSurfaceBorder;*/
	SDL_TimerID		timer;				//	延时视频更新
	uint32_t	sessionId;				//	会话标识

	//int original_width, original_height;

	stream_media_format  media_format;	//	传进来的 码流格式参数

	struct VideoState *next;
} VideoState;

//	管理所有的视频流

typedef struct VideoStateList {
	VideoState *first_pkt, *last_pkt;
	int abort_request;
	SDL_mutex *mutex;
	SDL_cond *cond;
} VideoStateList;

//////////////////////////////////////////////////////////////////////////

//自定义一个事件，用于调整播放进度
#define FFMFC_SEEK_BAR_EVENT    (SDL_USEREVENT + 4)


//是否拉伸-------------------------
#define FFMFC_STRETCH_EVENT (SDL_USEREVENT + 5)


typedef struct CustomEventData {
	SDL_Window * window;
	SDL_Renderer * renderer;
	SDL_Texture * sdlTextureBorder;
	SDL_Surface * sdlSurfaceBorder;
	int	nWidth;
	int nHeight;
	stream_media_format  media_format;
	SDL_mutex *mutex;
	SDL_cond *cond;
	int	allocated;			//	该标志位，用来确认 播放模块已经命令处理完毕
	uint32_t sessionId;

	PLAYER_MEDIA_TYPE /*AVMediaType*/ stream_type;
	const uint8_t *buf;
	int buf_size;
	int64_t pts;
} CustomEventData;


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
};

extern VideoStateList lstVideoState;

uint32_t our_random32();

extern void avpriv_set_pts_info(AVStream *s, int pts_wrap_bits,
	unsigned int pts_num, unsigned int pts_den);

extern int getNalType(unsigned char * pBSBuf);

