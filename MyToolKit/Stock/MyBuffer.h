


//////////////////////////////////////////////////////////////////////////

class SavedData {
public:
	SavedData(unsigned char* buf, unsigned size)
		: next(NULL), data(buf), dataSize(size), numBytesUsed(0) 
	{

	}

	virtual ~SavedData() {
		delete[] data;
		if(next)
			delete next;
	}

	long	seq;	//	数据包的序号
	SavedData* next;	
	unsigned char* data;
	unsigned dataSize, numBytesUsed;
};


class MyBuffer {
public:
	MyBuffer() {
		head = NULL;
		tail = NULL;
		totalBytesUsed = 0;
		totalSize = 0;
	}

	~MyBuffer() {
		if(head)
			delete head;
	}

	void Clear()
	{
		if(head)
			delete head;

		head = NULL;
		tail = NULL;
		totalBytesUsed = 0;
		totalSize = 0;
	}

	void InputData(unsigned char * pData, int len)
	{

		totalBytesUsed += len;

		SavedData* savedData = NULL;
		if(head==NULL)
		{
			int packLength = len > 1024 * 1024 ? len : 1024 * 1024;
			totalSize += packLength;

			unsigned char* buf = new unsigned char[packLength];
			memset(buf, 0, packLength);
			memcpy(buf, pData, len);
			savedData  = new SavedData(buf, packLength);
			savedData->next = NULL;
			savedData->numBytesUsed = len;
			head = savedData;
			tail = savedData;
		}
		else
		{
			//	先保存在 最后的包中
			savedData = tail;
			int remainSize = savedData->dataSize - savedData->numBytesUsed;
			if(remainSize >0)
			{
				int copySize = remainSize >= len ? len : remainSize; 
				memcpy(savedData->data + savedData->numBytesUsed, pData, copySize);
				savedData->numBytesUsed += copySize;

				pData += copySize;
				len -= copySize;

			}
			//	最后的包放不下时，生成新的包
			if(len>0)
			{
				int packLength = len > 1024 * 1024 ? len : 1024 * 1024;
				totalSize += packLength;

				unsigned char* buf = new unsigned char[packLength];
				memset(buf, 0 , packLength);
				memcpy(buf, pData, len);
				savedData  = new SavedData(buf, packLength);
				savedData->next = NULL;
				savedData->numBytesUsed = len;

				tail->next = savedData;
				tail = savedData;
			}										
		}
	}

	unsigned char * linearize()
	{
		if(totalBytesUsed>0)
		{
			unsigned char* buf = new unsigned char[totalBytesUsed + 1];
			memset(buf, 0, totalBytesUsed + 1);

			unsigned char * buf1 = buf;
			SavedData* tmpSavedData = head;
			while(tmpSavedData!=NULL)
			{
				memcpy(buf1, tmpSavedData->data, tmpSavedData->numBytesUsed );
				buf1 += tmpSavedData->numBytesUsed;
				tmpSavedData = tmpSavedData->next;
			}
			return buf;
		}
		else 
			return NULL;
	}

	long size()
	{
		return totalBytesUsed;
	}

	SavedData *  head;
	SavedData *  tail;
	long		totalBytesUsed;
	long		totalSize;

};