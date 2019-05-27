#pragma once
#include "abstractobject.h"

class CDllDeviceControlSession :
	public CAbstractObject
{
public:
	CDllDeviceControlSession(UsageEnvironment &	environ, int nSerialNumber);
	~CDllDeviceControlSession(void);

	string ObjectType() {  return "CDllDeviceControlSession"; }

	virtual void ProcessTimeout();

public :
	int m_tid;			
};
