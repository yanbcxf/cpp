#pragma once
class CColumnObj
{
public:
	CColumnObj();

	~CColumnObj();

	void Serialize(CArchive& ar);

	void Create();

	CString name;
	double	height;
	double	length;
	double	breadth;
	int		quantity;
};

