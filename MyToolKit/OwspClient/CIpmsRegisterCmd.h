#pragma once
#include "abstractcommand.h"

class CIpmsRegisterCmd :public AbstractCommand
{
public:
	CIpmsRegisterCmd(UsageEnvironment* pUsageEnvir, string strServerIp, int nPort, string strDeviceSN, bool bControlBox);
	~CIpmsRegisterCmd(void);

	virtual int Execute(UsageEnvironment* pUsageEnvir);

	virtual void ParseResponse();

private:
	string	m_strServerIp;
	int		m_nPort;
	string	m_strDeviceSN;
	bool	m_isControlBox;
};
