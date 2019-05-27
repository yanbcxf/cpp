#include "StdAfx.h"
#include "CalcLotteryNumberList.h"

CCalcLotteryNumberList::CCalcLotteryNumberList(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
	m_start_number = 0;
	m_end_number = 0;
	m_bContainSpecial = false;

	m_alpha = m_beta = m_upsilon = m_theta = m_eta = 0;
}

CCalcLotteryNumberList::~CCalcLotteryNumberList(void)
{
}

void CCalcLotteryNumberList::Execute1(void* firstArg)
{
	CCalcLotteryNumberList * sink = (CCalcLotteryNumberList *)firstArg;
	sink->Execute();
}



void CCalcLotteryNumberList::Execute()
{
	/*汇总需要下载财务报表的股票*/
	stringstream ss;
	m_vec_lottery_number.clear();

	try{
		session sql(g_MysqlPool);

		ss.str("");
		ss << "select code, name, periods, number_1, number_2, number_3, number_4,"
			"number_5, number_6, number_7, special_number, sum_number, odd, even, "
			"consecutive_number, equal_tail, var, stdev, devsq, kurt, skew "
			"from  lottery_mxn where code = \'"
			<< m_lottery_info.code <<"\'  ";

		if (m_start_number > 0)
			ss << " and periods >= " << Int2String(m_start_number);
		if (m_end_number > 0)
			ss << " and periods <= " << Int2String(m_end_number);
		ss << " order by periods ";
		
		string test = ss.str();
		//test = Gbk2Utf8(test);

		row r;
		statement st = (sql.prepare << test,into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{
			LotteryNumber ln;
			ln.code = r.get<string>("code", "");
			ln.name = r.get<string>("name", "");
			ln.periods = r.get<int>("periods", 0);
			ln.number[0] = r.get<int>("number_1", 0);
			ln.number[1] = r.get<int>("number_2", 0);
			ln.number[2] = r.get<int>("number_3", 0);
			ln.number[3] = r.get<int>("number_4", 0);
			ln.number[4] = r.get<int>("number_5", 0);
			ln.number[5] = r.get<int>("number_6", 0);
			ln.number[6] = r.get<int>("number_7", 0);
			ln.special_number = r.get<int>("special_number", 0);
			ln.sum = r.get<int>("sum_number", 0);
			ln.odd = r.get<int>("odd", 0);
			ln.even = r.get<int>("even", 0);
			//ln.consecutive_number = r.get<string>("consecutive_number", "");
			ln.equal_tail = r.get<string>("equal_tail", "");
			ln.var = r.get<double>("var", 0.0);
			ln.stdev = r.get<double>("stdev", 0.0);
			ln.devsq = r.get<double>("devsq", 0.0);
			ln.kurt = r.get<double>("kurt", 0.0);
			ln.skew = r.get<double>("skew", 0.0);
			
			bool bContainSpecial = false;
			if (m_lottery_info.specialnum > 0)
				bContainSpecial = m_bContainSpecial;

			//	计算特征值
			vector<double> vecFred;
			vecFred.push_back(m_alpha);
			vecFred.push_back(m_beta);
			vecFred.push_back(m_upsilon);
			vecFred.push_back(m_theta);
			vecFred.push_back(m_eta);
			ln = CalcCharacter(m_lottery_info.regularnum, m_lottery_info.totalnum, ln, m_region_start, m_region_end,
				vecFred, bContainSpecial);

			m_vec_lottery_number.insert(m_vec_lottery_number.end(), ln);
		}

		ss.str("");
		ss << "开奖号码数据读入完成， 共有" << m_vec_lottery_number.size() << "条记录\r\n";
		sendToOutput(ss.str().c_str(), NULL /*m_hWnd*/, m_nLogType);
	}
	catch(std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}
