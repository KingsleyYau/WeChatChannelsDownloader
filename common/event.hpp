
#ifndef _INC_EVENT_
#define _INC_EVENT_

#include <pthread.h>

class CAutoLock
{
    private:
        pthread_mutex_t *m_pLock;

    public:
        CAutoLock(pthread_mutex_t *apLock) : m_pLock(apLock) {
            pthread_mutex_lock(m_pLock);
        };

        ~CAutoLock() {
            pthread_mutex_unlock(m_pLock);
        };
};

class Event
{
    private:
        pthread_mutex_t mutex;
        pthread_cond_t _cond;
        //bool _signaled;
        //int _count;
        
        timespec tTime;
	
    public:
        Event() {
            pthread_cond_init(&_cond, NULL);
            pthread_mutex_init(&mutex, NULL);
            //_signaled = false;
            //_count = 0;
        }

        ~Event() {
            pthread_mutex_destroy(&mutex);
            pthread_cond_destroy(&_cond);
        }

        void reset(void) {
            //_signaled = false;
        }

        void signal(void) {
            pthread_mutex_lock(&mutex); 
            //_signaled = true;
            //++_count;
            pthread_cond_broadcast(&_cond);
            pthread_mutex_unlock(&mutex); 
        }

        bool wait(void) {
            pthread_mutex_lock(&mutex); 
            //long count = _count;
            //while(_count == count) {
                //pthread_cond_wait(&_cond, &mutex);  
            	tTime.tv_sec =  time(NULL) + 1;
            	tTime.tv_nsec = 0;
            	pthread_cond_timedwait(&_cond, &mutex, &tTime);
            //}
            pthread_mutex_unlock(&mutex); 
            return true;
        }
};

#endif
