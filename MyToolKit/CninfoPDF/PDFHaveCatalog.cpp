#include "StdAfx.h"
#include "PDFHaveCatalog.h"

#include "UrlUtils.h"
#include "TextExtractor.h"

CPDFHaveCatalog::CPDFHaveCatalog(void)
{
}

CPDFHaveCatalog::~CPDFHaveCatalog(void)
{
	mapCatalog.clear();
}

list<CCninfoAnnouncePDF *> CPDFHaveCatalog::CreatePDF()
{
	list<string>	announcementId;
	list<string>	adjunctUrl;
	list<string>	announcementTitle;

	list<string>	announcementTime;
	list<string>	secCode;
	list<string>	secName;

	stringstream ss;
	try
	{
		
		session sql(g_MysqlPool);

		//////////////////////////////////////////////////////////////////////////

		sql << SqlScript();

		ss.str("");
		ss << "select announcementId, secCode, secName, adjunctUrl, announcementTitle, " 
			"DATE_FORMAT(announcementTime, '%Y-%m-%d') as announcementTime from cninfoannouncement b "
			"where announcementTime > '2014-1-1' and adjunctType = 'PDF'  " 
			"and ( announcementTitle like '%股票激励%'  or announcementTitle like '%发行股票%' "
			"or announcementTitle like '%发行A股股票%' ) "
			"and ( announcementTitle like '%草案）'  or announcementTitle like '%草案'  "
			"or announcementTitle like '%预案）' or announcementTitle like '%预案'  "
			"or announcementTitle like '%修正案）' or announcementTitle like '%修订）' " 
			"or announcementTitle like '%修订稿' or announcementTitle like '%修订稿）' )  "
			"and not exists (select 1 from cninfoannouncepdf a where a.announcementId = b.announcementId ) "
			"order by announcementTime desc ";

		string tttt = ss.str();

		row r;
		statement st = (sql.prepare << /*Gbk2Utf8*/ ss.str(), into(r)) ;
		st.execute();

		while (st.fetch())
		{
			announcementId.push_back( r.get<string>("announcementId"));			
			adjunctUrl.push_back(  r.get<string>("adjunctUrl"));
			announcementTitle.push_back( r.get<string>("announcementTitle"));

			announcementTime.push_back( r.get<string>("announcementTime"));
			secCode.push_back( r.get<string>("secCode"));
			secName.push_back( r.get<string>("secName"));
		}

		//////////////////////////////////////////////////////////////////////////
		//	下载股权变动

	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), NULL);
	}

	list<CCninfoAnnouncePDF *>  lst;

	int nPDF = 1;
	while(adjunctUrl.size()>0)
	{
		ss.str("");
		ss << "【" << nPDF << "】PDF" << "(" <<  *announcementTitle.begin() << ")下载开始.\r\n";
		sendToOutput(ss.str().c_str(), NULL );

		CCninfoAnnouncePDF *  pdf = new CPDFHaveCatalog;
		pdf->announcementId = *announcementId.begin();
		pdf->adjunctUrl = *adjunctUrl.begin();
		pdf->announcementTitle = *announcementTitle.begin();

		pdf->announcementTime = *announcementTime.begin();
		pdf->secCode = *secCode.begin();
		pdf->secName = *secName.begin();
		pdf->m_hWnd = NULL;

		lst.push_back(pdf);

		announcementId.pop_front();
		adjunctUrl.pop_front();
		announcementTitle.pop_front();
		announcementTime.pop_front();
		secCode.pop_front();
		secName.pop_front();

		nPDF++;
	}

	return lst;
}


