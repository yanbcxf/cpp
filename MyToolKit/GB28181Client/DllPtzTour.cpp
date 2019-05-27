#include "StdAfx.h"
#include "DllPtzTour.h"

CDllPtzTour::CDllPtzTour(const sipd_tour_control * pTourControl)
:AbstractCommand(false)
{
	m_TourControl = *pTourControl;
}

CDllPtzTour::~CDllPtzTour(void)
{
}



int CDllPtzTour::Execute(UsageEnvironment* pEnviron)
{

	CDllDeviceControlSession * pSession = new CDllDeviceControlSession(*pEnviron, m_nSerialNumber);
	memset(m_TourControl.SN, 0, MAX_SIPD_DEVICE_ID_LEN + 1);
	sprintf_s(m_TourControl.SN, MAX_SIPD_DEVICE_ID_LEN, "%d", pSession->SerailNumber() );

	int tid = Sip_PTZ_Tour(&m_TourControl);
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