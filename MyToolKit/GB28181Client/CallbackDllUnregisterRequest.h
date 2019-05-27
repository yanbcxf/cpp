#pragma once
#include "CommandCallback.h"

class CCallbackDllUnregisterRequest :
	public CCommandCallback
{
public:
	CCallbackDllUnregisterRequest(int commandport);
	virtual ~CCallbackDllUnregisterRequest(void);

	virtual void Execute();

protected:
	int	m_commandport;
};
