
// HiredisTestDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "HiredisTest.h"
#include "HiredisTestDlg.h"
#include "afxdialogex.h"

#include "..\redis-2.4.6_win32\deps\hiredis\hiredis.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


extern string acquire_lock(redisContext*  conn, const char * lockname, float acquire_timeout);
extern string acquire_lock_with_expire(redisContext*  conn, const char * lockname, float acquire_timeout, int lock_expire);
extern bool release_lock(redisContext*  conn, const char * lockname, const char * identifier);

extern string acquire_semaphore(redisContext*  conn, const char * semname, int limit, int timeout);
extern bool release_semaphore(redisContext*  conn, const char * semname, const char * identifier);


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


// CHiredisTestDlg 对话框



CHiredisTestDlg::CHiredisTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_HIREDISTEST_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bListening = false;
	m_bWatching = false;
}

void CHiredisTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CHiredisTestDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_STRING_TEST, &CHiredisTestDlg::OnBnClickedBtnStringTest)
	ON_BN_CLICKED(IDC_BTN_LIST_TEST, &CHiredisTestDlg::OnBnClickedBtnListTest)
	ON_BN_CLICKED(IDC_BTN_SUBSCRIBE_TEST, &CHiredisTestDlg::OnBnClickedBtnSubscribeTest)
	ON_BN_CLICKED(IDC_BTN_PUBLISH_TEST, &CHiredisTestDlg::OnBnClickedBtnPublishTest)
	ON_BN_CLICKED(IDC_BTN_TRANSACTION_TEST, &CHiredisTestDlg::OnBnClickedBtnTransactionTest)
	ON_BN_CLICKED(IDC_BTN_WATCH_TEST, &CHiredisTestDlg::OnBnClickedBtnWatchTest)
	ON_BN_CLICKED(IDC_BTN_LOCK_TEST, &CHiredisTestDlg::OnBnClickedBtnLockTest)
	ON_BN_CLICKED(IDC_BTN_SEMAPHORE_TEST, &CHiredisTestDlg::OnBnClickedBtnSemaphoreTest)
END_MESSAGE_MAP()


// CHiredisTestDlg 消息处理程序

BOOL CHiredisTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CHiredisTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CHiredisTestDlg::OnPaint()
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
HCURSOR CHiredisTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CHiredisTestDlg::OnBnClickedBtnStringTest()
{
	// TODO: 在此添加控件通知处理程序代码

	redisContext*  redis = redisConnect("10.18.73.237", 16379);

	if (NULL == redis || redis->err)
	{       // redis为NULL与redis->err是两种不同的错误，若redis->err为true，可使用redis->errstr查看错误信息  
		redisFree(redis);
		AfxMessageBox("Connect to redisServer faile");
		return;
	}

	printf("Connect to redisServer Success\n");

	redisReply* reply = NULL;
	do {
		reply = (redisReply*)redisCommand(redis, "AUTH Starnet@234");
		if (reply == NULL || !(reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "OK") == 0))
		{
			AfxMessageBox("Execut AUTH failure\n");
			break;
		}
		freeReplyObject(reply);

		const char* command1 = "set stest1 1235";
		reply = (redisReply*)redisCommand(redis, command1);    // 执行命令，结果强转成redisReply*类型  
		if (reply == NULL || !(reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "OK") == 0))
		{
			AfxMessageBox("Execut command1 failure\n");
			break;
		}
		freeReplyObject(reply);
				
		reply = (redisReply*)redisCommand(redis, "setbit stest1 6 1");
		if (reply == NULL || reply->type != REDIS_REPLY_INTEGER)
		{
			AfxMessageBox("Execut incr failure\n");
			break;
		}
		freeReplyObject(reply);

		reply = (redisReply*)redisCommand(redis, "get stest1");
		if (reply == NULL || reply->type != REDIS_REPLY_STRING)
		{
			AfxMessageBox("Execut get failure\n");
			break;
		}


		CString str;
		str.Format("Succeed to execute command[%s]\n", command1);
		AfxMessageBox(str);

	} while (0);

	if(reply)
		freeReplyObject(reply);
	if(redis)
		redisFree(redis);

}


