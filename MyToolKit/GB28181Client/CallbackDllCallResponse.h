#pragma once
#include "CommandCallback.h"

class CCallbackDllCallResponse :
	public CCommandCallback
{
public:
	CCallbackDllCallResponse(int commandport, int type,  int result);
	virtual ~CCallbackDllCallResponse(void);

	virtual void Execute();

protected:
	int m_commandport;
	int m_type;
	int m_result;
};
