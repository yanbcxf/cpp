#pragma once
#include "CommandCallback.h"

class CCallbackDllStateinfoResponse :
	public CCommandCallback
{
public:
	CCallbackDllStateinfoResponse(int commandport, int type, int result, 
		sipd_devicestate_rsp_basic_info* pStateBasicInfo, sipd_alarm_status_item* pStateItem);
	virtual ~CCallbackDllStateinfoResponse(void);

	virtual void Execute();

protected:
	int	m_commandport;
	int	m_type;
	int m_result;
	sipd_devicestate_rsp_basic_info  m_StateBasicInfo;
	sipd_alarm_status_item	m_pStateItem[100];
};
