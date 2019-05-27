#include "StdAfx.h"
#include "CninfoCsv.h"
#include "UrlUtils.h"

#include "zlib.h"
#include "unzip.h"
#include "MyBuffer.h"

#include "stock.h"

CCninfoCsv::CCninfoCsv(void)
{
	
	m_strEnglishName = "";
}



string CCninfoCsv::SqlScript()
{
	stringstream ss;

	ss << " create table if not exists " << m_strEnglishName << " ( ";

	for(int i=0; i< m_vec_yield.size(); i++)
	{
		if(m_vec_yield[i].second=="code")
			ss << m_vec_yield[i].second << " int,";
		else if(m_vec_yield[i].second=="name")
			ss << m_vec_yield[i].second << " varchar(32),";
		else if(m_vec_yield[i].second=="announce_date")
			ss << m_vec_yield[i].second << " varchar(32),";
		else if(m_vec_yield[i].second=="start_date")
			ss << m_vec_yield[i].second << " varchar(32),";
		else if(m_vec_yield[i].second=="report_date")
			ss << m_vec_yield[i].second << " varchar(32),";
		else if(m_vec_yield[i].second=="year")
			ss << m_vec_yield[i].second << " varchar(32),";
		else if(m_vec_yield[i].second=="union_type")
			ss << m_vec_yield[i].second << " varchar(32),";
		else if(m_vec_yield[i].second=="report_source")
			ss << m_vec_yield[i].second << " varchar(32),";
		else if(m_vec_yield[i].second.empty()==false)
			ss <<  m_vec_yield[i].second <<  " DECIMAL(18,2),";
	}
	ss << " PRIMARY KEY(code, report_date))";		

	return ss.str();
}


CCninfoCsv::~CCninfoCsv(void)
{
}

void CCninfoCsv::KaptchaAsJpeg()
{
	unsigned char * pXls = NULL;
	char tmpCode[256] = {0};
	sprintf(tmpCode, "http://www.cninfo.com.cn/cninfo-new/Kaptcha.do?JSESSIONID=%s_%06d", m_strEnglishName.c_str(), m_nCode);

	string test = string(tmpCode);
	int xlsLen =CUrlUtils::PostUrlOfCninfo(tmpCode, pXls);
	if(xlsLen>0)
	{
		unsigned long tick =GetTickCount();
		CString target;
		target.Format("%s/%s/%d.jpeg",g_strCurrentDir.c_str(), "CninfoFinance", tick);
		string filename = target.GetBuffer();

		FILE * fp_zip=fopen(filename.c_str(),"wb");
		if(fp_zip)
		{
			fwrite(pXls,1,xlsLen ,fp_zip);
			fclose(fp_zip);
		}
		//free(pZip);
		delete[] pXls;
	}
}

// Kaptcha 是 java 下的一个开源验证码组件
void CCninfoCsv::Kaptcha()
{
	int xlsLen = 0;
	unsigned char * pXls = NULL;
	
	char tmpCode[256] = {0};
	sprintf(tmpCode, "http://www.cninfo.com.cn/cninfo-new/data/kaptcha");
	string test = string(tmpCode);

	sprintf(tmpCode, "JSESSIONID=%s_%06d&j_code=cdds", m_strEnglishName.c_str(), m_nCode);
	pXls = (unsigned char *)tmpCode;
	xlsLen =CUrlUtils::PostUrlOfCninfo(test, pXls);
}

