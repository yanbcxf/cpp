#include "StdAfx.h"
#include "CalcForecastDownload.h"

CCalcForecastDownload::CCalcForecastDownload(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalcForecastDownload::~CCalcForecastDownload(void)
{
}


void CCalcForecastDownload::NeedDownloadPerformanceForecast1(void* firstArg)
{
	CCalcForecastDownload * sink = (CCalcForecastDownload *)firstArg;
	sink->NeedDownloadPerformanceForecast();
}


void CCalcForecastDownload::NeedDownloadPerformanceForecast()
{
	/*汇总需要下载业绩预告的股票*/
	stringstream ss;
	m_vec_code.clear();

	try{

		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << " select a.code"  
			<< " from (SELECT code, max(report_date) as max_date FROM sohuperformanceforecast  group by code) a"
			<< " where date_sub(now(), INTERVAL 90 day) > a.max_date ";

		string test = ss.str();
		//test = Gbk2Utf8(test);

		row r;
		session sql(g_MysqlPool);

		statement st = (sql.prepare << test,into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{
			int code = r.get<int>("code");
			m_vec_code.insert(m_vec_code.end(), code);
			cnt++;
		}

		ss.str("");
		ss << "需要下载 《业绩预告》 的股票数 ：";
		ss << cnt << " \r\n";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch(std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}
