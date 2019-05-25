//
// Created by aviv_ on 25/05/2019.
//

#ifndef CODE_SKELETON_JOB_H
#define CODE_SKELETON_JOB_H
#include "../Part1/Headers.hpp"
//this is the job struct, it contains everything the thread need to know to work
struct job{
    bool_mat* curr;
    bool_mat* next;
    uint start_row;
    uint end_row;
    vector<double>* gen_hist;
    bool end_flag;
    int* threads_left;
    pthread_mutex_t* lock;
    pthread_cond_t* cond;
};

#endif //CODE_SKELETON_JOB_H
