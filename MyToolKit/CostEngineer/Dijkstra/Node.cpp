// Node.cpp: implementation of the CNode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Node.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNode::CNode()
{
	
}

CNode::~CNode()
{
}

CNode CNode::Copy()
{
	CNode ret;
	
	ret.m_p.x = m_p.x;
	ret.m_p.y = m_p.y;
	
	ret.m_top = m_top;
	ret.m_middle = m_middle;
	ret.m_bottom = m_bottom;
	return ret;
}
