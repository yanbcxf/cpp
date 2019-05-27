
// GB28181ClientMDI.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "afxwinappex.h"



#include "GB28181ClientMDI.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "GB28181ClientMDIDoc.h"
#include "GB28181ClientMDIView.h"

#include "RealplayView.h"
#include "PlaybackView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////

int Dll_search_recordfile_response_Callback(int commandport, int type, int result, 
											sipd_recordinfo_rsp_basic_info* pStateBasicInfo, sipd_file_item* pFileItem)
{
	HWND hwnd = NULL;
	{
		JMutexAutoLock lock(g_mtxNet);
		if(g_mapNet2Hwnd.count(commandport)>0)
			hwnd = g_mapNet2Hwnd[commandport];
	}

	if(hwnd)
	{
		if(type==1)
		{
			// 返回数据

			sipd_recordinfo_rsp_basic_info * pBasicInfo = new sipd_recordinfo_rsp_basic_info;
			memcpy(pBasicInfo, pStateBasicInfo, sizeof(sipd_recordinfo_rsp_basic_info));

			sipd_file_item * pItem = new sipd_file_item[pBasicInfo->RecordListNum];
			for(int i=0; i< pBasicInfo->RecordListNum; i++)
			{
				memcpy(pItem + i, pFileItem + i, sizeof(sipd_file_item));
			}

			::PostMessage(hwnd, WM_USER_RECORDFILE_DATA, (WPARAM)pBasicInfo, (LPARAM)pItem);
		}
		else
		{
			::PostMessage(hwnd, WM_USER_RECORDFILE_COMM, (WPARAM)result, 0);
		}
	}
	return 0;
}

int Dll_cataloginfo_response_Callback(int commandport, int type, int result, 
									  sipd_catalog_rsp_basic_info* pCataBasicInfo, sipd_catalog_item* pCataItem)
{
	HWND hwnd = NULL;
	{
		JMutexAutoLock lock(g_mtxNet);
		if(g_mapNet2Hwnd.count(commandport)>0)
			hwnd = g_mapNet2Hwnd[commandport];
	}
	if(hwnd)
	{
		if(type==1)
		{
			// 返回数据
			
			sipd_catalog_rsp_basic_info * pBasicInfo = new sipd_catalog_rsp_basic_info;
			memcpy(pBasicInfo, pCataBasicInfo, sizeof(sipd_catalog_rsp_basic_info));

			sipd_catalog_item * pItem = new sipd_catalog_item[pBasicInfo->DeviceList_Num];
			for(int i=0; i< pBasicInfo->DeviceList_Num; i++)
			{
				memcpy(pItem + i, pCataItem + i, sizeof(sipd_catalog_item));
			}

			::PostMessage(hwnd, WM_USER_CATALOG_DATA, (WPARAM)pBasicInfo, (LPARAM)pItem);
		}
		else
		{
			::PostMessage(hwnd, WM_USER_CATALOG_COMM, (WPARAM)result, 0);
		}
	}
	
	return 0;
}


