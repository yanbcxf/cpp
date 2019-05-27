#include "StdAfx.h"
#include "BalanceCsvFileShow.h"

CBalanceCsvFileShow::CBalanceCsvFileShow(void)
{
}

CBalanceCsvFileShow::~CBalanceCsvFileShow(void)
{
}


GV_ITEM CBalanceCsvFileShow::ProcessCell(int row, int col, string val)
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
		{
			Item.nFormat = DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;
			if(row==2 || row == 28 || row ==56 || row ==89 || row==99)
			{
				LOGFONT   logfont;   
				lstrcpy((LPSTR)logfont.lfFaceName,(LPSTR)"楷体_GB2312");   
				logfont.lfWeight=FW_BOLD;   
				logfont.lfWidth=6;   
				logfont.lfHeight=16;   
				logfont.lfEscapement=0;   
				logfont.lfUnderline=FALSE;   
				logfont.lfItalic=FALSE;   
				logfont.lfStrikeOut=FALSE;   
				logfont.lfCharSet=GB2312_CHARSET;       //以上参数好像一个都不能少
				memcpy(&Item.lfFont, &logfont, sizeof(LOGFONT));
				Item.mask |= GVIF_FONT;
			}
			
		}
		else
			Item.nFormat = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;
		//string val = m_arrayData[row-1][col];
		if(col==0)
			Item.strText.Format(_T("%s"), val.c_str());
		else if(row==2 || row == 28 || row ==56 || row ==89 || row==99)
		{
			Item.strText = "";
		/*	Item.crBkClr =  RGB(154, 166, 167);
			Item.mask    |= (GVIF_BKCLR );*/
		}
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

			//	合计栏目
			if(row==27 || row ==55 || row ==88  || row==98 )
			{
				Item.crFgClr =  RGB(40, 5, 133);
				Item.mask    |= (GVIF_FGCLR);

				LOGFONT   logfont;   
				lstrcpy((LPSTR)logfont.lfFaceName,(LPSTR)"楷体_GB2312");   
				logfont.lfWeight=10;   
				logfont.lfWidth=6;   
				logfont.lfHeight=16;   
				logfont.lfEscapement=0;   
				logfont.lfUnderline=FALSE;   
				logfont.lfItalic=FALSE;   
				logfont.lfStrikeOut=FALSE;   
				logfont.lfCharSet=GB2312_CHARSET;       //以上参数好像一个都不能少
				memcpy(&Item.lfFont, &logfont, sizeof(LOGFONT));
				Item.mask |= GVIF_FONT;
			}
		}
	}
	return Item;
}