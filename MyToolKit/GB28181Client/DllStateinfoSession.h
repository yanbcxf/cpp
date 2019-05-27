#pragma once
#include "abstractobject.h"

class CDllStateinfoSession :
	public CAbstractObject
{
public:
	CDllStateinfoSession(UsageEnvironment &	environ, int nSerialNumber);
	~CDllStateinfoSession(void);

	string ObjectType() {  return "CDllStateinfoSession"; }
	virtual void ProcessTimeout();

public :
	int m_tid;	
};
