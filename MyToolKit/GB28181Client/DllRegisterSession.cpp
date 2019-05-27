#include "StdAfx.h"
#include "DllRegisterSession.h"

CDllRegisterSession::CDllRegisterSession(UsageEnvironment &	environ, int nSerialNumber)
:CAbstractObject(environ, nSerialNumber)
{
	
}

CDllRegisterSession::~CDllRegisterSession(void)
{
	
}


bool  CDllRegisterSession::IsTimeout(unsigned long tvNow)
{
	long  delay = tvNow - m_KeepaliveTime; 

	if( delay > 120 * 1000)
		return true;

	return false;
}


void CDllRegisterSession::ProcessTimeout()
{
	char tmp[256] = { 0};
	sprintf_s(tmp, 256, "%s[%08d], Timeout", ObjectType().c_str(),  SerailNumber());
	LOG4CPLUS_DEBUG(m_Environ.m_Logger, string(tmp));

	CCallbackDllUnregisterRequest * pCb = new CCallbackDllUnregisterRequest(SerailNumber());
	m_Environ.PostCallBack(pCb);
}