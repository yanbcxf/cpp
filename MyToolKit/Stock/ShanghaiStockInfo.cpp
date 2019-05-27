#include "StdAfx.h"
#include "ShanghaiStockInfo.h"

#include "UrlUtils.h"



CShanghaiStockInfo::~CShanghaiStockInfo(void)
{
}

string  CShanghaiStockInfo::SaveAsCsv()
{

	//////////////////////////////////////////////////////////////////////////
	//	对于本日已经成功下载的 ，忽略该任务
	if(CheckDownloadFileInfo(GetCsvFileName())>0)
		return "NoMoreData";

	stringstream ss;
	ss.str("");
	ss << "http://query.sse.com.cn/security/stock/downloadStockListFile.do?";
	ss << "csrcCode=&stockCode=&areaName=&stockType=1" ;

	string  strUrlRes;
	try
	{
		strUrlRes = CUrlUtils::PostUrl(ss.str().c_str(),NULL);
	}
	catch(...)
	{
		return "";
	}
		

	vector<string> firstMatch;
	splitString(strUrlRes, "\n", firstMatch);

	CStdioFile targetFile;
	if(firstMatch.size()>0)
	{
		BOOL b = targetFile.Open(GetCsvFileName().c_str(),\
			CFile::modeWrite|CFile::shareDenyWrite|CFile::modeCreate|CFile::typeText);

		if(b==FALSE)
			return "";

		ss.str("");
		ss << "公司代码,公司简称,公司全称,英文名称,注册地址,A股代码,A股简称,A股上市日期,";
		ss << "A股总股本,A股流通股本,B股代码,B股简称,B股上市日期,B股总股本,B股流通股本,";
		ss << "地 区,省 份,城 市,所属行业,公司网址" << "\n";
		targetFile.WriteString(ss.str().c_str());
	}

	for(int i = 1; i<firstMatch.size(); i++)
	{
		string tmp = firstMatch[i];
		trimString(tmp);
		if(tmp.empty()==false)
		{
			ss.str("");
			string::size_type pos;
			int	 noField = 0;
			while((pos = tmp.find_first_of(" \t\r\n"))!=string::npos)
			{
				string field = tmp.substr(0, pos);
				ss << field << "," ;
				if(noField==1)
					ss << ", , , ";
				tmp = tmp.substr(pos);
				trimString(tmp);
				noField++;
			}
			ss << tmp << ", , , , , , , , , ,---\n";
			string sLine = ss.str();
			targetFile.WriteString(ss.str().c_str());
		}
	}
	
	if(firstMatch.size()>0)
	{
		targetFile.Close();
		return GetCsvFileName();
	}
	
	return "";
}