string  CCninfoCsv::SaveAsCsv()
{
	stringstream ss;

	int xlsLen = 0;
	unsigned char * pXls = NULL;
	string zipurl = GetZipUrl();
	pXls = (unsigned char *)zipurl.c_str();
		
	char tmpCode[256] = {0};
	sprintf(tmpCode, "http://www.cninfo.com.cn/cninfo-new/data/download");

	string test = string(tmpCode);
	xlsLen =CUrlUtils::PostUrlOfCninfo(tmpCode, pXls);
	if(xlsLen>0)
	{
		if(xlsLen==14201)
		{
			//	说明下载的 zip 文件中内容为空
			delete[] pXls;
			return "";
		}

		CStockDownloadData sdd = ((CStockApp *)AfxGetApp())->m_StockDataLog.SearchStockByCode(m_nCode);
		long filesize = 0;
		if(m_strEnglishName=="cninfo_balance")
			filesize = sdd.Balance_Zip;
		else if(m_strEnglishName=="cninfo_incomestatement")
			filesize = sdd.IncomeStatement_Zip;
		else if(m_strEnglishName=="cninfo_cashflows")
			filesize = sdd.CashFlows_Zip;

		if(filesize == xlsLen)
		{	
			delete[] pXls;

			// 说明数据库中已经保存了最新的财务报表， 仅更新如下信息为了避免当日重启后重复从巨潮网下载检查
			COleDateTime odt = COleDateTime::GetCurrentTime();
			string downloadTime = odt.Format("%Y-%m-%d").GetBuffer();

			if(m_strEnglishName.find("cninfo_balance")!=string::npos)
				((CStockApp *)AfxGetApp())->m_StockDataLog.UpdateStockByCode(m_nCode, "Balance", downloadTime);

			if(m_strEnglishName.find("cninfo_incomestatement")!=string::npos)
				((CStockApp *)AfxGetApp())->m_StockDataLog.UpdateStockByCode(m_nCode, "IncomeStatement", downloadTime);

			if(m_strEnglishName.find("cninfo_cashflows")!=string::npos)
				((CStockApp *)AfxGetApp())->m_StockDataLog.UpdateStockByCode(m_nCode, "CashFlows", downloadTime);


			return "NoMoreData";
		}

		string filename = m_strEnglishName + "_" + Int2String(m_nCode, "%06d");
		CString target;
		target.Format("%s/%s/%s.zip",g_strCurrentDir.c_str(), "CninfoFinance", filename.c_str());
		filename = target.GetBuffer();

		FILE * fp_zip=fopen(filename.c_str(),"wb");
		if(fp_zip)
		{
			fwrite(pXls,1,xlsLen ,fp_zip);
			fclose(fp_zip);
		}
		//free(pZip);
		delete[] pXls;

		if(Unzip(filename))
		{
			ss.str("");
			ss << "Zip 有新数据, " << m_strEnglishName << " [" << Int2String(m_nCode, "%06d") << "] \r\n";
			sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
			return "AlreadyInMemory";
		}
	}
	return "";
}

