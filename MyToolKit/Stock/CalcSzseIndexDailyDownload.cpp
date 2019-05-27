#include "StdAfx.h"
#include "CalcSzseIndexDailyDownload.h"

CCalcSzseIndexDailyDownload::CCalcSzseIndexDailyDownload(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalcSzseIndexDailyDownload::~CCalcSzseIndexDailyDownload(void)
{
}


void CCalcSzseIndexDailyDownload::NeedDownload1(void* firstArg)
{
	CCalcSzseIndexDailyDownload * sink = (CCalcSzseIndexDailyDownload *)firstArg;
	sink->NeedDownload();
}



void CCalcSzseIndexDailyDownload::NeedDownload()
{
	/*汇总需要下载深圳交易所 股票指数 行情等的日期*/
	stringstream ss;
	map<string, int> map_trade_date;
	map_trade_date.clear();

	ss.str("");
	ss << "任务【" << m_nTaskID << "】 ";
	ss << "NeedDownload 开始计算";
	sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

	try{

		ss.str("");
		ss << "select DATE_FORMAT(TradeDate, '%Y-%m-%d') as TradeDate from sohuindexdaily where code = 1 and TradeDate > '2014-1-1'";

		session sql(g_MysqlPool);
		row r;
		statement st = (sql.prepare << ss.str(),into(r)) ;
		st.execute();
		while (st.fetch())
		{
			string  tradeDate = r.get<string>("TradeDate");
			string	tradeDate1 = ReplaceString(tradeDate, "-", "");

			map_trade_date[tradeDate] = atoi(tradeDate1.c_str());
		}

		//	以下 xls 文件解析错误，导致程序崩溃
		map_trade_date.erase("2015-06-11");

		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << " select DISTINCT DATE_FORMAT(TradeDate, '%Y-%m-%d') as TradeDate , count(*) as cnt  ";
		ss << " from szse_index_daily group by TradeDate order by TradeDate ";
		
		string test = ss.str();
		//test = Gbk2Utf8(test);

		try{
			row r;

			statement st = (sql.prepare << test,into(r)) ;
			st.execute();

			int lastCnt = 0;
			while (st.fetch())
			{
				string  tradeDate = r.get<string>("TradeDate");
				int cnt = r.get<long long>("cnt");
				
				if(cnt + 5 >= lastCnt)   
				{
					//	今天的记录应该 >= 昨天的记录, 否则该日保存未完整, 允许今天比昨天少但不超过5个
					map_trade_date.erase(tradeDate);
				}
				lastCnt = cnt;
			}
		}
		catch(...){}

		m_vec_code.clear();
		map<string, int>::iterator it = map_trade_date.begin();
		while(it!=map_trade_date.end())
		{
			m_vec_code.push_back(it->second);
			it++;
		}

		ss.str("");
		ss << "任务【" << m_nTaskID << "】 ";
		ss << "需要下载 《深圳交易所 股票指数 行情》 的日期数 ：";
		ss << m_vec_code.size() << " \r\n";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch(std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}
