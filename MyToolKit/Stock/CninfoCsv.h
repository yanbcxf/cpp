#pragma once
#include "stockdatamodel.h"

class CCninfoCsv : public CStockDataModel
{
public:
	CCninfoCsv(void);
	~CCninfoCsv(void);

public:
	virtual string ModelType(){ return  "CCninfoCsv" /* 父类  */; }

	int  CheckReport(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData){ return 0;};
	void ParseCsvFile(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData){};
	bool ParseCsvFileName(string szFileName){ return true; };
	void ParseXmlElement(void){};
	CStockDataModel * NewCopy() { return NULL; }

	void ImportToDatabase(void);
	string Yield2Key(string strYield);
	void ExportFromDatabase(double startTime = 0, double endTime = 0);

	string GetDownloadUrl(){ return ""; };
	string SaveAsCsv();
	string SqlScript();
	virtual string GetZipUrl() = 0;

	string GetCsvFileName(){ return ""; };
	void NotifyDatabaseImportedToWindow(){};

	bool Unzip(string zipfilename );
	void KaptchaAsJpeg();
	void Kaptcha();

public:
	vector<pair<string, string>>	m_vec_yield;	//	用来保证 标题头能够按照 插入顺序排序
	map<string, string>		m_map_yield;	//	保存 文件标题头 与 数据库 域的映射关系

	vector<map<string, string>>	m_vec_row;

	string	m_strEnglishName;
};

//////////////////////////////////////////////////////////////////////////

class CCninfoIncomeStatement : public CCninfoCsv
{
public:
	CCninfoIncomeStatement();

	virtual string ModelType(){ return  "CCninfoIncomeStatement" /* 利润表  */; }
	string GetZipUrl();

	//	估算全年的利润
	double EstimateYearProfits();

	//	某日归属母公司所有者的净利润
	double NetProfitBelongingToParentCompany(string currDate, string & baseDate, bool & isLast);
};


class CCninfoBalance : public CCninfoCsv
{
public:
	CCninfoBalance();

	virtual string ModelType(){ return  "CCninfoBalance" /* 资产负债表  */; }
	string GetZipUrl();
};


class CCninfoCashFlows : public CCninfoCsv
{
public:
	CCninfoCashFlows();

	virtual string ModelType(){ return  "CCninfoCashFlows" /* 现金流量表  */; }
	string GetZipUrl();
};