#include "StdAfx.h"
#include "CalcFinanceDownload.h"

CCalcFinanceDownload::CCalcFinanceDownload(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalcFinanceDownload::~CCalcFinanceDownload(void)
{
}



void CCalcFinanceDownload::NeedDownloadFinanceReport1(void* firstArg)
{
	CCalcFinanceDownload * sink = (CCalcFinanceDownload *)firstArg;
	sink->NeedDownloadFinanceReport();
}



void CCalcFinanceDownload::NeedDownloadFinanceReport()
{
	/*汇总需要下载财务报表的股票*/
	stringstream ss;
	m_vec_code.clear();

	try{

		//////////////////////////////////////////////////////////////////////////
		//	以现金流量表为 标准，因为他是三张表中最后一个下载的

		ss.str("");

		ss << "select code, abbreviation, cninfo_orgid, "
			<< " DATE_FORMAT((select cashflows from stock_data_log b where b.code = a.code), '%Y-%m-%d') as cashflows "
			<< " from stockinfo a  where abbreviation not like '%(退)%'   ORDER BY cashflows ";

		string test = ss.str();
		//test = Gbk2Utf8(test);

		row r;
		session sql(g_MysqlPool);

		statement st = (sql.prepare << test,into(r)) ;
		st.execute();

		COleDateTime odt = COleDateTime::GetCurrentTime();
		string nowTime = odt.Format("%Y-%m-%d").GetBuffer();
		int cnt = 0;
		while (st.fetch())
		{
			string downloadDate = r.get<string>("cashflows", "");
			if(nowTime==downloadDate)
				continue;

			int code = r.get<int>("code");
			m_vec_code.insert(m_vec_code.end(), code);
			m_vec_cninfo_orgid.insert(m_vec_cninfo_orgid.end(), r.get<string>("cninfo_orgid", ""));
			cnt++;
		}

		//	对vector 进行随机重排列，避免每次都从第一个股票代码开始下载
		/*srand(unsigned(time(0))); 
		random_shuffle(m_vec_code.begin(), m_vec_code.end());*/

		ss.str("");
		ss << "需要下载 Cninfo Finance 的股票数 ：";
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
