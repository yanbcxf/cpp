#pragma once
class CBeamObj
{
public:
	CBeamObj();

	~CBeamObj();

	void Serialize(CArchive& ar);

	CString name;
	double	height;
	double	length;
	double	breadth;
	int		quantity;
};

