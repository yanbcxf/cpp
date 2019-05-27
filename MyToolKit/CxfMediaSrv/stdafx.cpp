// stdafx.cpp : 只包括标准包含文件的源文件
// CxfMediaSrv.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

// TODO: 在 STDAFX.H 中
// 引用任何所需的附加头文件，而不是在此文件中引用

//	正则表达试
#define PCRE2_CODE_UNIT_WIDTH 8
#define PCRE2_STATIC

#include "pcre2.h"

#include "log4cplus/fileappender.h"
#include "log4cplus/win32debugappender.h" 
#include "log4cplus/consoleappender.h"
#include "log4cplus/layout.h" 


string	g_strLocalIP;
string	g_strDBAddr;
int		g_iRtpPort;
string	g_strLocalDomainID;

Logger	g_logger;

string	g_strCurrentDir;

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


//读配置文件
bool ReadCfg(string fullpath)
{
	stringstream ss;
	char t_spbuf[64];

	//	本机 IP 地址
	memset(t_spbuf,0x00,64);
	GetPrivateProfileString("Local","g_strLocalIP","10.18.72.35",t_spbuf,64,fullpath.c_str());
	g_strLocalIP = t_spbuf;

	//	本机 IP 地址
	memset(t_spbuf,0x00,64);
	GetPrivateProfileString("Local","g_strLocalDomainID","35010400002020000055",t_spbuf,64,fullpath.c_str());
	g_strLocalDomainID = t_spbuf;

	//	数据服务
	memset(t_spbuf,0x00,64);
	GetPrivateProfileString("Server","g_strDBAddr","10.18.73.238",t_spbuf,64,fullpath.c_str());
	g_strDBAddr = t_spbuf;

	//	数据服务端口
	g_iRtpPort  = GetPrivateProfileInt("Local","g_iRtpPort",6000, fullpath.c_str());

	ss.str("");
	//LOG4CPLUS_DEBUG(g_logger, ss.str().c_str());
	return true;
}


