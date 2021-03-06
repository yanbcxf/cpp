
// Lottery.cpp: 定义应用程序的类行为。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "Lottery.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "LotteryDoc.h"
#include "LotteryView.h"
#include "LotteryNumberGridView.h"
#include "TrendDistributionGridView.h"

#include "UrlUtils.h"

#include "LottoMxn.h"

#include "ChartDlg.h"

extern UINT AutoDownloadThreadFunc(LPVOID param);



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
//

CMvDocTemplate::CMvDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
	CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass)
	: CMultiDocTemplate(nIDResource, pDocClass, pFrameClass, pViewClass)
{
}

// CLotteryApp

BEGIN_MESSAGE_MAP(CLotteryApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CLotteryApp::OnAppAbout)
	// 基于文件的标准文档命令
	//ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
END_MESSAGE_MAP()


// CLotteryApp 构造

CLotteryApp::CLotteryApp()
{
	m_bHiColorIcons = TRUE;

	m_bRunning = FALSE;

	// TODO: 将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	// SetAppID(_T("Lottery.AppID.NoVersion"));

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的 CLotteryApp 对象

CLotteryApp theApp;


// CLotteryApp 初始化

BOOL CLotteryApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	std::locale prev_loc = std::locale::global(std::locale("chs"));
	////////////////////////////////////////////////////////////////
	// 初始化log4cplus

	stringstream ss;
	/*PropertyConfigurator::doConfigure("log4cplus.properties");
	g_logger = Logger::getInstance("Stock");

	LOG4CPLUS_INFO(g_logger, "==============  程序开始 ============");*/

	m_bAppExit = CreateEvent(NULL, TRUE, FALSE, NULL);
	g_mtx_igraph.Init();
	//////////////////////////////////////////////////////////////////////////
	//	启动 MYSQL 数据库的连接池

	try {
		string connectString_ = "dbname=lottery  port=3306   host=127.0.0.1 user=root  charset=utf8 ";   // utf8
		backend_factory const &backEndFactory_ = *soci::factory_mysql();

		for (std::size_t i = 0; i != MYSQL_POOL_SIZE; ++i)
		{
			session & sql = g_MysqlPool.at(i);
			sql.open(backEndFactory_, connectString_);
		}
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << "Error ！初始化 Mysql 连接池失败，请重启尝试";
		ss << e.what() << ' \r\n';
		AfxMessageBox(ss.str().c_str());
		return FALSE;
	}

	///////////////////////////////////////////////////////////////
	//	读入Lottery 配置

	mxml_node_t * tree, *node, *item_node;
	FILE *fp = NULL;
	errno_t err = fopen_s(&fp, "Lottery.xml", "rb");
	if (fp != NULL)
	{
		tree = mxmlLoadFile(NULL, fp, /*MXML_OPAQUE_CALLBACK*/ /*MXML_TEXT_CALLBACK*/  MXML_TEXT_CALLBACK);
		if (tree)
		{
			node = mxmlFindElement(tree, tree, "LotteryList", NULL, NULL, MXML_DESCEND);

			for (item_node = mxmlFindElement(node, tree, "Lottery", NULL, NULL, MXML_DESCEND);
				item_node != NULL;
				item_node = mxmlFindElement(item_node, tree, "Lottery", NULL, NULL, MXML_DESCEND))
			{
				LotteryInfo info;
				char name[MAX_PATH] = { 0 };
				char code[MAX_PATH] = { 0 };

				node = mxmlFindElement(item_node, tree, "Code", NULL, NULL, MXML_DESCEND);
				if (node && mxmlGetText(node, NULL))
				{
					strncpy(code, (const char *)mxmlGetText(node, NULL), MAX_PATH);
					info.code = code;
				}

				node = mxmlFindElement(item_node, tree, "Name", NULL, NULL, MXML_DESCEND);
				if (node && mxmlGetText(node, NULL))
				{
					strncpy(name, (const char *)mxmlGetText(node, NULL), MAX_PATH);
					info.name = Utf8_GBK(name);
				}

				node = mxmlFindElement(item_node, tree, "TotalNum", NULL, NULL, MXML_DESCEND);
				if (node && mxmlGetText(node, NULL))
					info.totalnum = atoi((const char *)mxmlGetText(node, NULL));

				node = mxmlFindElement(item_node, tree, "RegularNum", NULL, NULL, MXML_DESCEND);
				if (node && mxmlGetText(node, NULL))
					info.regularnum = atoi((const char *)mxmlGetText(node, NULL));

				node = mxmlFindElement(item_node, tree, "SpecialNum", NULL, NULL, MXML_DESCEND);
				if (node && mxmlGetText(node, NULL))
					info.specialnum = atoi((const char *)mxmlGetText(node, NULL));

				if (info.name.empty() == false && info.totalnum > 0 
					&& info.regularnum > 0 && info.code.empty() == false && info.specialnum<=1)
					m_vec_lottery_info.insert(m_vec_lottery_info.end(), info);

			}
			mxmlDelete(tree);
		}

		fclose(fp);
	}

	////////////////////////////////////////////////////////////////
	extern void consecutive_number(int * p, int nCount, int(&Consecutive0)[5][10],
		int(&Consecutive1)[5][10], int(&Consecutive2)[5][10]);

	int vec0[5][10];
	int vec1[5][10];
	int vec2[5][10];
	int ln[7] = { 4, 10, 19, 20, 22, 0, 0 };

	int gg = group_number(ln, 5, NUM_PER_LINE);

	consecutive_number(ln, 7, vec0, vec1, vec2);

	LotteryNumber lotto;
	lotto.number[0] = 1345;
	lotto.number[1] = 1301;
	lotto.number[2] = 1368;
	lotto.number[3] = 1322;
	lotto.number[4] = 1310;
	lotto.number[5] = 1370;
	lotto.number[6] = 1318;
	lotto.number[7] = 1350;
	lotto.number[8] = 1303;
	lotto.number[9] = 1299;

	lotto.number[0] = 6;
	lotto.number[1] = 10;
	lotto.number[2] = 12;
	lotto.number[3] = 13;
	lotto.number[4] = 17;
	lotto.number[5] = 21;
	lotto.number[6] = 28;
	lotto.number[7] = 11;

	lotto.number[8] = 4;
	lotto.number[9] = 7;

	extern void StatisticCharacter(int * nNumber, int regular_num, int total_num, LotteryNumber & lotto);

	StatisticCharacter(lotto.number, 7, 31, lotto);

	////////////////////////////////////////////////////////////////
	AfxEnableControlContainer();

	// EnableTaskbarInteraction();

	// 使用 RichEdit 控件需要 AfxInitRichEdit2()
	// AfxInitRichEdit2();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	LoadStdProfileSettings(4);  // 加载标准 INI 文件选项(包括 MRU)

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// 注册应用程序的文档模板。  文档模板
	// 将用作文档、框架窗口和视图之间的连接
	CMultiDocTemplate* pDocTemplate;
	/*pDocTemplate = new CMvDocTemplate(IDR_LotteryTYPE,
		RUNTIME_CLASS(CLotteryDoc),
		RUNTIME_CLASS(CChildFrame), 
		RUNTIME_CLASS(CLotteryView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);*/

	pDocTemplate = new CMvDocTemplate(IDR_LOTTERY_NUMBER_GRID,  /*IDR_StockTYPE*/
		RUNTIME_CLASS(CLotteryDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CLotteryNumberGridView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMvDocTemplate(IDR_TREND_DISTRIBUTION_GRID,  /*IDR_StockTYPE*/
		RUNTIME_CLASS(CLotteryDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CTrendDistributionGridView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// 创建主 MDI 框架窗口
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;


	// 分析标准 shell 命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	//	修改命令，避免一启动就建立新文档
	cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;

	// 调度在命令行中指定的命令。  如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// 主窗口已初始化，因此显示它并对其进行更新
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	//	启动下载
	/*CDownloadRequest * req1 = new CDownloadRequest;
	req1->m_hwnd = NULL;
	req1->m_nLogType = WM_USER_LOG_2;
	((CLotteryApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc, req1);*/

	//////////////////////////////////////////////////////////////////////////
	//	启动计算任务执行模块

	CCalculateTask::init();

	return TRUE;
}

int CLotteryApp::ExitInstance()
{
	//TODO: 处理可能已添加的附加资源
	AfxOleTerm(FALSE);

	//	设置信号量，停止所有线程
	SetEvent(m_bAppExit);

	return CWinAppEx::ExitInstance();
}

// CLotteryApp 消息处理程序


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CLotteryApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CLotteryApp 自定义加载/保存方法

void CLotteryApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);

	GetContextMenuManager()->AddMenu("Lottery", IDR_POPUP_LOTTERY);
}

void CLotteryApp::LoadCustomState()
{
}

void CLotteryApp::SaveCustomState()
{
}

// CLotteryApp 消息处理程序


BOOL CLotteryApp::TestRequestExit()
{
	if (WaitForSingleObject(((CLotteryApp *)AfxGetApp())->m_bAppExit, 0) == WAIT_TIMEOUT)
		return FALSE;
	else
		return TRUE;
}


CWinThread * CLotteryApp::StartWorkerThread(AFX_THREADPROC pfnThreadProc, LPVOID pParam)
{
	CWinThread * pThread = AfxBeginThread(pfnThreadProc, pParam, \
		THREAD_PRIORITY_BELOW_NORMAL /*THREAD_PRIORITY_NORMAL*/, 1 * 1024 * 1024, CREATE_SUSPENDED);
	pThread->m_bAutoDelete = FALSE;
	//m_vec_pThread.push_back(pThread);
	pThread->ResumeThread();

	return pThread;
}


CDocTemplate* CLotteryApp::GetCurrDocTemplate(CView * pView)
{
	ASSERT_VALID(pView);
	// CDocTemplate* pDocTemp = NULL;
	CMvDocTemplate * pDocTemp = NULL;


	// Get the first document template position and iterate through the document template
	POSITION posDocTemplate = GetFirstDocTemplatePosition();
	while (posDocTemplate != NULL)
	{
		pDocTemp = (CMvDocTemplate *)GetNextDocTemplate(posDocTemplate);
		ASSERT_VALID(pDocTemp);

		CRuntimeClass * pClassView = pDocTemp->GetCurrView();
		if (pView->IsKindOf(pClassView))
		{
			return pDocTemp;
		}
	}
	return NULL;
}


CDocument * CLotteryApp::SearchDoc(int idx)
{
	CDocument * pDoc = NULL;

	CMvDocTemplate * pDocTemp = NULL;


	// Get the first document template position and iterate through the document template
	POSITION posDocTemplate = GetFirstDocTemplatePosition();
	while (posDocTemplate != NULL)
	{
		pDocTemp = (CMvDocTemplate *)GetNextDocTemplate(posDocTemplate);
		ASSERT_VALID(pDocTemp);

		//获得与模板相连的第一个文档的位置
		POSITION pos2 = pDocTemp->GetFirstDocPosition();
		//如果成功，则计数添加1
		while (pos2!= NULL)
		{
			//获取下一个文档及位置
			CLotteryDoc * pDocTmp =(CLotteryDoc *)pDocTemp->GetNextDoc(pos2);
			if (0/*pDocTmp->m_lottery_code == idx*/)
			{
				pDoc = pDocTmp;
				break;
			}
		}
		
	}

	return pDoc;
}