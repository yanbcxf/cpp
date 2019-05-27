#pragma once

#include "Error.h"
#include <WinSvc.h>

namespace ZvLibs
{
	class CService
	{
	public:
		CService(const string & szName = "");
		virtual ~CService();

		bool SvcInstall();
		bool SvcRemove();

		bool SvcCheckExist(bool & bExist, bool * pbSamePath = NULL);
		void SvcClose();
		SC_HANDLE SvcOpen(SC_HANDLE hSvcMgr);
		bool SvcStop(SC_HANDLE hSvc);

		bool SvcDispatch();

		SC_HANDLE OpenScMgr();

		static void WINAPI SvcMain(DWORD dwArgc, LPTSTR* lpszArgv);
		static void WINAPI SvcCtrlHandler(DWORD dwCtrl);

		void ReportSvcStatus(DWORD dwCurrentState,	DWORD dwWin32ExitCode, DWORD dwWaitHint);

		virtual bool Run() = 0;
		virtual void Stop() = 0;

		bool SvcStart();
		bool SvcGetStatus(LPSERVICE_STATUS Status);
		bool SvcRestart();

		string m_szName;

		SERVICE_STATUS m_SvcStatus;
		SERVICE_STATUS_HANDLE m_hSvcStatus;

		static CService * m_pSvc;
	};

}