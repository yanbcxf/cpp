
// stdafx.cpp : 只包括标准包含文件的源文件
// CostEngineer.pch 将作为预编译标头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"
// #include <iconv.h>


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

	/*iconv_t c_pt = iconv_open("utf-8", "gbk");
	if (c_pt == (iconv_t)-1) return ("ConvertEnc iconv_open error");


	iconv(c_pt, NULL, NULL, NULL, NULL);
	ret = iconv(c_pt, (const char **)&sin, (size_t *)&lenin, &sout, (size_t *)&lenout);

	if (ret == -1) return ("ConvertEnc Convert Failed");

	iconv_close(c_pt);*/

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

		/*iconv_t c_pt = iconv_open("gbk//IGNORE", "utf-8");
		if (c_pt == (iconv_t)-1)
		{
			target = "ConvertEnc iconv_open error";
			break;
		}

		iconv(c_pt, NULL, NULL, NULL, NULL);
		ret = iconv(c_pt, (const char **)&sin, (size_t *)&lenin, &sout, (size_t *)&lenout);

		iconv_close(c_pt);*/

		if (ret == -1)
			target = "ConvertEnc Convert Failed";
		else
			target = string(bufout);

	} while (0);

	if (bufout)
		free(bufout);
	return target;
}