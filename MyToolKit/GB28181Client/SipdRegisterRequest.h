#pragma once
#include "abstractcommand.h"

class CSipdRegisterRequest :
	public AbstractCommand
{
public:
	CSipdRegisterRequest(sipd_register_req * pRegisterReq);
	~CSipdRegisterRequest(void);

	virtual int Execute(UsageEnvironment* pThis);

	virtual string CommandType() { return "CSipdRegisterRequest"; }

protected:
	sipd_register_req	m_RegisterReq;
};
