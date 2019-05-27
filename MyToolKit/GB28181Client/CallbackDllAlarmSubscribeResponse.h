#pragma once
#include "CommandCallback.h"

class CCallbackDllAlarmSubscribeResponse :
	public CCommandCallback
{
public:
	CCallbackDllAlarmSubscribeResponse(int commandport, int result);
	virtual ~CCallbackDllAlarmSubscribeResponse(void);

	virtual void Execute();

protected:
	int m_commandport;
	int m_result;
};
