#include "StdAfx.h"
#include "Stock.h"
#include "FinanceReport.h"

CFinanceReport::CFinanceReport(void)
{
}

CFinanceReport::~CFinanceReport(void)
{
	while(m_vec_index.size()>0)
	{
		map<int, vector<double> *>::iterator it = m_vec_index.begin();
		m_vec_index.erase(it);
	}
	
	m_initial_report_date = "";
}

void CFinanceReport::ReleaseVector()
{
	m_vec_report_date.clear();
	for(int row=1; row<= m_vec_index.size(); row++)
		m_vec_index[row]->clear();
}


void CFinanceReport::ConvertXlsToCsv(CStdioFile & srcFile, CStdioFile & targetFile)
{
	CString sLine;
	int row = 0;
	int colNum = 0;
	vector<string> vecRowString;
	bool bCheckOK = true;

	while (srcFile.ReadString(sLine))
	{
		vector<string> vecMatch;
		string strRowHeader;
		if(Pcre2Grep(_T("[\\s\\t]+[\\-]?[\\d]+"), sLine.GetBuffer() ,vecMatch )<=0)
		{
			if(Pcre2Grep(_T("[\\s\\t]+[万千百元]+"), sLine.GetBuffer() ,vecMatch )<=0)
			{
				strRowHeader = string(sLine.GetBuffer());
				for(int k=0; k< colNum; k++)	vecMatch.push_back(" ");
			}
			else
			{
				string strLine = string(sLine.GetBuffer());
				string::size_type pos = strLine.find(vecMatch[0]);
				strRowHeader = strLine.substr(0, pos);
			}
		}
		else
		{
			string strLine = string(sLine.GetBuffer());
			string::size_type pos = strLine.find(vecMatch[0]);
			strRowHeader = strLine.substr(0, pos);
		}

		if(row==0)
		{	
			// 根据报表的第一行，计算列的数目
			string strLast = vecMatch[vecMatch.size()-1];
			string::size_type pos= strLast.find("19700101");
			if(pos==string::npos)
				colNum = vecMatch.size();
			else
				colNum = vecMatch.size() - 1;
		}

		// 输出到 csv 文件
		stringstream ss;
		ss << strRowHeader;
		for(int k =0 ;k < colNum; k++ )
		{
			if(k<vecMatch.size())
				ss << "," << vecMatch[k];
			else
			{
				// 当下载的 xls 文件不完整时，会出现检测通不过
				bCheckOK = false;
				break;
			}
		}
		ss << "\r\n";

		if(bCheckOK==false)
			break;

		//	过滤掉 “现金流量表”中的无标头的废弃行
		if(strRowHeader.length() > 0) 
		{
			//targetFile.WriteString(ss.str().c_str());
			vecRowString.push_back(ss.str());
		}
		row++;
	}

	for(int k =0; k<vecRowString.size() && bCheckOK; k++)
	{
		targetFile.WriteString(vecRowString[k].c_str());
	}
	
}


void CFinanceReport::ParseCsvFile(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData)
{
	stringstream ss;
	m_vec_report_date.clear();
	for(int row=1; row<= m_vec_index.size(); row++)
		m_vec_index[row]->clear();

	try
	{
		ss << m_strChineseName << " ParseCsvFile。。。。。。\r\n";
		//sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		/* 从第二列（col = 1）开始,时每个季度的财报  */
		for( int col = 1; col < arrayHeader.size(); col++)
		{
			COleDateTime dt;
			//dt.ParseDateTime(arrayHeader[col]);
			//	过滤掉“日期” 后面的“换行回车”
			vector<string> firstMatch;
			if(Pcre2Grep(_T("\\d+"), arrayHeader[col],firstMatch )<=0)
			{
				ss.str("");
				ss << "Error, " << m_strChineseName << " ParseCsvFile 过滤日期错误 \r\n";
				sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
				return;
			}

			//	arrayData 的 0 行为单位
			m_vec_report_date.insert(m_vec_report_date.end(),firstMatch[0] );
			double unitType = 1.0;
			if(arrayData[0][col].find("百万")!=string::npos)
				unitType = 10000000.0;
			else if(arrayData[0][col].find("万元")!=string::npos)
				unitType = 10000.0;
			else if(arrayData[0][col].find("千元")!=string::npos)
				unitType = 1000.0;

			for(int row = 1; row < arrayData.size(); row++)
			{
				//	根据 第一列的 报表行项名称，匹配行坐标
				string  indexName = arrayData[row][0];
				if(m_row_name_2_index.count(indexName)>0)
				{
					int ind = m_row_name_2_index[indexName];
					m_vec_index[ind]->insert(m_vec_index[ind]->end(), String2Double(arrayData[row][col]) * unitType);
		
				}
				else
				{
					//	如果不存在映射关系，说明目录系统不需要该行数据
				}
			}
		}

		ss.str("");
		ss << m_strChineseName << " ParseCsvFile 完毕 \r\n";
		sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
		//return EXIT_SUCCESS;
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << '\r\n';
		sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
	}
}

