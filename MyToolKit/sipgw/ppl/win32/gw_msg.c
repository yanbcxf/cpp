#include <stdio.h>
#include <Windows.h>
#include "gw_msg.h"

DWORD que_list[MAX_QUEQUE_NUM];

void SetQueque(int mod, DWORD queque)
{
	int key = mod;

	if(mod >= MOD_NUM+7)
	{
		printf("mod %d out of range\n", mod);
		return;
	}

	if(queque != -1)
	{
		que_list[mod]	= queque;
	}
	else
	{
		printf("CreatQueque msgget");
	}
}

DWORD GetQueue(int mod)
{
	DWORD queque = que_list[mod];

	return queque;
}

void SendMessageEx(int srcid, int destid, int msgtype, void *data, int msg_len)
{
	SN_MSG *pmsg = (SN_MSG *)malloc(sizeof(SN_MSG) + msg_len);
	DWORD src_id, dest_id;

	pmsg->thread_id = srcid;
	pmsg->msg_len = msg_len;

	src_id = GetQueue(srcid);
	dest_id = GetQueue(destid);

	if (!(msg_len == 0 || data == NULL))
	{
		memcpy(pmsg->para, data, msg_len);
	}

	if (!PostThreadMessage(dest_id, msgtype, (WPARAM)pmsg, 0))
	{
		printf("sendmessage err %d\n", GetLastError());
		free(pmsg);
		pmsg = NULL;
	}
	
	return;
}
