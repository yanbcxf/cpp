#pragma once

class CCalcSipfSettlementFundDownload  : public CCalculateTask
{
public:
	CCalcSipfSettlementFundDownload(HWND hwnd, int logType);
	~CCalcSipfSettlementFundDownload(void);

	string ClassName(){ return "CCalcSipfSettlementFundDownload"; }

	void NeedDownload();
	static void NeedDownload1(void* firstArg);
public:
	vector<int> m_vec_code;	//	"交易日期",
};
