#include "StdAfx.h"
#include "CipmsregisterCmd.h"

#include "CIpmsClient.h"

CIpmsRegisterCmd::CIpmsRegisterCmd(UsageEnvironment* pUsageEnvir, string strServerIp, int nPort, string strDeviceSN, bool bControlBox)
	:AbstractCommand(pUsageEnvir)
{
	m_strServerIp = strServerIp;
	m_nPort = nPort;
	m_strDeviceSN = strDeviceSN;
	m_isControlBox = bControlBox;
}

CIpmsRegisterCmd::~CIpmsRegisterCmd(void)
{
}

int CIpmsRegisterCmd::Execute(UsageEnvironment* pUsageEnvir)
{
	stringstream ss;
	ss << m_strServerIp << "_" << m_nPort;
	string strKey = ss.str();

	CIpmsClient * pClient = NULL;
	

	if(pClient==NULL)
	{
		pClient = new CIpmsClient(*pUsageEnvir, m_strServerIp, m_nPort, m_strDeviceSN, m_isControlBox);
		CString str;
		str.Format("Register %s ¿ªÊ¼", m_strDeviceSN.c_str());
		pUsageEnvir->AppendInfoLog(str);
		return pClient->sendRegisterCommand(this);
	}
	
}



void CIpmsRegisterCmd::ParseResponse()
{

	if(m_nRecvResult == RESULT_CODE_OK)
	{
		char strTmp[512 + 1] = { 0 };
		memcpy(strTmp, m_pRecvBuffer, m_nRecvLength > 512 ? 512 : m_nRecvLength );
		string strData = string(strTmp); 

		if(strData.find("responed=1")!=string::npos)
		{
			m_nRecvResult = RESULT_CODE_SERVER_PRIMARY;
		}
		else if(strData.find("responed=0")!=string::npos)
		{
			m_nRecvResult = RESULT_CODE_SERVER_SLAVE;
		}
		else if(strData.find("responed=-1")!=string::npos)
		{
			m_nRecvResult = RESULT_LOGON_ALREADY;
		}
		else if(strData.find("responed=-2")!=string::npos)
		{
			m_nRecvResult = RESULT_LOGON_DOG_EXPIRED;
		}
		else if(strData.find("responed=-3")!=string::npos)
		{
			m_nRecvResult = RESULT_LOGON_DOG_CHECK_FAIL;
		}
		else if(strData.find("responed=-4")!=string::npos)
		{
			m_nRecvResult = RESULT_LOGON_DOG_USER_EXCEEDING;
		}
		else 
		{
			m_nRecvResult = RESULT_CODE_SERVER_UNKOWN;
		}
	}
	
}