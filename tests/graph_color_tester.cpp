#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <map>
#include <time.h>
#include <chrono>
#include <algorithm>

using namespace std;

map<int, vector<int>> node_edge_connections;
vector<int> node_color;

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
		try {
			res.push_back (stoi(token));
		} catch (exception e){
			cout << "we had an OPSY" << endl;
		}
	}

	try {
		res.push_back (stoi(s.substr (pos_start)));
	} catch (exception e){
		cout << "we had an OPSY" << endl;
	}
	return res;
}

int test_graph() {

	int counter_wrong = 0;

	for(map<int, vector<int> >::const_iterator it = node_edge_connections.begin();
			it != node_edge_connections.end(); ++it)
	{
		int this_color = node_color[it->first - 1];

		if (this_color == 0)
		{
			cout << "We have a mistake at node: " << to_string(it->first) << " color is 0" << endl;
			counter_wrong++;
		}

		vector<int> connections = it->second;

		for (auto i : connections)
		{
			if (node_color[i - 1] == this_color)
			{
				cout << "We have a mistake at node: " << to_string(it->first) << " with " << to_string(i) << " (the color " << to_string(this_color) << ")" << endl;
				counter_wrong++;
			}
		}
	}
	cout << "Total Mistakes are: " << to_string(counter_wrong) << endl;

}

int test_consistency() {

	int counter_wrong = 0;

	for(map<int, vector<int> >::const_iterator it = node_edge_connections.begin();
			it != node_edge_connections.end(); ++it)
	{
		vector<int> connections = it->second;

		for (auto i : connections)
		{
			vector<int> temp_connection = node_edge_connections[i];

			if (std::find(temp_connection.begin(), temp_connection.end(), it->first) != temp_connection.end())
			{
				// nothing here
				// cout << "We found something : " << to_string(i) << " " << to_string(it->first) << endl; 
			} else {
				cout << "We have a mistake at node: " << to_string(it->first) << " with " << to_string(i) << endl;
				counter_wrong++;
			}
		}
	}
	cout << "Total Mistakes are: " << to_string(counter_wrong) << endl;

}

int main(int argc, char** argv) {

	if (argc < 3)
	{
		// not enought parameters
		cout << "Not enough parameters passed, please refer to this guide:" << endl;
		cout << "<execution file> <graph file> <color file>" << endl;
		return 1;
	}

	// TODO : we have to check if this is exeception prone

	fstream graph_file;
	graph_file.open(argv[1], ios::in);

	if (!graph_file.is_open())
	{
		// file does not exists
		cout << "File cannot be found, please refer to this guide:" << endl;
		cout << "<execution file> <graph file> <color file>" << endl;
		return 1;
	}

	fstream color_file;
	color_file.open(argv[2], ios::in);

	if (!color_file.is_open())
	{
		// file does not exists
		cout << "File cannot be found, please refer to this guide:" << endl;
		cout << "<execution file> <graph file> <color file>" << endl;
		return 1;
	}
	
	string line;
	int counter = 1;
	int number_node;
	int number_edge;
	int number_node_color;
	int number_edge_color;

	// we used srand to set seed for randomization of node numbers
	// srand(time(NULL));

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

			node_edge_connections.insert(std::pair<int, vector<int>>(counter, temp));

			counter++;
		}
	}

	while (node_edge_connections.size() != number_node)
	{
		node_edge_connections.insert(std::pair<int, vector<int>>(counter, {}));
		counter++;
	}

	if(color_file.is_open()) {

		// this is first line
		getline(color_file, line);
		vector<int> parse_first_line = split_to_int(line, " ");

		number_node_color = parse_first_line[0];
		number_edge_color = parse_first_line[1];

		// for (auto i : temp) cout << i << endl;

		while(getline(color_file, line)) {
			// parsing node line
			node_color.push_back(stoi(line));
		}
	}

	color_file.close();

	if (number_node < number_node_color)
	{
		cout << "Are you comparing the same graph?" << endl;
		return 1;
	}

	if (number_edge != number_edge_color)
	{
		cout << "Are you comparing the same graph?" << endl;
		return 1;
	}

	auto start = chrono::steady_clock::now();

	// unsync the I/O of C and C++.
	ios_base::sync_with_stdio(false);

	test_graph();

	// Recording end time.
	auto end = chrono::steady_clock::now();

	// Calculating total time taken by the program.
	cout << "Elapsed time in nanoseconds: "
		<< chrono::duration_cast<chrono::nanoseconds>(end - start).count()
		<< " ns" << endl;

	cout << "Elapsed time in microseconds: "
		<< chrono::duration_cast<chrono::microseconds>(end - start).count()
		<< " µs" << endl;

	return 0;

}