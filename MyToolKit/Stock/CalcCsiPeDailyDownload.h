#pragma once

class CCalcCsiPeDailyDownload : public CCalculateTask
{
public:
	CCalcCsiPeDailyDownload(HWND hwnd, int logType);
	~CCalcCsiPeDailyDownload(void);

	string ClassName(){ return "CCalcCsiPeDailyDownload"; }

	void NeedDownload();
	static void NeedDownload1(void* firstArg);
public:
	vector<int> m_vec_code;	//	"交易日期",
};