Logger  getDynamicLogger(string strLoggerName)
{
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



int Pcre2Grep(string patternStr,string subjectStr, vector<string> & vecMatch)
{
	pcre2_code *re;
	PCRE2_SPTR pattern;     /* PCRE2_SPTR is a pointer to unsigned code units of */
	PCRE2_SPTR subject;     /* the appropriate width (8, 16, or 32 bits). */

	int crlf_is_newline;
	int errornumber;
	int i;
	int rc;
	int utf8;

	PCRE2_SIZE erroroffset;
	PCRE2_SIZE *ovector;

	size_t subject_length;
	pcre2_match_data *match_data;

	pattern = (PCRE2_SPTR)patternStr.c_str();
	subject = (PCRE2_SPTR)subjectStr.c_str();
	subject_length = strlen((char *)subject);

	re = pcre2_compile(
		pattern,               /* the pattern */
		PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
		0,                     /* default options */
		&errornumber,          /* for error number */
		&erroroffset,          /* for error offset */
		NULL);                 /* use default compile context */

	/* Compilation failed: print the error message and exit. */

	if (re == NULL)
	{
		PCRE2_UCHAR buffer[256];
		pcre2_get_error_message(errornumber, buffer, sizeof(buffer));
		// printf("PCRE2 compilation failed at offset %d: %s\n", (int)erroroffset,	buffer);
		return -1;
	}

	match_data = pcre2_match_data_create_from_pattern(re, NULL);

	rc = pcre2_match(
		re,                   /* the compiled pattern */
		subject,              /* the subject string */
		subject_length,       /* the length of the subject */
		0,                    /* start at offset 0 in the subject */
		0,                    /* default options */
		match_data,           /* block for storing the result */
		NULL);                /* use default match context */

	/* Matching failed: handle error cases */

	if (rc < 0)
	{
	  switch(rc)
		{
		case PCRE2_ERROR_NOMATCH: printf("No match\n"); break;
		/*
		Handle other special cases if you like
		*/
		default: printf("Matching error %d\n", rc); break;
		}
	  pcre2_match_data_free(match_data);   /* Release memory used for the match */
	  pcre2_code_free(re);                 /* data and the compiled pattern. */
	  return -2;
	}


	/* Match succeded. Get a pointer to the output vector, where string offsets are
	stored. */

	ovector = pcre2_get_ovector_pointer(match_data);
	printf("\nMatch succeeded at offset %d\n", (int)ovector[0]);

	if (rc == 0)
	{
		printf("ovector was not big enough for all the captured substrings\n");
		return -3;
	}
	//////////////////////////////////////////////////////////////////////////
	//	输出找到的匹配字符串

	string strMatch;
	for (i = 0; i < rc; i++)
	{
		PCRE2_SPTR substring_start = subject + ovector[2*i];
		size_t substring_length = ovector[2*i+1] - ovector[2*i];
		strMatch = string((char *)substring_start, substring_length);
		vecMatch.push_back(strMatch);
		//printf("%2d: %.*s\n", i, (int)substring_length, (char *)substring_start);
	}

	/* Loop for second and subsequent matches */
	for (;;)
	{
		uint32_t options = 0;                    /* Normally no options */
		PCRE2_SIZE start_offset = ovector[1];  /* Start at end of previous match */

		/* If the previous match was for an empty string, we are finished if we are
		at the end of the subject. Otherwise, arrange to run another match at the
		same point to see if a non-empty match can be found. */

		if (ovector[0] == ovector[1])
		{
			if (ovector[0] == subject_length) break;
			options = PCRE2_NOTEMPTY_ATSTART | PCRE2_ANCHORED;
		}

		/* Run the next matching operation */

		rc = pcre2_match(
			re,                   /* the compiled pattern */
			subject,              /* the subject string */
			subject_length,       /* the length of the subject */
			start_offset,         /* starting offset in the subject */
			options,              /* options */
			match_data,           /* block for storing the result */
			NULL);                /* use default match context */

		if (rc == PCRE2_ERROR_NOMATCH)
		{
			if (options == 0) break;                    /* All matches found */
			ovector[1] = start_offset + 1;              /* Advance one code unit */
			if (crlf_is_newline &&                      /* If CRLF is newline & */
				start_offset < subject_length - 1 &&    /* we are at CRLF, */
				subject[start_offset] == '\r' &&
				subject[start_offset + 1] == '\n')
				ovector[1] += 1;                          /* Advance by one more. */
			else if (utf8)                              /* Otherwise, ensure we */
			{                                         /* advance a whole UTF-8 */
				while (ovector[1] < subject_length)       /* character. */
				{
					if ((subject[ovector[1]] & 0xc0) != 0x80) break;
					ovector[1] += 1;
				}
			}
			continue;    /* Go round the loop again */
		}

		/* Other matching errors are not recoverable. */

		if (rc < 0)
		{
			printf("Matching error %d\n", rc);
			pcre2_match_data_free(match_data);
			pcre2_code_free(re);
			return -4;
		}

		/* Match succeded */

		//printf("\nMatch succeeded again at offset %d\n", (int)ovector[0]);

		/* The match succeeded, but the output vector wasn't big enough. This
		should not happen. */
		/*if (rc == 0)
			printf("ovector was not big enough for all the captured substrings\n");*/

		//	输出找到的匹配字符串

		for (i = 0; i < rc; i++)
		{
			PCRE2_SPTR substring_start = subject + ovector[2*i];
			size_t substring_length = ovector[2*i+1] - ovector[2*i];
			strMatch = string((char *)substring_start, substring_length);
			vecMatch.push_back(strMatch);
			//printf("%2d: %.*s\n", i, (int)substring_length, (char *)substring_start);
		}
	}
	
	pcre2_match_data_free(match_data);
	pcre2_code_free(re);
	return vecMatch.size();
}
