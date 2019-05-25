#ifndef __THREAD_H
#define __THREAD_H
#include "../Part1/Headers.hpp"
class Thread
{
public:
	Thread(uint thread_id) 
	{
		m_thread_id = thread_id;
	} 
	virtual ~Thread() {} // Does nothing 

	// Returns true if the thread was successfully started, false if there was an error starting the thread 
	// Creates the internal thread via pthread_create 
	bool start()
	{
	    //this start the thread, sends it to do pop() and block it until
	    //producer add items to queue
        int res = pthread_create(&m_thread, NULL, entry_func, this);
	    return !res;
	}

	// Will not return until the internal thread has exited. 
	void join()
	{
	    //only a draft to avoid memory leaks
	    //TODO change this to whatever it supppose to be (maybe this is enough)
	    pthread_join(m_thread,NULL);
	}

	// Returns the user identifier
	uint thread_id()
	{
		return m_thread_id; 
	}
protected:
	// Implement this method in your subclass with the code you want your thread to run. 
	virtual void thread_workload() = 0;
	uint m_thread_id; // A number from 0 -> Number of threads initialized, providing a simple numbering for you to use

private:
	static void * entry_func(void * thread) {
	    ((Thread *)thread)->thread_workload();
	    return NULL;
	}
	pthread_t m_thread;
};

#endif
