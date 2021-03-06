
// stdafx.cpp : 只包括标准包含文件的源文件
// Lottery.pch 将作为预编译标头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"
#include <iconv.h>

#include <igraph.h>

Logger  g_logger;
JMutex  g_mtx_igraph;

connection_pool g_MysqlPool(MYSQL_POOL_SIZE);

void Swap(int A[], int i, int j)
{
	int temp = A[i];
	A[i] = A[j];
	A[j] = temp;
}

void BubbleSort(int A[], int n)
{
	for (int j = 0; j < n - 1; j++)         // 每次最大元素就像气泡一样"浮"到数组的最后
	{
		for (int i = 0; i < n - 1 - j; i++) // 依次比较相邻的两个元素,使较大的那个向后移
		{
			if (A[i] > A[i + 1])            // 如果条件改成A[i] >= A[i + 1],则变为不稳定的排序算法
			{
				Swap(A, i, i + 1);
			}
		}
	}
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
		// LOG4CPLUS_DEBUG(g_logger, ss.str());

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

string Double2String(double db, string strFormat)
{
	if (_isnan(db))
		return "";

	char buf[64];
	memset(buf, 0, 64);
	if (strFormat.empty())
	{
		sprintf_s(buf, 64, "%.2f", db);
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



string consecutive_number_to_string(int(&cons)[5][10])
{
	stringstream ss1;
	for (int i = 0; i < 5; i++)
	{
		if (cons[i][0] == 0)	break;
		stringstream ss;
		for (int k = 0; k < cons[i][k]; k++)
		{
			ss << cons[i][k];
			if (cons[i][k + 1])
				ss << "/";
		}

		ss1 << ss.str();
		if (cons[i + 1][0])
			ss1 << ", ";
	}
	return ss1.str();
}



void consecutive_number(int * nNumber, int nCount, int (&Consecutive0)[5][10] ,
	int (&Consecutive1)[5][10], int (&Consecutive2)[5][10])
{
	int		CurConsecutive[10] = { 0 };
	int		nCur = 0;
	int		nCur0, nCur1, nCur2;
	nCur0 = nCur1 = nCur2 = 0;

	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			Consecutive0[i][j] = 0;
			Consecutive1[i][j] = 0;
			Consecutive2[i][j] = 0;
		}
	}

	//	计算 间隔 0 的连号
	for (int i = 0; i < nCount; i++)
	{
		if (nCur == 0)
		{
			CurConsecutive[nCur] = nNumber[i];
			nCur++;
		}
		else
		{
			if (CurConsecutive[nCur - 1] + 1 == nNumber[i])
			{
				//	连续
				CurConsecutive[nCur] = nNumber[i];
				nCur++;
			}
			else
			{
				//	不连续
				if (nCur > 1)
				{
					//	保存之前连续的号码
					for(int k = 0; k< nCur; k++)
						Consecutive0[nCur0][k] = CurConsecutive[k];
					nCur0++;
				}

				nCur = 0;
				CurConsecutive[nCur] = nNumber[i];
				nCur++;
			}
		}
	}

	if (nCur > 1)
	{
		//	判断最后一个元素是否连号
		for (int k = 0; k < nCur; k++)
			Consecutive0[nCur0][k] = CurConsecutive[k];
		nCur0++;
	}

	//	过滤掉间隔 0 的号码， 生成计算间 1 的连号
	int vec1[10] = { 0 };
	int nVec1 = 0;
	for (int i = 0; i < nCount; i++)
	{
		bool bFound = false;
		for (int k = 0; k < nCur0; k++)
		{
			for (int j = 0; j < Consecutive0[k][j]; j++)
			{
				if (Consecutive0[k][j] == nNumber[i])
				{
					bFound = true;
					break;
				}
			}
		}
		if (bFound == false)
		{
			vec1[nVec1] = nNumber[i];
			nVec1++;
		}
	}
	if (nVec1 == 0) return;

	nCur = 0;
	for (int i = 0; i < nVec1; i++)
	{
		if (nCur == 0)
		{
			CurConsecutive[nCur] = vec1[i];
			nCur++;
		}
		else
		{
			if (CurConsecutive[nCur - 1] + 2 == vec1[i])
			{
				//	连续
				CurConsecutive[nCur] = vec1[i];
				nCur++;
			}
			else
			{
				//	不连续
				if (nCur > 1)
				{
					//	保存之前连续的号码
					for (int k = 0; k < nCur; k++)
						Consecutive1[nCur1][k] = CurConsecutive[k];
					nCur1++;
				}

				nCur = 0;
				CurConsecutive[nCur] = vec1[i];
				nCur++;
			}
		}
	}
	if (nCur > 1)
	{
		//	判断最后一个元素是否连号
		for (int k = 0; k < nCur; k++)
			Consecutive1[nCur1][k] = CurConsecutive[k];
		nCur1++;
	}

	//	过滤掉间隔 0 和 间 1 的号码， 生成计算间 2 的连号
	int vec2[10] = { 0 };
	int nVec2 = 0;
	for (int i = 0; i < nCount; i++)
	{
		bool bFound = false;
		for (int k = 0; k < nCur0; k++)
		{
			for (int j = 0; j < Consecutive0[k][j]; j++)
			{
				if (Consecutive0[k][j] == nNumber[i])
				{
					bFound = true;
					break;
				}
			}
		}

		if (bFound == false)
		{
			for (int k = 0; k < nCur1; k++)
			{
				for (int j = 0; j < Consecutive1[k][j]; j++)
				{
					if (Consecutive1[k][j] == nNumber[i])
					{
						bFound = true;
						break;
					}
				}
			}
		}

		if (bFound == false)
		{
			vec2[nVec2] = nNumber[i];
			nVec2++;
		}
	}
	if (nVec2 == 0) return;

	nCur = 0;
	for (int i = 0; i < nVec2; i++)
	{
		if (nCur == 0)
		{
			CurConsecutive[nCur] = vec2[i];
			nCur++;
		}
		else
		{
			if (CurConsecutive[nCur - 1] + 3 == vec1[i])
			{
				//	连续
				CurConsecutive[nCur] = vec2[i];
				nCur++;
			}
			else
			{
				//	不连续
				if (nCur > 1)
				{
					//	保存之前连续的号码
					for (int k = 0; k < nCur; k++)
						Consecutive2[nCur2][k] = CurConsecutive[k];
					nCur2++;
				}

				nCur = 0;
				CurConsecutive[nCur] = vec1[i];
				nCur++;
			}
		}
	}
	if (nCur > 1)
	{
		//	判断最后一个元素是否连号
		for (int k = 0; k < nCur; k++)
			Consecutive2[nCur2][k] = CurConsecutive[k];
		nCur2++;
	}
}

// 基本特征分析
void BaseCharacter(int * nNumber, int regular_num, int total_num, LotteryNumber & lotto, 
	vector<int> region_start, vector<int> region_end)
{
	
	//	进行 和值、奇偶、相同尾数等判断
	lotto.odd = 0;
	lotto.even = 0;
	lotto.sum = 0;
	string tail[10];
	for (int i = 0; i < 10; i++)	tail[i] = "";


	for (int i = 0; i < regular_num; i++)
	{
		//	奇偶
		if (nNumber[i] % 2)
			lotto.odd++;
		else
			lotto.even++;
		//
		lotto.sum += nNumber[i];

		//	相同尾数
		int tl = nNumber[i] % 10;
		if (tail[tl].empty())
			tail[tl] += Int2String(nNumber[i]);
		else
			tail[tl] += "/" + Int2String(nNumber[i]);
	}

	//	相同尾数
	lotto.equal_tail = "";
	for (int i = 0; i < 10; i++)
	{
		if (tail[i].empty() == false && tail[i].find("/") != string::npos)
		{
			if (lotto.equal_tail.empty())
				lotto.equal_tail += tail[i];
			else
				lotto.equal_tail += ", " + tail[i];
		}
	}

	//	相邻号码间隔分析
	for (int i = 1; i < regular_num; i++)
	{
		lotto.adjacent_interval[i - 1] = nNumber[i] - nNumber[i - 1];
	}

	//  区间内号码个数分析
	for (int i = 0; i < regular_num; i++)
	{
		for (int k = 0; k < region_start.size() && k < region_end.size(); k++)
		{
			if (region_start[k] > 0 && region_end[k] > region_start[k])
			{
				if (nNumber[i] >= region_start[k] && nNumber[i] <= region_end[k])
					lotto.region_count[k]++;
			}
		}
	}
}

// 统计特征分析
void StatisticCharacter(int * nNumber, int regular_num, int total_num, LotteryNumber & lotto)
{
	// 方差
	double var1, var2, mean;
	var1 = var2 = mean = 0;
	for (int i = 0; i < regular_num; i++)
	{
		var1 += nNumber[i] * nNumber[i];
		var2 += nNumber[i];
	}
	lotto.var = (regular_num * var1 - var2 * var2) / (regular_num * (regular_num - 1));
	lotto.stdev = sqrt(lotto.var);

	// 	
	lotto.devsq = 0;
	mean = var2 / regular_num;
	for (int i = 0; i < regular_num; i++)
	{
		lotto.devsq += (nNumber[i] - mean) * (nNumber[i] - mean);
	}

	//
	lotto.kurt = 0;
	double s = 0;	// 计算标准差
	for (int i = 0; i < regular_num; i++)
	{
		var1 = (nNumber[i] - mean) ;
		var2 = var1 * var1 * var1 * var1;
		lotto.kurt += var2;
		s += var1 * var1;
	}
	s = s / (regular_num - 1);
	s = sqrt(s);
	lotto.kurt = lotto.kurt / (s *  s * s *  s);
	int n = regular_num;
	lotto.kurt = lotto.kurt * (n + 1) * n / ((n - 1)*(n - 2)*(n - 3));
	lotto.kurt -= 3.0 * (n - 1) *(n - 1) / ((n - 2)*(n - 3));

	// 
	lotto.skew = 0;
	for (int i = 0; i < regular_num; i++)
	{
		var1 = (nNumber[i] - mean) / lotto.stdev;
		var2 = var1 * var1 * var1;
		lotto.skew += var2;
	}
	lotto.skew = lotto.skew * n / ((n - 1) * (n - 2));
}

// 边缘号码数分析
int fringe_no_qty_number(int * nNumber, int regular_num, int total_num)
{
	int fringe_no_qty = 0;
	for (int i = 0; i < regular_num; i++)
	{
		int nRow = (nNumber[i] - 1) / NUM_PER_LINE;
		int	nCol = (nNumber[i] - 1) % NUM_PER_LINE;

		if (nRow == 0 || nCol == 0 || nCol == NUM_PER_LINE - 1)
			fringe_no_qty++;	// 左右边缘 和 上边缘
		else if (nNumber[i] + NUM_PER_LINE > total_num)
			fringe_no_qty++;	//	下边缘
	}
	return fringe_no_qty;
}

// AC 值分析, 
int AC_number(int * nNumber, int regular_num, int total_num)
{
	set<int>	setAc;
	for (int i = 0; i < regular_num - 1; i++)
	{
		for (int k = i + 1; k < regular_num; k++)
		{
			int nSub = nNumber[k] - nNumber[i];
			setAc.insert(nSub);
		}
	}
	return setAc.size() - (regular_num - 1);
}

int is_in_group(int num, int * p, int nCount)
{
	for (int k = 0; k < nCount; k++)
	{
		if (p[k] == num)
			return 1;
	}
	return 0;
}

int group_number(int * p, int nCount, int nNumPerLine, int isLog)
{
	JMutexAutoLock lock(g_mtx_igraph);
	igraph_t g1;
	igraph_vector_t v1;
	int ret;
	stringstream ss;

	if (isLog)
	{
		ss << "igraph_create ==================== ";
		for (int k = 0; k < nCount; k++)
		{
			ss << p[k];
			if (k < nCount - 1)
				ss << ",";
		}
		// LOG4CPLUS_DEBUG(g_logger, ss.str());
	}
	

	/* Create a graph */
	igraph_vector_init(&v1, nCount * 2);
	for (int i = 0; i < nCount; i++)
	{
		VECTOR(v1)[2 * i] = p[i];
		VECTOR(v1)[2 * i + 1] = p[i];

	}
	
	/* 根据自反边来创建图 */
	ret = igraph_create(&g1, &v1, 0, 0);
	if (ret != 0)
		return -1;

	

	int nVec = igraph_vcount(&g1);
	int nEdge = igraph_ecount(&g1);

	// 根据开奖号码的相邻性，来添加边
	for (int i = 0; i < nCount; i++)
	{
		int nFromRow = (p[i] - 1) / nNumPerLine;
		int	nFromCol = (p[i] - 1) % nNumPerLine;

		for (int j = 0; j < nCount; j++)
		{
			if (i != j)
			{
				int nToRow = (p[j] - 1) / nNumPerLine;
				int	nToCol = (p[j] - 1) % nNumPerLine;

				if (nToRow == nFromRow && (nToCol - nFromCol == 1 || nToCol - nFromCol == -1) ||
					nToCol == nFromCol && (nToRow - nFromRow == 1 || nToRow - nFromRow == -1))
				{
					/*if (isLog)
					{
						stringstream ss;
						ss << "from " << p[i] << " --- to " << p[j];
						LOG4CPLUS_DEBUG(g_logger, ss.str());
					}*/
					
					/* Add edges */
					igraph_vector_resize(&v1, 2);
					VECTOR(v1)[0] = p[i]; VECTOR(v1)[1] = p[j];
					igraph_add_edges(&g1, &v1, 0);
				}
			}
		}
	}

	igraph_vector_destroy(&v1);

	ret = igraph_simplify(&g1, true, true, 0);
	if (ret != 0)
	{
		igraph_destroy(&g1);
		return -2;
	}

	int  nVec1 = igraph_vcount(&g1);
	int nEdge1 = igraph_ecount(&g1);
	if (nEdge1 == 0)
	{
		igraph_destroy(&g1);
		return nCount;
	}
	else if (isLog)
	{
		ss.str("");
		ss << "nVec1 = " << nVec1 << ",nEdge1 = " << nEdge1;
		// LOG4CPLUS_DEBUG(g_logger, ss.str());
	}

	// 对图进行广度优先遍历, vecGroup 用来保存每个群包含的点标识
	int vecGroup[10][10];
	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 10; j++)
			vecGroup[i][j] = 0;

	int nRow = 0;
	for (int i = 0; i < nCount; i++)
	{
		int num = p[i];
		bool bFound = false;
		for (int k = 0; k < 10 &&  vecGroup[k][0]; k++)
		{
			for (int j = 0; j < 10 && vecGroup[k][j]; j++)
			{
				if (vecGroup[k][j] == num)
				{
					bFound = true;
					break;
				}
			}
			if (bFound)	break;
		}
		if (bFound) continue;

		igraph_vector_t vids;
		igraph_vector_init(&vids, 0);
		
		igraph_bfs(&g1, /*root=*/num, /*roots=*/ 0, /*neimode=*/ IGRAPH_ALL,
			/*unreachable=*/ 0, /*restricted=*/ 0,
			&vids, /*&rank*/0, /*&father*/0, /*&pred*/0, /*&succ*/0, /*&dist*/0,
			/*callback=*/ 0, /*extra=*/ 0);
		
		if (isLog)
		{
			ss.str("");
			ss << "vids = ";
			for (int k = 0; k < igraph_vector_size(&vids) && k < 10; k++)
			{
				ss << VECTOR(vids)[k] << " ";
			}
			// LOG4CPLUS_DEBUG(g_logger, ss.str());
		}
		
		int	vec[10];
		int nVec = 0;
		for (int k = 0; k<igraph_vector_size(&vids); k++) {
			int nei = VECTOR(vids)[k];
			if (nei>0)
			{
				vec[nVec] = nei;
				nVec++;
				if (nVec == 10)
				{
					break;
				}
			}
			else
				break;
		}

		if (nVec < 10)
		{
			for (int k = 0; k < nVec; k++)
				vecGroup[nRow][k] = vec[k];
			nRow++;

			/*if (isLog)
			{
				ss.str("");
				ss << "vec[" << (nRow - 1) << "] = ";
				for (int k = 0; k < nVec; k++)
				{
					ss << vec[k] << " ";
				}
				LOG4CPLUS_DEBUG(g_logger, ss.str());
			}*/
		}
		
		igraph_vector_destroy(&vids);
	}

	igraph_destroy(&g1);
	return nRow;
}



