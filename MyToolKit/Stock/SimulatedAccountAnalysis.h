#pragma once

#include "AbstractIndex.h"


class CSimulatedAccountAnalysis
{
public:
	CSimulatedAccountAnalysis(void);
	~CSimulatedAccountAnalysis(void);

	void FetchInit();
	int  FetchNext();

	void SaveDetail(string strTradedate);

	static DWORD WINAPI CommandHandler1(void *data);
	DWORD  CommandHandler();

	
	static CSimulatedAccountAnalysis& getInstance();

private:
	static CSimulatedAccountAnalysis * instance;

public:

	//	每日交易日 对应的已经上市的公司代码 和 板块信息
	vector<string>		m_vecCode;
	vector<string>		m_vecPlate;

	//	所有上市交易日
	vector<string>		m_vecTradedate;

	//	记录已经读入的最晚数据的 上市交易日 的下标
	int								m_nLateset;
	
	//	记录当前所有的股票账户
	map<string, CSimulatedAccount *>	m_mapSimulatedAccount;

	//	key 为板块ID
	map<string, CStockPlateData>			m_mapPlate;

};
