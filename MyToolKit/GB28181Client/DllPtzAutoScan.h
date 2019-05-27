#pragma once

class CDllPtzAutoScan :
	public AbstractCommand
{
public:
	CDllPtzAutoScan(const sipd_autoscan_control * pAutoScanControl);
	~CDllPtzAutoScan(void);

	virtual int Execute(UsageEnvironment* pThis);

	virtual string CommandType() { return "CDllPtzAutoScan"; }

protected:
	sipd_autoscan_control  m_AutoScanControl;

};
