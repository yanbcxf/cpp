#pragma once

class CStatsGovHgjd : public CStatsGovCn
{
public:
	CStatsGovHgjd(void);
	~CStatsGovHgjd(void);

	string ModelType(){ return  "CStatsGovHgjd" /* 中国国家统计局 季度数据 */; }

	void ImportToDatabase(void);
	void ExportFromDatabase(double startTime = 0, double endTime = 0);

	string SaveAsCsv();
	string SqlScript();

	void NotifyDatabaseImportedToWindow();

	string LastSjCodeFromDatabase();

	vector<string>	NeedDownload();
	

public:
	//	请求下载的“指标”大类代码
	string	m_request_zb_code;

	vector<string>	zb_code;
	vector<string>	sj_code;
	vector<string>	zb_name;
	vector<double>	data;
	vector<string>	unit;

};