bool CFinanceReport::ParseCsvFileName(string szFileName)
{
	vector<string> firstMatch;
	string strPattern = string("\\(\\d+\\)_") + m_strChineseName;
	if(Pcre2Grep(strPattern.c_str(), szFileName,firstMatch )<=0)
		return false;

	string::size_type pos1 = firstMatch[0].find(")_");
	string codeStr = firstMatch[0].substr(1, pos1-1);
	m_nCode = atoi(codeStr.c_str());
	return true;
}

void CFinanceReport::ParseXmlElement(void)
{
}


string  CFinanceReport::SaveAsCsv()
{
	
	try{
		string::size_type pos = GetCsvFileName().find(".csv");
		string xlsFileName = GetCsvFileName().substr(0, pos);
		xlsFileName += ".xls";
		CStdioFile  srcFile(xlsFileName.c_str(), CFile::modeRead|CFile::shareDenyWrite|CFile::typeText);

		if(srcFile.GetLength()>1024)
		{
			CStdioFile  targetFile(GetCsvFileName().c_str(), CFile::modeWrite|CFile::shareDenyWrite|CFile::modeCreate|CFile::typeText);
			ConvertXlsToCsv(srcFile, targetFile);
			return GetCsvFileName();
		}
		else
		{
			stringstream ss;
			ss << "CFinanceReport::SaveAsCsv , 【" << m_nCode << "】"; 
			ss << "文件长度太小, Error!";
			sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
			return "";
		}
		
	}
	catch(CFileException e)
	{
		stringstream ss;

		TCHAR errStr[256];
		e.GetErrorMessage(errStr, 256);
		ss << "CFinanceReport::SaveAsCsv , 【" << m_nCode << "】"; 
		ss << string(errStr);
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
		return "";

	}
	catch(...)
	{
		stringstream ss;
		ss << "CFinanceReport::SaveAsCsv , 【" << m_nCode << "】"; 
		ss << ", Error!";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
		return "";

	}
	
}

string CFinanceReport::GetCsvFileName()
{
	CString target;
	target.Format("%s/%s/CSV(%06d)_%s.csv",g_strCurrentDir.c_str(), m_strEnglishName.c_str(), m_nCode, m_strChineseName.c_str());
	return target.GetBuffer();
}

string CFinanceReport::SqlScript()
{
	stringstream ss;

	ss << " create table if not exists " << m_strEnglishName << " ( "
		<< " code int ,report_date date, ";
	for(int k =1; k<= m_vec_index.size(); k++)
	{
		ss << "colume_" << k <<  " DECIMAL(18,2),";
	}
	ss << " PRIMARY KEY(code, report_date))";		

	return ss.str();
}


