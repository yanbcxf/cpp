
// CostEngineer.cpp: 定义应用程序的类行为。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "CostEngineer.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "CostEngineerDoc.h"
#include "CostEngineerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CMvDocTemplate::CMvDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
	CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass)
	: CMultiDocTemplate(nIDResource, pDocClass, pFrameClass, pViewClass)
{
}

// CCostEngineerApp

BEGIN_MESSAGE_MAP(CCostEngineerApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CCostEngineerApp::OnAppAbout)
	// 基于文件的标准文档命令
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// 标准打印设置命令
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
	ON_COMMAND(ID_CALC, &CCostEngineerApp::OnCalc)
	ON_COMMAND(ID_FIXED_ASSET_DEPRECIATION, &CCostEngineerApp::OnFixedAssetDepreciation)
	ON_COMMAND(ID_ANNUITY_CALC, &CCostEngineerApp::OnAnnuityCalc)
END_MESSAGE_MAP()


// CCostEngineerApp 构造

CCostEngineerApp::CCostEngineerApp() noexcept
{
	m_bHiColorIcons = TRUE;

	// TODO: 将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("CostEngineer.AppID.NoVersion"));

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的 CCostEngineerApp 对象

CCostEngineerApp theApp;


// CCostEngineerApp 初始化

void readCostEngnieerXml(mxml_node_t * tree, CostEngineerInfo & father) {
	mxml_node_t * node, *item_node;
	node = mxmlFindElement(tree, tree, "list", NULL, NULL, MXML_DESCEND);

	if (node) {
		for (item_node = mxmlFindElement(node, tree, "element", NULL, NULL, MXML_DESCEND);
			item_node != NULL;
			item_node = mxmlFindElement(item_node, tree, "element", NULL, NULL, MXML_NO_DESCEND)) {

			CostEngineerInfo info;
			char name[MAX_PATH] = { 0 };
			char code[MAX_PATH] = { 0 };

			node = mxmlFindElement(item_node, tree, "code", NULL, NULL, MXML_DESCEND);
			if (node && mxmlGetText(node, NULL))
			{
				strncpy(code, (const char *)mxmlGetText(node, NULL), MAX_PATH);
				info.code = code;
			}

			node = mxmlFindElement(item_node, tree, "name", NULL, NULL, MXML_DESCEND);
			if (node && mxmlGetText(node, NULL))
			{
				strncpy(name, (const char *)mxmlGetText(node, NULL), MAX_PATH);
				info.name = Utf8_GBK(name);
			}

			readCostEngnieerXml(item_node, info);
			father.list.push_back(info);
		}
	}
}


BOOL CCostEngineerApp::InitInstance()
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

	

	
	///////////////////////////////////////////////////////////////
	//	读入xml 文件进行 菜单 配置

	mxml_node_t * tree, *node, *item_node;
	FILE *fp = NULL;
	errno_t err = fopen_s(&fp, "CostEngineer.xml", "rb");
	if (fp != NULL)
	{
		tree = mxmlLoadFile(NULL, fp, /*MXML_OPAQUE_CALLBACK*/ /*MXML_TEXT_CALLBACK*/  MXML_TEXT_CALLBACK);
		if (tree)
		{
			readCostEngnieerXml(tree, m_cost_engineer_info);
			mxmlDelete(tree);
		}

		fclose(fp);
	}

	char buffer[1024 + 1] = { 0 };
	strncpy(buffer, "<?xml version=\"1.0\" encoding=\"UTF - 8\" standalone=\"yes\" ?><Query><CmdType>Catalog</CmdType><SN>102</SN><DeviceID>14022900002000000003</DeviceID></Query>", 1024);
	
	tree = mxmlLoadString(NULL, buffer, MXML_TEXT_CALLBACK);
	if (NULL == tree)
	{
		printf("%s line=%d mxmlLoadString is NULL. ERROR......\n", __FUNCTION__, __LINE__);
		return -1;
	}

	/*fill ptz_control*/
	node = mxmlFindElement(tree, tree, "CmdType", NULL, NULL, MXML_DESCEND);
	if (NULL == node) {

	}


	/////////////////////////////////////////////////////
	AfxEnableControlContainer();

	EnableTaskbarInteraction();

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
	pDocTemplate = new CMultiDocTemplate(IDR_CostEngineerTYPE,
		RUNTIME_CLASS(CCostEngineerDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CCostEngineerView));
	if (!pDocTemplate)
		return FALSE;
	pDocTemplate->SetContainerInfo(IDR_CostEngineerTYPE_CNTR_IP);
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



	// 调度在命令行中指定的命令。  如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// 主窗口已初始化，因此显示它并对其进行更新
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

int CCostEngineerApp::ExitInstance()
{
	//TODO: 处理可能已添加的附加资源
	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}

// CCostEngineerApp 消息处理程序


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg() noexcept;

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

CAboutDlg::CAboutDlg() noexcept : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CCostEngineerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CCostEngineerApp 自定义加载/保存方法

void CCostEngineerApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);

	GetContextMenuManager()->AddMenu("Component", IDR_POPUP_COMPONENT);
}

