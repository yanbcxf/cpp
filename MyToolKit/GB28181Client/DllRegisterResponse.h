#pragma once
#include "abstractcommand.h"

class CDllRegisterResponse :
	public AbstractCommand
{
public:
	CDllRegisterResponse(int commandport, int isOk);
	~CDllRegisterResponse(void);

	virtual int Execute(UsageEnvironment* pThis);
	virtual string CommandType();

protected:
	int	m_commandport;
	int m_isOk;
};
