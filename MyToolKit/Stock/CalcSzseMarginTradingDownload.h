#pragma once
 
class CCalcSzseMarginTradingDownload  : public CCalculateTask
{
public:
	CCalcSzseMarginTradingDownload(HWND hwnd, int logType);
	~CCalcSzseMarginTradingDownload(void);

	string ClassName(){ return "CCalcSzseMarginTradingDownload"; }

	void NeedDownload();
	static void NeedDownload1(void* firstArg);
public:
	vector<int> m_vec_code;	//	"交易日期",
};
