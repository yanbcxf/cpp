#include "StdAfx.h"
#include "Stock.h"
#include "BussinessUtils.h"

CBussinessUtils::CBussinessUtils(void)
{
}

CBussinessUtils::~CBussinessUtils(void)
{
}


double CBussinessUtils::EstimateYearProfits(int nCode)
{
	CStockApp * pApp = (CStockApp *)AfxGetApp();
	
	map<int, CCninfoIncomeStatement>::iterator it_in = pApp->m_mapIncomeStatement.find(nCode);
	if(it_in!=pApp->m_mapIncomeStatement.end())
	{
		CCninfoIncomeStatement income = it_in->second;
		return income.EstimateYearProfits();
	}
	return -1;
}

double CBussinessUtils::CurrentATotalShares(int nCode, string curDate)
{
	CStockApp * pApp = (CStockApp *)AfxGetApp();
	double nShare = -1;

	AcquireReadLock(&g_RWlock);
	map<int, CCapitalStructOf10jqka>::iterator it_in = pApp->m_mapCapitalStruct.find(nCode);
	if(it_in!=pApp->m_mapCapitalStruct.end())
	{
		CCapitalStructOf10jqka income = it_in->second;
		nShare =  income.CurrentTotalShare(curDate);
	}
	ReleaseReadLock(&g_RWlock);
	return nShare;
}


double CBussinessUtils::CurrentACalculateshares(int nCode, string curDate)
{
	CStockApp * pApp = (CStockApp *)AfxGetApp();
	double nShare = -1;

	AcquireReadLock(&g_RWlock);
	map<int, CCapitalStructOf10jqka>::iterator it_in = pApp->m_mapCapitalStruct.find(nCode);
	if(it_in!=pApp->m_mapCapitalStruct.end())
	{
		CCapitalStructOf10jqka income = it_in->second;
		nShare = income.CurrentACalculateShare(curDate);
	}
	ReleaseReadLock(&g_RWlock);
	return nShare;
}

double CBussinessUtils::NetProfitBelongingToParentCompanyFromIncome(int nCode, string curDate, string & baseDate,  bool & isLast)
{
	CStockApp * pApp = (CStockApp *)AfxGetApp();

	map<int, CCninfoIncomeStatement>::iterator it_in = pApp->m_mapIncomeStatement.find(nCode);
	if(it_in!=pApp->m_mapIncomeStatement.end())
	{
		//CIncomeStatement income = it_in->second;
		return it_in->second.NetProfitBelongingToParentCompany(curDate, baseDate, isLast);
	}
	return -1;
}

double CBussinessUtils::NetProfitBelongingToParentCompanyFromForecast(int nCode, string curDate,  string & baseDate)
{
	CStockApp * pApp = (CStockApp *)AfxGetApp();

	map<int, CSohuPerformanceForecast>::iterator it_in = pApp->m_CSohuPerformanceForecast.find(nCode);
	if(it_in!=pApp->m_CSohuPerformanceForecast.end())
	{
		//CSohuPerformanceForecast income = it_in->second;
		return it_in->second.NetProfitBelongingToParentCompany(curDate, baseDate);
	}
	return -1;
}

// 寻找当前日期前的第 nDay 个交易日期
time_t CBussinessUtils::CurrentTradingDate(int nDay)
{
	time_t curtime = time (NULL);
	if(nDay<0)
		return curtime;

	while(1)
	{
		struct tm ret;
		//ret = gmtime (&curtime);
		localtime_s(&ret, &curtime);

		if(ret.tm_wday==0 || ret.tm_wday==6)
		{
			//	周末
			curtime = curtime - 24 * 3600;
		}
		else
		{
			// 该天为交易日
			if(nDay==0)
				break;
			else
			{
				nDay--;
				curtime = curtime - 24 * 3600;
			}
		}
	}
	return curtime;
}

void CBussinessUtils::GetIncomestatement(int code, CCninfoIncomeStatement & income)
{
	CStockApp * pApp = (CStockApp *)AfxGetApp();

	AcquireReadLock(&g_RWlock);
	map<int, CCninfoIncomeStatement>::iterator it1 = pApp->m_mapIncomeStatement.find(code);
	if(it1!=pApp->m_mapIncomeStatement.end())
	{
		income = it1->second;
	}
	ReleaseReadLock(&g_RWlock);
}

