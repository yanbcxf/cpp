#include "StdAfx.h"
#include "AbstractObject.h"



CAbstractObject::CAbstractObject(UsageEnvironment &	environ, int nSerialNumber)
:m_Environ(environ), m_SerialNumber(nSerialNumber)
{
	JMutexAutoLock lock(m_Environ.m_mtxObjects);
	m_Environ.m_mapObjects[m_SerialNumber] = this;

	m_KeepaliveTime = GetTickCount();
	m_bMessage200OK = false;
	m_nSumNum = -1;
	m_nRecvNum = 0;
}

CAbstractObject::~CAbstractObject(void)
{

	for(int i=0; i< m_vecCallback.size(); i++)
	{
		delete m_vecCallback[i];
	}

	JMutexAutoLock lock(m_Environ.m_mtxObjects);
	m_Environ.m_mapObjects.erase(m_SerialNumber);
}


bool  CAbstractObject::IsTimeout(unsigned long tvNow)
{
	long  delay = tvNow - m_KeepaliveTime; 

	if( delay > 10 * 1000)
		return true;

	return false;
}

void CAbstractObject::PostCallBack(CCommandCallback * cb, bool bMessage200OK, int nSum, int nListNum)
{

	if(bMessage200OK)
	{
		//	Message 200 OK
		m_vecCallback.insert(m_vecCallback.begin(), cb);
		m_bMessage200OK = true;
	}
	else
	{
		m_nSumNum = nSum;
		m_vecCallback.insert(m_vecCallback.end(), cb);

		m_nRecvNum += nListNum;

		char tmp[256] = {0 };
		sprintf_s(tmp, 256, "%s[%08d], SumNum = %d ,m_nRecvNum = %d ", ObjectType().c_str(),
			m_SerialNumber, m_nSumNum, m_nRecvNum );

		LOG4CPLUS_DEBUG(m_Environ.m_Logger, string(tmp));
	}

	if(m_bMessage200OK)
	{
		//	已经收到 200 OK，可以发送了
		for(int i=0; i< m_vecCallback.size(); i++)
		{
			m_Environ.PostCallBack(m_vecCallback[i]);
		}
		m_vecCallback.clear();
	}

	if(m_nSumNum == m_nRecvNum && m_bMessage200OK)
		delete this;
}

