#pragma once

#define  RECORD_NUM_PER_PAGE 500

typedef void CalculateTaskFunc(void* clientData);

//////////////////////////////////////////////////////////////////////////
// 使用规则
// 

class CTipRecord
{
public:
	string	secCode;
	string	id;
	string	title;
	string	url;
	string	report_date;
};

class CCalculateTask
{
public:
	CCalculateTask(HWND hwnd, int logType);
	virtual ~CCalculateTask(void);

	LONG TaskID() { return m_nTaskID; }

	virtual string ClassName() = 0;
	
	void ScheduleTask(CalculateTaskFunc * proc);

	void NotifyTaskFinished();	//	在计算执行线程中发送通知到“任务发起窗口”,通知中包含任务标识

	static CCalculateTask * FetchTask(LONG taskID);

	static void init();

	static void unInit();

	static UINT CalculateThreadFunc(LPVOID param);

	void AddPoint(string strZbCode, string strZbName, string strSjCode, string strUnit, double dbData,double dbData1 = 0,double dbData2 = 0,double dbData3 = 0);
	

	double GetDoubleValue(row & r, string colName)
	{
		double strResult;
		try{
			strResult = r.get<double>(colName);
		}
		catch(...)
		{
			unsigned long nan[2]={0xffffffff, 0x7fffffff};    
			strResult = *( double* )nan;
		}
		return strResult;
	}

public:
	HWND m_hWnd;
	int	m_nLogType;
	vector<string>		m_vec_request_code;		//	请求计算的股票范围
	vector<int>		m_vec_request_is_stock;
	string			m_str_request_zb_code;		//	

	COleDateTime		m_StartDate;
	COleDateTime		m_EndDate;

	CalculateTaskFunc * m_proc;
	
protected:
	LONG	m_nTaskID;						//	计算任务的 标识，全局唯一
	
	/* 以下为标准的输出格式 */
public:
	vector<string>		zb_code;	//	纵坐标
	vector<string>		zb_name;
	vector<string>		sj_code;	//	横坐标
	vector<double>		data;
	vector<double>		data1;
	vector<double>		data2;
	vector<double>		data3;
	vector<string>		unit;		//	数据单位
	map<string,list<CTipRecord>>	tip;		//	气球提示

	string				title_code;	//	固定维 代码
	string				title_name;	//	固定维 名称

	int		m_windowInMatlab;				//	matlab 中窗口的标识
	string	m_strGraphOrTable;				//	matlab 中输出 “图表” 或者 “表格”
};
