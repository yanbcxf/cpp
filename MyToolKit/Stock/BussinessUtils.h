#pragma once

class CBussinessUtils
{
public:
	CBussinessUtils(void);
	~CBussinessUtils(void);
	static double EstimateYearProfits(int nCode);
	static double CurrentATotalShares(int nCode, string curDate="");
	static double CurrentClosePrice(int nCode);
	static double CurrentACalculateshares(int nCode, string curDate="");
	static time_t CurrentTradingDate(int nDay = 0);
	static double NetProfitBelongingToParentCompanyFromIncome(int nCode, string curDate,  string & baseDate,  bool & isLast);
	static double NetProfitBelongingToParentCompanyFromForecast(int nCode, string curDate,  string & baseDate);

	static void GetIncomestatement(int code, CCninfoIncomeStatement & income);
	static void GetBalance(int code, CCninfoBalance & balance);
	static void GetCashFlow(int code, CCninfoCashFlows & cash);
	static void GetFundHold(int code, CSohuFundHold & fund);
	static void GetCapitalFlows163(int code, CCapitalFlows163 & fund);
	static void GetSohuTradingDaily(int code, CNetEaseTradeDaily & trading);
	static void GetSohuPerformanceForecast(int code, CSohuPerformanceForecast & forecast);
	static void GetCapitalFlow(int code, CCapitalFlows163 & capitalFlow);
	static void GetCapitalStruct(int code, CCapitalStructOf10jqka & capitalStruct);

	static void GetShanghaiStockInfo(CShanghaiStockInfo & stockinfo);
	static void GetEntrepreneurship(CEntrepreneurship & entrepreneurship);
	static void GetSmallMidEnterprise(CSmallMidEnterprise & smallMid);
	static void GetShenzhenAshares(CShenzhenAshares & shenzhenA);
	
	static CStockInfoData GetStockInfo(int nCode);
};
