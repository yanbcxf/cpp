#include "StdAfx.h"
#include "ShenzhenIndexDaily.h"
#include "UrlUtils.h"

#include "BasicExcel.hpp"
using namespace YExcel;

CShenzhenIndexDaily::CShenzhenIndexDaily(void)
{
}

CShenzhenIndexDaily::~CShenzhenIndexDaily(void)
{
}

bool CShenzhenIndexDaily::ParseXls(string  xlsfilename)
{
	BasicExcel e;

	e.Load(xlsfilename.c_str());	
	BasicExcelWorksheet* sheet1 = e.GetWorksheet(L"指数行情");
	if (sheet1)
	{
		size_t maxRows = sheet1->GetTotalRows();
		size_t maxCols = sheet1->GetTotalCols();

		for (size_t r=0; r<maxRows; ++r)
		{
			for (size_t c=0; c<maxCols; ++c)
			{
				char tmp[256] = { 0 };
				BasicExcelCell* cell = sheet1->Cell(r,c);
				switch (cell->Type())
				{
				case BasicExcelCell::UNDEFINED:
					break;

				case BasicExcelCell::INT:
					sprintf_s(tmp, 256, "%10d", cell->GetInteger());
					break;

				case BasicExcelCell::DOUBLE:
					sprintf_s(tmp, 256, "%10.6lf", cell->GetDouble());
					break;

				case BasicExcelCell::STRING:
					sprintf_s(tmp, 256, "%10s", cell->GetString());
					if(r>0)
					{
						string strTmp = trimString(string(tmp));
						strTmp = ReplaceString(strTmp, ",", "");
						if(c==0)
							m_vec_tradedate.insert(m_vec_tradedate.end(),strTmp);
						else if(c==1)	
						{
							m_vec_code.insert(m_vec_code.end(), strTmp);
						}
						else if(c==2)	m_vec_name.insert(m_vec_name.end(),strTmp);
						else if(c==3)	m_vec_lclose.insert(m_vec_lclose.end(), String2Double(strTmp.c_str()));
						else if(c==4)	m_vec_close.insert(m_vec_close.end(), String2Double(strTmp.c_str()));
						else if(c==5)	m_vec_change_rate.insert(m_vec_change_rate.end(), String2Double(strTmp.c_str()));
						else if(c==6)	m_vec_turnover.insert(m_vec_turnover.end(), String2Double(strTmp.c_str()));
					}
					break;

				case BasicExcelCell::WSTRING:
					//wprintf(L"%10s", cell->GetWString());
					CString str1(cell->GetWString());	
					if(r>0)
					{
						if(c==0)
						{
							m_vec_tradedate.insert(m_vec_tradedate.end(), str1.GetBuffer());
						}
						else if(c==1)	
						{
							string strCode = str1.GetBuffer();
							m_vec_code.insert(m_vec_code.end(), strCode);
						}
						else if(c==2)	m_vec_name.insert(m_vec_name.end(), str1.GetBuffer());
						else if(c==3)	m_vec_lclose.insert(m_vec_lclose.end(), String2Double(str1.GetBuffer()));
						else if(c==4)	m_vec_close.insert(m_vec_close.end(), String2Double(str1.GetBuffer()));
						else if(c==5)	m_vec_change_rate.insert(m_vec_change_rate.end(), String2Double(str1.GetBuffer()));
						else if(c==6)	m_vec_turnover.insert(m_vec_turnover.end(), String2Double(str1.GetBuffer()));
					}
					break;
				}
			}
		}
	}

	if(m_vec_name.size()>0 &&
		m_vec_name.size() ==m_vec_code.size()&&
		m_vec_name.size() ==m_vec_lclose.size()&&
		m_vec_name.size() ==m_vec_close.size()&&
		m_vec_name.size() ==m_vec_turnover.size()&&
		m_vec_name.size() ==m_vec_change_rate.size())
		return true;

	return false;
}

