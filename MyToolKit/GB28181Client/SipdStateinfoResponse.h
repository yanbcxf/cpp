#pragma once
#include "abstractcommand.h"

class CSipdStateinfoResponse :
	public AbstractCommand
{
public:
	CSipdStateinfoResponse(sipd_devicestate_rsp_basic_info* pStateBasicInfo, sipd_alarm_status_item* pStateItem, int tid);
	~CSipdStateinfoResponse(void);

	virtual int Execute(UsageEnvironment* pThis);

	virtual string CommandType() { return "CSipdStateinfoResponse"; }

protected:
	sipd_devicestate_rsp_basic_info   m_StateBasicInfo;
	sipd_alarm_status_item *	m_pStateItem;
	int							m_tid;
};
