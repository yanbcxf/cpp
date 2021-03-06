
// LotteryDoc.h: CLotteryDoc 类的接口
//


#pragma once


class CLotteryDoc : public CDocument
{
protected: // 仅从序列化创建
public:
	CLotteryDoc();
	DECLARE_DYNCREATE(CLotteryDoc)

// 特性
public:
	LotteryInfo	m_lottery_info;
	bool	m_bContainSpecial;

	int		m_nOperType;

	//	数据源
	CString	m_strDataSource;
	//  过滤源
	CString m_strFilterSource;
	vector<vector<string>> m_vecFilterArrayData;
		
	// m_region_n_start[0] ~ m_region_n_end[0] 代表第 n 个区间的范围
	// m_region_n_start[x] ~ m_region_n_end[x] 代表第 n 个区间的第 x 个的个数设置
	vector<int> m_region_1_start, m_region_2_start, m_region_3_start, m_region_4_start, m_region_5_start, m_region_6_start;
	vector<int> m_region_1_end, m_region_2_end, m_region_3_end, m_region_4_end, m_region_5_end, m_region_6_end;

	vector<string>	m_adjacent_totality_1, m_adjacent_totality_3;
	vector<int>  m_adjacent_totality_2,  m_adjacent_totality_4;
	vector<string> m_adjacent_zone_1, m_adjacent_zone_3;
	vector<int>  m_adjacent_zone_2,  m_adjacent_zone_4;
	vector<string> m_adjacent_sum_1;
	vector<int>  m_adjacent_sum_2;

	vector<int>	m_sum_start, m_sum_end;
	vector<int>	m_odd;
	map<int, int>	m_tail_1, m_tail_2, m_tail_3, m_tail_4, m_tail_5, m_tail_6;
	map<int, int>	m_consecutive_1, m_consecutive_2, m_consecutive_3, m_consecutive_4, m_consecutive_5, m_consecutive_6;

	int	m_fred_select;
	vector<double>	m_fred_alpha, m_fred_beta, m_fred_upsilon, m_fred_theta, m_fred_eta;
	map<int, double>	m_fred_start, m_fred_end;
	vector<int>	m_ac_start, m_ac_end;
	vector<int>	m_grp_qty_start, m_grp_qty_end;
	vector<int>	m_fn_qty_start, m_fn_qty_end;


	vector<double>	m_var_start,	m_var_end;
	vector<double>	m_devsq_start, m_devsq_end;
	vector<double>	m_stdev_start, m_stdev_end;
	vector<double>	m_kurt_start,	m_kurt_end;
	vector<double>	m_skew_start, m_skew_end;
	
	int				m_filter_start, m_filter_end;
	map<int, int>	m_filter_arithmetic_correct;
	map<int, int>	m_filter_equal_number;

	//	π赋值
	double			m_pai_value;
	int				m_pai_analysis_periods;
	int				m_pai_start, m_pai_end;
	int				m_pai_period_difference;
	double			m_pai_mul_adjunstment;
	vector<double>	m_pai_add_start, m_pai_add_end;
	vector<double>	m_pai_mul_start, m_pai_mul_end;

	//	lnπ赋值
	double			m_ln_pai_value;
	int				m_ln_pai_analysis_periods;
	int				m_ln_pai_start, m_ln_pai_end;
	int				m_ln_pai_period_difference;
	double			m_ln_pai_mul_adjunstment;
	vector<double>	m_ln_pai_add_start, m_ln_pai_add_end;
	vector<double>	m_ln_pai_mul_start, m_ln_pai_mul_end;

	//	涨跌动力（Swing Power）赋值
	int				m_swing_analysis_periods;
	int				m_swing_start, m_swing_end;
	double			m_swing_mul_adjunstment;
	vector<double>	m_swing_add_start, m_swing_add_end;
	vector<double>	m_swing_mul_start, m_swing_mul_end;

	//	ln 涨跌动力（Swing Power）赋值
	int				m_ln_swing_analysis_periods;
	int				m_ln_swing_start, m_ln_swing_end;
	double			m_ln_swing_mul_adjunstment;
	vector<double>	m_ln_swing_add_start, m_ln_swing_add_end;
	vector<double>	m_ln_swing_mul_start, m_ln_swing_mul_end;

	//	伯努利赋值
	int				m_bernoulli_analysis_periods;
	int				m_bernoulli_start, m_bernoulli_end;
	int				m_bernoulli_period_difference;
	double			m_bernoulli_mul_adjunstment;
	vector<double>	m_bernoulli_add_start, m_bernoulli_add_end;
	vector<double>	m_bernoulli_mul_start, m_bernoulli_mul_end;

	//  势能赋值
	int				m_potential_analysis_periods;
	int				m_potential_start, m_potential_end;
	int				m_potential_period_difference;
	double			m_potential_mul_adjunstment;
	vector<double>	m_potential_add_start, m_potential_add_end;
	vector<double>	m_potential_mul_start, m_potential_mul_end;
	int				m_potential_period_absolute;

	//	各种过滤效果的统计
	map<string, int>	m_filter_statistic_input;
	map<string, int>	m_filter_statistic_output;

// 操作
public:

// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 实现
public:
	virtual ~CLotteryDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 用于为搜索处理程序设置搜索内容的 Helper 函数
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);

	string SqlScript();
	BOOL SaveDocumentIntoDB();
	BOOL LoadDocumentFromDB();

	string SelectClause(string strDataSource = "");
	string WhereSum();
	string WhereOdd();
	string WhereAc();
	string WhereGrpQty();
	string WhereFnQty();
	string WhereVar();
	string WhereDevsq();
	string WhereStdev();
	string WhereKurt();
	string WhereSkew();
	string WhereAdjacentSum();
	string WhereAdjacentTotality();
	string WhereAdjacentInterval();
	string WhereRegion();
	string WhereArithmeticCorrect();

	void FilterStatistic(string filterName, int nRet);
	void FilterStatisticPrint();

	int ConsecutiveFilter(string consecutive0, string consecutive1, string consecutive2);
	int TailFilter(string equal_tail);
	int FredFilter(int ac, int sum, int fringe_no_qty, int group_qty);
	int EqualNumberFilter(vector<LotteryNumber> & vec, int number[]);
	int ArithmeticCorrectFilter(vector<LotteryNumber> & vec, int number[]);
	int RegularExpressFilter(LotteryNumber ln);

	string LotteryLevel(int nLevel);
	int WinLotteryQuery(int * query, int * std, string &);
	vector<LotteryNumber> FetchAwardRecord();

	map<int, Assignment> AssignmentFilterPre(vector<LotteryNumber> & vec1, int periods_start, int periods_end, int goul_count_control);
	void AssignmentFilterLog(map<int, Assignment> mapPai, string type, LotteryNumber ln);
	int PaiFilter(map<int, Assignment> & map1, LotteryNumber ln, bool bLog = false);
	int LnPaiFilter(map<int, Assignment> & map1, LotteryNumber ln, bool bLog = false);
	int SwingPowerFilter(map<int, Assignment> & map1, LotteryNumber ln, bool bLog = false);
	int LnSwingPowerFilter(map<int, Assignment> & map1, LotteryNumber ln, bool bLog = false);
	int BernoulliFilter(map<int, Assignment> & map1, LotteryNumber ln, bool bLog = false);
	int PotentialFilter(map<int, Assignment> & map1, LotteryNumber ln, bool bLog = false);
};
