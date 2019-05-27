
#include "stdafx.h"
#include "ZvUtils.h"

#include <iomanip>
#pragma comment(lib, "Version.lib")

namespace ZvLibs
{
	tstring & StringTrim(tstring & str, const tstring & szTrim)
	{
		tstring::size_type pos1 = str.find_first_not_of(szTrim);
		tstring::size_type pos2 = str.find_last_not_of(szTrim);
		str = str.substr(pos1==tstring::npos?0:pos1, pos2==tstring::npos?str.length()-1:pos2-pos1+1);
		return str;
	}

	tstring & StringReplace(tstring & str, const tstring & old_value, const tstring & new_value)  
	{  
		for(tstring::size_type pos(0); pos!=tstring::npos; pos+=new_value.length())   
		{  
			if((pos=str.find(old_value,pos)) != tstring::npos)  
				str.replace(pos,old_value.length(),new_value);  
			else   
				break;  
		}  
		return str;  
	}   

	tstring GetExePath()
	{
		TCHAR buf[MAX_PATH] = {0};
		GetModuleFileName(NULL, buf ,MAX_PATH);
		return buf;
	}

	tstring GetAppPath()
	{
		TCHAR buf[MAX_PATH] = {0};
		GetModuleFileName(NULL, buf ,MAX_PATH);

		tstring str = buf;

		str = str.substr(0, str.rfind('\\'));

		//	yangbin : 改为以 工作目录为web 的起点
		int nPos = GetCurrentDirectory(MAX_PATH, buf);
		if (nPos >= 0)
			str = buf;
		
		return str;
	}

	tstring GetStrFileSize(double fSize)
	{
		tostringstream ss;

		if(fSize<1024)
			ss << fixed << setprecision(2) << fSize << " Bytes";
		else if(fSize<1024*1024)
			ss << fixed << setprecision(2) << fSize/(double)1024 << " KB";
		else if(fSize<1024*1024*1024)
			ss << fixed << setprecision(2) << fSize/(double)(1024*1024) << " MB";
		else if(fSize<(double)1024*1024*1024*1024)
			ss << fixed << setprecision(2) << fSize/(double)(1024*1024*1024) << " GB";
		else
			ss << fixed << setprecision(2) << fSize/(double)((double)1024*1024*1024*1024) << " TB";

		return ss.str();
	}

	tstring GetFileDirPath(const tstring & szFilePath)
	{
		tstring str = szFilePath;
		ToWinPath(str);

		if(str.rfind('\\')!=tstring::npos)
			return str.substr(0, str.rfind('\\'));
		return _T("");
	}

	tstring GetFileName(const tstring & szFilePath)
	{
		tstring str = szFilePath;
		StringReplace(str, _T("\\"), _T("/"));

		int p = str.rfind(_T("/"));

		if(p!=tstring::npos && p!=str.length()-1)
		{
			return str.substr(p+1, tstring::npos);
		}

		return str;
	}	

	tstring GetExeName()
	{
		TCHAR buf[MAX_PATH] = {0};

		GetModuleFileName(NULL, buf ,MAX_PATH);

		tstring str = buf;

		str = str.substr(str.rfind('\\')+1, tstring::npos);

		return str;
	}

	tstring & ToWinPath(tstring & szPath)
	{
		StringReplace(szPath, _T("/"), _T("\\"));
		StringReplace(szPath, _T(".\\"), GetAppPath() + _T("\\"));
		return szPath;
	}

	bool DelFile(const tstring & szPath)
	{
		tstring str = szPath;

		ToWinPath(str);

		return DeleteFile(str.c_str())==TRUE;
	}

	tstring MakeDirectory(const tstring & szPath)
	{
		int index, start = 0;
		tstring strTmp;
		tstring strPath = szPath;
		StringReplace(strPath, _T("/"), _T("\\"));
		StringReplace(strPath, _T(".\\"), GetAppPath()+_T("\\"));

		while((index = strPath.find('\\', start)) != tstring::npos)
		{  
			strTmp = strPath.substr(0, index);
			CreateDirectory(strTmp.c_str(),   NULL);  
			start = index + 1;
		}  

		if(strPath.length() != strTmp.length()+1)
			CreateDirectory(strPath.c_str(), NULL);

		if(GetFileAttributes(strPath.c_str())==0xFFFFFFFF)
			return _T("");

		return strPath;
	}

