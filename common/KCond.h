/*
 * KCond.h
 *
 *  Created on: 2014/10/27
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#ifndef KCond_H_
#define KCond_H_

#include "KMutex.h"
#include <sys/time.h>

class KCond {
public:
	KCond() {
		initlock();
		initcond();
	}
	virtual ~KCond() {
		desrtoycond();
		desrtoylock();
	}
	int signal() {
		return pthread_cond_signal(&m_Cond);
	}
	int timedwait(int second) {

		struct timeval now;
		gettimeofday(&now, NULL);

		struct timespec outtime;
		outtime.tv_sec = now.tv_sec + second;
		outtime.tv_nsec = now.tv_usec * 1000;

		return pthread_cond_timedwait(&m_Cond, &m_Mutex, &outtime);
	}
	int wait() {
		return pthread_cond_wait(&m_Cond, &m_Mutex);
	}
	int broadcast() {
		return pthread_cond_broadcast(&m_Cond);
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
	int initlock(){
		return pthread_mutex_init(&m_Mutex, NULL);
	}
	int desrtoylock(){
		return pthread_mutex_destroy(&m_Mutex);
	}
	int initcond() {
		return pthread_cond_init(&m_Cond, NULL);
	}
	int desrtoycond() {
		return pthread_cond_destroy(&m_Cond);
	}
private:
	pthread_cond_t m_Cond;
	pthread_condattr_t m_Condattr;
	pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_Mutexattr;
};

#endif /* KCond_H_ */
