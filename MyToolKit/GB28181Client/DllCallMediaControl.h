#pragma once

class CDllCallMediaControl :
	public AbstractCommand
{
public:
	CDllCallMediaControl(int playport, SIPD_TO_CMS_CMD_TYPE cmd_type, 
		sipd_playback_mans_rtsp_ind * playback_rtsp_control);
	~CDllCallMediaControl(void);

	virtual int Execute(UsageEnvironment* pThis);

	virtual string CommandType() { return "CDllCallMediaControl"; }

protected:
	int		m_playport;
	SIPD_TO_CMS_CMD_TYPE	m_cmd_type;
	sipd_playback_mans_rtsp_ind		m_playback_rtsp_control;
};
