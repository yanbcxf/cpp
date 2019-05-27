#pragma once
#include "BaseCsvFileShow.h"

class CIncomeStatementCsvFileShow :
	public CBaseCsvFileShow
{
public:
	CIncomeStatementCsvFileShow(void);
	~CIncomeStatementCsvFileShow(void);

	virtual GV_ITEM ProcessCell(int line, int col, string val);
};
