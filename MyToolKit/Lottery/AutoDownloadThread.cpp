
#include "stdafx.h"
#include "Lottery.h"
#include "LottoMxn.h"

UINT AutoDownloadThreadFunc(LPVOID param)
{
	CDownloadRequest * pStock  = (CDownloadRequest *)param; 
	pStock->m_strResult = "NoMoreData";

	//	生成要下载的报表对象
	vector<CLotteryDataModel *> vecStockModel;

	vector<LotteryInfo>	vecInfo = ((CLotteryApp *)AfxGetApp())->m_vec_lottery_info;

	//AcquireReadLock(&g_RWlock);
	{
		//	彩票票基础资料
		for(int i=0; i< vecInfo.size(); i++)
		{
			CLottoMxn * mxn = new CLottoMxn;
			mxn->m_lottery_name = vecInfo[i].name;
			mxn->m_lottery_code = vecInfo[i].code;
			mxn->m_regular_num = vecInfo[i].regularnum;
			mxn->m_special_num = vecInfo[i].specialnum;

			mxn->m_hWnd = pStock->m_hwnd;
			mxn->m_nLogType = pStock->m_nLogType;
			vecStockModel.push_back(mxn);
		}
		
	}
	//ReleaseReadLock(&g_RWlock);


	CString strOutput;  
	int	nTry = 0;
	while( vecStockModel.size()>0 && ((CLotteryApp *)AfxGetApp())->TestRequestExit()==FALSE)
	{
		// 通过发送信息到 AutoDownloadView， 可以判断是否视图已经被关闭，关闭则退出线程
		int nRemains = vecStockModel.size();
		CString strBegin;
		strBegin.Format("下载任务 %s, 余下 %d 个 \r\n", 
			vecStockModel[0]->ModelType().c_str(), nRemains);
		strOutput = strBegin;


		/*if(nRemains%10 == 1 && sendToOutput(strOutput, pStock->m_hwnd, pStock->m_nLogType)==FALSE)
		{
			break;
		}*/


		//	对于多次下载失败的任务，取消
		if(nTry>=3)
		{
			delete vecStockModel[0];
			vecStockModel.erase(vecStockModel.begin());
			nTry = 0;
			continue;
		}
		nTry ++;
		if(nTry>1)
			Sleep(100);

		string downloadUrl = vecStockModel[0]->GetDownloadUrl();
		if(downloadUrl.empty()==false && ((CLotteryApp *)AfxGetApp())->TestRequestExit()==FALSE)
		{
			//////////////////////////////////////////////////////////////////////////
			//	创建管道，用于与 wget.exe 进程通信
			SECURITY_ATTRIBUTES sa;    
			HANDLE hRead,hWrite;    

			sa.nLength = sizeof(SECURITY_ATTRIBUTES);    
			sa.lpSecurityDescriptor = NULL;    
			sa.bInheritHandle = TRUE;    
			if(!CreatePipe(&hRead,&hWrite,&sa,0))    
			{     
				AfxMessageBox("CreatePipe Failed");    
				break;    
			}    

			//////////////////////////////////////////////////////////////////////////
			//	开启 wget.exe 进程，开始报表下载
			STARTUPINFO si;     
			PROCESS_INFORMATION pi;    
			ZeroMemory(&si,sizeof(STARTUPINFO));    
			si.cb = sizeof(STARTUPINFO);    
			GetStartupInfo(&si);     
			si.hStdError = hWrite;    
			si.hStdOutput = hWrite;    
			si.wShowWindow = SW_HIDE;     
			si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;    



			if(!CreateProcess(NULL,(LPSTR)downloadUrl.c_str(),NULL,NULL,TRUE,NULL,NULL,NULL,&si,&pi))    
			{     
				AfxMessageBox("CreateProcess failed!");    
				break;    
			}     

			HANDLE hThreads[2];
			hThreads[0]=pi.hProcess;
			hThreads[1]=((CLotteryApp *)AfxGetApp())->m_bAppExit;
			//WaitForSingleObject( pi.hProcess, INFINITE );

			bool bAppExit = false;
			int  nTimeoutNum = 0;
			while(1)
			{
				DWORD wfm = WaitForMultipleObjects(2, hThreads, FALSE, 1000 /*INFINITE*/);
				if(wfm==WAIT_OBJECT_0 + 1)
				{
					// 主程序退出
					bAppExit = true;
					break;
				}
				else if(wfm == WAIT_TIMEOUT)
				{
					nTimeoutNum ++;
					/*超时，则判断超过 10 秒，终止 wget.exe 进程*/
					if(nTimeoutNum>=10)
					{
						TerminateProcess(pi.hProcess,0);
						break;
					}
				}
				else 
				{
					/*wget.exe 下载完毕后终止*/
					break;
				}
			}
			if(bAppExit)
				break;

			CloseHandle(pi.hProcess);

			CloseHandle(hWrite);    

			//////////////////////////////////////////////////////////////////////////
			//	监控 wget.exe 的输出

			char buffer[4096] = {0};    
			DWORD bytesRead;    

			while(1)    
			{     
				if(NULL == ReadFile(hRead,buffer,4095,&bytesRead,NULL))     
				{     
					break;    
				}     

				//strOutput += buffer;     
				strOutput = buffer;    
				sendToOutput(strOutput, pStock->m_hwnd,  pStock->m_nLogType);
				Sleep(500);    
			}    
			CloseHandle(hRead);    

		}

		//////////////////////////////////////////////////////////////////////////
		//	转成 CSV 文件
		string strCsvFile = vecStockModel[0]->SaveAsCsv();
		if(strCsvFile.empty()==false &&((CLotteryApp *)AfxGetApp())->TestRequestExit()==FALSE)
		{
			if(strCsvFile=="NoMoreData")
			{
				// 当前数据库中已为最新数据

				//	发送“数据库导入成功”通知
				vecStockModel[0]->NotifyDatabaseImportedToWindow();

				//	删除已经成功的任务
				delete vecStockModel[0];
				vecStockModel.erase(vecStockModel.begin());
				nTry = 0;

				Sleep(100);
			}
			else if(strCsvFile=="AlreadyInMemory")
			{
				vecStockModel[0]->ImportToDatabase();

				//	发送“数据库导入成功”通知
				vecStockModel[0]->NotifyDatabaseImportedToWindow();

				//	删除已经成功的任务
				delete vecStockModel[0];
				vecStockModel.erase(vecStockModel.begin());
				nTry = 0;

				pStock->m_strResult = "NewData";
			}
			else
			{
				
			}
		}

		Sleep(20);
	}

	for(int m=0; m< vecStockModel.size(); m++)
	{
		if(vecStockModel[m])
			delete vecStockModel[m];
	}


	{
		//	通知任务的源窗口，任务完成
		stringstream ss;
		ss << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>";
		ss << "<root>";
		ss << "<msg_type>DownloadFinished</msg_type>";
		ss << "<data param1=\"" << pStock->m_nDownloadType << "\" " ;
		ss << " param2=\"" << pStock->m_strResult << "\" >";
		ss << "</data></root>";

		//sendToOutput(ss.str().c_str(), pStock->m_hwnd);
	}

	int nDownType = pStock->m_nDownloadType;
	delete pStock;

	return 0;
}
