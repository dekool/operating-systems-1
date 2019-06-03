
#include "Consumer.h"
#include "LookUpTable.h"
//this is the main consumer function
void Consumer::thread_workload() {
    while(1){
        job job = pcQueue->pop(); //will block until there is a job to pop
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
        double start_time = (double)std::chrono::duration_cast<std::chrono::microseconds>(gen_start - *(job.gt)).count();
        double end_time = (double)std::chrono::duration_cast<std::chrono::microseconds>(gen_end - *(job.gt)).count();
        tile_record rec = {compute_time, m_thread_id, start_time, end_time};
        int tile_hist_index = m_thread_id + (*job.curr_gen) * job.num_of_threads;
        (*job.tile_hist)[tile_hist_index] = rec;

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
    vector<bool> look_up_table = LOOK_UP_TABLE;
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
        //and append the next col to the mat_conv_index
        for(int j = 1; j < inner_size + 1; j++){
            for(int k = -1 ; k < 2; k++){ //iterate over col j+1 and append the values
                indicator = ((*board)[i+k][j+1]) ? 1 : 0;
                mat_conv_index = mat_conv_index<<1;
                mat_conv_index += indicator;
            }
            mat_conv_index = mat_conv_index & 0x1ff; //mask only the first 9 bits (which represent the matrix)
            (*next_board)[i][j] = look_up_table[mat_conv_index]; //using the pre calculated array to find out the value
        }
    }

}

