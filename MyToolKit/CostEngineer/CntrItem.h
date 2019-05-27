
// CntrItem.h: CCostEngineerCntrItem 类的接口
//

#pragma once

class CCostEngineerDoc;
class CCostEngineerView;

class CCostEngineerCntrItem : public COleClientItem
{
	DECLARE_SERIAL(CCostEngineerCntrItem)

// 构造函数
public:
	CCostEngineerCntrItem(CCostEngineerDoc* pContainer = nullptr);
		// 注意: 允许 pContainer 为 NULL 以启用 IMPLEMENT_SERIALIZE
		//  IMPLEMENT_SERIALIZE 要求类具有带零
		//  参数的构造函数。  OLE 项通常是用
		//  非 null 文档指针

// 特性
public:
	CCostEngineerDoc* GetDocument()
		{ return reinterpret_cast<CCostEngineerDoc*>(COleClientItem::GetDocument()); }
	CCostEngineerView* GetActiveView()
		{ return reinterpret_cast<CCostEngineerView*>(COleClientItem::GetActiveView()); }

public:
	virtual void OnChange(OLE_NOTIFICATION wNotification, DWORD dwParam);
	virtual void OnActivate();

protected:
	virtual void OnGetItemPosition(CRect& rPosition);
	virtual void OnDeactivateUI(BOOL bUndoable);
	virtual BOOL OnChangeItemPosition(const CRect& rectPos);
	virtual BOOL OnShowControlBars(CFrameWnd* pFrameWnd, BOOL bShow);

// 实现
public:
	~CCostEngineerCntrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual void Serialize(CArchive& ar);
};

