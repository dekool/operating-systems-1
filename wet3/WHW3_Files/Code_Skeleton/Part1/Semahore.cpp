#include "Semaphore.hpp"

void Semaphore::up() {
    pthread_mutex_lock(&lock);
    counter++;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&lock);
}

void Semaphore::down() {
    pthread_mutex_lock(&lock);
    while(counter <= 0){
        pthread_cond_wait(&cond,&lock);
    }
    counter--;
    pthread_mutex_unlock(&lock);

}

