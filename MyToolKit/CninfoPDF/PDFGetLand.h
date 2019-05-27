#pragma once
#include "cninfoannouncepdf.h"

class CPDFGetLand :
	public CCninfoAnnouncePDF
{
public:
	CPDFGetLand(void);
	~CPDFGetLand(void);

	virtual string SaveAsCsv();
	void ProcessTable(string sentence);
	void ProcessItem(string title, string strVal);
	bool ProcessSentence(string sentence, bool needCheck);

	bool AnalysisClause(vector<string> vecWord, string originalWord);
	bool AnalysisSegment(string sentence ,bool isTableCell = false);

	bool ProcessNoLevel(vector<CLineOrCell>	 & pdf);
	bool ProcessLevel(vector<CLineOrCell>	 & pdf);

	void ProcessDealMoney(vector<CLineOrCell> & pdf);
	string SqlScript();

	virtual void ImportToDatabase(string csv);
	virtual list<CCninfoAnnouncePDF *> CreatePDF();

public:
	bool	m_bLevel;
	bool	m_bTable;

	string	m_DealMoney[200];
	string	m_LandName[200];
	string	m_LandLocation[200];
	string	m_LandPurpose[200];
	
	string	m_LandArea[200];
	string	m_RateOfCapacity[200]; 
	string	m_AreaOfStructure[200];

	int		m_nDealMoney;
	int		m_nLandName;
	int		m_nLandLocation;
	int		m_nLandPurpose;
	int		m_nLandArea;
	int		m_nRateOfCapacity;
	int		m_nAreaOfStructure;
};
