#include "StdAfx.h"
#include "CapitalStructOf10jqka.h"

#include "UrlUtils.h"

CCapitalStructOf10jqka::CCapitalStructOf10jqka(void)
{
}

CCapitalStructOf10jqka::~CCapitalStructOf10jqka(void)
{
}

string CCapitalStructOf10jqka::GetDownloadUrl()
{
	/*取消 wget.exe 下载*/
	return "";

	string url = "";
	char cmdline[512]; 
	CString tmp;    

	string::size_type pos = GetCsvFileName().find(".csv");
	string xlsFileName = GetCsvFileName().substr(0, pos);
	xlsFileName += ".html";

	//	对于本日已经成功下载的 ，忽略该任务
	if(CheckDownloadFileInfo(xlsFileName)>0)
		return url;

	//	从“同花顺”直接下载网页
	tmp.Format("wget.exe http://stockpage.10jqka.com.cn/%06d//holder/#astockchange -O %s", \
		m_nCode, xlsFileName.c_str());    

	sprintf(cmdline,"%s",tmp);    
	url = string(cmdline);
	return url;
}


void CCapitalStructOf10jqka::SaveTable(tree<HTML::Node> &dom, tree<HTML::Node>::sibling_iterator si_it,
										 tree<HTML::Node>::sibling_iterator si_end, int & nTotalRow)
{
	for( ; si_it!=si_end; si_it++)
	{
		string tagName = si_it->tagName();
		if(tagName=="tbody")
		{

			tree<HTML::Node>::sibling_iterator tr_it = dom.begin(si_it);
			tree<HTML::Node>::sibling_iterator tr_end = dom.end(si_it);
			for(; tr_it!=tr_end; tr_it++)
			{
				tagName = tr_it->tagName();
				if(tagName=="tr")
				{
					int td_index = 0;
					string AShares = "";
					string BShares = "0";
					string changeReason = "0";
					string changeReasonDesc = "";
					string realDate = "";
					string seq = "0";
					string totalShares = "0";
					string ATotalShares = "";

					tree<HTML::Node>::sibling_iterator td_it = dom.begin(tr_it);
					tree<HTML::Node>::sibling_iterator td_end = dom.end(tr_it);
					for(; td_it!=td_end; td_it++)
					{
						tree<HTML::Node>::sibling_iterator val_it = dom.begin(td_it);
						tree<HTML::Node>::sibling_iterator val_end = dom.end(td_it);
						for(; val_it!=val_end; val_it++)
						{
							string text = val_it->text();
							if(td_index==0)
								realDate = text;
							if(td_index==1)
								changeReasonDesc = text;	//	变动原因
							if(td_index==2)
								ATotalShares = text;		//	变动后A股总股本(万股)
							if(td_index==3)
								AShares = text;				//	变动后流通A股(万股)

							td_index++ ;
							//sendToOutput(text.c_str(), m_hWnd);
						}
					}

					m_vec_real_date[nTotalRow]= realDate;
					m_vec_ashares[nTotalRow]=  String2Double(AShares);
					if(AShares.find("亿")!=string::npos)
						m_vec_ashares[nTotalRow] *= 10000.0;

					m_vec_bshares[nTotalRow]=  String2Double(BShares);
					m_vec_change_reason[nTotalRow]=  String2Double(changeReason);
					m_vec_change_reason_desc[nTotalRow]=  changeReasonDesc.empty() == false? changeReasonDesc:"-";
					m_vec_seq[nTotalRow]= String2Double(seq);
					
					m_vec_total_shares[nTotalRow]= String2Double(totalShares);
					if(totalShares.find("亿")!=string::npos)
						m_vec_total_shares[nTotalRow] *= 10000.0;

					m_vec_a_total_shares[nTotalRow]= String2Double(ATotalShares);
					if(ATotalShares.find("亿")!=string::npos)
						m_vec_a_total_shares[nTotalRow] *= 10000.0;

					nTotalRow++;
				}
			}


		}
	}
}


string  CCapitalStructOf10jqka::SaveAsCsv()
{
	m_vec_real_date.clear();
	m_vec_ashares.clear();
	m_vec_bshares.clear();
	m_vec_change_reason.clear();
	m_vec_change_reason_desc.clear();
	m_vec_seq.clear();
	m_vec_total_shares.clear();
	m_vec_a_total_shares.clear();

	stringstream ss;
	string html;
	tree<HTML::Node> dom;

	int nTotalRow = 0;
	try
	{

		//////////////////////////////////////////////////////////////////////////

		char tmpCode[256] = {0};
		sprintf(tmpCode, "http://stockpage.10jqka.com.cn/%06d//holder/#astockchange", m_nCode);

		string test = string(tmpCode);
		string  strGBK=CUrlUtils::PostUrlOfSouhu(tmpCode,NULL);
		html = Utf8_GBK(strGBK);

		HTML::ParserDom parser;
		parser.parse(html);
		dom = parser.getTree();

		/* 先按照第一种情形解析 */
		int nLoopLimit = 0;
		tree<HTML::Node>::iterator it = dom.begin();
		tree<HTML::Node>::iterator end = dom.end();
		for (; it != end; ++it)
		{
			if (it->tagName() =="table")
			{
				it->parseAttributes();
				
				map<string,string> attrs = it->attributes();
				if(attrs["id"] == "astockchange_table")
				{
					tree<HTML::Node>::sibling_iterator si_it = dom.begin(it);
					tree<HTML::Node>::sibling_iterator si_end = dom.end(it);
					SaveTable(dom, si_it, si_end, nTotalRow);
					break;
				}
				
			}
			nLoopLimit++;
			if(nLoopLimit%100 == 0)
				Sleep(10);
		}

		/* 再按照第 二 种情形解析 */
		if(nTotalRow==0)
		{
			nLoopLimit = 0;
			it = dom.begin();
			end = dom.end();
			for (; it != end; ++it)
			{
				bool bBreak = false;
				if (it->tagName() =="div")
				{
					it->parseAttributes();

					map<string,string> attrs = it->attributes();
					if(attrs["id"] == "astockchange")
					{
						tree<HTML::Node>::sibling_iterator div_it = dom.begin(it);
						div_it ++;
						div_it ++;
						div_it ++;
						if(div_it->tagName()=="div")
						{
							tree<HTML::Node>::sibling_iterator table_it = dom.begin(div_it);
							tree<HTML::Node>::sibling_iterator table_end = dom.end(div_it);
							for(; table_it!=table_end; table_it++)
							{
								string tmp = table_it->tagName();
								if(tmp=="table")
								{
									tree<HTML::Node>::sibling_iterator si_it = dom.begin(table_it);
									tree<HTML::Node>::sibling_iterator si_end = dom.end(table_it);
									SaveTable(dom, si_it, si_end, nTotalRow);
									bBreak = true;
									break;
								}
								
							}
							
						}
						
					}

				}

				if(bBreak)
					break;

				nLoopLimit++;
				if(nLoopLimit%100 == 0)
					Sleep(10);
			}
		}


		if(nTotalRow>0)
			return "AlreadyInMemory";

	}
	catch(...)
	{

	}
	return "";

}