void CHiredisTestDlg::OnBnClickedBtnListTest()
{
	// TODO: 在此添加控件通知处理程序代码

	redisContext*  redis = redisConnect("10.18.73.237", 16379);

	if (NULL == redis || redis->err)
	{       // redis为NULL与redis->err是两种不同的错误，若redis->err为true，可使用redis->errstr查看错误信息  
		redisFree(redis);
		AfxMessageBox("Connect to redisServer faile");
		return;
	}

	printf("Connect to redisServer Success\n");

	redisReply* reply = NULL;
	do {
		reply = (redisReply*)redisCommand(redis, "AUTH Starnet@234");
		if (reply == NULL || !(reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "OK") == 0))
		{
			AfxMessageBox("Execut AUTH failure\n");
			break;
		}
		freeReplyObject(reply);

		reply = (redisReply*)redisCommand(redis, "info");
		if (reply == NULL || reply->type != REDIS_REPLY_INTEGER)
		{
			AfxMessageBox("Execut INFO failure\n");
			break;
		}
		freeReplyObject(reply);

		reply = (redisReply*)redisCommand(redis, "lpush  list item1");     
		if (reply == NULL || reply->type != REDIS_REPLY_INTEGER)
		{
			AfxMessageBox("Execut lpush failure\n");
			break;
		}
		freeReplyObject(reply);

		reply = (redisReply*)redisCommand(redis, "lpush  list item2");
		if (reply == NULL || reply->type != REDIS_REPLY_INTEGER)
		{
			AfxMessageBox("Execut lpush failure\n");
			break;
		}
		freeReplyObject(reply);

		reply = (redisReply*)redisCommand(redis, "lpush  list item2");
		if (reply == NULL || reply->type != REDIS_REPLY_INTEGER)
		{
			AfxMessageBox("Execut lpush failure\n");
			break;
		}
		freeReplyObject(reply);

		reply = (redisReply*)redisCommand(redis, "del  list");
		if (reply == NULL || reply->type != REDIS_REPLY_INTEGER)
		{
			AfxMessageBox("Execut ltrim failure\n");
			break;
		}
		freeReplyObject(reply);

		reply = (redisReply*)redisCommand(redis, "lrange list 0 -1");
		if (reply == NULL || reply->type != REDIS_REPLY_ARRAY)
		{
			AfxMessageBox("Execut lrange failure\n");
			break;
		}
		for (int j = 0; j < reply->elements; j++) {
			char buf[64] = { 0 };
			sprintf_s(buf, 64, "(%u) %s\n", j, reply->element[j]->str);
			AfxMessageBox(buf);
		}
		freeReplyObject(reply);

		reply = (redisReply*)redisCommand(redis, "get stest1");
		if (reply == NULL || reply->type != REDIS_REPLY_STRING)
		{
			AfxMessageBox("Execut get failure\n");
			break;
		}


		CString str;
		str.Format("Succeed to execute command\n");
		AfxMessageBox(str);

	} while (0);

	if (reply)
		freeReplyObject(reply);
	if (redis)
		redisFree(redis);
}

//////////////////////////////////////////////////////////////////////
//	订阅 与 发布 测试

UINT ChannelListenThreadFunc(LPVOID param)
{
	CHiredisTestDlg * pThis = (CHiredisTestDlg *)param;
	if (pThis->m_bListening)
	{ 
		AfxMessageBox("已经处于订阅监听中......");
		return 0;
	}

	pThis->m_bListening = true;
	redisContext*  redis = redisConnect("10.18.73.237", 16379);

	if (NULL == redis || redis->err)
	{       
		// redis为NULL与redis->err是两种不同的错误，若redis->err为true，可使用redis->errstr查看错误信息  
		redisFree(redis);
		AfxMessageBox("Connect to redisServer faile");
		return 0;
	}

	redisReply* reply = NULL;
	do {
		reply = (redisReply*)redisCommand(redis, "AUTH Starnet@234");
		if (reply == NULL || !(reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "OK") == 0))
		{
			AfxMessageBox("Execut AUTH failure\n");
			break;
		}
		freeReplyObject(reply);

		reply = (redisReply*)redisCommand(redis, "SUBSCRIBE channel_1");
		/*if (reply == NULL || !(reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "OK") == 0))
		{
			AfxMessageBox("Execut SUBSCRIBE failure\n");
			break;
		}*/
		freeReplyObject(reply);

		while (redisGetReply(redis, (void **)&reply) == REDIS_OK) {
			// consume message

			if (reply && reply->type == REDIS_REPLY_ARRAY)
			{
				CString str;
				for (int j = 0; j < reply->elements; j++) {
					char buf[64] = { 0 };
					sprintf_s(buf, 64, "(%u)%s", j, reply->element[j]->str);
					str.Format("%s %s", str.GetBuffer(), buf);
				}
				AfxMessageBox(str);
			}
			freeReplyObject(reply);
		}
	} while (0);

	if (reply)
		freeReplyObject(reply);
	if (redis)
		redisFree(redis);

	pThis->m_bListening = false;
	return 0;
}

