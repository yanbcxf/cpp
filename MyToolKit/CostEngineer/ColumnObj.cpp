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