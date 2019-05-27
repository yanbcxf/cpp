#pragma once
#include "BaseCsvFileShow.h"

class CCashFlowsCsvFileShow :
	public CBaseCsvFileShow
{
public:
	CCashFlowsCsvFileShow(void);
	~CCashFlowsCsvFileShow(void);

	virtual GV_ITEM ProcessCell(int line, int col, string val);

};
