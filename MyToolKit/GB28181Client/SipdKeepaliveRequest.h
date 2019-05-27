#pragma once
#include "abstractcommand.h"

class CSipdKeepaliveRequest :
	public AbstractCommand
{
public:
	CSipdKeepaliveRequest(int tid, sipd_keepalive_notify * pKeepaliveReq);
	~CSipdKeepaliveRequest(void);

	virtual int Execute(UsageEnvironment* pThis);

	virtual string CommandType() { return "CSipdKeepaliveRequest"; }

protected:
	sipd_keepalive_notify	m_KeepaliveReq;
	int		m_tid;
};
