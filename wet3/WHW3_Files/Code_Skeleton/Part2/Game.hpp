#ifndef __GAMERUN_H
#define __GAMERUN_H

#include "../Part1/PCQueue.hpp"
#include "Consumer.h"

/*--------------------------------------------------------------------------------
								  Auxiliary Structures
--------------------------------------------------------------------------------*/
struct game_params {
	// All here are derived from ARGV, the program's input parameters. 
	uint n_gen;
	uint n_thread;
	string filename;
	bool interactive_on; 
	bool print_on; 
};

struct tile_record {
	double tile_compute_time; // Compute time for the tile
	uint thread_id; // The thread responsible for the compute 
};

/*--------------------------------------------------------------------------------
									Class Declaration
--------------------------------------------------------------------------------*/

class Game {
public:
	Game(game_params);
	~Game();
	void run(); // Runs the game
	const vector<double> gen_hist() const; // Returns the generation timing histogram  
	const vector<tile_record> tile_hist() const; // Returns the tile timing histogram
	uint thread_num() const; //Returns the effective number of running threads = min(thread_num, field_height)


protected: // All members here are protected, instead of private for testing purposes

	// See Game.cpp for details on these three functions
	void _init_game(); 
	void _step(uint curr_gen); 
	void _destroy_game(); 
	inline void print_board(const char* header);
	//create jobs vector to add to the queue
	//it creates 2 vectors, one for each iteration and we switch them each iteration
    void make_jobs(bool end_flag, bool_mat* curr, bool_mat* next);

	uint m_gen_num; 			 		// The number of generations to run
	uint m_thread_num; 			 		// Effective number of threads = min(thread_num, field_height)
	vector<tile_record> m_tile_hist; 	// Shared Timing history for tiles: First m_thread_num cells are the calculation durations for tiles in generation 1 and so on. 
							   	 		// Note: In your implementation, all m_thread_num threads must write to this structure. 
	vector<double> m_gen_hist;  	 	// Timing history for generations: x=m_gen_hist[t] iff generation t was calculated in x microseconds
	vector<Thread*> m_threadpool; 		// A storage container for your threads. This acts as the threadpool. 

	bool interactive_on; // Controls interactive mode - that means, prints the board as an animation instead of a simple dump to STDOUT 
	bool print_on; // Allows the printing of the board. Turn this off when you are checking performance (Dry 3, last question)

	// TODO: Add in your variables and synchronization primitives  
    string filename;
    vector<bool_mat> mats; //curr and next matrix
    bool_mat* curr;
    bool_mat* next;
    int threads_left; //counts how many threads still working and begin another iteration only after all threads finished
    PCQueue<job> pcQueue;
    vector<vector<job>> job_array; //array how jobs vector
    vector<job>* curr_jobs;
    vector<job>* next_jobs;
    pthread_mutex_t lock;
    pthread_cond_t cond;
};
#endif
