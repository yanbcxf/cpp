#pragma once

class CCalcNewmat11Test :public CCalculateTask
{
public:
	CCalcNewmat11Test(HWND hwnd, int logType);
	~CCalcNewmat11Test(void);

	string ClassName(){ return "CCalcNewmat11Test"; }

	void Execute();
	static void Execute1(void* firstArg);

public:
	Matrix	m_result;
};
