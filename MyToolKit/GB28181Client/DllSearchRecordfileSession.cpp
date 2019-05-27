#include "StdAfx.h"
#include "DllSearchRecordfileSession.h"

CDllSearchRecordfileSession::CDllSearchRecordfileSession(UsageEnvironment &	environ, int nSerialNumber)
:CAbstractObject(environ,nSerialNumber)
{
	m_tid = -1;
	m_nCurrentSumNum = 0;

	m_Environ.m_nSearchRecordfileSession++;
}

CDllSearchRecordfileSession::~CDllSearchRecordfileSession(void)
{
	m_Environ.m_nSearchRecordfileSession--;

}




void CDllSearchRecordfileSession::ProcessTimeout()
{
	char tmp[256] = { 0};
	sprintf_s(tmp, 256, "%s[%08d], tid = %d, Timeout", ObjectType().c_str(),  SerailNumber(), m_tid);
	LOG4CPLUS_DEBUG(m_Environ.m_Logger, string(tmp));

	CCallbackDllSearchRecordfileResponse *pcb = new CCallbackDllSearchRecordfileResponse(SerailNumber(), 0 , -1, NULL, NULL);
	m_Environ.PostCallBack(pcb);
}