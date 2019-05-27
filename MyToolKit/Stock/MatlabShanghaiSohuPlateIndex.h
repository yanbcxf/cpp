#pragma once

 /*计算被大盘指数涨跌幅 修正后的个股总市值*/
class CMatlabShanghaiSohuPlateIndex : public CCalculateTask
{
public:
	CMatlabShanghaiSohuPlateIndex(HWND hwnd, int logType);
	~CMatlabShanghaiSohuPlateIndex(void);

	string ClassName(){ return "CMatlabShanghaiSohuPlateIndex"; }

	void Execute();
	static void Execute1(void* firstArg);

public:
	

};
