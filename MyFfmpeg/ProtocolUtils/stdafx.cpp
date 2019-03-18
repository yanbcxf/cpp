// stdafx.cpp : source file that includes just the standard includes
// ProtocolUtils.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

BufferManager g_BufferMgr;
UsageEnvironment * g_pUsageEnvironment;
JMutex	g_mutexDynamicLogger;

string	g_strDevsn;
int		g_nChannel;
string	g_strServerIP;
int		g_nServerPort;

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file


// 寻找 00 00 00 01 字节串开始位置
int SearchTag2(unsigned char * pBuffer, int begin, int dwBufSize, int & next)
{
	int i = 0;
	for (; i<dwBufSize - 3; i++)
	{
		//	先找 00 00 00 01
		if (0x00 == pBuffer[i + begin] && 0x00 == pBuffer[i + begin + 1] &&
			0x00 == pBuffer[i + begin + 2] && 0x01 == pBuffer[i + begin + 3])
		{
			next = begin + i + 4;
			break;
		}

		//	再找 00 00 01
		if (0x00 == pBuffer[i + begin] && 0x00 == pBuffer[i + begin + 1] &&
			0x01 == pBuffer[i + begin + 2])
		{
			next = begin + i + 3;
			break;
		}
	}
	if (i >= dwBufSize - 3) return -1;
	return i + begin;
}



//////////////////////////////////////////////////////////////////////////

char* strDup(char const* str) {
	if (str == NULL) return NULL;
	size_t len = strlen(str) + 1;
	char* copy = new char[len];

	if (copy != NULL) {
		memcpy(copy, str, len);
	}
	return copy;
}



#if defined(__WIN32__) || defined(_WIN32)
// For Windoze, we need to implement our own gettimeofday()

// used to make sure that static variables in gettimeofday() aren't initialized simultaneously by multiple threads
static LONG initializeLock_gettimeofday = 0;

#if !defined(_WIN32_WCE)
#include <sys/timeb.h>
#endif

int gettimeofday(struct timeval* tp, int* /*tz*/) {
	static LARGE_INTEGER tickFrequency, epochOffset;

	static Boolean isInitialized = False;

	LARGE_INTEGER tickNow;

#if !defined(_WIN32_WCE)
	QueryPerformanceCounter(&tickNow);
#else
	tickNow.QuadPart = GetTickCount();
#endif

	if (!isInitialized) {
		if (1 == InterlockedIncrement(&initializeLock_gettimeofday)) {
#if !defined(_WIN32_WCE)
			// For our first call, use "ftime()", so that we get a time with a proper epoch.
			// For subsequent calls, use "QueryPerformanceCount()", because it's more fine-grain.
			struct timeb tb;
			ftime(&tb);
			tp->tv_sec = tb.time;
			tp->tv_usec = 1000 * tb.millitm;

			// Also get our counter frequency:
			QueryPerformanceFrequency(&tickFrequency);
#else
			/* FILETIME of Jan 1 1970 00:00:00. */
			const LONGLONG epoch = 116444736000000000LL;
			FILETIME fileTime;
			LARGE_INTEGER time;
			GetSystemTimeAsFileTime(&fileTime);

			time.HighPart = fileTime.dwHighDateTime;
			time.LowPart = fileTime.dwLowDateTime;

			// convert to from 100ns time to unix timestamp in seconds, 1000*1000*10
			tp->tv_sec = (long)((time.QuadPart - epoch) / 10000000L);

			/*
			GetSystemTimeAsFileTime has just a seconds resolution,
			thats why wince-version of gettimeofday is not 100% accurate, usec accuracy would be calculated like this:
			// convert 100 nanoseconds to usec
			tp->tv_usec= (long)((time.QuadPart - epoch)%10000000L) / 10L;
			*/
			tp->tv_usec = 0;

			// resolution of GetTickCounter() is always milliseconds
			tickFrequency.QuadPart = 1000;
#endif     
			// compute an offset to add to subsequent counter times, so we get a proper epoch:
			epochOffset.QuadPart
				= tp->tv_sec * tickFrequency.QuadPart + (tp->tv_usec * tickFrequency.QuadPart) / 1000000L - tickNow.QuadPart;

			// next caller can use ticks for time calculation
			isInitialized = True;
			return 0;
		}
		else {
			InterlockedDecrement(&initializeLock_gettimeofday);
			// wait until first caller has initialized static values
			while (!isInitialized){
				Sleep(1);
			}
		}
	}

	// adjust our tick count so that we get a proper epoch:
	tickNow.QuadPart += epochOffset.QuadPart;

	tp->tv_sec = (long)(tickNow.QuadPart / tickFrequency.QuadPart);
	tp->tv_usec = (long)(((tickNow.QuadPart % tickFrequency.QuadPart) * 1000000L) / tickFrequency.QuadPart);

	return 0;
}
#endif
