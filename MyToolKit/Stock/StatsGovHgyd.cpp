#include "StdAfx.h"
#include "StatsGovHgyd.h"

#include "UrlUtils.h"

CStatsGovHgyd::CStatsGovHgyd(void)
{
}

CStatsGovHgyd::~CStatsGovHgyd(void)
{
}



string  CStatsGovHgyd::SaveAsCsv()
{
	stringstream ss;

	zb_code.clear();
	zb_name.clear();
	sj_code.clear();
	data.clear();
	unit.clear();

	string max_sj_code = LastSjCodeFromDatabase();
	if(max_sj_code.empty())
	{
		max_sj_code = "199201-";
	}
	else
	{
		/* 寻找最早需要下载月份 */
		int nYear = atoi(max_sj_code.substr(0,4).c_str());
		int nMonth = atoi(max_sj_code.substr(4,2).c_str());

		if(nMonth>=12)
		{
			nMonth=1;
			nYear++;
		}
		else
			nMonth++;
	
		char tmp[32] = { 0};
		sprintf_s(tmp, 32, "%04d%02d-", nYear, nMonth);
		max_sj_code = string(tmp);

		/* 判断当前时间对应的年份季度 */
		time_t time_current =  CBussinessUtils::CurrentTradingDate();
		struct tm curr_tm;
		localtime_s(&curr_tm, &time_current);

		sprintf_s(tmp, 32, "%04d%02d-", 1900 + curr_tm.tm_year, curr_tm.tm_mon+1);
		string curr_sj_code = string(tmp);
		
		if(curr_sj_code<=max_sj_code)
		{
			ModifyUpdateDate("hgyd", m_request_zb_code);
			return "NoMoreData";
		}
	}
	

			
	if(Logon())
	{
		/* 登录成功则获取各指标数据 */

		ss.str("");
		ss << "【" << m_request_zb_code << "】";
		ss << "开始获取数据的起始时间 ：" << max_sj_code;
		ss << "\r\n";
		sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);	

		string test1 = string("http://data.stats.gov.cn/easyquery.htm?m=QueryData&dbcode=hgyd&rowcode=zb&colcode=sj&wds=") + 
			UrlEncode("[]") + "&dfwds=" +
			UrlEncode("[{\"wdcode\":\"zb\",\"valuecode\":\"" + m_request_zb_code + "\"}, {\"wdcode\":\"sj\",\"valuecode\":\"" + max_sj_code + "\"}]") + 
			string("&k1=1471400916895");

		string strGBK = PostUrlOfStatsGov(test1, NULL);
		//strGBK = Utf8_GBK(strGBK);

		Json::Value jsonRoot;
		Json::Reader reader(Json::Features::strictMode());
		bool parsingSuccessful = reader.parse(strGBK, jsonRoot);
		if (!parsingSuccessful) {
			ss.str("");
			ss << "统计局月度数据，解析 JSON 错误 ：";
			if(strGBK.length()<256)
				ss << strGBK;
			else
				ss << reader.getFormattedErrorMessages().c_str();
			ss << "\r\n";
			sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
		}
		else
		{
			Json::Value json_result = jsonRoot["returndata"];
			if(json_result.type()==Json::stringValue)
			{
				ss.str("");
				ss << "统计局月度数据 ： \"";
				ss << json_result.asString();
				ss << "\" \r\n";
				sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
			}
			else
			{
				//Json::Value & datanodes = json_result["datanodes"];
				//Json::Value & wdnodes = json_result["wdnodes"];
				if(json_result["datanodes"].type()== Json::arrayValue)
				{
					string strZbLast = "";
					string strZbNameLast = "";
					string strUnitLast = "";
					for(int index = 0; index< json_result["datanodes"].size(); index++)
					{
						Json::Value  datanode = json_result["datanodes"][index];

						if(datanode["data"]["hasdata"].asBool()!=true)
							continue;

						data.insert(data.end(), String2Double(datanode["data"]["strdata"].asString()));
						string strZb = FindZbCodeFromDataNode(datanode, "zb");
						zb_code.insert(zb_code.end(), strZb);
						sj_code.insert(sj_code.end(), FindZbCodeFromDataNode(datanode, "sj"));

						if(strZb!=strZbLast)
						{
							FindZbNameFromBottom(json_result["wdnodes"], "zb", strZb, strZbNameLast, strUnitLast);
							strZbLast = strZb;
						}
						zb_name.insert(zb_name.end(), strZbNameLast);
						unit.insert(unit.end(), strUnitLast);
					}

					if(m_request_zb_code=="A010101")
					{
						int wwww = 1;
					}

					ss.str("");
					ss << "【" << m_request_zb_code << "】";
					ss << " 统计局月度数据，获取 JSON 成功.";
					ss << " 总共数据节点 ： " << json_result["datanodes"].size();
					ss << " 有数据节点 ： " << data.size();
					ss << "\r\n";
					sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);	

					if(data.size()==0)
					{
						//	向后延时 7 天再尝试下载
						ModifyUpdateDate("hgyd", m_request_zb_code, 7);
						return "NoMoreData";
					}
				}
				else
				{
					ss.str("");
					ss << "统计局月度数据，获取 JSON 中的 hq 数据失败. \r\n";
					sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
				}

				json_result.clear();
			}

			jsonRoot.clear();
		}
	}
	else
	{
		
	}
	
	if(zb_name.size()>0)
	{
			return "AlreadyInMemory";
	}
	
	return "";
}

