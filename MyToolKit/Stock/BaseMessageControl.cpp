#include "StdAfx.h"
#include "Stock.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "BaseMessageElementsParser.h"
#include "BaseMessageView.h"
#include "BaseMessageFormView.h"
#include "BaseRichEditFormView.h"
#include "BaseMessageControl.h"

CBaseMessageControl::CBaseMessageControl(void)
{
}

CBaseMessageControl::~CBaseMessageControl(void)
{
}




void CBaseMessageControl::FocusStockOnLeftPane(string strCode)
{
	 int is_stock;
	 if(strCode.length()==6)
		 is_stock = 1;
	 else
		 is_stock = 0;

	 int code = atoi(strCode.c_str());
	if(is_stock==1)
	{
		if(((CMainFrame *)AfxGetMainWnd())->m_wndEntrepreneurView.FocusOn(code)==true)
			((CMainFrame *)AfxGetMainWnd())->m_wndEntrepreneurView.ShowPane(TRUE, FALSE, TRUE);

		else if(((CMainFrame *)AfxGetMainWnd())->m_wndSmallMidEnterpriseView.FocusOn(code)==true)
			((CMainFrame *)AfxGetMainWnd())->m_wndSmallMidEnterpriseView.ShowPane(TRUE, FALSE, TRUE);

		else if(((CMainFrame *)AfxGetMainWnd())->m_wndShenzhenAsharesView.FocusOn(code)==true)
			((CMainFrame *)AfxGetMainWnd())->m_wndShenzhenAsharesView.ShowPane(TRUE, FALSE, TRUE);

		else if(((CMainFrame *)AfxGetMainWnd())->m_wndShanghaiStockView.FocusOn(code)==true)
			((CMainFrame *)AfxGetMainWnd())->m_wndShanghaiStockView.ShowPane(TRUE, FALSE, TRUE);
	}
	
	//	在“中证行业” 板块中进行显示
	/*if(((CMainFrame *)AfxGetMainWnd())->m_wndCsindexPlatesView.FocusOn(code, is_stock)==true)
		((CMainFrame *)AfxGetMainWnd())->m_wndCsindexPlatesView.ShowPane(TRUE, FALSE, TRUE);*/

	//	在各个板块中进行查找
	((CMainFrame *)AfxGetMainWnd())->m_wndCsindexPlatesView.SetFindTarget(code, is_stock);
	((CMainFrame *)AfxGetMainWnd())->m_wndCsindexCsrcPlatesView.SetFindTarget(code, is_stock);
	((CMainFrame *)AfxGetMainWnd())->m_wndSelfSelectPlatesView.SetFindTarget(code, is_stock);
	((CMainFrame *)AfxGetMainWnd())->m_wndHangyePlatesView.SetFindTarget(code, is_stock);
	((CMainFrame *)AfxGetMainWnd())->m_wndDiyuPlatesView.SetFindTarget(code, is_stock);
	((CMainFrame *)AfxGetMainWnd())->m_wndGainianPlatesView.SetFindTarget(code, is_stock);
	((CMainFrame *)AfxGetMainWnd())->m_wndShenzhenIndexPlatesView.SetFindTarget(code, is_stock);

}

LRESULT CBaseMessageControl::OnUserXmlWParam(WPARAM wParam, LPARAM lParam)
{
	// we expect the CMessageString in the WParam paramater
	CMessageString* pString = (CMessageString*)wParam;

	// make sure passed string is valid (this is important to prevent unexpected
	// results if an invalid string is passed)
	if (CMessageString::IsStringValid(pString))
	{
		CBaseMessageElementsParser parser(this);
		parser.Parse(pString->GetBuffer(), pString->GetLength());
		delete pString;

		if(m_pOwnerView->IsKindOf(RUNTIME_CLASS(CBaseMessageView)))
			((CBaseMessageView *)m_pOwnerView)->ProcessXmlWParam(parser.msg_type,parser.code,parser.model, parser.param1,parser.param2, parser.param3);

		if(m_pOwnerView->IsKindOf(RUNTIME_CLASS(CBaseMessageFormView)))
			((CBaseMessageFormView *)m_pOwnerView)->ProcessXmlWParam(parser.msg_type,parser.code,parser.model, parser.param1,parser.param2, parser.param3);

		if(m_pOwnerView->IsKindOf(RUNTIME_CLASS(CBaseRichEditFormView)))
			((CBaseRichEditFormView *)m_pOwnerView)->ProcessXmlWParam(parser.msg_type,parser.code,parser.model, parser.param1,parser.param2, parser.param3);
	}
	else
	{
		CString strMessage = "ERROR! Test message using wParam received, but CMessageString passed in wParam is invalid.";
		AfxMessageBox(strMessage);

		ASSERT(FALSE);
		return MESSAGESTRING_NOT_RECEIVED;
	}

	return 0;
}


