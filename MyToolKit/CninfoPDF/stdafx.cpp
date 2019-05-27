
// stdafx.cpp : 只包括标准包含文件的源文件
// CninfoPDF.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

#include <tlhelp32.h>

connection_pool g_MysqlPool(MYSQL_POOL_SIZE);

friso_t		g_friso;
friso_config_t g_config;


BOOL sendToOutput(LPCTSTR lpch, HWND hwnd, int logType)
{
	if(1/*logType!=WM_USER_XML*/)
	{
		struct timeval timeNow;
		gettimeofday(&timeNow, NULL);

		struct tm timeinfo;  

		time_t now = timeNow.tv_sec;
		localtime_s(&timeinfo,&now);		//	转换为tm  

		char tmp[256];
		sprintf_s(tmp, 256, "[%04d-%02d-%02d %02d:%02d:%02d.%03d] ", 
			timeinfo.tm_year + 1900,
			timeinfo.tm_mon + 1,
			timeinfo.tm_mday,
			timeinfo.tm_hour,
			timeinfo.tm_min,
			timeinfo.tm_sec,
			timeNow.tv_usec/1000);		
		tmp[256-1] = 0;

		stringstream ss;
		ss << string(tmp) << string(lpch);
		CMessageString* pString = new CMessageString(ss.str().c_str());
		
		if(hwnd==NULL)
		{
			HWND hwnd1 = AfxGetMainWnd()->m_hWnd;
			return pString->PostAsWParam(AfxGetMainWnd()->m_hWnd, logType, logType);
		}else
			return pString->PostAsWParam(hwnd, logType, logType);
	}
	
}


// For Windoze, we need to implement our own gettimeofday()

// used to make sure that static variables in gettimeofday() aren't initialized simultaneously by multiple threads
static LONG initializeLock_gettimeofday = 0;  

#if !defined(_WIN32_WCE)
#include <sys/timeb.h>
#endif

