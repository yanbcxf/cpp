#pragma once
#include "capitalstructure.h"

class CCapitalStructOf10jqka :
	public CCapitalStructure
{
public:
	CCapitalStructOf10jqka(void);
	~CCapitalStructOf10jqka(void);

	string ModelType(){ return "CCapitalStructOf10jqka" /*"股本结构"*/; }

	string SaveAsCsv();
	string SaveAsCsv_Old();
	string GetDownloadUrl();

	void SaveTable(tree<HTML::Node> &dom, tree<HTML::Node>::sibling_iterator si_it,
		tree<HTML::Node>::sibling_iterator si_end, int & nTotalRow);
};
