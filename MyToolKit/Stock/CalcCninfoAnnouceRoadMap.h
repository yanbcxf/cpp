#pragma once

class CCalcCninfoAnnouceRoadMap : public CCalculateTask
{
public:
	CCalcCninfoAnnouceRoadMap(HWND hwnd, int logType);
	~CCalcCninfoAnnouceRoadMap(void);

	string ClassName(){ return "CCalcCninfoAnnouceRoadMap"; }

	void Execute();
	static void Execute1(void* firstArg);

public:
	
};
