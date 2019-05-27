#include "StdAfx.h"
#include "DllDeviceinfoSession.h"

CDllDeviceinfoSession::CDllDeviceinfoSession(UsageEnvironment &	environ, int nSerialNumber)
:CAbstractObject(environ ,nSerialNumber)
{
	m_tid = -1;

	m_Environ.m_nDeviceinfoSession++;
}

CDllDeviceinfoSession::~CDllDeviceinfoSession(void)
{
	m_Environ.m_nDeviceinfoSession--;
}




void CDllDeviceinfoSession::ProcessTimeout()
{
	char tmp[256] = { 0};
	sprintf_s(tmp, 256, "%s[%08d], tid = %d, Timeout", ObjectType().c_str(),  SerailNumber(), m_tid);
	LOG4CPLUS_DEBUG(m_Environ.m_Logger, string(tmp));

	CCallbackDllDeviceinfoResponse * pcb = new CCallbackDllDeviceinfoResponse(SerailNumber(), 0 , -1, NULL);
	m_Environ.PostCallBack(pcb);
}