int group_number_old(int * p, int nCount, int nNumPerLine)
{
	int isLog = 0;
	JMutexAutoLock lock(g_mtx_igraph);
	igraph_t g1;
	igraph_vector_t v1;
	int ret;
	stringstream ss;

	/*if (isLog)
	{
		ss << "igraph_create ==================== ";
		for (int k = 0; k < nCount; k++)
		{
			ss << p[k];
			if (k < nCount - 1)
				ss << ",";
		}
		LOG4CPLUS_DEBUG(g_logger, ss.str());
	}*/


	/* Create a graph */
	igraph_vector_init(&v1, nCount * 2);
	for (int i = 0; i < nCount; i++)
	{
		VECTOR(v1)[2 * i] = p[i];
		VECTOR(v1)[2 * i + 1] = p[i];

	}

	/* 根据自反边来创建图 */
	ret = igraph_create(&g1, &v1, 0, 0);
	if (ret != 0)
		return -1;



	int nVec = igraph_vcount(&g1);
	int nEdge = igraph_ecount(&g1);

	// 根据开奖号码的相邻性，来添加边
	for (int i = 0; i < nCount; i++)
	{
		int nFromRow = (p[i] - 1) / nNumPerLine;
		int	nFromCol = (p[i] - 1) % nNumPerLine;

		for (int j = 0; j < nCount; j++)
		{
			if (i != j)
			{
				int nToRow = (p[j] - 1) / nNumPerLine;
				int	nToCol = (p[j] - 1) % nNumPerLine;

				if (nToRow == nFromRow && (nToCol - nFromCol == 1 || nToCol - nFromCol == -1) ||
					nToCol == nFromCol && (nToRow - nFromRow == 1 || nToRow - nFromRow == -1))
				{
					/*if (isLog)
					{
						stringstream ss;
						ss << "from " << p[i] << " --- to " << p[j];
						LOG4CPLUS_DEBUG(g_logger, ss.str());
					}*/

					/* Add edges */
					igraph_vector_resize(&v1, 2);
					VECTOR(v1)[0] = p[i]; VECTOR(v1)[1] = p[j];
					igraph_add_edges(&g1, &v1, 0);
				}
			}
		}
	}

	igraph_vector_destroy(&v1);

	ret = igraph_simplify(&g1, true, true, 0);
	if (ret != 0)
	{
		igraph_destroy(&g1);
		return -2;
	}

	int  nVec1 = igraph_vcount(&g1);
	int nEdge1 = igraph_ecount(&g1);
	if (nEdge1 == 0)
	{
		igraph_destroy(&g1);
		return nCount;
	}
	else if (isLog)
	{
		ss.str("");
		ss << "nVec1 = " << nVec1 << ",nEdge1 = " << nEdge1;
		// LOG4CPLUS_DEBUG(g_logger, ss.str());
	}

	// 对图进行广度优先遍历, vecGroup 用来保存每个群包含的点标识
	int vecGroup[10][10];
	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 10; j++)
			vecGroup[i][j] = 0;

	int nRow = 0;
	for (int i = 0; i < nCount; i++)
	{
		int num = p[i];
		bool bFound = false;
		for (int k = 0; k < 10 && vecGroup[k][0]; k++)
		{
			for (int j = 0; j < 10 && vecGroup[k][j]; j++)
			{
				if (vecGroup[k][j] == num)
				{
					bFound = true;
					break;
				}
			}
			if (bFound)	break;
		}
		if (bFound) continue;

		igraph_vector_t vids, layers, parents;
		igraph_vector_init(&vids, 0);
		igraph_vector_init(&layers, 0);
		igraph_vector_init(&parents, 0);
		ret = igraph_i_bfs(&g1, num, IGRAPH_ALL, &vids, &layers, &parents);

		int	vec[10];
		for (int k = 0; k < 10; k++)
		{
			vec[k] = 0;
		}

		/*if (isLog)
		{
			ss.str("");
			ss << "vids = ";
			for (int k = 0; k < igraph_vector_size(&vids) && k < 10; k++)
			{
				ss << VECTOR(vids)[k] << " ";
			}
			LOG4CPLUS_DEBUG(g_logger, ss.str());

			ss.str("");
			ss << "layers = ";
			for (int k = 0; k < igraph_vector_size(&layers) && k < 10; k++)
			{
				ss << VECTOR(layers)[k] << " ";
			}
			LOG4CPLUS_DEBUG(g_logger, ss.str());
		}*/


		int nVec = 0;
		for (int k = 0; k < igraph_vector_size(&vids); k++) {
			int nei = VECTOR(vids)[k];
			int nLayer = VECTOR(layers)[k];
			int nParent = VECTOR(parents)[k];
			if (is_in_group(nei, p, nCount))
			{
				vec[nVec] = nei;
				nVec++;
				if (nVec == 10)
				{
					break;
				}
			}
			else
				break;
		}

		if (nVec < 10)
		{
			for (int k = 0; k < nVec; k++)
				vecGroup[nRow][k] = vec[k];
			nRow++;

			/*if (isLog)
			{
				ss.str("");
				ss << "vec[" << (nRow - 1) << "] = ";
				for (int k = 0; k < nVec; k++)
				{
					ss << vec[k] << " ";
				}
				LOG4CPLUS_DEBUG(g_logger, ss.str());
			}*/
		}

		igraph_vector_destroy(&vids);
		igraph_vector_destroy(&layers);
		igraph_vector_destroy(&parents);

	}

	igraph_destroy(&g1);
	return nRow;
}

