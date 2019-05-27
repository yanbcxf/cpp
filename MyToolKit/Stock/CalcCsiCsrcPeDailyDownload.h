#pragma once

class CCalcCsiCsrcPeDailyDownload : public CCalculateTask
{
public:
	CCalcCsiCsrcPeDailyDownload(HWND hwnd, int logType);
	~CCalcCsiCsrcPeDailyDownload(void);

	string ClassName(){ return "CCalcCsiCsrcPeDailyDownload"; }

	void NeedDownload();
	static void NeedDownload1(void* firstArg);
public:
	vector<int> m_vec_code;	//	"交易日期",
};
