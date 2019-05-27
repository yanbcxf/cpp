#include "StdAfx.h"
#include "CsindexStockPlates.h"

CCsindexStockPlates::CCsindexStockPlates(void)
{
	m_pTree = new CStockPlateTree;
}

CCsindexStockPlates::~CCsindexStockPlates(void)
{
	if(m_pTree!=NULL)
		delete m_pTree;
}


string CCsindexStockPlates::SqlScript()
{
	stringstream ss;
	ss << "create table if not exists CsindexStockPlates (Code int , Name varchar(256), Company_Number int, "
		<< " Parent_Code int, Is_Stock int,  PRIMARY KEY ( Code, Is_Stock))";
	return ss.str();
}


/*
void CCsindexStockPlates::UpdateFromXls(string strCode, string name, int company_number, int is_stock)
{	

	if(strCode.length()!=2 && strCode.length()!=4 && strCode.length()!=6 && strCode.length()!=8)
		return;

	if(m_pTree==NULL)
		m_pTree = new CStockPlateTree;

	CStockPlateTree * pNode = NULL;
	CStockPlateData treeNode;
	treeNode.code = atoi(strCode.c_str());
	treeNode.name = name;
	treeNode.company_number = company_number;
	treeNode.is_stock = is_stock;

	CStockPlateData treeNodeParent;
	if(strCode.length()==4)
		treeNodeParent.code = atoi(strCode.substr(0,2).c_str());
	else if(strCode.length()==6)
		treeNodeParent.code = atoi(strCode.substr(0,4).c_str());
	else if(strCode.length()==8)
		treeNodeParent.code = atoi(strCode.substr(0,6).c_str());
	else
		treeNodeParent.code = "-1";
	treeNodeParent.is_stock = 0;

	pNode = m_pTree->SearchSpecialNode(treeNodeParent);
	if(pNode)
	{
		pNode->insert(treeNode);
	}
	else
	{
		m_pTree->insert(treeNode);
	}
}
*/


void CCsindexStockPlates::ExportFromDatabase(double startTime, double endTime)
{
	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();

	map<int, int>	vec_code;
	map<int,string>	vec_name;
	map<int, int>	vec_company_number;
	map<int, int>	vec_parent_code;
	map<int, int>	vec_is_stock;	

	try
	{
		ss << "CsindexStockPlates 开始从 MYSQL 获取数据......";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select Code, Name, Company_Number, Parent_Code, Is_Stock"
			<< " from CsindexStockPlates order by Is_Stock, Code" ;

		row r;
		//session sql(backEnd, m_ConnectString);
		session sql(g_MysqlPool);

		statement st = (sql.prepare << ss.str(),into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{
			vec_code[cnt] = r.get<int>("Code");
			vec_name[cnt] = /*Utf8_GBK*/(r.get<string>("Name"));
			vec_company_number[cnt] = r.get<int>("Company_Number");
			vec_parent_code[cnt] = r.get<int>("Parent_Code");
			vec_is_stock[cnt] = r.get<int>("Is_Stock");

			cnt++;
		}

		if(m_pTree!=NULL){
			delete m_pTree;
		}
		m_pTree = new CStockPlateTree;

		CStockPlateTree * pNode = NULL;
		for(int i=0; i<vec_code.size(); i++)
		{
			CStockPlateData treeNode;
			if(vec_is_stock[i])
				treeNode.code = Int2String(vec_code[i], "%06d");
			else
				treeNode.code = Int2String(vec_code[i], "%08d");
			treeNode.name = vec_name[i];
			treeNode.company_number = vec_company_number[i];
			treeNode.is_stock = vec_is_stock[i];

			CStockPlateData treeNodeParent;
			treeNodeParent.code = Int2String(vec_parent_code[i], "%08d");
			treeNodeParent.is_stock = 0;

			if(pNode)
			{
				CStockPlateData spd = *pNode->get();
				if(spd.code != treeNodeParent.code || spd.is_stock!=0)
					pNode = NULL;
			}

			if(pNode==NULL)
				pNode = m_pTree->SearchSpecialNode(treeNodeParent);
			if(pNode)
			{
				pNode->insert(treeNode);
			}
			else
			{
				m_pTree->insert(treeNode);
			}
			
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



void CCsindexStockPlates::ImportToDatabase(void)
{
	//	先序遍历，生成
	vector<CStockPlateData>  vec_stockplate;
	CStockPlateTree::pre_order_iterator pre_order_it = m_pTree->pre_order_begin();
	while(pre_order_it!=m_pTree->pre_order_end())
	{
		CStockPlateData spd = *pre_order_it;

		if(pre_order_it.node()->parent()->is_root()==false)
		{
			spd.parent_code = pre_order_it.node()->parent()->get()->code;
		}
		else
		{
			spd.parent_code = "-1";
		}
		vec_stockplate.push_back(spd);
		pre_order_it++;
	}

	if( vec_stockplate.size()<=0)
		return;

	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();

	try
	{
		ss << "CsindexStockPlates::ImportToDatabase 开始导入数据.....\r\n";
		//sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		session sql(g_MysqlPool);

		// 删除“过时”的板块分组
		ss.str("");
		ss << "delete  from CsindexStockPlates where (Code, Parent_Code) not in( ";
		for (int row = 0; row < vec_stockplate.size(); row++)
		{
			ss << "(" << vec_stockplate[row].code << "," << vec_stockplate[row].parent_code << ")";
			if(row < vec_stockplate.size()-1)
				ss << ",";
		}
		ss << ")";
		string test1 = ss.str();
		sql << test1; 

		for (int row = 0; row < vec_stockplate.size(); row++)
		{
			ss.str("");
			ss << "insert into CsindexStockPlates "
				<< " select " <<  vec_stockplate[row].code << ", \'" 
				<< vec_stockplate[row].name << "\', "
				<< Int2String(vec_stockplate[row].company_number) << ", "
				<< vec_stockplate[row].parent_code << ", "
				<< Int2String(vec_stockplate[row].is_stock) << " "

				<< " from dual where not exists (SELECT 1 from CsindexStockPlates " 
				<< " where Parent_Code=" << vec_stockplate[row].parent_code << " " 
				<< " and Code=" << vec_stockplate[row].code << "  ) ";

			string test = ss.str();
			//test = Gbk2Utf8(ss.str());
			sql << test; 

			ss.str("");
			ss << "update CsindexStockPlates "
				<< " set Code=" <<  vec_stockplate[row].code << ", Name=\'" 
				<< vec_stockplate[row].name << "\', Company_Number="
				<< Int2String(vec_stockplate[row].company_number)  
				<< ", Parent_Code="
				<< vec_stockplate[row].parent_code << ", Is_Stock="
				<< Int2String(vec_stockplate[row].is_stock) << " "

				<< " where Parent_Code=" << vec_stockplate[row].parent_code << " " 
				<< " and Code=" << vec_stockplate[row].code << "  ";

			//test = Gbk2Utf8(ss.str());
			sql << test; 
		}
		ss.str("");
		ss << "共有数据 " <<  vec_stockplate.size() ;
		ss << " OK, CsindexStockPlates::ImportToDatabase 数据导入完毕. \r\n";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << '\r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}