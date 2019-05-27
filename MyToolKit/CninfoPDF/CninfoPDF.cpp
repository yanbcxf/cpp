
// CninfoPDF.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "CninfoPDF.h"
#include "TabDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCninfoPDFApp

BEGIN_MESSAGE_MAP(CCninfoPDFApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CCninfoPDFApp 构造

CCninfoPDFApp::CCninfoPDFApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CCninfoPDFApp 对象

CCninfoPDFApp theApp;


// CCninfoPDFApp 初始化

BOOL CCninfoPDFApp::InitInstance()
{
//TODO: call AfxInitRichEdit2() to initialize richedit2 library.
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

	AfxEnableControlContainer();

	if( ! ::AfxInitRichEdit() )
	{
		return FALSE ;
	}

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	//////////////////////////////////////////////////////////////////////////
	//	启动 MYSQL 数据库的连接池, 采用 GBK 客户端

	stringstream ss;
	try{
		string connectString_ =  "dbname=stock  port=3366   host=127.0.0.1 user=root  charset=gbk ";
		backend_factory const &backEndFactory_ = *soci::factory_mysql();

		for (std::size_t i = 0; i != MYSQL_POOL_SIZE; ++i)
		{
			session & sql = g_MysqlPool.at(i);
			sql.open(backEndFactory_, connectString_);
		}
	}catch (std::exception const & e)
	{
		ss.str("");
		ss << "Error ！初始化 Mysql 连接池失败，请在桌面快捷方式打开 Mysql 数据库";
		ss << e.what() << ' \r\n';
		AfxMessageBox(ss.str().c_str());
		return FALSE;
	}

	//////////////////////////////////////////////////////////////////////////

	g_friso = friso_new();
	g_config = friso_new_config();

	if ( friso_init_from_ifile(g_friso, g_config, "friso.ini") != 1 ) {
		printf("fail to initialize friso and config.");
		return FALSE;
	}
	

	//////////////////////////////////////////////////////////////////////////
	

	CTabDialog dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}

	friso_free_config(g_config);
	friso_free(g_friso);
	
	TerminateProcess(GetCurrentProcess(), 1);  

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}


CWinThread * CCninfoPDFApp::StartWorkerThread(AFX_THREADPROC pfnThreadProc, LPVOID pParam)
{
	CWinThread * pThread = AfxBeginThread(pfnThreadProc, pParam, \
		/*THREAD_PRIORITY_BELOW_NORMAL*/ THREAD_PRIORITY_NORMAL, 1 * 1024 * 1024, CREATE_SUSPENDED);
	pThread->m_bAutoDelete = FALSE;
	pThread->ResumeThread();

	return pThread;
}