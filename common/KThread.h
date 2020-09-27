/*
 * KThread.h
 *
 *  Created on: 2014/10/27
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#ifndef _INC_KThread_
#define _INC_KThread_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>

//#include <time.h>
//#include <dirent.h>
//#include <sys/stat.h>
//#include <stdarg.h>
//#include <unistd.h>

#include "KMutex.h"
#include "KCond.h"
#include "KRunnable.h"

#include <string>
using namespace std;

class KThread
{
public:
	KThread();
	KThread(KRunnable *runnable);
	virtual ~KThread();
	pthread_t Start(KRunnable *runnable = NULL, string threadName = "");
	KRunnable* Stop();
	void sleep(uint32_t msec);
	bool isRunning() const;
	pthread_t getThreadId() const;

protected:
	virtual void onRun();

private:
	KRunnable *m_pKRunnable;
	bool m_isRunning;
	pthread_t m_pthread_t;
	string mThreadName;
	static void *thread_proc_func(void *args);
};
#endif