void CHiredisTestDlg::OnBnClickedBtnSubscribeTest()
{
	// TODO: 在此添加控件通知处理程序代码

	AfxBeginThread(ChannelListenThreadFunc, this);
}


void CHiredisTestDlg::OnBnClickedBtnPublishTest()
{
	// TODO: 向 频道 channel_1 发送信息

	redisContext*  redis = redisConnect("10.18.73.237", 16379);

	if (NULL == redis || redis->err)
	{       // redis为NULL与redis->err是两种不同的错误，若redis->err为true，可使用redis->errstr查看错误信息  
		redisFree(redis);
		AfxMessageBox("Connect to redisServer faile");
		return;
	}

	printf("Connect to redisServer Success\n");

	redisReply* reply = NULL;
	do {
		reply = (redisReply*)redisCommand(redis, "AUTH Starnet@234");
		if (reply == NULL || !(reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "OK") == 0))
		{
			AfxMessageBox("Execut AUTH failure\n");
			break;
		}
		freeReplyObject(reply);

		reply = (redisReply*)redisCommand(redis, "publish channel_1 111111");
		if (reply == NULL || reply->type != REDIS_REPLY_INTEGER)
		{
			AfxMessageBox("Execut publish failure\n");
			break;
		}

	} while (0);

	if (reply)
		freeReplyObject(reply);
	if (redis)
		redisFree(redis);
}

////////////////////////////////////////////////////////////////////////
//	事务操作测试

void CHiredisTestDlg::OnBnClickedBtnTransactionTest()
{
	// TODO: 实现多命令的事务提交

	redisContext*  redis = redisConnect("10.18.73.237", 16379);

	if (NULL == redis || redis->err)
	{       // redis为NULL与redis->err是两种不同的错误，若redis->err为true，可使用redis->errstr查看错误信息  
		redisFree(redis);
		AfxMessageBox("Connect to redisServer faile");
		return;
	}

	printf("Connect to redisServer Success\n");

	redisReply* reply = NULL;
	do {
		reply = (redisReply*)redisCommand(redis, "AUTH Starnet@234");
		if (reply == NULL || !(reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "OK") == 0))
		{
			AfxMessageBox("Execut AUTH failure\n");
			break;
		}
		freeReplyObject(reply);

		reply = (redisReply*)redisCommand(redis, "multi");
		if (reply == NULL || !(reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "OK") == 0))
		{
			AfxMessageBox("Execut multi failure\n");
			break;
		}
		freeReplyObject(reply);


		reply = (redisReply*)redisCommand(redis, "set test1 123");
		if (reply == NULL || !(reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "QUEUED") == 0))
		{
			AfxMessageBox("Execut QUEUED failure\n");
			break;
		}
		freeReplyObject(reply);

		reply = (redisReply*)redisCommand(redis, "set test2 1234");
		if (reply == NULL || !(reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "QUEUED") == 0))
		{
			AfxMessageBox("Execut QUEUED failure\n");
			break;
		}
		freeReplyObject(reply);

		reply = (redisReply*)redisCommand(redis, "set test3 12345");
		if (reply == NULL || !(reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "QUEUED") == 0))
		{
			AfxMessageBox("Execut QUEUED failure\n");
			break;
		}
		freeReplyObject(reply);

		reply = (redisReply*)redisCommand(redis, "exec");
		if (reply == NULL || reply->type != REDIS_REPLY_ARRAY)
		{
			AfxMessageBox("Execut exec failure\n");
			break;
		}

		// 返回上述 三条命令的执行结果
		CString str;
		for (int j = 0; j < reply->elements; j++) {
			char buf[64] = { 0 };
			sprintf_s(buf, 64, "(%u)%s", j, reply->element[j]->str);
			str.Format("%s %s", str.GetBuffer(), buf);
		}
		AfxMessageBox(str);

	} while (0);

	if (reply)
		freeReplyObject(reply);
	if (redis)
		redisFree(redis);

}


////////////////////////////////////////////////////////////////////////////////
// Watch 监视线程