void CFinanceReport::ImportToDatabase(void)
{
	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();

	try
	{
		ss << m_strChineseName << " 开始导入数据......\r\n";

		/*ss.str("");
		ss << "delete from " << m_strEnglishName ;
		ss << " where code =" << Int2String(m_nCode);
		sql << ss.str();*/

		vector<string>  vecSqlStr;
		CStockDownloadData sdd = ((CStockApp *)AfxGetApp())->m_StockDataLog.SearchStockByCode(m_nCode);
		string maxDate = "0000-00-00";
		if(m_strEnglishName == "Balance")
			maxDate = sdd.Balance;
		if(m_strEnglishName == "CashFlows")
			maxDate = sdd.CashFlows;
		if(m_strEnglishName == "IncomeStatement")
			maxDate = sdd.IncomeStatement;

		string maxDate1 = maxDate;

		for (int row = 0; row < m_vec_report_date.size(); row++)
		{
			if(m_vec_report_date[row] > maxDate1)
			{
				ss.str("");
				ss << "insert into "<<  m_strEnglishName << " (code, report_date, ";
				for(int k=1; k<= m_vec_index.size(); k++)
				{
					ss << "colume_" << k ;
					if(k< m_vec_index.size())
						ss << ",";
					else 
						ss << ")";
				}
				ss << " select " <<  Int2String(m_nCode) << ", " 
					<< " \'" << m_vec_report_date[row]  << "\' , ";
				for(int k=1; k<= m_vec_index.size(); k++)
				{
					//map<int, double>  tmpMap = *m_vec_index[k];
					vector<double>  tmpMap = *m_vec_index[k];
					//	该财报无此项，则插入 NULL
					if(tmpMap.size()>0)
						ss << Double2String(tmpMap[row]);
					else
						ss << "NULL";
					if( k< m_vec_index.size())
						ss << ", ";
				}
				ss << " from dual where not exists (SELECT 1 from " << m_strEnglishName 
					<< " where code =" << Int2String(m_nCode) << " and report_date = \'" <<  m_vec_report_date[row]  << "\'  ) ";

				string test = ss.str();

				vecSqlStr.insert(vecSqlStr.end(), test);

				if(maxDate < m_vec_report_date[row])
					maxDate = m_vec_report_date[row];
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

			((CStockApp *)AfxGetApp())->m_StockDataLog.UpdateStockByCode(m_nCode, m_strEnglishName, maxDate);

			ss.str("");
			ss << "共有数据 " << vecSqlStr.size() ;
			ss << " OK, " << m_strEnglishName << "::ImportToDatabase 数据导入完毕. \r\n";
			sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
		}

	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << '\r\n';
		sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
	}
}



void CFinanceReport::ExportFromDatabase(double startTime, double endTime)
{
	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();

	try
	{
		m_vec_report_date.clear();
		for(int k=1; k<= m_vec_index.size(); k++)
			m_vec_index[k]->clear();

		ss << "\n 开始从 MYSQL 获取数据......" << m_strEnglishName;
		ss << "【" << m_nCode << "】";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select code, DATE_FORMAT(report_date, '%Y-%m-%d') as report_date, ";
		for(int k=1; k<= m_vec_index.size(); k++)
		{
			ss << "colume_" << Int2String(k);
			if( k< m_vec_index.size())
				ss << ",";
		}
		ss 	<< " from " << m_strEnglishName << " where code=" << Int2String(m_nCode)
			<< " order by report_date";

		string test = ss.str();

		row r;
		//session sql(backEnd, m_ConnectString);
		session sql(g_MysqlPool);

		statement st = (sql.prepare << ss.str(),into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{
			m_vec_report_date.insert(m_vec_report_date.end(), r.get<string>("report_date"));

			for(int k=1; k<= m_vec_index.size(); k++)
			{
				string columeName = string("colume_") + Int2String(k);
				double price = r.get<double>(columeName);
				m_vec_index[k]->insert(m_vec_index[k]->end(), price);
			}
			cnt++;
		}

		if(cnt>0)
		{
			m_nRowNum = cnt;
			vector<double>  test1 = *m_vec_index[5];
			m_initial_report_date = m_vec_report_date[cnt-1];
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

string CFinanceReport::GetFirstReportDate()
{
	if(m_vec_report_date.size()==0)
		return "";
	else
	{
		vector<string>::iterator it = m_vec_report_date.begin();
		return *it;
	}
}

void CFinanceReport::ExportFromDatabaseByTime(string afterDate)
{
	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();

	try
	{
		m_vec_report_date.clear();
		for(int k=1; k<= m_vec_index.size(); k++)
			m_vec_index[k]->clear();

		ss << "\n 开始从 MYSQL 获取数据......" << m_strEnglishName;
		ss << "【" << m_nCode << "】";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select code, report_date, ";
		for(int k=1; k<= m_vec_index.size(); k++)
		{
			ss << "colume_" << Int2String(k);
			if( k< m_vec_index.size())
				ss << ",";
		}
		ss 	<< " from " << m_strEnglishName << " where code=" << Int2String(m_nCode)
			<< " and report_date >= \'" << afterDate << "\' "
			<< " order by report_date";

		string test = ss.str();

		row r;
		//session sql(backEnd, m_ConnectString);
		session sql(g_MysqlPool);

		statement st = (sql.prepare << ss.str(),into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{
			{
				std::tm  tm = r.get<std::tm>("report_date");
				ss.str("");
				ss << (tm.tm_year + 1900) << "-" << (tm.tm_mon + 1) << "-" << tm.tm_mday ;
				//m_vec_report_date[cnt] = ss.str();
				m_vec_report_date.insert(m_vec_report_date.end(), ss.str());
			}

			for(int k=1; k<= m_vec_index.size(); k++)
			{
				string columeName = string("colume_") + Int2String(k);
				double price = r.get<double>(columeName);
				//m_vec_index[k]->insert(make_pair(cnt,price));
				m_vec_index[k]->insert(m_vec_index[k]->end(), price);
			}
			cnt++;
		}


		if(cnt>0)
		{
			m_nRowNum = cnt;
			vector<double>  test1 = *m_vec_index[5];
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