LotteryNumber  CalcCharacter(int regular_num, int total_num, LotteryNumber lotto,  
	vector<int> region_start, vector<int> region_end, vector<double> vecFred, bool bContainSpecial)
{
	static long baseTime = 0;
	static long consecutiveTime = 0;
	static long StatistiTime = 0;
	static long groupTime = 0;

	int nNumber[10];
	for (int i = 0; i < 10; i++)	nNumber[i] = 0;
	for (int i = 0; i < regular_num; i++)
	{
		nNumber[i] = lotto.number[i];
	}
	// 计算包含特别号, bContainSpecail = true 要确保有特别号
	if (bContainSpecial)
	{
		nNumber[regular_num] = lotto.special_number;
		regular_num++;
		BubbleSort(nNumber, regular_num);
	}
	
	long tick1 = GetTickCount();
	////////////////////////////////////////////////////////////////
	BaseCharacter(nNumber, regular_num, total_num, lotto, region_start, region_end);

	long tick2 = GetTickCount();
	baseTime += tick2 - tick1;
	tick1 = tick2;

	// 连号分析
	consecutive_number(nNumber, regular_num, lotto.Consecutive0, lotto.Consecutive1, lotto.Consecutive2);

	tick2 = GetTickCount();
	consecutiveTime += tick2 - tick1;
	tick1 = tick2;

	/////////////////////////////////////////////////////////////
	// 统计特征分析
	StatisticCharacter(nNumber, regular_num, total_num, lotto);

	tick2 = GetTickCount();
	StatistiTime += tick2 - tick1;
	tick1 = tick2;
	
	////////////////////////////////////////////////////////////////////
	//	高级特征分析
	
	// 计算集团数
	lotto.group_qty = group_number(nNumber, regular_num, NUM_PER_LINE);

	tick2 = GetTickCount();
	groupTime += tick2 - tick1;
	tick1 = tick2;

	//	计算 AC 值
	lotto.ac = AC_number(nNumber, regular_num, total_num);

	//	边缘数
	lotto.fringe_no_qty = fringe_no_qty_number(nNumber, regular_num, total_num);

	if (vecFred.size() >= 5)
	{
		lotto.fred = vecFred[0] + vecFred[1] * lotto.ac + vecFred[2] * log((double)lotto.sum)
			+ vecFred[3] * lotto.fringe_no_qty + vecFred[4] * lotto.group_qty;
	}
	return lotto;
}
