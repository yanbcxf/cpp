// Graph.h: interface for the CGraph class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRAPH_H__0A20F055_21B0_4D48_A1F0_E916CFF57053__INCLUDED_)
#define AFX_GRAPH_H__0A20F055_21B0_4D48_A1F0_E916CFF57053__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Node.h"
#include "Edge.h"

class CGraph  
{
public:
	long GetNrNodes();
	CGraph();
	virtual ~CGraph();
	VTYPE_NODE m_nodes;	// array of nodes
	VTYPE_EDGE m_edges;	// array of edges
	VTYPE_NODE_P d;		//	array of longs that contain the shortest path at every step
	VTYPE_NODE_P pi;	//  array of longs that contain the predecessor of each node for the shortest path
};

#endif // !defined(AFX_GRAPH_H__0A20F055_21B0_4D48_A1F0_E916CFF57053__INCLUDED_)
