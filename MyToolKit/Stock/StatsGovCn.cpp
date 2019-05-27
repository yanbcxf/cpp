#include "StdAfx.h"
#include "StatsGovCn.h"

#include "UrlUtils.h"

bool CStatsGovCn::m_bLogon = false;

CStatsGovCn::CStatsGovCn(void)
{
	//	先分配小的内存，因为统计局的菜单下载成功后，会产生 1000 多个的下载任务，
	//  分配过大会瞬间产生大量的 “提交内存”，造成奔溃
	m_capacity = 10 * 1024;		
	m_size = 0;
	m_buffer = new unsigned char[m_capacity];
	memset(m_buffer, 0, m_capacity);
}

CStatsGovCn::~CStatsGovCn(void)
{
	delete[] m_buffer;
}


void  CStatsGovCn::FindZbNameFromBottom(Json::Value  & wdnodes,  string strWdCode,  string strWdSubCode,
										string & strZbName, string & strUnit)
{
	strZbName = "";
	strUnit = "";
	for(int k=0; k< wdnodes.size(); k++)
	{
		string test1 = wdnodes[k]["wdcode"].asString();
		if(wdnodes[k]["wdcode"].asString()!=strWdCode)
			continue;

		Json::Value  zb_nodes = wdnodes[k]["nodes"];
		for(int i=0; i< zb_nodes.size(); i++)
		{
			string test2 = zb_nodes[i]["code"].asString();
			if(zb_nodes[i]["code"].asString()==strWdSubCode)
			{
				strZbName = zb_nodes[i]["cname"].asString();
				strUnit = zb_nodes[i]["unit"].asString();
				break;
			}
		}
		break;
	}
}


string CStatsGovCn::FindZbCodeFromDataNode(Json::Value  & datanode, string strWdCode)
{
	for(int i=0; i< datanode["wds"].size(); i++)
	{
		string wdcode =  datanode["wds"][i]["wdcode"].asString();
		if(strWdCode==wdcode)
			return datanode["wds"][i]["valuecode"].asString();
	}
	return "";
}


bool CStatsGovCn::Logon()
{
	/* 进行登录 */
	if(m_bLogon==false)
	{
		char tmpCode[256] = {0};
		sprintf(tmpCode, "http://data.stats.gov.cn/login.htm?m=ajaxLogin");
		string  strGBK=PostUrlOfStatsGov(tmpCode, "username=yss2002%40qq.com&keyp=991105");

		//strGBK = Utf8_GBK(strGBK);
		if(strGBK.find("{\"returncode\":200,\"returndata\":\"1\"}")!=string::npos)
			m_bLogon = true;
	}

	return m_bLogon;
}


/* GET /easyquery.htm?m=getOtherWds&dbcode=fsjd&rowcode=zb&colcode=sj&wds=%5B%5D&k1=1471488443880
	该调用会放回本菜单页面所需要的其他 "列表下拉栏"中的数据, 如 “地区”，“时间”下拉栏
*/

/*	strDbCode ： hgjd = 季度数据 ， hgyd = 月度数据 , fsnd = 分省年度, fsjd = 分省季度 */
/*	strWdCode :  zb = 指标， reg = 地区 */

void CStatsGovCn::GetMenuTree(string strDbCode, string strWdCode, string strWdSubCode)
{
	stringstream ss;
	vector<string>  vecId;
	if(Logon())
	{
		/* 根据指标大类获取子菜单数据 */

		string test1 = string("http://data.stats.gov.cn/easyquery.htm"); 
		string postData = string("id=") + strWdSubCode + "&dbcode=" + strDbCode + "&wdcode=" + strWdCode + "&m=getTree"; 

		ss.str("");
		ss << "CStatsGovCn::获取子菜单 ";
		ss << test1 + "?" + postData;
		ss << "\r\n";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		string strGBK  = PostUrlOfStatsGov(test1 + "?" + postData, NULL);
		//strGBK = Utf8_GBK(strGBK);

		Json::Value jsonRoot;
		Json::Reader reader(Json::Features::strictMode());
		bool parsingSuccessful = reader.parse(strGBK, jsonRoot);
		if(parsingSuccessful)
		{
			for(int i=0; i< jsonRoot.size(); i++)
			{
				tree_dbcode.insert(tree_dbcode.end(), jsonRoot[i]["dbcode"].asString());
				tree_id.insert(tree_id.end(), jsonRoot[i]["id"].asString());
				tree_name.insert(tree_name.end(), jsonRoot[i]["name"].asString());
				tree_pid.insert(tree_pid.end(), jsonRoot[i]["pid"].asString());
				tree_wdcode.insert(tree_wdcode.end(), jsonRoot[i]["wdcode"].asString());

				tree_isParent.insert(tree_isParent.end(), jsonRoot[i]["isParent"].asBool());

				if(jsonRoot[i]["isParent"].asBool())
				{
					vecId.push_back(jsonRoot[i]["id"].asString());
				}
			}
		}
	}

	for(int i=0; i< vecId.size(); i++)
	{
		/* 递归调用，获取子菜单 */
		GetMenuTree(strDbCode, strWdCode, vecId[i]);
	}
}

