#include "Semaphore.hpp"

void Semaphore::up() {
    //locking to avoid changing semaphore from 2 different threads at the same time
    pthread_mutex_lock(&lock);
    counter++;
    pthread_cond_signal(&cond); // wake a singel thread that waits for the semaphore
    pthread_mutex_unlock(&lock);
}

void Semaphore::down() {
    pthread_mutex_lock(&lock);
    while(counter <= 0){ //if counter <= 0 we block
        pthread_cond_wait(&cond,&lock);
    }
    counter--;
    pthread_mutex_unlock(&lock);

}

