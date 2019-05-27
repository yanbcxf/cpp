#pragma once
#include "CommandCallback.h"

class CCallbackDllPtzResponse :
	public CCommandCallback
{
public:
	CCallbackDllPtzResponse(int commandport, int result);
	virtual ~CCallbackDllPtzResponse(void);

	virtual void Execute();

protected:
	int m_commandport;
	int m_result;
};
