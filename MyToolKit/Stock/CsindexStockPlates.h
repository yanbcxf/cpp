#pragma once
#include "stockdatamodel.h"

class CCsindexStockPlates : public CStockDataModel
{
public:
	CCsindexStockPlates(void);
	~CCsindexStockPlates(void);

	string SqlScript();
	void ImportToDatabase();
	void ExportFromDatabase(double startTime = 0, double endTime = 0);
	//void UpdateFromXls(string strCode, string name, int company_number, int is_stock);

public:
	CStockPlateTree * m_pTree;

	HWND m_hWnd;	//	接收通知消息的目标窗口句柄
	int	m_nLogType;	//	日志的级别
};
