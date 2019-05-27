#pragma once
#include "abstractobject.h"

class CDllCataloginfoSession :
	public CAbstractObject
{
public:
	CDllCataloginfoSession(UsageEnvironment &	environ, int nSerialNumber , string device_id);
	~CDllCataloginfoSession(void);

	string ObjectType() {  return "CDllCataloginfoSession"; }

	virtual void ProcessTimeout();

public :
	int m_tid;	
	int	m_nCurrentSumNum;
	string	m_device_id;
};
