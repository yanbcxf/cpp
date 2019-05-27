#pragma once

class CDllStateinfoRequest :
	public AbstractCommand
{
public:
	CDllStateinfoRequest(const sipd_devicestate_req * pStateInfoReq);
	~CDllStateinfoRequest(void);

	virtual int Execute(UsageEnvironment* pThis);

	virtual string CommandType();

protected:
	sipd_devicestate_req m_StateInfoReq;

};