LRESULT CBaseMessageControl::OnUserLogWParam_1(WPARAM wParam, LPARAM lParam)
{
	// we expect the CMessageString in the WParam paramater
	CMessageString* pString = (CMessageString*)wParam;

	// make sure passed string is valid (this is important to prevent unexpected
	// results if an invalid string is passed)
	if (CMessageString::IsStringValid(pString))
	{
		//	缺省处理，消息转发到主窗口
		CString curString;
		curString = *pString;
		sendToOutput(curString, NULL, WM_USER_LOG_1);
		delete pString;
	}
	else
	{
		CString strMessage = "ERROR! Test message using wParam received, but CMessageString passed in wParam is invalid.";
		AfxMessageBox(strMessage);

		ASSERT(FALSE);
		return MESSAGESTRING_NOT_RECEIVED;
	}

	return 0;
}

//	option  0 = 行业代码，  1 = 股票代码， 2 = 直接子节点  3 = 所有代码 
void CBaseMessageControl::SetStockRangeForCalculateTask(CIndividualShareDoc * pDoc, CCalculateTask * cse,  int option)
{
	if(pDoc->m_nCommandCode != ID_ALL_STOCK_LIST)
	{

		if( pDoc->m_nIs_Stock==1 && option==Search_Stock)
		{
			if(cse)
			{
				cse->m_vec_request_code.push_back(pDoc->m_nCode);
				cse->m_vec_request_is_stock.push_back(1);
			}
			else
			{
				pDoc->m_vec_request_code.push_back(pDoc->m_nCode);
				pDoc->m_vec_request_is_stock.push_back(1);
			}
			
		}
		else
		{
			JMutexAutoLock lock(((CStockApp *)AfxGetApp())->m_pCSelfSelectStockPlates->m_mutex);
			CStockPlateTree * pPlates = NULL;
			if(pDoc->m_nCommandCode==ID_PLATE_TYPE_SOHU)
				pPlates = ((CStockApp *)AfxGetApp())->m_pCSouhuStockPlates;
			if(pDoc->m_nCommandCode==ID_PLATE_TYPE_SELF_SELECT)
				pPlates = ((CStockApp *)AfxGetApp())->m_pCSelfSelectStockPlates->m_pTree;
			if(pDoc->m_nCommandCode==ID_PLATE_TYPE_CSINDEX)
				pPlates = ((CStockApp *)AfxGetApp())->m_pCsindexStockPlates;
			if(pDoc->m_nCommandCode==ID_PLATE_TYPE_CSINDEX_CSRC)
				pPlates = ((CStockApp *)AfxGetApp())->m_pCsindexCsrcStockPlates;
			if(pDoc->m_nCommandCode==ID_PLATE_TYPE_SHENZHEN_INDEXINFO)
				pPlates = ((CStockApp *)AfxGetApp())->m_ShenzhenIndexList.m_pTree;

			if(pPlates)
			{
				CStockPlateData treeNode;
				treeNode.is_stock = pDoc->m_nIs_Stock;
				treeNode.code = pDoc->m_nCode;
				/*if(treeNode.is_stock)
					treeNode.code = Int2String(pDoc->m_nCode, "%06d");
				else
					treeNode.code = Int2String(pDoc->m_nCode, "%08d");*/
				

				CStockPlateTree * pNode = pPlates->SearchSpecialNode(treeNode);
				if(pNode==NULL)
					pNode = pPlates;
				if(pNode)
				{
					if(option==Search_Child)
					{

						CStockPlateData spd = *pNode->get();
						if(cse)
						{
							cse->m_vec_request_code.push_back((spd.code.c_str()));
							cse->m_vec_request_is_stock.push_back(spd.is_stock);
						}
						else
						{
							pDoc->m_vec_request_code.push_back((spd.code.c_str()));
							pDoc->m_vec_request_is_stock.push_back(spd.is_stock);
						}
						
						CStockPlateTree::iterator child_it = pNode->begin();
						while(child_it!=pNode->end())
						{
							CStockPlateData spd = *child_it;
							if(cse)
							{
								cse->m_vec_request_code.push_back((spd.code.c_str()));
								cse->m_vec_request_is_stock.push_back(spd.is_stock);
							}
							else
							{
								pDoc->m_vec_request_code.push_back((spd.code.c_str()));
								pDoc->m_vec_request_is_stock.push_back(spd.is_stock);
							}

							child_it++;
						}
					}
					else
					{
						CStockPlateTree::pre_order_iterator pre_order_it = pNode->pre_order_begin();
						while(pre_order_it!=pNode->pre_order_end())
						{
							CStockPlateData spd = *pre_order_it;
							if(spd.is_stock==Search_Stock && (option ==Search_Stock || option!=Search_Industry ))
							{
								if(cse)
								{
									cse->m_vec_request_code.push_back((spd.code.c_str()));
									cse->m_vec_request_is_stock.push_back(spd.is_stock);
								}
								else
								{
									pDoc->m_vec_request_code.push_back((spd.code.c_str()));
									pDoc->m_vec_request_is_stock.push_back(spd.is_stock);
								}
								
							}
							if(spd.is_stock==Search_Industry && (option ==Search_Industry || option!=Search_Stock ))
							{
								if(cse)
								{
									cse->m_vec_request_code.push_back((spd.code.c_str()));
									cse->m_vec_request_is_stock.push_back(spd.is_stock);
								}
								else
								{
									pDoc->m_vec_request_code.push_back((spd.code.c_str()));
									pDoc->m_vec_request_is_stock.push_back(spd.is_stock);
								}
								
							}
							pre_order_it++;
						}
					}
				}
			}
		}
	}
	else
	{
		/*所有股票的概况列表*/
		CEntrepreneurship Entrepreneurship ;
		CBussinessUtils::GetEntrepreneurship(Entrepreneurship);
		for(int i=0; i< Entrepreneurship.m_nRowNum; i++)
		{
			if(cse)
				cse->m_vec_request_code.push_back(Int2String(Entrepreneurship.m_vec_code[i], "%06d"));
			else
				pDoc->m_vec_request_code.push_back(Int2String(Entrepreneurship.m_vec_code[i], "%06d"));
		}

		CSmallMidEnterprise midsmall ;
		CBussinessUtils::GetSmallMidEnterprise(midsmall);
		for(int i=0; i< midsmall.m_nRowNum; i++)
		{
			if(cse)
				cse->m_vec_request_code.push_back(Int2String(midsmall.m_vec_code[i], "%06d"));
			else
				pDoc->m_vec_request_code.push_back(Int2String(midsmall.m_vec_code[i], "%06d"));
		}

		CShenzhenAshares shenzhenA ;
		CBussinessUtils::GetShenzhenAshares(shenzhenA);
		for(int i=0; i< shenzhenA.m_nRowNum; i++)
		{
			if(cse)
				cse->m_vec_request_code.push_back(Int2String(shenzhenA.m_vec_code[i], "%06d"));
			else
				pDoc->m_vec_request_code.push_back(Int2String(shenzhenA.m_vec_code[i], "%06d"));
		}

		CShanghaiStockInfo shanghai ;
		CBussinessUtils::GetShanghaiStockInfo(shanghai);
		for(int i=0; i< shanghai.m_nRowNum; i++)
		{
			if(cse)
				cse->m_vec_request_code.push_back(Int2String(shanghai.m_vec_code[i], "%06d"));
			else
				pDoc->m_vec_request_code.push_back(Int2String(shanghai.m_vec_code[i], "%06d"));
		}
	}
}


void CBaseMessageControl::OnSetFocus(CString strTitle)
{
	
	/*当获取焦点时，自动在左边的股票各个板块中寻找对应项*/

	vector<string> firstMatch;
	string strPattern = string("\\(\\d+\\)");
	if(Pcre2Grep(strPattern.c_str(), strTitle.GetBuffer(),firstMatch )>0)
	{
		//sendToOutput(strTitle, NULL, WM_USER_LOG_1);

		string::size_type pos1 = firstMatch[0].find(")");
		string codeStr = firstMatch[0].substr(1, pos1-1);

		FocusStockOnLeftPane(codeStr);
	}	
	
}
