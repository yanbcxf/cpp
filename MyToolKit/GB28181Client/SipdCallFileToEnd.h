#pragma once
#include "abstractcommand.h"

class CSipdCallFileToEnd :
	public AbstractCommand
{
public:
	CSipdCallFileToEnd(int cid);
	~CSipdCallFileToEnd(void);

	virtual int Execute(UsageEnvironment* pThis);

	virtual string CommandType() { return "CSipdCallFileToEnd"; }

protected:
	int		m_cid;
	
};
