%{
/* We usually need these... */
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h> 

/* Include this to use yylex_destroy for flex version < 2.5.9 */
#include "flex_memory_fix.h"

/* This is required and is generated automatically by bison from the .y file */
#include "y.tab.h"

/* Local stuff we need here... */
#include <math.h>

extern int eval_getinput(char *buf, int maxlen);
#undef YY_INPUT
#define YY_INPUT(buf, retval, maxlen)        (retval = eval_getinput(buf, maxlen))


int chars = 0;
int words = 0;
int lines = 0;

HANDLE hFileMapping   = NULL; 
LPVOID lpShareMemory  = NULL; 
HANDLE hEvtRequestSubmitted = NULL;
HANDLE hEvtResultReturned = NULL;	
int	   nRequestAlreadyRead = 0;

char article[2048] = { 0 };

%}

/* Add this to get line numbers... */
%option yylineno

%%
[a-zA-Z0-9\.\-]+	{ /*  */ memcpy(article + chars, yytext, strlen(yytext));  chars += strlen(yytext); }

(\xCE\xBB\xD3\xDA[\x80-\xFF]+((\xA3\xAC)|(\xA1\xA3)))+ { /* 位于xxx */ memcpy(article + chars, yytext, strlen(yytext));  chars += strlen(yytext); }

(\xC3\xBF\xC6\xBD\xB7\xBD\xC3\xD7)+ { /* 每平方米 */ memcpy(article + chars, yytext, strlen(yytext));  chars += strlen(yytext); }

(\xBD\xA8\xD6\xFE\xC3\xE6\xBB\xFD)+ { /* 建筑面积 */ memcpy(article + chars, yytext, strlen(yytext));  chars += strlen(yytext); }

(\xCD\xF2\xC6\xBD\xB7\xBD\xC3\xD7)+ { /* 万平方米 */ memcpy(article + chars, yytext, strlen(yytext));  chars += strlen(yytext); }

(\xC6\xBD\xB7\xBD\xC3\xD7)+ { /* 平方米 */ memcpy(article + chars, yytext, strlen(yytext));  chars += strlen(yytext); }

(\xD7\xDC\xB5\xD8\xBC\xDB)+ { /* 总地价 */ memcpy(article + chars, yytext, strlen(yytext));  chars += strlen(yytext); }

(\xB3\xC9\xBD\xBB\xBC\xDB)+ { /* 成交价 */ memcpy(article + chars, yytext, strlen(yytext));  chars += strlen(yytext); }

(\xC8\xDD\xBB\xFD\xC2\xCA)+ { /* 容积率 */ memcpy(article + chars, yytext, strlen(yytext));  chars += strlen(yytext); }

(\xB3\xC9\xBD\xBB\xB6\xEE)+ { /* 成交额 */ memcpy(article + chars, yytext, strlen(yytext));  chars += strlen(yytext); }

(\xD7\xDC\xBC\xDB)+ { /* 总价 */ memcpy(article + chars, yytext, strlen(yytext));  chars += strlen(yytext); }

(\xB3\xF6\xC8\xC3)+ { /* 出让 */ memcpy(article + chars, yytext, strlen(yytext));  chars += strlen(yytext); }

(\xC3\xE6\xBB\xFD)+ { /* 面积 */ memcpy(article + chars, yytext, strlen(yytext));  chars += strlen(yytext); }

(\xB5\xD8\xBF\xE9)+ { /* 地块 */ memcpy(article + chars, yytext, strlen(yytext));  chars += strlen(yytext); }

(\xCE\xBB\xD3\xDA)+ { /* 位于 */ memcpy(article + chars, yytext, strlen(yytext));  chars += strlen(yytext); }

(\xCD\xF2\xD4\xAA)+ { /* 万元 */ memcpy(article + chars, yytext, strlen(yytext));  chars += strlen(yytext); }

(\xD2\xDA\xD4\xAA)+ { /* 亿元 */ memcpy(article + chars, yytext, strlen(yytext));  chars += strlen(yytext); }

(\xD7\xDC\xBC\xDB)+ { /* 总价 */ memcpy(article + chars, yytext, strlen(yytext));  chars += strlen(yytext); }

(\xD7\xDC\xB6\xEE)+ { /* 总额 */ memcpy(article + chars, yytext, strlen(yytext));  chars += strlen(yytext); }

(\xD7\xDC\xBC\xC6)+ { /* 总计 */ memcpy(article + chars, yytext, strlen(yytext));  chars += strlen(yytext); }

(\xBE\xBA\xB5\xC3)+ { /* 竞得 */ memcpy(article + chars, yytext, strlen(yytext));  chars += strlen(yytext); }

(\xBE\xB9\xB5\xC3)+ { /* 竟得 */ memcpy(article + chars, yytext, strlen(yytext));  chars += strlen(yytext); }

(\xBB\xF1\xB5\xC3)+ { /* 获得 */ memcpy(article + chars, yytext, strlen(yytext));  chars += strlen(yytext); }

(\xC4\xEA\xCF\xDE)+ { /* 年限 */ memcpy(article + chars, yytext, strlen(yytext));  chars += strlen(yytext); }

(\xB1\xE0\xBA\xC5)+ { /* 编号 */ memcpy(article + chars, yytext, strlen(yytext));  chars += strlen(yytext); }

(\xD3\xC3\xCD\xBE)+ { /* 用途 */ memcpy(article + chars, yytext, strlen(yytext));  chars += strlen(yytext); }

