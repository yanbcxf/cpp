
// stdafx.cpp : 只包括标准包含文件的源文件
// CostEngineer.pch 将作为预编译标头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"
#include <iconv.h>


BOOL sendToOutput(LPCTSTR lpch, HWND hwnd, int logType)
{
	if (logType != WM_USER_XML)
	{
		/*struct timeval timeNow;
		gettimeofday(&timeNow, NULL);*/

		struct tm timeinfo;

		//time_t now = timeNow.tv_sec;
		time_t now = time(NULL);
		localtime_s(&timeinfo, &now);		//	转换为tm  

		char tmp[256] = { 0 };
		sprintf_s(tmp, 256, "[%04d-%02d-%02d %02d:%02d:%02d] ",
			timeinfo.tm_year + 1900,
			timeinfo.tm_mon + 1,
			timeinfo.tm_mday,
			timeinfo.tm_hour,
			timeinfo.tm_min,
			timeinfo.tm_sec);
		//timeNow.tv_usec/1000,
		//GetCurrentThreadId());
		tmp[256 - 1] = 0;

		stringstream ss;
		ss << string(tmp) << string(lpch);
		CMessageString* pString = new CMessageString(ss.str().c_str());

		//LOG4CPLUS_INFO(g_logger, ss.str());

		if (hwnd == NULL)
		{
			HWND hwnd1 = AfxGetMainWnd()->m_hWnd;
			return pString->PostAsWParam(AfxGetMainWnd()->m_hWnd, logType, logType);
		}
		else
			return pString->PostAsWParam(hwnd, logType, logType);
	}
	else
	{
		stringstream ss;
		ss << "sendToOutput, WM_USER_XML = " << string(lpch);
		//LOG4CPLUS_DEBUG(g_logger, ss.str());

		CMessageString* pString = new CMessageString(lpch);
		if (hwnd == NULL)
			return pString->PostAsWParam(AfxGetMainWnd()->m_hWnd, WM_USER_XML, 0);
		else
			return pString->PostAsWParam(hwnd, WM_USER_XML, 0);
	}

}



string trimString(string & str)
{
	int s = str.find_first_not_of(" \t\r\n");
	int e = str.find_last_not_of(" \t\r\n");
	if (s >= 0 && e >= 0)
		str = str.substr(s, e - s + 1);
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
	if (position != string::npos)
	{
		left = target.substr(0, position);
		right = target.substr(position + target_string.length());
		target = left + replace_string + ReplaceString(right, target_string, replace_string);

	}
	return target;
}


string FindLastString(string src, string target_string, int & nLastPosition)
{
	string target = src;
	string right = "";
	size_t position = target.find(target_string);
	nLastPosition = 0;
	while (position != string::npos)
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
	while (position != string::npos)
	{
		target.replace(position, 1, replce_char.c_str(), replce_char.length());
		position = target.find(target_char, position + replce_char.length());
	}
	return target;
}