string  CShanghaiStockInfo::SaveAsCsvOld()
{
	
	vector<StockListInfo> vecStock;
	SearchStockByAreaName(vecStock);

	stringstream ss;

	if(vecStock.size()>0)
	{
		CStdioFile  targetFile(GetCsvFileName().c_str(), CFile::modeWrite|CFile::shareDenyWrite|CFile::modeCreate|CFile::typeText);
		ss.str("");
		ss << "公司代码,公司简称,公司全称,英文名称,注册地址,A股代码,A股简称,A股上市日期,";
		ss << "A股总股本,A股流通股本,B股代码,B股简称,B股上市日期,B股总股本,B股流通股本,";
		ss << "地 区,省 份,城 市,所属行业,公司网址" << "\r\n";
		targetFile.WriteString(ss.str().c_str());

		for(int index=0; index<vecStock.size(); index++)
		{
			try
			{
				ss.str("");
				ss << "http://query.sse.com.cn/security/stock/commonQuery.do?";
				ss << "jsonCallBack=jsonpCallback78750&isPagination=false" ;
				ss << "&sqlId=COMMON_SSE_ZQPZ_GP_GPLB_C&productid=" << vecStock[index].companycode;


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
						ss << "ShanghaiStock，解析 JSON 错误 ：";
						ss << reader.getFormattedErrorMessages().c_str();
						ss << " companycode : " << vecStock[index].companycode;
						ss << " companyname : " << vecStock[index].companyname;
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


								for(int index = 0; index< size; index++)
								{
									Json::Value  struction = json_result[index];
									string company_code = struction["COMPANY_CODE"].asString();
									string fullname = struction["FULLNAME"].asString();
									string full_name_in_english = struction["FULL_NAME_IN_ENGLISH"].asString();
									string company_address = struction["COMPANY_ADDRESS"].asString();
									string security_code_a = struction["SECURITY_CODE_A"].asString();
									string security_abbr_a = struction["SECURITY_ABBR_A"].asString();
									string security_code_b = struction["SECURITY_CODE_B"].asString();
									string security_abbr_b = struction["SECURITY_ABBR_B"].asString();
									string area_name_desc = struction["AREA_NAME_DESC"].asString();
									string csrc_great_code_desc = struction["CSRC_GREAT_CODE_DESC"].asString();
									string www_address = struction["WWW_ADDRESS"].asString();


									ss.str("");
									ss << company_code << "," ;
									ss << fullname << "," ;
									ss << fullname << "," ;
									ss << "\"" << full_name_in_english << "\",";
									ss << company_address << ",";  
									ss << security_code_a << "," ;
									ss << security_abbr_a << "," ;
									ss << "1970/01/01" << "," << "0" << "," << "0" << "," ;
									ss << security_code_b << "," ;
									ss << security_abbr_b << "," ;
									ss << "1970/01/01" << "," << "0" << "," << "0" << "," ;
									ss << area_name_desc << "," ;
									ss << area_name_desc << "," ;
									ss << area_name_desc << "," ;
									ss << csrc_great_code_desc << "," ;
									ss << www_address <<  "\n";
									targetFile.WriteString(ss.str().c_str());
								}
							}
							ss.str("");
							ss << "ShanghaiStock，获取 JSON 数据成功.";
							ss << " companycode : " << vecStock[index].companycode;
							ss << " companyname : " << vecStock[index].companyname;
							ss << "\r\n";
							sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
							
						}
						else
						{
							ss.str("");
							ss << "ShanghaiStock，获取 JSON 中的 Result 数据失败.";
							ss << " companycode : " << vecStock[index].companycode;
							ss << " companyname : " << vecStock[index].companyname;
							ss << "\r\n";
							sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
						}

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
		return GetCsvFileName();
	}
	else
		return "";
}


void   CShanghaiStockInfo::SearchStockByAreaName(vector<StockListInfo> & vecStock)
{

	stringstream ss;
	try
	{
		for(int areaname=1; areaname<=35; areaname++)	// 页面上仅仅支持到 31
		{
			ss.str("");
			ss << "http://query.sse.com.cn/security/stock/commonQuery.do?";
			ss << "jsonCallBack=jsonpCallback78750&isPagination=false" ;
			ss << "&sqlId=COMMON_SSE_ZQPZ_GPLB_DQJS_L&areaname=" << areaname;

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
					ss << "CShanghaiStockInfo::SearchStockByAreaName，解析 JSON 错误 ：areaname = " << areaname;
					ss << reader.getFormattedErrorMessages().c_str();
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
							for(int index = 0; index< size; index++)
							{
								Json::Value  struction = json_result[index];
								StockListInfo info;

								info.areadesc = struction["AREADESC"].asString();
								info.companycode = struction["COMPANYCODE"].asString();
								info.companyname = struction["COMPANYNAME"].asString();
								info.securitycodea = struction["SECURITYCODEA"].asString();
								info.securitycodeb = struction["SECURITYCODEB"].asString();
								info.ssecode = struction["SSECODE"].asString();
								info.ssename = struction["SSENAME"].asString();
								vecStock.push_back(info);

							}
						}
						ss.str("");
						ss << "CShanghaiStockInfo::SearchStockByAreaName，获取 JSON 数据成功. areaname = " << areaname;
						sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
					}
					else
					{
						ss.str("");
						ss << "CShanghaiStockInfo::SearchStockByAreaName，获取 JSON 中的 Result 数据失败. areaname = " << areaname;
						sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
					}

				}
			}
		}
		
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << '\n';
		sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
	}
}


string CShanghaiStockInfo::GetDownloadUrl()
{
	return "";
}


bool CShanghaiStockInfo::ParseCsvFileName(string szFileName)
{
	vector<string> firstMatch;
	if(Pcre2Grep(_T("ShanghaiStock"), szFileName,firstMatch )<=0)
		return false;

	return true;
}

string CShanghaiStockInfo::GetCsvFileName()
{
	CString target;
	target.Format("%s/StockInfo/ShanghaiStock.csv" ,g_strCurrentDir.c_str(), m_nCode);
	return target.GetBuffer();
}

CStockDataModel * CShanghaiStockInfo::NewCopy()
{
	CShanghaiStockInfo * pTrading = new CShanghaiStockInfo();
	pTrading->m_nRowNum = m_nRowNum;


	return pTrading;
}
