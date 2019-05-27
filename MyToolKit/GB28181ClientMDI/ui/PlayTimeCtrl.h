#ifndef __PLAYTIMECTRL__
#define __PLAYTIMECTRL__

/////////////////////////////////////////////////////////////////////////////
// CPlayTimeCtrl window
#include <map>
#include <list>
using namespace std;


#define WM_PLAYTIME_LBUTTONDBLCLK		WM_USER + 9423

// 当前时间点要显示的类型.
enum
{
	_TIME_DAY	= 0,	// 单天.
	_TIME_HOUR2,		// 两小时.
	_TIME_HOUR1,		// 1小时.
	_TIME_MIN30			// 30分.
};

// 年月日.
typedef struct _STRU_DATE
{
	DWORD dwYear;
	DWORD dwMonth;
	DWORD dwDay;

	_STRU_DATE()
	{
		dwYear = 1970;
		dwMonth = 1;
		dwDay = 1;
	}
}STRU_DATE,*LPSTRU_DATE;

// 时分秒.
typedef struct _STRU_TIME
{
	short nHour;
	short nMin;
	short nSec;

	_STRU_TIME()
	{
		nHour = 0;
		nMin = 0;
		nSec = 0;
	}
}STRU_TIME,*LPSTRU_TIME;

//年月日时分秒.
typedef struct _STRU_DATETIME
{	
	STRU_DATE struDate;
	STRU_TIME struTimeStart;	// 开始时间.
	STRU_TIME struTimeEnd;		// 结束时间.

}STRU_DATETIME,*LPSTRU_DATETIME;

typedef struct _STRU_DAY
{
	STRU_DATE struDate;

	list<STRU_DATETIME> listDate;	// 所有有效时间.

	_STRU_DAY()
	{
		listDate.clear();
	}

}STRU_DAY,*LPSTRU_DAY;

enum
{
	_TYPE_BTN_ = 0,	// 按钮.
	_TYPE_TEXT_,	// 文本.
};

// 按钮信息.
typedef struct _STRUBTN{
	UINT    uBtnID;		// 按钮ID.
	HRGN	hRgn;		// 按钮区域.
	CString strText;	// 文本.
	CString strToolTip;	// 按钮提示文本.
	int		nType;		// 类型.

	_STRUBTN()
	{
		nType		= _TYPE_BTN_;
		uBtnID		= -1;
		hRgn		= NULL;
		strText		= _T("");
		strToolTip	= _T("");
	}
}STRUBTN,*LPSTRUBTN;

// 各个窗口对应的数据.
typedef std::map<int,STRU_DAY> WndDataMap;
typedef WndDataMap::iterator WndDataMapIt;
typedef WndDataMap::value_type WndDataMapValueType;

// 按钮的具体信息.
typedef std::map<UINT,STRUBTN> BtnsMap;
typedef BtnsMap::iterator BtnsMapIt;
typedef BtnsMap::value_type BtnsMapValueType;

class CPlayTimeCtrl : public CWnd
{
public:
	CPlayTimeCtrl();
	virtual ~CPlayTimeCtrl();

	//{{AFX_VIRTUAL(CPlayTimeCtrl)
public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL

public:	
	void InvalidateCtrl();
	void RecalcLayout();

	// 清空指定窗口的时间.[清除内部时间]
	void ClearWnd(int nWndIndex);

	// 设置对应窗口的文件时间.
	void SetWndData(int nWndIndex ,DWORD dwYear,DWORD dwMonth,DWORD dwDay,RETSEARCH *pstruFileInfo ,int nFileCnt);
	void AddWndData(int nWndIndex ,DWORD dwYear,DWORD dwMonth,DWORD dwDay,RETSEARCH *pstruFileInfo ,int nFileCnt);

	void SetCurWndText(CString strCurWndText);
protected:	
	void DrawTicks(CDC* pDC);
	// 绘制底部控制按钮等.
	void DrawBottomBtn(CDC *pDC,CRect rect);

protected:
	void InitToolTips();

	// 返回鼠标所在窗口下标. 
	// 从0开始,失败 -1.点击间隔也返回-1.
	int  PtInWnd(CPoint point);

	// 返回鼠标是否在按钮位置上.
	int PtInBtns(CPoint point,CString &strToolTip);

	// 清除按钮资源.
	void ClearBtns();

	// 获取支持的时间长度.精确到秒.
	// bCalcSec TRUE 计算到秒,FALSE 计算到分.
	// 返回值 -1 不支持.
	// 大于0.
	long GetTimeLength(int nType,BOOL bCalcSec = TRUE);

	// 获取一个最大能拆分成多少个时间段.
	int GetTimeOffsetSupportCnt(int nType);

	// 获取一个时间段要拆分的格数.
	// 显示全部类型则查找第一个有时间段的天数间隔,
	int GetSplitCnt(int nType);

	// 根据当前类型,计算传入的时间所对应的位置.
	// -1: 传入的时间点小于当前窗口的时间段范围.
	// 0 : 传入的时间点在当前窗口的时间段范围.
	// 1: 传入的时间点大于当前窗口的时间段范围.
	int CalcTimePos(
		const STRU_DATE &struCurDate,
		const STRU_DATETIME &struDateTime,
		int &nStartPos,int &nEndPos);

	int GetYearDay(int y,int m,int d);//获取某天是一年中的第机天
	// 获取当前位置对应的时间点.
	// 返回鼠标所在窗口位置,以及对应时间点信息.
	// point 鼠标位置.
	// strTime 对应的时间点.
	BOOL GetCurrentPointTime(CPoint point,int &nWndIndex,CString &strTime);
	
protected:
	//{{AFX_MSG(CPlayTimeCtrl)
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CToolTipCtrl	m_ToolTip;

	CDC				m_MemDC;
	CBitmap			m_Bitmap;

protected:
	BtnsMap			m_MapBtns;		// 所有按钮信息.
	WndDataMap		m_MapWnds;		// 所有窗口的文件数据..

protected:
	int			m_nType;			// 当前显示的类型.
	int			m_nTimeOffset;		// 时间偏移.
	int			m_nCtrlType;		// 控制按钮类型.

protected:
	COLORREF	m_clrGapUp;			// 上半部间隔颜色.
	COLORREF	m_clrGapDown;		// 下半部间隔颜色.
	COLORREF	m_clrBackGround;	// 背景颜色.

	COLORREF	m_clrFile;			// 文件颜色.

	COLORREF	m_clrText;			// 文本颜色.

	COLORREF    m_clrBtnNorm;		// 按钮正常颜色.
	COLORREF	m_clrBtnSelect;		// 按钮选中颜色.

	int			m_nWndCnt;			// 窗口个数.
	int			m_nLeftW;			// 左边预留宽.
	int			m_nEachWndH;		// 每个窗口高度.
	int			m_nEachWndW;		// 每个窗口的宽度.
	int			m_nGapOffset;		// 间隔大小.

	int			m_nEachHourW;		// 两个单位占的宽度.	

	CString		m_strCurWndText;	// 当前窗口要显示的文本.
	CString     m_strMouseMoveTm;	// 鼠标在时间轴上的文本.

private:
	BOOL        m_TipFlag;
};

/////////////////////////////////////////////////////////////////////////////
#endif
