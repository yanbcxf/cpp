#pragma once

class CCalcTechnicalAnalysisRoadMap : public CCalculateTask
{
public:
	CCalcTechnicalAnalysisRoadMap(HWND hwnd, int logType);
	~CCalcTechnicalAnalysisRoadMap(void);

	string ClassName(){ return "CCalcTechnicalAnalysisRoadMap"; }

	void Execute();
	static void Execute1(void* firstArg);

public:
	
};
