#pragma once

#include "BollingerBands.h"

class CCalcCapitalFlow : public CCalculateTask
{
public:
	CCalcCapitalFlow(HWND hwnd, int logType);
	~CCalcCapitalFlow(void);

	string ClassName(){ return "CCalcCapitalFlow"; }

	void Execute();
	static void Execute1(void* firstArg);

public:
	vector<CCapitalFlows163 *>		m_capital;
	vector<CNetEaseTradeDaily *>	m_trading ;

	vector<CBollingerBands *>		m_bollinger ;
	vector<CDirectionalMovement *>	m_dmi;
	vector<CTechAnalysisEvent *>	m_tech_analysis_event;
};
