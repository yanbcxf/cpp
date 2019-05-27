#include "stdafx.h"
#include "Service.h"

#include <iomanip>

namespace ZvLibs
{
	CService * CService::m_pSvc = NULL;

	CService::CService(const string & szName)
	{
		if(szName=="")
			m_szName = GetExeName();
		else
			m_szName = szName;

		m_pSvc = this;
	}

	CService::~CService()
	{

	}

	bool CService::SvcDispatch()
	{
		SERVICE_TABLE_ENTRY DispatchTable[] =
		{
			{ TEXT((LPSTR)m_szName.c_str()), (LPSERVICE_MAIN_FUNCTION)SvcMain},
			{ NULL, NULL }
		};

		BOOL b = StartServiceCtrlDispatcher(DispatchTable);

		return b?true:false;
	}

	// 打开服务
	SC_HANDLE CService::SvcOpen(SC_HANDLE hScMgr)
	{
		return OpenService(hScMgr, m_szName.c_str(), SC_MANAGER_ALL_ACCESS);
	}

	// 打开服务管理器
	SC_HANDLE CService::OpenScMgr()
	{
		return OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	}

	// 检查服务是否存在
	bool CService::SvcCheckExist(bool & bExist, bool * pbSamePath)
	{
		SC_HANDLE hSvcMgr = NULL;
		SC_HANDLE hSvc = NULL;
		bool bRet = false;

		LPQUERY_SERVICE_CONFIG lpsc = NULL; 
		LPSERVICE_DESCRIPTION lpsd = NULL;
		
		hSvcMgr = OpenScMgr();
		hSvc = SvcOpen(hSvcMgr);

		if(hSvc==NULL && ERROR_SERVICE_DOES_NOT_EXIST!=GetLastError())
			goto clearup;

		if(hSvc==NULL)
		{
			bExist = false;
			bRet = true;
			goto clearup;
		}

		bExist = true;


		DWORD dwBytesNeeded, cbBufSize, dwError; 

		if(pbSamePath!=NULL)
		{
			if(!QueryServiceConfig(hSvc, NULL, 0, &dwBytesNeeded))
			{
				dwError = GetLastError();
				if(ERROR_INSUFFICIENT_BUFFER == dwError)
				{
					cbBufSize = dwBytesNeeded;
					lpsc = (LPQUERY_SERVICE_CONFIG) LocalAlloc(LMEM_FIXED, cbBufSize);
				}
				else 
					goto clearup;
			}

			if(!QueryServiceConfig(hSvc, lpsc, cbBufSize, &dwBytesNeeded))
				goto clearup;

			if(!QueryServiceConfig2(hSvc, SERVICE_CONFIG_DESCRIPTION, NULL, 0, &dwBytesNeeded))
			{
				dwError = GetLastError();
				if(ERROR_INSUFFICIENT_BUFFER == dwError)
				{
					cbBufSize = dwBytesNeeded;
					lpsd = (LPSERVICE_DESCRIPTION) LocalAlloc(LMEM_FIXED, cbBufSize);
				}
				else
					goto clearup;
			}

			if(!QueryServiceConfig2(hSvc, SERVICE_CONFIG_DESCRIPTION,(LPBYTE)lpsd, cbBufSize, &dwBytesNeeded)) 
				goto clearup;
			
			string szP1, szP2;
			szP1 = lpsc->lpBinaryPathName;
			szP2 = GetExePath()+" - Service";
			
			transform(szP1.begin(), szP1.end(), szP1.begin(), toupper);
			transform(szP2.begin(), szP2.end(), szP2.begin(), toupper);
		

			*pbSamePath = szP2==szP1;

			//::MessageBox(NULL, lpsc->lpBinaryPathName, "", MB_OK);
			//::MessageBox(NULL, (GetExePath()+" - Service").c_str(), "", MB_OK);

		}

		bRet = true;
		
clearup:

		if(lpsc!=NULL) LocalFree(lpsc);
		if(lpsd!=NULL) LocalFree(lpsd);
		if(hSvcMgr!=NULL) CloseServiceHandle(hSvcMgr);
		if(hSvc!=NULL) CloseServiceHandle(hSvc);

		return bRet;
	}

	bool CService::SvcGetStatus(LPSERVICE_STATUS pStatus)
	{
		SC_HANDLE hSvcMgr, hSvc;

		hSvcMgr = OpenScMgr();
		hSvc = SvcOpen(hSvcMgr);

		bool b = QueryServiceStatus(hSvc, pStatus)?TRUE:FALSE;

		CloseServiceHandle(hSvcMgr);
		CloseServiceHandle(hSvc);

		return b;
	}

