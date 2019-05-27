#pragma once
#include "abstractobject.h"

class CDllAlarmSubscribeSession :
	public CAbstractObject
{
public:
	CDllAlarmSubscribeSession(UsageEnvironment &	environ, int nSerialNumber);
	~CDllAlarmSubscribeSession(void);

	string ObjectType() {  return "CDllAlarmSubscribeSession"; }
	virtual void ProcessTimeout();

public :
	int m_sid;	
};
