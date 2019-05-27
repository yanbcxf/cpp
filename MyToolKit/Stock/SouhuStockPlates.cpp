#include "StdAfx.h"
#include "SouhuStockPlates.h"

#include "UrlUtils.h"


CSouhuStockPlates::CSouhuStockPlates(void)
{
	m_strEnglishName = "SouhuStockPlates";
}

CSouhuStockPlates::~CSouhuStockPlates(void)
{
	m_vec_code.clear();
	m_vec_name.clear();
	m_vec_company_number.clear();
	m_vec_total_volume.clear();
	m_vec_total_turnover.clear();
	m_vec_parent_code.clear();
	m_vec_is_stock.clear();
}



int CSouhuStockPlates::CheckReport(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData)
{
	string header[] = {
		"板块或股票代码",
		"板块或股票名称",
		"公司家数",
		"总手",
		"总成交金额",
		"上级板块代码",
		"是否股票"
	};

	if(arrayHeader.size()!=7)
		return CSV_CHECK_FORMAT_ERROR;

	for(int i=0; i<7; i++)
	{
		if(arrayHeader[i]!=header[i])
			return CSV_CHECK_FORMAT_ERROR;
	}

	return CSV_CHECK_NO_ERROR;
}


void CSouhuStockPlates::ParseCsvFile(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData)
{
	stringstream ss;
	/*string connectString = "dbname=stock  port=3306   host=127.0.0.1 user=root  ";
	backend_factory const &backEnd = *soci::factory_mysql();*/

	m_vec_code.clear();
	m_vec_name.clear();
	m_vec_company_number.clear();
	m_vec_total_volume.clear();
	m_vec_total_turnover.clear();
	m_vec_parent_code.clear();
	m_vec_is_stock.clear();
	
	try
	{
		ss << "CSouhuStockPlates ParseCsvFile。。。。。。\r\n";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		for (int row = 0; row < arrayData.size(); row++)
		{
			int colnum = arrayData[row].size();
			
			m_vec_code[row]=atoi(arrayData[row][0].c_str());
			m_vec_name[row]= 1<colnum ? (arrayData[row][1]):0;
			m_vec_company_number[row]= 2<colnum ? atoi(arrayData[row][2].c_str()):0;
			m_vec_total_volume[row]= 3<colnum ? String2Double(arrayData[row][3]):0;
			m_vec_total_turnover[row]= 4<colnum ? String2Double(arrayData[row][4]):0;
			m_vec_parent_code[row]= 5<colnum ? atoi(arrayData[row][5].c_str()):0;
			m_vec_is_stock[row]= 6<colnum ? atoi(arrayData[row][6].c_str()):0;
		}

		ss.str("");
		ss << "OK, CSouhuStockPlates ParseCsvFile 完毕.\r\n";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << '\r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}



void CSouhuStockPlates::ParseXmlElement(void)
{
}

bool CSouhuStockPlates::ParseCsvFileName(string szFileName)
{
	vector<string> firstMatch;
	if(Pcre2Grep(m_strEnglishName.c_str(), szFileName,firstMatch )<=0)
		return false;

	return true;
}

void CSouhuStockPlates::ImportToDatabase(void)
{
	if( m_vec_code.size()<=0)
		return;

	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();

	try
	{
		ss << "CSouhuStockPlates::ImportToDatabase 开始导入数据.....\r\n";
		//sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		session sql(g_MysqlPool);

		// 删除“过时”的板块分组
		ss.str("");
		ss << "delete  from SouhuStockPlates where (Code, Parent_Code) not in( ";
		for (int row = 0; row < m_vec_code.size(); row++)
		{
			ss << "(" << m_vec_code[row] << "," << m_vec_parent_code[row] << ")";
			if(row < m_vec_code.size()-1)
				ss << ",";
		}
		ss << ")";
		string test1 = ss.str();
		sql << test1; 

		for (int row = 0; row < m_vec_code.size(); row++)
		{
			ss.str("");
			ss << "insert into SouhuStockPlates "
				<< " select " <<  Int2String(m_vec_code[row]) << ", \'" 
				<< m_vec_name[row] << "\', "
				<< Int2String(m_vec_company_number[row])  << ", "
				<< Double2String(m_vec_total_volume[row]) << ", "
				<< Double2String(m_vec_total_turnover[row]) << ", "
				<< Int2String(m_vec_parent_code[row]) << ", "
				<< Int2String(m_vec_is_stock[row]) << " "

				<< " from dual where not exists (SELECT 1 from SouhuStockPlates " 
				<< " where Parent_Code=" << Int2String(m_vec_parent_code[row]) << " " 
				<< " and Code=" << Int2String(m_vec_code[row]) << "  ) ";
			
			string test = /*Gbk2Utf8*/(ss.str());
			sql << test; 

			ss.str("");
			ss << "update SouhuStockPlates "
				<< " set Code=" <<  Int2String(m_vec_code[row]) << ", Name=\'" 
				<< m_vec_name[row] << "\', Company_Number="
				<< Int2String(m_vec_company_number[row])  << ",Total_Volume="
				<< Double2String(m_vec_total_volume[row]) << ", Total_Turnover="
				<< Double2String(m_vec_total_turnover[row]) << ", Parent_Code="
				<< Int2String(m_vec_parent_code[row]) << ", Is_Stock="
				<< Int2String(m_vec_is_stock[row]) << " "

				<< " where Parent_Code=" << Int2String(m_vec_parent_code[row]) << " " 
				<< " and Code=" << Int2String(m_vec_code[row]) << "  ";

			test = /*Gbk2Utf8*/(ss.str());
			sql << test; 

		}
		ss.str("");
		ss << "共有数据 " <<  m_vec_code.size() ;
		ss << " OK, CSouhuStockPlates::ImportToDatabase 数据导入完毕. \r\n";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << '\r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}

CStockDataModel * CSouhuStockPlates::NewCopy()
{
	CSouhuStockPlates * pTrading = new CSouhuStockPlates();
	pTrading->m_nRowNum = m_nRowNum;

	/*pTrading->m_vec_open = m_vec_open;
	pTrading->m_vec_high = m_vec_high;
	pTrading->m_vec_low = m_vec_low;
	pTrading->m_vec_close = m_vec_close;
	pTrading->m_vec_volume = m_vec_volume;*/

	return pTrading;
}



string CSouhuStockPlates::GetDownloadUrl()
{
	return "";
}


string CSouhuStockPlates::SqlScript()
{
	stringstream ss;
	ss << "create table if not exists SouhuStockPlates (Code int , Name varchar(256), Company_Number int, "
		<< " Total_Volume DECIMAL(15,4), Total_Turnover DECIMAL(15,4), "
		<< " Parent_Code int, Is_Stock int,  PRIMARY KEY ( Code, Parent_Code))";
	return ss.str();
}

string  CSouhuStockPlates::SaveAsCsv()
{

	stringstream ss;

	//////////////////////////////////////////////////////////////////////////
	//	对于本日已经成功下载的 ，忽略该任务
	if(CheckDownloadFileInfo(GetCsvFileName())>0)
	{
		ss.str("");
		ss << "搜狐板块，本日已经最新";
		ss << "\r\n";
		sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);

		return "NoMoreData";
	}



	CStdioFile  targetFile;
	bool bWriteTitle = true;

	string  mainPlate[3] = {
		//"http://hq.stock.sohu.com/pl/pl-1.html?uid=1512222240560727&7027194625967",
		"http://hq.stock.sohu.com/pl/pl-1631.html?uid=1512222240560727&7027194625967",
		"http://hq.stock.sohu.com/pl/pl-1632.html?uid=1512222240560727&7027194625967",
		"http://hq.stock.sohu.com/pl/pl-1630.html?uid=1512222240560727&7027194625967"
	};

	//	先下载主板的信息

	vector<string>  vecCode;
	for(int index =0; index<3; index++)
	{
		try
		{
			string  strGBK=CUrlUtils::PostUrlOfSouhu(mainPlate[index].c_str(),NULL);

			vector<string> vecMatch;
			string pattern = "<script>document.domain=\\'sohu.com\\';PEAK_ODIA=parent\\.PEAK_ODIA;</script><script>PEAK_ODIA\\(";
			if(Pcre2Grep(pattern, strGBK.c_str() ,vecMatch )>0)
			{
				string::size_type pos = strGBK.find(vecMatch[0]);
				strGBK = strGBK.substr(pos + 10 + vecMatch[0].length());
				strGBK = strGBK.substr(0, strGBK.length()-11);
				
				strGBK = ReplaceChar(strGBK, '\'', "\"");
				strGBK = "[" + strGBK + "]";

				Json::Value jsonRoot;
				Json::Reader reader(Json::Features::strictMode());
				bool parsingSuccessful = reader.parse(strGBK, jsonRoot);
				if (!parsingSuccessful) {
					ss.str("");
					ss << "搜狐板块【" << index << "】，解析 JSON 错误 ：";
					ss << reader.getFormattedErrorMessages().c_str();
					ss << "\r\n";
					sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
				}
				else
				{
					//Json::Value json_result = jsonRoot[1];
					if(jsonRoot.type()== Json::arrayValue)
					{
						int size = jsonRoot.size();
						if(size>0)
						{
							if(bWriteTitle)
							{
								BOOL b = targetFile.Open(GetCsvFileName().c_str(),\
									CFile::modeWrite|CFile::shareDenyWrite|CFile::modeCreate|CFile::typeText);
								if(b==FALSE)
									return "";

								//	第一次循环时，才输出 CSV 文件的标题头
								ss.str("");
								ss << "板块或股票代码," << "板块或股票名称," << "公司家数," << "总手," << "总成交金额," ;
								ss << "上级板块代码," << "是否股票" << "\n";
								targetFile.WriteString(ss.str().c_str());
								bWriteTitle = false;

								ss.str("");
								ss << "1631," << "行业分类," << "0," << "0," << "0," ;
								ss << "0," << "0" << "\n";
								targetFile.WriteString(ss.str().c_str());

								ss.str("");
								ss << "1632," << "地域板块," << "0," << "0," << "0," ;
								ss << "0," << "0" << "\n";
								targetFile.WriteString(ss.str().c_str());

								ss.str("");
								ss << "1630," << "概念板块," << "0," << "0," << "0," ;
								ss << "0," << "0" << "\n";
								targetFile.WriteString(ss.str().c_str());
							}	


							for(int r = 0; r< size; r++)
							{
								Json::Value  struction = jsonRoot[r];
								
								ss.str("");
								ss << struction[0] << "," << struction[1] << "," << struction[2] << "," << struction[6] << ",";
								ss << struction[7] << ",";
								if(index==0)
									ss << "1631";
								else if(index==1)
									ss << "1632";
								else if(index==2)
									ss << "1630";
								ss << ",0" << "\n";
								string test = ss.str();
								targetFile.WriteString(ss.str().c_str());

								vecCode.push_back(struction[0].asString());
							}

							ss.str("");
							ss << "搜狐板块【" << index << "】获取 JSON 成功.\r\n";
							sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
						}

					}
					else
					{
						ss.str("");
						ss << "搜狐板块【" << index << "】 获取数据失败.\r\n";
						sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
					}
				}
			}
		}
		catch (std::exception const & e)
		{
			ss.str("");
			ss << "搜狐板块,异常" << e.what() << ' \r\n';
			sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
		}
	}

	if(bWriteTitle==true)
		return "";

	//	开始下载各个子板块所属的股票信息
	for(int i=0; i < vecCode.size(); i++)
	{
		string strUrl = vecCode[i];
		strUrl = string("http://hq.stock.sohu.com/pl/") +  strUrl  + string("-1.html?uid=1512222240560727&7027194625967");
		string  strGBK=CUrlUtils::PostUrlOfSouhu(strUrl.c_str(),NULL);

		vector<string> vecMatch;
		string pattern = "<script>document.domain=\\'sohu.com\\';PEAK_ODIA=parent\\.PEAK_ODIA;</script><script>PEAK_ODIA\\(";
		if(Pcre2Grep(pattern, strGBK.c_str() ,vecMatch )>0)
		{
			string::size_type pos = strGBK.find(vecMatch[0]);
			strGBK = strGBK.substr(pos + vecMatch[0].length());
			strGBK = strGBK.substr(0, strGBK.length()-10);

			strGBK = ReplaceChar(strGBK, '\'', "\"");
			
			Json::Value jsonRoot;
			Json::Reader reader(Json::Features::strictMode());
			bool parsingSuccessful = reader.parse(strGBK, jsonRoot);
			if (!parsingSuccessful) {
				ss.str("");
				ss << "搜狐板块【" << vecCode[i] << "】，解析 JSON 错误 ：";
				ss << reader.getFormattedErrorMessages().c_str();
				ss << "\r\n";
				sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
			}
			else
			{
				//Json::Value json_result = jsonRoot[1];
				if(jsonRoot.type()== Json::arrayValue)
				{
					int size = jsonRoot.size();
					if(size>0)
					{
						for(int r = 2; r< size; r++)
						{
							Json::Value  struction = jsonRoot[r];

							string strCode = struction[0].asString();
							strCode = strCode.substr(3);
							ss.str("");
							ss << strCode << "," << struction[1] << "," << 1 << "," << struction[6] << ",";
							ss << struction[7] << ",";
							ss << vecCode[i] << ", 1";
							ss <<  "\n";
							string test = ss.str();
							targetFile.WriteString(ss.str().c_str());
						}

						ss.str("");
						ss << "搜狐板块【" << vecCode[i] << "】获取 JSON 成功.\r\n";
						sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
					}

				}
				else
				{
					ss.str("");
					ss << "搜狐板块【" << vecCode[i] << "】 获取数据失败. \r\n";
					sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
				}
			}
		}
	}

	if(bWriteTitle==false)
	{
		//	bWriteTitle == false, 说明本次成功下载了数据
		targetFile.Flush();
		targetFile.Close();
		return GetCsvFileName();
	}
	return "";
}

string CSouhuStockPlates::GetCsvFileName()
{
	CString tmp;
	tmp.Format("%s/StockInfo/%s.csv" , g_strCurrentDir.c_str(), m_strEnglishName.c_str());
	return string(tmp.GetBuffer());
}


void CSouhuStockPlates::ExportFromDatabase(double startTime, double endTime)
{
	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();

	try
	{
		ss << "CSouhuStockPlates 开始从 MYSQL 获取数据......";
		ss << "【" << m_nCode << "】";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select Code, Name, Company_Number, Total_Volume, Total_Turnover, Parent_Code, Is_Stock"
			<< " from SouhuStockPlates order by Is_Stock, Parent_Code, Code " ;	/*order by Is_Stock, Parent_Code, Code*/

		string test = ss.str();

		row r;
		//session sql(backEnd, m_ConnectString);
		session sql(g_MysqlPool);

		statement st = (sql.prepare << ss.str(),into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{
			m_vec_code[cnt] = r.get<int>("Code");
			m_vec_name[cnt] = /*Utf8_GBK*/(r.get<string>("Name"));
			m_vec_company_number[cnt] = r.get<int>("Company_Number");
			m_vec_total_volume[cnt] = r.get<double>("Total_Volume");
			m_vec_total_turnover[cnt] = r.get<double>("Total_Turnover");
			m_vec_parent_code[cnt] = r.get<int>("Parent_Code");
			m_vec_is_stock[cnt] = r.get<int>("Is_Stock");
			
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


void CSouhuStockPlates::NotifyDatabaseImportedToWindow()
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

	sendToOutput(ss.str().c_str(), m_hWnd);
}

CStockPlateTree * CSouhuStockPlates::GenerateTree()
{
	CStockPlateTree * pTree = new CStockPlateTree;

	CStockPlateTree * pNode = pTree;
	for(int i=0; i<m_vec_code.size(); i++)
	{
		CStockPlateData treeNode;
		treeNode.code = Int2String(m_vec_code[i], m_vec_is_stock[i]==1? "%06d":"%08d");
		treeNode.name = m_vec_name[i];
		treeNode.company_number = m_vec_company_number[i];
		treeNode.total_turnover = m_vec_total_turnover[i];
		treeNode.total_volume = m_vec_total_volume[i];
		treeNode.is_stock = m_vec_is_stock[i];

		CStockPlateData treeNodeParent;
		treeNodeParent.code = Int2String(m_vec_parent_code[i], "%08d");
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

vector<CStockPlateData> CSouhuStockPlates::FetchDataFromTree(CStockPlateTree * pPlates, int nPlateCode)
{
	vector<CStockPlateData> vecStockPlateData;
	if(pPlates)
	{	
		//	以某个节点为根，进行子树的搜索
		CStockPlateData treeNode;
		treeNode.code = Int2String(nPlateCode, "%08d");
		treeNode.is_stock = 0;

		CStockPlateTree * pNode = pPlates->SearchSpecialNode(treeNode);
		if(pNode==NULL)
			pNode = pPlates;
		if(pNode)
		{
			//	该节点为根节点
			CStockPlateData spd = *pNode->get();
			spd.code = Int2String(nPlateCode, "%08d");
			spd.parent_code = "-1";
			spd.is_stock = 0;
			if(spd.name.empty())
				spd.name = "根目录";
			vecStockPlateData.push_back(spd);

			CStockPlateTree::pre_order_iterator pre_order_it = pNode->pre_order_begin();
			while(pre_order_it!=pNode->pre_order_end())
			{
				CStockPlateData spd = *pre_order_it;
				if(pre_order_it.node()->parent()->is_root()==false)
				{
					spd.parent_code = pre_order_it.node()->parent()->get()->code;
				}
				else
				{
					spd.parent_code = Int2String(nPlateCode, "%08d");
				}
				vecStockPlateData.push_back(spd);
				pre_order_it++;
			}

		}
	}
	return vecStockPlateData;
}