string Double2Currency(double val, bool isInteger)
{
	char buf[64];
	memset(buf, 0, 64);
	if (isInteger)
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

	CString     strCurrency = "";			//* Temporary Currency string
	int			nEndPos = 0;			//* Position of last character prior to decimal 										
	int			nCommaPos = 0;			//* Position of comma symbol 
	bool		bIsNegative = false;		//* Sign of number	

											// 2 - Temporary string to manipulate into the Currency string,

	strCurrency.Format("%s", val.c_str());
	strCurrency.TrimLeft();
	strCurrency.TrimRight();

	// 3 - Determine the sign +/-
	if (strCurrency.Find('-') != -1)
	{
		bIsNegative = true;
	}

	// 4 - Determine the position of the decimal place
	//	   e.g a) 1234.0	nEndPos=4
	//		   b) 1234      nEndPos=4							
	if (strCurrency.Find('.') != -1)
	{
		nEndPos = strCurrency.Find('.');
	}
	else
	{
		nEndPos = strCurrency.GetLength();
	}

	// 5 - If the number has greater then or equal to 4 characters 
	//     format with commas
	//	   e.g a) unformatted 12345  formatted 1,234

	if (nEndPos >= (4 + bIsNegative))
	{
		nCommaPos = nEndPos;
		do
		{
			nCommaPos = nCommaPos - 3;
			strCurrency.Insert(nCommaPos, ',');
		} while (nCommaPos >= (4 + bIsNegative));
	}

	// 6 - Insert the currency symbol after any negative sign 
	//strCurrency.Insert(bIsNegative,m_chCurrency_Symbol);

	return string(strCurrency.GetBuffer());

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

string Double2String(double db, string strFormat)
{
	if (_isnan(db))
		return "";

	char buf[64];
	memset(buf, 0, 64);
	if (strFormat.empty())
	{
		sprintf_s(buf, 64, "%.3f", db);
	}
	else if (strFormat.find("%") != string::npos)
	{
		sprintf_s(buf, 64, strFormat.c_str(), db);
	}
	else
	{
		stringstream ss;

		ss.setf(ios::fixed, ios::floatfield);
		ss.precision(4);          // 3 digits behind the .

		ss << db / 10000.0;
		return ss.str();
	}

	return string(buf);
}

string Int2String(int num, string strFormat)
{
	char buf[128];
	memset(buf, 0, 128);
	if (strFormat.empty())
		sprintf_s(buf, 128, "%d", num);
	else
		sprintf_s(buf, 128, strFormat.c_str(), num);
	return string(buf);
}


void splitString(string s, string dim, vector<string>& resultVector)
{
	resultVector.clear();
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

BOOL ModifyControlStyle(CWnd* pWnd, LPCTSTR lpszClassName, DWORD dwRemove, DWORD dwAdd, DWORD dwRemoveEx, DWORD dwAddEx)
{
	CWnd* pParentWnd = pWnd->GetParent();
	CFont* pFont = pWnd->GetFont();

	CString strText;
	pWnd->GetWindowText(strText);

	CRect rcWindow;
	pWnd->GetWindowRect(&rcWindow);
	pParentWnd->ScreenToClient(rcWindow);

	DWORD dwNewStyle = (pWnd->GetStyle() & ~dwRemove) | dwAdd;
	DWORD dwNewStyleEx = (pWnd->GetExStyle() & ~dwRemoveEx) | dwAddEx;

	UINT nID = pWnd->GetDlgCtrlID();

	pWnd->ShowWindow(SW_HIDE);
	pWnd->DestroyWindow();

	BOOL bResult = pWnd->CreateEx(dwNewStyleEx, lpszClassName, strText, dwNewStyle, rcWindow, pParentWnd, nID);

	pWnd->SetFont(pFont);

	return bResult;
}


// MBCS 转换到Unicode编码
int MbcsToUnicode(const char * lpMbcsStr, wchar_t * lpUnicodeStr, int nUnicodeStrLen)
{
	int nRetLen = 0;
	if (!lpMbcsStr) //如果MBCS 字符串为NULL则出错退出
	{
		return 0;
	}

	//获取转换到Unicode编码后所需要的字符空间长度
	nRetLen = ::MultiByteToWideChar(CP_UTF8, 0, (char *)lpMbcsStr, -1, NULL, NULL);
	if (nRetLen >= nUnicodeStrLen)
		return 0;

	nRetLen = ::MultiByteToWideChar(CP_UTF8, 0, (char *)lpMbcsStr, -1, lpUnicodeStr, nRetLen); //转换到Unicode编码
	return nRetLen;
}


string Gbk2Utf8(string strGBK)
{
	string target;
	char*bufout, *sin, *sout;
	int lenin, lenout, ret;

	sin = (char *)strGBK.c_str();
	lenin = strlen(sin) + 1;
	lenout = lenin * 10;

	bufout = (char *)calloc(lenout, sizeof(char));
	sout = bufout;

	iconv_t c_pt = iconv_open("utf-8", "gbk");
	if (c_pt == (iconv_t)-1) return ("ConvertEnc iconv_open error");


	iconv(c_pt, NULL, NULL, NULL, NULL);
	ret = iconv(c_pt, (const char **)&sin, (size_t *)&lenin, &sout, (size_t *)&lenout);

	if (ret == -1) return ("ConvertEnc Convert Failed");

	iconv_close(c_pt);

	target = string(bufout);
	free(bufout);
	return target;
}


string Utf8_GBK(string strUtf8)
{
	string target;
	char*bufout, *sin, *sout;
	int lenin, lenout, ret;

	do {
		sin = (char *)(strUtf8.c_str());
		lenin = strlen(sin) + 1;
		lenout = lenin * 3;

		bufout = (char *)calloc(lenout, sizeof(char));
		if (bufout == NULL)
		{
			target = "ConvertEnc calloc Failed";
			break;
		}
		sout = bufout;

		iconv_t c_pt = iconv_open("gbk//IGNORE", "utf-8");
		if (c_pt == (iconv_t)-1)
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

	} while (0);

	if (bufout)
		free(bufout);
	return target;
}

int Pcre2Grep(string patternStr, string subjectStr, vector<string> & vecMatch)
{
	vecMatch.clear();

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
		switch (rc)
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
		PCRE2_SPTR substring_start = subject + ovector[2 * i];
		size_t substring_length = ovector[2 * i + 1] - ovector[2 * i];
		strMatch = string((char *)substring_start, substring_length);
		vecMatch.push_back(strMatch);
		//printf("%2d: %.*s\n", i, (int)substring_length, (char *)substring_start);
	}

	/* Loop for second and subsequent matches */
	for (;;)
	{
		uint32_t options = 0;                    /* Normally no options */
		PCRE2_SIZE start_offset = ovector[2 * (rc - 1) + 1];  /* Start at end of previous match */

		/* If the previous match was for an empty string, we are finished if we are
		at the end of the subject. Otherwise, arrange to run another match at the
		same point to see if a non-empty match can be found. */

		if (ovector[0] == ovector[2 * (rc - 1) + 1])
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
			PCRE2_SPTR substring_start = subject + ovector[2 * i];
			size_t substring_length = ovector[2 * i + 1] - ovector[2 * i];
			strMatch = string((char *)substring_start, substring_length);
			vecMatch.push_back(strMatch);
			//printf("%2d: %.*s\n", i, (int)substring_length, (char *)substring_start);
		}
	}

	pcre2_match_data_free(match_data);
	pcre2_code_free(re);
	return vecMatch.size();
}



