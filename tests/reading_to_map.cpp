#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <map>
#include <time.h>

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

vector<int> split_to_int (string s, string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<int> res;

    while ((pos_end = s.find (delimiter, pos_start)) != string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (stoi(token));
    }

    res.push_back (stoi(s.substr (pos_start)));
    return res;
}

int main() {

	fstream graph_file;
	graph_file.open("graph.txt", ios::in);
	
	string line;
	int counter = 1;
	int number_node;
	int number_edge;

	map<int, vector<int>> node_edge_connections;
	vector<int> node_random;
	vector<int> node_color;
	
	srand(time(NULL));

	if(graph_file.is_open()) {

		// this is first line
		getline(graph_file, line);
		vector<int> parse_first_line = split_to_int(line, " ");

		number_node = parse_first_line[0];
		number_edge = parse_first_line[1];

    	// for (auto i : temp) cout << i << endl;

		while(getline(graph_file, line)) {
			// parsing node line

			vector<int> temp = split_to_int(line, " ");

			// cout << "node number :" << to_string(counter) << endl;
			// for (auto i : temp) cout << "conn " << to_string(i) << endl;

			node_edge_connections.insert(std::pair<int, vector<int>>(counter, temp));

			// add random number to each node
			node_random.push_back(rand() % 100 + 1);
			// add color 0
			node_color.push_back(0);

			counter++;
		}
	}

	graph_file.close();

	cout << "$$$$$$$$$$$$$$$$$$$$" << endl;

	int counter_2 = 0;

	for(map<int, vector<int> >::const_iterator it = node_edge_connections.begin();
	    it != node_edge_connections.end(); ++it)
	{
	    cout << "node :" << it->first << endl;
	    cout << "random :" << node_random[counter_2] << endl;
	    for (auto i : it->second) cout << "conn " << to_string(i) << endl;
	    counter_2++;
	}

	
	return 0;

}