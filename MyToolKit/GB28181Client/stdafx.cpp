// stdafx.cpp : 只包括标准包含文件的源文件
// GB28181Client.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

#include <iconv.h>

// TODO: 在 STDAFX.H 中
// 引用任何所需的附加头文件，而不是在此文件中引用

char* strDup(char const* str) 
{
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
    if(1 == InterlockedIncrement(&initializeLock_gettimeofday)) {
#if !defined(_WIN32_WCE)
      // For our first call, use "ftime()", so that we get a time with a proper epoch.
      // For subsequent calls, use "QueryPerformanceCount()", because it's more fine-grain.
      struct timeb tb;
      ftime(&tb);
      tp->tv_sec = tb.time;
      tp->tv_usec = 1000*tb.millitm;

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
    } else {
        InterlockedDecrement(&initializeLock_gettimeofday);
        // wait until first caller has initialized static values
        while(!isInitialized){
          Sleep(1);
        }
    }
  }

  // adjust our tick count so that we get a proper epoch:
  tickNow.QuadPart += epochOffset.QuadPart;

  tp->tv_sec =  (long)(tickNow.QuadPart / tickFrequency.QuadPart);
  tp->tv_usec = (long)(((tickNow.QuadPart % tickFrequency.QuadPart) * 1000000L) / tickFrequency.QuadPart);

  return 0;
}
#endif



string Gbk2Utf8(string strGBK)
{
	string target;
	char*bufout, *sin, *sout;
	int lenin, lenout, ret;

	do{
		sin    = (char *)(strGBK.c_str());
		lenin  = strlen(sin) + 1;
		lenout =lenin*3;

		bufout=(char *)calloc(lenout,sizeof(char));
		if (bufout==NULL) 
		{
			target = "ConvertEnc calloc Failed";
			break;
		}
		sout   = bufout;

		iconv_t c_pt=iconv_open("utf-8", "gbk");
		if (c_pt==(iconv_t)-1)
		{
			target = "ConvertEnc iconv_open error";
			break;
		}

		iconv(c_pt, NULL, NULL, NULL, NULL);
		ret = iconv(c_pt, (const char **)&sin, (size_t *)&lenin, &sout, (size_t *)&lenout);

		iconv_close(c_pt);

		if (ret == -1) 
			target = "ConvertEnc Convert Failed";
		else
			target = string(bufout);

	}while(0);

	if(bufout)
		free(bufout);
	return target;
}


string Utf8_GBK(string strUtf8)
{
	string target;
	char*bufout, *sin, *sout;
	int lenin, lenout, ret;

	do{
		sin    = (char *)(strUtf8.c_str());
		lenin  = strlen(sin) + 1;
		lenout =lenin*3;

		bufout=(char *)calloc(lenout,sizeof(char));
		if (bufout==NULL) 
		{
			target = "ConvertEnc calloc Failed";
			break;
		}
		sout   = bufout;

		iconv_t c_pt=iconv_open("gbk//IGNORE", "utf-8");
		if (c_pt==(iconv_t)-1)
		{
			target = "ConvertEnc iconv_open error";
			break;
		}

		iconv(c_pt, NULL, NULL, NULL, NULL);
		ret = iconv(c_pt, (const char **)&sin, (size_t *)&lenin, &sout, (size_t *)&lenout);

		iconv_close(c_pt);

		if (ret == -1) 
			target = "ConvertEnc Convert Failed";
		else
			target = string(bufout);

	}while(0);

	if(bufout)
		free(bufout);
	return target;
}


//	采用递归算法，将所有的目标字符串进行替换
string ReplaceString(string src, string target_string, string replace_string)
{
	string target = src;
	string left;
	string right;
	size_t position = target.find(target_string);
	if(position!=string::npos)
	{
		left = target.substr(0, position);
		right = target.substr(position + target_string.length());
		target = left + replace_string + ReplaceString( right, target_string, replace_string);

	}
	return target;
}


string UrlDecodeSimple(string src)
{
	string strResult;

	strResult = ReplaceString(src, "&lt;",  "<");

	strResult = ReplaceString(strResult, "&gt;",  ">");

	strResult = ReplaceString(strResult, "&apos;", "\'");

	strResult = ReplaceString(strResult, "&quot;", "\"");

	strResult = ReplaceString(strResult, "&amp;", "&");

	return strResult;
}
