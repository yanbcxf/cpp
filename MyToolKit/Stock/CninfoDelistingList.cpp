#include "StdAfx.h"
#include "CninfoDelistingList.h"

#include "UrlUtils.h"




CCninfoDelistingList::CCninfoDelistingList(void)
{
}

CCninfoDelistingList::~CCninfoDelistingList(void)
{
	
}



int CCninfoDelistingList::CheckReport(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData)
{
	return CSV_CHECK_NO_ERROR;
}



void CCninfoDelistingList::ParseCsvFile(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData)
{
	
}

void CCninfoDelistingList::ParseXmlElement(void)
{
}

bool CCninfoDelistingList::ParseCsvFileName(string szFileName)
{
	return true;
}

void CCninfoDelistingList::ImportToDatabase(void)
{
	/*if(m_nCode<=0)
		return;*/

	stringstream ss;
	try
	{
		ss << "CCninfoDelistingList::ImportToDatabase 开始导入数据.....\r\n";
		session sql(g_MysqlPool);

		sql << "call add_col('StockInfo','a_time_to_delisting','date', '')";

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
			ss << "insert into StockInfo ( code, a_code, abbreviation, a_abbreviation, name, a_time_to_delisting, stock_plate, securities_exchange )"
				<< " select " <<  secCode[row] << ", " 
				<< " " << (secCode[row]) << ", "
				<< " \'" << (secName[row]) << "\', "
				<< " \'" << (secName[row]) << "\', "
				<< " \'" << (secName[row]) << "\', "
				<< " \"" << delistingTime[row] << "\", "
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
				<< " abbreviation =\'" << (secName[row]) << "\', "
				<< " a_abbreviation =\'" << (secName[row]) << "\', "
				<< " abbreviation =\'" << (secName[row]) << "\', "
				<< " name =\'" << (secName[row]) << "\', "
				<< " a_time_to_delisting =\"" << delistingTime[row] << "\", "
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
				ss << "CCninfoDelistingList::ImportToDatabase,  code【" << secName[row] << "】";
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
		ss << " OK, CCninfoDelistingList::ImportToDatabase 数据导入完毕. \r\n";
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

CStockDataModel * CCninfoDelistingList::NewCopy()
{
	return NULL;
}



string CCninfoDelistingList::GetDownloadUrl()
{
	return "";
}


string CCninfoDelistingList::SqlScript()
{
	return "";
}



string  CCninfoDelistingList::SaveAsCsv()
{
	stringstream ss;

	for(int k=0; k<2 ;k++)
	{
		ss.str("");
		if(k==0)
			ss << "market=sz";
		else
			ss << "market=sh";
		string jsonPage =CUrlUtils::PostUrlOfSouhu("http://www.cninfo.com.cn//cninfo-new/information/delistinglist-1",(char *)ss.str().c_str());
		jsonPage = Utf8_GBK(jsonPage);

		Json::Value jsonRoot;
		Json::Reader reader(Json::Features::strictMode());
		bool parsingSuccessful = reader.parse(jsonPage, jsonRoot);
		if (!parsingSuccessful) {
			ss.str("");
			ss << "CCninfoDelistingList::SaveAsCsv，解析 JSON 错误 " ;
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

						string strCode = struction["y_seccode_0007"].asString();
						bool isNeed  = strCode.find("00")==0 ? true : false;
						if(isNeed==false)
							isNeed  = strCode.find("30")==0 ? true : false;
						if(isNeed==false)
							isNeed  = strCode.find("60")==0 ? true : false;

						if(isNeed)
						{
							secCode.insert(secCode.end(), struction["y_seccode_0007"].asString());
							string strName = struction["y_secname_0007"].asString();
							strName = strName + "(退)";
							secName.insert(secName.end(), strName);
							delistingTime.insert(delistingTime.end(), struction["f008d_0007"].asString());
						}
					}
				}
				ss.str("");
				ss << "CCninfoDelistingList::SaveAsCsv，获取 JSON 数据成功.";
				//sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
			}
			else
			{
				ss.str("");
				ss << "CCninfoDelistingList::SaveAsCsv，获取 JSON 中的 Result 数据失败." ;
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

string CCninfoDelistingList::GetCsvFileName()
{
	return "";
}


void CCninfoDelistingList::ExportFromDatabase(double startTime, double endTime)
{
	
}


string CCninfoDelistingList::LastTradeDateFromDatabase()
{
	return "";
}

void CCninfoDelistingList::NotifyDatabaseImportedToWindow()
{
	
}
