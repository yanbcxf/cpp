#ifndef __GRID_CELL_BUTTON__
#define __GRID_CELL_BUTTON__
#include "../GridCtrl_src/GridCell.h"  
class CGridCellButton : public CGridCell
{
    friend class CGridCtrl;
    DECLARE_DYNCREATE(CGridCellButton)
public:
    CGridCellButton(void);
    ~CGridCellButton(void);
public:
    virtual BOOL Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd = TRUE);
protected:
    virtual void OnClick(CRect cellRect, CPoint PointCellRelative);
protected:
    CRect m_rect;
    BOOL m_bPushing;
};
#endif