//	获取省份的编码及名称
map<string, string> CStatsGovCn::GetOtherWds(string strDbCode, string strZbCode)
{
	stringstream ss;
	map<string, string>	map_reg_code_name;
	if(1 /*Logon()*/)
	{
		struct timeval timeNow;
		gettimeofday(&timeNow, NULL);
		long k1 = timeNow.tv_sec + timeNow.tv_usec/1000;
		ss << "http://data.stats.gov.cn/easyquery.htm?m=getOtherWds&dbcode=" << strDbCode << "&rowcode=zb&colcode=sj&wds=";
		ss << UrlEncode(string("[{\"wdcode\":\"zb\",\"valuecode\":\"") + strZbCode + "\"}]") << "&k1=" << k1;
		string test1 = ss.str();
		
		string test2 = UrlDecode(test1);

		//string strGBK  = PostUrlOfStatsGov(test1, NULL);

		string strGBK = "{\"returncode\":200,\"returndata\":[{\"issj\":false,\"nodes\":[{\"code\":\"110000\",\"name\":\"北京市\",\"sort\":\"1\"},{\"code\":\"120000\",\"name\":\"天津市\",\"sort\":\"1\"},{\"code\":\"130000\",\"name\":\"河北省\",\"sort\":\"1\"},{\"code\":\"140000\",\"name\":\"山西省\",\"sort\":\"1\"},{\"code\":\"150000\",\"name\":\"内蒙古自治区\",\"sort\":\"1\"},{\"code\":\"210000\",\"name\":\"辽宁省\",\"sort\":\"1\"},{\"code\":\"220000\",\"name\":\"吉林省\",\"sort\":\"1\"},{\"code\":\"230000\",\"name\":\"黑龙江省\",\"sort\":\"1\"},{\"code\":\"310000\",\"name\":\"上海市\",\"sort\":\"1\"},{\"code\":\"320000\",\"name\":\"江苏省\",\"sort\":\"1\"},{\"code\":\"330000\",\"name\":\"浙江省\",\"sort\":\"1\"},{\"code\":\"340000\",\"name\":\"安徽省\",\"sort\":\"1\"},{\"code\":\"350000\",\"name\":\"福建省\",\"sort\":\"1\"},{\"code\":\"360000\",\"name\":\"江西省\",\"sort\":\"1\"},{\"code\":\"370000\",\"name\":\"山东省\",\"sort\":\"1\"},{\"code\":\"410000\",\"name\":\"河南省\",\"sort\":\"1\"},{\"code\":\"420000\",\"name\":\"湖北省\",\"sort\":\"1\"},{\"code\":\"430000\",\"name\":\"湖南省\",\"sort\":\"1\"},{\"code\":\"440000\",\"name\":\"广东省\",\"sort\":\"1\"},{\"code\":\"450000\",\"name\":\"广西壮族自治区\",\"sort\":\"1\"},{\"code\":\"460000\",\"name\":\"海南省\",\"sort\":\"1\"},{\"code\":\"500000\",\"name\":\"重庆市\",\"sort\":\"1\"},{\"code\":\"510000\",\"name\":\"四川省\",\"sort\":\"1\"},{\"code\":\"520000\",\"name\":\"贵州省\",\"sort\":\"1\"},{\"code\":\"530000\",\"name\":\"云南省\",\"sort\":\"1\"},{\"code\":\"540000\",\"name\":\"西藏自治区\",\"sort\":\"1\"},{\"code\":\"610000\",\"name\":\"陕西省\",\"sort\":\"1\"},{\"code\":\"620000\",\"name\":\"甘肃省\",\"sort\":\"1\"},{\"code\":\"630000\",\"name\":\"青海省\",\"sort\":\"1\"},{\"code\":\"640000\",\"name\":\"宁夏回族自治区\",\"sort\":\"1\"},{\"code\":\"650000\",\"name\":\"新疆维吾尔自治区\",\"sort\":\"1\"}],\"selcode\":\"\",\"wdcode\":\"reg\",\"wdname\":\"地区\"},{\"issj\":true,\"nodes\":[{\"code\":\"LAST13\",\"name\":\"最近13个月\",\"sort\":\"4\"},{\"code\":\"LAST24\",\"name\":\"最近24个月\",\"sort\":\"4\"},{\"code\":\"LAST36\",\"name\":\"最近36个月\",\"sort\":\"4\"}],\"selcode\":\"last13\",\"wdcode\":\"sj\",\"wdname\":\"时间\"}]}";

		Json::Value jsonRoot;
		Json::Reader reader(Json::Features::strictMode());
		bool parsingSuccessful = reader.parse(strGBK, jsonRoot);
		if(parsingSuccessful)
		{
			Json::Value json_result = jsonRoot["returndata"];
			if(json_result.type()!=Json::stringValue)
			{
				if(json_result[0]["nodes"].type()== Json::arrayValue)
				{
					for(int index = 0; index< json_result[0]["nodes"].size(); index++)
					{
						Json::Value  datanode = json_result[0]["nodes"][index];
						map_reg_code_name[datanode["code"].asString()] = datanode["name"].asString();
						
					}
				}
			}
		}
		int kkkk = 1;
	}
	return map_reg_code_name;
}

