// C program to demonstrate working of Semaphores 


#include <ctime>
#include <iostream>

#include "PCQueue.hpp"

PCQueue<int> pc = PCQueue<int>();

PCQueue<int> res = PCQueue<int>();

pthread_mutex_t lock ;

struct n{
    int i;
    unsigned int id;
};
std::vector<struct n> __log = std::vector<struct n>();


void* pop(void* arg){

    int r = *(int*)arg;

    if(r < 50){
        sleep(1);
    }
    int a = pc.pop();

    res.push(a);

    struct n i = {a, pthread_self()};
    pthread_mutex_lock(&lock);
    __log.insert(__log.begin(), i);
    pthread_mutex_unlock(&lock);
    return NULL;
}

void* push(void* arg){
    int a = *((int*)arg);
    pc.push(a);
    return NULL;

}

int main() 
{
    srand(time(NULL));
    pthread_mutex_init(&lock, NULL);



    std::vector<int> v = {1,2,3,4,5,6,7,8,9};
    pc.pushAll(v);
    int r = rand();

    pthread_t t1,t2,t3,t4;
    pthread_create(&t1,NULL, pop, &r);
    r = rand();
    pthread_create(&t2,NULL, pop, &r);
    r = rand();
    pthread_create(&t3,NULL, pop, &r);
    r = rand();
    pthread_create(&t4,NULL, pop, &r);

    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
    pthread_join(t3,NULL);
    pthread_join(t4,NULL);

    std::vector<struct n> k  = __log;
    res.printQueue(cout);
    pc.printQueue(cout);
    for(int i = 0 ; i < k.size(); i++){
        cout << k[i].id << "-->" << k[i].i << "  ";
    }
    return 0;

} 
