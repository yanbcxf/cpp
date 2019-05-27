#include "StdAfx.h"
#include "CalcCapiStructDownload.h"

CCalcCapiStructDownload::CCalcCapiStructDownload(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalcCapiStructDownload::~CCalcCapiStructDownload(void)
{
}



void CCalcCapiStructDownload::NeedDownload1(void* firstArg)
{
	CCalcCapiStructDownload * sink = (CCalcCapiStructDownload *)firstArg;
	sink->NeedDownload();
}



void CCalcCapiStructDownload::NeedDownload()
{
	/*汇总需要下载财务报表的股票*/
	stringstream ss;
	m_vec_code.clear();

	try{

		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss  << "select t.code, date_format(t.max_date, '%Y-%m-%d') as max_date  from"
			<< " (select  a.code, NULL as max_date "
			<< " from stockinfo a where not exists ( select b.code from capitalstructure b where b.code = a.code ) "
			<< " and  a.a_time_to_delisting is null "
			<< " union "
			<< " (select a.code, a.max_date"  
			<< " from ( SELECT code, max(download_date) as max_date FROM  capitalstructure  group by code ) a"
			<< " where max_date is null or CURDATE() <> a.max_date ORDER BY a.max_date )) t ORDER BY t.max_date ";

		string test = ss.str();
		//test = Gbk2Utf8(test);

		row r;
		session sql(g_MysqlPool);

		statement st = (sql.prepare << test,into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{
			string maxDate = "";
			try{
				maxDate = r.get<string>("max_date");
			}
			catch(...){}

			bool bSave = false;
			if( maxDate.empty()) 
				bSave = true;
			else if(cnt<500) 
				bSave = true;
			else
			{
				COleDateTime odt ;
				odt.ParseDateTime(maxDate.c_str());

				COleDateTime t = COleDateTime::GetCurrentTime();
				if(t.m_dt - odt.m_dt < 7 )
				{
					// 如果在 7 天内下载过，则忽略本次下载
					break;
				}
				else
					bSave = true;
			}

			if(bSave)
			{
				int code = r.get<int>("code");
				m_vec_code.insert(m_vec_code.end(), code);
				cnt++;
			}
			
		}

		ss.str("");
		ss << "需要下载 《同花顺股本结构》 的股票数 ：";
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
