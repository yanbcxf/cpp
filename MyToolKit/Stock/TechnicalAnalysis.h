#pragma once

#include "AbstractIndex.h"


class CTechnicalAnalysis
{
public:
	CTechnicalAnalysis(void);
	~CTechnicalAnalysis(void);

	void FetchInit(string beginTradedate);
	bool FetchNext();

	
	static DWORD WINAPI CommandHandler1(void *data);
	DWORD  CommandHandler();

	static CTechnicalAnalysis& getInstance();

private:
	static CTechnicalAnalysis * instance;

public:

	//	每个交易日 、 每只股票 对应的交易信息
	map<string, CTechnicalAnalysisData>		m_mapTechnicalAnalysisData;

	//	每日交易日 对应的已经上市的公司代码 和 板块信息
	vector<string>		m_vecCode;
	vector<string>		m_vecPlate;

	//	所有上市交易日
	vector<string>		m_vecTradedate;

	//	记录已经读入的最晚数据的 上市交易日 的下标
	int								m_nLateset;
	int								m_nEarliest;

	//	记录要计算的所有指标
	vector<CAbstractIndex *>		m_vecIndex;

};