UINT WatchThreadFunc(LPVOID param)
{
	CHiredisTestDlg * pThis = (CHiredisTestDlg *)param;
	if (pThis->m_bWatching)
	{
		AfxMessageBox("已经处于 Watch 监听中......");
		return 0;
	}

	pThis->m_bWatching = true;
	redisContext*  redis = redisConnect("10.18.73.237", 16379);

	if (NULL == redis || redis->err)
	{
		// redis为NULL与redis->err是两种不同的错误，若redis->err为true，可使用redis->errstr查看错误信息  
		redisFree(redis);
		AfxMessageBox("Connect to redisServer faile");
		return 0;
	}

	redisReply* reply = NULL;
	do {
		reply = (redisReply*)redisCommand(redis, "AUTH Starnet@234");
		if (reply == NULL || !(reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "OK") == 0))
		{
			AfxMessageBox("Execut AUTH failure\n");
			break;
		}
		freeReplyObject(reply);

		

		while (1) {
			// consume message
			reply = (redisReply*)redisCommand(redis, "watch stest1");
			if (reply == NULL || !(reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "OK") == 0))
			{
				AfxMessageBox("Execut watch failure\n");
				break;
			}
			freeReplyObject(reply);

			/* WATCH命令可以监控一个或多个键，一旦其中有一个键被修改（或删除），之后的事务就不会执行。
			监控一直持续到EXEC命令（事务中的命令是在EXEC之后才执行的，所以在MULTI命令后可以修改WATCH监控的键值） */

			Sleep(100);

			reply = (redisReply*)redisCommand(redis, "multi");
			if (reply == NULL || !(reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "OK") == 0))
			{
				AfxMessageBox("Execut multi failure\n");
				break;
			}
			freeReplyObject(reply);


			reply = (redisReply*)redisCommand(redis, "set stest1 123");
			if (reply == NULL || !(reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "QUEUED") == 0))
			{
				AfxMessageBox("Execut QUEUED failure\n");
				break;
			}
			freeReplyObject(reply);

			reply = (redisReply*)redisCommand(redis, "set stest2 123");
			if (reply == NULL || !(reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "QUEUED") == 0))
			{
				AfxMessageBox("Execut QUEUED failure\n");
				break;
			}
			freeReplyObject(reply);

			reply = (redisReply*)redisCommand(redis, "exec");
			if (reply == NULL || reply->type != REDIS_REPLY_ARRAY)
			{
				AfxMessageBox("Execut exec failure\n");
				break;
			}

			CString str;
			for (int j = 0; j < reply->elements; j++) {
				char buf[64] = { 0 };
				sprintf_s(buf, 64, "(%u)%s", j, reply->element[j]->str);
				str.Format("%s %s", str.GetBuffer(), buf);
			}

			if (str.FindOneOf("OK") < 0)
			{
				AfxMessageBox(str);
			}

			freeReplyObject(reply);
			reply = NULL;
			
		}
	} while (0);

	if (reply)
		freeReplyObject(reply);
	if (redis)
		redisFree(redis);

	pThis->m_bWatching = false;
	return 0;
}


void CHiredisTestDlg::OnBnClickedBtnWatchTest()
{
	// TODO: 在此添加控件通知处理程序代码

	AfxBeginThread(WatchThreadFunc, this);
}


///////////////////////////////////////////////////////////////////////////////
//	进行多线程的加锁测试



UINT LockThreadFunc(LPVOID param)
{
	bool * bThread = (bool *)param;
	redisContext*  redis = redisConnect("10.18.73.237", 16379);

	if (NULL == redis || redis->err)
	{
		// redis为NULL与redis->err是两种不同的错误，若redis->err为true，可使用redis->errstr查看错误信息  
		redisFree(redis);
		AfxMessageBox("Connect to redisServer faile");
		*bThread = true;
		return 0;
	}

	redisReply* reply = NULL;
	do {
		reply = (redisReply*)redisCommand(redis, "AUTH Starnet@234");
		if (reply == NULL || !(reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "OK") == 0))
		{
			AfxMessageBox("Execut AUTH failure\n");
			break;
		}
		freeReplyObject(reply);
		
		for (int j = 0; j < 1000; j++) {
			// consume message
			
			string identifier = acquire_lock_with_expire(redis, "yb_counter", 1, 5);
			if (identifier != "false")
			{
				int yb_counter = -1;
				reply = (redisReply*)redisCommand(redis, "get yb_counter");
				if (reply)
				{
					if (reply->type == REDIS_REPLY_STRING)
					{
						yb_counter = atoi(reply->str);
						yb_counter++;

					}
					freeReplyObject(reply);
				}

				if (yb_counter >= 0)
				{
					reply = (redisReply*)redisCommand(redis, "set yb_counter %d", yb_counter);
					freeReplyObject(reply);
				}

				release_lock(redis, "yb_counter", identifier.c_str());
			}
			else
			{ 
				j--;
			}
			
		}
	} while (0);

	
	if (redis)
		redisFree(redis);

	*bThread = true;

	return 0;
}


