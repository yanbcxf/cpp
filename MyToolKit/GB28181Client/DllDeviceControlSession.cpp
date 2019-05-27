#include "StdAfx.h"
#include "DllDeviceControlSession.h"

CDllDeviceControlSession::CDllDeviceControlSession(UsageEnvironment &	environ, int nSerialNumber)
:CAbstractObject(environ, nSerialNumber)
{
	m_tid = -1;

	m_Environ.m_nDeviceControlSession++;
}

CDllDeviceControlSession::~CDllDeviceControlSession(void)
{

	m_Environ.m_nDeviceControlSession--;
}



void CDllDeviceControlSession::ProcessTimeout()
{
	char tmp[256] = { 0};
	sprintf_s(tmp, 256, "%s[%08d], tid = %d, Timeout", ObjectType().c_str(),  SerailNumber(), m_tid);
	LOG4CPLUS_DEBUG(m_Environ.m_Logger, string(tmp));

	/*CCallbackDllCallResponse *pcb = new CCallbackDllCallResponse(SerailNumber(), 0, -1);
	m_Environ.PostCallBack(pcb);*/
}