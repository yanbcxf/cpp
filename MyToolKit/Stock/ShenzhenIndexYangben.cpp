#include "StdAfx.h"
#include "ShenzhenIndexYangben.h"
#include "UrlUtils.h"

#include "BasicExcel.hpp"
using namespace YExcel;

CShenzhenIndexYangben::CShenzhenIndexYangben(void)
{
}

CShenzhenIndexYangben::~CShenzhenIndexYangben(void)
{
}

bool CShenzhenIndexYangben::ParseXls(string  xlsfilename)
{
	BasicExcel e;

	e.Load(xlsfilename.c_str());	
	BasicExcelWorksheet* sheet1 = e.GetWorksheet(L"指数样本股");
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
						else if(c==2)	m_vec_total_shares.insert(m_vec_total_shares.end(), String2Double(strTmp.c_str()));
						else if(c==3)	m_vec_circulating.insert(m_vec_circulating.end(), String2Double(strTmp.c_str()));
						else if(c==4)	m_vec_industry.insert(m_vec_industry.end(), strTmp);
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
						else if(c==2)	m_vec_total_shares.insert(m_vec_total_shares.end(), String2Double(str1.GetBuffer()));
						else if(c==3)	m_vec_circulating.insert(m_vec_circulating.end(), String2Double(str1.GetBuffer()));
						else if(c==4)	m_vec_industry.insert(m_vec_industry.end(), string(str1.GetBuffer()));
					}
					break;
				}
			}
		}
	}

	if(m_vec_name.size()>0 &&
		m_vec_name.size() ==m_vec_code.size()&&
		m_vec_name.size() ==m_vec_total_shares.size()&&
		m_vec_name.size() ==m_vec_circulating.size()&&
		m_vec_name.size() ==m_vec_industry.size())
		return true;

	return false;
}

string  CShenzhenIndexYangben::SaveAsCsv()
{
	stringstream ss;

	int xlsLen = 0;
	unsigned char * pXls = NULL;

	CString target;
	target.Format("%s/%s/shenzhen_%6d.xls",g_strCurrentDir.c_str(), "IndexInfo", m_nCode);
	string filename = target.GetBuffer();
	long filesize = GetDownloadFileSize(filename);
	
	char tmpCode[256] = {0};
	sprintf(tmpCode, "http://www.szse.cn/szseWeb/ShowReport.szse?SHOWTYPE=xls&CATALOGID=1747&ZSDM=%6d&ENCODE=1&TABKEY=tab1", m_nCode);

	string test = string(tmpCode);
	xlsLen =CUrlUtils::PostUrlOfCsindex(tmpCode,pXls);
	if(xlsLen>0)
	{
		if(filesize == xlsLen)
		{	

			delete[] pXls;
			return "NoMoreData";
		}
		else
		{
			FILE * fp_zip=fopen(filename.c_str(),"wb");
			if(fp_zip)
			{
				fwrite(pXls,1,xlsLen ,fp_zip);
				fclose(fp_zip);
			}
			delete[] pXls;

			ss.str("");
			ss << "OK, CShenzhenIndexYangben [" << m_nCode ;
			ss << "] xls 保存完毕.\r\n";
			sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
			// if(ParseXls(filename))
			return "AlreadyInMemory";
		}
	}
	
	return "";
}