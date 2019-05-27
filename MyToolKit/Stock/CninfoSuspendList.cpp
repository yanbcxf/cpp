#include "StdAfx.h"
#include "CninfoSuspendList.h"

#include "UrlUtils.h"




CCninfoSuspendList::CCninfoSuspendList(void)
{
}

CCninfoSuspendList::~CCninfoSuspendList(void)
{
	
}



int CCninfoSuspendList::CheckReport(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData)
{
	return CSV_CHECK_NO_ERROR;
}



void CCninfoSuspendList::ParseCsvFile(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData)
{
	
}

void CCninfoSuspendList::ParseXmlElement(void)
{
}

bool CCninfoSuspendList::ParseCsvFileName(string szFileName)
{
	return true;
}

void CCninfoSuspendList::ImportToDatabase(void)
{
	/*if(m_nCode<=0)
		return;*/

	stringstream ss;
	try
	{
		ss << "CCninfoDelistingList::ImportToDatabase 开始导入数据.....\r\n";
		session sql(g_MysqlPool);

		sql << "call add_col('StockInfo','a_time_to_suspend','date', '')";

		for (int row = 0; row < secCode.size(); row++)
		{
			string stock_plate = "创业板";
			string stock_exchange = "深圳证券交易所";
			if(secCode[row].find("30")==0)
			{

			}
			else if(secCode[row].find("002")==0)
			{
				stock_plate = "中小企业板";
			}
			else if(secCode[row].find("60")==0)
			{
				stock_plate = "上海证券板";
				stock_exchange = "上海证券交易所";
			}
			else
			{
				stock_plate = "深圳A股";
			}

			ss.str("");
			ss << "insert into StockInfo ( code, a_code, abbreviation, a_abbreviation,  name, a_time_to_suspend )"
				<< " select " <<  secCode[row] << ", " 
				<< " " << (secCode[row]) << ", "
				<< " \'" << (secName[row]) << "\', "
				<< " \'" << (secName[row]) << "\', "
				<< " \'" << (secName[row]) << "\', "
				<< " \"" << suspendTime[row] << "\", "
				<< " \"" << stock_plate << "\", "
				<< " \"" << stock_exchange << "\"  "
				<< " from dual where not exists (SELECT 1 from StockInfo " 
				<< " where code =" << secCode[row] << "  ) ";

			string sql1 = ss.str();
			string sql1test = sql1;

			//sql1 = Gbk2Utf8(sql1);

			ss.str("");
			ss << "update StockInfo set "
				<< " a_code =" << (secCode[row]) << ", "
				<< " a_abbreviation =\'" << (secName[row]) << "\', "
				<< " abbreviation =\'" << (secName[row]) << "\', "
				<< " name =\'" << (secName[row]) << "\', "
				<< " a_time_to_suspend =\"" << suspendTime[row] << "\", "
				<< " stock_plate =\"" << stock_plate << "\", "
				<< " securities_exchange = \"" << stock_exchange << "\" "
				<< " where code =" << secCode[row] << "   ";

			string sql2 = ss.str();
			string sql2test = sql2;
			//sql2 = Gbk2Utf8(sql2);

			try{
				sql << sql1;
				sql << sql2;
			}
			catch (std::exception const & e)
			{
				ss.str("");
				ss << "CCninfoSuspendList::ImportToDatabase,  code【" << secName[row] << "】";
				ss << e.what() << '\n';
				sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
			}

			//sendToOutput(ss.str().c_str());
		}
		

		string test = ss.str();
		//test = Gbk2Utf8(test);
		sql << test;
		
		ss.str("");
		ss << "共有数据 " << secName.size() ;
		ss << " OK, CCninfoSuspendList::ImportToDatabase 数据导入完毕. \r\n";
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

CStockDataModel * CCninfoSuspendList::NewCopy()
{
	return NULL;
}



string CCninfoSuspendList::GetDownloadUrl()
{
	return "";
}


string CCninfoSuspendList::SqlScript()
{
	return "";
}



string  CCninfoSuspendList::SaveAsCsv()
{
	stringstream ss;

	for(int k=0; k<2 ;k++)
	{
		ss.str("");
		if(k==0)
			ss << "market=sz";
		else
			ss << "market=sh";
		string jsonPage =CUrlUtils::PostUrlOfSouhu("http://www.cninfo.com.cn//cninfo-new/information/suspendlist-1",(char *)ss.str().c_str());
		jsonPage = Utf8_GBK(jsonPage);

		Json::Value jsonRoot;
		Json::Reader reader(Json::Features::strictMode());
		bool parsingSuccessful = reader.parse(jsonPage, jsonRoot);
		if (!parsingSuccessful) {
			ss.str("");
			ss << "CCninfoSuspendList::SaveAsCsv，解析 JSON 错误 " ;
			ss << reader.getFormattedErrorMessages().c_str();
			sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
		}
		else
		{
			if(jsonRoot.type()== Json::arrayValue)
			{
				int size = jsonRoot.size();
				if(size>0)
				{
					for(int index = 0; index< size; index++)
					{
						Json::Value  struction = jsonRoot[index];

						string strCode = struction["seccode"].asString();
						bool isNeed  = strCode.find("00")==0 ? true : false;
						if(isNeed==false)
							isNeed  = strCode.find("30")==0 ? true : false;
						if(isNeed==false)
							isNeed  = strCode.find("60")==0 ? true : false;

						if(isNeed)
						{
							secCode.insert(secCode.end(), struction["seccode"].asString());
							string strName = struction["secname"].asString();
							strName = strName + "(暂)";
							secName.insert(secName.end(), strName);
							suspendTime.insert(suspendTime.end(), struction["suspenddate"].asString());
						}
					}
				}
				ss.str("");
				ss << "CCninfoSuspendList::SaveAsCsv，获取 JSON 数据成功.";
				//sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
			}
			else
			{
				ss.str("");
				ss << "CCninfoSuspendList::SaveAsCsv，获取 JSON 中的 Result 数据失败." ;
				sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
			}
		}
	}
	
	int nSize = secCode.size();
	if(nSize==secName.size() && 
		nSize > 0)
	{
	
		return "AlreadyInMemory";
	}

	return "NoMoreData";
}

string CCninfoSuspendList::GetCsvFileName()
{
	return "";
}


void CCninfoSuspendList::ExportFromDatabase(double startTime, double endTime)
{
	
}


string CCninfoSuspendList::LastTradeDateFromDatabase()
{
	return "";
}

void CCninfoSuspendList::NotifyDatabaseImportedToWindow()
{
	
}
