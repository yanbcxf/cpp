#pragma once

// 自选股板块

class CSelfSelectStockPlates
{
public:
	CSelfSelectStockPlates(void);
	~CSelfSelectStockPlates(void);

	string SqlScript();
	void ImportToDatabase();
	void ExportFromDatabase(double startTime = 0, double endTime = 0);
	vector<string> MailContents();
	
public:
	//CStockPlateTree * m_pTree;

	CStockPlateTree * m_pTree;

	HWND m_hWnd;	//	接收通知消息的目标窗口句柄
	int	m_nLogType;	//	日志的级别

	JMutex	m_mutex;

	map<int, int>	vec_code;
	map<int,string>	vec_name;
	map<int, int>	vec_company_number;
	map<int, int>	vec_parent_code;
	map<int, int>	vec_is_stock;	
};
