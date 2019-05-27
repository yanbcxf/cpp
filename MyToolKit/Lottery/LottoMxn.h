#pragma once

#include "lotterydatamodel.h"

class CLottoMxn : public CLotteryDataModel
{
public:
	CLottoMxn(void);
	~CLottoMxn(void);
	
	string ModelType(){ return  "CLottoMxn" /* 同花顺分红送转 */; }

	int  CheckReport(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData){ return 0;};
	void ParseCsvFile(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData){};
	bool ParseCsvFileName(string szFileName){ return true; };
	void ParseXmlElement(void){};
	CLotteryDataModel * NewCopy() { return NULL; }

	void ImportToDatabase(void);
	void ExportFromDatabase(double startTime = 0, double endTime = 0){};

	string GetDownloadUrl(){ return ""; };
	string SaveAsCsv();
	string SqlScript();

	string GetCsvFileName(){ return ""; };
	void NotifyDatabaseImportedToWindow();

public:
	string	m_lottery_name;
	string	m_lottery_code;
	int		m_regular_num;	//	正选位数
	int		m_special_num;	//	特别号码位数
	int		m_lottery_cpqx;	//	指定下载的时间
	vector<LotteryNumber>	m_vec_lottery;
};
