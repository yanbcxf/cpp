#pragma once
#include "CommandCallback.h"

class CCallbackDllDeviceinfoResponse :
	public CCommandCallback
{
public:
	CCallbackDllDeviceinfoResponse(int commandport, int type, int result, sipd_deviceinfo_rsp* pDeviceInfo);
	virtual ~CCallbackDllDeviceinfoResponse(void);

	virtual void Execute();

protected:
	int	m_commandport;
	int	m_type;
	int m_result;
	sipd_deviceinfo_rsp  m_DeviceInfo;
};
