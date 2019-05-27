#include "StdAfx.h"
#include "SohuFundHoldCsvFileShow.h"

CSohuFundHoldCsvFileShow::CSohuFundHoldCsvFileShow(void)
{
}

CSohuFundHoldCsvFileShow::~CSohuFundHoldCsvFileShow(void)
{
}

GV_ITEM  CSohuFundHoldCsvFileShow::ProcessCell(int row, int col, string val)
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
		if(col==0 || col==1 || col==2)
			Item.nFormat = DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;
		else
			Item.nFormat = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;
		//string val = m_arrayData[row-1][col];
		if(col==0 || col==1 || col==2)
			Item.strText.Format(_T("%s"), val.c_str());
		else
		{
			double momey = String2Double(val);
			if(col==4)
				Item.strText.Format(_T("%s"), Double2Currency(momey ,true).c_str());
			else
				Item.strText.Format(_T("%s"), Double2Currency(momey).c_str());
		}
	}
	return Item;
}