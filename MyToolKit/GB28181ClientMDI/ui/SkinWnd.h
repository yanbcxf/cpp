#if !defined _SKIN_WND_H_
#define _SKIN_WND_H_

#include <afxtempl.h>
#include <map>
using namespace std;

#include "SkinButton.h"
#include "BitmapSlider.h"

typedef struct _StruSepBmp{
	UINT	uBmpID;
	CRect   rect;
	_StruSepBmp()
	{
		uBmpID = -1;
		rect = CRect(0,0,0,0);
	}
}StruSepBmp,*LPStruSepBmp;

typedef std::map<int,StruSepBmp> SepBmpMap;
typedef SepBmpMap::iterator SepBmpIt;
typedef SepBmpMap::value_type SepBmpValueType;

class CSkinWnd : public CWnd
{
public:
	CSkinWnd(); 
	~CSkinWnd();

	//{{AFX_DATA(CSkinWnd)
	//}}AFX_DATA
public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);

public:
	void SetButtonToolTip(int nBtnID, CString m_NewToolTip);
	void SetButtonCheck(int nBtnID, BOOL m_NewValue);
	BOOL GetButtonCheck(int nBtnID);
	void SetButtonEnable(int nBtnID, BOOL m_Enable);

	// 设置某个按钮为选中状态,其余为非选中状态.
	void SetButtonSelect(int nBtnID);
	void SetButtonTextStyle(DWORD dwStyle = TEXT_CENTER);

	void FreeAll();

	int AddButton(UINT uID,CRect rect,CString strToolTip = _T(""),CString strText = _T(""),DWORD dwStyle = TEXT_CENTER);
	CSkinButton *GetButton(int nBtnID);

	int AddSlider(UINT uID,CRect rect,CString strToolTip);
	CBitmapSlider *GetSlider(int nSliderID);

	int AddComboBox(UINT uID,CRect rect,DWORD dwStyle = CBS_DROPDOWNLIST);
	CComboBox *GetComboBox(int nID);
	// 往文本中添加字符串,并设置相应的值.
	BOOL AddComboboxString(UINT uID,CString strText,DWORD dwValue);
	BOOL SetComboboxCurSel(UINT uID,DWORD dwValue);
	DWORD GetCurItemData(UINT uID);

	int AddDateTimeCtrl(UINT uID,CRect rect,DWORD dwStyle = DTS_SHORTDATEFORMAT);
	CDateTimeCtrl *GetDateTimeCtrl(int nID);
	CTime GetDateCtrl(UINT uID);

	void AddSepBmp(UINT uID,UINT ubmpID,CRect rect);
	void EditSepBmpRect(UINT uID,CRect rect);

	void CopyFrom(UINT uBtnID,CRect rect, CBitmap& Normal, CBitmap& Over, CBitmap& Down, CBitmap& Disabled);
	void CopyFrom(UINT uBtnID,UINT uNormal, UINT uOver, UINT uDown, UINT uDisabled);
	BOOL LoadDlgBitmap(UINT uLeftID,UINT uBKID,UINT uRightID,BOOL bstrechRight = FALSE);
	void DrawBKBitmap(CDC* pMemDC, CRect &rect,int nRightPosX = -1);
	
	void RecalcLayout(BOOL bReDraw = TRUE);

	//{{AFX_VIRTUAL(CSkinWnd)
	//}}AFX_VIRTUAL
protected:
	//{{AFX_MSG(CSkinWnd)
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT PressButtonUp(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT PressButtonDown(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnBitmapSliderMoved(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnBitmapSliderMoving(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CBitmap m_bmpLeft;
	CBitmap m_bmpBK;
	CBitmap m_bmpRight;

	CBitmap	*m_pOldBitmap;	// 

	CDC      m_MemDC;
	CBitmap  m_Bitmap;

	BOOL	 m_bReDraw;

	BOOL	m_bstrechRight;		// 右底图是否拉伸.

	CTypedPtrArray<CPtrArray, CSkinButton*> m_Buttons;
	CTypedPtrArray<CPtrArray, CBitmapSlider*> m_Sliders;
	CTypedPtrArray<CPtrArray, CComboBox*> m_cmbs;
	CTypedPtrArray<CPtrArray, CDateTimeCtrl*> m_CtrlDateTime;

	SepBmpMap m_mapSepBmp;	// 位图上的分隔图.
};

#endif