bool CCninfoCsv::Unzip(string zipfilename )
{
	unzFile unzfile = unzOpen(zipfilename.c_str());                    
	if(unzfile == NULL)
	{
		Sleep(120*1000);
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
				MyBuffer buffer;

				//打开文件
				nReturnValue = unzOpenCurrentFile(unzfile);
				if(nReturnValue != UNZ_OK)
				{
					sendToOutput("unzOpenCurrentFile 失败!",m_hWnd, m_nLogType);
					//CloseHandle(hFile);
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
						//CloseHandle(hFile);
						return false;

					}
					else if(nReadFileSize == 0)            //读取文件完毕
					{
						unzCloseCurrentFile(unzfile);
						//CloseHandle(hFile);

						//	以便解压下一个文件
						break;			
					}
					else                                //写入读取的内容
					{
						buffer.InputData((unsigned char *)szReadBuffer, nReadFileSize);
					}
				}

				// 解析 csv 文件
				char * pCsv = (char *)buffer.linearize();
				if(pCsv)
				{
					vector<string>	vecLine;
					splitString(string(pCsv),"\r\n" , vecLine);
					if(vecLine.size()>2)
					{
						//	先解析 标题头
						vector<string>	vecHeader;
						splitString(vecLine[0], "," , vecHeader);

						/*for(int i=0; i<vecHeader.size(); i++)
						{
							stringstream ss;
							ss << "m_vec_yield.insert(m_vec_yield.end(), make_pair(_T(\"" << vecHeader[i]  << "\") , \"\"));";
							LOG4CPLUS_DEBUG(g_logger, ss.str());
						}*/

						//	再解析数据行
						for(int k=1; k<vecLine.size(); k++)
						{
							map<string, string>	mapRow;
							vector<string> vecRow;
							splitString(vecLine[k], "," , vecRow);
							if(vecRow.size()>0 && vecHeader.size()== vecRow.size())
							{
								for(int j=0; j< vecRow.size(); j++)
								{
									mapRow[vecHeader[j]] = ReplaceString(vecRow[j], "\"", "");
								}
								m_vec_row.insert(m_vec_row.end(), mapRow);
							}
						}

					}
					delete[] pCsv;
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

	if(m_vec_row.size()>0)
		return true;
	return false;
}


void CCninfoCsv::ImportToDatabase(void)
{
	stringstream ss;
	try{
		
		vector<string> vecHeader;
		map<string, string>::iterator it = m_map_yield.begin();
		for(; it!=m_map_yield.end(); it++)
		{
			if(it->second.empty()==false)
				vecHeader.push_back(it->first);
		}

		stringstream ss;
		ss << "insert into " << m_strEnglishName << " ( ";
		for(int k=0; k<vecHeader.size(); k++)
		{
			ss << m_map_yield[vecHeader[k]];
			if(k< vecHeader.size()-1)
				ss << ",";
		}
		ss << ") values ";

		for(int k=0; k<m_vec_row.size(); k++)
		{
			map<string, string>  mapRow = m_vec_row[k];
			ss << "(";
			for( int i=0; i< vecHeader.size(); i++)
			{
				ss << "\"" << mapRow[vecHeader[i]] << "\"";
				if(i < vecHeader.size()-1)
					ss << ",";
			}
			ss << ")";
			if(k < m_vec_row.size()-1)
				ss << ",";
		}

		if(m_vec_row.size()>0)
		{
			session sql(g_MysqlPool);

			string test = SqlScript();
			sql << SqlScript();

			test = "delete from " + m_strEnglishName + " where code = " + Int2String(m_nCode, "%06d");
			sql << test;

			test = ss.str();
			sql << test;
		}
		

		if(m_vec_row.size()>0)
		{
			COleDateTime odt = COleDateTime::GetCurrentTime();
			string downloadTime = odt.Format("%Y-%m-%d").GetBuffer();

			string filename = m_strEnglishName + "_" + Int2String(m_nCode, "%06d");
			CString target;
			target.Format("%s/%s/%s.zip",g_strCurrentDir.c_str(), "CninfoFinance", filename.c_str());
			filename = target.GetBuffer();
			string filesize = Int2String(GetDownloadFileSize(filename));

			if(m_strEnglishName.find("cninfo_balance")!=string::npos)
			{
				((CStockApp *)AfxGetApp())->m_StockDataLog.UpdateStockByCode(m_nCode, "Balance", downloadTime);
				((CStockApp *)AfxGetApp())->m_StockDataLog.UpdateStockByCode(m_nCode, "Balance_Zip", filesize);
			}

			if(m_strEnglishName.find("cninfo_incomestatement")!=string::npos)
			{
				((CStockApp *)AfxGetApp())->m_StockDataLog.UpdateStockByCode(m_nCode, "IncomeStatement", downloadTime);
				((CStockApp *)AfxGetApp())->m_StockDataLog.UpdateStockByCode(m_nCode, "IncomeStatement_Zip", filesize);
			}

			if(m_strEnglishName.find("cninfo_cashflows")!=string::npos)
			{
				((CStockApp *)AfxGetApp())->m_StockDataLog.UpdateStockByCode(m_nCode, "CashFlows", downloadTime);
				((CStockApp *)AfxGetApp())->m_StockDataLog.UpdateStockByCode(m_nCode, "CashFlows_Zip", filesize);
			}
		}
		
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << '\r\n';
		sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
	}
	
}


string CCninfoCsv::Yield2Key(string strYield)
{
	map<string, string>::iterator it = m_map_yield.begin();
	for(; it!=m_map_yield.end(); it++)
	{
		if(it->second==strYield)
			return it->first;
	}
	return "-";
}


void CCninfoCsv::ExportFromDatabase(double startTime, double endTime)
{

	m_vec_row.clear();
	stringstream ss;
	try
	{
		vector<string>	vecHeader;
		map<string, string>::iterator it = m_map_yield.begin();
		for(; it!= m_map_yield.end(); it++)
		{
			if(it->second.empty()==false)
			{
				vecHeader.push_back(it->second);
			}
		}

		ss.str("");
		ss << "select ";

		for(int i = 0; i< vecHeader.size(); i++)
		{
			if(i>0)
				ss << ",";
			ss << vecHeader[i];
		}

		ss << " from " << m_strEnglishName ;
		ss << " where code = " << Int2String(m_nCode, "%06d");
		string test = ss.str();

		row r;
		session sql(g_MysqlPool);

		statement st = (sql.prepare << ss.str(),into(r)) ;
		st.execute();

		while (st.fetch())
		{
			map<string, string>	mapRow;
			for(int i=0; i< vecHeader.size(); i++)
			{
				string header = vecHeader[i];
				if(header=="code")
					mapRow[Yield2Key(header)] = Int2String(r.get<int>(header, 0), "%06d");
				else if(header=="name")
					mapRow[Yield2Key(header)] = r.get<string>(header, "");
				else if(header=="report_source")
					mapRow[Yield2Key(header)] = r.get<string>(header, "");
				else if(header=="union_type")
					mapRow[Yield2Key(header)] = r.get<string>(header, "");
				else if(header=="report_date")
					mapRow[Yield2Key(header)] = r.get<string>(header, "");
				else if(header=="start_date")
					mapRow[Yield2Key(header)] = r.get<string>(header, "");
				else if(header=="announce_date")
					mapRow[Yield2Key(header)] = r.get<string>(header, "");
				else if(header=="year")
					mapRow[Yield2Key(header)] = r.get<string>(header, "");
				else 
					mapRow[Yield2Key(header)] = Double2String(r.get<double>(header, 0),  "%.2f");
 			}
			m_vec_row.insert(m_vec_row.end(), mapRow);
		}

	}
	catch (...)
	{
		/*ss.str("");
		ss << e.what() << '\r\n';
		sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);*/
	}
	
}

//////////////////////////////////////////////////////////////////////////

CCninfoIncomeStatement::CCninfoIncomeStatement(void)
{
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("机构ID"), "code")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("机构名称"), "name")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("公告日期"), "announce_date")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("开始日期"), "start_date")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("截止日期"), "report_date")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("报告年度"), "year")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("合并类型"), "union_type")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("报表来源"), "report_source")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("一、营业总收入"), "overall_sales")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("其中：营业收入（元）"), "sales_from_operations")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("二、营业总成本"), "overall_costs")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("其中：营业成本"), "cost_of_operations")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("营业税金及附加"), "sales_tax_and_additions")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("销售费用"), "selling_and_distribution_expenses")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("管理费用"), "general_and_administrative_expenses")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("堪探费用"), "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("财务费用"), "financial_expenses")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("资产减值损失"), "impairment_loss_on_assets")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("加：公允价值变动净收益"), "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("投资收益"), "investment_income")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("其中：对联营企业和合营企业的投资收益"), "investment_income_from_joint_ventures_and_affiliates")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("汇兑收益"), "gain_or_loss_on_foreign_exchange_transactions")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("影响营业利润的其他科目"), "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("三、营业利润"), "profit_from_operations")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("加：补贴收入"), "subsidy_income")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("营业外收入"), "non_operating_profit")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("减：营业外支出"), "non_operating_expenses")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("其中：非流动资产处置损失"), "losses_from_disposal_of_non_current_assets")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("加：影响利润总额的其他科目"), "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("四、利润总额"), "profit_before_tax")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("减：所得税"), "income_tax_expenses")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("加：影响净利润的其他科目"), "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("五、净利润"), "net_profit")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("归属于母公司所有者的净利润"), "net_profit_belonging_to_parent_company")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("少数股东损益"), "minority_interest")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("六、每股收益："), "earnings_per_share")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("（一）基本每股收益"), "basic_earnings_per_share")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("（二）稀释每股收益"), "diluted_earnings_per_share")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("七、其他综合收益"), "other_consolidated_income")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("八、综合收益总额"), "total_consolidated_income")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("其中：归属于母公司"), "consolidated_income_belonging_to_parent_company")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("其中：归属于少数股东"), "consolidated_income_belonging_to_minority_shareholders")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("利息收入"), "interest_income")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("已赚保费"), "insurance_premiums_earned")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("手续费及佣金收入"), "handling_charges_and_commissions_income")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("利息支出"), "interest_expenses")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("手续费及佣金支出"), "handling_charges_and_commissions_expenses")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("退保金"), "refund_of_insurance_premiums")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("赔付支出净额"), "net_payments_for_insurance_claims")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("提取保险合同准备金净额"), "net_provision_for_insurance_contracts")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("保单红利支出"), "commissions_on_insurance_policies")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("分保费用"), "cession_charges")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("其中：非流动资产处置利得"), "")); 

	for(int i=0; i< m_vec_yield.size(); i++)
	{
		m_map_yield.insert(m_vec_yield[i]);
	}

	m_strEnglishName = "cninfo_incomestatement";

}


