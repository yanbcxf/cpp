// FfplaySdl2.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "FfplaySdl2.h"

#include "ffplaycore.h"

// This is an example of an exported variable
FFPLAYSDL2_API int nFfplaySdl2=0;

// This is an example of an exported function.
FFPLAYSDL2_API int fnFfplaySdl2(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see FfplaySdl2.h for the class definition
CFfplaySdl2::CFfplaySdl2()
{
	return;
}

///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
//	接口函数



FFPLAYSDL2_API int player_init()
{
	do_init();
	return 0;
}

FFPLAYSDL2_API int player_unInit()
{
	return 0;
}


FFPLAYSDL2_API int player_stream_open(char * filename, HWND hwnd)
{
	CustomEventData data;
	data.allocated = 0;
	strcpy(data.filename, filename);
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
		while (!data.allocated ) {
			SDL_CondWait(data.cond, data.mutex);
		}

	}

	SDL_DestroyMutex(data.mutex);
	SDL_DestroyCond(data.cond);
	return 0;
}