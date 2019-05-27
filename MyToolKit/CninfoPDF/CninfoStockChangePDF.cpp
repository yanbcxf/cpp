#include "StdAfx.h"
#include "CninfoStockChangePDF.h"

#include "TextExtractor.h"

#include "UrlUtils.h"



CCninfoStockChangePDF::CCninfoStockChangePDF(void)
{
}

CCninfoStockChangePDF::~CCninfoStockChangePDF(void)
{
	
}


void CCninfoStockChangePDF::ImportToDatabase(void)
{
	/*if(m_nCode<=0)
		return;*/

	if(mapCatalog.size()<=0)
		return;

	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();

	try
	{
		ss << "CCninfoStockChangePDF::ImportToDatabase 开始导入数据.....\r\n";
		//sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//session sql(backEnd, m_ConnectString);
		session sql(g_MysqlPool);

		sql << SqlScript();

		ss.str("");
		ss << " insert into CninfoAnnouncePDF (  announcementId, secCode, secName, announcementTime,  "
			<< "announcementTitle, adjunctUrl, isDisplay, catalogNo, catalogName, catalogContent ) values ";

		map<int, CPdfCatalog>::iterator it = mapCatalog.begin();
		for( ; it!= mapCatalog.end(); )
		{
			ss	<< "( \'" << announcementId << "\' ," 
				<< "\'" << secCode << "\' ,"
				<< "\'" << secName << "\' ,"
				<< "\'" << announcementTime << "\' ,"
				<< "\'" << announcementTitle << "\' ,"
				<< "\'" << adjunctUrl << "\' ,"
				<< 0 << ","
				<< it->first << ", \'"
				<< it->second.strName << "\', \'"
				<< ReplaceString(it->second.strContent, "\'", "\"") << "\' )";

			it++;
			if(it != mapCatalog.end())
				ss << ",";
		}

		string test = ss.str();
		//test = Gbk2Utf8(test);
		sql << test;
		
		ss.str("");
		ss << "共有数据 " << mapCatalog.size() ;
		ss << " OK, CCninfoStockChangePDF::ImportToDatabase 数据导入完毕. \r\n";
		sendToOutput(ss.str().c_str(), m_hWnd);
		//return EXIT_SUCCESS;
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << '\r\n';
		sendToOutput(ss.str().c_str(), m_hWnd);
	}
}




string CCninfoStockChangePDF::SqlScript()
{
	stringstream ss;
	ss << "create table if not exists CninfoAnnouncePDF ( " 
		<< "announcementId varchar(32), secCode varchar(16), secName varchar(32), announcementTime date, "
		<< "announcementTitle varchar(128), adjunctUrl varchar(128), catalogNo int, catalogName varchar(128),  "
		<< "isDisplay int, catalogContent varchar(4000),  PRIMARY KEY ( announcementId, catalogNo ))";
	return ss.str();
}

string  CCninfoStockChangePDF::SaveAsCsv()
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
		ss << " PDF 文件下载 成功. \r\n";
		sendToOutput(ss.str().c_str(), m_hWnd);

	}
	else
	{
		ss.str("");
		ss << "【"  << adjunctUrl << "】";
		ss << " PDF 文件下载 失败. \r\n";
		sendToOutput(ss.str().c_str(), m_hWnd);

		return ss.str();
	}
	
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
		sendToOutput(ss.str().c_str(), m_hWnd);

		if(pZip)
			free(pZip);

		return ss.str();
	}

	if(pZip)
		free(pZip);



	
	

	
	if( mapCatalog.size() > 0)
	{
		ss.str("");
		ss << "【"  << adjunctUrl << "】";
		ss << " PDF 共有数据 " << mapCatalog.size()  << ". \r\n";
		sendToOutput(ss.str().c_str(), m_hWnd);
		return "AlreadyInMemory";
	}

	return "NoMoreData";
}







