#pragma once

class CCalcCapiStructDownload : public CCalculateTask
{
public:
	CCalcCapiStructDownload(HWND hwnd, int logType);
	~CCalcCapiStructDownload(void);

	string ClassName(){ return "CCalcCapiStructDownload"; }

	void NeedDownload();
	static void NeedDownload1(void* firstArg);
public:
	vector<int>		m_vec_code			;	//	"¹«Ë¾´úÂë",
};
