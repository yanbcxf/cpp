#include "StdAfx.h"
#include "CalcNewmat11Test.h"




CCalcNewmat11Test::CCalcNewmat11Test(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalcNewmat11Test::~CCalcNewmat11Test(void)
{
}




void CCalcNewmat11Test::Execute1(void* firstArg)
{
	CCalcNewmat11Test * sink = (CCalcNewmat11Test *)firstArg;
	sink->Execute();
}



void CCalcNewmat11Test::Execute()
{
	stringstream ss;
	

	try{

		Matrix C(3, 3);
		C << 1 << 2 << -1
			<< 3 << 1 << 0
			<< -1 << 0 << -2;

		Matrix B(3, 3);
		B << 1 << 2 << 3
			<< 4 << 5 << 6
			<< 7 << 8 << 9;

		double determinant = B.determinant();
		
		
		ColumnVector x(3);
		x << 1 << 2 << 3;

		ColumnVector b(3);
		b << 1 << 2 << 3;

		SymmetricMatrix S(3);
		S.row(1) << 11;
		S.row(2) << 21 << 22;
		S.row(3) << 31 << 32 << 33;

		DiagonalMatrix D;
		Matrix V;

		
		Jacobi(S,D,V);  
		
		m_result = D;

		ss.str("");
		ss << "CCalcNewmat11Test ¼ÆËãÍê³É";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch(std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}


