#pragma once
#include "abstractcommand.h"

class CSipdCall200ok :
	public AbstractCommand
{
public:
	CSipdCall200ok(int cid, int did, int tid, sipd_media_session* pMediaInfo);
	~CSipdCall200ok(void);

	virtual int Execute(UsageEnvironment* pThis);

	virtual string CommandType() { return "CSipdCall200ok"; }

protected:
	sipd_media_session  m_MediaInfo;
	int		m_cid;
	int		m_did;
	int		m_tid;
};
