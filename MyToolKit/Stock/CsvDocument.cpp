// CsvDocument.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "CsvDocument.h"




// CCsvDocument

IMPLEMENT_DYNCREATE(CCsvDocument, CDocument)

CCsvDocument::CCsvDocument()
{
	m_nCurrStockData = -1;
	m_vecStockDataModel.push_back(new CIncomeStatement);
	m_vecStockDataModel.push_back(new CTradingDaily);
	m_vecStockDataModel.push_back(new CNetEaseTradeDaily);

	m_vecStockDataModel.push_back(new CBalance);
	m_vecStockDataModel.push_back(new CCashFlows);
	m_vecStockDataModel.push_back(new CEntrepreneurship);
	m_vecStockDataModel.push_back(new CSmallMidEnterprise);
	m_vecStockDataModel.push_back(new CShenzhenAshares);
	m_vecStockDataModel.push_back(new CCapitalStructure);
	m_vecStockDataModel.push_back(new CShanghaiStockInfo);
	m_vecStockDataModel.push_back(new CCapitalFlows163);
	m_vecStockDataModel.push_back(new CSohuPerformanceForecast);
	m_vecStockDataModel.push_back(new CSohuMainIncomeComposition);
	m_vecStockDataModel.push_back(new CSohuFundHold);
}

BOOL CCsvDocument::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CCsvDocument::~CCsvDocument()
{
	for(int i=0; i<m_vecStockDataModel.size(); i++)
	{
		delete m_vecStockDataModel[i];
		m_vecStockDataModel[i] = NULL;
	}
}


BEGIN_MESSAGE_MAP(CCsvDocument, CDocument)
	ON_COMMAND(ID_CSV_OPEN, &CCsvDocument::OnCsvOpen)
	ON_COMMAND(ID_CSV_IMPORT, &CCsvDocument::OnCsvImport)
END_MESSAGE_MAP()


// CCsvDocument 诊断

#ifdef _DEBUG
void CCsvDocument::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CCsvDocument::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CCsvDocument 序列化

void CCsvDocument::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}
#endif


// CCsvDocument 命令

void CCsvDocument::OnCsvOpen()
{
	// TODO: 在此添加命令处理程序代码

	TCHAR szFileName[_MAX_PATH] = { 0 };
	OPENFILENAMEA ofn = {0};
	LPCTSTR szFilter = "Comma separated values (*.CSV)\0*.CSV\0All files (*.*)\0*.*\0", defExt = "CSV";
	static TCHAR szDir[_MAX_DIR] = { 0 };
	ofn.lStructSize		= sizeof(OPENFILENAME);
	ofn.hwndOwner		= theApp.GetMainWnd()->m_hWnd;
	ofn.hInstance		= NULL; //theApp.getInstance();
	ofn.lpstrFilter		= szFilter;
	ofn.lpstrFile		= szFileName;
	ofn.nMaxFile		= _MAX_PATH;
	ofn.Flags			= OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_EXPLORER | OFN_NOCHANGEDIR /*不改变当前目录*/;
	ofn.lpstrDefExt		= defExt;

	if(GetOpenFileNameA(&ofn) == 0)
		return ;

		
	//	从文件名称中获取“股票编码”
	int k = 0;
	for(; k<m_vecStockDataModel.size(); k++)
	{
		if(m_vecStockDataModel[k]->ParseCsvFileName(szFileName))
		{
			CString str;
			str.Format("CSV 文件名称解析成功，【股票代码 %d 】\"%s\"", \
				m_vecStockDataModel[k]->m_nCode, \
				m_vecStockDataModel[k]->ModelType().c_str());
			AfxMessageBox(str);
			break;
		}
	}
	if(k== m_vecStockDataModel.size())
	{
		AfxMessageBox("CSV 文件类型不支持");
		return;
	}

	//	保存当前从文件名称中获取的数据类型
	m_nCurrStockData = k;

	//	读入 CSV 文件到内存
	CCSVFile csvFile(szFileName);
	csvFile.ReadAllData(m_arrayHeader, m_arrayData);
	csvFile.Close();
	
	UpdateAllViews(NULL);

	return;
}

//	导入到数据库
UINT CsvImportThreadFunc(LPVOID param)
{
	stringstream ss;
	CStockDataModel * pModel = (CStockDataModel *)param;

	pModel->ImportToDatabase();
	delete pModel;
	return 0;
}


void CCsvDocument::OnCsvImport()
{
	// TODO: 在此添加命令处理程序代码

	int k = 0;
	if(m_nCurrStockData >=0 && m_nCurrStockData < m_vecStockDataModel.size())
	{
		if(m_vecStockDataModel[m_nCurrStockData]->CheckReport(m_arrayHeader, m_arrayData)>=0)
		{
			CString str;
			str.Format("CSV 文件中内容符合 \"%s\"，确定开始导入数据库", \
				m_vecStockDataModel[m_nCurrStockData]->ModelType().c_str());
			AfxMessageBox(str);
			m_vecStockDataModel[m_nCurrStockData]->ParseCsvFile(m_arrayHeader, m_arrayData);
			m_vecStockDataModel[m_nCurrStockData]->ImportToDatabase();
			
		}
		else
			AfxMessageBox("CSV 文件内容不匹配");
		
	}
	else
	{
		AfxMessageBox("CSV 文件类型未知");
	}

}
