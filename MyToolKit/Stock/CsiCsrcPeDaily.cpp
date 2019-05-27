#include "StdAfx.h"
#include "CsiCsrcPeDaily.h"

#include "UrlUtils.h"
#include "zlib.h"
#include "unzip.h"

#include "BasicExcel.hpp"
using namespace YExcel;

CCsiCsrcPeDaily::CCsiCsrcPeDaily(void)
{
}

CCsiCsrcPeDaily::~CCsiCsrcPeDaily(void)
{
}



string CCsiCsrcPeDaily::SqlScript()
{
	stringstream ss;
	ss << "create table if not exists CsiCsrcPeDaily (Code varchar(32) , TradeDate date, is_stock int, " 
		<< " name varchar(128), company_number int,  parent_code varchar(32), "
		<< " static_pe DECIMAL(10,2), dynamic_pe DECIMAL(10,2), pb DECIMAL(10,2), dividend_yield_ratio DECIMAL(10,2), "
		<< " PRIMARY KEY ( Code, TradeDate, is_stock))";
	return ss.str();
}

bool CCsiCsrcPeDaily::Unzip(string zipfilename, string & xlsfilename)
{
	
	unzFile unzfile = unzOpen(zipfilename.c_str());                    
	if(unzfile == NULL)
	{
		sendToOutput("unzOpen err",m_hWnd, m_nLogType);
		return false;
	}
			
	//获取zip文件的信息
	unz_global_info* pGlobalInfo = new unz_global_info;
	int nReturnValue = unzGetGlobalInfo(unzfile, pGlobalInfo);
	if(nReturnValue != UNZ_OK)
	{
		sendToOutput("unzGetGlobalInfo err",m_hWnd, m_nLogType);
		return false;
	}

	//解析zip文件
	unz_file_info* pFileInfo = new unz_file_info;
	char szZipFName[MAX_PATH];                            //存放从zip中解析出来的内部文件名
	for(int i=0; i<pGlobalInfo->number_entry; i++)
	{
		//解析得到zip中的文件信息
		nReturnValue = unzGetCurrentFileInfo(unzfile, pFileInfo, szZipFName, MAX_PATH, 
					NULL, 0, NULL, 0);
		if(nReturnValue != UNZ_OK)
		{
			sendToOutput("解析zip文件信息失败!",m_hWnd, m_nLogType);
			return false;
		}

		//判断是文件夹还是文件
		switch(pFileInfo->external_fa)
		{
		case FILE_ATTRIBUTE_DIRECTORY:                    //文件夹
			{
				/*CString strDiskPath = strTempPath + _T("//") + szZipFName;
				CreateDirectory(strDiskPath, NULL);*/
			}
		break;
		default:                                        //文件
			{
				//创建文件
				CString strDiskFile;
				strDiskFile.Format("%s/%s/%s",g_strCurrentDir.c_str(), "CsiCsrcPeDaily", szZipFName);
				HANDLE hFile = CreateFile(strDiskFile, GENERIC_WRITE,
								0, NULL, OPEN_ALWAYS, FILE_FLAG_WRITE_THROUGH, NULL);
				if(hFile == INVALID_HANDLE_VALUE)
				{
					sendToOutput("创建文件失败!",m_hWnd, m_nLogType);
					return false;
				}

				//打开文件
				nReturnValue = unzOpenCurrentFile(unzfile);
				if(nReturnValue != UNZ_OK)
				{
					sendToOutput("unzOpenCurrentFile 失败!",m_hWnd, m_nLogType);
					CloseHandle(hFile);
					return false;
				}

				//读取文件
				const int BUFFER_SIZE = 4096;
				char szReadBuffer[BUFFER_SIZE];
				while(TRUE)
				{
					memset(szReadBuffer, 0, BUFFER_SIZE);
					int nReadFileSize = unzReadCurrentFile(unzfile, szReadBuffer, BUFFER_SIZE);
					if(nReadFileSize < 0)                //读取文件失败
					{
						sendToOutput("读取文件失败!",m_hWnd, m_nLogType);
						unzCloseCurrentFile(unzfile);
						CloseHandle(hFile);
						return false;

					}
					else if(nReadFileSize == 0)            //读取文件完毕
					{
						unzCloseCurrentFile(unzfile);
						CloseHandle(hFile);
						xlsfilename = strDiskFile.GetBuffer();
						//	以便解压下一个文件
						break;			
					}
					else                                //写入读取的内容
					{
						DWORD dWrite = 0;
						BOOL bWriteSuccessed = WriteFile(hFile, szReadBuffer, BUFFER_SIZE, &dWrite, NULL);
						if(!bWriteSuccessed)
						{
							sendToOutput("WriteFile 失败!",m_hWnd, m_nLogType);
							unzCloseCurrentFile(unzfile);
							CloseHandle(hFile);
							//	以便解压下一个文件
							break;			
						}			
					}
				}
			}
			break;
		}
		unzGoToNextFile(unzfile);
	}

	//关闭
	if(unzfile)
	{
		unzClose(unzfile);
	}	
	return true;
}



