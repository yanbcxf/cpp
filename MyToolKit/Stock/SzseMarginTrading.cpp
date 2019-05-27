#include "StdAfx.h"
#include "SzseMarginTrading.h"

#include "UrlUtils.h"

#include "BasicExcel.hpp"
using namespace YExcel;

CSzseMarginTrading::CSzseMarginTrading(void)
{
}

CSzseMarginTrading::~CSzseMarginTrading(void)
{
}



string CSzseMarginTrading::SqlScript()
{
	stringstream ss;
	ss << "create table if not exists SzseMarginTrading ( Code int , TradeDate date, " 
		<< " name varchar(32), financing_buying DECIMAL(15,2),  financing_balance DECIMAL(15,2), "
		<< " stock_loan_selling DECIMAL(15,2), stock_loan_remaider DECIMAL(15,2), "
		<< " stock_loan_balance DECIMAL(15,2), financing_and_stock_loan_balance DECIMAL(15,2), "
		<< " PRIMARY KEY ( Code, TradeDate))";
	return ss.str();
}



bool CSzseMarginTrading::ParseXls(string  xlsfilename)
{

	if(xlsfilename.find("20160104")!=string::npos)
	{
		int kkk = 1;
	}

	BasicExcel e;

	e.Load(xlsfilename.c_str());	
	BasicExcelWorksheet* sheet1 = e.GetWorksheet(L"融资融券交易明细");
	if (sheet1)
	{
		size_t maxRows = sheet1->GetTotalRows();
		size_t maxCols = sheet1->GetTotalCols();

		for (size_t r=0; r<maxRows; ++r)
		{
			for (size_t c=0; c<maxCols; ++c)
			{
				BasicExcelCell* cell = sheet1->Cell(r,c);
				switch (cell->Type())
				{
				case BasicExcelCell::UNDEFINED:
					break;

				case BasicExcelCell::INT:
					//printf("%10d", cell->GetInteger());
					break;

				case BasicExcelCell::DOUBLE:
					//printf("%10.6lf", cell->GetDouble());
					break;

				case BasicExcelCell::STRING:
					if(r>0)
					{
						CString str1(cell->GetString());
						if(c==0)		m_vec_code.insert(m_vec_code.end(), atoi(str1.GetBuffer()));
						else if(c==1)	m_vec_name.insert(m_vec_name.end(), str1.GetBuffer());
						else if(c==2)	m_vec_financing_buying.insert(m_vec_financing_buying.end(), String2Double(str1.GetBuffer()));
						else if(c==3)	m_vec_financing_balance.insert(m_vec_financing_balance.end(),  String2Double(str1.GetBuffer()));
						else if(c==4)	m_vec_stock_loan_selling.insert(m_vec_stock_loan_selling.end(),  String2Double(str1.GetBuffer()));
						else if(c==5)	m_vec_stock_loan_remaider.insert(m_vec_stock_loan_remaider.end(),  String2Double(str1.GetBuffer()));
						else if(c==6)	m_vec_stock_loan_balance.insert(m_vec_stock_loan_balance.end(),  String2Double(str1.GetBuffer()));
						else if(c==7)	m_vec_financing_and_stock_loan_balance.insert(m_vec_financing_and_stock_loan_balance.end(),  String2Double(str1.GetBuffer()));
					}
					break;

				case BasicExcelCell::WSTRING:
					if(r>0)
					{
						CString str1(cell->GetWString());	
						if(c==0)		m_vec_code.insert(m_vec_code.end(), atoi(str1.GetBuffer()));
						else if(c==1)	m_vec_name.insert(m_vec_name.end(), str1.GetBuffer());
						else if(c==2)	m_vec_financing_buying.insert(m_vec_financing_buying.end(), String2Double(str1.GetBuffer()));
						else if(c==3)	m_vec_financing_balance.insert(m_vec_financing_balance.end(),  String2Double(str1.GetBuffer()));
						else if(c==4)	m_vec_stock_loan_selling.insert(m_vec_stock_loan_selling.end(),  String2Double(str1.GetBuffer()));
						else if(c==5)	m_vec_stock_loan_remaider.insert(m_vec_stock_loan_remaider.end(),  String2Double(str1.GetBuffer()));
						else if(c==6)	m_vec_stock_loan_balance.insert(m_vec_stock_loan_balance.end(),  String2Double(str1.GetBuffer()));
						else if(c==7)	m_vec_financing_and_stock_loan_balance.insert(m_vec_financing_and_stock_loan_balance.end(),  String2Double(str1.GetBuffer()));
					}
					break;
				}
			}
		}
	}


	int recordSize = m_vec_code.size();
	if(m_vec_financing_buying.size() == recordSize &&
		m_vec_financing_balance.size()== recordSize &&
		m_vec_stock_loan_selling.size()==recordSize &&
		m_vec_stock_loan_remaider.size()==recordSize &&
		m_vec_stock_loan_balance.size()==recordSize &&
		m_vec_financing_and_stock_loan_balance.size()== recordSize &&
		recordSize > 0)

		return true;
	else
	{
		stringstream ss;
		ss.str("");
		ss << "CSzseMarginTrading（深圳）【"  << Int2String(m_nCode) << "】";
		ss << " ParseXls 文件异常 ，Err" ;
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
		return false;
	}
}

