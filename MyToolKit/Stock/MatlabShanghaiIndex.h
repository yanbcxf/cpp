#pragma once

 /*计算被大盘指数涨跌幅 修正后的个股总市值*/
class CMatlabShanghaiIndex : public CCalculateTask
{
public:
	CMatlabShanghaiIndex(HWND hwnd, int logType);
	~CMatlabShanghaiIndex(void);

	string ClassName(){ return "CMatlabShanghaiIndex"; }

	void Execute();
	static void Execute1(void* firstArg);

public:
	string  m_strIndexCode;

};
