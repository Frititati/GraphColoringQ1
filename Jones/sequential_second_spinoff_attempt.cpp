#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <map>
#include <time.h>
#include <chrono>

using namespace std;

// the datastructure where we keep the node index and all of it's connections
map<int, vector<int>> node_edge_connections;
// the datastructure where we keep the node random based on index
vector<int> node_random;
// the datastructure where we keep the node color based on index
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
			// we don't have any node connections
			// cout << "There are no connections" << endl;
		}
	}

	try {
		res.push_back (stoi(s.substr (pos_start)));
	} catch (exception e){
		// we don't have any node connections
		// cout << "There are no connections" << endl;
	}
	return res;
}

int jones_sequential() {
	// color is defined as integer
	int color = 1;

	while(node_edge_connections.size() != 0)
	{
		// vector of all the nodes to be evaluated this iteration
		vector<int> to_be_evaluated;

		// cout << "size node " << to_string(node_edge_connections.size()) << " color " << color << endl;

		// generate array of all colors that are available
		map<int, int> colors_used;
		for (int c = 1; c <= color; ++c)
		{
			colors_used.insert(std::pair<int, int>(c, c));
		}

		for(map<int, vector<int> >::const_iterator it = node_edge_connections.begin();
			it != node_edge_connections.end(); ++it)
		{
			// explore connected nodes
			vector<int> connections = it->second;

			bool is_highest = true;

			vector<int> temp_color_remove;

			int node_random_this = node_random[it->first - 1];
			int node_key_this = it->first;
			
			for (auto i : connections)
			{
				// cout << "node " << it->first << " look at " << to_string(i) << endl;
				// check if connections is colored
				if (node_color[i - 1] == 0)
				{
					if (node_random_this < node_random[i - 1] || ((node_random_this == node_random[i - 1]) && (node_key_this < i)))
					{
						is_highest = false;
						break;
					}
				} else {
					temp_color_remove.push_back(node_color[i - 1]);
				}
			}


			if (is_highest)
			{
				// cout << "node " << node_key_this << " highest" << endl;
				to_be_evaluated.push_back(node_key_this);
				
				for (auto j : temp_color_remove)
				{
					colors_used.erase(j);
				}

			}
		}

		bool increased_color = false;

		// we have to remove
		for (auto i : to_be_evaluated)
		{
			if (colors_used.size() > 1)
			{
				auto it = colors_used.begin();
				node_color[i - 1] = it->second;
			} else {
				increased_color = true;
				node_color[i - 1] = color;
			}
			node_edge_connections.erase(i);
		}

		if (increased_color)
		{
			color++;
		}
	}

	cout << "Colors used: " << to_string(color) << endl;
	return 0;
}

int main(int argc, char** argv) {

	if (argc < 2)
	{
		// not enought parameters
		cout << "Not enough parameters passed, please refer to this guide:" << endl;
		cout << "<execution file> <graph file>" << endl;
		return 1;
	}

	// TODO : we have to check if this is exeception prone

	fstream graph_file;
	graph_file.open(argv[1], ios::in);

	if (!graph_file.is_open())
	{
		// file does not exists
		cout << "File cannot be found, please refer to this guide:" << endl;
		cout << "<execution file> <graph file>" << endl;
		return 1;
	}
	
	string line;
	int new_node_index = 1;
	int number_nodes;
	int number_edges;

	// we used srand to set seed for randomization of node numbers
	// srand(time(NULL));

	if(graph_file.is_open()) {

		// this is first line
		getline(graph_file, line);
		vector<int> parse_first_line = split_to_int(line, " ");

		number_nodes = parse_first_line[0];
		number_edges = parse_first_line[1];

		// for (auto i : temp) cout << i << endl;

		while(getline(graph_file, line)) {
			// parsing node line

			vector<int> temp = split_to_int(line, " ");

			// cout << "node number :" << to_string(new_node_index) << endl;
			// for (auto i : temp) cout << "conn " << to_string(i) << endl;

			node_edge_connections.insert(std::pair<int, vector<int>>(new_node_index, temp));

			// add random number to each node
			node_random.push_back(rand() % 100 + 1);
			// add color 0
			node_color.push_back(0);

			new_node_index++;
		}
	}

	while (node_edge_connections.size() < number_nodes)
	{
		node_edge_connections.insert(std::pair<int, vector<int>>(new_node_index, {}));
		node_random.push_back(rand() % 100 + 1);
		node_color.push_back(0);
		new_node_index++;
	}

	graph_file.close();

	// start a timer
	auto start = chrono::steady_clock::now();
	ios_base::sync_with_stdio(false);

	jones_sequential();
	
	// end timer
	auto end = chrono::steady_clock::now();

	cout << "Elapsed time in nanoseconds: "
		<< chrono::duration_cast<chrono::nanoseconds>(end - start).count()
		<< " ns" << endl;
	cout << "Elapsed time in microseconds: "
		<< chrono::duration_cast<chrono::microseconds>(end - start).count()
		<< " µs" << endl;

	fstream write_file;
	write_file.open(argv[2], ios::out);
	if (write_file.is_open())
	{
		// file exists write to file results
		write_file << to_string(number_nodes) << " " << to_string(number_edges) << endl;
		for (auto i : node_color)
		{
			write_file << to_string(i) << endl;
		}
	}
	write_file.close();

	return 0;
}