void CStatsGovCn::SaveMenuTree()
{
	stringstream ss;
	try
	{
		ss << "CStatsGovCn::ImportToDatabase 开始导入数据.....\r\n";
		//sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//session sql(backEnd, m_ConnectString);
		session sql(g_MysqlPool);

		/* 创建数据表 */
		ss.str("");
		ss << "create table if not exists " 
			<< " StatsGovTree ( dbcode varchar(32), id varchar(64), " 
			<< " isParent int,  name varchar(256),  pid varchar(32) , wdcode varchar(32), updateDate date, "
			<< " PRIMARY KEY ( dbcode, id ))";
		sql << ss.str();

		
		for (int row = 0; row < tree_dbcode.size(); row++)
		{
			ss.str("");
			ss << "insert into StatsGovTree ("
				<< "dbcode, id, isParent, name, pid, wdcode ) " 
				<< " select  \'" 
				<< tree_dbcode[row] << "\',\'"
				<< tree_id[row] << "\', "
				<< (tree_isParent[row]? 1:0) << ",\'"
				<< tree_name[row] << "\',\'"
				<< tree_pid[row] << "\',\'"
				<< tree_wdcode[row] << "\' "
			
				<< " from dual where not exists ( SELECT 1 from StatsGovTree " 
				<< " where dbcode=\'" << tree_dbcode[row] << "\' " 
				<< " and id=\'" << tree_id[row] << "\'  ) ";

			string test = ss.str();
			//test = Gbk2Utf8(test);
			sql << test; 

			ss.str("");
			ss << "update StatsGovTree set "
				<< " dbcode=\'" <<  tree_dbcode[row] << "\',"
				<< " id=\'" << tree_id[row] <<"\',"
				<< " isParent=" << (tree_isParent[row]? 1:0) <<","
				<< " name=\'" << tree_name[row] << "\',"
				<< " pid=\'" << tree_pid[row] << "\',"
				<< " wdcode=\'" << tree_wdcode[row] << "\' "

				<< " where dbcode=\'" << tree_dbcode[row] << "\' " 
				<< " and id=\'" << tree_id[row] << "\'  ";

			test = ss.str();
			//test = Gbk2Utf8(test);
			sql << test; 

			//sendToOutput(ss.str().c_str());
		}


		ss.str("");
		ss << "【"  << (tree_dbcode.size()>0? tree_dbcode[0] : "无菜单") << "】";
		ss << "共有数据 " << tree_dbcode.size() ;
		ss << " OK, CStatsGovCn::ImportToDatabase 数据导入完毕. \r\n";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << '\r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}

}

vector<string>	CStatsGovCn::NeedDownload(string strDbCode)
{
	vector<string>  vecId;
	stringstream ss;
	try
	{
		session sql(g_MysqlPool);

		ss << "select id from  StatsGovTree where  isParent = 0 and dbcode=\'" 
			<< strDbCode << "\' and ( updateDate is null or  updateDate < date_sub(now(), INTERVAL 1 day) )";
		string test = ss.str();
		
		row r;
		statement st = (sql.prepare << ss.str() ,into(r)) ;
		st.execute();

		while (st.fetch())
		{
			string	id = r.get<string>("id");
			vecId.push_back(id);
		}

	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << '\r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	return vecId;
}

void  CStatsGovCn::ModifyUpdateDate(string strDbCode, string strZbCode, int nDelay)
{
	vector<string>  vecId;
	stringstream ss;
	try
	{
		session sql(g_MysqlPool);

		// 更新本菜单下数据的最新日期
		ss.str("");
		ss << "update StatsGovTree set updateDate = date_add(now(), INTERVAL " 
			<< nDelay <<" day)  where dbcode=\'" << strDbCode << "\' and "
			<< " id=\'" << strZbCode << "\' ";
		sql << ss.str();

	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << '\r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}



CStockPlateTree * CStatsGovCn::GenerateTree(string strDbCode)
{

	stringstream ss;

	tree_id.clear();
	tree_pid.clear();
	tree_isParent.clear();
	tree_name.clear();

	try
	{
		ss << "CStatsGovCn 开始从 MYSQL 获取数据......";
		ss << "【" << m_nCode << "】";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select dbcode , id , isParent,  name ,  pid  , wdcode , updateDate  " 
			<< " from statsgovtree where dbcode = \'"<< strDbCode << "\' order by isParent desc , id asc " ;	

		string test = ss.str();

		row r;
		//session sql(backEnd, m_ConnectString);
		session sql(g_MysqlPool);

		statement st = (sql.prepare << ss.str(),into(r)) ;
		st.execute();

		while (st.fetch())
		{
			tree_id.insert(tree_id.end(), r.get<string>("id"));
			tree_isParent.insert(tree_isParent.end(), r.get<int>("isParent"));
			tree_name.insert(tree_name.end(), /*Utf8_GBK*/(r.get<string>("name")));
			tree_pid.insert(tree_pid.end(), r.get<string>("pid").empty()? "-1" : r.get<string>("pid"));
			
		}

		ss.str("");
		ss << "OK, 从 MYSQL 获取数据完毕.";
		ss << " 共 " << tree_id.size() << "条 \r\n" ;
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}

	CStockPlateTree * pTree = new CStockPlateTree;

	CStockPlateTree * pNode = pTree;
	for(int i=0; i<tree_id.size(); i++)
	{
		CStockPlateData treeNode;
		treeNode.code = tree_id[i];
		treeNode.name = tree_name[i];
		treeNode.company_number = 1;
		treeNode.is_stock = tree_isParent[i] ? 0 : 1;

		CStockPlateData treeNodeParent;
		treeNodeParent.code = tree_pid[i];
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


int CStatsGovCn::HeaderOfStatsGov( char *data, size_t size, size_t nmemb, CStatsGovCn * pStatsGovCn)
{
	unsigned long sizes = size * nmemb;
	memcpy(pStatsGovCn->m_header + pStatsGovCn->m_size_header, data, sizes);
	pStatsGovCn->m_size_header += sizes;
	return sizes;
}


int CStatsGovCn::WriterOfStatsGov(char *data, size_t size, size_t nmemb, CStatsGovCn * pStatsGovCn)
{
	unsigned long sizes = size * nmemb;
	
	if(pStatsGovCn->m_size + sizes >= pStatsGovCn->m_capacity)
	{
		pStatsGovCn->m_capacity = pStatsGovCn->m_capacity + sizes + 500 * 1024;
		unsigned char * pTmp = new unsigned char[pStatsGovCn->m_capacity];
		memset(pTmp, 0 , pStatsGovCn->m_capacity);
		memcpy(pTmp, pStatsGovCn->m_buffer, pStatsGovCn->m_size);
		delete[] pStatsGovCn->m_buffer;
		pStatsGovCn->m_buffer = pTmp;
	}

	memcpy(pStatsGovCn->m_buffer + pStatsGovCn->m_size, data, sizes);
	pStatsGovCn->m_size += sizes;

	if(pStatsGovCn->m_size - pStatsGovCn->m_size_log > 200 * 1024)
	{
		stringstream ss;
		ss.str("");
		ss << "收到 ";
		ss << pStatsGovCn->m_size;
		ss << " 字节数据, 当前接收缓存取为 ";
		ss << pStatsGovCn->m_capacity;
		ss << " 字节\r\n";
		sendToOutput(ss.str().c_str(), pStatsGovCn->m_hWnd, pStatsGovCn->m_nLogType);

		pStatsGovCn->m_size_log = pStatsGovCn->m_size;

	}

	return sizes;
}


string CStatsGovCn::PostUrlOfStatsGov(string url,char* data)
{

	CURL *conn = NULL;
	CURLcode code;
	string logstr;

	//	清理接收缓存
	m_size_log = 0;
	m_size = 0;
	memset(m_buffer, 0, m_capacity);
	m_size_header = 0;
	memset(m_header, 0 ,1024);

	string HqJs = "";
	char errorBuffer[CURL_ERROR_SIZE];

	do{
		conn = curl_easy_init();

		if(conn==NULL) break;

		code = curl_easy_setopt(conn, CURLOPT_ERRORBUFFER, errorBuffer);
		if (code != CURLE_OK) break;

		curl_easy_setopt(conn,CURLOPT_CONNECTTIMEOUT_MS, 2000);
		curl_easy_setopt(conn,CURLOPT_TIMEOUT_MS, 600000);  // 10 分钟

		code = curl_easy_setopt(conn, CURLOPT_URL, url.c_str());
		if (code != CURLE_OK) break;

		code = curl_easy_setopt(conn, CURLOPT_FOLLOWLOCATION, 1);
		if (code != CURLE_OK) break;

		code = curl_easy_setopt(conn, CURLOPT_WRITEFUNCTION, CStatsGovCn::WriterOfStatsGov);
		if (code != CURLE_OK) break;

		code = curl_easy_setopt(conn, CURLOPT_WRITEDATA, this);
		if (code != CURLE_OK) break;

		code = curl_easy_setopt(conn, CURLOPT_HEADERFUNCTION,CStatsGovCn::HeaderOfStatsGov);
		if (code != CURLE_OK) break;

		code = curl_easy_setopt(conn, CURLOPT_HEADERDATA, this);
		if (code != CURLE_OK) break;

		if(0!=data){
			curl_easy_setopt(conn, CURLOPT_POSTFIELDS,data);
		}

		curl_easy_setopt(conn, CURLOPT_REFERER,"http://www.stats.gov.cn/tjfw/wsbs/");
		curl_easy_setopt(conn, CURLOPT_ACCEPT_ENCODING, "gzip, deflate");

		curl_easy_setopt(conn, CURLOPT_COOKIE, "_gscu_1771678062=71335754gr3yqe50; _trs_uv=aq3u_6_irx7bieo; _gscs_1771678062=713398504yihzx16|pv:2; _gscbrs_1771678062=1; JSESSIONID=7DCCB2C566A780B2DB2B5AE113F38EAA; u=2");

		code = curl_easy_perform(conn); 
		curl_easy_reset(conn);
		curl_easy_cleanup(conn);

		if(m_capacity >10 * 1024 * 1024)
		{
			int kkk = 1;
		}

		if (code != CURLE_OK) //CUrlUtils::ThrowError(errorBuffer);
			break;

		//	解析http 回复头, 如果包含头 Content-Length 则说明不是通过 deflate 算法压缩的
		//	如果 包含 “Transfer-Encoding: chunked”，但是不包含 Content-Encoding: gzip，说明时分段但不压缩

		if(m_size_header<=0)
			break;

		string strHeader((char *)m_header);
		if(strHeader.find("Content-Length")!=string::npos ||
			strHeader.find("Content-Encoding: gzip")==string::npos )
		{
			
			HqJs = Utf8_GBK_((char *)m_buffer);
			if(HqJs=="ConvertEnc Convert Failed")
				HqJs =  Utf8_GBK_((char *)m_buffer);

			stringstream ss;
			ss.str("");
			ss << "总共收到 ";
			ss << m_size;
			ss << " 字节数据, 当前接收缓存取为 ";
			ss << m_capacity;
			ss << " 字节\r\n";
			sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		}
	}while(0);

	return HqJs;

}

