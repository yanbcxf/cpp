// SDL2TimerTest.cpp : Defines the entry point for the console application.
//

/*
1. Uint32 SDL_GetTicks() : 获取从SDL库初始化(定时器模块初始化)开始到当前的运行时间(ms);
2. Uint64 SDL_GetPerformanceCounter() : 获取高精度定时器的当前值;
3. Uint64 SDL_GetPerformanceFrequency() : 获取高进度定时器频率(每秒钟高精度定时器的次数);
4. void SDL_Delay(Uint32 ms) : 延迟函数;
5. SDL_TimerID SDL_AddTimer(Uint32 interval, SDL_TimerCallback callback, void *param) :
	向系统请求增加一个定时器.
	SDL_TimerID： 定时器的ID, 若该值为NULL，表示请求失败;
	interval:  定时间隔;
	callback: 回调函数, 定时时间到后会调用此函数;
	备注: typedef Uint32(*SDL_TimerCallback)(Uint32 interval, void *param) :
	该函数返回值为下次唤醒的时长, 若返回0, 则不会再唤醒.

6. SDL_bool SDL_RemoveTimer(SDL_TimerID id) : 删除给定id的定时器, 返回值表示是否删除成功.
*/

#include "stdafx.h"
#include "SDL.h"

void testfunc()
{
}

Uint32 callback(Uint32 interval, void *param)
{
	SDL_Log("current time: %d ms.\n", SDL_GetTicks());
	return interval;
}

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	Uint32 start32;
	Uint32 loop_count = 50000;
	Uint64 start, now;

	if (SDL_Init(SDL_INIT_TIMER) < 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL Timer SubSystem: %s\n", SDL_GetError());
	}

	start32 = SDL_GetTicks();
	start = SDL_GetPerformanceCounter();
	for (Uint32 i = 0; i < loop_count; ++i)
	{
		testfunc();
	}
	SDL_Log("%d times function call take %d ms\n", loop_count, SDL_GetTicks() - start32);
	now = SDL_GetPerformanceCounter();
	SDL_Log("%d times function call take %f ms \n", loop_count, (double)(now - start) * 1000 / SDL_GetPerformanceFrequency());

	SDL_TimerID timer = SDL_AddTimer(100, callback, NULL);
	if (NULL == timer)
	{
		SDL_Log("add timer fail: %s\n", SDL_GetError());
	}

	Uint32 test = 100;
	while (test--)
	{
		SDL_Delay(100);
	}

	if (SDL_TRUE != SDL_RemoveTimer(timer))
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Remove timer fail: %s\n", SDL_GetError());
	}
	return 0;
}

