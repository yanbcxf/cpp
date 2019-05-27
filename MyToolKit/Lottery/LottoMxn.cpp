#include "StdAfx.h"
#include "Lottery.h"
#include "LottoMxn.h"

#include "UrlUtils.h"


#include "HtmlParser.h"

//////////////////////////////////////////////////////////////////////////

class CLottoMxnHtmlParser: public CHtmlParser
{
public:
	CLottoMxnHtmlParser(tree<HTML::Node> &dom, CLottoMxn & userContext):CHtmlParser(dom),m_cfiBonus(userContext)
	{

	}

	~CLottoMxnHtmlParser()
	{

	}

	void HtmlNodeCallBack(string xpath, string val, map<string, string> attrsOfParent)
	{
		int tbody, tr, td;
		if (sscanf_s(xpath.c_str(), "/tr[%d]/td[%d]", &tr, &td) != 2)
			return;

	
		if (tr > 0 && td>=0)
		{
			if (td == 0)
			{
				m_lottery_number.periods = atoi(val.c_str());
			}
			else if (td <= m_cfiBonus.m_regular_num)
			{
				m_lottery_number.number[td-1] = atoi(val.c_str());
			}
			else if (td == m_cfiBonus.m_regular_num + 1 && m_cfiBonus.m_special_num)
			{
				m_lottery_number.special_number = atoi(val.c_str());
			}
			else
			{
				if (m_lottery_number.periods > 0)
				{
					//m_lottery_number = CalcCharacter(m_cfiBonus.m_regular_num, m_lottery_number);
					m_cfiBonus.m_vec_lottery.insert(m_cfiBonus.m_vec_lottery.end(), m_lottery_number);

					m_lottery_number.periods = 0;
					for (int i = 0; i< 7; i++)
						m_lottery_number.number[i] = 0;
					m_lottery_number.special_number = 0;
				}
			}
		}
	}

private:
	LotteryNumber m_lottery_number;
	CLottoMxn & m_cfiBonus;

};

//////////////////////////////////////////////////////////////////////////

CLottoMxn::CLottoMxn(void)
{
	m_regular_num = 7;
	m_special_num = 1;
	m_lottery_name = "";
	m_lottery_code = "fj31x8";
}

CLottoMxn::~CLottoMxn(void)
{
}


string  CLottoMxn::SaveAsCsv()
{
	stringstream ss;

	m_vec_lottery.clear();
	bool bExist = false;

	try {
		ss << "select count(*) as cnt from lottery_mxn "
			<< " where Code=\'" << m_lottery_code << "\'  ";
		string test = ss.str();

		row r;
		session sql(g_MysqlPool);
		sql << SqlScript();
		sql << ss.str(), into(r);
		assert(r.size() == 1);
		long long existCnt = r.get<long long>("cnt");
		if (existCnt>0)
		{
			bExist = true;
		}
	}
	catch (std::exception const & e)
	{
		
	}

	vector<int>	cpqx;
	if (bExist)
		cpqx.push_back(200);
	else
	{
		time_t nowtime;
		nowtime = time(NULL); //获取日历时间  
		struct tm *local;
		local = localtime(&nowtime);  //获取当前系统时间  

		int nYear = local->tm_year + 1900;

		for (int n = 2005; n <= nYear; n++)
			cpqx.push_back(n);
	}
		
	try
	{
		char tmpCode[256] = {0};
		sprintf(tmpCode, "http://www.cpzst.com/%s.asp", m_lottery_code.c_str());

		if (bExist==false)
		{
			ss.str("");
			ss << m_lottery_name;
			ss << ", 开始下载数据 ";
			sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
		}

		for (int k = 0; k < cpqx.size(); k++)
		{
			if (bExist == false)
			{
				ss.str("");
				ss << "...... 年份 " << cpqx[k];
				sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
			}
			

			char tmpPost[256] = { 0 };
			sprintf(tmpPost, "cpqx =%d&Submit2=%CC%E1%BD%BB", cpqx[k]);

			string  strGBK = CUrlUtils::PostUrlOfSouhu(tmpCode, tmpPost);
			strGBK = Utf8_GBK(strGBK);
			//strGBK = ReplaceString(strGBK, "value=显示", "value=\'显示\'");

			HTML::ParserDom parser;
			parser.parse(strGBK);
			tree<HTML::Node> dom = parser.getTree();

			tree<HTML::Node>::iterator it = dom.begin();
			tree<HTML::Node>::iterator end = dom.end();
			int nLoopLimit = 0;
			for (; it != end; ++it)
			{
				if (it->tagName() == "TABLE")
				{
					it->parseAttributes();

					map<string, string> attrs = it->attributes();
					if (attrs.size()>0 && attrs["style"] == "border-collapse:collapse")
					{
						break;
					}

				}
				nLoopLimit++;
				if (nLoopLimit % 100 == 0)
					Sleep(10);
			}

			if (it != end)
			{
				CLottoMxnHtmlParser parser(dom, *this);
				parser.Parse(it, "");
			}
			Sleep(10);
		}
	
		if (m_vec_lottery.size() > 0)
			return "AlreadyInMemory";

		return "NoMoreData";
		
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
	}

	return "";
}


