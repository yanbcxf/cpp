#pragma once

class CCalcSohuMainIncomeCompoDownload  : public CCalculateTask 
{
public:
	CCalcSohuMainIncomeCompoDownload(HWND hwnd, int logType);
	~CCalcSohuMainIncomeCompoDownload(void);

	string ClassName(){ return "CCalcSohuMainIncomeCompoDownload"; }

	void Execute();
	static void Execute1(void* firstArg);

public:
	vector<int>		m_vec_code			;	//	"¹«Ë¾´úÂë",
};
