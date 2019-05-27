
#include "stdafx.h"
#include "CninfoAnnouncePDF.h"
#include "PDFHaveCatalog.h"
#include "PDFGetLand.h"
#include "PDFStockIncentivePlan.h"
#include "PDFShareHolding.h"

UINT AutoDownloadThreadFunc(LPVOID param)
{

	stringstream ss;

	/*{
		ss.str("");
		ss << "PDF 测试  下载开始.\r\n";
		sendToOutput(ss.str().c_str(), NULL );

		CPDFShareHolding  pdf;
		pdf.announcementId = "1202742046";
		pdf.adjunctUrl = "/finalpage/2015-04-23/1200889625.PDF";
		pdf.announcementTitle = "1202742046";
		pdf.secCode = "000069";
		pdf.m_hWnd = NULL;

		pdf.SaveAsCsv();

		int kkk = 1;
		return 0 ;
	}*/

	/*{
		CPDFShareHolding  pdf;
		pdf.UpdateQuarter();
		return 0;
	}*/


	while(true)
	{
		list<CCninfoAnnouncePDF *> lstPdf;
		//////////////////////////////////////////////////////////////////////////
		//	读入所有需要分析的 PDF 文件目录
		/*{
			CPDFHaveCatalog pdf1;
			list<CCninfoAnnouncePDF *> lst1 = pdf1.CreatePDF();
			list<CCninfoAnnouncePDF *>::iterator it = lst1.begin();
			for(; it!=lst1.end(); it++)
			{
				lstPdf.push_back(*it);
			}
		}*/
		
		{
			CPDFShareHolding pdf1;
			list<CCninfoAnnouncePDF *> lst1 = pdf1.CreatePDF();
			list<CCninfoAnnouncePDF *>::iterator it = lst1.begin();
			for(; it!=lst1.end(); it++)
			{
				lstPdf.push_back(*it);
			}
		}

		{
			CPDFGetLand pdf1;
			list<CCninfoAnnouncePDF *> lst1 = pdf1.CreatePDF();
			list<CCninfoAnnouncePDF *>::iterator it = lst1.begin();
			for(; it!=lst1.end(); it++)
			{
				lstPdf.push_back(*it);
			}
		}
		
		{
			CPDFStockIncentivePlan pdf1;
			list<CCninfoAnnouncePDF *> lst1 = pdf1.CreatePDF();
			list<CCninfoAnnouncePDF *>::iterator it = lst1.begin();
			for(; it!=lst1.end(); it++)
			{
				lstPdf.push_back(*it);
			}
		}
	
		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << " ============================== PDF 文件下载开始.";
		ss << " 共 " << lstPdf.size() << "个文件 \r\n";
		sendToOutput(ss.str().c_str(), NULL );

		if(lstPdf.size()==0)
		{
			//	检测 Stock.exe 程序是否存在，以便决定是否关闭本程序
			if(CheckProcessOnlyOne("Stock.exe")==0)
			{
				// TerminateProcess(GetCurrentProcess(), 1 );
				sendToOutput(_T("程序结束"), NULL);
			}
			else
				Sleep(600 * 1000);
		}
		

		//////////////////////////////////////////////////////////////////////////
		//	开始逐个的 下载分析

		int nPDF = 1;
		while(lstPdf.size()>0)
		{

			if(lstPdf.size()==1)
			{
				int kkk = 1;
			}
			CCninfoAnnouncePDF *  pdf = *lstPdf.begin();

			ss.str("");
			ss << "【" << nPDF << "】PDF" << "(" <<  pdf->announcementTitle << ")下载开始.\r\n";
			sendToOutput(ss.str().c_str(), NULL );
			
			pdf->m_hWnd = NULL;

			string csv = pdf->SaveAsCsv();
			pdf->ImportToDatabase(csv);

			delete pdf;

			lstPdf.pop_front();

			nPDF++;
		}

		Sleep(10 * 1000);
	}
	
	return 0;
}