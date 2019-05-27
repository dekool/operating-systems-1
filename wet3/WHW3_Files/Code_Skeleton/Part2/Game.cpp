#include "Game.hpp"
#include "utils.hpp"

/*--------------------------------------------------------------------------------
								
--------------------------------------------------------------------------------*/
Game::Game(game_params gameParams) : mats(2), job_array(2){
    //initialize all the fields needed in Game
    m_gen_num = gameParams.n_gen;
    m_thread_num = gameParams.n_thread;
    input_filename = gameParams.filename;
    interactive_on = gameParams.interactive_on;
    print_on = gameParams.print_on;
    //initialize the locks
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_init(&lock1, &attr);
    pthread_mutex_init(&lock2, &attr);
    pthread_cond_init(&cond, NULL);
    pthread_mutexattr_destroy(&attr);
    //matrix and job vector pointers
    curr = &mats[0];
    next = &mats[1];
    curr_jobs = &job_array[0];
    next_jobs = &job_array[1];

    num_of_rows = 0;
    num_of_cols = 0;
}

void Game::run() {

	_init_game(); // Starts the threads and all other variables you need
	print_board("Initial Board");
	for (uint i = 0; i < m_gen_num; ++i) {
		auto gen_start = std::chrono::system_clock::now();
		_step(i); // Iterates a single generation 
		auto gen_end = std::chrono::system_clock::now();
		m_gen_hist.push_back((double)std::chrono::duration_cast<std::chrono::microseconds>(gen_end - gen_start).count());
		print_board(nullptr);
	} // generation loop
	print_board("Final Board");
	_destroy_game();
}

void Game::_init_game() {

	// Create game fields - Consider using utils:read_file, utils::split
	// Calculate effective threads number
	// Create & Start threads
	// Testing of your implementation will presume all threads are started here

	//converting input matrix to bool_mat
    *curr = utils::read_input(input_filename);
    num_of_rows = curr->size();
    num_of_cols = (curr->front()).size(); // assuming there is at least 1 row
    // create empty field in the next field
    vector<bool> temp_line;
    temp_line.assign(num_of_cols, false); //create a row. all value start as false
    next->assign(num_of_rows, temp_line); // create all the rows

    //calculating effective threads num
    m_thread_num = (curr->size() < m_thread_num) ? curr->size() : m_thread_num;

    //initialize threads
    //TODO check if start was a success
    //creates all consumer thread and starts them (they will be blocked until we add jobs to queue
    for(uint i = 0; i < m_thread_num ; i++){
        auto c = new Consumer(i, &pcQueue);
        c->start();
        m_threadpool.push_back(c);
    }

    //creates jobs, only need to calculate once
    make_jobs(false, curr, next);

}

//TODO check if we can implement this better (maybe seperate the end flag and the vector?)
void Game::make_jobs(bool end_flag, bool_mat* curr, bool_mat* next) {
    for(uint i = 0 ; i < m_thread_num ; i++){
        uint block_size = num_of_rows / m_thread_num;
        uint start_row = block_size * i ;
        uint end_row = start_row + block_size - 1;
        //giving last thread all the rows that remains
        if( i == m_thread_num - 1){
            end_row = num_of_rows - 1;
        }
        //job is define in job.h
        job j1 = {curr, next, start_row, end_row, &m_tile_hist, end_flag, &threads_left, &lock1, &lock2, &cond};
        job j2 = {next, curr, start_row, end_row, &m_tile_hist, end_flag, &threads_left, &lock1, &lock2, &cond};
        job_array[0].push_back(j1);
        job_array[1].push_back(j2);
    }
}

void Game::_step(uint curr_gen) {
	// Push jobs to queue
	// Wait for the workers to finish calculating 
	// Swap pointers between current and next field 
	// NOTE: Threads must not be started here - doing so will lead to a heavy penalty in your grade

    threads_left = m_thread_num; //reset threads left each iteration
    pcQueue.pushAll(*curr_jobs); //add curr_job to the queue

    //wait until all threads finish
    pthread_mutex_lock(&lock1);
    while(threads_left > 0){
        pthread_cond_wait(&cond, &lock1);
    }
    pthread_mutex_unlock(&lock1);

    // Boards pointer swap
    std::swap(curr, next);

    // Jobs pointer swap
    std::swap(curr_jobs, next_jobs);
}

void Game::_destroy_game(){
	// Destroys board and frees all threads and resources 
	// Not implemented in the Game's destructor for testing purposes. 
	// All threads must be joined here
	//TODO make this better looking
	make_jobs(true, curr, next);
	pcQueue.pushAll(*curr_jobs);
	for (uint i = 0; i < m_thread_num; ++i) {
        m_threadpool[i]->join();
        delete m_threadpool[i];
    }
    pthread_mutex_destroy(&lock1);
    pthread_mutex_destroy(&lock2);
    pthread_cond_destroy(&cond);
}

uint Game::thread_num() const {
    return m_thread_num;
}

const vector<tile_record> Game::tile_hist() const {
    return m_tile_hist;
}

const vector<double> Game::gen_hist() const {
    return m_gen_hist;
}

Game::~Game() {
}
/*--------------------------------------------------------------------------------
								
--------------------------------------------------------------------------------*/
inline void Game::print_board(const char* header) {

	if(print_on){
		// Clear the screen, to create a running animation 
		if(interactive_on)
			system("clear");

		// Print small header if needed
		if (header != nullptr)
			cout << "<------------" << header << "------------>" << endl;
        cout << u8"╔" << string(u8"═") * num_of_cols << u8"╗" << endl;
        for (uint i = 0; i < num_of_rows; ++i) {
            cout << u8"║";
            for (uint j = 0; j < num_of_cols; ++j) {
                cout << ((*curr)[i][j] ? u8"█" : u8"░");
            }
            cout << u8"║" << endl;
        }
        cout << u8"╚" << string(u8"═") * num_of_cols << u8"╝" << endl;

		// Display for GEN_SLEEP_USEC micro-seconds on screen 
		if(interactive_on)
			usleep(GEN_SLEEP_USEC);
	}

}


/* Function sketch to use for printing the board. You will need to decide its placement and how exactly 
	to bring in the field's parameters. 

		cout << u8"╔" << string(u8"═") * field_width << u8"╗" << endl;
		for (uint i = 0; i < field_height ++i) {
			cout << u8"║";
			for (uint j = 0; j < field_width; ++j) {
				cout << (field[i][j] ? u8"█" : u8"░");
			}
			cout << u8"║" << endl;
		}
		cout << u8"╚" << string(u8"═") * field_width << u8"╝" << endl;
*/ 



