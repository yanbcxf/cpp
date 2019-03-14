// FfplaySdl2FormView.cpp : implementation file
//

#include "stdafx.h"
#include "FfplaySdl2Test.h"
#include "FfplaySdl2FormView.h"


// CFfplaySdl2FormView

IMPLEMENT_DYNCREATE(CFfplaySdl2FormView, CFormView)

CFfplaySdl2FormView::CFfplaySdl2FormView()
	: CFormView(CFfplaySdl2FormView::IDD)
{

}

CFfplaySdl2FormView::~CFfplaySdl2FormView()
{
}

void CFfplaySdl2FormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);

	//DDX_Control(pDX, IDC_STATIC, m_black);
}

BEGIN_MESSAGE_MAP(CFfplaySdl2FormView, CFormView)
	ON_COMMAND(ID_STREAM_OPEN, &CFfplaySdl2FormView::OnStreamOpen)
	ON_COMMAND(ID_PLAYERTEST_CHANGESIZE, &CFfplaySdl2FormView::OnPlayertestChangesize)
END_MESSAGE_MAP()


// CFfplaySdl2FormView diagnostics

#ifdef _DEBUG
void CFfplaySdl2FormView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFfplaySdl2FormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFfplaySdl2FormView message handlers


void CFfplaySdl2FormView::OnStreamOpen()
{
	// TODO: Add your command handler code here
	int len = ::GetWindowTextLength(m_hWnd);

	stream_media_format media_format;
	media_format.media_type = SIPD_MEDIA_TYPE_VIDEO;
	media_format.video_type = SIPD_MEDIA_F_VIDEO_TYPE_H264;
	int sessionid = player_stream_open(&media_format, m_hWnd);

	player_stream_loop_input_data(sessionid);

	//player_stream_open("bigbuckbunny_480x272.h264" /*"c://test.sdp"*/ /*"rtsp://10.18.73.242:554/ch1/0"*/
	//	/*"ipc133.ps"*/ /*"速度与激情4BD中英双字1280高清.rmvb"*/ /*"bigbuckbunny_480x272.h265"*/,
	//	m_hWnd/*GetDlgItem(IDC_STATIC)->GetSafeHwnd()*/);


}


void CFfplaySdl2FormView::OnPlayertestChangesize()
{
	// TODO: Add your command handler code here

	//m_black.MoveWindow(10, 10, 600, 400, TRUE); 
	::MoveWindow(GetDlgItem(IDC_STATIC)->GetSafeHwnd(), 10, 10, 600, 400, TRUE);
}
