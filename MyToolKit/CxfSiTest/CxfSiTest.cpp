
// CxfSiTest.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "CxfSiTest.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "CxfSiTestDoc.h"
#include "CxfSiTestView.h"

#include "UsageEnvironment.h"
#include "GbUsageEnvironment.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCxfSiTestApp

BEGIN_MESSAGE_MAP(CCxfSiTestApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CCxfSiTestApp::OnAppAbout)
	// 基于文件的标准文档命令
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// 标准打印设置命令
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// CCxfSiTestApp 构造

CCxfSiTestApp::CCxfSiTestApp()
{

	m_bHiColorIcons = TRUE;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的一个 CCxfSiTestApp 对象

CCxfSiTestApp theApp;


// CCxfSiTestApp 初始化

BOOL CCxfSiTestApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
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
	AfxEnableControlContainer();
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

	// 初始化log4cplus
	PropertyConfigurator::doConfigure("log4cplus.properties");

	g_strLocalDomainID = "35010400002000000005";

	//	启动 Sip 服务
	SIPD_GATEWAY_CFG cfg;
	memset(&cfg, 0, sizeof(SIPD_GATEWAY_CFG));
	strncpy(cfg.local_domain_name, "35010400", MAX_SIPD_DEVICE_ID_LEN);
	strncpy(cfg.local_domain_id, g_strLocalDomainID.c_str(), MAX_SIPD_DEVICE_ID_LEN);
	cfg.local_port = 5068;
	/*strncpy(cfg.local_ip, "192.168.1.105", MAX_SIPD_NORMAL_LEN);
	strncpy(cfg.local_gateway, "192.168.1.1", MAX_SIPD_NORMAL_LEN);*/
	strncpy(cfg.local_ip, "10.18.72.35", MAX_SIPD_NORMAL_LEN);
	strncpy(cfg.local_gateway, "10.18.72.254", MAX_SIPD_NORMAL_LEN);

	strncpy(cfg.upper_server_name, "43000000", MAX_SIPD_DEVICE_ID_LEN);
	strncpy(cfg.upper_server_id, "43000000002000000000", MAX_SIPD_DEVICE_ID_LEN);
	strncpy(cfg.upper_server_ip, "10.18.72.35", MAX_SIPD_NORMAL_LEN);
	//strncpy(cfg.upper_server_ip, "192.168.1.109", MAX_SIPD_NORMAL_LEN);
	cfg.upper_server_port = 5060;
	strncpy(cfg.register_upper_server_passwd, "12345678", MAX_SIPD_NORMAL_LEN);
	cfg.register_keepalive = 3600;
	cfg.keepalive_internal = 60;

	//	启动 sip 消息处理
	CGbUsageEnvironment::getInstance()->init(cfg);

	sipd_init(&cfg);

	

	// 注册应用程序的文档模板。文档模板
	// 将用作文档、框架窗口和视图之间的连接
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_CxfSiTestTYPE,
		RUNTIME_CLASS(CCxfSiTestDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CCxfSiTestView));
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
	// 仅当具有后缀时才调用 DragAcceptFiles
	//  在 MDI 应用程序中，这应在设置 m_pMainWnd 之后立即发生


	// 分析标准外壳命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);


	// 调度在命令行中指定的命令。如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// 主窗口已初始化，因此显示它并对其进行更新
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}



// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CCxfSiTestApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CCxfSiTestApp 自定义加载/保存方法

void CCxfSiTestApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

void CCxfSiTestApp::LoadCustomState()
{
}

void CCxfSiTestApp::SaveCustomState()
{
}

// CCxfSiTestApp 消息处理程序