bool CCsiCsrcPeDaily::ParseXls(string  xlsfilename)
{

	if(xlsfilename.find("20160722")!=string::npos)
	{
		int kkk = 1;
	}

	BasicExcel e;

	e.Load(xlsfilename.c_str());	
	BasicExcelWorksheet* sheet1 = e.GetWorksheet(L"证监会行业静态市盈率");
	if (sheet1)
	{
		size_t maxRows = sheet1->GetTotalRows();
		size_t maxCols = sheet1->GetTotalCols();

		string strParent = "";
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
					//printf("%10s", cell->GetString());
					break;

				case BasicExcelCell::WSTRING:
					//wprintf(L"%10s", cell->GetWString());
					CString str1(cell->GetWString());	
					if(r>0)
					{
						if(c==0)	
						{
							string strCode = str1.GetBuffer();
							m_vec_code.insert(m_vec_code.end(), strCode);
							if(strCode.find_first_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ")!=string::npos)
							{
								m_vec_parent_code.insert(m_vec_parent_code.end(), "0");
								strParent = strCode;
							}
							else
							{
								m_vec_parent_code.insert(m_vec_parent_code.end(), strParent);
							}
							
							m_vec_is_stock.insert(m_vec_is_stock.end(), 0);
						}
						else if(c==1)	m_vec_name.insert(m_vec_name.end(), str1.GetBuffer());
						else if(c==2)	m_vec_static_pe.insert(m_vec_static_pe.end(), String2Double(str1.GetBuffer()));
						else if(c==3)	m_vec_company_number.insert(m_vec_company_number.end(), atoi(str1.GetBuffer()));
					}
					break;
				}
			}
		}
	}

	BasicExcelWorksheet* sheet2 = e.GetWorksheet(L"证监会行业滚动市盈率");
	if (sheet2)
	{
		size_t maxRows = sheet2->GetTotalRows();
		size_t maxCols = sheet2->GetTotalCols();

		for (size_t r=0; r<maxRows; ++r)
		{
			for (size_t c=0; c<maxCols; ++c)
			{
				BasicExcelCell* cell = sheet2->Cell(r,c);
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
					//printf("%10s", cell->GetString());
					break;

				case BasicExcelCell::WSTRING:
					//wprintf(L"%10s", cell->GetWString());
					CString str1(cell->GetWString());	
					if(r>0)
					{
						if(c==2)	m_vec_dynamic_pe.insert(m_vec_dynamic_pe.end(), String2Double(str1.GetBuffer()));
					}
					break;
				}
			}
		}
	}

	BasicExcelWorksheet* sheet3 = e.GetWorksheet(L"证监会行业市净率");
	if (sheet3)
	{
		size_t maxRows = sheet3->GetTotalRows();
		size_t maxCols = sheet3->GetTotalCols();

		for (size_t r=0; r<maxRows; ++r)
		{
			for (size_t c=0; c<maxCols; ++c)
			{
				BasicExcelCell* cell = sheet3->Cell(r,c);
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
					//printf("%10s", cell->GetString());
					break;

				case BasicExcelCell::WSTRING:
					//wprintf(L"%10s", cell->GetWString());
					CString str1(cell->GetWString());	
					if(r>0)
					{
						if(c==2)	m_vec_pb.insert(m_vec_pb.end(),String2Double(str1.GetBuffer()));
					}
					break;
				}
			}
		}
	}

	BasicExcelWorksheet* sheet4 = e.GetWorksheet(L"证监会行业股息率");
	if(sheet4==NULL)     sheet4 = e.GetWorksheet("证监会行业股息率");
	if (sheet4)
	{
		size_t maxRows = sheet4->GetTotalRows();
		size_t maxCols = sheet4->GetTotalCols();

		for (size_t r=0; r<maxRows; ++r)
		{
			for (size_t c=0; c<maxCols; ++c)
			{
				BasicExcelCell* cell = sheet4->Cell(r,c);
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
					//printf("%10s", cell->GetString());
					break;

				case BasicExcelCell::WSTRING:
					//wprintf(L"%10s", cell->GetWString());
					CString str1(cell->GetWString());	
					if(r>0)
					{
						if(c==2)	m_vec_dividend_yield_ratio.insert(m_vec_dividend_yield_ratio.end(), String2Double(str1.GetBuffer()));
					}
					break;
				}
			}
		}
	}

	BasicExcelWorksheet* sheet5 = e.GetWorksheet(L"个股数据");
	if (sheet5)
	{
		size_t maxRows = sheet5->GetTotalRows();
		size_t maxCols = sheet5->GetTotalCols();

		for (size_t r=0; r<maxRows; ++r)
		{
			for (size_t c=0; c<maxCols; ++c)
			{
				BasicExcelCell* cell = sheet5->Cell(r,c);
				switch (cell->Type())
				{
				/*case BasicExcelCell::UNDEFINED:
					break;

				case BasicExcelCell::INT:
					break;

				case BasicExcelCell::DOUBLE:
					break;

				case BasicExcelCell::STRING:
					break;*/

				case BasicExcelCell::WSTRING:
					{
						CString str1(cell->GetWString());	
						if(r>0)
						{
							if(c==0)	
							{
								m_vec_code.insert(m_vec_code.end(), str1.GetBuffer());
								m_vec_is_stock.insert(m_vec_is_stock.end(), 1);
								m_vec_company_number.insert(m_vec_company_number.end(), 1);
							}
							else if(c==1)	m_vec_name.insert(m_vec_name.end(), str1.GetBuffer());
							else if(c==4)	
							{
								//	证监会大类代码
								string strParent = str1.GetBuffer();
								m_vec_parent_code.insert(m_vec_parent_code.end(), strParent.c_str());
							}
							else if(c==6)	m_vec_static_pe.insert(m_vec_static_pe.end(), String2Double(str1.GetBuffer()));
							else if(c==7)	m_vec_dynamic_pe.insert(m_vec_dynamic_pe.end(),String2Double(str1.GetBuffer()));
							else if(c==8)	m_vec_pb.insert(m_vec_pb.end(), String2Double(str1.GetBuffer()));
							else if(c==9)	m_vec_dividend_yield_ratio.insert(m_vec_dividend_yield_ratio.end(), String2Double(str1.GetBuffer()));
						}
					}
					break;
				default:
					break;
				}
			}
		}
	}

	int recordSize = m_vec_code.size();
	if(m_vec_dividend_yield_ratio.size() == recordSize &&
		m_vec_dynamic_pe.size()== recordSize &&
		m_vec_company_number.size()==recordSize &&
		m_vec_static_pe.size()==recordSize &&
		m_vec_parent_code.size()==recordSize &&
		m_vec_pb.size()== recordSize &&
		m_vec_name.size()== recordSize && 
		m_vec_is_stock.size() == recordSize && recordSize > 1000)

		return true;
	else
	{
		stringstream ss;
		ss.str("");
		ss << "【"  << Int2String(m_nCode) << "】";
		ss << " ParseXls 文件异常 ，Err" ;
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
		return false;
	}
}

