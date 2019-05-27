// TabDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "CninfoPDF.h"
#include "TabDialog.h"

#include "CompileUtf8RangeDlg.h"
#include "CninfoPDFDlg.h"

// CTabDialog 对话框

IMPLEMENT_DYNAMIC(CTabDialog, CDialog)

CTabDialog::CTabDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CTabDialog::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CTabDialog::~CTabDialog()
{
}

void CTabDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_TabCtrl);
}


BEGIN_MESSAGE_MAP(CTabDialog, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP

	ON_MESSAGE(WM_NOTIFY_FROM_TRAY,OnNotifyIcon)
	ON_MESSAGE(WM_USER_LOG_1,OnUserLogWParam_1)
	ON_WM_DESTROY()
	ON_WM_SYSCOMMAND()
	ON_WM_TIMER()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CTabDialog 消息处理程序

BOOL CTabDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	//////////////////////////////////////////////////////////////////////////
	//	开启自动下载 PDF 并分析线程

	((CCninfoPDFApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc, NULL);	

	ToTray();

	//	启动时隐藏窗口
	SetTimer(1, 100, 0);


	//////////////////////////////////////////////////////////////////////////
	CCompileUtf8RangeDlg * dialog1 = new CCompileUtf8RangeDlg;
	CCninfoPDFDlg * dialog2 = new CCninfoPDFDlg;

	m_TabCtrl.AddPage("巨潮网PDF 分析", dialog2, IDD_CNINFOPDF_DIALOG);
	m_TabCtrl.AddPage("正则表达式", dialog1, IDD_COMPILE_UTF8_RANGE);
	m_TabCtrl.Show();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}



// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTabDialog::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}


//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTabDialog::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



//////////////////////////////////////////////////////////////////////////
//	最小化到托盘代码

void CTabDialog::ToTray()
{ 
	NOTIFYICONDATA nid;
	nid.cbSize=sizeof(NOTIFYICONDATA);
	nid.hIcon=AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	nid.uID = IDR_MAINFRAME;
	nid.hWnd=m_hWnd;
	lstrcpy(nid.szTip,_T("巨潮网 PDF 分析"));
	nid.uCallbackMessage= WM_NOTIFY_FROM_TRAY;
	nid.uFlags=NIF_ICON | NIF_MESSAGE | NIF_TIP;
	Shell_NotifyIcon(NIM_ADD,&nid);
}


LRESULT CTabDialog::OnNotifyIcon(WPARAM wParam,LPARAM IParam)
{
	if ((IParam == WM_LBUTTONDOWN) || (IParam == WM_RBUTTONDOWN))
	{ 
		//ModifyStyleEx( WS_EX_TOOLWINDOW, WS_EX_APPWINDOW);
		ModifyStyleEx(0,WS_EX_TOPMOST);
		ShowWindow(SW_SHOW);
		this->UpdateWindow();
		//Shell_NotifyIcon(NIM_DELETE, &NotifyIcon);//消除托盘图标
	}
	return 0;
}

void CTabDialog::DeleteTray()
{
	NOTIFYICONDATA nid;
	nid.cbSize=(DWORD)sizeof(NOTIFYICONDATA);
	nid.hWnd=this->m_hWnd;
	nid.uID=IDR_MAINFRAME;
	nid.uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP ;
	nid.uCallbackMessage= WM_NOTIFY_FROM_TRAY;			//自定义的消息名称
	nid.hIcon= AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	strcpy(nid.szTip,"巨潮网 PDF 分析");				//信息提示条为“计划任务提醒”
	Shell_NotifyIcon(NIM_DELETE,&nid);					//在托盘区删除图标
}

void CTabDialog::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码

	DeleteTray();
}

void CTabDialog::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	switch(nID)
	{
	case SC_MINIMIZE:
		ShowWindow(SW_HIDE);
		break;
	case SC_CLOSE:
		{
			int nRes = AfxMessageBox(_T("任务完成后会自动关闭，确定现在退出吗？"), MB_OKCANCEL | MB_ICONQUESTION);
			if(nRes == IDOK)
				CDialog::OnSysCommand(nID, lParam);
		}
		break;
	default:
		CDialog::OnSysCommand(nID, lParam);
		break;
	}


	//CDialog::OnSysCommand(nID, lParam);
}

void CTabDialog::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	ShowWindow(SW_HIDE);

	KillTimer(1);

	CDialog::OnTimer(nIDEvent);
}


//////////////////////////////////////////////////////////////////////////



LRESULT CTabDialog::OnUserLogWParam_1(WPARAM wParam, LPARAM lParam)
{
	// we expect the CMessageString in the WParam paramater
	CMessageString* pString = (CMessageString*)wParam;
	int		logType = (int)lParam;

	if (CMessageString::IsStringValid(pString))
	{
		string strMsg = string(pString->GetBuffer());
		delete pString;
	
		if(strMsg.find("程序结束")!=string::npos)
		{
			//this->EndDialog(IDOK);
		}
		else
		{
			m_TabCtrl.ProcessMessage(strMsg);
		}
	}
	else
	{
		CString strMessage = "ERROR! Test message using wParam received, but CMessageString passed in wParam is invalid.";
		AfxMessageBox(strMessage);

		ASSERT(FALSE);
		return MESSAGESTRING_NOT_RECEIVED;
	}

	return 0;
}
void CTabDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
}
