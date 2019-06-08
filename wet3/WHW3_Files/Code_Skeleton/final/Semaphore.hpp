#ifndef __SEMAPHORE_H
#define __SEMAPHORE_H
#include "Headers.hpp"

// Synchronization Warm up 
class Semaphore {
public:
	Semaphore() : counter(0){
	    pthread_mutexattr_t attr;
	    pthread_mutexattr_init(&attr);
	    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
	    pthread_mutex_init(&lock, &attr);
	    pthread_cond_init(&cond, NULL);
        pthread_mutexattr_destroy(&attr);
	}; // Constructs a new semaphore with a counter of 0
	Semaphore(unsigned val) : counter(val){
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
        pthread_mutex_init(&lock, &attr);
        pthread_cond_init(&cond, NULL);
        pthread_mutexattr_destroy(&attr);
	}; // Constructs a new semaphore with a counter of val

	~Semaphore(){
	    pthread_mutex_destroy(&lock);
	    pthread_cond_destroy(&cond);
	}
	void up(); // Mark: 1 Thread has left the critical section
	void down(); // Block untill counter >0, and mark - One thread has entered the critical section.

private:
	int counter;
	pthread_mutex_t lock;
    pthread_cond_t cond;

};

#endif