int Dll_Call_Response_Callback(int netport, int type, int result, unsigned char * pBuffer, int nBufSize)
{
	static FILE * fp = NULL;
	if(type ==0 )
	{
	
		HWND hwnd = NULL;
		{
			/*JMutexAutoLock lock(g_mtxNet);
			if(g_mapNet2Hwnd.count(netport)>0)
				hwnd = g_mapNet2Hwnd[netport];*/
		}
		if(hwnd)
		{
			::PostMessage(hwnd, WM_USER_CALL_RESP, (WPARAM)result, 0);
		}
	}
	else if(type == 1)
	{
		//	数据流回调
		int playerid = -1;
		{
			JMutexAutoLock lock(g_mtxNet);
			if(g_mapNet2Player.count(netport)>0)
				playerid = g_mapNet2Player[netport];
		}
		if(playerid > 0)
		{
			H264_PARAM * pHeader = (H264_PARAM *)pBuffer;
			if(pHeader->knd==0)
			{
				/* 回调出来的 pts 以 us 为单位， 而播放器需要以 time_base 为单位， 在函数avpriv_set_pts_info 中设置的 */
				/* 计算的结果 每个 new_pts 时间戳应该相差 3600 左右 */

				u_int64 new_pts = pHeader->pts/(1000000.0/90000);
				stringstream ss;
				ss << "Video  pts = " << pHeader->pts << ", new_pts = " <<  new_pts << ", playerid = " << playerid; 
				//LOG4CPLUS_DEBUG(g_Logger, ss.str() );

				player_stream_input_data(playerid, PLAYER_MEDIA_TYPE_VIDEO, pBuffer+ 72, nBufSize-76, new_pts /*0x8000000000000000*/);
			}
			else if(pHeader->knd!=10)
			{
				u_int64 new_pts = pHeader->pts/(1000000.0/8000);
				stringstream ss;
				ss << "Audio  pts = " << pHeader->pts << ", new_pts = " <<  new_pts << ", playerid = " << playerid;
				//LOG4CPLUS_DEBUG(g_Logger, ss.str() );

				player_stream_input_data(playerid, PLAYER_MEDIA_TYPE_AUDIO, pBuffer+ 72, nBufSize-76, new_pts);
			}

		}
		else
		{
			/*if(fp==NULL)
			{
				fp = fopen("gb_28181_client.snv", "wb");
			}
			if(fp)
				fwrite(pBuffer, 1, nBufSize, fp);*/
		}		
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// CGB28181ClientMDIApp

BEGIN_MESSAGE_MAP(CGB28181ClientMDIApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CGB28181ClientMDIApp::OnAppAbout)
	// 基于文件的标准文档命令
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// 标准打印设置命令
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// CGB28181ClientMDIApp 构造

CGB28181ClientMDIApp::CGB28181ClientMDIApp()
{

	m_bHiColorIcons = TRUE;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的一个 CGB28181ClientMDIApp 对象

CGB28181ClientMDIApp theApp;


// CGB28181ClientMDIApp 初始化

BOOL CGB28181ClientMDIApp::InitInstance()
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


	//////////////////////////////////////////////////////////////////////////

	g_mtxNet.Init();

	//	初始化log4cplus
	PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));	
	g_Logger =  Logger::getInstance(LOG4CPLUS_TEXT("Test_28181"));

	SIPD_GATEWAY_CFG cfg;
	memset(&cfg, 0, sizeof(SIPD_GATEWAY_CFG));
	strncpy(cfg.local_domain_name, "3402000000", MAX_SIPD_DEVICE_ID_LEN);
	strncpy(cfg.local_domain_id, LOCAL_DOMAIN_ID, MAX_SIPD_DEVICE_ID_LEN);		//	202 媒体分发服务器
	cfg.local_port = LOCAL_PORT;
	strncpy(cfg.local_ip, LOCAL_IP, MAX_SIPD_NORMAL_LEN);			//	此处可填 外网映射 IP 
	strncpy(cfg.local_gateway, "10.18.72.254", MAX_SIPD_NORMAL_LEN);

	strncpy(cfg.upper_server_name, "35010400", MAX_SIPD_DEVICE_ID_LEN);
	strncpy(cfg.upper_server_id, "35010400002000000005", MAX_SIPD_DEVICE_ID_LEN);		//	200 中小信令控制服务器
	strncpy(cfg.upper_server_ip, "10.18.72.35", MAX_SIPD_NORMAL_LEN);
	//strncpy(cfg.upper_server_ip, "192.168.1.109", MAX_SIPD_NORMAL_LEN);
	cfg.upper_server_port = 5068;
	strncpy(cfg.register_upper_server_passwd, "12345678", MAX_SIPD_NORMAL_LEN);

	cfg.register_keepalive = 3600;
	cfg.keepalive_internal = 60;


	DLL_Init(&cfg);
	//Register_Dll_Register_Request(Dll_Register_Request_Callback);
	Register_Dll_cataloginfo_response(Dll_cataloginfo_response_Callback);

	//Register_Dll_deviceinfo_response(Dll_deviceinfo_response_Callback);
	//Register_Dll_stateinfo_response(Dll_stateinfo_response_Callback);

	//Register_Dll_Alarm_Request(Dll_Alarm_Request_Callback);

	Register_Dll_search_recordfile_response(Dll_search_recordfile_response_Callback);
	//Register_Dll_ptz_response(Dll_ptz_response_Callback);

	Register_Dll_Call_Response(Dll_Call_Response_Callback);

	//	初始化 播放器
	player_init();


	//////////////////////////////////////////////////////////////////////////

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

	// 注册应用程序的文档模板。文档模板
	// 将用作文档、框架窗口和视图之间的连接
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_GB28181ClientMDTYPE,
		RUNTIME_CLASS(CGB28181ClientMDIDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CGB28181ClientMDIView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(IDR_GB28181ClientMDTYPE,
		RUNTIME_CLASS(CGB28181ClientMDIDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CRealplayView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(IDR_GB28181ClientMDTYPE,
		RUNTIME_CLASS(CGB28181ClientMDIDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CPlaybackView));
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

	//	修改命令，避免一启动就建立新文档
	cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;

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
void CGB28181ClientMDIApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CGB28181ClientMDIApp 自定义加载/保存方法

void CGB28181ClientMDIApp::PreLoadState()
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

void CGB28181ClientMDIApp::LoadCustomState()
{
}

void CGB28181ClientMDIApp::SaveCustomState()
{
}



CDocTemplate* CGB28181ClientMDIApp::GetCurrDocTemplate(CView * pView)
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
		if(pView->IsKindOf(pClassView))	
		{
			return pDocTemp;
		}	
	}
	return NULL;
}

// CGB28181ClientMDIApp 消息处理程序