string CCninfoIncomeStatement::GetZipUrl()
{
	COleDateTime odt = COleDateTime::GetCurrentTime();
	char tmpPost[256] = { 0};
	if(m_nCode>=600000)
		sprintf(tmpPost,  "market=sh&type=lrb&code=%06d&orgid=%s&minYear=2001&maxYear=%d", 
		m_nCode, m_cninfo_orgid.c_str(), odt.GetYear());
	else
		sprintf(tmpPost,  "market=sz&type=lrb&code=%06d&orgid=%s&minYear=2001&maxYear=%d", 
		m_nCode, m_cninfo_orgid.c_str(), odt.GetYear());
	return  string(tmpPost);
}



double CCninfoIncomeStatement::EstimateYearProfits()
{
	double nProfit = -1;
	if(m_vec_row.size()>0)
	{
		COleDateTime odt_in ;
		odt_in.ParseDateTime( m_vec_row[m_vec_row.size()-1][_T("截止日期")].c_str());
		int mon = odt_in.GetMonth();
		int day = odt_in.GetDay();

		nProfit = String2Double(m_vec_row[m_vec_row.size()-1][_T("归属于母公司所有者的净利润")]);  //  m_vec_net_profit
		if(mon==3)
			nProfit = 4 * nProfit;
		else if(mon==6)
			nProfit = 2 * nProfit;
		else if(mon==9)
			nProfit = 4 * nProfit / 3;
	}

	return nProfit;
}