void CBussinessUtils::GetBalance(int code, CCninfoBalance & balance)
{
	CStockApp * pApp = (CStockApp *)AfxGetApp();

	AcquireReadLock(&g_RWlock);
	map<int, CCninfoBalance>::iterator it1 = pApp->m_mapBalance.find(code);
	if(it1!=pApp->m_mapBalance.end())
	{
		balance = it1->second;
	}
	ReleaseReadLock(&g_RWlock);
}

void CBussinessUtils::GetFundHold(int code, CSohuFundHold & fund)
{
	CStockApp * pApp = (CStockApp *)AfxGetApp();

	AcquireReadLock(&g_RWlock);
	map<int, CSohuFundHold>::iterator it1 = pApp->m_mapFundHold.find(code);
	if(it1!=pApp->m_mapFundHold.end())
	{
		fund = it1->second;
	}
	ReleaseReadLock(&g_RWlock);
}

void CBussinessUtils::GetCapitalFlows163(int code, CCapitalFlows163 & fund)
{
	CStockApp * pApp = (CStockApp *)AfxGetApp();

	AcquireReadLock(&g_RWlock);
	map<int, CCapitalFlows163>::iterator it1 = pApp->m_mapCapitalFlows.find(code);
	if(it1!=pApp->m_mapCapitalFlows.end())
	{
		fund = it1->second;
	}
	ReleaseReadLock(&g_RWlock);
}

void CBussinessUtils::GetCashFlow(int code, CCninfoCashFlows & cash)
{
	CStockApp * pApp = (CStockApp *)AfxGetApp();

	AcquireReadLock(&g_RWlock);
	map<int, CCninfoCashFlows>::iterator it1 = pApp->m_mapCashFlows.find(code);
	if(it1!=pApp->m_mapCashFlows.end())
	{
		cash = it1->second;
	}
	ReleaseReadLock(&g_RWlock);
}

void CBussinessUtils::GetSohuTradingDaily(int code, CNetEaseTradeDaily & trading)
{
	CStockApp * pApp = (CStockApp *)AfxGetApp();

	AcquireReadLock(&g_RWlock);
	map<int, CNetEaseTradeDaily>::iterator it1 = pApp->m_mapTradingDaily.find(code);
	if(it1!=pApp->m_mapTradingDaily.end())
	{
		trading = it1->second;
	}
	ReleaseReadLock(&g_RWlock);
}

void CBussinessUtils::GetSohuPerformanceForecast(int code, CSohuPerformanceForecast & forecast)
{
	CStockApp * pApp = (CStockApp *)AfxGetApp();

	AcquireReadLock(&g_RWlock);
	map<int, CSohuPerformanceForecast>::iterator it1 = pApp->m_CSohuPerformanceForecast.find(code);
	if(it1!=pApp->m_CSohuPerformanceForecast.end())
	{
		forecast = it1->second;
	}
	ReleaseReadLock(&g_RWlock);
}

void CBussinessUtils::GetCapitalFlow(int code, CCapitalFlows163 & capitalFlow)
{
	CStockApp * pApp = (CStockApp *)AfxGetApp();

	AcquireReadLock(&g_RWlock);
	map<int, CCapitalFlows163>::iterator it1 = pApp->m_mapCapitalFlows.find(code);
	if(it1!=pApp->m_mapCapitalFlows.end())
	{
		capitalFlow = it1->second;
	}
	ReleaseReadLock(&g_RWlock);
}

void CBussinessUtils::GetCapitalStruct(int code, CCapitalStructOf10jqka & capitalStruct)
{
	CStockApp * pApp = (CStockApp *)AfxGetApp();

	AcquireReadLock(&g_RWlock);
	map<int, CCapitalStructOf10jqka>::iterator it1 = pApp->m_mapCapitalStruct.find(code);
	if(it1!=pApp->m_mapCapitalStruct.end())
	{
		capitalStruct = it1->second;
	}
	ReleaseReadLock(&g_RWlock);
}

void CBussinessUtils::GetShanghaiStockInfo(CShanghaiStockInfo & stockinfo)
{
	CStockApp * pApp = (CStockApp *)AfxGetApp();
	AcquireReadLock(&g_RWlock);
	stockinfo = pApp->m_CShanghaiStock;
	ReleaseReadLock(&g_RWlock);
}

