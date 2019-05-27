#pragma once

class CCalcCsdcFundInSettlementStatDownload : public CCalculateTask
{
public:
	CCalcCsdcFundInSettlementStatDownload(HWND hwnd, int logType);
	~CCalcCsdcFundInSettlementStatDownload(void);

	string ClassName(){ return "CCalcCsdcFundInSettlementStatDownload"; }

	void NeedDownload();
	static void NeedDownload1(void* firstArg);
public:
	vector<int>		m_vec_code			;	//	"每周的开始日期",
};
