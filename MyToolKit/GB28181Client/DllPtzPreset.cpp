#include "StdAfx.h"
#include "DllPtzPreset.h"

CDllPtzPreset::CDllPtzPreset(const sipd_preset_control * pPresetControl)
:AbstractCommand(false)
{
	m_PresetControl = * pPresetControl;
}

CDllPtzPreset::~CDllPtzPreset(void)
{
}


int CDllPtzPreset::Execute(UsageEnvironment* pEnviron)
{

	CDllDeviceControlSession * pSession = new CDllDeviceControlSession(*pEnviron, m_nSerialNumber);
	memset(m_PresetControl.SN, 0, MAX_SIPD_DEVICE_ID_LEN + 1);
	sprintf_s(m_PresetControl.SN, MAX_SIPD_DEVICE_ID_LEN, "%d", pSession->SerailNumber() );

	int tid = Sip_PTZ_Preset(&m_PresetControl);
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
