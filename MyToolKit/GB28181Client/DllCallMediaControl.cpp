#include "StdAfx.h"
#include "DllCallMediaControl.h"

CDllCallMediaControl::CDllCallMediaControl(int playport, SIPD_TO_CMS_CMD_TYPE cmd_type, 
										   sipd_playback_mans_rtsp_ind * playback_rtsp_control):
AbstractCommand(false)
{
	m_playport = playport;
	m_cmd_type = cmd_type;
	if(playback_rtsp_control)
	{
		memcpy(&m_playback_rtsp_control, playback_rtsp_control, sizeof(sipd_playback_mans_rtsp_ind));
	}
}

CDllCallMediaControl::~CDllCallMediaControl(void)
{
}


int CDllCallMediaControl::Execute(UsageEnvironment* pEnviron)
{
	int nResult = -1;

	CDllMediaSession * pSession = (CDllMediaSession *)pEnviron->SearchSession("CDllMediaSession", -1 * m_playport );
	
	if(pSession)
	{
		nResult = pSession->SerailNumber();
		Sip_Call_media_control(pSession->m_did, pSession->m_tid, m_cmd_type, &m_playback_rtsp_control);	

	}

	return nResult;
}
