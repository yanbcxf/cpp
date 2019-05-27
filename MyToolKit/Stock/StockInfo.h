#pragma once
#include "stockdatamodel.h"


class CStockInfoData
{
public:
	int					code			;	//	"公司代码",
	string				abbreviation			;	//	"公司简称",
	string				name			;	//	"公司全称",
	string				a_abbreviation			;	//	"A股简称",
	string				a_time_to_market			;	//	"A股上市日期",
	long				a_total_share			;	//	"A股总股本",
	long				a_circulation_share			;	//	"A股流通股本",
	string				area			;	//	"地 区",
	string				province			;	//	"省 份",
	string				city			;	//	"城 市",
	string				belong_to_industry 			;	//	"所属行业",
	string				web_addr			;	//	"公司网址"

	string				stock_plate;				//	股票板块： 创业版、中小版
	string				securities_exchange;		//	证交所

	string				cninfo_orgid;		//	巨潮网

	bool operator == (const CStockInfoData &otherData)
	{
		if(code==otherData.code )
			return true;
		else
			return false;
	}
};

class CStockInfo :
	public CStockDataModel
{
public:
	CStockInfo(void){  m_CStockInfoDataMap.clear(); };
	virtual ~CStockInfo(void);

public:
	map<int, int>		m_vec_code			;	//	"公司代码",
	map<int, string>	m_vec_abbreviation			;	//	"公司简称",
	map<int, string>	m_vec_name			;	//	"公司全称",
	map<int, string>	m_vec_english_name			;	//	"英文名称",
	map<int, string>	m_vec_register_addr			;	//	"注册地址",
	map<int, int>		m_vec_a_code			;	//	"A股代码",
	map<int, string>	m_vec_a_abbreviation			;	//	"A股简称",
	map<int, string>	m_vec_a_time_to_market			;	//	"A股上市日期",
	map<int, long>		m_vec_a_total_share			;	//	"A股总股本",
	map<int, long>		m_vec_a_circulation_share			;	//	"A股流通股本",
	map<int, int>		m_vec_b_code			;	//	"B股代码",
	map<int, string>	m_vec_b_abbreviation			;	//	"B股 简 称",
	map<int, string>	m_vec_b_time_to_market			;	//	"B股上市日期",
	map<int, long>		m_vec_b_total_share			;	//	"B股总股本",
	map<int, long>		m_vec_b_circulation_share			;	//	"B股流通股本",
	map<int, string>	m_vec_area			;	//	"地 区",
	map<int, string>	m_vec_province			;	//	"省 份",
	map<int, string>	m_vec_city			;	//	"城 市",
	map<int, string>	m_vec_belong_to_industry 			;	//	"所属行业",
	map<int, string>	m_vec_web_addr			;	//	"公司网址"

	map<int, string>	m_vec_cninfo_orgid;			//	巨潮网
	map<int, string>	m_vec_cninfo_pinyin;		//	巨潮网

	string	m_stock_plate;				//	股票板块： 创业版、中小版
	string  m_securities_exchange;		//	证交所

	map<int, CStockInfoData> m_CStockInfoDataMap;

public:
	
	string SqlScript();

	int	 SearchStockByCode(int code);

	void GenerateMap();

	/*virtual string GetDownloadUrl() = 0;
	virtual bool ParseCsvFileName(string szFileName) = 0;*/

};

class CShenzhenStockInfo : public CStockInfo
{
public:
	CShenzhenStockInfo(void);
	~CShenzhenStockInfo(void);

public:
	int  CheckReport(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData);
	void ParseCsvFile(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData);
	void ParseXmlElement(void);
	void ImportToDatabase(void);
	void ExportFromDatabase(double startTime = 0, double endTime = 0);
	string SaveAsCsv();

	bool ParseCsvFileName(string szFileName);
	string GetCsvFileName();

protected:
	string	m_strEnglishName;
};


//////////////////////////////////////////////////////////////////////////
//	创业板
class CEntrepreneurship : public CShenzhenStockInfo
{
public:
	CEntrepreneurship()
	{ 
		m_stock_plate="创业板";
		m_securities_exchange = "深圳证券交易所";
		m_strEnglishName = "Entrepreneurship";
	}

	string GetDownloadUrl();
	
	CStockDataModel * NewCopy();

	string ModelType(){ return "CEntrepreneurship" /*"创业板股票基础信息"*/; }
};

//////////////////////////////////////////////////////////////////////////
//	中小板


 class CSmallMidEnterprise : public CShenzhenStockInfo
 {
 public:
	 CSmallMidEnterprise()
	 { 
		 m_stock_plate="中小企业板";
		 m_securities_exchange = "深圳证券交易所";
		 m_strEnglishName = "SmallMidEnterprise";
	 }

	 string GetDownloadUrl();
	 CStockDataModel * NewCopy();

	 string ModelType(){ return "CSmallMidEnterprise"  /*"中小企业板股票基础信息"*/; }
 };

 //////////////////////////////////////////////////////////////////////////
 //	深圳A股


 class CShenzhenAshares : public CShenzhenStockInfo
 {
 public:
	 CShenzhenAshares()
	 { 
		 m_stock_plate="深圳A股";
		 m_securities_exchange = "深圳证券交易所";
		 m_strEnglishName = "ShenzhenAshares";
	 }

	 string GetDownloadUrl();
	 CStockDataModel * NewCopy();

	 string ModelType(){ return  "CShenzhenAshares"  /*"深圳A股基础信息"*/; }
 };