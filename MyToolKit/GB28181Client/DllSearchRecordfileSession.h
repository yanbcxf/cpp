#pragma once
#include "abstractobject.h"

class CDllSearchRecordfileSession :
	public CAbstractObject
{
public:
	CDllSearchRecordfileSession(UsageEnvironment &	environ, int nSerialNumber);
	~CDllSearchRecordfileSession(void);

	string ObjectType() {  return "CDllSearchRecordfileSession"; }
	virtual void ProcessTimeout();

public :
	int m_tid;		
	int	m_nCurrentSumNum;
};
