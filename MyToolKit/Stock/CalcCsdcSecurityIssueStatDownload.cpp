#include "StdAfx.h"
#include "CalcCsdcSecurityIssueStatDownload.h"

CCalcCsdcSecurityIssueStatDownload::CCalcCsdcSecurityIssueStatDownload(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalcCsdcSecurityIssueStatDownload::~CCalcCsdcSecurityIssueStatDownload(void)
{
}




void CCalcCsdcSecurityIssueStatDownload::NeedDownload1(void* firstArg)
{
	CCalcCsdcSecurityIssueStatDownload * sink = (CCalcCsdcSecurityIssueStatDownload *)firstArg;
	sink->NeedDownload();
}



void CCalcCsdcSecurityIssueStatDownload::NeedDownload()
{
	/*汇总需要下载中国结算的每周资金结算统计表数据的周期*/
	stringstream ss;
	map<string, int> map_trade_date;
	map_trade_date.clear();

	ss.str("");
	ss << "任务【" << m_nTaskID << "】 ";
	ss << "NeedDownload 开始计算";
	sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);


	bool bContinue = true;
	time_t time_end = CBussinessUtils::CurrentTradingDate(0);
	time_t time_start = convert_string_to_time_t("2005-06-03 01:00:00");	//	2005-06-03 为周五
	while(bContinue)
	{
		char tmp[256] = { 0 };
		struct tm  curr_tm;
		//curr_tm = gmtime (&time_start);
		localtime_s(&curr_tm, &time_start);

		snprintf (tmp, 255, "%4d%02d%02d", 1900 + curr_tm.tm_year, curr_tm.tm_mon+1, curr_tm.tm_mday);
		m_vec_code.push_back(atoi(tmp));

		time_start = time_start + 7 * 24 * 3600;

		if(time_end<=time_start)
			bContinue = false;
	}

	ss.str("");
	ss << "任务【" << m_nTaskID << "】 ";
	ss << "需要下载 《一周证券发行登记统计表》 的周数 ：";
	ss << m_vec_code.size() << " \r\n";
	sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);


}