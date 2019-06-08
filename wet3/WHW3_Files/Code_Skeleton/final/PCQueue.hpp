#ifndef _QUEUEL_H
#define _QUEUEL_H
#include "Headers.hpp"
#include "Semaphore.hpp"

// Single Producer - Multiple Consumer queue
template <typename T>class PCQueue
{

public:
    PCQueue() : queue(), producerWaiting(false), queueItems(){
        //initializing errorcheck lock and cond
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
        pthread_mutex_init(&lock, &attr);
        pthread_cond_init(&cond, NULL);
        pthread_mutexattr_destroy(&attr);
    };

    ~PCQueue(){
        pthread_mutex_destroy(&lock);
        pthread_cond_destroy(&cond);
    }

	// Blocks while queue is empty. When queue holds items, allows for a single
	// thread to enter and remove an item from the front of the queue and return it. 
	// Assumes multiple consumers.
    T pop(){
        queueItems.down(); //will block if there are no items
        pthread_mutex_lock(&lock);
        while(producerWaiting){ //giving the producer priority over consumers
            pthread_cond_wait(&cond, &lock); //waits until there is no producer
        }

        T item = queue.front();
        queue.pop();
        pthread_mutex_unlock(&lock);

        return item;
	};

	// Allows for producer to enter with *minimal delay* and push items to back of the queue.
	// Hint for *minimal delay* - Allow the consumers to delay the producer as little as possible.  
	// Assumes single producer 
	void push(const T& item){
        producerWaiting = true; // marks to all threads that producer has arrived
        //consumer who pops right now will finish but no other consumer will get the lock before producer
        pthread_mutex_lock(&lock);
        queue.push(item);
        queueItems.up();  //will wake one consumer who waits for item
        producerWaiting = false;
        pthread_cond_broadcast(&cond); //wake all consumers that waited for producer to finish
        pthread_mutex_unlock(&lock);
	};

	//Allow for producer to enter and push as many items as he like to the back of the queue.
	void pushAll(const std::vector<T>& items){
        producerWaiting = true; //marks that producer has arrived
        pthread_mutex_lock(&lock);
        //pushing all the items in the vector
        for(int i = 0; i < items.size(); i++){
            queue.push(items[i]);
            queueItems.up();
        }
        producerWaiting = false;
        pthread_cond_broadcast(&cond); //wake all consumers that waited for producer to finish
        pthread_mutex_unlock(&lock);
	};
	
private:
	// Add your class members here
	std::queue<T> queue; //queue for the objects we push and pop
	bool producerWaiting;   //indicator if there is a producer waiting to push or is pushing
    Semaphore queueItems;   //num of items in the queue which no consumer took ownership on
    pthread_mutex_t lock;
    pthread_cond_t cond;

};
// Recommendation: Use the implementation of the std::queue for this exercise
#endif