string  CSzseMarginTrading::SaveAsCsv()
{
	stringstream ss;

	m_vec_code.clear();
	m_vec_name.clear();
	m_vec_financing_buying.clear();

	m_vec_financing_balance.clear();
	m_vec_stock_loan_selling.clear();

	m_vec_stock_loan_remaider.clear();
	m_vec_stock_loan_balance.clear();
	m_vec_financing_and_stock_loan_balance.clear();

	//	开始下载
	char tmp[256] = { 0 };
	snprintf (tmp, 255, "%8d", m_nCode);	
	string strEnd = string(tmp);
	strEnd = strEnd.substr(0,4) + "-" + strEnd.substr(4,2) + "-" + strEnd.substr(6.2);

	int xlsLen = 0;
	unsigned char * pXls = NULL;
	/*下载 xls 文件（包含所有股票当日的 融资融券情况）*/
	char tmpCode[256] = {0};
	sprintf(tmpCode, "http://www.szse.cn/szseWeb/ShowReport.szse?SHOWTYPE=xls&CATALOGID=1837_xxpl&tab2PAGENUM=1&ENCODE=1&TABKEY=tab2&txtDate=%s",
		strEnd.c_str());

	string test = string(tmpCode);
	xlsLen =CUrlUtils::PostUrlOfSzse(tmpCode,pXls);

	if(xlsLen>0)
	{
		ss.str("");
		ss << "CSzseMarginTrading（深圳）【"  << Int2String(m_nCode) << "】";
		ss << " xls 文件下载成功. \r\n";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		CString target;
		target.Format("%s/%s/%s.xls",g_strCurrentDir.c_str(), "SzseMarginTrading", strEnd.c_str());
		string filename = target.GetBuffer();

		FILE * fp_xls=fopen(filename.c_str(),"wb");
		if(fp_xls)
		{
			fwrite(pXls,1,xlsLen ,fp_xls);
			fclose(fp_xls);
		}
		//free(pXls);
		delete[] pXls;

		if(ParseXls(filename))
			return "AlreadyInMemory";
	}
	return "";
}


void CSzseMarginTrading::ImportToDatabase(void)
{
	if(m_nCode<=0)
		return;

	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();

	try
	{
		ss << "CSzseMarginTrading（深圳）::ImportToDatabase 开始导入数据.....\r\n";
		//sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//session sql(backEnd, m_ConnectString);
		session sql(g_MysqlPool);
		
		ss.str("");
		ss << SqlScript();
		sql << ss.str();

		ss.str("");
		ss << "insert into SzseMarginTrading ("
			<< "Code, TradeDate, name, financing_buying, financing_balance, " 
			<< "stock_loan_selling, stock_loan_remaider, stock_loan_balance, financing_and_stock_loan_balance ) values ";

		for (int row = 0; row < m_vec_code.size(); row++)
		{
			ss	<< "( " <<  Int2String(m_vec_code[row]) << ", \'" 
				<< Int2String(m_nCode) << "\', \'"
				<< m_vec_name[row] << "\',"
				<< Double2String(m_vec_financing_buying[row]) << ", "
				<< Double2String(m_vec_financing_balance[row]) << ","
				<< Double2String(m_vec_stock_loan_selling[row]) << ","
				<< Double2String(m_vec_stock_loan_remaider[row]) << ","
				<< Double2String(m_vec_stock_loan_balance[row]) << ","
				<< Double2String(m_vec_financing_and_stock_loan_balance[row]) << " ) ";
				
			if(row< m_vec_code.size() - 1)
				ss << ",";
		}

		string test = ss.str();
		//test = Gbk2Utf8(test);
		sql << test; 

		ss.str("");
		ss << "【"  << Int2String(m_nCode) << "】";
		ss << "共有数据 " << m_vec_code.size() ;
		ss << " OK, CSzseMarginTrading（深圳）::ImportToDatabase 数据导入完毕. \r\n";
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


void CSzseMarginTrading::NotifyDatabaseImportedToWindow()
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
