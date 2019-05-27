#pragma once
#include "abstractcommand.h"

class CSipdCallNoAnswer :
	public AbstractCommand
{
public:
	CSipdCallNoAnswer(int cid);
	~CSipdCallNoAnswer(void);

	virtual int Execute(UsageEnvironment* pThis);

	virtual string CommandType() { return "CSipdCallNoAnswer"; }


protected:
	int		m_cid;
};
