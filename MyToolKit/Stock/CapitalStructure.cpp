#include "StdAfx.h"
#include "Stock.h"
#include "CapitalStructure.h"

#include "UrlUtils.h"


CCapitalStructure::CCapitalStructure(void)
{
}

CCapitalStructure::~CCapitalStructure(void)
{
	m_vec_real_date.clear();
	m_vec_ashares.clear();
	m_vec_bshares.clear();
	m_vec_change_reason.clear();
	m_vec_change_reason_desc.clear();
	m_vec_seq.clear();
	m_vec_total_shares.clear();
	m_vec_a_total_shares.clear();
	
}



int CCapitalStructure::CheckReport(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData)
{
	string header[] = {
		"realDate",
		"AShares",
		"BShares",
		"changeReason",
		"changeReasonDesc",
		"seq",
		"totalShares",
		"ATotalShares"
	};

	if(arrayHeader.size()!=8)
		return CSV_CHECK_FORMAT_ERROR;

	for(int i=0; i<8; i++)
	{
		if(arrayHeader[i]!=header[i])
			return CSV_CHECK_FORMAT_ERROR;
	}
	return CSV_CHECK_NO_ERROR;
}



void CCapitalStructure::ParseCsvFile(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData)
{
	stringstream ss;
	/*string connectString = "dbname=stock  port=3306   host=127.0.0.1 user=root  ";
	backend_factory const &backEnd = *soci::factory_mysql();*/

	m_vec_real_date.clear();
	m_vec_ashares.clear();
	m_vec_bshares.clear();
	m_vec_change_reason.clear();
	m_vec_change_reason_desc.clear();
	m_vec_seq.clear();
	m_vec_total_shares.clear();
	m_vec_a_total_shares.clear();

	try
	{
		ss << "CCapitalStructure ParseCsvFile......\r\n";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		for (int row = 0; row < arrayData.size(); row++)
		{
			int colnum = arrayData[row].size();
			
			m_vec_real_date[row]=arrayData[row][0];
			m_vec_ashares[row]= 1<colnum ? String2Double(arrayData[row][1]):0;
			m_vec_bshares[row]= 2<colnum ? String2Double(arrayData[row][2]):0;
			m_vec_change_reason[row]= 3<colnum ? String2Double(arrayData[row][3]):0;
			m_vec_change_reason_desc[row]= 4<colnum ? arrayData[row][4]:"-";
			m_vec_seq[row]=5<colnum ? String2Double(arrayData[row][5]):0;
			m_vec_total_shares[row]=6<colnum ? String2Double(arrayData[row][6]):0;
			m_vec_a_total_shares[row]=7<colnum ? String2Double(arrayData[row][7]):0;
		}

		ss.str("");
		ss << "OK, CCapitalStructure ParseCsvFile 完毕.\r\n";
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

void CCapitalStructure::ParseXmlElement(void)
{
}

bool CCapitalStructure::ParseCsvFileName(string szFileName)
{
	vector<string> firstMatch;
	if(Pcre2Grep(_T("CapitalStructure\\(\\d+\\)"), szFileName,firstMatch )<=0)
		return false;

	string::size_type pos1 = firstMatch[0].find(")");
	string codeStr = firstMatch[0].substr(17, pos1-17);
	m_nCode = atoi(codeStr.c_str());
	return true;
}

void CCapitalStructure::ImportToDatabase(void)
{
	if(m_nCode<=0)
		return;

	stringstream ss;

	try
	{
		vector<string>  vecSqlStr;
		CStockDownloadData sdd = ((CStockApp *)AfxGetApp())->m_StockDataLog.SearchStockByCode(m_nCode);
		string maxDate = sdd.CapitalStructure;

		for (int row = 0; row < m_vec_real_date.size(); row++)
		{
			if(m_vec_real_date[row] > sdd.CapitalStructure)
			{
				ss.str("");
				ss << "insert into CapitalStructure "
					<< " select " <<  Int2String(m_nCode) << ", \'" 
					<< m_vec_real_date[row] << "\', "
					<< Double2String(m_vec_ashares[row])  << ", "
					<< Double2String(m_vec_bshares[row]) << ", "
					<< Int2String(m_vec_change_reason[row]) << ", \'"
					<< m_vec_change_reason_desc[row] << "\', "
					<< Int2String(m_vec_seq[row]) << ", "
					<< Double2String(m_vec_total_shares[row]) << ", "
					<< Double2String(m_vec_a_total_shares[row]) << ", now() "

					<< " from dual where not exists (SELECT 1 from CapitalStructure " 
					<< " where real_date=\'" << m_vec_real_date[row] << "\' " 
					<< " and Code=" << Int2String(m_nCode) << " ) ";

				string sqlStr = ss.str();
				vecSqlStr.insert(vecSqlStr.end(), sqlStr);

				ss.str("");
				ss << "update CapitalStructure set download_date = now() "
					<< " where real_date=\'" << m_vec_real_date[row] << "\' " 
					<< " and Code=" << Int2String(m_nCode) /*<< " and download_date is NULL "*/;

				sqlStr = ss.str();
				vecSqlStr.insert(vecSqlStr.end(), sqlStr);

				if(maxDate < m_vec_real_date[row])
					maxDate = m_vec_real_date[row];
			}
			
		}

		if(vecSqlStr.size()>0)
		{
			session sql(g_MysqlPool);

			ss.str("");
			ss << SqlScript();
			sql << ss.str();

			for(int i = 0; i< vecSqlStr.size(); i++)
				sql << vecSqlStr[i];

			((CStockApp *)AfxGetApp())->m_StockDataLog.UpdateStockByCode(m_nCode, "CapitalStructure", maxDate);

			ss.str("");
			ss << "共有数据 " << vecSqlStr.size() ;
			ss << " OK, CCapitalStructure 数据导入完毕. -------------------------- ";
			ss << "【" << m_nCode << "】\r\n";
			sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
		}
		
		//return EXIT_SUCCESS;
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << '\r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}


CStockDataModel * CCapitalStructure::NewCopy()
{
	/*CCapitalStructure * pTrading = new CCapitalStructure();
	pTrading->m_nRowNum = m_nRowNum;
	pTrading->m_vec_open = m_vec_open;
	pTrading->m_vec_high = m_vec_high;
	pTrading->m_vec_low = m_vec_low;
	pTrading->m_vec_close = m_vec_close;
	pTrading->m_vec_volume = m_vec_volume;
	pTrading->m_vec_adj_close = m_vec_adj_close;
	pTrading->m_vec_report_date = m_vec_report_date;*/

	return NULL;
}



string CCapitalStructure::GetDownloadUrl()
{
	string url = "";
	return url;
}


string CCapitalStructure::SqlScript()
{
	stringstream ss;
	ss << "create table if not exists CapitalStructure (Code int ,real_date date, AShares DECIMAL(15,4), BShares DECIMAL(15,4), "
		<< "change_reason int, change_reason_desc varchar(256), seq int, total_shares DECIMAL(15,4), "
		<< "A_total_shares DECIMAL(15,4), download_date date,  PRIMARY KEY ( Code, real_date))";
	return ss.str();
}

string CCapitalStructure::GetCsvFileName()
{
	CString target;
	target.Format("%s/CapitalStructure/CapitalStructure(%06d)_.csv" ,g_strCurrentDir.c_str(), m_nCode);
	return target.GetBuffer();
}

string  CCapitalStructure::SaveAsCsv()
{

	stringstream ss;
	try
	{
		ss.str("");
		ss << "http://query.sse.com.cn/security/stock/queryEquityChangeAndReasonDetails.do?";
		ss << "jsonCallBack=jsonpCallback78750&isPagination=false" ;
		ss << "&companyCode=" << m_nCode << "&_=1446525013415";

		string  strUrlRes=CUrlUtils::PostUrl(ss.str().c_str(),NULL);
		string  strGBK = Utf8_GBK(strUrlRes);

		vector<string> vecMatch;
		if(Pcre2Grep(_T("jsonpCallback[\\d]+\\("), strGBK.c_str() ,vecMatch )>0)
		{
			string::size_type pos = strGBK.find(vecMatch[0]);
			strGBK = strGBK.substr(pos + vecMatch[0].length());
			strGBK = strGBK.substr(0, strGBK.length()-1);

			Json::Value jsonRoot;
			Json::Reader reader(Json::Features::strictMode());
			bool parsingSuccessful = reader.parse(strGBK, jsonRoot);
			if (!parsingSuccessful) {
				ss.str("");
				ss << "股本结构，解析 JSON 错误 ：";
				ss << reader.getFormattedErrorMessages().c_str();
				ss << "\r\n";
				sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
			}
			else
			{
				Json::Value json_result = jsonRoot["result"];
				if(json_result.type()== Json::arrayValue)
				{
					int size = json_result.size();
					if(size>0)
					{
						CStdioFile  targetFile(GetCsvFileName().c_str(), CFile::modeWrite|CFile::shareDenyWrite|CFile::modeCreate|CFile::typeText);
						ss.str("");
						ss << "realDate," << "AShares," << "BShares," << "changeReason," << "changeReasonDesc,"; 
						ss << "seq," << "totalShares," << "ATotalShares" << "\n";
						targetFile.WriteString(ss.str().c_str());

						for(int index = 0; index< size; index++)
						{
							Json::Value  struction = json_result[index];
							string AShares = struction["AShares"].asString();
							string BShares = struction["BShares"].asString();
							string changeReason = struction["changeReason"].asString();
							string changeReasonDesc = struction["changeReasonDesc"].asString();
							string realDate = struction["realDate"].asString();
							string seq = struction["seq"].asString();
							string totalShares = struction["totalShares"].asString();

							ss.str("");
							ss << realDate << "," << AShares << "," << BShares << "," << changeReason << ",";
							ss << changeReasonDesc << "," << seq << "," << totalShares << ",0" << "\n";
							targetFile.WriteString(ss.str().c_str());
						}
						ss.str("");
						ss << "股本结构，获取 JSON 数据成功. \r\n";
						sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
						return GetCsvFileName();
					}
					
				}
				else
				{
					ss.str("");
					ss << "股本结构，获取 JSON 中的 Result 数据失败. \r\n";
					sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
				}

			}
		}
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
	}
	
	return "";
	
}


void CCapitalStructure::ExportFromDatabase(double startTime, double endTime)
{
	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();

	try
	{
		ss << "\n 开始从 MYSQL 获取数据......";
		ss << "【" << m_nCode << "】";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select Code, real_date, AShares, BShares, change_reason, "
			<< " change_reason_desc, seq, total_shares, A_total_shares"
			<< " from CapitalStructure where Code=" << Int2String(m_nCode)
			<< " order by real_date";

		row r;
		//session sql(backEnd, m_ConnectString);
		session sql(g_MysqlPool);

		statement st = (sql.prepare << ss.str(),into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{
			{
				std::tm  tm = r.get<std::tm>("real_date");
				ss.str("");
				ss << (tm.tm_year + 1900) << "-" << (tm.tm_mon + 1) << "-" << tm.tm_mday ;
				m_vec_real_date[cnt] = ss.str();
			}
			m_vec_ashares[cnt] = r.get<double>("AShares");
			m_vec_bshares[cnt] = r.get<double>("BShares");
			m_vec_change_reason[cnt] = r.get<int>("change_reason");
			m_vec_change_reason_desc[cnt] = /*Utf8_GBK*/(r.get<string>("change_reason_desc"));
			m_vec_seq[cnt] = r.get<int>("seq");
			m_vec_total_shares[cnt] = r.get<double>("total_shares");
			m_vec_a_total_shares[cnt] = r.get<double>("A_total_shares");


			cnt++;
		}


		if(cnt>0)
		{
			m_nRowNum = cnt;
		}

		ss.str("");
		ss << "OK, 从 MYSQL 获取数据完毕.";
		ss << " 共 " << cnt << "条 \r\n" ;
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
		//return EXIT_SUCCESS;
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}

double CCapitalStructure::CurrentTotalShare(string curDate)
{
	double nShare = -1;
	if(m_vec_a_total_shares.size()>0)
	{
		if(curDate.empty())
			nShare = m_vec_a_total_shares[m_vec_a_total_shares.size()-1];
		else
		{
			COleDateTime odt_cur ;
			odt_cur.ParseDateTime( curDate.c_str());

			for(int k = m_vec_real_date.size() - 1; k>=0; k--)
			{
				COleDateTime odt_cap ;
				odt_cap.ParseDateTime( m_vec_real_date[k].c_str());
				if(odt_cap.m_dt<= odt_cur.m_dt)
				{
					nShare = m_vec_a_total_shares[k];
					break;
				}
			}
		}
	}
	return nShare;
}


double CCapitalStructure::CurrentACalculateShare(string curDate)
{
	double nShare = -1;
	if(m_vec_ashares.size()>0)
	{
		if(curDate.empty())
			nShare = m_vec_ashares[m_vec_ashares.size()-1];
		else
		{
			COleDateTime odt_cur ;
			odt_cur.ParseDateTime( curDate.c_str());

			for(int k = m_vec_real_date.size() - 1; k>=0; k--)
			{
				COleDateTime odt_cap ;
				odt_cap.ParseDateTime( m_vec_real_date[k].c_str());
				if(odt_cap.m_dt<= odt_cur.m_dt)
				{
					nShare = m_vec_ashares[k];
					break;
				}
			}
		}
	}
	return nShare;
}