	int GetRandomInt()
	{
		static DWORD dwRandom = rand();
		
		srand(dwRandom+GetTickCount());

		dwRandom = rand();
		return dwRandom;
	}

	tstring GetRandomString(int nLen)
	{
		vector<char> vtChar;
		tstring str;

		GetRandomInt();

		char c;
		for(c='0'; c<='9'; c++)
		{
			vtChar.push_back(c);
		}

		for(c='a'; c<='z'; c++)
		{
			vtChar.push_back(c);
		}

		for(int i=0; i<nLen; i++)
		{
			random_shuffle(vtChar.begin(), vtChar.end());
			str += vtChar[0];
		}

		return str;
	}

	bool FileExist(const tstring & szFilePath, LPWIN32_FIND_DATA pFd)
	{
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind;

		tstring szWinPath = szFilePath;
		ToWinPath(szWinPath);

		hFind = FindFirstFile(szWinPath.c_str(), &FindFileData);
		if (hFind == INVALID_HANDLE_VALUE) 
			return false;

		if(pFd!=NULL)
		{
			*pFd = FindFileData;
		}

		FindClose(hFind);
		return true;
	}

	void StringSplit(const tstring & szStr, const tstring & szSplitter, vector<tstring> & vtStr, const tstring & szEndStr)
	{
		vtStr.clear();

		if(szStr.length()<=0 || szSplitter.length()<=0) return;

		tstring str = szStr;
		tstring strSplitter = szSplitter;

		size_t i,j;
		size_t l=strSplitter.size();

		i = j = 0;
		tstring strSub;
		size_t k = szEndStr==_T("")?str.npos:str.find(szEndStr);

		while(1)
		{
			j = str.find(szSplitter, i);
			if(j>k)
				j=k;
			strSub = str.substr(i,j-i);
			if(strSub.size()>0)
				vtStr.push_back(strSub);
			i=j+l;
			if(j==str.npos || j>=k)
				break;
		}
	}

	tstring GetOsVersion()
	{
		tostringstream ss;
		#define BUFSIZE 80

		typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);

		OSVERSIONINFOEX osvi;
		SYSTEM_INFO si;
		PGNSI pGNSI;
		BOOL bOsVersionInfoEx;

