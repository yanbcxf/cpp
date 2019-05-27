#include "StdAfx.h"
#include "MatlabShanghaiIndex.h"

//#include "StShanghaiIndex.h"
//#include "StCloseWindow.h"
#include "StockMatlab.h"

CMatlabShanghaiIndex::CMatlabShanghaiIndex(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CMatlabShanghaiIndex::~CMatlabShanghaiIndex(void)
{
}



void CMatlabShanghaiIndex::Execute1(void* firstArg)
{
	CMatlabShanghaiIndex * sink = (CMatlabShanghaiIndex *)firstArg;
	sink->Execute();
}



void CMatlabShanghaiIndex::Execute()
{
	/* */
	stringstream ss;
	
	
	try{
		
		char wndName[32] = { 0 };
		sprintf_s(wndName, 32, "%d", m_windowInMatlab);

		
		if(m_StartDate < 0)
			m_StartDate.ParseDateTime("2012-8-30");
		
		if(m_EndDate < 0)
			m_EndDate = COleDateTime::GetCurrentTime();

		// yangbin 
		mwArray mwOut(1, 1, mxINT32_CLASS);
		
		StHuShenIndex(1, mwOut, wndName, 
			m_StartDate.Format("%Y-%m-%d").GetBuffer(), m_EndDate.Format("%Y-%m-%d").GetBuffer(),
			m_strIndexCode.c_str());

		m_windowInMatlab  = mwOut.Get(1, 1);


		ss.str("");
		ss << "CMatlabShanghaiIndex 统计完成, Matlab 窗口标识 = " << m_windowInMatlab;
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch(std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}