string CLottoMxn::SqlScript()
{
	stringstream ss;
	ss << "create table if not exists lottery_mxn (code varchar(32), name varchar(64), issue_date date, periods int, "
		<< " number_1 int, number_2 int,number_3 int, number_4 int, number_5 int, number_6 int, number_7 int, "
		<< " special_number int, sum_number int, odd int ,even int ,consecutive_number varchar(32), equal_tail varchar(32), "
		<< " var decimal(8,2), stdev decimal(8,2), devsq decimal(8,2), kurt decimal(8,2), skew decimal(8,2), "
		<< " PRIMARY KEY ( code, periods))";
	return ss.str();
}

void CLottoMxn::ImportToDatabase(void)
{
	stringstream ss;

	if(m_vec_lottery.size()<=0)
	{
		ss.str("");
		ss << m_lottery_name;
		ss << " Err, 数据下载错误. \r\n";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
		return;
	}


	try
	{
		vector<string>  vecSqlStr;
		
		for (int row = 0; row < m_vec_lottery.size(); row++)
		{
			if (1)
			{
				ss.str("");
				ss << "insert into lottery_mxn ("
					<< "code, name, periods, number_1, number_2, number_3, number_4, number_5, number_6, number_7,"
					<< "special_number, sum_number, odd, even,  equal_tail, var, stdev, devsq, kurt, skew )"
					<< " select \'" << m_lottery_code << "\'"
					<< ",\'" << m_lottery_name << "\'"
					<< "," << Int2String(m_vec_lottery[row].periods)
					<< "," << m_vec_lottery[row].number[0]
					<< "," << m_vec_lottery[row].number[1]
					<< "," << m_vec_lottery[row].number[2]
					<< "," << m_vec_lottery[row].number[3]
					<< "," << m_vec_lottery[row].number[4]
					<< "," << m_vec_lottery[row].number[5]
					<< "," << m_vec_lottery[row].number[6]
					<< "," << m_vec_lottery[row].special_number
					<< "," << Int2String(m_vec_lottery[row].sum)
					<< "," << m_vec_lottery[row].odd
					<< "," << m_vec_lottery[row].even
					//<< ",\'" << m_vec_lottery[row].consecutive_number << "\'"
					<< ",\'" << m_vec_lottery[row].equal_tail << "\'"
					<< "," << Double2String(m_vec_lottery[row].var)
					<< "," << Double2String(m_vec_lottery[row].stdev)
					<< "," << Double2String(m_vec_lottery[row].devsq)
					<< "," << Double2String(m_vec_lottery[row].kurt)
					<< "," << Double2String(m_vec_lottery[row].skew)

					<< " from dual where not exists (SELECT 1 from lottery_mxn "
					<< " where code=\'" << m_lottery_code << "\' "
					<< " and periods=" << Int2String(m_vec_lottery[row].periods) << "  ) ";

				string test = ss.str();
				vecSqlStr.insert(vecSqlStr.end(), test);

			}
		}

		if (vecSqlStr.size()>0)
		{
			session sql(g_MysqlPool);

			ss.str("");
			ss << SqlScript();
			sql << ss.str();

			for (int i = 0; i< vecSqlStr.size(); i++)
				sql << vecSqlStr[i];
			
			ss.str("");
			ss << m_lottery_name;
			ss << ", 共有数据 " << vecSqlStr.size();
			ss << " OK, 数据导入完毕. \r\n";
			sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
		}


	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << '\r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}



void CLottoMxn::NotifyDatabaseImportedToWindow()
{
	stringstream ss;
	/*ss << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>";
	ss << "<root>";
	ss << "<msg_type code=\"" << m_nCode << "\" ";
	ss << " model=\"" << ModelType() << "\" ";
	ss << " >DatabaseImported</msg_type>";
	ss << "<data " ;
	if(m_vec_close.size()>0)
	{
		ss << " param1=\"" << m_vec_report_date[0] << "\" ";
		ss << " param2=\"" << Double2String(m_vec_close[0]) << "\" ";
	}
	ss << " >";
	ss << "</data></root>";*/

	//sendToOutput(ss.str().c_str(), m_hWnd);
}
