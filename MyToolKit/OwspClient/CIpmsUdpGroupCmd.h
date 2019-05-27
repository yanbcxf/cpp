#pragma once
#include "abstractcommand.h"

class CIpmsUdpGroupCmd :public AbstractCommand
{
public:
	CIpmsUdpGroupCmd(UsageEnvironment* pUsageEnvir, string strServerIp, int nPort, string strDeviceSN);
	~CIpmsUdpGroupCmd(void);

	virtual int Execute(UsageEnvironment* pUsageEnvir);

	virtual void ParseResponse();

private:
	string	m_strServerIp;
	int		m_nPort;
	string	m_strDeviceSN;
};
