#include "StdAfx.h"
#include "DllCataloginfoSession.h"

CDllCataloginfoSession::CDllCataloginfoSession(UsageEnvironment &	environ, int nSerialNumber, string device_id)
:CAbstractObject(environ, nSerialNumber)
{
	m_tid = -1;
	m_nCurrentSumNum = 0;
	m_device_id = device_id;

	m_Environ.m_nCataloginfoSession++;
}

CDllCataloginfoSession::~CDllCataloginfoSession(void)
{
	m_Environ.m_nCataloginfoSession--;
}


void CDllCataloginfoSession::ProcessTimeout()
{
	char tmp[256] = { 0};
	sprintf_s(tmp, 256, "%s[%08d], device_id = %s, tid = %d, Timeout", ObjectType().c_str(), 
		SerailNumber(), m_device_id.c_str(), m_tid);
	LOG4CPLUS_DEBUG(m_Environ.m_Logger, string(tmp));

	CCallbackDllCataloginfoResponse * pcb = new CCallbackDllCataloginfoResponse(SerailNumber(), 0 , -1, NULL, NULL);
	m_Environ.PostCallBack(pcb);
	
}