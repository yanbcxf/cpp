#pragma once

class CDllDeviceinfoRequest :
	public AbstractCommand
{
public:
	CDllDeviceinfoRequest(const sipd_deviceinfo_req * pDeviceinfoReq);
	~CDllDeviceinfoRequest(void);

	virtual int Execute(UsageEnvironment* pThis);

	virtual string CommandType();

protected:
	sipd_deviceinfo_req		m_DeviceinfoReq;

};
