#ifndef __UTILS_H
#define __UTILS_H
#include "Headers.hpp"

namespace utils {
	// Read all non-empty lines from file as strings with no carriage returns, and returns them in a std::vector 
	vector<string> read_lines(const string& filename); 
	// Returns a string array that contains the substrings in the string s that are delimited by the char delimiter
	vector<string> split(const string& s, char delimiter); //Splits a string
    // Returns the matrix we will work on according to the input matrix
    bool_mat read_input(string input_filename);
}

string repeat(string str, const size_t n);
string operator*(string str, size_t n);

struct tile_record {
    double tile_compute_time; // Compute time for the tile
    uint thread_id; // The thread responsible for the compute
    double start_time;
    double end_time;
};

//Comparison function to use std::sort on the tile hist
bool compareRecords(tile_record t1, tile_record t2);



#endif
