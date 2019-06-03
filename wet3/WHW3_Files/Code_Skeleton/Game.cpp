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
    pthread_cond_init(&cond, NULL);
    pthread_mutexattr_destroy(&attr);
    //matrix and job vector pointers
    curr = &mats[0];
    next = &mats[1];
    curr_jobs = &job_array[0];
    next_jobs = &job_array[1];
    //init matrix size
    num_of_rows = 0;
    num_of_cols = 0;
    curr_gen = 0;
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
	sortTileHist();
	_destroy_game();
}

void Game::_init_game() {

	// Create game fields - Consider using utils:read_file, utils::split
	// Calculate effective threads number
	// Create & Start threads
	// Testing of your implementation will presume all threads are started here

	//Converting input matrix to bool_mat (will add frame of zeroes to input mat)
    *curr = utils::read_input(input_filename);
    num_of_rows = curr->size() - 2; //real num of rows and cols is without the frame
    num_of_cols = (curr->front()).size() - 2; // assuming there is at least 1 row
    // create empty field in the next field
    vector<bool> temp_line;
    //init the next mat to be all zeroes and size of curr mat
    temp_line.assign(num_of_cols + 2, false); //create a row. all value start as false
    next->assign(num_of_rows + 2, temp_line); // create all the rows

    //Calculating effective threads num
    m_thread_num = (num_of_rows < m_thread_num) ? num_of_rows : m_thread_num;

    //initialize threads
    //creates all consumer thread and starts them (they will be blocked until we add jobs to queue
    for(uint i = 0; i < m_thread_num ; i++){
        auto c = new Consumer(i, &pcQueue);
        c->start();
        m_threadpool.push_back(c);
    }

    //creates jobs, only need to calculate once
    make_jobs(false, curr, next);

    gt = std::chrono::system_clock::now();
    m_tile_hist.resize(m_thread_num * m_gen_num); //allocating tile hist size in advance

}

void Game::make_jobs(bool end_flag, bool_mat* curr_board, bool_mat* next_board) {
    for(uint i = 0 ; i < m_thread_num ; i++){ //creates m_thread_num jobs
        uint block_size = num_of_rows / m_thread_num;
        uint start_row = block_size * i ;
        uint end_row = start_row + block_size - 1;
        //giving last thread all the rows that remains
        if( i == m_thread_num - 1){
            end_row = num_of_rows - 1;
        }
        //job is define in job.h
        //creates 2 jobs array, one for each iteration according to curr and next boards
        job j1 = {i, curr_board, next_board, start_row, end_row, &m_tile_hist, end_flag,
                        &threads_left, &lock1,  &cond, &gt, &curr_gen, m_thread_num};
        job j2 = {i, next_board, curr_board, start_row, end_row, &m_tile_hist, end_flag,
                        &threads_left, &lock1,  &cond, &gt, &curr_gen, m_thread_num};
        job_array[0].push_back(j1);
        job_array[1].push_back(j2);
    }
}

void Game::_step(uint curr_gen) {
	// Push jobs to queue
	// Wait for the workers to finish calculating 
	// Swap pointers between current and next field 
	// NOTE: Threads must not be started here - doing so will lead to a heavy penalty in your grade
    this->curr_gen = curr_gen;
    threads_left = m_thread_num; //reset threads left each iteration
    pcQueue.pushAll(*curr_jobs); //add curr_job to the queue
    //all consumers will start to work here

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
	make_jobs(true, curr, next); //we create "destructive" jobs
	//this job will make the consumers stop
	pcQueue.pushAll(*curr_jobs);
	for (uint i = 0; i < m_thread_num; ++i) {
        m_threadpool[i]->join();
        delete m_threadpool[i];
    }
    pthread_mutex_destroy(&lock1);
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

void Game::sortTileHist() {
    std::sort(m_tile_hist.begin(), m_tile_hist.end(), compareRecords);
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
        for (uint i = 1; i < num_of_rows + 1; ++i) { // real mat is from row 1 and col 1
            cout << u8"║";
            for (uint j = 1; j < num_of_cols + 1; ++j) {
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



