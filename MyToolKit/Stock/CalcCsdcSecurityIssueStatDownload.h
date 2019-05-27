#pragma once

class CCalcCsdcSecurityIssueStatDownload : public CCalculateTask
{
public:
	CCalcCsdcSecurityIssueStatDownload(HWND hwnd, int logType);
	~CCalcCsdcSecurityIssueStatDownload(void);

	string ClassName(){ return "CCalcCsdcSecurityIssueStatDownload"; }

	void NeedDownload();
	static void NeedDownload1(void* firstArg);
public:
	vector<int>		m_vec_code			;	//	"每周的开始日期",
};