string  CShenzhenIndexDaily::SaveAsCsv()
{
	stringstream ss;

	int xlsLen = 0;
	unsigned char * pXls = NULL;

	//	开始下载
	char tmp[256] = { 0 };
	snprintf (tmp, 255, "%8d", m_nCode);	
	string strEnd = string(tmp);

	CString target;
	target.Format("%s/%s/shenzhen_hq_%s.xls",g_strCurrentDir.c_str(), "IndexInfo", strEnd.c_str());
	string filename = target.GetBuffer();

	strEnd = strEnd.substr(0, 4) + "-" + strEnd.substr(4, 2) + "-" + strEnd.substr(6, 2); 
	
	char tmpCode[256] = {0};
	sprintf(tmpCode, "http://www.szse.cn/szseWeb/ShowReport.szse?SHOWTYPE=xls&CATALOGID=1826&txtDate=%s&txtEndDate=%s&ENCODE=1&TABKEY=tab1", 
		strEnd.c_str(), strEnd.c_str());

	string test = string(tmpCode);
	xlsLen =CUrlUtils::PostUrlOfCsindex(tmpCode,pXls);
	if(xlsLen>0)
	{
		

		FILE * fp_zip=fopen(filename.c_str(),"wb");
		if(fp_zip)
		{
			fwrite(pXls,1,xlsLen ,fp_zip);
			fclose(fp_zip);
		}
		//free(pZip);
		delete[] pXls;

		bool bOk = false;
		try
		{
			bOk = ParseXls(filename);
		}
		catch (...)
		{
			ss.str("");
			ss << "[" << m_nCode ;
			ss << "], CShenzhenIndexDaily::ParseXls 失败. \r\n";
			sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
		}
		
		//	解析完毕，删除文件

		if(bOk)
		{
			return "AlreadyInMemory";
		}
	}
	
	return "";
}

void CShenzhenIndexDaily::ImportToDatabase()
{
	vector<string>  vecSqlStr;
	stringstream ss;

	for(int i=0; i< m_vec_change_rate.size(); i++)
	{
		ss.str("");
		ss << "(\'" << m_vec_tradedate[i] << "\'";
		ss <<  ",\'" << m_vec_code[i] << "\'";
		ss <<  ",\'" << m_vec_name[i] << "\'";
		ss <<  ", " << Double2String(m_vec_lclose[i], "%.3f");
		ss <<  ", " << Double2String(m_vec_close[i], "%.3f");
		ss <<  ", " << Double2String(m_vec_change_rate[i], "%.3f");
		ss <<  ", " << Double2String(m_vec_turnover[i], "%.2f");

		ss << ")";

		vecSqlStr.insert(vecSqlStr.end(), ss.str());
	}


	//	分组插入 ，每组 1000 个
	int nLoop = vecSqlStr.size()/1000;
	if(nLoop * 1000 < vecSqlStr.size())
		nLoop++;

	for(int k=0; k< nLoop; k++)
	{
		session sql(g_MysqlPool);

		ss.str("");
		ss << "create table if not exists szse_index_daily (code varchar(32), name VARCHAR(32), tradedate date,  "
			<< " lclose DECIMAL(10, 3), close DECIMAL(10, 3), change_rate DECIMAL(6, 3), turnover DECIMAL(20,2), "
			<< " PRIMARY key( code, tradedate )) ";

		sql << ss.str();

		ss.str("");
		ss << "insert into szse_index_daily (tradedate, code,  name, "
			<< " lclose,  close,  change_rate,  turnover ) values  ";

		for(int i = k * 1000; i < (k+1) * 1000 && i <vecSqlStr.size(); i++)
		{
			ss << vecSqlStr[i];
			if( (i+1) != (k+1)*1000 && (i+1) != vecSqlStr.size() )
			{
				ss << ",";
			}
		}

		string test = ss.str();
		sql << ss.str();
	}

	/*if(nLoop >0)
	{
		((CStockApp *)AfxGetApp())->m_StockDataLog.UpdateTable("daily_shenzhen_index", strTradedate);
	}*/

	ss.str("");
	ss << "【" << m_nCode << "】, ";
	ss << "共有数据 " << vecSqlStr.size() ;
	ss << " OK, CShenzhenIndexDaily::ImportToDatabase 数据导入完毕. \r\n";
	sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

}