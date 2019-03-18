#pragma once
#include "abstractcommand.h"

class COwspLoginCommand :public AbstractCommand
{
public:
	COwspLoginCommand(string strIPAddr, int nPort);
	~COwspLoginCommand(void);

	virtual void Execute(UsageEnvironment* pUsageEnvir);

	string	m_strIpAddr;
	int		m_nPort;
	
	string	m_strClientName;		//	作为命令执行的结果进行返回
};