string  CCsiCsrcPeDaily::SaveAsCsv()
{
	stringstream ss;

	m_vec_code.clear();
	m_vec_name.clear();
	m_vec_is_stock.clear();

	m_vec_company_number.clear();
	m_vec_parent_code.clear();

	m_vec_static_pe.clear();
	m_vec_dynamic_pe.clear();
	m_vec_pb.clear();
	m_vec_dividend_yield_ratio.clear();

	//	开始下载
	char tmp[256] = { 0 };
	snprintf (tmp, 255, "%8d", m_nCode);	
	string strEnd = string(tmp);

	int zipLen = 0;
	unsigned char * pZip = NULL;
	/*下载 zip 文件（包含所有股票当日的 市盈率、市净率、股息率等）*/
	char tmpCode[256] = {0};
	sprintf(tmpCode, "http://115.29.204.48/syl/%s.zip", strEnd.c_str());

	string test = string(tmpCode);
	zipLen =CUrlUtils::PostUrlOfCsindex(tmpCode,pZip);

	if(zipLen>0)
	{
		ss.str("");
		ss << "【"  << Int2String(m_nCode) << "】";
		ss << " Zip 文件下载成功. \r\n";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		CString target;
		target.Format("%s/%s/csi%s.zip",g_strCurrentDir.c_str(), "CsiCsrcPeDaily", strEnd.c_str());
		string filename = target.GetBuffer();

		FILE * fp_zip=fopen(filename.c_str(),"wb");
		if(fp_zip)
		{
			fwrite(pZip,1,zipLen ,fp_zip);
			fclose(fp_zip);
		}
		//free(pZip);
		delete[] pZip;

		/*从 zip 文件中解压出 xls 文件*/
		string xlsfilename;
		if(Unzip(filename, xlsfilename))
		{
			ss.str("");
			ss << "【"  << Int2String(m_nCode) << "】";
			ss << " Zip 文件 Extract 成功. \r\n";
			sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

			if(ParseXls(xlsfilename))
				return "AlreadyInMemory";
		}
		
	}

	return "";

}


