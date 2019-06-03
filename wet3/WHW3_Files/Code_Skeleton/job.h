
#ifndef CODE_SKELETON_JOB_H
#define CODE_SKELETON_JOB_H
#include "utils.hpp"
//this is the job struct, it contains everything the thread need to know to work
struct job{
    uint id;
    bool_mat* curr;
    bool_mat* next;
    uint start_row;
    uint end_row;
    vector<tile_record>* tile_hist;
    bool end_flag;  //marks if the thread should end his work
    int* threads_left; //counter of how many threads has left to finish
    pthread_mutex_t* lock1;
    pthread_cond_t* cond;
    std::chrono::time_point<std::chrono::system_clock>* gt;
    uint* curr_gen;
    uint num_of_threads;
};

#endif //CODE_SKELETON_JOB_H
