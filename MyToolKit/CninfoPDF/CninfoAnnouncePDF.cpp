#include "StdAfx.h"
#include "CninfoAnnouncePDF.h"





CCninfoAnnouncePDF::CCninfoAnnouncePDF(void)
{
}

CCninfoAnnouncePDF::~CCninfoAnnouncePDF(void)
{
	
}


void CCninfoAnnouncePDF::SaveAnalysisToDatabase(vector<CLineOrCell>  pdf)
{
	if(pdf.size()<=0)
		return;

	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();

	try
	{

		session sql(g_MysqlPool);

		ss.str("");
		ss << "create table if not exists CPDFAnalysis ( " 
			<< "announcementId varchar(32),  code int, content varchar(1000), parent_code int, "
			<< "announcementTitle varchar(128), adjunctUrl varchar(128),  "
			<< " PRIMARY KEY ( announcementId, code ))";
		sql << ss.str();

		ss.str("");
		ss << " SELECT count(*)  from CPDFAnalysis " 
			<< " where announcementId=\'" << announcementId << "\' "; 

		long long t;
		sql << ss.str(), into(t);
		if(t>0)
			return ;

		ss.str("");
		ss << "CCninfoAnnouncePDF::SaveAnalysisToDatabase 开始导入数据.....\r\n";
		//sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//session sql(backEnd, m_ConnectString);

		ss.str("");
		ss << " insert into CPDFAnalysis (  announcementId, code, content,  parent_code,  "
			<< "announcementTitle, adjunctUrl ) values ";

		for(int i=0; i<pdf.size(); i++ )
		{
			ss	<< "( \'" << announcementId << "\' ," 
				<< "\'" << i << "\' ,"
				<< "\'" << pdf[i].content << "\' ,"
				<< "\'" << pdf[i].parent << "\' ,"
				<< "\'" << announcementTitle << "\' ,"
				<< "\'" << adjunctUrl << "\'  )";

			if(i< pdf.size() -1)
				ss << ",";
		}

		string test = ss.str();
		//test = Gbk2Utf8(test);
		sql << test;

		ss.str("");
		ss << "共有数据 " << pdf.size() ;
		ss << " OK, CCninfoAnnouncePDF::SaveAnalysisToDatabase 数据导入完毕. \r\n";
		sendToOutput(ss.str().c_str(), NULL);
		//return EXIT_SUCCESS;
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << '\r\n';
		sendToOutput(ss.str().c_str(), NULL);
	}
}



