#pragma once

class UsageEnvironment;

class CAbstractObject
{
public:
	CAbstractObject(UsageEnvironment &	environ, int nSerialNumber);
	virtual ~CAbstractObject(void);

public:
	unsigned long	m_KeepaliveTime;
	

protected: 
	UsageEnvironment &	m_Environ;
	int		m_SerialNumber;
	
public:
	int SerailNumber() { return m_SerialNumber; }
	virtual string ObjectType() = 0;

	virtual bool IsTimeout(unsigned long tvNow);
	virtual void ProcessTimeout() = 0;

	virtual void PostCallBack(CCommandCallback * cb, bool bMessage200OK, int nSum, int nListNum);

protected:
	bool	m_bMessage200OK;
	vector<CCommandCallback * >	m_vecCallback;
	int		m_nSumNum;
	int		m_nRecvNum;
};
