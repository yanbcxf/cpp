#pragma once

class CCalcPeAndValueDaily : public CCalculateTask
{
public:
	CCalcPeAndValueDaily(HWND hwnd, int logType);
	~CCalcPeAndValueDaily(void);

	string ClassName(){ return "CCalcPeAndValueDaily"; }

	void Execute();
	static void Execute1(void* firstArg);


};
