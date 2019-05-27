#include "StdAfx.h"
#include "IncomeStatementCsvFileShow.h"

CIncomeStatementCsvFileShow::CIncomeStatementCsvFileShow(void)
{
}

CIncomeStatementCsvFileShow::~CIncomeStatementCsvFileShow(void)
{
}

GV_ITEM CIncomeStatementCsvFileShow::ProcessCell(int row, int col, string val)
{
	GV_ITEM Item;
	Item.mask = GVIF_TEXT|GVIF_FORMAT;
	Item.row = row;
	Item.col = col;
	if (row < 1) {
		Item.nFormat = DT_LEFT|DT_WORDBREAK;
		//string val = m_arrayHeader[col];
		Item.strText.Format(_T("%s"), val.c_str());
	} 
	else 
	{
		if(col==0)
			Item.nFormat = DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;
		else
			Item.nFormat = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;
		//string val = m_arrayData[row-1][col];
		if(col==0)
			Item.strText.Format(_T("%s"), val.c_str());
		else
		{
			if(val.find('Ԫ')!=string::npos)
			{
				Item.strText.Format(_T("%s"), val.c_str());
			}
			else
			{
				Item.strText = String2Currency(val).c_str();
			}
		}
	}
	return Item;
}