void CCostEngineerApp::LoadCustomState()
{
}

void CCostEngineerApp::SaveCustomState()
{
}

// CCostEngineerApp 消息处理程序


BOOL CCostEngineerApp::TestRequestExit()
{
	if (WaitForSingleObject(((CCostEngineerApp *)AfxGetApp())->m_bAppExit, 0) == WAIT_TIMEOUT)
		return FALSE;
	else
		return TRUE;
}


CWinThread * CCostEngineerApp::StartWorkerThread(AFX_THREADPROC pfnThreadProc, LPVOID pParam)
{
	CWinThread * pThread = AfxBeginThread(pfnThreadProc, pParam, \
		THREAD_PRIORITY_BELOW_NORMAL /*THREAD_PRIORITY_NORMAL*/, 1 * 1024 * 1024, CREATE_SUSPENDED);
	pThread->m_bAutoDelete = FALSE;
	//m_vec_pThread.push_back(pThread);
	pThread->ResumeThread();

	return pThread;
}


CDocTemplate* CCostEngineerApp::GetCurrDocTemplate(CView * pView)
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


CDocument * CCostEngineerApp::SearchDoc(int idx)
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
		while (pos2 != NULL)
		{
			//获取下一个文档及位置
			CCostEngineerDoc * pDocTmp = (CCostEngineerDoc *)pDocTemp->GetNextDoc(pos2);
			if (0/*pDocTmp->m_lottery_code == idx*/)
			{
				pDoc = pDocTmp;
				break;
			}
		}

	}

	return pDoc;
}



void CCostEngineerApp::OnCalc()
{
	// TODO: 在此添加命令处理程序代码
	ShellExecute(NULL, "open", "calc", NULL, NULL, SW_SHOW);
}


void CCostEngineerApp::OnFixedAssetDepreciation()
{
	// TODO: 在此添加命令处理程序代码
	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 80;
	infd.CXCOMBOX = 100;
	infd.GROUP_NUM_PER_LINE = 3;
	int g = 0;
	int i = 0;
	double amount = 0;

	i = 0;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("建设投资(含建设期利息)"), 64);
	infd.m_vecFindItem[0][i][0].dbMin = -0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 1000000;


	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("固定资产进项税额"), 64);
	infd.m_vecFindItem[0][i][0].dbMin = 0;
	infd.m_vecFindItem[0][i][0].dbMax = 1000000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("使用年限"), 64);
	infd.m_vecFindItem[0][i][0].dbMin = 0;
	infd.m_vecFindItem[0][i][0].dbMax = 1000000;


	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("期末净残值率(%)"), 64);
	infd.m_vecFindItem[0][i][0].dbMin = 0;
	infd.m_vecFindItem[0][i][0].dbMax = 1000000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("剩余使用年限"), 64);
	infd.m_vecFindItem[0][i][0].dbMin = 0;
	infd.m_vecFindItem[0][i][0].dbMax = 1000000;

	infd.Init(_T("参数设置"), _T("参数设置"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		double v1 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
		double v2 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());

		double v3 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
		double rate = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer()) / 100;
		double v4 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());

		double v = (v1 - v2) * (1 - rate) / v3;
		double v5 = v * v4 + (v1 - v2) * rate;
		CString source;
		source.Format("年折旧费:  %.3f ,  固定资产余值: %.3f", v, v5);
		AfxMessageBox(source);
		paste(source);
		// m_amount_of_money = v;

	}
}


void CCostEngineerApp::OnAnnuityCalc()
{
	// TODO: 在此添加命令处理程序代码
	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 80;
	infd.CXCOMBOX = 100;
	infd.GROUP_NUM_PER_LINE = 3;
	int g = 0;
	int i = 0;
	double amount = 0;

	i = 0;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("现值"), 64);
	infd.m_vecFindItem[0][i][0].dbMin = 0.00001;
	infd.m_vecFindItem[0][i][0].dbMax = 1000000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("年利率(%)"), 64);
	infd.m_vecFindItem[0][i][0].dbMin = 0.001;
	infd.m_vecFindItem[0][i][0].dbMax = 1000000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("年份数"), 64);
	infd.m_vecFindItem[0][i][0].nMin = 1;
	infd.m_vecFindItem[0][i][0].nMax = 1000000;

	infd.Init(_T("参数设置"), _T("参数设置"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		double v1 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
		double rate = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer()) / 100;
		int n = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());

		double k = Annuity2Present(rate, n, 3);
		double a = v1 / k;

		CString source;
		source.Format("年金:  %.3f", a);
		AfxMessageBox(source);

		source.Format("%.3f", a);
		paste(source);
		// m_amount_of_money = v;

	}
}
