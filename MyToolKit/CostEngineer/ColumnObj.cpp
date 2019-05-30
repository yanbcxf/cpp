#include "stdafx.h"
#include "ColumnObj.h"


CColumnObj::CColumnObj()
{
	height = 0;
	length = 0;
	breadth = 0;
	quantity = 0;
	name = "";
}


CColumnObj::~CColumnObj()
{
}

void CColumnObj::Serialize(CArchive& ar) {
	if (ar.IsStoring()) {
		ar << breadth;
		ar << height;
		ar << length;
		ar << name;
		ar << quantity;
	}
	else {
		ar >> breadth;
		ar >> height;
		ar >> length;
		ar >> name;
		ar >> quantity;
	}
}

void CColumnObj::Create() {
	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 50;
	infd.GROUP_NUM_PER_LINE = 3;
	int i = 0;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("名称"), 64);

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("高"), 64);
	// infd.m_vecFindItem[0][i][0].strItem.Format("%d", pDoc->m_pai_start);
	infd.m_vecFindItem[0][i][0].dbMin = 0;
	infd.m_vecFindItem[0][i][0].dbMax = 100000000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("长"), 64);
	// infd.m_vecFindItem[0][i][0].strItem.Format("%d", pDoc->m_pai_start);
	infd.m_vecFindItem[0][i][0].dbMin = 0;
	infd.m_vecFindItem[0][i][0].dbMax = 100000000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("宽"), 64);
	// infd.m_vecFindItem[0][i][0].strItem.Format("%d", pDoc->m_pai_start);
	infd.m_vecFindItem[0][i][0].dbMin = 0;
	infd.m_vecFindItem[0][i][0].dbMax = 100000000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("数量"), 64);
	// infd.m_vecFindItem[0][i][0].strItem.Format("%d", pDoc->m_pai_start);
	infd.m_vecFindItem[0][i][0].nMin = 0;
	infd.m_vecFindItem[0][i][0].nMax = 100000000;

	infd.Init(_T("矩形柱 参数设置"), _T("矩形柱 参数设置"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		name = infd.m_vecFindItem[0][i++][0].strItem;
		height = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		length = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		breadth = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		quantity = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
	}
}