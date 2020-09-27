/*
 * KMutex.h
 *
 *  Created on: 2014/10/27
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#ifndef _INC_KMutex_
#define _INC_KMutex_

#include <pthread.h>
using namespace std;

class KMutex
{
public:
	typedef enum MutexType {
		MutexType_Normal = PTHREAD_MUTEX_NORMAL,
		MutexType_ErrorCheck = PTHREAD_MUTEX_ERRORCHECK,
		MutexType_Recursive = PTHREAD_MUTEX_RECURSIVE,
		MutexType_Default = PTHREAD_MUTEX_DEFAULT,
	} MutexType;

public:
	KMutex() {
		initlock();
	}

	KMutex(MutexType type) {
		initlock(type);
	}

	~KMutex(){
		desrtoylock();
	}

	int trylock(){
		return pthread_mutex_trylock(&m_Mutex);
	}

	int lock(){
		return pthread_mutex_lock(&m_Mutex);
	}

	int unlock(){
		return pthread_mutex_unlock(&m_Mutex);
	}

protected:
	void initlock(MutexType type = MutexType_Default) {
		pthread_mutexattr_t mattr;
        pthread_mutexattr_init(&mattr);
        pthread_mutexattr_settype(&mattr, (int)type);
        pthread_mutex_init(&m_Mutex, &mattr);
        pthread_mutexattr_destroy(&mattr);
	}

	void desrtoylock() {
		pthread_mutex_destroy(&m_Mutex);
	}

private:
	pthread_mutex_t m_Mutex;

};
#endif
