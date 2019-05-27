#include "StdAfx.h"
#include "DllAlarmSubscribeSession.h"

CDllAlarmSubscribeSession::CDllAlarmSubscribeSession(UsageEnvironment &	environ, int nSerialNumber)
:CAbstractObject(environ, nSerialNumber)
{
	m_sid = -1;

	//m_Environ.m_nStateinfoSession++;
}

CDllAlarmSubscribeSession::~CDllAlarmSubscribeSession(void)
{
	//m_Environ.m_nStateinfoSession--;
}



void CDllAlarmSubscribeSession::ProcessTimeout()
{
	char tmp[256] = { 0};
	sprintf_s(tmp, 256, "%s[%08d], sid = %d, Timeout", ObjectType().c_str(),  SerailNumber(), m_sid);
	LOG4CPLUS_DEBUG(m_Environ.m_Logger, string(tmp));

	CCallbackDllAlarmSubscribeResponse *pcb = new CCallbackDllAlarmSubscribeResponse(SerailNumber(), -1);
	m_Environ.PostCallBack(pcb);
}