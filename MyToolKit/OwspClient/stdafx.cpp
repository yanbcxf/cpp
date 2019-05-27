// stdafx.cpp : 只包括标准包含文件的源文件
// OwspClient.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

// TODO: 在 STDAFX.H 中
// 引用任何所需的附加头文件，而不是在此文件中引用

#include "log4cplus/fileappender.h"
#include "log4cplus/win32debugappender.h" 
#include "log4cplus/consoleappender.h"
#include "log4cplus/layout.h" 

BufferManager g_BufferMgr;
UsageEnvironment * g_pUsageEnvironment;
JMutex	g_mutexDynamicLogger;

string	g_strDevsn;
int		g_nChannel;
string	g_strServerIP;
int		g_nServerPort;

Logger	g_Logger;

//////////////////////////////////////////////////////////////////////////


//读配置文件
bool ReadCfg()
{
	stringstream ss;
	char szTemp[256];
	char t_spbuf[64];
	char strUser[20],strPassword[20],strIP[26];

	char		szWorkDir[256];		//	工作目录

	ZeroMemory(szWorkDir, 256);
	GetModuleFileName(NULL,szWorkDir,MAX_PATH); 
	(_tcsrchr(szWorkDir,'\\'))[1] = 0; 
	szWorkDir[strlen(szWorkDir)-1] = 0;

	sprintf(szTemp, "%s\\MobileMTS.ini", szWorkDir);
	//	手机流媒体服务器端口
	g_nServerPort = GetPrivateProfileInt("Local","g_nServerPort",10076,szTemp);

	//	手机流媒体服务器端口
	g_nChannel = GetPrivateProfileInt("Local","g_nChannel",4,szTemp);


	//	本机流媒体服务器 IP 地址
	memset(t_spbuf,0x00,64);
	GetPrivateProfileString("Local","g_strServerIP","120.25.204.144",t_spbuf,64,szTemp);
	g_strServerIP = t_spbuf;
	
	//	设备序列好 地址
	memset(t_spbuf,0x00,64);
	GetPrivateProfileString("Local","g_strDevsn","8490GC0AX0099",t_spbuf,64,szTemp);
	g_strDevsn = t_spbuf;

	ss.clear();
	ss.str("");
	ss << "手机流媒体服务器, ip = "<<   g_strServerIP; 
	ss << ", port = " << g_nServerPort;
	ss << ", devSN = " << g_strDevsn;
	ss << ", g_nChannel = " << g_nChannel;
	LOG4CPLUS_INFO(g_Logger, ss.str().c_str());

	return true;
}

string NowTime()
{
	struct tm timeinfo;

	//time_t now = timeNow.tv_sec;
	time_t now = time(NULL);
	localtime_s(&timeinfo, &now);		//	转换为tm  

	char tmp[256] = { 0 };
	sprintf_s(tmp, 256, "%04d-%02d-%02d %02d:%02d:%02d",
		timeinfo.tm_year + 1900,
		timeinfo.tm_mon + 1,
		timeinfo.tm_mday,
		timeinfo.tm_hour,
		timeinfo.tm_min,
		timeinfo.tm_sec);
	tmp[256 - 1] = 0;

	return string(tmp);
}

void splitString(string s, string dim, vector<string>& resultVector)
{
	string::size_type  position = 0;
	string::size_type  lastPosition = 0;
	while ((position = s.find_first_of(dim, position)) != string::npos)
	{
		//position=s.find_first_of(flag,position);   
		string elem = s.substr(lastPosition, position - lastPosition);
		resultVector.push_back(elem);
		position++;
		lastPosition = position;
	}
	string elem = s.substr(lastPosition);
	resultVector.push_back(elem);

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


Logger  getDynamicLogger(string strLoggerName)
{
	//boost::mutex::scoped_lock  lock(g_mutexDynamicLogger);
	if(Logger::exists(strLoggerName.c_str()))
	{
		return Logger::getInstance(strLoggerName.c_str());   
	}
	else
	{
		/* step 1: Instantiate an appender object */    
		string logFileName = string("./DailyLog/") + strLoggerName + ".log";
		SharedObjectPtr<Appender> _append (new ConsoleAppender());    
		SharedObjectPtr<Appender> _append1 (new Win32DebugAppender());    
		SharedObjectPtr<Appender> _append2 (new RollingFileAppender(LOG4CPLUS_TEXT(logFileName.c_str()), 50*1024*1024, 7));   

		/* step 2: Instantiate a layout object */    
		string pattern = "[%D{%Y-%m-%d %H:%M:%S-%q}] [%-5t] [%-5p] %m%n";    
		auto_ptr<Layout> _layout(new PatternLayout(pattern));   
		auto_ptr<Layout> _layout1(new PatternLayout(pattern));   
		auto_ptr<Layout> _layout2(new PatternLayout(pattern));   

		/* step 3: Attach the layout object to the appender */    
		_append->setLayout( _layout );   
		_append1->setLayout( _layout1 );   
		_append2->setLayout( _layout2 ); 

		/* step 4: Instantiate a logger object */    
		Logger _logger = Logger::getInstance(strLoggerName.c_str()); 

		/* step 5: Attach the appender object to the logger  */   
		//_logger.addAppender(_append);   
		_logger.addAppender(_append1);   
		_logger.addAppender(_append2);   

		/* step 6: Set a priority for the logger  */    
		_logger.setLogLevel(ALL_LOG_LEVEL);   
		return _logger;
	}
}