string  CCapitalStructOf10jqka::SaveAsCsv_Old()
{
	stringstream ss;
	string html;
	tree<HTML::Node> dom;

	try
	{

		string::size_type pos = GetCsvFileName().find(".csv");
		string xlsFileName = GetCsvFileName().substr(0, pos);
		xlsFileName += ".html";
		//////////////////////////////////////////////////////////////////////////

		/*ifstream file(xlsFileName.c_str());
		if (!file.is_open()) 
		{
			ss.str("");
			ss << "Error! Open " << xlsFileName; 
			sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
			return "";
		}
		else
		{
			while (1)
			{
				char buf[BUFSIZ];
				file.read(buf, BUFSIZ);
				if(file.gcount() == 0) {
					break;
				}
				html.append(buf, file.gcount());
			}
			file.close();
		}
		

		html = Utf8_GBK(html);*/

		char tmpCode[256] = {0};
		sprintf(tmpCode, "http://stockpage.10jqka.com.cn/%06d//holder/#astockchange", m_nCode);

		string test = string(tmpCode);
		string  strGBK=CUrlUtils::PostUrlOfSouhu(tmpCode,NULL);
		html = Utf8_GBK(strGBK);

		HTML::ParserDom parser;
		parser.parse(html);
		dom = parser.getTree();

		tree<HTML::Node>::iterator it = dom.begin();
		tree<HTML::Node>::iterator end = dom.end();
		for (; it != end; ++it)
		{
			if (it->tagName() =="table")
			{
				it->parseAttributes();
				
				map<string,string> attrs = it->attributes();
				if(attrs["id"] == "astockchange_table")
				{
					break;
				}
				
			}
		}

		if(it!=end)
		{
			int nChild = dom.number_of_children(it);
			tree<HTML::Node>::sibling_iterator si_it = dom.begin(it);
			tree<HTML::Node>::sibling_iterator si_end = dom.end(it);
			for( ; si_it!=si_end; si_it++)
			{
				string tagName = si_it->tagName();
				if(tagName=="tbody")
				{
					CStdioFile  targetFile(GetCsvFileName().c_str(), CFile::modeWrite|CFile::shareDenyWrite|CFile::modeCreate|CFile::typeText);
					ss.str("");
					ss << "realDate," << "AShares," << "BShares," << "changeReason," << "changeReasonDesc,"; 
					ss << "seq," << "totalShares," << "ATotalShares" << "\n";
					targetFile.WriteString(ss.str().c_str());

					tree<HTML::Node>::sibling_iterator tr_it = dom.begin(si_it);
					tree<HTML::Node>::sibling_iterator tr_end = dom.end(si_it);
					for(; tr_it!=tr_end; tr_it++)
					{
						tagName = tr_it->tagName();
						if(tagName=="tr")
						{
							int td_index = 0;
							string AShares = "";
							string BShares = "0";
							string changeReason = "0";
							string changeReasonDesc = "";
							string realDate = "";
							string seq = "0";
							string totalShares = "0";
							string ATotalShares = "";

							tree<HTML::Node>::sibling_iterator td_it = dom.begin(tr_it);
							tree<HTML::Node>::sibling_iterator td_end = dom.end(tr_it);
							for(; td_it!=td_end; td_it++)
							{
								tree<HTML::Node>::sibling_iterator val_it = dom.begin(td_it);
								tree<HTML::Node>::sibling_iterator val_end = dom.end(td_it);
								for(; val_it!=val_end; val_it++)
								{
									string text = val_it->text();
									if(td_index==0)
										realDate = text;
									if(td_index==1)
										changeReasonDesc = text;	//	变动原因
									if(td_index==2)
										ATotalShares = text;		//	变动后A股总股本(万股)
									if(td_index==3)
										AShares = text;				//	变动后流通A股(万股)
									
									td_index++ ;
									//sendToOutput(text.c_str(), m_hWnd);
								}
							}

							ss.str("");
							ss << realDate << "," << AShares << "," << BShares << "," << changeReason << ",\"";
							ss << changeReasonDesc << "\"," << seq << "," << totalShares << "," << ATotalShares << "\n";
							targetFile.WriteString(ss.str().c_str());
						}
					}
					targetFile.Flush();
					targetFile.Close();
					return GetCsvFileName();

				}
			}
			
		}

	}
	catch(...)
	{

	}
	return "";

}