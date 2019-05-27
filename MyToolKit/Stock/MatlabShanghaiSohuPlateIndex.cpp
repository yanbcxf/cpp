#include "StdAfx.h"
#include "resource.h"
#include "MatlabShanghaiSohuPlateIndex.h"
//#include "StShanghaiSohuPlateIndex.h"
//#include "StShanghaiStockIndex.h"
//#include "StCloseWindow.h"

#include "StockMatlab.h"

CMatlabShanghaiSohuPlateIndex::CMatlabShanghaiSohuPlateIndex(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CMatlabShanghaiSohuPlateIndex::~CMatlabShanghaiSohuPlateIndex(void)
{
}



void CMatlabShanghaiSohuPlateIndex::Execute1(void* firstArg)
{
	CMatlabShanghaiSohuPlateIndex * sink = (CMatlabShanghaiSohuPlateIndex *)firstArg;
	sink->Execute();
}



void CMatlabShanghaiSohuPlateIndex::Execute()
{
	/* */
	stringstream ss;
	
	
	try{
		
		char wndName[32] = { 0 };
		sprintf_s(wndName, 32, "%d", m_windowInMatlab);

		if(m_StartDate < 0)
		{
			//	缺省查询 一年的数据
			m_StartDate = COleDateTime::GetCurrentTime();
			m_StartDate.m_dt -= 365;	
			//m_StartDate.ParseDateTime("2012-8-30");
		}
		
		if(m_EndDate < 0)
			m_EndDate = COleDateTime::GetCurrentTime();

		// yangbin matlab
		mwArray mwPlateCodes(1, m_vec_request_code.size(), mxINT32_CLASS);
		int * xxx = new int[m_vec_request_code.size()];
		int nShanghai = 0;
		for(int k=0; k< m_vec_request_code.size(); k++)
		{
			if( 1 /*m_vec_request_code[k]>="600000" || m_vec_request_is_stock[0]==0*/)
			{
				xxx[nShanghai] = atoi(m_vec_request_code[k].c_str());
				nShanghai++;
			}
			
		}
		
		mwPlateCodes.SetData(xxx, nShanghai);
		delete[] xxx;

		mwArray mwOut(1, 1, mxINT32_CLASS);

		if(m_vec_request_is_stock[0]==1)
		{
			StShanghaiStockIndex(1, mwOut, wndName, 
				m_StartDate.Format("%Y-%m-%d").GetBuffer(), m_EndDate.Format("%Y-%m-%d").GetBuffer(),
				m_strGraphOrTable.c_str(), mwPlateCodes);
		}
		else if(m_vec_request_is_stock[0]==ID_PLATE_TYPE_SOHU)
		{
			StShanghaiSohuPlateIndex(1, mwOut, wndName, 
				m_StartDate.Format("%Y-%m-%d").GetBuffer(), m_EndDate.Format("%Y-%m-%d").GetBuffer(),
				m_strGraphOrTable.c_str(), mwPlateCodes);
		}
		else if(m_vec_request_is_stock[0]==ID_PLATE_TYPE_SHENZHEN_INDEXINFO)
		{
			StShenzhenIndexPlateIndex(1, mwOut, wndName, 
				m_StartDate.Format("%Y-%m-%d").GetBuffer(), m_EndDate.Format("%Y-%m-%d").GetBuffer(),
				m_strGraphOrTable.c_str(), mwPlateCodes);
		}
		
		m_windowInMatlab  = mwOut.Get(1, 1);


		ss.str("");
		ss << "CMatlabShanghaiSohuPlateIndex 统计完成, Matlab 窗口标识 = " << m_windowInMatlab;
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch(std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}
