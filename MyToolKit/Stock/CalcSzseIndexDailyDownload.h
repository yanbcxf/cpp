#pragma once

class CCalcSzseIndexDailyDownload : public CCalculateTask
{
public:
	CCalcSzseIndexDailyDownload(HWND hwnd, int logType);
	~CCalcSzseIndexDailyDownload(void);

	string ClassName(){ return "CCalcSzseIndexDailyDownload"; }

	void NeedDownload();
	static void NeedDownload1(void* firstArg);
public:
	vector<int> m_vec_code;	//	"交易日期",
};