string CStatsGovHgyd::SqlScript()
{
	stringstream ss;
	ss << "create table if not exists " 
		<< " StatsGovHgyd ( zb_code varchar(32), zb_name varchar(128), " 
		<< " sj_code varchar(32),  data DECIMAL(15,2),  unit varchar(32) , pid varchar(32), "
		<< " PRIMARY KEY ( zb_code, sj_code))";
	return ss.str();
}

void CStatsGovHgyd::ImportToDatabase(void)
{
	if(zb_code.size()==0)
		return ;

	stringstream ss;
	try
	{
		ss << "【" << m_request_zb_code << "】";
		ss << "CStatsGovHgyd::ImportToDatabase 开始导入数据.....\r\n";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//session sql(backEnd, m_ConnectString);
		session sql(g_MysqlPool);

		ss.str("");
		ss << SqlScript();
		sql << ss.str();

		ss.str("");
		ss << "insert into StatsGovHgyd ("
			<< "zb_code, zb_name, sj_code, data, pid, unit ) " 
			<< "values ";

		for (int row = 0; row < zb_code.size(); row++)
		{
			ss	<< "( \'" << zb_code[row] << "\' , \'" 
				<< /*Gbk2Utf8*/(zb_name[row]) << "\', \'"
				<< sj_code[row] << "\',"
				<< Double2String(data[row]) << ",\'"
				<< m_request_zb_code << "\',\'"
				<< /*Gbk2Utf8*/(unit[row]) << "\' )";

			if(row< zb_code.size() - 1)
				ss << ",";
		}

		sql << ss.str();

		ModifyUpdateDate("hgyd", m_request_zb_code);

		ss.str("");
		ss << "【"  << m_request_zb_code << "】";
		ss << "共有数据 " << zb_code.size() ;
		ss << " OK, CStatsGovHgyd::ImportToDatabase 数据导入完毕. \r\n";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << '\r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}


void CStatsGovHgyd::NotifyDatabaseImportedToWindow()
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

void CStatsGovHgyd::ExportFromDatabase(double startTime, double endTime)
{
	
}


string CStatsGovHgyd::LastSjCodeFromDatabase()
{
	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();
	string lastTradeDate = "";
	try
	{
		ss << "【" << m_request_zb_code << "】";
		ss << "CStatsGovHgyd::LastSjCodeFromDatabase 开始计算.....\r\n";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << " select max(sj_code) as sj_code  "
			<< " from StatsGovHgyd where  pid=\'" << m_request_zb_code << "\'" ;

		string test = ss.str();

		row r;
		//session sql(backEnd, m_ConnectString);
		session sql(g_MysqlPool);

		statement st = (sql.prepare << ss.str(),into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{
			lastTradeDate = r.get<string>("sj_code");
			break;

			cnt++;
		}

	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << "CStatsGovHgyd::LastSjCodeFromDatabase maxdate = NULL.";
		ss << "\r\n";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	return lastTradeDate;
}


vector<string>	CStatsGovHgyd::NeedDownload()
{
	GetMenuTree("hgyd", "zb", "zb");
	SaveMenuTree();

	vector<string>  vec = CStatsGovCn::NeedDownload("hgyd");
	
	stringstream ss;
	ss << "【" << m_request_zb_code << "】";
	ss << "CStatsGovHgyd::NeedDownload, "  << vec.size() << " 个指标菜单 " ;
	ss << "\r\n";
	sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	return vec;
}