#include "StdAfx.h"
#include "CninfoAnnouncePDF.h"

#include "TextExtractor.h"

#include "UrlUtils.h"



CCninfoAnnouncePDF::CCninfoAnnouncePDF(void)
{
}

CCninfoAnnouncePDF::~CCninfoAnnouncePDF(void)
{
	
}



int CCninfoAnnouncePDF::CheckReport(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData)
{
	return CSV_CHECK_NO_ERROR;
}



void CCninfoAnnouncePDF::ParseCsvFile(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData)
{
	
}

void CCninfoAnnouncePDF::ParseXmlElement(void)
{
}

bool CCninfoAnnouncePDF::ParseCsvFileName(string szFileName)
{
	vector<string> firstMatch;
	if(Pcre2Grep(_T("SouhuTradingDaily\\(\\d+\\)"), szFileName,firstMatch )<=0)
		return false;

	string::size_type pos1 = firstMatch[0].find(")");
	string codeStr = firstMatch[0].substr(18, pos1-18);
	m_nCode = atoi(codeStr.c_str());
	return true;
}

void CCninfoAnnouncePDF::ImportToDatabase(void)
{
	/*if(m_nCode<=0)
		return;*/

	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();

	try
	{
		ss << "CCninfoAnnouncePDF::ImportToDatabase 开始导入数据.....\r\n";
		//sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//session sql(backEnd, m_ConnectString);
		session sql(g_MysqlPool);

		sql << SqlScript();

		ss.str("");
		ss << " insert into CninfoAnnouncePDF (  announcementId, catalogNo, "
			<< "catalogName, catalogContent ) values ";

		map<int, CPdfCatalog>::iterator it = mapCatalog.begin();
		for( ; it!= mapCatalog.end(); )
		{
			ss	<< "( \'" << announcementId << "\' , " 
				<< it->first << ", \'"
				<< it->second.strName << "\', \'"
				<< it->second.strContent << "\' )";

			it++;
			if(it != mapCatalog.end())
				ss << ",";
		}

		string test = ss.str();
		test = Gbk2Utf8(test);
		sql << test;
		
		ss.str("");
		ss << "共有数据 " << mapCatalog.size() ;
		ss << " OK, CCninfoAnnouncePDF::ImportToDatabase 数据导入完毕. \r\n";
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

CStockDataModel * CCninfoAnnouncePDF::NewCopy()
{
	return NULL;
}



string CCninfoAnnouncePDF::GetDownloadUrl()
{
	return "";
}


string CCninfoAnnouncePDF::SqlScript()
{
	stringstream ss;
	ss << "create table if not exists CninfoAnnouncePDF ( " 
		<< "announcementId varchar(32), catalogNo int, catalogName varchar(128), "
		<< "catalogContent varchar(4000), PRIMARY KEY ( announcementId, catalogNo ))";
	return ss.str();
}



string  CCninfoAnnouncePDF::SaveAsCsv()
{
	stringstream ss;

	int zipLen = 0;
	unsigned char * pZip = NULL;
	/* 下载 PDF 文件 */
	
	/*2016-06-01/1202348087.PDF*/

	string test = string("http://www.cninfo.com.cn/") + adjunctUrl;
	zipLen =CUrlUtils::PostUrlOfCsindex(test, pZip);

	if(zipLen>0)
	{
		ss.str("");
		ss << "【"  << adjunctUrl << "】";
		ss << " PDF 文件下载成功. \r\n";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

	}
	else
		return "";
	
	//////////////////////////////////////////////////////////////////////////

	TextExtractor extractor;

	try {
		extractor.Init( (char *)pZip , zipLen );

	} 
	catch( PdfError & e ) {

		int tt = e.GetError();

		ss.str("");
		ss << "【"  << adjunctUrl << "】";
		ss << " PDF TextExtractor Err. \r\n";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		return "";
	}

	//	截取前 20 页面，以便搜寻“目录”
	vector<string>		vecMatch;
	int  nCatalogPage = -1;
	for(int i=0; i<20; i++)
	{
		if(extractor.m_mapPageContent.count(i)>0 && 
			Pcre2Grep(_T("目[\\s\\t]*录"), extractor.m_mapPageContent[i].c_str() ,vecMatch )>0)
		{
			nCatalogPage = i;
			break;
		}
	}
	if(nCatalogPage < 0)
	{
		//	没有 目录的 pdf 文件暂不做解析
		ss.str("");
		ss << "【"  << announcementTitle << "】";
		ss << " PDF 没有 目录. \r\n";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		return "NoMoreData";
	}

	string strQqjl = "";
	for(int i=nCatalogPage; i<20; i++)
	{
		if(extractor.m_mapPageContent.count(i)>0 )
		{
			strQqjl += extractor.m_mapPageContent[i];
		}
		else 
			break;
	}

	if(Pcre2Grep(_T("目[\\s\\t]*录"), strQqjl.c_str() ,vecMatch )>0)
	{
		string::size_type pos1 = strQqjl.find(vecMatch[0]);
		if(pos1!=string::npos)
			strQqjl = strQqjl.substr(pos1 + vecMatch[0].length());
	}

	
	if(Pcre2Grep(_T("[^\\-\\d\\t\\.]+[\\.]{3,}[\\-\\s\\t]*[\\d]+"), strQqjl.c_str() ,vecMatch )>0)
	{
		int k = vecMatch.size()-1;
		string::size_type startPos = strQqjl.find(vecMatch[k]);
		startPos += vecMatch[k].length();

		strQqjl = strQqjl.substr(startPos);

		//	目录字符过滤
		for(int k = 0; k< vecMatch.size(); k++)
		{
			vector<string> firstMatch;
			if(Pcre2Split(_T("[\\.]+[\\-\\s\\t]*"), vecMatch[k].c_str(), firstMatch )>0)
			{
				mapCatalog[k].strName = firstMatch[0];
				mapCatalog[k].nPage = atoi(firstMatch[1].c_str());
			}

		}

		//	调整目录页号，有的文档封面页编号为 1， 有的编号为 0, 有的编号为 - 1 等 
		if(mapCatalog.size()>0)
		{
			int nTry = 5;
			do
			{
				nTry--;

				if(extractor.m_mapPageContent.count(mapCatalog[0].nPage)<=0)
					continue;
				string  strPage0 = extractor.m_mapPageContent[mapCatalog[0].nPage];
				strPage0 = ReplaceString(strPage0, "　", "");
				strPage0 = ReplaceString(strPage0, " ", "");
				string  strKey = ReplaceString(mapCatalog[0].strName, " ", "");

				// 第一条目录指向的主体内容 必定在 目录页后面
				if(strPage0.find(strKey)!=string::npos && mapCatalog[0].nPage != nCatalogPage)
					break;

				map<int, CPdfCatalog>::iterator it = mapCatalog.begin();
				for(; it!=mapCatalog.end(); it++)
				{
					it->second.nPage -= 1;
				}
				
			}while(nTry>=0);

			if(nTry<0)
			{
				//	未能正确调整 页面号
				mapCatalog.clear();

				ss.str("");
				ss << "【"  << adjunctUrl << "】";
				ss << " PDF 未能正确调整 页面号 Err. \r\n";
				sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
			}
		}

		//	提取每个目录的内容
		for(int k=0; k< mapCatalog.size(); k++)
		{
			if(mapCatalog.count(k)>0)
			{
				//	确定本目录条的 起始页 与 最后页
				int nPage = mapCatalog[k].nPage;
				string strName = mapCatalog[k].strName;

				int nNextPage = extractor.m_mapPageContent.size() -1;
				string strNextName = "";
				if(mapCatalog.count(k + 1)>0)
				{
					nNextPage = mapCatalog[k + 1].nPage;
					strNextName = mapCatalog[k + 1].strName;
				}

				if(nPage== nNextPage)
				{
					string startContent =  extractor.m_mapPageContent[nPage];
					string::size_type pos1 = startContent.find(strName);

					if(pos1!= string::npos)
						startContent = startContent.substr(pos1);

					if(strNextName.empty()!=true)
					{
						pos1 = startContent.find(strNextName);
						if(pos1!= string::npos)
							startContent = startContent.substr(0, pos1);
					}

					mapCatalog[k].strContent = startContent;
				}
				else if(nPage < nNextPage)
				{
					//	第一页
					string startContent =  extractor.m_mapPageContent[nPage];
					string::size_type pos1 = startContent.find(strName);

					if(pos1!= string::npos)
						startContent = startContent.substr(pos1);

					mapCatalog[k].strContent = startContent;

					//	中间页
					int nCurrPage = nPage + 1;
					while(nCurrPage < nNextPage)
					{
						mapCatalog[k].strContent += extractor.m_mapPageContent[nCurrPage];
						nCurrPage++;
					}

					//	最后页
					startContent = extractor.m_mapPageContent[nNextPage];
					if(strNextName.empty()!=true)
					{
						pos1 = startContent.find(strNextName);
						if(pos1!= string::npos)
							startContent = startContent.substr(0, pos1);
					}

					mapCatalog[k].strContent += startContent;
				}

			}
		}
	}
	
	if( mapCatalog.size() > 0)
	{
		ss.str("");
		ss << "【"  << adjunctUrl << "】";
		ss << " PDF 共有数据 " << mapCatalog.size()  << ". \r\n";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
		return "AlreadyInMemory";
	}

	return "NoMoreData";
}

string CCninfoAnnouncePDF::GetCsvFileName()
{
	return "";
}


void CCninfoAnnouncePDF::ExportFromDatabase(double startTime ,double endTime)
{
	
}


string CCninfoAnnouncePDF::LastTradeDateFromDatabase()
{
	return "";
}

void CCninfoAnnouncePDF::NotifyDatabaseImportedToWindow()
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
