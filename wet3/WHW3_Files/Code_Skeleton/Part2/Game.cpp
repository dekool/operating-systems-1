#include "Game.hpp"
#include "utils.hpp"

/*--------------------------------------------------------------------------------
								
--------------------------------------------------------------------------------*/
Game::Game(game_params gameParams) : mats(2), job_array(2){
    //initiailize all the fields needed in Game
    m_gen_num = gameParams.n_gen;
    m_thread_num = gameParams.n_thread;
    filename = gameParams.filename;
    interactive_on = gameParams.interactive_on;
    print_on = gameParams.print_on;
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_init(&lock, &attr);
    pthread_cond_init(&cond, NULL);
    pthread_mutexattr_destroy(&attr);
    //TODO destroy lock and cond in destroy
    //matrix and job vector pointers
    curr = &mats[0];
    next = &mats[1];
    curr_jobs = &job_array[0];
    next_jobs = &job_array[1];
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
	//TODO check if there is a better way to implement this part
    vector<string> input_mat = utils::read_lines(filename);
    vector<vector<string>> matrix;
    for(int i = 0 ; i< input_mat.size(); i++){
        matrix.push_back(utils::split(input_mat[i], DEF_MAT_DELIMITER));
    }
    *curr = utils::stringToMat(matrix);
    *next = *curr; //this is only to make next the same dimentions of curr
    //caluculating effective threads num
    m_thread_num = (curr->size() < m_thread_num) ? curr->size() : m_thread_num;

    //initialize threads
    //TODO check if start was a success
    //creates all consumer thread and starts them (they will be blocked until we add jobs to queue
    for(int i = 0; i < m_thread_num ; i++){
        auto c = new Consumer(i, &pcQueue);
        c->start();
        m_threadpool.push_back(c);
    }

    //creates jobs, only need to calculate once
    make_jobs(false, curr, next);

}

//TODO check if we can implement this better (maybe seperate the end flag and the vector?)
void Game::make_jobs(bool end_flag, bool_mat* curr, bool_mat* next) {
    for(int i = 0 ; i < m_thread_num ; i++){
        int row_num = curr->size();
        int block_size = row_num / m_thread_num;
        int start_row = block_size * i ;
        int end_row = start_row + block_size - 1;
        //givinig last thread all the rows that remains
        if( i == m_thread_num - 1){
            end_row = row_num - 1;
        }
        //job is define in job.h
        job j1 = {curr, next, start_row, end_row, &m_gen_hist, end_flag, &threads_left, &lock, &cond};
        job j2 = {next, curr, start_row, end_row, &m_gen_hist, end_flag, &threads_left, &lock, &cond};
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
    pthread_mutex_lock(&lock);
    while(threads_left > 0){
        pthread_cond_wait(&cond, &lock);
    }
    pthread_mutex_unlock(&lock);

    // Boards pointer swap
    bool_mat* temp = curr;
    curr = next;
    next = temp;

    // Jobs pointer swap
    vector<job>* t = curr_jobs;
    curr_jobs = next_jobs;
    next_jobs = t;
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

}

uint Game::thread_num() const {
    return m_thread_num;
}

//TODO implement this.(this is only for testing the game)
const vector<tile_record> Game::tile_hist() const {
    vector<tile_record> a;
    return a;
}

//TODO implement this.(this is only for testing the game)
const vector<double> Game::gen_hist() const {
    vector<double> a;
    return a;
}

Game::~Game() {
    //I`m not sure what we suppose to destroy here
}
/*--------------------------------------------------------------------------------
								
--------------------------------------------------------------------------------*/
inline void Game::print_board(const char* header) {

	if(print_on){ 
        //TODO check this prints is okay, tested in clion
		// Clear the screen, to create a running animation 
		if(interactive_on)
			system("clear");

		// Print small header if needed
		if (header != nullptr)
			cout << "<------------" << header << "------------>" << endl;
        int field_height = curr->size();
        int field_width = curr->front().size();
        bool_mat field = *curr;
        cout << u8"╔" << string(u8"═") * field_width << u8"╗" << endl;
        for (uint i = 0; i < field_height; ++i) {
            cout << u8"║";
            for (uint j = 0; j < field_width; ++j) {
                cout << (field[i][j] ? u8"█" : u8"░");
            }
            cout << u8"║" << endl;
        }
        cout << u8"╚" << string(u8"═") * field_width << u8"╝" << endl;

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



