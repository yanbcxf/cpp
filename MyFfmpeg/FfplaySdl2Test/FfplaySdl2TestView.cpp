
// FfplaySdl2TestView.cpp : implementation of the CFfplaySdl2TestView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "FfplaySdl2Test.h"
#endif

#include "FfplaySdl2TestDoc.h"
#include "FfplaySdl2TestView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFfplaySdl2TestView

IMPLEMENT_DYNCREATE(CFfplaySdl2TestView, CView)

BEGIN_MESSAGE_MAP(CFfplaySdl2TestView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CFfplaySdl2TestView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_STREAM_OPEN, &CFfplaySdl2TestView::OnStreamOpen)
END_MESSAGE_MAP()

// CFfplaySdl2TestView construction/destruction

CFfplaySdl2TestView::CFfplaySdl2TestView()
{
	// TODO: add construction code here

}

CFfplaySdl2TestView::~CFfplaySdl2TestView()
{
}

BOOL CFfplaySdl2TestView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CFfplaySdl2TestView drawing

void CFfplaySdl2TestView::OnDraw(CDC* /*pDC*/)
{
	CFfplaySdl2TestDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CFfplaySdl2TestView printing


void CFfplaySdl2TestView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CFfplaySdl2TestView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CFfplaySdl2TestView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CFfplaySdl2TestView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CFfplaySdl2TestView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CFfplaySdl2TestView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CFfplaySdl2TestView diagnostics

#ifdef _DEBUG
void CFfplaySdl2TestView::AssertValid() const
{
	CView::AssertValid();
}

void CFfplaySdl2TestView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CFfplaySdl2TestDoc* CFfplaySdl2TestView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CFfplaySdl2TestDoc)));
	return (CFfplaySdl2TestDoc*)m_pDocument;
}
#endif //_DEBUG


// CFfplaySdl2TestView message handlers


void CFfplaySdl2TestView::OnStreamOpen()
{
	// TODO: Add your command handler code here
	//player_stream_open("bigbuckbunny_480x272.h265", NULL);
	stream_media_format media_format;
	media_format.media_type = SIPD_MEDIA_TYPE_VIDEO;
	media_format.video_type = SIPD_MEDIA_F_VIDEO_TYPE_H264;
	player_stream_open(&media_format, NULL);

}
