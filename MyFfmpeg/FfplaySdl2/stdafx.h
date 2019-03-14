// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>


#define UINT64_C unsigned __int64

//补充定义
#define INT64_MIN       (-9223372036854775807i64 - 1)
#define INT64_MAX       9223372036854775807i64

#include "cmdutils.h"			//	其中包含了 include "stdint.h"

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

typedef struct CustomEventData {
	SDL_Window * window;
	SDL_Renderer * renderer;
	SDL_Texture * sdlTextureBorder;
	SDL_Surface * sdlSurfaceBorder;
	int	nWidth;
	int nHeight;
	char filename[1024];
	SDL_mutex *mutex;
	SDL_cond *cond;
	int	allocated;
} CustomEventData;


uint32_t our_random32();