#pragma once

class CCalcCsdcTransferBonusStatDownload : public CCalculateTask
{
public:
	CCalcCsdcTransferBonusStatDownload(HWND hwnd, int logType);
	~CCalcCsdcTransferBonusStatDownload(void);

	string ClassName(){ return "CCalcCsdcTransferBonusStatDownload"; }

	void NeedDownload();
	static void NeedDownload1(void* firstArg);
public:
	vector<int>		m_vec_code			;	//	"每周的开始日期",
};