int Pcre2Split(string patternStr, string subjectStr, vector<string> & vecSplit)
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
		switch (rc)
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
		PCRE2_SPTR substring_start = subject + ovector[2 * i];
		size_t substring_length = ovector[2 * i + 1] - ovector[2 * i];
		strMatch = string((char *)substring_start, substring_length);

		vecStart.insert(vecStart.end(), ovector[2 * i]);
		vecEnd.insert(vecEnd.end(), ovector[2 * i + 1]);
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
			PCRE2_SPTR substring_start = subject + ovector[2 * i];
			size_t substring_length = ovector[2 * i + 1] - ovector[2 * i];
			strMatch = string((char *)substring_start, substring_length);

			vecStart.insert(vecStart.end(), ovector[2 * i]);
			vecEnd.insert(vecEnd.end(), ovector[2 * i + 1]);
		}
	}

	pcre2_match_data_free(match_data);
	pcre2_code_free(re);

	if (vecStart.size() > 0)
	{
		if (vecStart[0] > 0);
		{
			strMatch = string((char *)subject, vecStart[0]);
			trimString(strMatch);
			if (strMatch.length() > 0)
				vecSplit.push_back(strMatch);
		}
		for (int i = 0; i < vecEnd.size(); i++)
		{
			if (i < vecEnd.size() - 1)
			{
				int length = vecStart[i + 1] - vecEnd[i];
				if (length > 0)
				{
					strMatch = string((char *)subject + vecEnd[i], length);
					trimString(strMatch);
					if (strMatch.length() > 0)
						vecSplit.push_back(strMatch);
				}
			}
			else
			{
				/*最后一个分割符号*/
				int length = subject_length - vecEnd[i];
				if (length > 0)
				{
					strMatch = string((char *)subject + vecEnd[i], length);
					trimString(strMatch);
					if (strMatch.length() > 0)
						vecSplit.push_back(strMatch);
				}
			}
		}
	}

	return vecSplit.size();
}

