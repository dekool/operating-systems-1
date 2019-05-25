
#ifndef CODE_SKELETON_CONSUMER_H
#define CODE_SKELETON_CONSUMER_H

#include "../Part1/PCQueue.hpp"
#include "job.h"
#include "Thread.hpp"


class Consumer : public Thread {
public:

    Consumer(uint thread_id, PCQueue<job>* queue): Thread(thread_id), pcQueue(queue){};

    void thread_workload();

private:
    PCQueue<job>* pcQueue;

    void calculateBoard(job job);
};


#endif //CODE_SKELETON_CONSUMER_H
