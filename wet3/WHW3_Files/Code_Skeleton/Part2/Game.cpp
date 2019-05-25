#include "Game.hpp"
#include "utils.hpp"
/*--------------------------------------------------------------------------------
								
--------------------------------------------------------------------------------*/
/*
 * constructor - save the parameters from the input and initiate the field parameters
 */
Game::Game(game_params params) {
    m_gen_num = params.n_gen;
    m_thread_num = params.n_thread;
    input_filename = params.filename;
    interactive_on = params.interactive_on;
    print_on = params.print_on;

    // not sure we need to allocate space for these
    //m_tile_hist = new vector<tile_record>();
    //m_gen_hist = new vector<double>();
    //m_threadpool = new vector<Thread*>();
    //curr_field = new bool_mat();
    //next_field = new bool_mat();

    num_of_rows = 0;
    num_of_cols = 0;
}

Game::~Game() {}

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
	// TODO: after adding the threads - add the destroy game
	//_destroy_game();
}

void Game::_init_game() {

	// Create game fields - Consider using utils:read_file, utils::split
	// Create & Start threads
	// Testing of your implementation will presume all threads are started here

    vector<string> input = utils::read_lines(this->input_filename);
    vector<string> line;
    char delimiter = ' ';
    vector<bool> temp_line;
    for (uint i = 0; i < input.size(); i++) {
        line = utils::split(input[i], delimiter);
        for (uint j = 0; j < line.size(); j++) {
            temp_line.push_back(line[j] == "1"); // save as bool
        }
        this->curr_field.push_back(temp_line);
        temp_line.clear();
    }
	this->num_of_rows = this->curr_field.size();
	this->num_of_cols = this->curr_field[0].size(); // assuming there is at least 1 row
	string temp_str;
	// create empty field in the next field
	temp_line.assign(this->num_of_cols, false); //create a row. all value start as false
	this->next_field.assign(this->num_of_rows, temp_line); // create all the rows
	// TODO: separate the work between different threads, and create them
}

void Game::_step(uint curr_gen) {
	// Push jobs to queue
	// Wait for the workers to finish calculating 
	// Swap pointers between current and next field 
	// NOTE: Threads must not be started here - doing so will lead to a heavy penalty in your grade

	int neighbors;
	// for now - do all the job here TODO: give the job to the workers
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
    // swap fields
    std::swap(this->curr_field, this->next_field);
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

void Game::_destroy_game(){
	// Destroys board and frees all threads and resources 
	// Not implemented in the Game's destructor for testing purposes. 
	// All threads must be joined here
	for (uint i = 0; i < m_thread_num; ++i) {
        m_threadpool[i]->join();
    }
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
		
		// TODO: Print the board
        cout << u8"╔" << string(u8"═") * num_of_cols << u8"╗" << endl;
        for (uint i = 0; i < num_of_rows; ++i) {
            cout << u8"║";
            for (uint j = 0; j < num_of_cols; ++j) {
                cout << (curr_field[i][j] ? u8"█" : u8"░");
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



