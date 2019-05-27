#pragma once
#include "abstractcommand.h"

class CSipdMessage200ok :
	public AbstractCommand
{
public:
	CSipdMessage200ok(int tid);
	~CSipdMessage200ok(void);

	virtual int Execute(UsageEnvironment* pThis);

	virtual string CommandType() { return "CSipdMessage200ok"; }

protected:
	int  m_tid;
};
