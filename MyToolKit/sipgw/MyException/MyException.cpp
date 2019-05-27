#include "MyException.h"

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include  <dbghelp.h>


#pragma comment(lib,  "dbghelp.lib")

///创建Dump文件
void CreateDumpFile(LPCTSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException)  
{  
	HANDLE hDumpFile = CreateFile(lpstrDumpFilePathName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);  

	// Dump信息  
	MINIDUMP_EXCEPTION_INFORMATION dumpInfo;  
	dumpInfo.ExceptionPointers = pException;  
	dumpInfo.ThreadId = GetCurrentThreadId();  
	dumpInfo.ClientPointers = TRUE;  

	// 写入Dump文件内容  
	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);  

	CloseHandle(hDumpFile);  
}  

LONG WINAPI CrashHandler(EXCEPTION_POINTERS *pException)
{
	char cPath[256]={0};
	sprintf(cPath,"./crash.dmp");

	CreateDumpFile(cPath, pException);

	//MessageBox(NULL, _T("程序活不成要死掉了！尸体在：deathbody.dmp"), _T("摊上大事了"), MB_OK);

	return EXCEPTION_CONTINUE_SEARCH;
}

void SetMyException()
{
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)CrashHandler);// 设置未处理异常的处理函数
}