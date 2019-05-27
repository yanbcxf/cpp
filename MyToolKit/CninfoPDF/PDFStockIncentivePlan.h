#pragma once
#include "cninfoannouncepdf.h"

class CPDFStockIncentivePlan :
	public CCninfoAnnouncePDF
{
public:
	CPDFStockIncentivePlan(void);
	~CPDFStockIncentivePlan(void);

	virtual string SaveAsCsv();
	string SqlScript();

	virtual void ImportToDatabase(string csv);
	virtual list<CCninfoAnnouncePDF *> CreatePDF();

public:
	vector<string>	m_sentences;
	map<int,int>		m_incentive_year;
	map<int,int>		m_base_from_year;
	map<int,int>		m_base_to_year;
	map<int,double>		m_increase_percent;
	map<int,double>		m_net_profit;					//	净利润
	map<int,double>		m_sales_from_operations;		//	营业收入

protected:
	bool AnalysisSentence(string sentence);
	bool AnalysisSentenceForSalsFromOperations(string sentence);

};
