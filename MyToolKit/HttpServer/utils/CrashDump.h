#pragma once

namespace ZvLibs
{
	class CCrashDump
	{
	public:
		CCrashDump(const string & szDumpFilePath = "");
		virtual ~CCrashDump();

		static LONG WINAPI TopLevelFilter(struct _EXCEPTION_POINTERS *pExceptionInfo);
		virtual bool CrashHandle(struct _EXCEPTION_POINTERS *pExceptionInfo);
		static void DisableSetUnhandledExceptionFilter();
		void SetFilePath(const string & szDumpFilePath);
		string m_szDumpFilePath;

		static CCrashDump * m_pCrashDump;
	};

}