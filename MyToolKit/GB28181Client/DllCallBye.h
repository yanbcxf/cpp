#pragma once

class CDllCallBye :
	public AbstractCommand
{
public:
	CDllCallBye(int playport);
	~CDllCallBye(void);

	virtual int Execute(UsageEnvironment* pThis);

	virtual string CommandType() { return "CDllCallBye"; }

protected:
	int		m_playport;

};
