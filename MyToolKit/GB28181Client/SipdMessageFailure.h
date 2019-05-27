#pragma once
#include "abstractcommand.h"

class CSipdMessageFailure :
	public AbstractCommand
{
public:
	CSipdMessageFailure(int tid);
	~CSipdMessageFailure(void);

	virtual int Execute(UsageEnvironment* pThis);

	virtual string CommandType() { return "CSipdMessageFailure"; }

protected:
	int  m_tid;
};
