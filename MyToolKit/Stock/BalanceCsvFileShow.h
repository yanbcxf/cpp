#pragma once
#include "BaseCsvFileShow.h"


class CBalanceCsvFileShow :
	public CBaseCsvFileShow
{
public:
	CBalanceCsvFileShow(void);
	~CBalanceCsvFileShow(void);

	virtual GV_ITEM ProcessCell(int line, int col, string val);
};