	// 删除
	bool CService::SvcRemove()
	{
		bool bExist;

		if(SvcCheckExist(bExist) && !bExist)
			return true;

		SC_HANDLE hSvc, hSvcMgr;

		hSvcMgr = OpenScMgr();
		hSvc = SvcOpen(hSvcMgr);

		bool bRet = SvcStop(hSvc) && DeleteService(hSvc);

		CloseServiceHandle(hSvc);
		CloseServiceHandle(hSvcMgr);

		return bRet;
	}

	// 停止
	bool CService::SvcStop(SC_HANDLE hSvc)
	{
		SC_HANDLE hMgr = NULL;

		if(hSvc==NULL)
		{
			hMgr = OpenScMgr();
			hSvc = SvcOpen(hMgr);
		}

		SERVICE_STATUS ssStatus;

		if(ControlService(hSvc, SERVICE_CONTROL_STOP, &ssStatus)==0 && ERROR_SERVICE_NOT_ACTIVE==GetLastError())
		{
			if(hMgr!=NULL)
			{
				CloseServiceHandle(hMgr);
				CloseServiceHandle(hSvc);
			}
			return true;
		}

		Sleep(1000);
		int i=0;

		while(QueryServiceStatus(hSvc, &ssStatus)&&i++<10)
		{
			if(ssStatus.dwCurrentState == SERVICE_STOP_PENDING)
			{
				Sleep(1000);
			}
			else
				break;
		}

		if(hMgr!=NULL)
		{
			CloseServiceHandle(hMgr);
			CloseServiceHandle(hSvc);
		}

		if(i>10)
		{
			return false;
		}

		return true;
	}


	// 启动
	bool CService::SvcStart()
	{
		SvcInstall();

		SC_HANDLE hMgr = OpenScMgr();
		SC_HANDLE hSvc = SvcOpen(hMgr);

		bool b = StartService(hSvc, 0, NULL) || ERROR_SERVICE_ALREADY_RUNNING==GetLastError();

		CloseServiceHandle(hSvc);
		CloseServiceHandle(hMgr);

		return b;
	}

	bool CService::SvcRestart()
	{
		return SvcStop(NULL) && SvcStart();
	}

	// 安装
	bool CService::SvcInstall()
	{
		bool bExist;
		bool bSamePath;

		if(!SvcCheckExist(bExist, &bSamePath))
		{
			return false;
		}

		if(bExist && bSamePath) return true;

		SvcRemove();
	
		SC_HANDLE hMgr;
		
		hMgr = OpenScMgr();

		string szExePath = GetExePath() + " - service";

		SC_HANDLE hSvc = CreateService(
			hMgr,
			TEXT(m_szName.c_str()),
			TEXT(m_szName.c_str()),
			SERVICE_ALL_ACCESS,
			SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS,
			SERVICE_AUTO_START,     
			SERVICE_ERROR_NORMAL,
			szExePath.c_str(),             
			NULL,                  
			NULL,                      
			TEXT(""),
			NULL,                   
			NULL);

		CloseServiceHandle(hSvc);
		CloseServiceHandle(hMgr);
		
		return hSvc!=NULL;
	}

	// 报告状态
	void CService::ReportSvcStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint)
	{
		static DWORD dwCheckPoint = 1;

		m_SvcStatus.dwCurrentState = dwCurrentState;
		m_SvcStatus.dwWin32ExitCode = dwWin32ExitCode;
		m_SvcStatus.dwWaitHint = dwWaitHint;

		if (dwCurrentState == SERVICE_START_PENDING)
			m_SvcStatus.dwControlsAccepted = 0;
		else m_SvcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

		if ( (dwCurrentState == SERVICE_RUNNING) ||
			(dwCurrentState == SERVICE_STOPPED) )
			m_SvcStatus.dwCheckPoint = 0;
		else m_SvcStatus.dwCheckPoint = dwCheckPoint++;

		SetServiceStatus(m_hSvcStatus, &m_SvcStatus);
	}

	void WINAPI CService::SvcCtrlHandler(DWORD dwCtrl)
	{
		switch(dwCtrl) 
		{  
		case SERVICE_CONTROL_SHUTDOWN:
		case SERVICE_CONTROL_STOP: 
			m_pSvc->ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);
			m_pSvc->Stop();
			m_pSvc->ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
			break;

		default: 
			break;
		} 
	}

	// Main
	void WINAPI CService::SvcMain(DWORD dwArgc, LPTSTR* lpszArgv)
	{	
		m_pSvc->m_hSvcStatus = RegisterServiceCtrlHandler(m_pSvc->m_szName.c_str(), SvcCtrlHandler);

		m_pSvc->m_SvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS; 
		m_pSvc->m_SvcStatus.dwServiceSpecificExitCode = 0;    

		m_pSvc->ReportSvcStatus( SERVICE_RUNNING, NO_ERROR, 0);
		m_pSvc->Run();
		m_pSvc->ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
	}
}