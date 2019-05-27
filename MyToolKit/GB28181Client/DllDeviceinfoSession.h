#pragma once
#include "abstractobject.h"

class CDllDeviceinfoSession :
	public CAbstractObject
{
public:
	CDllDeviceinfoSession(UsageEnvironment &	environ , int nSerialNumber);
	~CDllDeviceinfoSession(void);

	string ObjectType() {  return "CDllDeviceinfoSession"; }

	virtual void ProcessTimeout();

public :
	int m_tid;			
};
