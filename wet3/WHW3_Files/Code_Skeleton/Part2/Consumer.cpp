
#include "Consumer.h"
#include "Arr.h"
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

void Consumer::calculateBoard(job job){
    //pre calculated array
    vector<bool> convert_arr = _CONVERT;
    //job is calculated according to the original mat without the frame
    int start = job.start_row + 1; // add 1 to ignore first row and col of zeroes
    int end = job.end_row + 1;
    bool_mat* board = job.curr;
    bool_mat* next_board = job.next;
    //real size of inner row
    int inner_size = board->front().size() - 2;
    uint mat_conv_index;  // represent 3x3 matrix converted to int
    uint indicator;
    for(int i = start; i <= end ; i++){
        mat_conv_index = 0; //for each new row we calculate, we first reset the mat_conv_index
        //adding col 1 to the mat_conv_index at the beginning of each new row
        for(int w = i - 1 ; w <= i + 1; w++){
            indicator = ((*board)[w][1]) ? 1 : 0;
            mat_conv_index = mat_conv_index<<1; //shift left the index and add the indicator
            mat_conv_index += indicator;
        }
        //for each cell in the row, we remove first 3 bits of the mat_conv_index
        //and append the next row to the mat_conv_index
        for(int j = 1; j < inner_size + 1; j++){
            for(int k = -1 ; k < 2; k++){ //iterate over col j+1 and append the values
                indicator = ((*board)[i+k][j+1]) ? 1 : 0;
                mat_conv_index = mat_conv_index<<1;
                mat_conv_index += indicator;
            }
            mat_conv_index = mat_conv_index & 0x1ff; //mask only the first 9 bits (which represent the matrix)
            (*next_board)[i][j] = convert_arr[mat_conv_index]; //using the pre calculated array to find out the value
        }
    }

}

//TODO make this more efficient, this is only a draft to test the game
/*void Consumer::calculateBoard(job job) {
    int start = job.start_row + 1; // add 1 to ignore first row of zeroes
    int end = job.end_row + 1;
    bool_mat* board = job.curr;
    bool_mat* next_board = job.next;
    int inner_size = board->front().size() - 2;
    vector<int> v(inner_size,0);
    vector<vector<int>> temp = vector<vector<int>>(end-start + 1, v);
    for(int i = start ; i <= end ; i++){
        for(int j = 1; j < inner_size + 1; j++){
            for(int k = j-1; k <= j+1 ; k++){
                for(int w = i-1 ; w <= i+1; w++){
                    if(!(k == j && w == i )){
                        if((*board)[w][k]){
                            temp[i - start][j-1]++;
                        }
                    }
                }
            }
        }
    }
    for(int i = start ; i <= end ; i++) {
        for (int j = 1; j < inner_size+1; j++) {
            if(temp[i - start][j-1] > 3 || temp[i - start][j-1] < 2){
                (*next_board)[i][j] = false;
            }
            else if(temp[i - start][j-1] == 3){
                (*next_board)[i][j] = true;
            }
            else{
                (*next_board)[i][j] = (*board)[i][j];
            }

        }
    }*/

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


//}

