#include "StdAfx.h"
#include "DllPtzMove.h"

CDllPtzMove::CDllPtzMove(const sipd_ptz_control* pControl)
:AbstractCommand(false)
{
	m_Control = *pControl;
}

CDllPtzMove::~CDllPtzMove(void)
{
}


int CDllPtzMove::Execute(UsageEnvironment* pEnviron)
{
	CDllDeviceControlSession * pSession = new CDllDeviceControlSession(*pEnviron, m_nSerialNumber);
	memset(m_Control.SN, 0, MAX_SIPD_DEVICE_ID_LEN + 1);
	sprintf_s(m_Control.SN, MAX_SIPD_DEVICE_ID_LEN, "%d", pSession->SerailNumber() );

	int tid = Sip_PTZ_Move(&m_Control);
	if(tid>=0) 
	{
		pSession->m_tid = tid;
		return pSession->SerailNumber();
	}
	else
	{
		CCallbackDllPtzResponse * pcb = new  CCallbackDllPtzResponse(pSession->SerailNumber(), -1);
		pEnviron->PostCallBack(pcb);

		delete pSession;
		return tid;
	}
}
