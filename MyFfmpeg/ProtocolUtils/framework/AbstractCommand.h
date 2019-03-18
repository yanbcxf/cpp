#pragma once

class UsageEnvironment;

class AbstractCommand
{
public:
	AbstractCommand(void);

	~AbstractCommand(void);

	void CommandExecute(UsageEnvironment* pThis); 

	void CommandWaitingForEnd();

	virtual void Execute(UsageEnvironment* pThis) = 0;

protected:
	HANDLE hSempaphore;
};
