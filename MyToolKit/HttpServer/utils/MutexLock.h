#pragma once

namespace ZvLibs
{
	class CMutexLock
	{
	public:
		CMutexLock(HANDLE hMutex, DWORD dwMilliseconds = 1000);
		virtual ~CMutexLock();
		
		bool IsTimeout();

	private:
		HANDLE m_hMutex;
		bool m_bTimeout;
	};
}