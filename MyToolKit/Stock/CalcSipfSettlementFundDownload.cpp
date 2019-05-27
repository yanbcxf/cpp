#include "StdAfx.h"
#include "CalcSipfSettlementFundDownload.h"

CCalcSipfSettlementFundDownload::CCalcSipfSettlementFundDownload(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalcSipfSettlementFundDownload::~CCalcSipfSettlementFundDownload(void)
{
}



void CCalcSipfSettlementFundDownload::NeedDownload1(void* firstArg)
{
	CCalcSipfSettlementFundDownload * sink = (CCalcSipfSettlementFundDownload *)firstArg;
	sink->NeedDownload();
}



void CCalcSipfSettlementFundDownload::NeedDownload()
{
	/*汇总需要下载中国投资者保护基金的银证交易金数据的日期*/
	stringstream ss;
	map<string, int> map_trade_date;
	map_trade_date.clear();

	ss.str("");
	ss << "任务【" << m_nTaskID << "】 ";
	ss << "NeedDownload 开始计算";
	sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

	

	m_vec_code.push_back(20140101);
	m_vec_code.push_back(20140201);
	m_vec_code.push_back(20140301);
	m_vec_code.push_back(20140401);
	m_vec_code.push_back(20140501);
	m_vec_code.push_back(20140601);
	m_vec_code.push_back(20140701);
	m_vec_code.push_back(20140801);
	m_vec_code.push_back(20140901);
	m_vec_code.push_back(20141001);
	m_vec_code.push_back(20141101);
	m_vec_code.push_back(20141201);

	m_vec_code.push_back(20150101);
	m_vec_code.push_back(20150201);
	m_vec_code.push_back(20150301);
	m_vec_code.push_back(20150401);
	m_vec_code.push_back(20150501);
	m_vec_code.push_back(20150601);
	m_vec_code.push_back(20150701);
	m_vec_code.push_back(20150801);
	m_vec_code.push_back(20150901);
	m_vec_code.push_back(20151001);
	m_vec_code.push_back(20151101);
	m_vec_code.push_back(20151201);

	m_vec_code.push_back(20160101);
	m_vec_code.push_back(20160201);
	m_vec_code.push_back(20160301);
	m_vec_code.push_back(20160401);
	m_vec_code.push_back(20160501);
	m_vec_code.push_back(20160601);
	m_vec_code.push_back(20160701);
	m_vec_code.push_back(20160801);

	m_vec_code.push_back(20160901);
	m_vec_code.push_back(20161001);
	m_vec_code.push_back(20161101);
	m_vec_code.push_back(20161201);


	ss.str("");
	ss << "任务【" << m_nTaskID << "】 ";
	ss << "需要下载 《中国投资者保护基金的银证交易金》 的日期数 ：";
	ss << m_vec_code.size() << " \r\n";
	sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);


}