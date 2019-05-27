#pragma once
#include "abstractcommand.h"

class CSipdDeviceinfoResponse :
	public AbstractCommand
{
public:
	CSipdDeviceinfoResponse(sipd_deviceinfo_rsp* pDeviceInfo, int tid);
	~CSipdDeviceinfoResponse(void);

	virtual int Execute(UsageEnvironment* pThis);

	virtual string CommandType() { return "CSipdDeviceinfoResponse"; }

protected:
	sipd_deviceinfo_rsp		m_DeviceInfo;
	int						m_tid;
};
