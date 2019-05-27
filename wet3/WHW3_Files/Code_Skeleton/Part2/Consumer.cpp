
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
        auto gen_start = std::chrono::system_clock::now();
        //calculating next board (only for the block given)
        calculateBoard(job);
        auto gen_end = std::chrono::system_clock::now();
        //compute calculation time and push to tile hist
        double compute_time = (double)std::chrono::duration_cast<std::chrono::microseconds>(gen_end - gen_start).count();
        tile_record rec = {compute_time, m_thread_id};
        // must lock because we don`t have the step number here
        //TODO check at what order we need to put the tile records (might be by thread_id)
        pthread_mutex_lock(job.lock2);
        job.tile_hist->push_back(rec);
        pthread_mutex_unlock(job.lock2);

        //updating that the thread has finished it`s job, must lock because all other threads will do the same
        pthread_mutex_lock(job.lock1);
        (*(job.threads_left))--;
        //if all threads has finished wake the producer
        if(!(*job.threads_left)){
            pthread_cond_signal(job.cond);
        }
        pthread_mutex_unlock(job.lock1);
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

    // TODO: test if this method makes it faster
    /*
    int neighbors;
    for (uint i=0; i < this->num_of_rows; i++) {
        for (uint j=0; j < this->num_of_cols; j++) {
            neighbors = this->number_of_neighbors(i, j);
            if (neighbors == 3) {
                this->next_field[i][j] = true;
            }
            else if (this->curr_field[i][j] && neighbors == 2) { // the case of 3 neighbors is already covered
                this->next_field[i][j] = true;
            }
            else {
                this->next_field[i][j] = false;
            }
        }
    }

     int Game::number_of_neighbors(uint i, uint j) {
    int counter = 0;
    // adding is faster than branching
    // TODO: idea to make it faster - add dummy rows and column around the field, and don't branch at all
    if (i == 0) { // first row
        if (j == 0) {
            counter += this->curr_field[i+1][j+1] + this->curr_field[i+1][j] +  this->curr_field[i][j+1];
        }
        else if (j == this->num_of_cols - 1) {
            counter += this->curr_field[i+1][j-1] + this->curr_field[i+1][j] +  this->curr_field[i][j-1];
        }
        else {
            counter += this->curr_field[i+1][j+1] + this->curr_field[i+1][j] +  this->curr_field[i][j+1] + \
            this->curr_field[i+1][j-1] +  this->curr_field[i][j-1];
        }
    }
    else if (j == 0) { // first column
        if (i == this->num_of_rows - 1) {
            counter += this->curr_field[i-1][j+1] + this->curr_field[i-1][j] +  this->curr_field[i][j+1];
        }
        else {
            counter += this->curr_field[i+1][j+1] + this->curr_field[i+1][j] +  this->curr_field[i][j+1] + \
            this->curr_field[i-1][j+1] +  this->curr_field[i-1][j];
        }
    }
    else if (i == this->num_of_rows - 1) { // last row
        if (j == this->num_of_cols - 1) {
            counter += this->curr_field[i-1][j-1] + this->curr_field[i-1][j] +  this->curr_field[i][j-1];
        }
        else {
            counter += this->curr_field[i-1][j+1] + this->curr_field[i-1][j] +  this->curr_field[i][j+1] + \
            this->curr_field[i-1][j-1] +  this->curr_field[i][j-1];
        }
    }
    else if (j == this->num_of_cols - 1) { // last column
        if (i == this->num_of_rows - 1) {
            counter += this->curr_field[i-1][j-1] + this->curr_field[i-1][j] +  this->curr_field[i][j-1];
        }
        else {
            counter += this->curr_field[i+1][j-1] + this->curr_field[i+1][j] +  this->curr_field[i][j-1] + \
            this->curr_field[i-1][j-1] +  this->curr_field[i-1][j];
        }
    }
    else { // middle of the field
        counter = this->curr_field[i-1][j-1] + this->curr_field[i-1][j] + this->curr_field[i-1][j+1] + \
        this->curr_field[i][j-1] + this->curr_field[i][j+1] + \
        this->curr_field[i+1][j-1] + this->curr_field[i+1][j] + this->curr_field[i+1][j+1];
    }
    return counter;
     }

     */


}

