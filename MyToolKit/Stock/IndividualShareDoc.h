#pragma once

// CIndividualShareDoc 文档

class CIndividualShareDoc : public CDocument
{
	DECLARE_DYNCREATE(CIndividualShareDoc)

public:
	CIndividualShareDoc();
	CIndividualShareDoc(string code, int command_code = 0, int is_stock = 0);
	virtual ~CIndividualShareDoc();
#ifndef _WIN32_WCE
	virtual void Serialize(CArchive& ar);   // 为文档 I/O 重写
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()

public:
	string m_nCode;
	int m_nCommandCode;
	int m_nIs_Stock;

	//	查找后，计算得到的查询目标集合

	vector<string>		m_vec_request_code;		
	vector<int>		m_vec_request_is_stock;

	/* 用于下载视图等，可以初始时传入多个的菜单命令，自动执行 */
	vector<int>	m_vecCommandCode;	

	vector<string>	m_vec_zb_code;
	vector<string>	m_vec_reg_code;
	
	string m_strInput;

	COleDateTime		m_StartDate;
	COleDateTime		m_EndDate;

	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
};
