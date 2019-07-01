// Node.h: interface for the CNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NODE_H__8243C025_02CF_4AD7_B2AC_A5BE86912FA4__INCLUDED_)
#define AFX_NODE_H__8243C025_02CF_4AD7_B2AC_A5BE86912FA4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <algorithm>
#include <cmath>
using namespace std;

typedef vector <long> VTYPE_NODE_P;

class CNode
{
public:
	CNode Copy();
	
	POINT m_p;		// graphical point for that node
	CNode();

	string  m_top;
	string  m_middle;
	string	m_bottom;
	string  m_tips;

	virtual ~CNode();
};

typedef vector <CNode> VTYPE_NODE;

#endif // !defined(AFX_NODE_H__8243C025_02CF_4AD7_B2AC_A5BE86912FA4__INCLUDED_)
