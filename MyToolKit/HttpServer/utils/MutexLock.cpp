#include "stdafx.h"
#include "MutexLock.h"

namespace ZvLibs
{
	CMutexLock::CMutexLock(HANDLE hMutex, DWORD dwMilliseconds)
	{
		m_hMutex = hMutex;
		m_bTimeout = (WaitForSingleObject(m_hMutex, dwMilliseconds)==WAIT_TIMEOUT);
	}

	CMutexLock::~CMutexLock()
	{
		ReleaseMutex(m_hMutex);
	}

	bool CMutexLock::IsTimeout()
	{
		return m_bTimeout;
	}
}