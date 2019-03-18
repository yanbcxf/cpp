#include "StdAfx.h"
#include "AbstractCommand.h"

AbstractCommand::AbstractCommand(void)
{
	hSempaphore = CreateSemaphore(NULL, 0, 1, NULL );
}

AbstractCommand::~AbstractCommand(void)
{
	CloseHandle(hSempaphore);
}


void AbstractCommand::CommandExecute(UsageEnvironment* pThis)
{
	Execute(pThis);
	//	命令执行完成后，发送通知
	ReleaseSemaphore(hSempaphore, 1, NULL);
}

void AbstractCommand::CommandWaitingForEnd()
{
	WaitForSingleObject(hSempaphore, INFINITE);
}