// 6C12 2/4
bool parseSteelMarking(string marking, int* _quantity, int* _diameter, double* _weight, int* _first, int* _second) {
	int quantity = 0;
	int diameter = 0;
	double weight = 0;
	int first = 0;
	int second = 0;
	bool result = false;

	do {
		vector<string>  match1;
		if (Pcre2Split(_T("[\\s\\t]+"), marking.c_str(), match1) > 0) {
			if (match1.size() > 0) {
				vector<string>  match;
				if (Pcre2Split(_T("[ABCDEabcde]+"), match1[0].c_str(), match) > 0)
				{
					if (match.size() == 2) {
						quantity = atoi(match[0].c_str());
						int d = atoi(match[1].c_str());
						if (d == 8)  weight = 0.395;
						if (d == 10) weight = 0.619;
						if (d == 12) weight = 0.888;
						if (d == 20) weight = 2.466;
						if (d == 22) weight = 2.984;
						if (d == 25) weight = 3.850;
						diameter = d;
					}
				}
			}

			if (match1.size() > 1) {
				vector<string>  match;
				if (Pcre2Split(_T("[/]+"), match1[1].c_str(), match) > 0)
				{
					if (match.size() == 2) {
						first = atoi(match[0].c_str());
						second = atoi(match[1].c_str());
					}
				}

				if (quantity > 0 && diameter > 0 && quantity == first + second) {
					result = true;
					break;;
				}
			}
			else {
				if (quantity > 0 && diameter > 0) {
					result = true;
					break;
				}
			}
		}
		else {
			vector<string>  match;
			if (Pcre2Split(_T("[ABCDEabcde]+"), marking.c_str(), match) > 0)
			{
				if (match.size() == 2) {
					quantity = atoi(match[0].c_str());
					int d = atoi(match[1].c_str());
					if (d == 8)  weight = 0.395;
					if (d == 10) weight = 0.619;
					if (d == 12) weight = 0.888;
					if (d == 20) weight = 2.466;
					if (d == 22) weight = 2.984;
					if (d == 25) weight = 3.850;
					diameter = d;

					result = true;
					break;
				}
			}
		}
	} while (0);
	
	if (_quantity) *_quantity = quantity;
	if (_diameter) *_diameter = diameter;
	if (_weight) *_weight = weight;
	if (_first) *_first = first;
	if (_second) *_second = second;
	return result;
}


/* 年金现值系数 */
double Annuity2Present(double i, int n, int digitalNum) {
	double q = Annuity2Future(i, n);
	q = q / pow((1 + i), n);

	if (digitalNum >= 0) {
		long x = q * pow(10, digitalNum) + 0.5;
		q = x / pow(10, digitalNum);
	}
	return q;
}

/* 年金终值系数, 精确到小数点后 digitalNum 位 */
double Annuity2Future(double i, int n, int digitalNum) {
	if (i == 0) return n;
	double q = pow((1 + i), n);
	q = (1 - q) / (-i);

	if (digitalNum >= 0) {
		long x = q * pow(10, digitalNum) + 0.5;
		q = x / pow(10, digitalNum);
	}
	return q;
}

double Future2Present(double i, int n, int digitalNum) {
	double q = 1/pow(1 + i, n);
	if (digitalNum >= 0) {
		long x = q * pow(10, digitalNum) + 0.5;
		q = x / pow(10, digitalNum);
	}
	return q;
}

double Present2Future(double i, int n, int digitalNum) {
	double q = pow(1 + i, n);
	if (digitalNum >= 0) {
		long x = q * pow(10, digitalNum) + 0.5;
		q = x / pow(10, digitalNum);
	}
	return q;
}

string ActionType2String(ActionType at) {
	return "[AT:" + Int2String(at) + "]";
}

