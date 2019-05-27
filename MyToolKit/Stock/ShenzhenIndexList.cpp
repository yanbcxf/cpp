#include "StdAfx.h"
#include "ShenzhenIndexList.h"
#include "UrlUtils.h"

#include "BasicExcel.hpp"
using namespace YExcel;

CShenzhenIndexList::CShenzhenIndexList(void)
{
	m_pTree = new CStockPlateTree;
}

CShenzhenIndexList::~CShenzhenIndexList(void)
{
	if(m_pTree!=NULL)
		delete m_pTree;
}

bool CShenzhenIndexList::ParseXls(string  xlsfilename)
{
	BasicExcel e;

	e.Load(xlsfilename.c_str());	
	BasicExcelWorksheet* sheet1 = e.GetWorksheet(L"指数列表");
	if (sheet1)
	{
		size_t maxRows = sheet1->GetTotalRows();
		size_t maxCols = sheet1->GetTotalCols();

		for (size_t r=0; r<maxRows; ++r)
		{
			for (size_t c=0; c<maxCols; ++c)
			{
				char tmp[256] = { 0 };
				BasicExcelCell* cell = sheet1->Cell(r,c);
				switch (cell->Type())
				{
				case BasicExcelCell::UNDEFINED:
					break;

				case BasicExcelCell::INT:
					sprintf_s(tmp, 256, "%10d", cell->GetInteger());
					break;

				case BasicExcelCell::DOUBLE:
					sprintf_s(tmp, 256, "%10.6lf", cell->GetDouble());
					break;

				case BasicExcelCell::STRING:
					sprintf_s(tmp, 256, "%10s", cell->GetString());
					if(r>0)
					{
						string strTmp = trimString(string(tmp));
						strTmp = ReplaceString(strTmp, ",", "");
						if(c==0)	
						{
							m_vec_code.insert(m_vec_code.end(), strTmp);
						}
						else if(c==1)	m_vec_name.insert(m_vec_name.end(),strTmp);
						else if(c==2)	m_vec_base_date.insert(m_vec_base_date.end(), /*String2Double*/(strTmp));
						else if(c==3)	m_vec_base_point.insert(m_vec_base_point.end(), String2Double(strTmp));
						else if(c==4)	m_vec_initial_date.insert(m_vec_initial_date.end(), strTmp);
					}
					break;

				case BasicExcelCell::WSTRING:
					//wprintf(L"%10s", cell->GetWString());
					CString str1(cell->GetWString());	
					if(r>0)
					{
						if(c==0)	
						{
							string strCode = str1.GetBuffer();
							m_vec_code.insert(m_vec_code.end(), strCode);
						}
						else if(c==1)	m_vec_name.insert(m_vec_name.end(), str1.GetBuffer());
						else if(c==2)	m_vec_base_date.insert(m_vec_base_date.end(), str1.GetBuffer());
						else if(c==3)	m_vec_base_point.insert(m_vec_base_point.end(), String2Double(str1.GetBuffer()));
						else if(c==4)	m_vec_initial_date.insert(m_vec_initial_date.end(), str1.GetBuffer());
						/*else if(c==2)	m_vec_total_shares.insert(m_vec_total_shares.end(), String2Double(str1.GetBuffer()));
						else if(c==3)	m_vec_circulating.insert(m_vec_circulating.end(), String2Double(str1.GetBuffer()));
						else if(c==4)	m_vec_industry.insert(m_vec_industry.end(), string(str1.GetBuffer()));*/
					}
					break;
				}
			}
		}
	}

	if(m_vec_name.size()>0 &&
		m_vec_name.size() ==m_vec_code.size()/*&&
		m_vec_name.size() ==m_vec_total_shares.size()&&
		m_vec_name.size() ==m_vec_circulating.size()&&
		m_vec_name.size() ==m_vec_industry.size()*/)
		return true;

	return false;
}

string  CShenzhenIndexList::SaveAsCsv()
{
	stringstream ss;

	int xlsLen = 0;
	unsigned char * pXls = NULL;
	
	char tmpCode[256] = {0};
	sprintf(tmpCode, "http://www.szse.cn/szseWeb/ShowReport.szse?SHOWTYPE=xls&CATALOGID=1812&tab1PAGENUM=1&ENCODE=1&TABKEY=tab1");

	string test = string(tmpCode);
	xlsLen =CUrlUtils::PostUrlOfCsindex(tmpCode,pXls);
	if(xlsLen>0)
	{
		CString target;
		target.Format("%s/%s/%s.xls",g_strCurrentDir.c_str(), "IndexInfo", "shenzhen_index_list");
		string filename = target.GetBuffer();

		FILE * fp_zip=fopen(filename.c_str(),"wb");
		if(fp_zip)
		{
			fwrite(pXls,1,xlsLen ,fp_zip);
			fclose(fp_zip);
		}
		//free(pZip);
		delete[] pXls;

		if(ParseXls(filename))
			return "AlreadyInMemory";

	}
	
	return "";
}



void CShenzhenIndexList::GenerateTree()
{
	stringstream ss;

	try
	{
		ss << "CShenzhenIndexList 开始从 xls 获取数据......";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//////////////////////////////////////////////////////////////////////////
		ss.str("");
	

		if(m_pTree!=NULL){
			delete m_pTree;
		}
		m_pTree = new CStockPlateTree;
	

		for(int i=0; i<m_vec_code.size(); i++)
		{
			CStockPlateData treeNode;
			treeNode.code = "00" + m_vec_code[i];
			treeNode.name = m_vec_name[i];
			treeNode.company_number = 1;
			treeNode.is_stock = 0;

			m_pTree->insert(treeNode);
		}

		int cnt = 0;
		CStockPlateTree::iterator it;   // 这是一个 child iterator
		CStockPlateTree * pNode = NULL;
		for(int i=0; i<m_vec_code.size(); i++)
		{
			//	这些 股指不需要读入它们的 样本股
			if(m_vec_code[i]=="399100" || m_vec_code[i]=="399106" || m_vec_code[i] == "399107")
				continue;

			CString target;
			target.Format("%s/%s/shenzhen_%s.xls",g_strCurrentDir.c_str(), "IndexInfo", m_vec_code[i].c_str());
			string filename = target.GetBuffer();

			CStockPlateData treeNodeParent;
			treeNodeParent.code = "00" + m_vec_code[i];
			treeNodeParent.is_stock = 0;

			CShenzhenIndexYangben yangben;
			if(yangben.ParseXls(filename))
			{
				for(int k = 0 ; k< yangben.m_vec_code.size(); k++)
				{
					CStockPlateData treeNode;
					treeNode.code = yangben.m_vec_code[k];
					treeNode.name = yangben.m_vec_name[k];
					treeNode.company_number = 1;
					treeNode.is_stock = 1;

					if(pNode)
					{
						CStockPlateData spd = *pNode->get();
						if(spd.code != treeNodeParent.code || spd.is_stock!=0)
							pNode = NULL;
					}

					if(pNode==NULL)
						pNode = m_pTree->SearchSpecialNode(treeNodeParent);
					if(pNode)
						pNode->insert(treeNode);
					else
						m_pTree->insert(treeNode);

					cnt++;
				}
			}
		}

		ss.str("");
		ss << "OK, 从 xls 获取数据完毕.";
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