//	某日归属母公司所有者的净利润
double CCninfoIncomeStatement::NetProfitBelongingToParentCompany(string currDate, string & baseDate, bool & isLast)
{
	double nProfit = -1;
	COleDateTime odt;
	int k = m_vec_row.size() - 1;
	odt.ParseDateTime(currDate.c_str());
	for(; k>=0; k--)
	{
		COleDateTime odt_in ;

		odt_in.ParseDateTime( m_vec_row[k][_T("截止日期")].c_str());
		if(odt_in.m_dt <= odt.m_dt)
		{
			nProfit = String2Double(m_vec_row[k][_T("归属于母公司所有者的净利润")]);
			baseDate = m_vec_row[k][_T("截止日期")];
			isLast = k==m_vec_row.size()-1 ? true : false;
			break;
		}
	}

	return nProfit;
}



CCninfoBalance::CCninfoBalance(void)
{

	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("机构ID"), "code")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("机构名称"), "name")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("公告日期"), "announce_date")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("截止日期"), "report_date")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("报告年度"), "year")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("合并类型"), "union_type")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("报表来源"), "report_source")); 

	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("货币资金（元）") , "cash")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("以公允价值计量且其变动计入当期损益的金融资产") , "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("应收票据") , "notes_receivable")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("应收账款") , "accounts_receivable")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("预付款项") , "advances_to_suppliers")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("其他应收款") , "other_receivable")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("应收关联公司款") , "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("应收利息") , "interests_receivable")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("应收股利") , "dividends_receivable")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("存货") , "inventories")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("其中：消耗性生物资产") , "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("一年内到期的非流动资产") , "non_current_assets_maturing_within_one_year")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("其他流动资产") , "other_current_assets")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("流动资产合计") , "total_current_assets")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("可供出售金融资产") , "available_for_sale_financial_assets")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("持有至到期投资") , "held_to_maturity_investments")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("长期应收款") , "long_term_receivables")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("长期股权投资") , "long_term_equity_investments")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("投资性房地产") , "investment_real_estates")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("固定资产") , "fixed_assets_net_value")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("在建工程") , "construction_in_progress")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("工程物资") , "construction_supplies")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("固定资产清理") , "fixed_assets_pending_disposal")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("生产性生物资产") , "bearer_biological_assets")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("油气资产") , "oil_and_natural_gas_assets")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("无形资产") , "intangibel_assets")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("开发支出") , "research_and_development_costs")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("商誉") , "goodwill")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("长期待摊费用") , "long_term_deferred_expenses")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("递延所得税资产") , "deferred_tax_assets")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("其他非流动资产") , "other_non_current_assets")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("非流动资产合计") , "total_non_current_assets")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("资产总计") , "total_assets")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("短期借款") , "short_term_borrowings")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("以公允价值计量且其变动计入当期损益的金融负债") , "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("应付票据") , "notes_payable")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("应付账款") , "accounts_payable")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("预收款项") , "advances_from_customers")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("应付职工薪酬") , "employee_benefits_payable")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("应交税费") , "taxes_and_surcharges_payable")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("应付利息") , "interests_payable")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("应付股利") , "dividends_payable")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("其他应付款") , "other_payables")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("应付关联公司款") , "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("一年内到期的非流动负债") , "non_current_liabilities_maturing_within_one_year")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("其他流动负债") , "other_current_liablities")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("流动负债合计") , "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("长期借款") , "total_current_liabilities")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("应付债券") , "debentures_payable")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("长期应付款") , "long_term_payables")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("专项应付款") , "specific_payable")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("预计负债") , "anticipation_liabilities")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("递延所得税负债") , "deferred_tax_liabilities")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("其他非流动负债") , "other_non_current_liablities")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("非流动负债合计") , "total_non_current_liabilities")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("负债合计") , "total_liability")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("实收资本（或股本）") , "registered_capital")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("资本公积") , "capital_surplus")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("盈余公积") , "surplus_reserve")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("专项储备") , "special_reserve")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("减：库存股") , "treasury_stock")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("一般风险准备") , "provision_for_normal_risks")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("未分配利润") , "undistributed_profits")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("归属于母公司所有者权益") , "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("少数股东权益") , "minority_interest")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("外币报表折算价差") , "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("非正常经营项目收益调整") , "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("所有者权益（或股东权益）合计") , "total_owners_equity")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("负债和所有者（或股东权益）合计") , "total_liabilities_and_owners_equity")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("其他综合收益") , "other_consolidated_income")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("递延收益-非流动负债") , "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("结算备付金") , "provision_of_settlement_fund")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("拆出资金") , "funds_lent")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("发放贷款及垫款-流动资产") , "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("衍生金融资产") , "derivative_financial_assets")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("应收保费") , "insurance_premiums_receivable")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("应收分保账款") , "cession_premiums_receivable")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("应收分保合同准备金") , "provision_of_cession_receivable")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("买入返售金融资产") , "buying_and_selling_financial_assets")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("划分为持有待售的资产") , "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("发放贷款及垫款-非流动资产") , "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("向中央银行借款") , "borrowings_from_central_bank")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("吸收存款及同业存放") , "deposits_from_customers_and_interbank")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("拆入资金") , "deposit_funds")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("衍生金融负债") , "derivative_financial_liabilities")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("卖出回购金融资产款") , "funds_from_sales_of_financial_assets_with_repurchasement")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("应付手续费及佣金") , "handling_charges_and_commissions_payable")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("应付分保账款") , "cession_insurance_premiums_payable")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("保险合同准备金") , "provision_for_insurance_contracts")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("代理买卖证券款") , "funds_received_as_agent_of_stock_exchange")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("代理承销证券款") , "funds_received_as_stock_underwrite")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("划分为持有待售的负债") , "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("预计负债-流动负债") , "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("递延收益-流动负债") , "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("其中：优先股-非流动负债") , "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("永续债-非流动负债") , "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("长期应付职工薪酬") , "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("其他权益工具") , "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("其中：优先股-所有者权益") , "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("永续债-所有者权益") , "")); 
	
	


	for(int i=0; i< m_vec_yield.size(); i++)
	{
		m_map_yield.insert(m_vec_yield[i]);
	}

	m_strEnglishName = "cninfo_balance";
}


