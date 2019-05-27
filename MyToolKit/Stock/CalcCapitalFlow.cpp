#include "StdAfx.h"

#include "TechAnalysisEvent.h"
#include "DirectionalMovement.h"

#include "CalcCapitalFlow.h"

CCalcCapitalFlow::CCalcCapitalFlow(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalcCapitalFlow::~CCalcCapitalFlow(void)
{
	for(int i = 0; i< m_trading.size(); i++)
	{
		if(m_trading[i])
			delete m_trading[i];
	}

	for(int i = 0; i< m_capital.size(); i++)
	{
		if(m_capital[i])
			delete m_capital[i];
	}

	for(int i = 0; i< m_bollinger.size(); i++)
	{
		if(m_bollinger[i])
			delete m_bollinger[i];
	}

	for(int i = 0; i< m_dmi.size(); i++)
	{
		if(m_dmi[i])
			delete m_dmi[i];
	}

	for(int i = 0; i< m_dmi.size(); i++)
	{
		if(m_tech_analysis_event[i])
			delete m_tech_analysis_event[i];
	}
}




void CCalcCapitalFlow::Execute1(void* firstArg)
{
	CCalcCapitalFlow * sink = (CCalcCapitalFlow *)firstArg;
	sink->Execute();
}



void CCalcCapitalFlow::Execute()
{
	/*汇总需要下载财务报表的股票*/

	if(m_vec_request_code.size()<=0)
		return;

	stringstream ss;
	
	try{
		
		//////////////////////////////////////////////////////////////////////////
		for(int i=0; i < m_vec_request_code.size(); i++)
		{
			m_trading.insert(m_trading.end(), new CNetEaseTradeDaily);
			m_capital.insert(m_capital.end(), new CCapitalFlows163);
			m_trading[i]->m_nCode = atoi(m_vec_request_code[i].c_str());
			m_capital[i]->m_nCode = atoi(m_vec_request_code[i].c_str());

			m_trading[i]->ExportFromDatabase(m_StartDate, m_EndDate);
			m_trading[i]->Reorganize();
			m_capital[i]->ExportFromDatabase(m_StartDate, m_EndDate);

			m_bollinger.insert(m_bollinger.end(), new CBollingerBands);
			m_bollinger[i]->ExportFromDatabase(m_vec_request_code[i], m_StartDate, m_EndDate);

			m_dmi.insert(m_dmi.end(), new CDirectionalMovement);
			m_dmi[i]->ExportFromDatabase(m_vec_request_code[i], m_StartDate, m_EndDate);

			m_tech_analysis_event.insert(m_tech_analysis_event.end(), new CTechAnalysisEvent);
			m_tech_analysis_event[i]->ExportFromDatabase(m_vec_request_code[i], m_StartDate, m_EndDate);
		}

		ss.str("");
		ss << "CCalcCapitalFlow 统计完成";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch(std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}
