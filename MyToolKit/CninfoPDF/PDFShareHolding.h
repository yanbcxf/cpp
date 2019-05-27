#pragma once
#include "cninfoannouncepdf.h"

class CPDFShareHolding :
	public CCninfoAnnouncePDF
{
public:
	CPDFShareHolding(void);
	~CPDFShareHolding(void);

	virtual string SaveAsCsv();

	string SqlScript();
	string ParseYearQuarter(string title);
	void UpdateQuarter();

	virtual void ImportToDatabase(string csv);
	virtual list<CCninfoAnnouncePDF *> CreatePDF();

public:
	

	string	m_ShareHolder[20];		//	股东名称
	long	m_NumberOfHolding[20];	//	期末持股数量
	double	m_SharePercent[20];
	long	m_NumberOfRestrict[20];	//	限售股数量
	string	m_Character[20];		//	股东性质
	
	int		m_nShareHolder;
};
