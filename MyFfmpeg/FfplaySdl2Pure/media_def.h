
#ifndef __MEDIA_DEF_H__
#define __MEDIA_DEF_H__


typedef unsigned long long u64_t;
typedef unsigned int u32_t;
typedef unsigned short u16_t;
typedef unsigned char u8_t;
typedef long long s64_t;
typedef int s32_t;
typedef short s16_t;
typedef char s8_6;

/*媒体类型，视频/音频/音视频。当取值为SIPD_MEDIA_TYPE_NONE 时设备根据自己的情况默认发送视频或者视音频，推荐只发送视频即可*/
enum PLAYER_MEDIA_TYPE{ PLAYER_MEDIA_TYPE_NONE = 0, PLAYER_MEDIA_TYPE_VIDEO = 1, PLAYER_MEDIA_TYPE_AUDIO = 2, PLAYER_MEDIA_TYPE_MIX = 3 };


enum PLAYER_MEDIA_F_STREAM_TYPE{
	PLAYER_MEDIA_F_VIDEO_TYPE_DEF = 0,
	PLAYER_MEDIA_F_VIDEO_TYPE_MPEG2 = 0x02,
	PLAYER_MEDIA_F_VIDEO_TYPE_MPEG4 = 0x10,
	PLAYER_MEDIA_F_VIDEO_TYPE_H264 = 0x1B,
	PLAYER_MEDIA_F_VIDEO_TYPE_SVAC = 0x80,
	PLAYER_MEDIA_F_VIDEO_TYPE_3GP = 4,

	PLAYER_MEDIA_F_AUDIO_TYPE_G711 = 0x90,
	PLAYER_MEDIA_F_AUDIO_TYPE_G722 = 0x92,
	PLAYER_MEDIA_F_AUDIO_TYPE_G723 = 0x93,
	PLAYER_MEDIA_F_AUDIO_TYPE_G729 = 0x99,
	PLAYER_MEDIA_F_AUDIO_TYPE_SVAC = 0x9B
};

/*视频分辨率，当取值为SIPD_MEDIA_F_VIDEO_RESOLUTION_DEF 时设备使用自己的默认分辨率*/
enum PLAYER_MEDIA_F_VIDEO_RESOLUTION{ PLAYER_MEDIA_F_VIDEO_RESOLUTION_DEF = 0, PLAYER_MEDIA_F_VIDEO_RESOLUTION_QCIF = 1, PLAYER_MEDIA_F_VIDEO_RESOLUTION_CIF = 2, PLAYER_MEDIA_F_VIDEO_RESOLUTION_4CIF = 3, PLAYER_MEDIA_F_VIDEO_RESOLUTION_D1 = 4, PLAYER_MEDIA_F_VIDEO_RESOLUTION_720P = 5, PLAYER_MEDIA_F_VIDEO_RESOLUTION_1080P = 6 };

/*音频采样率，当为SIPD_MEDIA_F_AUDIO_SAMPLE_RATE_DEF 时设备使用自己默认的音频采样率。
SIPD_MEDIA_F_AUDIO_SAMPLE_RATE_8表示 8kHz
SIPD_MEDIA_F_AUDIO_SAMPLE_RATE_14表示14kHz
SIPD_MEDIA_F_AUDIO_SAMPLE_RATE_16表示16kHz
SIPD_MEDIA_F_AUDIO_SAMPLE_RATE_32表示32kHz*/
enum PLAYER_MEDIA_F_AUDIO_SAMPLE_RATE{ PLAYER_MEDIA_F_AUDIO_SAMPLE_RATE_DEF = 0, PLAYER_MEDIA_F_AUDIO_SAMPLE_RATE_8 = 1, PLAYER_MEDIA_F_AUDIO_SAMPLE_RATE_14 = 2, PLAYER_MEDIA_F_AUDIO_SAMPLE_RATE_16 = 3, PLAYER_MEDIA_F_AUDIO_SAMPLE_RATE_32 = 4 };

#endif