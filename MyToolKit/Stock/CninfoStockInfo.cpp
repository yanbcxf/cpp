#include "StdAfx.h"
#include "CninfoStockInfo.h"

#include "UrlUtils.h"



CCninfoStockInfo::CCninfoStockInfo(void)
{
}

CCninfoStockInfo::~CCninfoStockInfo(void)
{
	
}



int CCninfoStockInfo::CheckReport(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData)
{
	return CSV_CHECK_NO_ERROR;
}



void CCninfoStockInfo::ParseCsvFile(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData)
{
	
}

void CCninfoStockInfo::ParseXmlElement(void)
{
}

bool CCninfoStockInfo::ParseCsvFileName(string szFileName)
{
	vector<string> firstMatch;
	if(Pcre2Grep(_T("SouhuTradingDaily\\(\\d+\\)"), szFileName,firstMatch )<=0)
		return false;

	string::size_type pos1 = firstMatch[0].find(")");
	string codeStr = firstMatch[0].substr(18, pos1-18);
	m_nCode = atoi(codeStr.c_str());
	return true;
}

void CCninfoStockInfo::ImportToDatabase(void)
{
	
	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();

	try
	{
		ss << "CCninfoStockInfo::ImportToDatabase 开始导入数据.....\r\n";
		//sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//session sql(backEnd, m_ConnectString);
		session sql(g_MysqlPool);

		sql << "call add_col('stockinfo', 'cninfo_orgid', 'varchar(32)', '')";
		sql << "call add_col('stockinfo', 'cninfo_category', 'varchar(32)', '')";
		sql << "call add_col('stockinfo', 'cninfo_code', 'varchar(32)', '')";
		sql << "call add_col('stockinfo', 'cninfo_pinyin', 'varchar(32)', '')";
		sql << "call add_col('stockinfo', 'cninfo_zwjc', 'varchar(32)', '')";

		for(int i=0; i< code.size(); i++)
		{
			ss.str("");
			ss << "update stockinfo set "
				<< "cninfo_orgid=\'" << orgId[i] << "\',"
				<< "cninfo_category=\'" << category[i] << "\',"
				<< "cninfo_code=\'" << code[i] << "\',"
				<< "cninfo_pinyin=\'" << pinyin[i] << "\',"
				<< "cninfo_zwjc=\'" << zwjc[i] << "\'  "
				<< "where code = " << atoi(code[i].c_str()) ;
			string test = ss.str();
			//test = Gbk2Utf8(test);
			sql << test;
		}
		
		ss.str("");
		ss << "共有数据 " << zwjc.size() ;
		ss << " OK, CCninfoStockInfo::ImportToDatabase 数据导入完毕. \r\n";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
		//return EXIT_SUCCESS;
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << '\r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}

CStockDataModel * CCninfoStockInfo::NewCopy()
{
	return NULL;
}



string CCninfoStockInfo::GetDownloadUrl()
{
	return "";
}


string CCninfoStockInfo::SqlScript()
{
	stringstream ss;
	
	return ss.str();
}



string  CCninfoStockInfo::SaveAsCsv()
{
	stringstream ss;

	time_t time_start;
	time_t time_end;

	string strGBK =  CUrlUtils::PostUrlOfSouhu("http://www.cninfo.com.cn/cninfo-new/js/data/szse_stock.json",NULL); ;
	strGBK = Utf8_GBK(strGBK);

	vector<string> vecMatch;
	if(strGBK.find("{\"stockList\":")!=string::npos)
	{
		Json::Value jsonRoot;
		Json::Reader reader(Json::Features::strictMode());
		bool parsingSuccessful = reader.parse(strGBK, jsonRoot);
		if (!parsingSuccessful) {
			ss.str("");
			ss << "CCninfoStockInfo::SaveAsCsv，解析 JSON 错误 " ;
			ss << reader.getFormattedErrorMessages().c_str();
			sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
		}
		else
		{
			Json::Value json_result = jsonRoot["stockList"];
			if(json_result.type()== Json::arrayValue)
			{
				int size = json_result.size();
				if(size>0)
				{
					for(int index = 0; index< size; index++)
					{
						Json::Value  struction = json_result[index];

						orgId.insert(orgId.end(), struction["orgId"].asString());
						category.insert(category.end(), struction["category"].asString());
						code.insert(code.end(), struction["code"].asString());
						pinyin.insert(pinyin.end(), struction["pinyin"].asString());
						zwjc.insert(zwjc.end(), struction["zwjc"].asString());
					}
				}
				ss.str("");
				ss << "CCninfoStockInfo::SaveAsCsv，获取 JSON 数据成功.";
				//sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
			}
			else
			{
				ss.str("");
				ss << "CCninfoStockInfo::SaveAsCsv，获取 JSON 中的 Result 数据失败." ;
				sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
			}

		}
	}

	if(code.size()==orgId.size() &&
		code.size()==category.size() && 
		code.size()==pinyin.size() &&
		code.size()==zwjc.size() &&
		code.size()>0)
		return "AlreadyInMemory";

	return "";
}

string CCninfoStockInfo::GetCsvFileName()
{
	return "";
}


void CCninfoStockInfo::ExportFromDatabase(double startTime, double endTime)
{
	
}


string CCninfoStockInfo::LastTradeDateFromDatabase()
{
	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();
	string lastTradeDate = "";
	
	return lastTradeDate;
}

void CCninfoStockInfo::NotifyDatabaseImportedToWindow()
{
	/*stringstream ss;
	ss << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>";
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
	ss << "</data></root>";

	sendToOutput(ss.str().c_str(), m_hWnd);*/
}
