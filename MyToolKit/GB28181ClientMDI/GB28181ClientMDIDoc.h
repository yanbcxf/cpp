
// GB28181ClientMDIDoc.h : CGB28181ClientMDIDoc 类的接口
//


#pragma once


class CGB28181ClientMDIDoc : public CDocument
{
protected: // 仅从序列化创建
public:
	CGB28181ClientMDIDoc();

	CGB28181ClientMDIDoc(string strDeviceId);

	DECLARE_DYNCREATE(CGB28181ClientMDIDoc)

// 属性
public:
	string m_strDeviceId;

// 操作
public:

// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// 实现
public:
	virtual ~CGB28181ClientMDIDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
};


