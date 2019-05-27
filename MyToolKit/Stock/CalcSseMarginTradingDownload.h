#pragma once

class CCalcSseMarginTradingDownload  : public CCalculateTask
{
public:
	CCalcSseMarginTradingDownload(HWND hwnd, int logType);
	~CCalcSseMarginTradingDownload(void);

	string ClassName(){ return "CCalcSseMarginTradingDownload"; }

	void NeedDownload();
	static void NeedDownload1(void* firstArg);
public:
	vector<int> m_vec_code;	//	"交易日期",
};