bool DrawGrid(CGridCtrl * pGridCtrl, vector<string> & vecHeader, vector<vector<string>> & vecData) {
	try {
		pGridCtrl->SetRowCount(vecData.size() + 1);
		pGridCtrl->SetColumnCount(vecHeader.size());	//	额外增加三列 ： 修改/删除
		pGridCtrl->SetFixedRowCount(1);
		pGridCtrl->SetFixedColumnCount(1);
		pGridCtrl->SetHeaderSort(FALSE);
		pGridCtrl->SetEditable(FALSE);

		pGridCtrl->GetDefaultCell(FALSE, FALSE)->SetFormat(DT_WORDBREAK | DT_EDITCONTROL | DT_NOPREFIX | DT_END_ELLIPSIS);
		pGridCtrl->EnableTitleTips(FALSE);		//	关闭每个单元格的文本超长的 气球提示
	}
	catch (CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
		return TRUE;
	}

	string strRow = "";
	int nColor = 0;
	for (int row = 0; row < pGridCtrl->GetRowCount(); row++)
	{
		if (row > 0) {
			if (strRow == vecData[row - 1][0]) {

			}
			else {
				nColor++;
				strRow = vecData[row - 1][0];
			}
		}
		
		for (int col = 0; col < pGridCtrl->GetColumnCount(); col++)
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT | GVIF_FORMAT;
			Item.row = row;
			Item.col = col;
			if (row < 1) {
				Item.nFormat = DT_LEFT | DT_WORDBREAK;

				Item.strText.Format(_T("%s"), vecHeader[col].c_str());
			}
			else
			{
				/*if (col == 0)
					Item.nFormat = DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;
				else
					Item.nFormat = DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;*/

				if (col < vecHeader.size() - 2)
				{
					if (!pGridCtrl->SetCellType(row, col, RUNTIME_CLASS(CGridCellNumeric)))
						return TRUE;
				}

				Item.strText.Format(_T("%s"), vecData[row - 1][col].c_str());
				
				Item.lParam = ActionType::None;
				vector<string> firstMatch;
				if (Pcre2Grep(_T("\\[AT:[\\d]\\]"), vecData[row - 1][col].c_str(), firstMatch) > 0)
				{
					int at;
					sscanf_s(firstMatch[0].c_str(), "[AT:%d]", &at);
					Item.lParam = at;

					at = vecData[row - 1][col].find(firstMatch[0]);
					string substr = vecData[row - 1][col].substr(0, at);
					Item.strText.Format(_T("%s"), substr.c_str());

					Item.mask |= GVIF_PARAM;
				}

				if (Item.lParam == ActionType::Delete) {
					Item.crFgClr = RGB(255, 0, 0);
					Item.mask |= GVIF_FGCLR;
				}
				else if (Item.lParam != ActionType::None) {
					Item.crFgClr = RGB(0, 120, 250);
					Item.mask |= GVIF_FGCLR;
				}
				else {
					Item.crFgClr = RGB(0, 0, 0);
					Item.mask |= GVIF_FGCLR;
				}

				/* 相邻行 变背景色 */
				if (nColor % 2 ==0) {
					Item.crBkClr = RGB(199, 237, 204);
					Item.mask |= GVIF_BKCLR;
				}
				else {
					Item.crBkClr = RGB(255, 255, 255);
					Item.mask |= GVIF_BKCLR;
				}
			}

			pGridCtrl->SetItem(&Item);
		}
	}

	bool bFormat = false;
	//	确定列的宽度
	for (int i = 0; i < vecHeader.size(); i++) {
		vector<string> vec;
		splitString(vecHeader[i], ",", vec);
		if (vec.size() > 1) {
			int nWidth = String2Double(vec[1]);
			if (nWidth > 0) {
				pGridCtrl->SetColumnWidth(i, nWidth);
				bFormat = true;
			}

		}
	}

	//	确定行的高度
	for (int row = 1; row < pGridCtrl->GetRowCount(); row++)
	{
		int nHeight = 0;
		for (int i = 0; i < vecHeader.size(); i++) {
			vector<string> vec;
			splitString(vecHeader[i], ",", vec);
			if (vec.size() > 1) {
				int nWidth = String2Double(vec[1]);
				if (nWidth > 0) {
					CString strrr = pGridCtrl->GetItemText(row, i);
					CGridCellBase* pCell = pGridCtrl->GetCell(row, i);
					if (pCell)
					{
						CSize si = pCell->GetTextExtentByWidth(strrr, nWidth);
						if( si.cy > nHeight ) nHeight = si.cy;
					}

				}
			}
		}
		if(nHeight > 0)
			pGridCtrl->SetRowHeight(row, nHeight);
	}

	
	if (!bFormat) {
		pGridCtrl->AutoSizeColumns();
		pGridCtrl->AutoSizeRows();
	}
	pGridCtrl->Refresh();

	return TRUE;
}

void paste(CString source) {
	if (OpenClipboard(NULL))
	{
		HGLOBAL clipbuffer;
		char * buffer;
		EmptyClipboard();
		clipbuffer = GlobalAlloc(GMEM_DDESHARE, source.GetLength() + 1);
		buffer = (char*)GlobalLock(clipbuffer);
		strcpy(buffer, LPCSTR(source));
		GlobalUnlock(clipbuffer);
		SetClipboardData(CF_TEXT, clipbuffer);
		CloseClipboard();
	}
}