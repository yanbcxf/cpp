#pragma once

class CDllPtzMove :
	public AbstractCommand
{
public:
	CDllPtzMove(const sipd_ptz_control* pControl);
	~CDllPtzMove(void);

	virtual int Execute(UsageEnvironment* pThis);

	virtual string CommandType() { return "CDllPtzMove"; }

protected:
	sipd_ptz_control  m_Control;

};
