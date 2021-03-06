
// stdafx.cpp : 只包括标准包含文件的源文件
// HiredisTest.pch 将作为预编译标头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

#include "..\redis-2.4.6_win32\deps\hiredis\hiredis.h"

void GetGUID(char * szGUID, int szLen)
{
	memset(szGUID, szLen, 0);
#ifdef WIN32
	GUID uuid;
	CoCreateGuid(&uuid);
#else
	TmUUID_t    uuid;
	TmMsg_CreateUUID(&uuid);
#endif
	sprintf_s(szGUID, szLen - 1, "%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X",
		uuid.Data1, uuid.Data2, uuid.Data3, uuid.Data4[0], uuid.Data4[1], uuid.Data4[2],
		uuid.Data4[3], uuid.Data4[4], uuid.Data4[5], uuid.Data4[6], uuid.Data4[7]);
}

//////////////////////////////////////////////////////////////////////////
//	加解锁操作

string acquire_lock(redisContext*  conn, const char * lockname, float acquire_timeout)
{
	char  identifier[64] = { 0 };
	GetGUID(identifier, 64);

	DWORD end = clock() + acquire_timeout * 1000;
	while (clock() < end)
	{
		bool bResult = false;
		redisReply * reply = (redisReply*)redisCommand(conn, "setnx lock:%s  %s", lockname, identifier);
		if (reply)
		{
			if (reply->type == REDIS_REPLY_INTEGER && reply->integer == 1 )
				bResult = true;
			freeReplyObject(reply);
		}
		if (bResult)
			return identifier;
		Sleep(5);
		
	}
	return "false";
}

//	acquire_timeout 、 lock_expire 单位为秒 （s）
string acquire_lock_with_expire(redisContext*  conn, const char * lockname, float acquire_timeout, int lock_expire)
{
	if (lock_expire <= 0)
		return "false";

	char  identifier[64] = { 0 };
	GetGUID(identifier, 64);

	DWORD end = clock() + acquire_timeout * 1000;
	while (clock() < end)
	{
		//	获取锁
		redisAppendCommand(conn, "setnx lock:%s  %s", lockname, identifier);
		redisAppendCommand(conn, "ttl lock:%s", lockname);

		int bResult = false;	//	假设该锁不存在
		redisReply * reply = NULL;
		redisGetReply(conn, (void **)&reply);
		if (reply)
		{
			//	针对 setnx 
			if (reply->type == REDIS_REPLY_INTEGER && reply->integer == 1)
				bResult = true;	// 本次加锁成功
			freeReplyObject(reply);
		}

		int nTtl = -2;
		redisGetReply(conn, (void **)&reply);
		if (reply)
		{
			// 针对 ttl 
			// 当 key 不存在时，返回 -2 。 当 key 存在但没有设置剩余生存时间时，返回 -1。 
			// 否则，以毫秒为单位，返回 key 的剩余生存时间。
			if (reply->type == REDIS_REPLY_INTEGER && reply->integer >= 0)
				nTtl = reply->integer;
			freeReplyObject(reply);
		}

		if (bResult)
		{
			//	设置该锁的过期时间
			redisReply * reply = (redisReply*)redisCommand(conn, "expire lock:%s  %d", lockname, lock_expire);
			if (reply)
			{
				/*if (reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "OK") == 0)
					bResult = true;*/
				freeReplyObject(reply);
			}

			//	成功获得锁
			return identifier;
		}
		else if(nTtl < 0)
		{
			//	设置该锁的过期时间， 此时可能是使用 acquire_lock 进行加锁的
			redisReply * reply = (redisReply*)redisCommand(conn, "expire lock:%s  %d", lockname, lock_expire);
			if (reply)
			{
				freeReplyObject(reply);
			}
		}

		Sleep(5);
	}
	return "false";
}

bool release_lock(redisContext*  conn, const char * lockname, const char * identifier)
{
	redisReply* reply = NULL;
	char buf[64] = { 0 };
	while (true)
	{
		bool bResult = false;
		reply = (redisReply*)redisCommand(conn, "watch lock:%s", lockname);
		if (reply)
		{
			if (reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "OK") == 0)
				bResult = true;
			freeReplyObject(reply);
		}
		if (bResult == false)
			break;

		//	检查线程是否仍然持有锁, 可能已经超时自动释放了
		memset(buf, 64, 0);
		reply = (redisReply*)redisCommand(conn, "get lock:%s", lockname);
		if (reply)
		{
			if (reply->type == REDIS_REPLY_STRING)
				sprintf_s(buf, 64, "%s", reply->str);
			freeReplyObject(reply);
		}
		
		if (strcmp(buf, identifier) == 0)
		{
			//	释放锁
			redisAppendCommand(conn, "multi");
			redisAppendCommand(conn, "del lock:%s", lockname);
			redisAppendCommand(conn, "exec");

			bResult = false;
			redisGetReply(conn, (void **)&reply);
			if (reply)
			{
				if (reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "OK") == 0)
					bResult = true;
				freeReplyObject(reply);
			}
			if (bResult == false)
				break;

			bResult = false;
			redisGetReply(conn, (void **)&reply);
			if (reply)
			{
				if (reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "QUEUED") == 0)
					bResult = true;
				freeReplyObject(reply);
			}
			if (bResult == false)
				break;


			bResult = false;
			redisGetReply(conn, (void **)&reply);
			if (reply)
			{
				if (reply->type == REDIS_REPLY_ARRAY)
					bResult = true;
				freeReplyObject(reply);
			}
			
			if (bResult == false)
			{
				// 有其他用户修改了锁，重试
				continue;
			}

			return true;
		}
		else
		{
			//	已经不持有该锁了，不能够多次释放同一把锁
			reply = (redisReply*)redisCommand(conn, "unwatch lock:%s", lockname);
			if (reply)
			{
				if (reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "OK") == 0)
					bResult = true;
				freeReplyObject(reply);
			}
			break;
		}
	}
	return false;
}


