// C program to demonstrate working of Semaphores
#include "Headers.hpp"
#include "PCQueue.hpp"
#include <map>
#define ELEM_NUM 100000
#define MILISECOND 1000

typedef std::chrono::time_point<std::chrono::system_clock> Test_time_t;


class Consumer_args {
public:
    PCQueue<int>* ppcQueue;
    std::pair<int, Test_time_t> out;
};

void* produce (void* arg) { // only one thread
    printf("producing %d elements...\n", ELEM_NUM/2);
    auto pcQueue = (PCQueue<int> *) arg;
    // just fill up the queue for a little bit at the begining
    for (int i = 0; i < ELEM_NUM/2; i++) {
        pcQueue->push(i);
    }

    printf("producer went to sleep\n");
    sleep(3);
    printf("producer wakes up\n");

    printf("producing another %d elements...\n", ELEM_NUM/2);
    // wake up every mili second to add a new element

    Test_time_t start = std::chrono::system_clock::now();
    for (int i = ELEM_NUM/2; i < ELEM_NUM; i++) {
        pcQueue->push(i);
        usleep(MILISECOND);
    }
//    Test_time_t end = std::chrono::system_clock::now();
//    printf("produced in %f\n", end-start);
    return NULL;
}

void* consume (void* arg) { // many threads
//    usleep(MILISECOND);
    auto ca = (PCQueue<int>*) arg;
    ca->pop();
    return NULL;
}

int main()
{
    PCQueue<int> pcQueue;

    pthread_t producer_thread;
    pthread_create(&producer_thread, NULL, produce, &pcQueue);
    sleep(1); // should have fill ELEM/2 buy now

    pthread_t consumers[ELEM_NUM];
    //Consumer_args arr[ELEM_NUM];
    for(int i = 0; i < ELEM_NUM; i++) {
        //arr[i].ppcQueue = &pcQueue;
        pthread_create(consumers+i, NULL, consume, &pcQueue);
    }

    pthread_join(producer_thread, NULL);
    for (int i = 0; i < ELEM_NUM; i++) {
        pthread_join(consumers[i], NULL);
    }

   /* std::map<int, Test_time_t> m;

    for(int i = 0; i < ELEM_NUM; i++) {
        m.insert(arr[i].out);
    }
    auto time = m.begin()->second;
    for (const auto& pair : m) {
        if (pair.second < time) printf("pb %d\n", pair.first);
        time = pair.second;
    }*/
    return 0;
}