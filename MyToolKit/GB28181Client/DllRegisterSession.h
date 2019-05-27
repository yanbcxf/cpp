#pragma once
#include "abstractobject.h"

class CDllRegisterSession :
	public CAbstractObject
{
public:
	CDllRegisterSession(UsageEnvironment &	environ, int nSerialNumber);
	virtual ~CDllRegisterSession(void);

	string ObjectType() {  return "CDllRegisterSession"; }

	virtual void ProcessTimeout();

	bool IsTimeout(unsigned long tvNow);
public :
	sipd_register_req  m_RegisterReq;	

	struct timeval  m_HeartbeatTime;
	
};
