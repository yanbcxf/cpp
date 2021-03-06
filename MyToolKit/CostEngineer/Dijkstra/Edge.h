// Edge.h: interface for the CEdge class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EDGE_H__7DA4D413_150C_4C94_AAE6_A5C1C4BA80AC__INCLUDED_)
#define AFX_EDGE_H__7DA4D413_150C_4C94_AAE6_A5C1C4BA80AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CEdge  
{
public:
	bool m_red;			// used to draw the result (if an edge it is a part of the shortest path it is drawn in red)
	bool m_solid;		// 实线 或者 虚线
	long m_secondNode;	// the second node number
	long m_firstNode;	// the first node number
	POINT m_secondPct;	// graphical elements for drawing the edges
	POINT m_firstPct;
	POINT m_arrow1;
	POINT m_arrow2;

	string m_top;
	string m_bottom;
	string m_tips;

	CEdge();
	virtual ~CEdge();

};

typedef vector <CEdge> VTYPE_EDGE;

// the graph is oriented from the first node to the second node

#endif // !defined(AFX_EDGE_H__7DA4D413_150C_4C94_AAE6_A5C1C4BA80AC__INCLUDED_)
