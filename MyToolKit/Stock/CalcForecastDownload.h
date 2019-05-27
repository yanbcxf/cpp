#pragma once

class CCalcForecastDownload  : public CCalculateTask
{
public:
	CCalcForecastDownload(HWND hwnd, int logType);
	~CCalcForecastDownload(void);

	string ClassName(){ return "CCalcForecastDownload"; }

	void NeedDownloadPerformanceForecast();
	static void NeedDownloadPerformanceForecast1(void* firstArg);

public:
	vector<int>		m_vec_code			;	//	"¹«Ë¾´úÂë",

};