void CPDFHaveCatalog::ImportToDatabase(string csv)
{

	if(csv=="AlreadyInMemory")
	{
				
	}
	else if(csv.empty()==false)
	{
		CPdfCatalog  pdfCatalog;
		pdfCatalog.nPage = 0;
		pdfCatalog.strName =  "Error! " + csv;
		pdfCatalog.strContent = "Error! " + csv;
		mapCatalog[0] = pdfCatalog;
				
	}
	/*if(m_nCode<=0)
		return;*/

	if(mapCatalog.size()<=0)
		return;

	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();

	try
	{
		ss << "CPDFHaveCatalog::ImportToDatabase 开始导入数据.....\r\n";
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
		ss << " OK, CPDFHaveCatalog::ImportToDatabase 数据导入完毕. \r\n";
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




string CPDFHaveCatalog::SqlScript()
{
	stringstream ss;
	ss << "create table if not exists CninfoAnnouncePDF ( " 
		<< "announcementId varchar(32), secCode varchar(16), secName varchar(32), announcementTime date, "
		<< "announcementTitle varchar(128), adjunctUrl varchar(128), catalogNo int, catalogName varchar(128),  "
		<< "isDisplay int, catalogContent varchar(4000),  PRIMARY KEY ( announcementId, catalogNo ))";
	return ss.str();
}


int  AdjustCatalog(TextExtractor &extractor, int nCatalogPage, int nLastCatalog, map<int, CPdfCatalog> & mapCatalogTmp)
{
	int nTry = 15;
	do
	{
		nTry--;
		if(extractor.m_mapPageContent.count(mapCatalogTmp[nLastCatalog].nPage)<=0)
		{
			map<int, CPdfCatalog>::iterator it = mapCatalogTmp.begin();
			for(; it!=mapCatalogTmp.end(); it++)
			{
				it->second.nPage -= 1;
			}
			continue;
		}

		string  strPage0 = extractor.m_mapPageContent[mapCatalogTmp[nLastCatalog].nPage];
		strPage0 = ReplaceString(strPage0, "                                                                     ", "");
		strPage0 = ReplaceString(strPage0, "　　　　　　　　　　　　　　　　　　　　　　　　　　　　", "");  // 全角空格
		strPage0 = ReplaceString(strPage0, "　", "");  // 全角空格
		strPage0 = ReplaceString(strPage0, " ", "");
		string  strKey = ReplaceString(mapCatalogTmp[nLastCatalog].strName, " ", "");

		// 第一条目录指向的主体内容 必定不在 目录页后面
		if(strPage0.find(strKey)!=string::npos && mapCatalogTmp[nLastCatalog].nPage != nCatalogPage)
			break;

		map<int, CPdfCatalog>::iterator it = mapCatalogTmp.begin();
		for(; it!=mapCatalogTmp.end(); it++)
		{
			it->second.nPage -= 1;
		}

	}while(nTry>=0);

	return nTry;
}


string  CPDFHaveCatalog::SaveAsCsv()
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

	//	截取前 20 页面，以便搜寻“目录”
	vector<string>		vecMatch;
	vector<int>			firstOffset;
	int  nCatalogPage = -1;
	for(int i=0; i<20; i++)
	{
		if(extractor.m_mapPageContent.count(i)>0 && 
			Pcre2Grep(_T("目[\\s\\t]*录"), extractor.m_mapPageContent[i].c_str() ,vecMatch, firstOffset )>0)
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
		sendToOutput(ss.str().c_str(), m_hWnd);

		return ss.str();
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

	if(Pcre2Grep(_T("目[\\s\\t]*录"), strQqjl.c_str() ,vecMatch, firstOffset )>0)
	{
		string::size_type pos1 = strQqjl.find(vecMatch[0]);
		if(pos1!=string::npos)
			strQqjl = strQqjl.substr(pos1 + vecMatch[0].length());
	}

	
	if(Pcre2Grep(_T("[^\\-\\d\\t\\.]+[\\.]{3,}[\\-\\s\\t]*[\\d]+"), strQqjl.c_str() ,vecMatch, firstOffset )>0)
	{
		int k = vecMatch.size()-1;
		string::size_type startPos = strQqjl.find(vecMatch[k]);
		startPos += vecMatch[k].length();

		strQqjl = strQqjl.substr(startPos);

		//	目录字符过滤， 并且确定调整 目录页号时使用的 目录项, （一般使用最后一项）
		int nLastCatalog = 0 ;
		for(int k = 0; k< vecMatch.size(); k++)
		{
			vector<string> firstMatch;
			if(Pcre2Split(_T("[\\.]+[\\-\\s\\t]*"), vecMatch[k].c_str(), firstMatch )>0)
			{
				if(firstMatch.size()!=2)
				{
					mapCatalog.clear();
					ss.str("");
					ss << "【"  << announcementTitle << "】";
					ss << " PDF 目录解析错误. \r\n";
					sendToOutput(ss.str().c_str(), m_hWnd);

					return ss.str();
				}
				else
				{
					//	推后 6 页开始调整 目录页号
					mapCatalog[k].nPage = atoi(firstMatch[1].c_str()) + 10;
					mapCatalog[k].strName = firstMatch[0];
					nLastCatalog = k;
				}
				
			}

		}

		//	调整目录页号，有的文档封面页编号为 1， 有的编号为 0, 有的编号为 - 1 等 
		if(mapCatalog.size()>=3)
		{
			map<int, CPdfCatalog> mapCatalogTmp1 = mapCatalog;
			map<int, CPdfCatalog> mapCatalogTmp2 = mapCatalog;
			map<int, CPdfCatalog> mapCatalogTmp3 = mapCatalog;
			int nTry1 = AdjustCatalog(extractor, nCatalogPage, nLastCatalog, mapCatalogTmp1);
			int nTry2 = AdjustCatalog(extractor, nCatalogPage, nLastCatalog -1, mapCatalogTmp2);
			int nTry3 = AdjustCatalog(extractor, nCatalogPage, nLastCatalog -2, mapCatalogTmp3);
	
			if(nTry1>=0 && nTry2>=0 && mapCatalogTmp1[nLastCatalog].nPage == mapCatalogTmp2[nLastCatalog].nPage)
				mapCatalog = mapCatalogTmp1;
			else if(nTry1>=0 && nTry3>=0 && mapCatalogTmp1[nLastCatalog].nPage == mapCatalogTmp3[nLastCatalog].nPage)
				mapCatalog = mapCatalogTmp1;
			else if(nTry2>=0 && nTry3>=0 && mapCatalogTmp2[nLastCatalog].nPage == mapCatalogTmp3[nLastCatalog].nPage)
				mapCatalog = mapCatalogTmp2;
			else
			{
				//	未能正确调整 页面号
				mapCatalog.clear();

				ss.str("");
				ss << "【"  << adjunctUrl << "】";
				ss << " PDF 未能正确调整 页面号 Err. \r\n";
				sendToOutput(ss.str().c_str(), m_hWnd);

				return ss.str();
			}
			
			//	对所有目录项进行验证，删除所有不合格目录项
			{
				
				vector<int>	vecDel;
				int nTotal = mapCatalog.size();
				for(int k=0; k<nTotal; k++)
				{
					string  strPage0 = extractor.m_mapPageContent[mapCatalog[k].nPage];
					//	由于 ReplaceString 采用递归算法，因此首先采用如下两行删除 连续的空格（含全角）
					strPage0 = ReplaceString(strPage0, "                                                                     ", "");
					strPage0 = ReplaceString(strPage0, "　　　　　　　　　　　　　　　　　　　　　　　　　　　　", "");  // 全角空格
					strPage0 = ReplaceString(strPage0, "　", "");  // 全角空格
					strPage0 = ReplaceString(strPage0, " ", "");
					string  strKey = ReplaceString(mapCatalog[k].strName, " ", "");

					if(strPage0.find(strKey)==string::npos)
					{
						vecDel.insert(vecDel.end(), k);
						mapCatalog.erase(k);
					}
				}

				if(vecDel.size()> 3 &&  (vecDel.size() * 10.0/nTotal)> 1.0)
				{
					mapCatalog.clear();

					ss.str("");
					ss << "【"  << adjunctUrl << "】";
					ss << " PDF 调整后页面号，不能通过验证 Err. 删除数 " << vecDel.size();
					ss << ",总目录数 " << nTotal << "\r\n";
					sendToOutput(ss.str().c_str(), m_hWnd);

					return ss.str();
				}
				else if(vecDel.size()>0)
				{
					ss.str("");
					ss << "【"  << adjunctUrl << "】 PDF 调整后页面后, 删除了页号 ";
					for( int k = 0; k< vecDel.size(); k++)
					{
						ss <<  vecDel[k] << "," ;
					}
					
					ss << " 总目录数 " << nTotal << "\r\n";
					sendToOutput(ss.str().c_str(), m_hWnd);
				}
			}
		}
		else
		{
			//	目录项个数太少
			mapCatalog.clear();

			ss.str("");
			ss << "【"  << adjunctUrl << "】";
			ss << " PDF 目录项个数太少 Err. \r\n";
			sendToOutput(ss.str().c_str(), m_hWnd);

			return ss.str();
		}

		//////////////////////////////////////////////////////////////////////////
		//	提取每个目录的内容
		map<int, CPdfCatalog>::iterator  it = mapCatalog.begin();
		for(; it!= mapCatalog.end(); it++)
		{
			//	确定本目录条的 起始页 与 最后页
			int nPage = it->second.nPage;
			string strName = it->second.strName;

			int nNextPage = extractor.m_mapPageContent.size() -1;
			string strNextName = "";
			it++;
			if( it != mapCatalog.end())
			{
				nNextPage = it->second.nPage;
				strNextName = it->second.strName;
			}
			it--;

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

				it->second.strContent = startContent;
			}
			else if(nPage < nNextPage)
			{
				//	第一页
				string startContent =  extractor.m_mapPageContent[nPage];
				string::size_type pos1 = startContent.find(strName);

				if(pos1!= string::npos)
					startContent = startContent.substr(pos1);

				it->second.strContent = startContent;

				//	中间页
				int nCurrPage = nPage + 1;
				while(nCurrPage < nNextPage)
				{
					it->second.strContent += extractor.m_mapPageContent[nCurrPage];
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

				it->second.strContent += startContent;
			}
		}
	}
	
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



