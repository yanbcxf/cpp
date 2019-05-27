// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the FFPLAYSDL2PURE_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// FFPLAYSDL2PURE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#include "media_def.h"

#ifdef FFPLAYSDL2PURE_EXPORTS
#define FFPLAYSDL2PURE_API __declspec(dllexport)
#else
#define FFPLAYSDL2PURE_API __declspec(dllimport)
#endif

// This class is exported from the FfplaySdl2Pure.dll
class FFPLAYSDL2PURE_API CFfplaySdl2Pure {
public:
	CFfplaySdl2Pure(void);
	// TODO: add your methods here.
};

extern FFPLAYSDL2PURE_API int nFfplaySdl2Pure;

FFPLAYSDL2PURE_API int fnFfplaySdl2Pure(void);



typedef struct stream_media_format
{
	/*媒体类型*/
	enum PLAYER_MEDIA_TYPE media_type;
	
	/*视频编码格式*/
	enum PLAYER_MEDIA_F_STREAM_TYPE video_type;

	/*视频帧率：0～99*/
	int f_framerate;
		
	/*音频编码格式*/
	enum PLAYER_MEDIA_F_STREAM_TYPE audio_type;
	
	/*音频采样率*/
	enum PLAYER_MEDIA_F_AUDIO_SAMPLE_RATE audio_samplerate;

	/*音频通道数*/
	int audio_channels;
} stream_media_format;


FFPLAYSDL2PURE_API int player_init();
FFPLAYSDL2PURE_API int player_unInit();

FFPLAYSDL2PURE_API int player_stream_open(stream_media_format * format, HWND hwnd);

/* 异步调用 */
FFPLAYSDL2PURE_API int player_stream_input_data(unsigned int  sessionId, int /*AVMediaType*/ stream_type,
	const unsigned char *buf, int buf_size, long long pts);
FFPLAYSDL2PURE_API int player_stream_close(unsigned int sessionId);
FFPLAYSDL2PURE_API int player_stream_loop_input_data(unsigned int sessionId);

FFPLAYSDL2PURE_API int player_stream_speed(unsigned int sessionId, double speed);