#pragma once


// CMySplitterWndEx

class CMySplitterWndEx : public CSplitterWndEx
{
	DECLARE_DYNAMIC(CMySplitterWndEx)

public:
	CMySplitterWndEx();
	virtual ~CMySplitterWndEx();

	void RefreshSplitBars(void);


protected:
	void OnDrawSplitter(CDC* pDC, ESplitType nType,	const CRect& rectArg);

protected:
	DECLARE_MESSAGE_MAP()
};


