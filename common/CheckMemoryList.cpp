/**
  * Author	: Samson
  * Date	: 2016-01-26
  * Description	: for check momory leak
  * File	: CheckMemoryList.cpp
  */

#include "CheckMemoryList.h"
#include <stdlib.h>
#include <string.h>

AllocList::AllocList(void)
{
	m_head = NULL;
	memset(&m_lock, 0, sizeof(m_lock));
	InitLock();
}

AllocList::~AllocList(void)
{
	RemoveAll();
	ReleaseLock();
}

bool AllocList::Insert(AllocHeader* header)
{
	bool result = false;

	Lock();
	if (NULL == m_head) 
	{
		// new alloc item
		AllocItem* item = (AllocItem*)malloc(sizeof(AllocItem));
		item->header = header;
		item->next = NULL;

		m_head = item;
		result = true;
	}
	else
	{
		// go to last item
		AllocItem* lastItem = m_head;
		while (lastItem->next != NULL) {
			lastItem = lastItem->next;
		}

		// new alloc item
		AllocItem* item = (AllocItem*)malloc(sizeof(AllocItem));
		item->header = header;
		item->next = NULL;

		lastItem->next = item;
		result = true;
	}
	Unlock();

	return result;
}

bool AllocList::Has(void* data)
{
	bool result = false;

	Lock();
	for (AllocItem* item = m_head;
		item != NULL;
		item = item->next)
	{
		if (data == item->header->GetData())
		{
			result = true;
			break;
		}
	}
	Unlock();

	return result;
}
	
AllocHeader* AllocList::GetAndRemove(void* data)
{
	AllocHeader* header = NULL;

	Lock();
	if (NULL != m_head)
	{
		if (data == m_head->header->GetData())
		{
			header = m_head->header;

			AllocItem* item = m_head;
			m_head = item->next;
			free(item);
		}
		else
		{
			AllocItem* preItem = m_head;
			AllocItem* item = m_head->next;
			while (item != NULL)
			{
				if (data == item->header->GetData())
				{
					header = item->header;

					preItem->next = item->next;
					free(item);
					break;
				}

				preItem = item;
				item = item->next;
			}
		}
	}
	Unlock();

	return header;
}

AllocHeader* AllocList::GetHeadAndRemove()
{
	AllocHeader* header = NULL;

	Lock();
	if (NULL != m_head) 
	{
		header = m_head->header;

		AllocItem* item = m_head;
		m_head = m_head->next;
		free(item);
	}
	Unlock();

	return header;
}
	
bool AllocList::Remove(AllocHeader* header)
{
	bool result = false;

	Lock();
	if (NULL != m_head)
	{
		if (header == m_head->header)
		{
			AllocItem* item = m_head;
			m_head = item->next;
			free(item);
			result = true;
		}
		else
		{
			AllocItem* preItem = m_head;
			AllocItem* item = m_head->next;
			while (item != NULL)
			{
				if (header == item->header)
				{
					preItem->next = item->next;
					free(item);
					result = true;
					break;
				}

				preItem = item;
				item = item->next;
			}
		}
	}
	Unlock();

	 return result;
}

void AllocList::RemoveAll()
{
	Lock();
	while (NULL != m_head)
	{
		AllocItem* item = m_head->next;
		delete m_head;
		m_head = item;
	}
	Unlock();
}

bool AllocList::InitLock()
{
	bool result = false;
#ifdef WIN32
	result = InitializeCriticalSectionAndSpinCount(&m_lock, 1) ? true : false;
#else
	result = pthread_mutex_init(&m_lock, NULL) == 0;
#endif
	return result;
}

bool AllocList::ReleaseLock()
{
	bool result = true;
#ifdef WIN32
	DeleteCriticalSection(&m_lock);
#else
	pthread_mutex_destroy(&m_lock);
#endif
	return result;
}

bool AllocList::Lock()
{
	bool result = false;
#ifdef WIN32
	EnterCriticalSection(&m_lock);
	result = true;
#else
	result = pthread_mutex_lock(&m_lock) == 0;
#endif
	return result;
}

bool AllocList::Unlock()
{
	bool result = false;
#ifdef WIN32
	LeaveCriticalSection(&m_lock);
	result = true;
#else
	result = pthread_mutex_unlock(&m_lock) == 0;
#endif
	return result;
}

DataList::DataList(void)
{
	m_head = NULL;
}

DataList::~DataList(void)
{
	RemoveAll();
}

bool DataList::Insert(void* data)
{
	bool result = false;
	if (NULL == m_head)
	{
		// new item
		DataItem* item = new DataItem;
		item->data = data;
		item->next = NULL;

		// add to list header
		m_head = item;
		result = true;
	}
	else
	{
		// new item
		DataItem* item = new DataItem;
		item->data = data;
		item->next = NULL;

		// go to last item
		DataItem* lastItem = m_head;
		while (lastItem->next != NULL) {
			lastItem = lastItem->next;
		}

		// insert to list
		lastItem->next = item;
		result = true;
	}
	return result;
}
	
void* DataList::GetHeadAndRemove()
{
	void* data = NULL;
	if (NULL != m_head)
	{
		DataItem* dataItem = m_head;
		data = dataItem->data;

		m_head = m_head->next;
		delete dataItem;
	}

	return data;
}
	
void DataList::RemoveAll()
{
	DataItem* dataItem = m_head;
	while (NULL != dataItem)
	{
		delete dataItem;
		dataItem = m_head->next;
	}
}


