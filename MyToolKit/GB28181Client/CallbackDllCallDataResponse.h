#pragma once
#include "CommandCallback.h"

class CCallbackDllCallDataResponse :
	public CCommandCallback
{
public:
	CCallbackDllCallDataResponse(int commandport, unsigned char * pBuffer, int nBufSize);
	virtual ~CCallbackDllCallDataResponse(void);

	virtual void Execute();
	virtual int	 FrameType();

public:
	int m_commandport;

	unsigned char * m_pBuffer;
	int	m_nBufSize;
};
