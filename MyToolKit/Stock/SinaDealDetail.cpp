#include "StdAfx.h"
#include "SinaDealDetail.h"

#include "UrlUtils.h"

CSinaDealDetail::CSinaDealDetail(void)
{
}

CSinaDealDetail::~CSinaDealDetail(void)
{
}



string CSinaDealDetail::SqlScript()
{
	stringstream ss;
	ss << "create table if not exists " 
		<< "SinaDealDetail_" << ReplaceString(m_request_trade_date, "-", "") << " ( Code int , deal_time time, deal_type varchar(32), " 
		<< " deal_price DECIMAL(10,2),  deal_volume DECIMAL(12,0),  deal_turnover DECIMAL(15,2), "
		<< " PRIMARY KEY ( Code, deal_type, deal_time))";
	return ss.str();
}

string  CSinaDealDetail::SaveAsCsv()
{
	stringstream ss;

	m_vec_deal_time.clear();
	m_vec_deal_price.clear();
	m_vec_deal_volume.clear();
	m_vec_deal_turnover.clear();
	m_vec_deal_type.clear();

	
	char tmp[256] = { 0 };
	if(m_nCode<600000)
		snprintf (tmp, 255, "http://market.finance.sina.com.cn/downxls.php?date=%s&symbol=sz%06d",
			m_request_trade_date.c_str(),  m_nCode);	
	else
		snprintf (tmp, 255, "http://market.finance.sina.com.cn/downxls.php?date=%s&symbol=sh%06d",
		m_request_trade_date.c_str(),  m_nCode);
	string strMonth = string(tmp);

	string htmlPage =CUrlUtils::PostUrlOfSouhu(strMonth,NULL);
	vector<string> vecLine;
	splitString(htmlPage,"\r\n" , vecLine);
	if(vecLine.size()>2)
	{
		for(int i=1; i<vecLine.size(); i++)
		{
			vector<string>	vecYield;
			splitString(vecLine[i],"\t\s" , vecYield);
			if(vecYield.size()==6 &&  atoi(vecYield[3].c_str())>0)
			{
				m_vec_deal_time.insert(m_vec_deal_time.end(), vecYield[0]);
				m_vec_deal_price.insert(m_vec_deal_price.end(), String2Double(vecYield[1]));
				m_vec_deal_volume.insert(m_vec_deal_volume.end(), atoi(vecYield[3].c_str()));
				m_vec_deal_turnover.insert(m_vec_deal_turnover.end(), String2Double(vecYield[4]));
				m_vec_deal_type.insert(m_vec_deal_type.end(), vecYield[5]);
			}
		}

		if(m_vec_deal_time.size()>0)
		{
			return "AlreadyInMemory";
		}
	}
	return "";
}



void CSinaDealDetail::ImportToDatabase(void)
{
	if(m_nCode<=0)
		return;

	stringstream ss;
	try
	{
		ss << "【"  << Int2String(m_nCode) << "】";
		ss << "CSinaDealDetail::ImportToDatabase 开始导入数据.....\r\n";
		//sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//session sql(backEnd, m_ConnectString);
		session sql(g_MysqlPool);

		ss.str("");
		ss << SqlScript();
		sql << ss.str();

		ss.str("");
		ss << "insert into SinaDealDetail_" << ReplaceString(m_request_trade_date, "-", "") << " ("
			<< "Code, deal_time, deal_type, deal_price, deal_volume, " 
			<< "deal_turnover ) values ";

		for (int row = 0; row < m_vec_deal_time.size(); row++)
		{
			ss	<< "( " << Int2String(m_nCode) << ", \'" 
				<< m_vec_deal_time[row] << "\', \'"
				<< m_vec_deal_type[row] << "\',"
				<< Double2String(m_vec_deal_price[row]) << ", "
				<< Double2String(m_vec_deal_volume[row]) << ","
				<< Double2String(m_vec_deal_turnover[row]) << " ) ";

			if(row< m_vec_deal_time.size() - 1)
				ss << ",";
		}

		string test = ss.str();
		//test = Gbk2Utf8(test);
		sql << test; 

		ss.str("");
		ss << "【"  << Int2String(m_nCode) << "】";
		ss << "共有数据 " << m_vec_deal_time.size() ;
		ss << " OK, CSinaDealDetail::ImportToDatabase 数据导入完毕. \r\n";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
		
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << '\r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}



void CSinaDealDetail::NotifyDatabaseImportedToWindow()
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

void CSinaDealDetail::ExportFromDatabase(double startTime, double endTime)
{
	
}

vector<int>	CSinaDealDetail::NeedDownload()
{
	stringstream ss;

	map<int, int>	mapCode;
	CEntrepreneurship  entrepreneurship;
	CBussinessUtils::GetEntrepreneurship(entrepreneurship);
	for(int i=0; i< entrepreneurship.m_nRowNum; i++)
		mapCode[entrepreneurship.m_vec_code[i]] = entrepreneurship.m_vec_code[i];

	CSmallMidEnterprise  smallMid;
	CBussinessUtils::GetSmallMidEnterprise(smallMid);
	for(int i=0; i< smallMid.m_nRowNum; i++)
		mapCode[smallMid.m_vec_code[i]] = smallMid.m_vec_code[i];

	CShenzhenAshares  shenzhenA;
	CBussinessUtils::GetShenzhenAshares(shenzhenA);
	for(int i=0; i< shenzhenA.m_nRowNum; i++)
		mapCode[shenzhenA.m_vec_code[i]] = shenzhenA.m_vec_code[i];

	CShanghaiStockInfo  shanghaiA;
	CBussinessUtils::GetShanghaiStockInfo(shanghaiA);
	for(int i=0; i< shanghaiA.m_nRowNum; i++)
		mapCode[shanghaiA.m_vec_code[i]] = shanghaiA.m_vec_code[i];

	try
	{
		ss.str("");
		ss << " select Code , count(*) as cnt  ";
		ss << " from SinaDealDetail_" << ReplaceString(m_request_trade_date, "-", "") << " group by Code order by Code ";

		string test = ss.str();
		test = Gbk2Utf8(test);

		row r;
		session sql(g_MysqlPool);

		statement st = (sql.prepare << test,into(r)) ;
		st.execute();

		int lastCnt = 0;
		while (st.fetch())
		{
			int	nCode = r.get<int>("Code");
			int cnt = r.get<long long>("cnt");

			if(cnt >0)   
			{
				//	
				mapCode.erase(nCode);
			}
		}

	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << '\r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}

	vector<int>	vecCode;
	map<int, int>::iterator it = mapCode.begin();
	for(; it!=mapCode.end(); it++)
	{
		vecCode.insert(vecCode.end(), it->first);
	}
	return vecCode;
}
