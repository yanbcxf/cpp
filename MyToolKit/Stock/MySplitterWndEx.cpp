// MySplitterWndEx.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "MySplitterWndEx.h"


// CMySplitterWndEx

IMPLEMENT_DYNAMIC(CMySplitterWndEx, CSplitterWndEx)

CMySplitterWndEx::CMySplitterWndEx()
{

}

CMySplitterWndEx::~CMySplitterWndEx()
{
}


BEGIN_MESSAGE_MAP(CMySplitterWndEx, CSplitterWndEx)
END_MESSAGE_MAP()



// CMySplitterWndEx 消息处理程序


void CMySplitterWndEx::RefreshSplitBars(void)
{	
	CRect rectInside;
	GetInsideRect(rectInside);
	DrawAllSplitBars(NULL, rectInside.right, rectInside.bottom);
}


void CMySplitterWndEx::OnDrawSplitter(CDC* pDC, ESplitType nType, const CRect& rectArg)
{
	/* 高亮显示 获取焦点的 视图 */

	int x_ActivePane, y_ActivePane; 
	COLORREF hilightcolor = RGB(255,0,0);
	GetActivePane(&x_ActivePane, &y_ActivePane);

	if( ((GetRowCount()>1) || (GetColumnCount()>1)) && (nType == splitBorder))
	{
		int pRow = 0;
		int pCol = 0;
		if(rectArg.top)
		{
			pRow = 1;	
		}
		if(rectArg.left)
		{
			pCol = 1;
		}
		if((pCol == y_ActivePane) && (pRow == x_ActivePane)) 
		{
			if (pDC == NULL)
			{
				RedrawWindow(rectArg, NULL, RDW_INVALIDATE|RDW_NOCHILDREN);
				return; 		
			}
			ASSERT_VALID(pDC);	
			CRect rect = rectArg;
			pDC->Draw3dRect(rect, hilightcolor, hilightcolor);
			int dx = -GetSystemMetrics(SM_CXBORDER);
			int dy = -GetSystemMetrics(SM_CYBORDER);
			rect.InflateRect(dx,dy);
			pDC->Draw3dRect(rect, hilightcolor, hilightcolor);
			return;
		}
	}
	CSplitterWndEx::OnDrawSplitter(pDC,nType,rectArg);
}