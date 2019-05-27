#pragma once
#include "abstractcommand.h"

class CSipdSubscribe200ok :
	public AbstractCommand
{
public:
	CSipdSubscribe200ok(sipd_subscribe_200_ok_info *pInfo, int sid);
	~CSipdSubscribe200ok(void);

	virtual int Execute(UsageEnvironment* pThis);

	virtual string CommandType() { return "CSipdSubscribe200ok"; }

protected:
	int  m_sid;
	sipd_subscribe_200_ok_info	m_subscribe_200_ok;
};
