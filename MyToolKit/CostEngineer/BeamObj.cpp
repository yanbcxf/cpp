#include "stdafx.h"
#include "BeamObj.h"


CBeamObj::CBeamObj()
{
	height = 0;
	length = 0;
	breadth = 0;
	quantity = 0;
	name = "";
}


CBeamObj::~CBeamObj()
{
}

void CBeamObj::Serialize(CArchive& ar) {
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
