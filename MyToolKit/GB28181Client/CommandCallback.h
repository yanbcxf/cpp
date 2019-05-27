#pragma once

class CCommandCallback
{
public:
	CCommandCallback(void);
	virtual ~CCommandCallback(void);

	virtual void Execute() = 0;
	virtual int	 FrameType(){ return -1; }
};