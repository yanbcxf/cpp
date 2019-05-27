#pragma once

class CCalcIncomestatementForecast : public CCalculateTask
{
public:
	CCalcIncomestatementForecast(HWND hwnd, int logType);
	~CCalcIncomestatementForecast(void);

	string ClassName(){ return "CCalcIncomestatementForecast"; }

	void Execute();
	static void Execute1(void* firstArg);

public:

	CIncomeStatement  m_IncomeStatement;
	
};