		ZeroMemory(&si, sizeof(SYSTEM_INFO));
		ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));

		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

		if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
		{
			osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
			if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
				return _T("");
		}

		// Call GetNativeSystemInfo if supported
		// or GetSystemInfo otherwise.

		pGNSI = (PGNSI) GetProcAddress(
			GetModuleHandle(TEXT("kernel32.dll")), 
			"GetNativeSystemInfo");
		if(NULL != pGNSI)
			pGNSI(&si);
		else ::GetSystemInfo(&si);

		switch (osvi.dwPlatformId)
		{
			// Test for the Windows NT product family.

		case VER_PLATFORM_WIN32_NT:

			// Test for the specific product.

			if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0 )
			{
				if( osvi.wProductType == VER_NT_WORKSTATION )
					ss << ("Windows Vista ");
				else ss << ("Windows Server 2008 " );
			}

			if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
			{
				if( GetSystemMetrics(89) )
					ss << ( "Microsoft Windows Server 2003 \"R2\" ");
				else if( osvi.wProductType == VER_NT_WORKSTATION &&
					si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
				{
					ss << ( "Microsoft Windows XP Professional x64 Edition ");
				}
				else ss <<  ("Microsoft Windows Server 2003, ");
			}

			if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
				ss << ("Microsoft Windows XP ");

			if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
				ss << ("Microsoft Windows 2000 ");

			if ( osvi.dwMajorVersion <= 4 )
				ss << ("Microsoft Windows NT ");

			// Test for specific product on Windows NT 4.0 SP6 and later.
			if( bOsVersionInfoEx )
			{
				// Test for the workstation type.
				if ( osvi.wProductType == VER_NT_WORKSTATION &&
					si.wProcessorArchitecture!=PROCESSOR_ARCHITECTURE_AMD64)
				{
					if( osvi.dwMajorVersion == 4 )
						ss << ( "Workstation 4.0 " );
					else if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
						ss << ( "Home Edition " );
					else ss << ( "Professional " );
				}

				// Test for the server type.
				else if ( osvi.wProductType == VER_NT_SERVER || 
					osvi.wProductType == VER_NT_DOMAIN_CONTROLLER )
				{
					if(osvi.dwMajorVersion==5 && osvi.dwMinorVersion==2)
					{
						if ( si.wProcessorArchitecture ==
							PROCESSOR_ARCHITECTURE_IA64 )
						{
							if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
								ss << ( "Datacenter Edition "
								"for Itanium-based Systems" );
							else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
								ss << ( "Enterprise Edition "
								"for Itanium-based Systems" );
						}

						else if ( si.wProcessorArchitecture ==
							PROCESSOR_ARCHITECTURE_AMD64 )
						{
							if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
								ss << ( "Datacenter x64 Edition " );
							else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
								ss << ( "Enterprise x64 Edition " );
							else ss << ( "Standard x64 Edition " );
						}

						else
						{
							if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
								ss << ( "Datacenter Edition " );
							else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
								ss << ( "Enterprise Edition " );
							else if ( osvi.wSuiteMask & VER_SUITE_BLADE )
								ss << ( "Web Edition " );
							else ss << ( "Standard Edition " );
						}
					}
					else if(osvi.dwMajorVersion==5 && osvi.dwMinorVersion==0)
					{
						if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
							ss << ( "Datacenter Server " );
						else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
							ss << ( "Advanced Server " );
						else ss << ( "Server " );
					}
					else  // Windows NT 4.0 
					{
						if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
							ss << ("Server 4.0, Enterprise Edition " );
						else ss << ( "Server 4.0 " );
					}
				}
			}
			// Test for specific product on Windows NT 4.0 SP5 and earlier
			else  
			{
				HKEY hKey;
				TCHAR szProductType[BUFSIZE];
				DWORD dwBufLen=BUFSIZE*sizeof(TCHAR);
				LONG lRet;

				lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
					TEXT("SYSTEM\\CurrentControlSet\\Control\\ProductOptions"), 0, KEY_QUERY_VALUE, &hKey );
				if( lRet != ERROR_SUCCESS )
					return _T("");

				lRet = RegQueryValueEx( hKey, TEXT("ProductType"),
					NULL, NULL, (LPBYTE) szProductType, &dwBufLen);
				RegCloseKey( hKey );

				if( (lRet != ERROR_SUCCESS) ||
					(dwBufLen > BUFSIZE*sizeof(TCHAR)) )
					return _T("");

				if ( lstrcmpi( TEXT("WINNT"), szProductType) == 0 )
					ss << ( "Workstation " );
				if ( lstrcmpi( TEXT("LANMANNT"), szProductType) == 0 )
					ss << ( "Server " );
				if ( lstrcmpi( TEXT("SERVERNT"), szProductType) == 0 )
					ss << ( "Advanced Server " );
				ss << osvi.dwMajorVersion << "." << osvi.dwMinorVersion;
			}

			// Display Service pack (if any) and build number.

			if( osvi.dwMajorVersion == 4 && 
				lstrcmpi( osvi.szCSDVersion, TEXT("Service Pack 6") ) == 0 )
			{ 
				HKEY hKey;
				LONG lRet;

				// Test for SP6 versus SP6a.
				lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
					TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Hotfix\\Q246009"), 0, KEY_QUERY_VALUE, &hKey );
				if( lRet == ERROR_SUCCESS )
					ss << "Service Pack 6a (Build " << (osvi.dwBuildNumber & 0xFFFF) << ")\n";   
				else // Windows NT 4.0 prior to SP6a
				{
					ss << osvi.szCSDVersion << " (Build " << (osvi.dwBuildNumber & 0xFFFF) << ")\n";
				}

				RegCloseKey( hKey );
			}
			else // not Windows NT 4.0 
			{
				ss << osvi.szCSDVersion << " (Build " << (osvi.dwBuildNumber & 0xFFFF) << ")\n";
			}

			break;

			// Test for the Windows Me/98/95.
		case VER_PLATFORM_WIN32_WINDOWS:

			if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
			{
				ss << ("Microsoft Windows 95 ");
				if (osvi.szCSDVersion[1]=='C' || osvi.szCSDVersion[1]=='B')
					ss << ("OSR2 " );
			} 

			if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
			{
				ss << ("Microsoft Windows 98 ");
				if ( osvi.szCSDVersion[1]=='A' || osvi.szCSDVersion[1]=='B')
					ss << ("SE " );
			} 

			if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
			{
				ss << ("Microsoft Windows Millennium Edition\n");
			} 
			break;

		case VER_PLATFORM_WIN32s:

			ss << ("Microsoft Win32s\n");
			break;
		}
		return ss.str(); 
	}

	tstring & ChangeQuotationMarks(tstring & szSrc)
	{
		StringReplace(szSrc, _T("'"), _T("''"));
		return szSrc;
	}	

	tstring & EscapeJsonCharacter(tstring & szStr)
	{
		StringReplace(szStr, _T("\\"), _T("\\\\"));
		StringReplace(szStr, _T("\""), _T("\\\""));
		StringReplace(szStr, _T("\b"), _T("\\b"));
		StringReplace(szStr, _T("\t"), _T("\\t"));
		StringReplace(szStr, _T("\r"), _T("\\r"));
		StringReplace(szStr, _T("\n"), _T("\\n"));

		return szStr;
	}

	tstring & EscapeHtmlCharacter(tstring & szStr)
	{
		StringReplace(szStr, _T("&"), _T("&amp;"));
		StringReplace(szStr, _T("'"), _T("&apos;"));
		StringReplace(szStr, _T("\""), _T("&quot;"));
		StringReplace(szStr, _T("<"), _T("&lt;"));
		StringReplace(szStr, _T(">"), _T("&gt;"));
		// StringReplace(szStr, "'", "''");

		return szStr;
	}	

	tstring GetFileVersion()
	{
		tstring szVersion;
		
		DWORD	dwTemp, dwSize, dwHandle = 0;
		BYTE	*pData = NULL;
		UINT	uLen;
		TCHAR	path[MAX_PATH] = {0};
		VS_FIXEDFILEINFO   *lpInfo;   
		
		GetModuleFileName(NULL, path, MAX_PATH);
		dwSize = GetFileVersionInfoSize(path, &dwTemp);
		
		if (dwSize > 0) 
		{
			pData = new BYTE[dwSize];
			
			if (GetFileVersionInfo(path, dwHandle, dwSize, pData))
			{
				if(VerQueryValue(pData, _T("\\"), (LPVOID*)&lpInfo, &uLen))
				{
					tostringstream ss;
					ss << HIWORD(lpInfo->dwFileVersionMS) << "."
					   << LOWORD(lpInfo->dwFileVersionMS) << "."
					   << HIWORD(lpInfo->dwFileVersionLS) << "."
					   << LOWORD(lpInfo->dwFileVersionLS);
					szVersion = ss.str();
				}
			}
		}
		
		if(pData)
			delete [] pData;
		
		return szVersion;
	}

	tstring GetSysTmOneDaySpan(SYSTEMTIME & tmStart, SYSTEMTIME & tmEnd)
	{
		unsigned __int64 tmLen;
		
		ULARGE_INTEGER fTime1;
		ULARGE_INTEGER fTime2;
		
		SystemTimeToFileTime(&tmStart,(FILETIME*)&fTime1);  
		SystemTimeToFileTime(&tmEnd,(FILETIME*)&fTime2); 

		tmLen = abs((int)((fTime2.QuadPart-fTime1.QuadPart)/10000000));

		tostringstream ss;

		ss << setw(2) << (int)(tmLen%3600) << ":" << (int)((tmLen-tmLen%3600*3600)%60) << ":" << (int)(tmLen%60);

		return ss.str();
	}

	tstring GetFormatedSysTm(SYSTEMTIME & SysTm, bool bOneDay)
	{
		tostringstream ss;
		
		if(bOneDay)
			ss << setw(2) << SysTm.wHour << ":" << SysTm.wMinute << ":" << SysTm.wSecond; 
		else
			ss << SysTm.wYear << "-" << setw(2) << SysTm.wMonth << "-" << SysTm.wDay << " " << SysTm.wHour << ":" << SysTm.wMinute << ":" << SysTm.wSecond; 
		
		return ss.str();
	}

	int CompareSysTm(SYSTEMTIME & tm1, SYSTEMTIME & tm2)
	{
		ULARGE_INTEGER fTime1;
		ULARGE_INTEGER fTime2;
		
		SystemTimeToFileTime(&tm1, (FILETIME*)&fTime1);  
		SystemTimeToFileTime(&tm2, (FILETIME*)&fTime2); 

		return (fTime1.QuadPart==fTime2.QuadPart)?0:(fTime1.QuadPart>fTime2.QuadPart?1:-1);
	}

	tstring GetFormatedTmNow()
	{
		SYSTEMTIME tmNow;
		GetLocalTime(&tmNow);

		tostringstream ss;
		
		ss << tmNow.wYear << "-" << setw(2) << tmNow.wMonth << "-" << tmNow.wDay << " " << tmNow.wHour << ":" << tmNow.wMinute << ":" << tmNow.wSecond; 
		
		return ss.str();
	}

	bool GetDiskSpaceInfo(char cLetter, double * pTotal, double * pUsed, double * pPercentUsed, double * pLeft, double * pPercentLeft)
	{
		DWORD dwMask = GetLogicalDrives();
		DWORD Disk;
		TCHAR szBuf[10];
		tstring szAppPath;

		if(toupper(cLetter)<'A' || toupper(cLetter)>'Z')
			szAppPath = GetAppPath();
		else
			szAppPath = toupper(cLetter);

		if(szAppPath.length()<=0) return false;

		for(Disk='C'; Disk<='Z'; Disk++)
		{
			tsprintf(szBuf, _T("%c:\\"), Disk);

			if(dwMask & (1 << (Disk-'A')))
			{
				if(szBuf[0]==toupper(szAppPath[0]))
				{
					DWORD SectorsPerCluster, BytesPerSector, NumberOfFreeClusters, TotalNumberOfClusters;
					double Total, Used, PercentUsed, Left, PercentLeft;

					if(GetDiskFreeSpace(szBuf, &SectorsPerCluster, &BytesPerSector, &NumberOfFreeClusters, &TotalNumberOfClusters))
					{
						Total = (double)TotalNumberOfClusters*SectorsPerCluster*BytesPerSector;
						Used = (double)(TotalNumberOfClusters-NumberOfFreeClusters)*SectorsPerCluster*BytesPerSector;
						PercentUsed = 100*Used/(Total==0?1:Total);
						Left = Total-Used;
						PercentLeft = 100-PercentUsed;

						if(pTotal!=NULL) *pTotal = Total;
						if(pUsed!=NULL) *pUsed = Used;
						if(pPercentUsed!=NULL) *pPercentUsed = PercentUsed;
						if(pLeft!=NULL) *pLeft = Left;
						if(pPercentLeft!=NULL) *pPercentLeft = PercentLeft;

						return true;
					}
				}
			}
		}

		return false;
	}

	bool ExeCmd(const tstring & szCmdLine, int nTimeoutMil, tstring & szOutput, DWORD & dwExitCode)
	{
		SECURITY_ATTRIBUTES sa; 
		ZeroMemory(&sa, sizeof(sa));
		sa.nLength = sizeof(sa);
		sa.bInheritHandle = TRUE;
		tstring szTxtPath = GetAppPath()+_T("\\temp\\")+GetRandomString(8);

		HANDLE hTxtFile = CreateFile(szTxtPath.c_str(), GENERIC_WRITE|GENERIC_READ, 0, &sa, CREATE_ALWAYS, 0, NULL);
		if(hTxtFile==INVALID_HANDLE_VALUE)
		{
			return false;
		}
	
		STARTUPINFO si; 
		ZeroMemory(&si, sizeof(si));

		PROCESS_INFORMATION pi; 
		ZeroMemory(&pi, sizeof(pi));
		si.cb=sizeof(si);
		si.dwFlags=STARTF_USESTDHANDLES;
		si.hStdInput=NULL; 
		si.hStdOutput=hTxtFile; 
		si.hStdError=hTxtFile;

		if(!CreateProcess(0, (LPTSTR)szCmdLine.c_str(), 0, 0, TRUE, NORMAL_PRIORITY_CLASS|CREATE_NO_WINDOW, 0, 0, &si, &pi))
		{
			CloseHandle(hTxtFile);
			DeleteFile(szTxtPath.c_str());
			return false;
		}

		CloseHandle(pi.hThread);
		if(WaitForSingleObject(pi.hProcess, nTimeoutMil)==WAIT_TIMEOUT)
		{
			TerminateProcess(pi.hProcess, 1);
		}
		else
			GetExitCodeProcess(pi.hProcess, &dwExitCode);

		CloseHandle(pi.hProcess);
		CloseHandle(hTxtFile);

		TCHAR buf[4096] = {0};
		DWORD dwRead;

		hTxtFile = CreateFile(szTxtPath.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
		if(hTxtFile!=INVALID_HANDLE_VALUE)
		{
			if(ReadFile(hTxtFile, buf, 4096, &dwRead, 0) && dwRead>0)
			{
				szOutput = buf;
			}
			CloseHandle(hTxtFile);
			DeleteFile(szTxtPath.c_str());
		}
		
		return true;
	}
};



