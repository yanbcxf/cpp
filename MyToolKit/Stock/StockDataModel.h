#pragma once

class CStockDataModel
{
public:
	CStockDataModel(void);

	//	析构函数应定义为虚函数，以便子类对象释放 m_vec_xxx 中的内存，避免memory leaks
	virtual ~CStockDataModel(void);
	
	virtual string ModelType() = 0;

	// 返回值， CSV_CHECK_NO_ERROR ： 成功   CSV_CHECK_FORMAT_ERROR : 下载的CSV 格式错误   
	// CSV_CHECK_DATA_EXIST_ERROR ：数据库中已存在CSV中的记录
	virtual int  CheckReport(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData) = 0;
	virtual void ParseCsvFile(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData) = 0;
	virtual bool ParseCsvFileName(string szFileName) = 0;
	virtual void ParseXmlElement(void) = 0;
	virtual void ImportToDatabase(void) = 0;
	virtual void ExportFromDatabase(double startTime = 0.0, double endTime = 0.0) =0;
	virtual CStockDataModel * NewCopy() = 0;

	virtual string GetDownloadUrl() = 0;
	virtual string SaveAsCsv() = 0;
	virtual string GetCsvFileName() = 0;
	virtual string SqlScript() = 0;	//	创建数据库的脚本
	virtual void NotifyDatabaseImportedToWindow();

	long CheckDownloadFileInfo(string middleFileName);
	long GetDownloadFileSize(string middleFileName);
	

public:
	int m_nRowNum;
	int m_nCode;	//	股票代码 （或者交易日期：CsiPeDaily, CsiCsrcPeDaily ）
	HWND m_hWnd;	//	接收通知消息的目标窗口句柄
	int	m_nLogType;	//	日志的级别
	string m_cninfo_orgid;	
	string m_ConnectString ;

	enum eCheckResult { 
		CSV_CHECK_NO_ERROR,
		CSV_CHECK_FORMAT_ERROR,
		CSV_CHECK_DATA_EXIST_ERROR
	};

};