void CHiredisTestDlg::OnBnClickedBtnLockTest()
{
	// TODO: 在此添加控件通知处理程序代码

	redisContext*  redis = redisConnect("10.18.73.237", 16379);

	if (NULL == redis || redis->err)
	{
		// redis为NULL与redis->err是两种不同的错误，若redis->err为true，可使用redis->errstr查看错误信息  
		redisFree(redis);
		AfxMessageBox("Connect to redisServer faile");
		return;
	}

	redisReply * reply = (redisReply*)redisCommand(redis, "AUTH Starnet@234");
	if (reply == NULL || !(reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "OK") == 0))
	{
		AfxMessageBox("Execut AUTH failure\n");
		return;
	}
	freeReplyObject(reply);


	reply = (redisReply*)redisCommand(redis, "set yb_counter 0");
	freeReplyObject(reply);
	reply = (redisReply*)redisCommand(redis, "del lock:yb_counter");
	freeReplyObject(reply);
	
	m_bThread_1 = false;
	m_bThread_2 = false;
	m_bThread_3 = false;
	m_bThread_4 = false;
	m_bThread_5 = false;

	m_bThread_6 = false;
	m_bThread_7 = false;
	m_bThread_8 = false;
	m_bThread_9 = false;
	m_bThread_10 = false;

	AfxBeginThread(LockThreadFunc, &m_bThread_1);
	AfxBeginThread(LockThreadFunc, &m_bThread_2);
	AfxBeginThread(LockThreadFunc, &m_bThread_3);
	AfxBeginThread(LockThreadFunc, &m_bThread_4);
	AfxBeginThread(LockThreadFunc, &m_bThread_5);

	AfxBeginThread(LockThreadFunc, &m_bThread_6);
	AfxBeginThread(LockThreadFunc, &m_bThread_7);
	AfxBeginThread(LockThreadFunc, &m_bThread_8);
	AfxBeginThread(LockThreadFunc, &m_bThread_9);
	AfxBeginThread(LockThreadFunc, &m_bThread_10);


	Sleep(33 * 1000);

	reply = (redisReply*)redisCommand(redis, "get yb_counter");
	freeReplyObject(reply);

	if (redis)
		redisFree(redis);
}

///////////////////////////////////////////////////////////////
//	信号量测试


void CHiredisTestDlg::OnBnClickedBtnSemaphoreTest()
{
	// TODO: 在此添加控件通知处理程序代码

	redisContext*  redis = redisConnect("10.18.73.237", 16379);

	if (NULL == redis || redis->err)
	{
		// redis为NULL与redis->err是两种不同的错误，若redis->err为true，可使用redis->errstr查看错误信息  
		redisFree(redis);
		AfxMessageBox("Connect to redisServer faile");
		return;
	}

	redisReply * reply = (redisReply*)redisCommand(redis, "AUTH Starnet@234");
	if (reply == NULL || !(reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "OK") == 0))
	{
		AfxMessageBox("Execut AUTH failure\n");
		return;
	}
	freeReplyObject(reply);

	string identifier1 = acquire_semaphore(redis, "semaphore_1", 3, 60);
	string identifier2 = acquire_semaphore(redis, "semaphore_1", 3, 60);
	string identifier3 = acquire_semaphore(redis, "semaphore_1", 3, 60);
	string identifier4 = acquire_semaphore(redis, "semaphore_1", 3, 60);
	string identifier5 = acquire_semaphore(redis, "semaphore_1", 3, 60);

	bool b1 = release_semaphore(redis, "semaphore_1", identifier1.c_str());
	bool b2 = release_semaphore(redis, "semaphore_1", identifier2.c_str());
	string identifier6 = acquire_semaphore(redis, "semaphore_1", 3, 60);
	string identifier7 = acquire_semaphore(redis, "semaphore_1", 3, 60);
	string identifier8 = acquire_semaphore(redis, "semaphore_1", 3, 60);

	if (redis)
		redisFree(redis);
}
