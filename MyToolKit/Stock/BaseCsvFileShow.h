#pragma once
#include "./GridCtrl_src/GridCtrl.h"

class CBaseCsvFileShow
{
public:
	CBaseCsvFileShow(void);
	~CBaseCsvFileShow(void);

	virtual GV_ITEM ProcessCell(int line, int col, string val) = 0;
};