(\xC9\xCC\xB7\xFE)+ { /* 商服 */ memcpy(article + chars, yytext, strlen(yytext));  chars += strlen(yytext); }

(\xD7\xA1\xD5\xAC)+ { /* 住宅 */ memcpy(article + chars, yytext, strlen(yytext));  chars += strlen(yytext); }

(\xA9\x4F)+ { /* ㎡ */ memcpy(article + chars, yytext, strlen(yytext));  chars += strlen(yytext); }

(\xCE\xAA)+ { /* 为 */ memcpy(article + chars, yytext, strlen(yytext));  chars += strlen(yytext); }

(\xD2\xD4)+ { /* 以 */ memcpy(article + chars, yytext, strlen(yytext));  chars += strlen(yytext); }

(\xC4\xEA)+ { /* 年 */ memcpy(article + chars, yytext, strlen(yytext));  chars += strlen(yytext); }

(\xBA\xC5)+ { /* 号 */ memcpy(article + chars, yytext, strlen(yytext));  chars += strlen(yytext); }

(\xD4\xAA)+ { /* 元 */ memcpy(article + chars, yytext, strlen(yytext));  chars += strlen(yytext); }

(\xA1\xDC)+ { /* ≤ */ memcpy(article + chars, yytext, strlen(yytext));  chars += strlen(yytext); }

(\xA3\xBC)+ { /* ＜ */ memcpy(article + chars, yytext, strlen(yytext));  chars += strlen(yytext); }

(\xA1\xA2)+ { /* 、 */ memcpy(article + chars, yytext, strlen(yytext));  chars += strlen(yytext); }

(\xA3\xAC)+ { /* ， */ memcpy(article + chars, yytext, strlen(yytext));  chars += strlen(yytext); }

(\xA3\xBB)+ { /* ； */ memcpy(article + chars, yytext, strlen(yytext));  chars += strlen(yytext); }

(\xA1\xA3)+ { /* 。 */ memcpy(article + chars, yytext, strlen(yytext));  chars += strlen(yytext); }



%%

int eval_getinput(char *buf, int maxlen)
{
	int nRequestSize = strlen(lpShareMemory);
	if(nRequestSize - nRequestAlreadyRead ==0)
	{
		return 0 ;
	}
	else if(nRequestSize - nRequestAlreadyRead > maxlen)
	{
		memcpy(buf, (char *)lpShareMemory + nRequestAlreadyRead, maxlen);
		nRequestAlreadyRead += maxlen;
		return maxlen;
	}
	else
	{
		int nSend = nRequestSize - nRequestAlreadyRead;
		memcpy(buf, (char *)lpShareMemory + nRequestAlreadyRead, nSend);
		nRequestAlreadyRead = nRequestSize;
		return nSend;	
	}
}

int main(int argc ,char* argv[])
{

	// create share memory 
	hFileMapping = CreateFileMapping(INVALID_HANDLE_VALUE, 
		NULL, 
		PAGE_READWRITE, 
		0, 
		16*1024, 
		"ShareMemoryTest"); 
	if (NULL == hFileMapping) 
	{ 
		printf("%s, %d\n", "CreateFileMapping", GetLastError());
		goto CLIENT_SHARE_MEMORY_END; 
	} 
  
	lpShareMemory = MapViewOfFile(hFileMapping, 
		FILE_MAP_ALL_ACCESS, 
		0, 
		0,   //memory start address 
		0);   //all memory space 
		
	if (NULL == lpShareMemory) 
	{ 
		printf("%s, %d\n", "MapViewOfFile", GetLastError());
		goto CLIENT_SHARE_MEMORY_END; 
	} 
	
	hEvtRequestSubmitted = CreateEvent(NULL, 
		FALSE, 
		FALSE, 
		"RequestSubmitted"); 
	if (NULL == hEvtRequestSubmitted) 
	{ 
		printf("%s, %d\n", "hEvtRequestSubmitted", GetLastError());
		goto CLIENT_SHARE_MEMORY_END; 
	} 

	hEvtResultReturned = CreateEvent(NULL, 
		FALSE, 
		FALSE, 
		"ResultReturned");
	if (NULL == hEvtResultReturned) 
	{ 
		printf("%s, %d\n", "hEvtResultReturned", GetLastError());
		goto CLIENT_SHARE_MEMORY_END; 
	}  
  
	while(1)
	{
		nRequestAlreadyRead = 0;
		
		WaitForSingleObject(hEvtRequestSubmitted, INFINITE);
				
		memset(article, 0, 2048);
		printf("================================please input text\n");
		yyparse();
		printf("%8d%8d%8d\n", lines, words, chars);
		printf("%s\n", article);
		
		memcpy(lpShareMemory, article, strlen(article));
		
		SetEvent(hEvtResultReturned);
	}
	

	yylex_destroy();	/* Add to clean up memory leaks */
	
CLIENT_SHARE_MEMORY_END: 
	//release share memory 
	if (NULL != lpShareMemory)   UnmapViewOfFile(lpShareMemory); 
	if (NULL != hFileMapping)    CloseHandle(hFileMapping); 
	if (NULL != hEvtRequestSubmitted)    CloseHandle(hEvtRequestSubmitted); 
	if (NULL != hEvtResultReturned)    CloseHandle(hEvtResultReturned); 
	
	getchar();
	
    return 0;
} 
