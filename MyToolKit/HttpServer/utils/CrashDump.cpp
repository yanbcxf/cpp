#include "stdafx.h"
#include "CrashDump.h"

#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")

namespace ZvLibs
{
	CCrashDump * CCrashDump::m_pCrashDump = NULL;

	CCrashDump::CCrashDump(const string & szDumpFilePath)
	{
		SetFilePath(szDumpFilePath);

		m_pCrashDump = this;

		SetUnhandledExceptionFilter(TopLevelFilter);

		DisableSetUnhandledExceptionFilter();
	}

	CCrashDump::~CCrashDump()
	{

	}

	void CCrashDump::SetFilePath(const string & szDumpFilePath)
	{
		if(szDumpFilePath.length()<0)
			m_szDumpFilePath = ".\\crash.dmp";
		else
			m_szDumpFilePath = szDumpFilePath;		
	}

	bool CCrashDump::CrashHandle(struct _EXCEPTION_POINTERS *pExceptionInfo)
	{
		ZvLibs::MakeDirectory(GetFileDirPath(m_szDumpFilePath));

		HANDLE hFile = CreateFile(_T(m_pCrashDump->m_szDumpFilePath.c_str()), GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); 

		if(hFile==INVALID_HANDLE_VALUE) return false;

		MINIDUMP_EXCEPTION_INFORMATION mdei;

		mdei.ThreadId           = GetCurrentThreadId(); 
		mdei.ExceptionPointers  = pExceptionInfo;
		mdei.ClientPointers     = NULL; 

		BOOL b = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, (pExceptionInfo!=NULL)?&mdei:NULL, NULL, NULL );

		CloseHandle(hFile);

		return b?true:false;
	}

	LONG CCrashDump::TopLevelFilter(struct _EXCEPTION_POINTERS *pExceptionInfo)
	{
		return m_pCrashDump->CrashHandle(pExceptionInfo)?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH;
	}

	void CCrashDump::DisableSetUnhandledExceptionFilter()
	{

		#ifndef _M_IX86
		#error "The following code only works for x86!"
		#endif

		void *addr = (void*)GetProcAddress(LoadLibrary(_T("kernel32.dll")),

			"SetUnhandledExceptionFilter");

		if (addr)

		{

			unsigned char code[16];

			int size = 0;

			//xor eax,eax;

			code[size++] = 0x33;

			code[size++] = 0xC0;

			//ret 4

			code[size++] = 0xC2;

			code[size++] = 0x04;

			code[size++] = 0x00;



			DWORD dwOldFlag, dwTempFlag;

			VirtualProtect(addr, size, PAGE_READWRITE, &dwOldFlag);

			WriteProcessMemory(GetCurrentProcess(), addr, code, size, NULL);

			VirtualProtect(addr, size, dwOldFlag, &dwTempFlag);

		}

	}

}