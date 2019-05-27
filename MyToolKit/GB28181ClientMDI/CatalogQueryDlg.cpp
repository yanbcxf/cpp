// CatalogQueryDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GB28181ClientMDI.h"
#include "CatalogQueryDlg.h"


// CCatalogQueryDlg 对话框

IMPLEMENT_DYNAMIC(CCatalogQueryDlg, CDialog)

CCatalogQueryDlg::CCatalogQueryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCatalogQueryDlg::IDD, pParent)
{

}

CCatalogQueryDlg::~CCatalogQueryDlg()
{
}

void CCatalogQueryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCatalogQueryDlg, CDialog)
END_MESSAGE_MAP()


// CCatalogQueryDlg 消息处理程序
