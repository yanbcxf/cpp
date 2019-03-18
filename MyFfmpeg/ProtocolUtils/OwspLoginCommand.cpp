#include "StdAfx.h"
#include "OwspLoginCommand.h"

#include "OwspClient.h"

COwspLoginCommand::COwspLoginCommand(string strIPAddr, int nPort)
{
	m_strIpAddr = strIPAddr;
	m_nPort = nPort;
}

COwspLoginCommand::~COwspLoginCommand(void)
{

}



void COwspLoginCommand::Execute(UsageEnvironment* pUsageEnvir)
{
	COwspClient * pDvrSdkClient = new COwspClient(*pUsageEnvir, m_strIpAddr.c_str(), m_nPort, "111111");
	if(pDvrSdkClient)
	{
		pDvrSdkClient->sendLoginCommand();
		m_strClientName = string(pDvrSdkClient->name());
	}

}
