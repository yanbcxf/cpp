#pragma once
#include "BaseCsvFileShow.h"

class CSohuFundHoldCsvFileShow :
	public CBaseCsvFileShow
{
public:
	CSohuFundHoldCsvFileShow(void);
	~CSohuFundHoldCsvFileShow(void);

	virtual GV_ITEM ProcessCell(int line, int col, string val);

};
