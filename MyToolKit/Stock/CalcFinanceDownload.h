#pragma once

class CCalcFinanceDownload  : public CCalculateTask
{
public:
	CCalcFinanceDownload(HWND hwnd, int logType);
	~CCalcFinanceDownload(void);

	string ClassName(){ return "CCalcFinanceDownload"; }

	void NeedDownloadFinanceReport();
	static void NeedDownloadFinanceReport1(void* firstArg);

public:
	vector<int>		m_vec_code			;	//	"¹«Ë¾´úÂë",
	vector<string>	m_vec_cninfo_orgid;
};
