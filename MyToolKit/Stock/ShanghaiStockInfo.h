#pragma once

class StockListInfo
{
public:
	string areadesc;
	string companycode;
	string companyname;
	string securitycodea;
	string securitycodeb;
	string ssecode;
	string ssename;
};

class StockCompanyDesc
{
public:
	string area_name_desc;
	string changeable_bond_abbr;
	string changeable_bond_code;
	string company_abbr;
	string company_address;
	string company_code;
	string csrc_code_desc;
	string csrc_great_code_desc;
	string csrc_middle_code_desc;
	string english_abbr;
	string e_mail_address;
	string foreign_listing_address;
	string foreign_listing_desc;
	string fullname;
	string full_name_in_english;
	string legal_representative;
	string office_address;
	string office_zip;
	string repr_phone;
	string security_30_desc;
	string security_abbr_a;
	string security_code_a;
	string security_code_a_sz;
	string security_code_b;
	string sse_code_desc;
	string state_code_a_desc;
	string state_code_b_desc;
	string www_address;

};

class CShanghaiStockInfo : public CShenzhenStockInfo
{
public:
	~CShanghaiStockInfo(void);

	CShanghaiStockInfo()
	{ 
		m_stock_plate="上海证券板";
		m_securities_exchange = "上海证券交易所";
	}

	string GetDownloadUrl();
	bool ParseCsvFileName(string szFileName);
	CStockDataModel * NewCopy();
	string SaveAsCsv();
	string SaveAsCsvOld();
	string GetCsvFileName();

	string ModelType(){ return  "CShanghaiStockInfo" /*"上海证券交易所股票基础信息"*/; }
private:
	void SearchStockByAreaName(vector<StockListInfo> & vecStock);
};
