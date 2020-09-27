/**
  * Author	: Samson
  * Date	: 2016-01-26
  * Description	: for check momory leak
  * File	: CheckMemoryList.h
  */

#pragma once

#ifdef WIN32
	#include <windows.h>
#else
	#include <pthread.h>
#endif

const unsigned int CHECKCODE = 0x01010101;

typedef struct _tagAllocHeader  // we'll put a header at the beginning of each alloc
{
	unsigned int checkCode;
	char file[512];
    int nLineNo;
	unsigned long size;

	void Init() 
	{
		checkCode = CHECKCODE;
	}

	bool IsCheckCodeOK()
	{
		return CHECKCODE == checkCode;
	}

	void* GetData()
	{
		return ((char*)this + sizeof(_tagAllocHeader));
	}
} AllocHeader;

typedef struct _tagAllocEnd
{
	unsigned int checkCode;

	void Init()
	{
		checkCode = CHECKCODE;
	}

	bool IsCheckCodeOK()
	{
		return CHECKCODE == checkCode;
	}
} AllocEnd;

class AllocList
{
public:
	typedef struct _tagAllocItem
	{
		AllocHeader* header;
		_tagAllocItem* next;
	} AllocItem;

public:
	AllocList(void);
	virtual ~AllocList(void);

public:
	bool Insert(AllocHeader* header);
	bool Has(void* data);
	AllocHeader* GetAndRemove(void* data);
	AllocHeader* GetHeadAndRemove();
	bool Remove(AllocHeader* header);
	void RemoveAll();

private:
	bool InitLock();
	bool ReleaseLock();
	bool Lock();
	bool Unlock();

private:
	AllocItem*	m_head;

#ifdef WIN32
	CRITICAL_SECTION	m_lock;
#else
	pthread_mutex_t		m_lock;
#endif
};

class DataList
{
public:
	typedef struct _tagDataItem
	{
		void* data;
		_tagDataItem* next;
	} DataItem;

public:
	DataList(void);
	virtual ~DataList(void);

public:
	bool Insert(void* data);
	void* GetHeadAndRemove();
	void RemoveAll();

private:
	DataItem*	m_head;
};
