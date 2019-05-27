//
// Created by aviv_ on 25/05/2019.
//

#ifndef CODE_SKELETON_JOB_H
#define CODE_SKELETON_JOB_H
#include "utils.hpp"
//this is the job struct, it contains everything the thread need to know to work
struct job{
    bool_mat* curr;
    bool_mat* next;
    uint start_row;
    uint end_row;
    vector<tile_record>* tile_hist;
    bool end_flag;
    int* threads_left;
    pthread_mutex_t* lock1;
    pthread_mutex_t* lock2;
    pthread_cond_t* cond;
};

#endif //CODE_SKELETON_JOB_H