void CBussinessUtils::GetEntrepreneurship(CEntrepreneurship & entrepreneurship)
{
	CStockApp * pApp = (CStockApp *)AfxGetApp();
	AcquireReadLock(&g_RWlock);
	entrepreneurship = pApp->m_CEntrepreneurship;
	ReleaseReadLock(&g_RWlock);
}

void CBussinessUtils::GetSmallMidEnterprise(CSmallMidEnterprise & smallMid)
{
	CStockApp * pApp = (CStockApp *)AfxGetApp();
	AcquireReadLock(&g_RWlock);
	smallMid = pApp->m_CSmallMidEnterprise;
	ReleaseReadLock(&g_RWlock);
}

void CBussinessUtils::GetShenzhenAshares(CShenzhenAshares & shenzhenA)
{
	CStockApp * pApp = (CStockApp *)AfxGetApp();
	AcquireReadLock(&g_RWlock);
	shenzhenA = pApp->m_CShenzhenAshares;
	ReleaseReadLock(&g_RWlock);
}

CStockInfoData CBussinessUtils::GetStockInfo(int nCode)
{
	CStockInfoData stock;
	/* 获取对应代码的股票信息 */
	CEntrepreneurship Entrepreneurship ;
	CBussinessUtils::GetEntrepreneurship(Entrepreneurship);
	for(int i=0; i< Entrepreneurship.m_nRowNum; i++)
	{
		if(Entrepreneurship.m_vec_code[i]==nCode)
		{
			stock.code = nCode;
			stock.a_abbreviation = Entrepreneurship.m_vec_a_abbreviation[i];
			stock.abbreviation = Entrepreneurship.m_vec_abbreviation[i];
			stock.a_time_to_market = Entrepreneurship.m_vec_a_time_to_market[i];
			stock.a_total_share = Entrepreneurship.m_vec_a_total_share[i];
			stock.cninfo_orgid = Entrepreneurship.m_vec_cninfo_orgid[i];
			return stock;
		}
	}

	CSmallMidEnterprise midsmall ;
	CBussinessUtils::GetSmallMidEnterprise(midsmall);
	for(int i=0; i< midsmall.m_nRowNum; i++)
	{
		if(midsmall.m_vec_code[i]==nCode)
		{
			stock.code = nCode;
			stock.a_abbreviation = midsmall.m_vec_a_abbreviation[i];
			stock.abbreviation = midsmall.m_vec_abbreviation[i];
			stock.a_time_to_market = midsmall.m_vec_a_time_to_market[i];
			stock.a_total_share = midsmall.m_vec_a_total_share[i];
			stock.cninfo_orgid = midsmall.m_vec_cninfo_orgid[i];
			return stock;
		}
		
	}

	CShenzhenAshares shenzhenA ;
	CBussinessUtils::GetShenzhenAshares(shenzhenA);
	for(int i=0; i< shenzhenA.m_nRowNum; i++)
	{
		if(shenzhenA.m_vec_code[i]==nCode)
		{
			stock.code = nCode;
			stock.a_abbreviation = shenzhenA.m_vec_a_abbreviation[i];
			stock.abbreviation = shenzhenA.m_vec_abbreviation[i];
			stock.a_time_to_market = shenzhenA.m_vec_a_time_to_market[i];
			stock.a_total_share = shenzhenA.m_vec_a_total_share[i];
			stock.cninfo_orgid = shenzhenA.m_vec_cninfo_orgid[i];
			return stock;
		}
		
	}

	CShanghaiStockInfo shanghai ;
	CBussinessUtils::GetShanghaiStockInfo(shanghai);
	for(int i=0; i< shanghai.m_nRowNum; i++)
	{
		if(shanghai.m_vec_code[i]==nCode)
		{
			stock.code = nCode;
			stock.a_abbreviation = shanghai.m_vec_a_abbreviation[i];
			stock.abbreviation = shanghai.m_vec_abbreviation[i];
			stock.a_time_to_market = shanghai.m_vec_a_time_to_market[i];
			stock.a_total_share = shanghai.m_vec_a_total_share[i];
			stock.cninfo_orgid = shanghai.m_vec_cninfo_orgid[i];
			return stock;
		}
		
	}
	return stock;
}