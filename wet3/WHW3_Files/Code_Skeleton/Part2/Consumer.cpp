
#include "Consumer.h"

//this is the main consumer function
void Consumer::thread_workload() {
    while(1){
        job job = pcQueue->pop();
        assert(job.start_row >= 0 && job.end_row >= 0);
        //add more asserts for all job fields
        //will finish only if end job has arrived
        if(job.end_flag){
            break;
        }

        //calculating next board (only for the block given)
        calculateBoard(job);

        //updating that the thread has finished it`s job, must lock because all other threads will do the same
        pthread_mutex_lock(job.lock);
        (*(job.threads_left))--;
        //if all threads has finished wake the producer
        if(!(*job.threads_left)){
            pthread_cond_signal(job.cond);
        }
        pthread_mutex_unlock(job.lock);
    }
}

//TODO make this more efficient, this is only a draft to test the game
void Consumer::calculateBoard(job job) {
    int start = job.start_row;
    int end = job.end_row;
    bool_mat* board = job.curr;
    bool_mat* next_board = job.next;
    int inner_size = board->front().size();
    vector<int> v(inner_size,0);
    vector<vector<int>> temp = vector<vector<int>>(end-start + 1, v);
    for(int i = start ; i <= end ; i++){
        for(int j = 0; j < inner_size; j++){
            for(int k = j-1; k <= j+1 ; k++){
                for(int w = i-1 ; w <= i+1; w++){
                    if(!(k < 0 || w < 0 || k >= inner_size || w >= board->size() || (k == j && w == i ))){
                        if((*board)[w][k]){
                            temp[i - start][j]++;
                        }
                    }
                }
            }
        }
    }
    for(int i = start ; i <= end ; i++) {
        for (int j = 0; j < inner_size; j++) {
            if(temp[i - start][j] > 3 || temp[i - start][j] < 2){
                (*next_board)[i][j] = false;
            }
            else if(temp[i - start][j] == 3){
                (*next_board)[i][j] = true;
            }
            else{
                (*next_board)[i][j] = (*board)[i][j];
            }

        }
    }
}