/////////////////////////////////////////////////////////////////////////////////////
//	信号量操作

string acquire_semaphore(redisContext*  conn, const char * semname, int limit, int timeout)
{
	char  identifier[64] = { 0 };
	GetGUID(identifier, 64);

	//	加锁是为了避免竞争条件，防止先获取到 time 的客户端踢掉 后获取time但是先得到信号量的客户端
	string lockname = "lock:semaphore:" + string(semname);
	string lock = acquire_lock_with_expire(conn, lockname.c_str(), 0.01, 1);
	if (lock == "false")
		return "false";

	bool bResult;
	redisReply* reply = NULL;
	do {
		//	获取当前时间，最先获取到 时间的客户端，首先得到信号量
		bResult = false;
		reply = (redisReply*)redisCommand(conn, "time");
		double dbNow;
		if (reply)
		{
			if (reply->type == REDIS_REPLY_ARRAY && reply->elements == 2)
			{
				int nSecond;
				sscanf_s(reply->element[0]->str, "%d", &nSecond);
				int nMacroSecond;
				sscanf_s(reply->element[1]->str, "%d", &nMacroSecond);
				dbNow = nSecond + (nMacroSecond / 1000000.0);
				bResult = true;
			}
			freeReplyObject(reply);
		}
		if (bResult == false)
			break;

		//
		char buf[128] = { 0 };
		redisAppendCommand(conn, "multi");
		//	删除超时信号量
		sprintf_s(buf, 128, "zremrangebyscore  semaphore:%s -inf  %.3f", semname, dbNow - timeout);
		redisAppendCommand(conn, buf);
		//	尝试获取信号量
		memset(buf, 0, 128);
		sprintf_s(buf, 128, "zadd  semaphore:%s  %.3f  %s", semname, dbNow, identifier);
		redisAppendCommand(conn, buf);
		//	获取新添加的标识符在集合中的排名
		redisAppendCommand(conn, "zrank  semaphore:%s  %s", semname, identifier);
		redisAppendCommand(conn, "exec");

		bResult = false;
		redisGetReply(conn, (void **)&reply);
		if (reply)
		{
			if (reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "OK") == 0)
				bResult = true;
			freeReplyObject(reply);
		}
		if (bResult == false)
			break;

		redisGetReply(conn, (void **)&reply);
		if (reply) freeReplyObject(reply);

		redisGetReply(conn, (void **)&reply);
		if (reply) freeReplyObject(reply);

		redisGetReply(conn, (void **)&reply);
		if (reply) freeReplyObject(reply);

		//	exec 命令的执行结果, 检测是否成功获取了信号量
		int nRank = limit+10;
		bResult = false;
		redisGetReply(conn, (void **)&reply);
		if (reply)
		{
			if (reply->type == REDIS_REPLY_ARRAY)
			{
				/*CString str;
				for (int j = 0; j < reply->elements; j++) {
					char buf[64] = { 0 };
					if(reply->element[j]->type== REDIS_REPLY_STRING)
						sprintf_s(buf, 64, "(%u)%s", j, reply->element[j]->str);
					else if (reply->element[j]->type == REDIS_REPLY_INTEGER)
						sprintf_s(buf, 64, "(%u)%d", j, reply->element[j]->integer);
					else if (reply->element[j]->type == REDIS_REPLY_NIL)
						sprintf_s(buf, 64, "(%u)NIL", j);
					else
						sprintf_s(buf, 64, "(%u)Others", j);
					str.Format("%s %s", str.GetBuffer(), buf);
				}*/

				if (reply->element[2]->type == REDIS_REPLY_INTEGER)
					nRank = reply->element[2]->integer;

				bResult = true;
			}
			freeReplyObject(reply);
		}
		if (bResult == false)
			break;

		if (nRank < limit)
			bResult = true;
		else
		{
			bResult = false;
			//	获取信号量失败，删除之前添加的标识符
			reply = (redisReply*)redisCommand(conn, "zrem  semaphore:%s  %s", semname, identifier);
			if (reply)
				freeReplyObject(reply);
		}
	} while (0);

	release_lock(conn, lockname.c_str(), lock.c_str());

	if (bResult)
		return identifier;

	return "false";
}


bool release_semaphore(redisContext*  conn, const char * semname, const char * identifier)
{
	//	如果该信息量已经被本客户端释放，则返回 false
	bool bResult = false;
	redisReply* reply = (redisReply*)redisCommand(conn, "zrem semaphore:%s %s", semname, identifier);
	if (reply)
	{
		if (reply->type == REDIS_REPLY_INTEGER && reply->integer >0)
		{
			bResult = true;
		}
		freeReplyObject(reply);
	}
	return bResult;
}