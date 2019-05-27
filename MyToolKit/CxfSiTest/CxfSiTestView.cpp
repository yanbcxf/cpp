
// CxfSiTestView.cpp : CCxfSiTestView 类的实现
//

#include "stdafx.h"
#include "CxfSiTest.h"

#include "CxfSiTestDoc.h"
#include "CxfSiTestView.h"

#include "rtpsession.h"
#include "rtpsessionparams.h"
#include "rtpudpv4transmitter.h"
#include "rtpipv4address.h"
#include "rtptimeutilities.h"
#include "rtppacket.h"

using namespace jrtplib;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

UINT RtplibTestThreadFunc(LPVOID param)
{
#ifdef WIN32
	/*WSADATA dat;
	WSAStartup(MAKEWORD(2,2),&dat);*/
#endif // WIN32

	RTPSession session;

	RTPSessionParams sessionparams;
	sessionparams.SetOwnTimestampUnit(1.0/8000.0);

	RTPUDPv4TransmissionParams transparams;
	transparams.SetPortbase(8000);

	int status = session.Create(sessionparams,&transparams);
	if (status < 0)
	{
		//std::cerr << RTPGetErrorString(status) << std::endl;
		exit(-1);
	}

	uint8_t localip[]={127,0,0,1};
	RTPIPv4Address addr(localip,9000);

	status = session.AddDestination(addr);
	if (status < 0)
	{
		//std::cerr << RTPGetErrorString(status) << std::endl;
		exit(-1);
	}

	session.SetDefaultPayloadType(96);
	session.SetDefaultMark(false);
	session.SetDefaultTimestampIncrement(160);

	uint8_t silencebuffer[160];
	for (int i = 0 ; i < 160 ; i++)
		silencebuffer[i] = 128;

	RTPTime delay(0.020);
	RTPTime starttime = RTPTime::CurrentTime();

	//getchar();

	FILE * fp_wlz1 = NULL;
	

	bool done = false;
	while (!done)
	{
		status = session.SendPacket(silencebuffer,160);
		if (status < 0)
		{
			//std::cerr << RTPGetErrorString(status) << std::endl;
			exit(-1);
		}

		session.BeginDataAccess();
		if (session.GotoFirstSource())
		{
			do
			{
				RTPPacket *packet;

				while ((packet = session.GetNextPacket()) != 0)
				{
					if(NULL == fp_wlz1)
						fp_wlz1=fopen("C:/ipc133.ps","ab");

					size_t lengh = packet->GetPayloadLength();
					uint8_t * RawData = packet->GetPayloadData();   //得到数据

					fwrite(RawData,1,lengh ,fp_wlz1);

					/*std::cout << "Got packet with " 
						<< "extended sequence number " 
						<< packet->GetExtendedSequenceNumber() 
						<< " from SSRC " << packet->GetSSRC() 
						<< std::endl;*/
					session.DeletePacket(packet);
				}
			} while (session.GotoNextSource());
		}
		session.EndDataAccess();

		RTPTime::Wait(delay);

		RTPTime t = RTPTime::CurrentTime();
		t -= starttime;
		if (t > RTPTime(360.0))
			done = true;
	}

	delay = RTPTime(10.0);
	session.BYEDestroy(delay,"Time's up",9);

#ifdef WIN32
	/*WSACleanup();*/
#endif // WIN32
	return 0;
}

//////////////////////////////////////////////////////////////////////////

// CCxfSiTestView

IMPLEMENT_DYNCREATE(CCxfSiTestView, CView)

BEGIN_MESSAGE_MAP(CCxfSiTestView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CCxfSiTestView::OnFilePrintPreview)
	ON_COMMAND(ID_CXFSI_CATALOG, &CCxfSiTestView::OnCxfsiCatalog)
	ON_COMMAND(ID_CXFSI_JRTPLIB_TEST, &CCxfSiTestView::OnCxfsiJrtplibTest)
END_MESSAGE_MAP()

// CCxfSiTestView 构造/析构

CCxfSiTestView::CCxfSiTestView()
{
	// TODO: 在此处添加构造代码

}

CCxfSiTestView::~CCxfSiTestView()
{
}

BOOL CCxfSiTestView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CCxfSiTestView 绘制

void CCxfSiTestView::OnDraw(CDC* /*pDC*/)
{
	CCxfSiTestDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}


// CCxfSiTestView 打印


void CCxfSiTestView::OnFilePrintPreview()
{
	AFXPrintPreview(this);
}

BOOL CCxfSiTestView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CCxfSiTestView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CCxfSiTestView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void CCxfSiTestView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CCxfSiTestView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}


// CCxfSiTestView 诊断

#ifdef _DEBUG
void CCxfSiTestView::AssertValid() const
{
	CView::AssertValid();
}

void CCxfSiTestView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCxfSiTestDoc* CCxfSiTestView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCxfSiTestDoc)));
	return (CCxfSiTestDoc*)m_pDocument;
}
#endif //_DEBUG


// CCxfSiTestView 消息处理程序

void CCxfSiTestView::OnCxfsiCatalog()
{
	// TODO: 在此添加命令处理程序代码
	sipd_catalog_req catalogReq;
	memset(catalogReq.device_id, 0, MAX_SIPD_DEVICE_ID_LEN );
	memset(catalogReq.SN, 0, MAX_SIPD_DEVICE_ID_LEN );

	memcpy(catalogReq.device_id, "35010400001320000001", 20);
	memcpy(catalogReq.SN, "123456", 6 );
	strcpy(catalogReq.to_uri , "sip:35010400001320000001@10.18.72.133:5062");
	strcpy(catalogReq.from_uri, "sip:35010400002000000005@10.18.72.35:5068");
	Sip_cataloginfo_request(&catalogReq);
}

void CCxfSiTestView::OnCxfsiJrtplibTest()
{
	// TODO: 在此添加命令处理程序代码

	CWinThread * pThread = AfxBeginThread(RtplibTestThreadFunc, NULL, \
		THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	pThread->m_bAutoDelete = FALSE;
	pThread->ResumeThread();
}
