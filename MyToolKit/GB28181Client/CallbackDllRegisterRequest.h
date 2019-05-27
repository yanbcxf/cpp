#pragma once
#include "CommandCallback.h"

class CCallbackDllRegisterRequest :
	public CCommandCallback
{
public:
	CCallbackDllRegisterRequest(int commandport, sipd_register_req * pRegisterReq);
	virtual ~CCallbackDllRegisterRequest(void);

	virtual void Execute();

protected:
	int	m_commandport;
	sipd_register_req	m_RegisterReq;
};
