#pragma once

class CStatsGovHgyd : public CStatsGovCn
{
public:
	CStatsGovHgyd(void);
	~CStatsGovHgyd(void);

	string ModelType(){ return  "CStatsGovHgyd" /* 中国国家统计局 月度数据 */; }

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