string CCninfoBalance::GetZipUrl()
{
	COleDateTime odt = COleDateTime::GetCurrentTime();
	char tmpPost[256] = { 0};
	if(m_nCode>=600000)
		sprintf(tmpPost,  "market=sh&type=fzb&code=%06d&orgid=%s&minYear=2001&maxYear=%d", 
		m_nCode, m_cninfo_orgid.c_str(), odt.GetYear());
	else
		sprintf(tmpPost,  "market=sz&type=fzb&code=%06d&orgid=%s&minYear=2001&maxYear=%d", 
		m_nCode, m_cninfo_orgid.c_str(), odt.GetYear());
	return  string(tmpPost);
}


CCninfoCashFlows::CCninfoCashFlows(void)
{

	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("机构ID"), "code")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("机构名称"), "name")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("公告日期"), "announce_date")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("开始日期"), "start_date")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("截止日期"), "report_date")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("报告年度"), "year")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("合并类型"), "union_type")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("报表来源"), "report_source")); 

	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("销售商品、提供劳务收到的现金（元）") , "cash_received_from_sales_of_goods_or_rending_of_services")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("收到的税费返还") , "refund_of_tax_and_levies")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("收到其他与经营活动有关的现金") , "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("经营活动现金流入小计") , "cash_inflow_from_business_activities")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("购买商品、接受劳务支付的现金") , "cash_from_purchasement_and_receiving_service_payments")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("支付给职工以及为职工支付的现金") , "cash_paid_to_employee_and_for_employee")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("支付的各项税费") , "payments_of_all_types_of_taxes")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("支付其他与经营活动有关的现金") , "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("经营活动现金流出小计") , "sub_total_of_cash_outflows_relating_to_business_activities")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("经营活动产生的现金流量净额") , "net_cash_flow_from_business_activities")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("收回投资收到的现金") , "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("取得投资收益收到的现金") , "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("处置固定资产、无形资产和其他长期资产收回的现金净额") , "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("处置子公司及其他营业单位收到的现金净额") , "net_cash_received_from_disposals_of_subsidiaries_and_other")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("收到其他与投资活动有关的现金") , "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("投资活动现金流入小计") , "sub_total_of_cash_inflows_relating_to_investment_activities")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("购建固定资产、无形资产和其他长期资产支付的现金") , "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("投资支付的现金") , "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("质押贷款净增加额") , "net_increase_of_hypothecated_loan")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("取得子公司及其他营业单位支付的现金净额") , "net_cash_from_subsidiary_corporation_and_other_business")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("支付其他与投资活动有关的现金") , "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("投资活动现金流出小计") , "sub_total_of_cash_outflows_relating_to_investment_activities")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("投资活动产生的现金流量净额") , "net_cash_flows_from_investing_activities")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("吸收投资收到的现金") , "cash_received_from_investors")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("取得借款收到的现金") , "cash_from_borrowings")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("发行债券收到的现金") , "proceeds_from_issuance_of_bonds")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("收到其他与筹资活动有关的现金") , "cash_from_other_relevant_financing_activities")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("筹资活动现金流入小计") , "sub_total_of_cash_inflows")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("偿还债务支付的现金") , "cash_repayments_of_amount_borrowed")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("分配股利、利润或偿付利息支付的现金") , "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("支付其他与筹资活动有关的现金") , "other_cash_payment_relating_to_financing_activities")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("筹资活动现金流出小计") , "sub_total_of_cash_outflows_relating_to_financing_activities")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("筹资活动产生的现金流量净额") , "net_cash_flows_from_financing_activities")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("四、汇率变动对现金的影响") , "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("四(2)、其他原因对现金的影响") , "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("五、现金及现金等价物净增加额") , "net_increase_in_cash_and_cash_equivalents")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("期初现金及现金等价物余额") , "cash_and_cash_equivalents_at_beginning_of_period")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("期末现金及现金等价物余额") , "cash_and_cash_equivalents_at_end_of_period")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("附注：") , "supplemental_information")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("1、将净利润调节为经营活动现金流量：") , "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("净利润") , "net_profit")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("加：资产减值准备") , "provision_for_assets")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("固定资产折旧、油气资产折耗、生产性生物资产折旧") , "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("无形资产摊销") , "amortisation_of_intangible_assets")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("长期待摊费用摊销") , "amortisation_of_long_term_deffered_expenses")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("处置固定资产、无形资产和其他长期资产的损失") , "losses_on_disposal_of_fixed_intangible_and_other_assets")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("固定资产报废损失") , "losses_on_scrapping_of_fixed_assets")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("公允价值变动损失") , "losses_on_fair_value")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("财务费用") , "finance_expense")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("投资损失") , "losses_arsing_from_investment")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("递延所得税资产减少") , "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("递延所得税负债增加") , "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("存货的减少") , "decrease_in_inventories")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("经营性应收项目的减少") , "decrease_in_operating_receivables")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("经营性应付项目的增加") , "increase_in_operating_receivables")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("其他") , "others")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("经营活动产生的现金流量净额2") , "net_cash_flow_from_business_activities_2")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("2、不涉及现金收支的重大投资和筹资活动：") , "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("债务转为资本") , "debts_transfer_to_capital")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("一年内到期的可转换公司债券") , "one_year_due_convertible_bonds")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("融资租入固定资产") , "financing_rent_to_the_fixed_asset")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("3、现金及现金等价物净变动情况：") , "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("现金的期末余额") , "cash_at_the_end_of_period")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("减：现金的期初余额") , "cash_at_the_beginning_of_the_period")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("加：现金等价物的期末余额") , "cash_equivalents_at_the_end_of_the_period")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("减：现金等价物的期初余额") , "cash_equivalents_at_the_beginning_of_the_period")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("加：其他原因对现金的影响2") , "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("现金及现金等价物净增加额2") , "net_increase_in_cash_and_cash_equivalents_2")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("客户存款和同业存放款项净增加额") , "net_increase_of_customers_deposit_and_deposit_of_interbank")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("向中央银行借款净增加额") , "net_increase_borrowings_from_central_bank")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("向其他金融机构拆入资金净增加额") , "net_increase_borrowing_funds_to_other_financing_institution")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("收到原保险合同保费取得的现金") , "cash_from_original_insurance_contract_premium")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("收到再保险业务现金净额") , "net_cash_from_reinsurance_business")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("保户储金及投资款净增加额") , "net_increase_of_insured_deposit_and_investment")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("处置以公允价值计量且其变动计入当期损益的金融资产净增加额") , "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("收取利息、手续费及佣金的现金") , "interest_handling_charges_and_commissions_received")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("拆入资金净增加额") , "net_increase_of_loans")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("回购业务资金净增加额") , "net_increase_of_repurchasing_business_funds")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("客户贷款及垫款净增加额") , "net_increase_of_customer_loans_and_advances")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("存放中央银行和同业款项净增加额") , "net_increase_depositing_in_central_bank_and_other_banks")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("支付原保险合同赔付款项的现金") , "cash_paid_for_original_insurance_contract_compensation_payment")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("支付利息、手续费及佣金的现金") , "interests_handling_charges_and_commissions_paid")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("支付保单红利的现金") , "cash_paid_for_policy_dividend")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("其中：子公司吸收少数股东投资收到的现金") , "cash_from_minority_stockholders_by_subsidiary_corporation")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("其中：子公司支付给少数股东的股利、利润") , "")); 
	m_vec_yield.insert(m_vec_yield.end(), make_pair(_T("投资性房地产的折旧及摊销") , "")); 


	for(int i=0; i< m_vec_yield.size(); i++)
	{
		m_map_yield.insert(m_vec_yield[i]);
	}

	m_strEnglishName = "cninfo_cashflows";
}


string CCninfoCashFlows::GetZipUrl()
{
	COleDateTime odt = COleDateTime::GetCurrentTime();
	char tmpPost[256] = { 0};
	if(m_nCode>=600000)
		sprintf(tmpPost,  "market=sh&type=llb&code=%06d&orgid=%s&minYear=2001&maxYear=%d", 
		m_nCode, m_cninfo_orgid.c_str(), odt.GetYear());
	else
		sprintf(tmpPost,  "market=sz&type=llb&code=%06d&orgid=%s&minYear=2001&maxYear=%d", 
		m_nCode, m_cninfo_orgid.c_str(), odt.GetYear());
	return  string(tmpPost);
}