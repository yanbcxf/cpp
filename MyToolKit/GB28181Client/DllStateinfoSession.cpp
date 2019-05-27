#include "StdAfx.h"
#include "DllStateinfoSession.h"

CDllStateinfoSession::CDllStateinfoSession(UsageEnvironment &	environ, int nSerialNumber)
:CAbstractObject(environ, nSerialNumber)
{
	m_tid = -1;

	m_Environ.m_nStateinfoSession++;
}

CDllStateinfoSession::~CDllStateinfoSession(void)
{
	m_Environ.m_nStateinfoSession--;
}



void CDllStateinfoSession::ProcessTimeout()
{
	char tmp[256] = { 0};
	sprintf_s(tmp, 256, "%s[%08d], tid = %d, Timeout", ObjectType().c_str(),  SerailNumber(), m_tid);
	LOG4CPLUS_DEBUG(m_Environ.m_Logger, string(tmp));

	CCallbackDllStateinfoResponse *pcb = new CCallbackDllStateinfoResponse(SerailNumber(), 0 ,-1, NULL, NULL);
	m_Environ.PostCallBack(pcb);
}