int gettimeofday(struct timeval* tp, int* /*tz*/) {
  static LARGE_INTEGER tickFrequency, epochOffset;

  static bool isInitialized = false;

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
      isInitialized = true; 
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

/*
    string to time_t
    时间格式  2009-3-24
*/
int API_StringToTime(const string &strDateStr,time_t &timeData)
{
    char *pBeginPos = (char*) strDateStr.c_str();
    char *pPos = strstr(pBeginPos,"-");
    if(pPos == NULL)
    {
        return -1;
    }
    int iYear = atoi(pBeginPos);
    int iMonth = atoi(pPos + 1);

    pPos = strstr(pPos + 1,"-");
    if(pPos == NULL)
    {
        return -1;
    }

    int iDay = atoi(pPos + 1);

    struct tm sourcedate;
    sourcedate.tm_mday = iDay;
    sourcedate.tm_mon = iMonth - 1; 
    sourcedate.tm_year = iYear - 1900;
	sourcedate.tm_hour = 0;
	sourcedate.tm_min = 0;
	sourcedate.tm_sec = 1;
    
    timeData = mktime(&sourcedate);  

    return 0;
}


string trimString(string str )
{
	int s = str.find_first_not_of(" \t\r\n");
	int e = str.find_last_not_of(" \t\r\n");
	if(s>=0 && e>=0)
		str = str.substr(s,e-s+1);
	else
		str.clear();
	return str;
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


string FindLastString(string src, string target_string, int & nLastPosition)
{
	string target = src;
	string right = "";
	size_t position = target.find(target_string);
	nLastPosition = 0;
	while(position!=string::npos)
	{
		right = target.substr(position + target_string.length());
		nLastPosition += position + target_string.length();
		target = right;
		position = target.find(target_string);
	}
	return right;
}


string ReplaceChar(string src, char target_char, string replce_char)
{
	string target = src;
	size_t position = target.find(target_char);
	while(position!=string::npos)
	{
		target.replace(position, 1, replce_char.c_str(), replce_char.length() );
		position = target.find(target_char, position + replce_char.length());
	}
	return target;
}

double String2Double(string str)
{  
	str = ReplaceChar(str, ',', "");
	istringstream iss(str);  
	//iss.imbue();
	double x;  
	if (iss >> x)  
		return x;  
	return 0.0;  
}  

string Double2Currency(double val, bool isInteger)
{
	char buf[64];
	memset(buf, 0, 64);
	if(isInteger)
	{
		long long l = val;
		sprintf_s(buf, 64, "%lld", l);
	}
	else
	{
		sprintf_s(buf, 64, "%.2f", val);
	}

	return String2Currency(string(buf));
}

string String2Currency(string val)
{
	// 1 - Variables

	CString     strCurrency =   "";			//* Temporary Currency string
	int			nEndPos		=	0;			//* Position of last character prior to decimal 										
	int			nCommaPos	=	0;			//* Position of comma symbol 
	bool		bIsNegative=	false;		//* Sign of number	

	// 2 - Temporary string to manipulate into the Currency string,

	strCurrency.Format("%s", val.c_str());
	strCurrency.TrimLeft();
	strCurrency.TrimRight();

	// 3 - Determine the sign +/-
	if(strCurrency.Find('-') != -1)
	{
		bIsNegative = true;
	}

	// 4 - Determine the position of the decimal place
	//	   e.g a) 1234.0	nEndPos=4
	//		   b) 1234      nEndPos=4							
	if(strCurrency.Find('.') != -1)
	{
		nEndPos  = strCurrency.Find('.');  
	}
	else
	{
		nEndPos  = strCurrency.GetLength();
	}

	// 5 - If the number has greater then or equal to 4 characters 
	//     format with commas
	//	   e.g a) unformatted 12345  formatted 1,234

	if(nEndPos   >= (4 + bIsNegative))
	{
		nCommaPos = nEndPos; 
		do
		{
			nCommaPos = nCommaPos - 3;
			strCurrency.Insert(nCommaPos,',');
		}
		while(nCommaPos >= (4+bIsNegative));
	}

	// 6 - Insert the currency symbol after any negative sign 
	//strCurrency.Insert(bIsNegative,m_chCurrency_Symbol);

	return string(strCurrency.GetBuffer());

}

string Double2String(double db, string strFormat)
{
	if(_isnan(db))
		return "";

	char buf[64];
	memset(buf, 0, 64);
	if(strFormat.empty())
	{
		sprintf_s(buf, 64, "%.2f", db);
	}
	else
	{
		stringstream ss;

		ss.setf(ios::fixed, ios::floatfield);
		ss.precision(4);          // 3 digits behind the .

		ss << db/10000.0;
		return ss.str();
	}
	
	return string(buf);
}

string Int2String(int num ,string strFormat)
{
	char buf[128];
	memset(buf, 0, 128);
	if(strFormat.empty())
		sprintf_s(buf, 128, "%d", num);
	else
		sprintf_s(buf, 128, strFormat.c_str(), num);
	return string(buf);
}

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


string Utf8_GBK_(char * strUtf8)
{
	string target;
	char*bufout, *sin, *sout;
	int lenin, lenout, ret;

	do{
		sin    = strUtf8;
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


int Pcre2Grep(string patternStr,string subjectStr, vector<string> & vecMatch, vector<int> & vecOffset)
{
	vecMatch.clear();
	vecOffset.clear();

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
		vecOffset.push_back(ovector[2*i]);
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
			vecOffset.push_back(ovector[2*i]);
		}
	}
	
	pcre2_match_data_free(match_data);
	pcre2_code_free(re);
	return vecMatch.size();
}



int Pcre2Split(string patternStr,string subjectStr, vector<string> & vecSplit)
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
	vector<int>	vecStart;
	vector<int>	vecEnd;

	string strMatch;
	for (i = 0; i < rc; i++)
	{
		PCRE2_SPTR substring_start = subject + ovector[2*i];
		size_t substring_length = ovector[2*i+1] - ovector[2*i];
		strMatch = string((char *)substring_start, substring_length);

		vecStart.insert(vecStart.end(), ovector[2*i]);
		vecEnd.insert(vecEnd.end(),ovector[2*i+1]);
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

			vecStart.insert(vecStart.end(), ovector[2*i]);
			vecEnd.insert(vecEnd.end(),ovector[2*i+1]);
		}
	}
	
	pcre2_match_data_free(match_data);
	pcre2_code_free(re);

	if(vecStart.size()>0)
	{
		if(vecStart[0] > 0);
		{
			strMatch = string((char *)subject, vecStart[0]);
			trimString(strMatch);
			if(strMatch.length()>0)
				vecSplit.push_back(strMatch);
		}
		for(int i = 0; i< vecEnd.size(); i++)
		{
			if(i< vecEnd.size()-1)
			{
				int length = vecStart[i+1] - vecEnd[i];
				if(length>0)
				{
					strMatch = string((char *)subject + vecEnd[i], length);
					trimString(strMatch);
					if(strMatch.length()>0)
						vecSplit.push_back(strMatch);
				}
			}
			else
			{
				/*最后一个分割符号*/
				int length = subject_length - vecEnd[i];
				if(length>0)
				{
					strMatch = string((char *)subject + vecEnd[i], length);
					trimString(strMatch);
					if(strMatch.length()>0)
						vecSplit.push_back(strMatch);
				}
			}
		}
	}

	return vecSplit.size();
}


bool  Pcre2Find(string strSrc, string strPattern)
{
	vector<string> firstMatch;
	vector<int>		firstOffset;

	if(Pcre2Grep(strPattern, strSrc ,firstMatch, firstOffset) > 0)
	{
		return true;
	}
	return false;
}

