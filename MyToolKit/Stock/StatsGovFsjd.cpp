#include "StdAfx.h"
#include "StatsGovFsjd.h"

#include "UrlUtils.h"

CStatsGovFsjd::CStatsGovFsjd(void)
{
}

CStatsGovFsjd::~CStatsGovFsjd(void)
{
}




string  CStatsGovFsjd::SaveAsCsv()
{
	stringstream ss;

	zb_code.clear();
	zb_name.clear();
	sj_code.clear();
	data.clear();
	unit.clear();

	reg_code.clear();
	reg_name.clear();

	string max_sj_code = LastSjCodeFromDatabase();
	if(max_sj_code.empty())
	{
		max_sj_code = "1992-";
	}
	else
	{
		/* 寻找最早需要下载季度 */
		int nYear = atoi(max_sj_code.substr(0,4).c_str());
		char strQuarter = max_sj_code.substr(4)[0];

		switch(strQuarter)
		{
		case 'A':
			strQuarter = 'B';
			break;
		case 'B':
			strQuarter = 'C';
			break;
		case 'C':
			strQuarter = 'D';
			break;
		case 'D':
			{
				strQuarter = 'A';
				nYear ++;
			}
			break;
		default:
			break;
		}

		char tmp[32] = { 0};
		sprintf_s(tmp, 32, "%04d%c", nYear, strQuarter);
		max_sj_code = string(tmp);

		/* 判断当前时间对应的年份季度 */
		time_t time_current =  CBussinessUtils::CurrentTradingDate();
		//struct tm * curr_tm = gmtime (&time_current);
		struct tm curr_tm;
		localtime_s(&curr_tm, &time_current);

		string curr_sj_code = Int2String(1900 + curr_tm.tm_year);
		switch(curr_tm.tm_mon+1)
		{
		case 1:
		case 2:
		case 3:
			curr_sj_code += "A";
			break;

		case 4:
		case 5:
		case 6:
			curr_sj_code += "B";
			break;

		case 7:
		case 8:
		case 9:
			curr_sj_code += "C";
			break;

		case 10:
		case 11:
		case 12:
			curr_sj_code += "D";
			break;
		}

		if(curr_sj_code<=max_sj_code)
		{
			ModifyUpdateDate("fsjd", m_request_zb_code);
			return "NoMoreData";
		}

		max_sj_code += "-";
	}
	

			
	if(Logon())
	{
		/* 登录成功则获取各指标数据 */

		map<string, string>		mapReg = GetOtherWds("fsjd", m_request_zb_code);
		map<string, string>::iterator it = mapReg.begin();
		for(; it!=mapReg.end(); it++)
		{
			string regCode = it->first;
			string regName = it->second;
			ss.str("");
			ss << "【" << m_request_zb_code << "】【" << regName << "】";
			ss << "开始获取数据的起始时间 ：" << max_sj_code;
			ss << "\r\n";
			sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);	

			string test1 = string("http://data.stats.gov.cn/easyquery.htm?m=QueryData&dbcode=fsjd&rowcode=zb&colcode=sj&wds=") + 
				UrlEncode("[]") + "&dfwds=" +
				UrlEncode("[{\"wdcode\":\"zb\",\"valuecode\":\"" + m_request_zb_code + "\"}, {\"wdcode\":\"sj\",\"valuecode\":\"" + max_sj_code + "\"},") + 
				UrlEncode("{\"wdcode\":\"reg\",\"valuecode\":\"" + regCode + "\"}]") + 
				string("&k1=1471400916895");


			string test2 = UrlDecode("/easyquery.htm?m=getOtherWds&dbcode=fsjd&rowcode=zb&colcode=sj&wds=%5B%7B%22wdcode%22%3A%22zb%22%2C%22valuecode%22%3A%22A030102%22%7D%5D&k1=1472191976818");

			string test3 = UrlDecode("/easyquery.htm?m=QueryData&dbcode=fsjd&rowcode=zb&colcode=sj&wds=%5B%7B%22wdcode%22%3A%22reg%22%2C%22valuecode%22%3A%22110000%22%7D%5D&dfwds=%5B%5D&k1=1472191976915");

			string strGBK = PostUrlOfStatsGov(test1, NULL);
			//strGBK = Utf8_GBK(strGBK);

			Json::Value jsonRoot;
			Json::Reader reader(Json::Features::strictMode());
			bool parsingSuccessful = reader.parse(strGBK, jsonRoot);
			if (!parsingSuccessful) {
				ss.str("");
				ss << "统计局 分省季度数据，解析 JSON 错误 ：";
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
					ss << "统计局 分省季度数据 ： \"";
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

							string strReg = FindZbCodeFromDataNode(datanode, "reg");
							reg_code.insert(reg_code.end(), strReg);
							reg_name.insert(reg_name.end(), regName);

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
						ss << "【" << m_request_zb_code << "】【" << regName << "】";
						ss << " 统计局 分省季度数据，获取 JSON 成功.";
						ss << " 总共数据节点 ： " << json_result["datanodes"].size();
						ss << " 有数据节点 ： " << data.size();
						ss << "\r\n";
						sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);	

						
					}
					else
					{
						ss.str("");
						ss << "统计局 分省季度数据，获取 JSON 中的 hq 数据失败. \r\n";
						sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
					}

					json_result.clear();
				}

				jsonRoot.clear();
			}
		} // end for

		if(data.size()==0)
		{
			//	向后延时 7 天再尝试下载
			ModifyUpdateDate("fsjd", m_request_zb_code, 7);
			return "NoMoreData";
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

string CStatsGovFsjd::SqlScript()
{
	stringstream ss;
	ss << "create table if not exists " 
		<< " StatsGovFsjd ( zb_code varchar(32), zb_name varchar(128), reg_code varchar(32), reg_name varchar(64), " 
		<< " sj_code varchar(32),  data DECIMAL(15,2),  unit varchar(32) , pid varchar(32), "
		<< " PRIMARY KEY ( zb_code, sj_code, reg_code ))";
	return ss.str();
}

void CStatsGovFsjd::ImportToDatabase(void)
{
	if(zb_code.size()==0)
		return ;

	stringstream ss;
	try
	{
		ss << "【" << m_request_zb_code << "】";
		ss << "StatsGovFsjd::ImportToDatabase 开始导入数据.....\r\n";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//session sql(backEnd, m_ConnectString);
		session sql(g_MysqlPool);

		ss.str("");
		ss << SqlScript();
		sql << ss.str();

		// 分成 5000 条记录为 1 组进行存储
		for(int nStart = 0; nStart <  zb_code.size(); nStart = nStart + 5000)
		{
			int nEnd = (nStart + 5000)>= zb_code.size()? zb_code.size() : nStart + 5000;

			ss.str("");
			ss << "insert into StatsGovFsjd ("
				<< "zb_code, zb_name, reg_code, reg_name, sj_code, data, pid, unit ) " 
				<< "values ";

			for (int row = nStart; row < nEnd; row++)
			{
				ss	<< "( \'" << zb_code[row] << "\' , \'" 
					<< /*Gbk2Utf8*/(zb_name[row]) << "\', \'"
					<< reg_code[row] << "\' , \'" 
					<< /*Gbk2Utf8*/(reg_name[row]) << "\', \'"
					<< sj_code[row] << "\',"
					<< Double2String(data[row]) << ",\'"
					<< m_request_zb_code << "\',\'"
					<< /*Gbk2Utf8*/(unit[row]) << "\' )";

				if(row< nEnd - 1)
					ss << ",";
			}
			sql << ss.str();
		}
		
		ModifyUpdateDate("fsjd", m_request_zb_code);

		ss.str("");
		ss << "【"  << m_request_zb_code << "】";
		ss << "共有数据 " << zb_code.size() ;
		ss << " OK, StatsGovFsjd::ImportToDatabase 数据导入完毕. \r\n";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << '\r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}


void CStatsGovFsjd::NotifyDatabaseImportedToWindow()
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

void CStatsGovFsjd::ExportFromDatabase(double startTime, double endTime)
{
	
}


string CStatsGovFsjd::LastSjCodeFromDatabase()
{
	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();
	string lastTradeDate = "";
	try
	{
		ss << "【" << m_request_zb_code << "】";
		ss << "CStatsGovFsjd::LastSjCodeFromDatabase 开始计算.....\r\n";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << " select max(sj_code) as sj_code  "
			<< " from StatsGovFsjd where  pid=\'" << m_request_zb_code << "\'" ;

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
		ss << "CStatsGovFsjd::LastSjCodeFromDatabase maxdate = NULL.";
		ss << "\r\n";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	return lastTradeDate;
}


vector<string>	CStatsGovFsjd::NeedDownload()
{
	GetMenuTree("fsjd", "zb", "zb");
	SaveMenuTree();

	vector<string>  vec = CStatsGovCn::NeedDownload("fsjd");
	
	stringstream ss;
	ss << "【" << m_request_zb_code << "】";
	ss << "CStatsGovFsjd::NeedDownload, "  << vec.size() << " 个指标菜单 " ;
	ss << "\r\n";
	sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	return vec;
}