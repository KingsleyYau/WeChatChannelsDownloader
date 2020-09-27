/*
 * KThread.cpp
 *
 *  Created on: 2014/10/27
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#include "KThread.h"

#include <time.h>
#include <sys/prctl.h>

KThread::KThread() {
	m_pthread_t = 0;
	m_isRunning = false;
	m_pKRunnable = NULL;
}

KThread::KThread(KRunnable *runnable) {
	m_pthread_t = 0;
	m_isRunning = false;
	this->m_pKRunnable = runnable;
}

KThread::~KThread() {
//	stop();
}

void* KThread::thread_proc_func(void *args){
	pthread_t threadId = 0;
	KThread *pKThread = (KThread*)args;
	if(pKThread){
		threadId = pKThread->getThreadId();
		pKThread->onRun();
	}
	return (void*)0;
}

pthread_t KThread::Start(KRunnable *runnable, string threadName){
	if( isRunning() ) {
		return 0;
	}

	if( runnable != NULL ) {
		this->m_pKRunnable = runnable;
	}

	mThreadName = threadName;

	pthread_attr_t attrs;
	pthread_attr_init(&attrs);
	pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_JOINABLE);

	int ret = pthread_create(&m_pthread_t, &attrs, &thread_proc_func, (void*)this);
	if(0 != ret) {
		m_pthread_t = 0;
	}
	return m_pthread_t;
}

KRunnable* KThread::Stop() {
	if(isRunning()) {
		if(0 != pthread_join(m_pthread_t, NULL)){
		}
		else{
		}
	}

	KRunnable* pRunnable = m_pKRunnable;
	m_pKRunnable = NULL;
	m_pthread_t = 0;
	return pRunnable;
}

void KThread::sleep(uint32_t msec){
}

pthread_t KThread::getThreadId() const {
	//return pthread_self();
	return m_pthread_t;
}

bool KThread::isRunning() const{
	bool bFlag = false;
	if( m_pthread_t != 0 ) {
		bFlag = true;
	}
	return bFlag;
}

void KThread::onRun() {
	if ( mThreadName.length() > 0 ) {
		prctl(PR_SET_NAME, mThreadName.c_str());
	}

	if( NULL != m_pKRunnable ) {
		m_pKRunnable->onRun();
	}
}