void CCsiCsrcPeDaily::ImportToDatabase(void)
{
	if(m_nCode<=0)
		return;

	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();

	try
	{
		ss << "CCsiCsrcPeDaily::ImportToDatabase 开始导入数据.....\r\n";
		//sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//session sql(backEnd, m_ConnectString);
		session sql(g_MysqlPool);
		
		ss.str("");
		ss << SqlScript();
		sql << ss.str();

		ss.str("");
		ss << "insert into CsiCsrcPeDaily ("
			<< "Code, TradeDate, is_stock, name, company_number, " 
			<< "parent_code, static_pe, dynamic_pe, pb, dividend_yield_ratio ) values  ";

		for (int row = 0; row < m_vec_code.size(); row++)
		{
			ss	<< "( \'" <<  m_vec_code[row] << "\', \'"
				<< Int2String(m_nCode) << "\',"
				<< Int2String(m_vec_is_stock[row]) << ", \'"
				<< m_vec_name[row] << "\',"
				<< Int2String(m_vec_company_number[row]) << ", \'"
				<< m_vec_parent_code[row] << "\',"
				<< Double2String(m_vec_static_pe[row]) << ","
				<< Double2String(m_vec_dynamic_pe[row]) << ","
				<< Double2String(m_vec_pb[row]) << ","
				<< Double2String(m_vec_dividend_yield_ratio[row]) << " )";

			if(row< m_vec_code.size() - 1)
				ss << ",";
		}

		string test = ss.str();
		//test = Gbk2Utf8(test);
		sql << test; 

		ss.str("");
		ss << "【"  << Int2String(m_nCode) << "】";
		ss << "共有数据 " << m_vec_code.size() ;
		ss << " OK, CCsiCsrcPeDaily::ImportToDatabase 数据导入完毕. \r\n";
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



string CCsiCsrcPeDaily::LastTradeDateFromDatabase()
{
	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();
	string lastTradeDate = "";
	try
	{

		ss << "CCsiCsrcPeDaily::LastTradeDateFromDatabase 开始计算.....\r\n";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << " select DATE_FORMAT(MaxDateForCsiCsrcPeDaily(), '%Y%m%d') as TradeDate  "
			<< " from dual " ;

		string test = ss.str();

		row r;
		//session sql(backEnd, m_ConnectString);
		session sql(g_MysqlPool);

		statement st = (sql.prepare << ss.str(),into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{
			lastTradeDate = r.get<string>("TradeDate");
			break;

			cnt++;
		}

	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << "CCsiCsrcPeDaily::LastTradeDateFromDatabase maxdate = NULL.";
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	return lastTradeDate;
}


void CCsiCsrcPeDaily::NotifyDatabaseImportedToWindow()
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



CStockPlateTree * CCsiCsrcPeDaily::GenerateTree()
{
	CStockPlateTree * pTree = new CStockPlateTree;

	CStockPlateTree * pNode = pTree;
	for(int i=0; i<m_vec_code.size(); i++)
	{
		CStockPlateData treeNode;
		treeNode.code =  m_vec_code[i]; //Int2String(m_vec_code[i],  m_vec_is_stock[i]==1? "%06d":"%08d");
		treeNode.name = m_vec_name[i];
		treeNode.company_number = m_vec_company_number[i];
		treeNode.total_turnover = 0;
		treeNode.total_volume = 0;
		treeNode.is_stock = m_vec_is_stock[i];

		CStockPlateData treeNodeParent;
		treeNodeParent.code = m_vec_parent_code[i];
		treeNodeParent.is_stock = 0;

		if(pNode)
		{
			CStockPlateData spd = *pNode->get();
			if(spd.code != treeNodeParent.code || spd.is_stock!=0)
				pNode = NULL;
		}

		if(pNode==NULL)
			pNode = pTree->SearchSpecialNode(treeNodeParent);
		if(pNode)
		{
			pNode->insert(treeNode);
		}
		else
		{
			pTree->insert(treeNode);
		}

	}
	return pTree;
}



void CCsiCsrcPeDaily::ExportFromDatabase(double startTime, double endTime)
{
	m_vec_code.clear();
	m_vec_name.clear();
	m_vec_is_stock.clear();

	m_vec_company_number.clear();
	m_vec_parent_code.clear();

	m_vec_static_pe.clear();
	m_vec_dynamic_pe.clear();
	m_vec_pb.clear();
	m_vec_dividend_yield_ratio.clear();

	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();

	try
	{
		ss << "CCsiCsrcPeDaily 开始从 MYSQL 获取数据......";
		ss << "【" << m_nCode << "】";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select Code, name, company_number, parent_code, is_stock"
			<< " from CsiCsrcPeDaily " 
			<< " where TradeDate=\'" << Int2String(m_nCode) << "\' "
			<< " order by is_stock, parent_code, Code ";

		row r;
		//session sql(backEnd, m_ConnectString);
		session sql(g_MysqlPool);
		sql << "call add_ind('CsiCsrcPeDaily', 'tradedate', 'idx_tradedate')";

		string test = ss.str();
		statement st = (sql.prepare << ss.str(),into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{
			m_vec_code.insert(m_vec_code.end(), r.get<string>("Code"));
			m_vec_name.insert(m_vec_name.end(), /*Utf8_GBK*/(r.get<string>("name")));
			m_vec_company_number.insert(m_vec_company_number.end(), r.get<int>("company_number"));
			/*m_vec_total_volume[cnt] = r.get<double>("Total_Volume");
			m_vec_total_turnover[cnt] = r.get<double>("Total_Turnover");*/
			m_vec_parent_code.insert(m_vec_parent_code.end(), r.get<string>("parent_code"));
			m_vec_is_stock.insert(m_vec_is_stock.end(), r.get<int>("is_stock"));

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
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}
