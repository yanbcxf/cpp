#include "stdafx.h"
#include "UtfConverter.h"

namespace ZvLibs
{

	tstring Utf8ToTstring(const string & szSrc)
	{
		string szStr;
		int i;

		wstring szWstr;

		i = MultiByteToWideChar(CP_UTF8, 0, szSrc.c_str(), -1, NULL, 0);
		szWstr.resize(i-1);

		MultiByteToWideChar(CP_UTF8, 0, szSrc.c_str(), -1, (LPWSTR)szWstr.c_str(), i); 

	#ifdef _UNICODE

		szStr = szWstr;

	#else

		i = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)szWstr.c_str(), -1, NULL,0,NULL,NULL);
		szStr.resize(i-1);
		WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)szWstr.c_str(), -1, (LPSTR)szStr.c_str(), i, NULL, NULL);

	#endif

		return szStr;
	}

	string TstringToUtf8(const tstring & szSrc)
	{
		string szStr;
		int i;

	#ifdef _UNICODE
		
		i = WideCharToMultiByte(CP_UTF8, 0, szSrc.c_str(), -1, NULL,0,NULL,NULL);
		szStr.resize(i-1);
		WideCharToMultiByte(CP_UTF8, 0, szSrc.c_str(), -1, (LPSTR)szStr.c_str(), i, NULL, NULL);

	#else

		wstring szWstr;

		i = MultiByteToWideChar(CP_ACP, 0, szSrc.c_str(), -1, NULL, 0);

		szWstr.resize(i-1);

		MultiByteToWideChar(CP_ACP, 0, szSrc.c_str(), -1, (LPWSTR)szWstr.c_str(), i); 
		i = WideCharToMultiByte(CP_UTF8, 0, szWstr.c_str(), -1, NULL,0,NULL,NULL);

		szStr.resize(i-1);
	
		WideCharToMultiByte(CP_UTF8, 0, szWstr.c_str(), -1, (LPSTR)szStr.c_str(), i, NULL, NULL);

	#endif

		return szStr;
	}

	tstring StringToTstring(const string & szSrc)
	{
		#ifdef _UNICODE

		int i;
		wstring szWstr;

		i = MultiByteToWideChar(CP_ACP, 0, szSrc.c_str(), -1, NULL, 0);
		szWstr.resize(i);
		MultiByteToWideChar(CP_ACP, 0, szSrc.c_str(), -1, (LPWSTR)szWstr.c_str(), i); 
		return szWstr;
		
		#else

		return szSrc;

		#endif
	}

}