time_t convert_string_to_time_t(string  time_string)
{
	struct tm tm1;
	time_t time1;
	int i = sscanf(time_string.c_str(), "%4d-%d-%d %d:%d:%d" ,     
		&(tm1.tm_year), 
		&(tm1.tm_mon), 
		&(tm1.tm_mday),
		&(tm1.tm_hour),
		&(tm1.tm_min),
		&(tm1.tm_sec),
		&(tm1.tm_wday),
		&(tm1.tm_yday));

	tm1.tm_year -= 1900;
	tm1.tm_mon --;
	tm1.tm_isdst=-1;
	time1 = mktime(&tm1);

	return time1;
}


void splitString(string s, string dim, vector<string>& resultVector)
{
	string::size_type  position=0; 
	string::size_type  lastPosition=0;  
	while((position=s.find_first_of(dim, position))!=string::npos)  
	{  
		//position=s.find_first_of(flag,position);   
		string elem = s.substr(lastPosition,position-lastPosition);
		resultVector.push_back(elem);
		position++;
		lastPosition = position;
	}  
	string elem = s.substr(lastPosition);
	resultVector.push_back(elem);

}



//	-1 ： 失败 ，  0 ： 成功但不存在  ， 1 ： 成功已存在
int CheckProcessOnlyOne(const char *name)
{
	PROCESSENTRY32 pe;
	long id=0;
	HANDLE hSnapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(hSnapshot == INVALID_HANDLE_VALUE )
		return -1;
	pe.dwSize=sizeof(PROCESSENTRY32);
	if(!Process32First(hSnapshot,&pe))
	{
		CloseHandle(hSnapshot);
		return -1;
	}

	int nResult = 0;
	while(1)
	{
		pe.dwSize=sizeof(PROCESSENTRY32);
		if(Process32Next(hSnapshot,&pe)==FALSE)
			break;

		size_t len = strlen(pe.szExeFile) + 1;			//	wcslen
		char CStr[256] = {0};
		memcpy(CStr, pe.szExeFile, len);

		if(strcmp((const char *)CStr,name)==0)	//相同进程名
		{
			nResult = 1;	
			break;
		}
	}
	CloseHandle(hSnapshot);
	return nResult;

}

int UnicodeToGBK(wchar_t * lpUnicodeStr, unsigned char * lpGBKStr,int nGBKStrLen)
{
	int nRetLen = 0;
	nRetLen = ::WideCharToMultiByte(CP_ACP,0,lpUnicodeStr, -1,NULL,NULL,NULL,NULL); //获取转换到GBK编码后所需要的字符空间长度

	if(!lpGBKStr)
		return nRetLen;

	if(nGBKStrLen < nRetLen) //如果输出缓冲区长度不够则退出
	{
		return 0;
	}

	nRetLen = ::WideCharToMultiByte(CP_ACP,0,lpUnicodeStr,-1,(char *)lpGBKStr,nRetLen,NULL,NULL); //转换到GBK编码

	return nRetLen;
	
}

// UTF8编码转换到GBK编码
int UTF8ToGBK(unsigned char * lpUTF8Str,unsigned char * lpGBKStr,int nGBKStrLen)
{
	wchar_t * lpUnicodeStr = NULL;
	int nRetLen = 0;

	if(!lpUTF8Str) //如果UTF8字符串为NULL则出错退出
	{
		return 0;
	}

	nRetLen = ::MultiByteToWideChar(CP_UTF8,0,(char *)lpUTF8Str,-1,NULL,NULL); //获取转换到Unicode编码后所需要的字符空间长度
	lpUnicodeStr = new WCHAR[nRetLen + 1]; //为Unicode字符串空间
	nRetLen = ::MultiByteToWideChar(CP_UTF8,0,(char *)lpUTF8Str,-1,lpUnicodeStr,nRetLen); //转换到Unicode编码
	if(!nRetLen) //转换失败则出错退出
	{
		return 0;
	}

	nRetLen = ::WideCharToMultiByte(CP_ACP,0,lpUnicodeStr,-1,NULL,NULL,NULL,NULL); //获取转换到GBK编码后所需要的字符空间长度

	if(!lpGBKStr) //输出缓冲区为空则返回转换后需要的空间大小
	{
		if(lpUnicodeStr)
		{
			delete []lpUnicodeStr;
		}
		return nRetLen;
	}

	if(nGBKStrLen < nRetLen) //如果输出缓冲区长度不够则退出
	{
		if(lpUnicodeStr)
		{
			delete []lpUnicodeStr;
		}
		return 0;
	}

	nRetLen = ::WideCharToMultiByte(CP_ACP,0,lpUnicodeStr,-1,(char *)lpGBKStr,nRetLen,NULL,NULL); //转换到GBK编码

	if(lpUnicodeStr)
	{
		delete []lpUnicodeStr;
	}

	return nRetLen;
}