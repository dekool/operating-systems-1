#include "utils.hpp"

/*--------------------------------------------------------------------------------
									Misc Utils Implementation
--------------------------------------------------------------------------------*/
vector<string> utils::read_lines(const string& filename) {

	ifstream file(filename);
	user_error(string("Invalid file: ") + filename, file.good());
	vector<string> lines;
	string curr;

	while (getline(file, curr))
	{
		if (curr.size() > 0)
			lines.push_back(curr);
	}
	file.close();
	return lines;
}

vector<string> utils::split(const string& s, char delimiter)
{
	vector<string> tokens;
	string token;
	istringstream token_stream(s);
	while (getline(token_stream, token, delimiter))
	{
		tokens.push_back(token);
	}
	return tokens;
}

bool_mat utils::read_input(string input_filename) {
    bool_mat field;
    vector<string> input = utils::read_lines(input_filename);
    vector<string> line;
    vector<bool> temp_line;
    vector<bool> zero_line;
    //adding all rows + frames of zeroes to help calculate later
    for (uint i = 0; i < input.size(); i++) {
        line = utils::split(input[i], DEF_MAT_DELIMITER);
        if( i == 0 ){
            zero_line = vector<bool>(line.size() + 2, false);
            field.push_back(zero_line); //adding first line of zeroes
        }
        temp_line.push_back(false); //adding frame of zeroes
        //adding each cell in the row
        for (uint j = 0; j < line.size(); j++) {
            temp_line.push_back(line[j] == "1"); // save as bool
        }
        temp_line.push_back(false); //adding frame of zeroes
        field.push_back(temp_line);//adding last line of zeroes
        temp_line.clear();
    }
    field.push_back(zero_line);
    return field;
}

bool compareRecords(tile_record t1, tile_record t2){
    return (t1.end_time < t2.end_time);
}
/*--------------------------------------------------------------------------------
								String Extentions
--------------------------------------------------------------------------------*/

string repeat(string str, const size_t n)
{
	if (n == 0) {
		str.clear();
		str.shrink_to_fit();
		return str;
	}
	else if (n == 1 || str.empty()) {
		return str;
	}
	const auto period = str.size();
	if (period == 1) {
		str.append(n - 1, str.front());
		return str;
	}
	str.reserve(period * n);
	size_t m{ 2 };
	for (; m < n; m *= 2) str += str;
	str.append(str.c_str(), (n - (m / 2)) * period);
	return str;
}

string operator*(string str, size_t n)
{
	return repeat(move(str), n);
}

