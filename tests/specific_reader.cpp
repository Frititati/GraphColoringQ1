#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>

using namespace std;

vector<string> split (string s, string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> res;

    while ((pos_end = s.find (delimiter, pos_start)) != string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}

int main() {

	fstream graph_file;
	graph_file.open("graph.txt", ios::in);
	
	string line;
	int counter = 1;
	int number_node;
	int number_edge;

	if(graph_file.is_open()) {

		// this is first line
		getline(graph_file, line);
		vector<string> parse_first_line = split(line, " ");

		number_node = stoi(parse_first_line[0]);
		number_edge = stoi(parse_first_line[1]);

    	// for (auto i : temp) cout << i << endl;

		while(getline(graph_file, line)) {
			// parsing node line

			vector<string> temp = split(line, " ");

			cout << "node number :" << to_string(counter) << endl;

			for (auto i : temp) cout << "conn " << i << endl;

			counter++;
		}
	}

	graph_file.close();
	
	return 0;

}