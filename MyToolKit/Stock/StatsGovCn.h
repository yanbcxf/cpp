#pragma once
#include "stockdatamodel.h"

class CStatsGovCn  : public CStockDataModel
{
public:
	CStatsGovCn(void);
	~CStatsGovCn(void);

	string ModelType(){ return  "CStatsGovCn" /* 中国国家统计局 基类 */; }

	int  CheckReport(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData){ return 0;};
	void ParseCsvFile(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData){};
	bool ParseCsvFileName(string szFileName){ return true; };
	void ParseXmlElement(void){};
	CStockDataModel * NewCopy() { return NULL; }
	string GetCsvFileName(){ return ""; };
	string GetDownloadUrl(){ return ""; };

	//////////////////////////////////////////////////////////////////////////
	void ImportToDatabase(void){};
	void ExportFromDatabase(double starTime = 0, double endTime = 0){};

	string SaveAsCsv(){ return ""; };
	string SqlScript(){ return ""; };

	void NotifyDatabaseImportedToWindow(){};

	/* 从每个的数据节点中获取该节点对应的指标编码 */
	string FindZbCodeFromDataNode(Json::Value  & datanode, string strWdCode);

	/* 根据指标编码从JSON 的底部数据中查找对应的指标名称 */
	void  FindZbNameFromBottom(Json::Value  & wdnodes,  string strWdCode,  string strWdSubCode,
		string & strZbName, string & strUnit);

	bool Logon();

	map<string, string> GetOtherWds(string strDbCode, string strZbCode);
	void GetMenuTree(string strDbCode, string strWdCode, string strWdSubCode);
	void SaveMenuTree();
	CStockPlateTree * GenerateTree(string strDbCode);

	vector<string>	NeedDownload(string strDbCode);
	void  ModifyUpdateDate(string strDbCode, string strZbCode, int nDelay = 0);

	static int WriterOfStatsGov(char *data, size_t size, size_t nmemb, CStatsGovCn * pStatsGovCn);
	static int HeaderOfStatsGov( char *data, size_t size, size_t nmemb, CStatsGovCn * pStatsGovCn);
	string PostUrlOfStatsGov(string url,char* data);

protected:
	static bool m_bLogon;

	/* 保存菜单树 */
	vector<string>	tree_dbcode;
	vector<string>	tree_id;
	vector<bool>	tree_isParent;
	vector<string>	tree_name;
	vector<string>	tree_pid;
	vector<string>	tree_wdcode;
	
	
	/*用来接收数据的缓存*/
	unsigned char * m_buffer ;
	int	m_capacity;
	int	m_size;
	int m_size_log;		//	最后打印日志的时，收到的数据大小

	unsigned char  m_header[1024];
	int m_size_header;

};
