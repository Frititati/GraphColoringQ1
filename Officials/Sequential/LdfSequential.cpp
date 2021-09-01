#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <map>
#include <time.h>
#include <algorithm>
#include <set>
#include <iterator>
#include <chrono>

using namespace std;

// the datastructure where we keep the node index and all of it's connections
map < int, vector < int > > node_edge_connections;
// the datastructure where we keep the node random based on index
vector < int > node_random;
// the datastructure where we keep the node color based on index
vector < int > node_color;
// the datastructure where we keep the degree of each node
vector < int > node_degree;

bool directed = 0;

// path to the graph
string graph_path;

vector < string > split(string s, string delimiter) {
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	string token;
	vector < string > res;

	while ((pos_end = s.find(delimiter, pos_start)) != string::npos) {
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		res.push_back(token);
	}

	res.push_back(s.substr(pos_start));
	return res;
}

vector < int > split_to_int(string s, string delimiter) {
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	string token;
	vector < int > res;

	while ((pos_end = s.find(delimiter, pos_start)) != string::npos) {
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		try {
			res.push_back(stoi(token));
		} catch (exception e) {
			// we don't have any node connections
			// cout << "There are no connections" << endl;
		}
	}

	try {
		res.push_back(stoi(s.substr(pos_start)));
	} catch (exception e) {
		// we don't have any node connections
		// cout << "There are no connections" << endl;
	}
	return res;
}

vector < int > split_to_int_mod(string line, string delimiter) {
	string s;
	int start_point = line.find(':') + 2;
	int end_point = line.find("#");
	int line_size = end_point - 1 - start_point;
	if (end_point != start_point) {
		s = line.substr(start_point, line_size);
	} else {
		s = "";
	}

	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	string token;
	vector < int > res;
	while ((pos_end = s.find(delimiter, pos_start)) != string::npos) {
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		try {
			res.push_back(stoi(token) + 1);
		} catch (exception e) {
			// we don't have any node connections
			// cout << "There are no connections" << endl;
		}
	}

	try {
		res.push_back(stoi(s.substr(pos_start)) + 1);
	} catch (exception e) {
		// we don't have any node connections
		// cout << "There are no connections" << endl;
	}
	return res;
}

int ldf_sequential() {
	// color is defined as integer
	int color = 1;
	// we have a set of the colors used (this is useful to compare later in the iterative step)
	set < int > colors_used_global = { 1 };

	bool first_iter = true;

	while (node_edge_connections.size() != 0) {

		// map of all the nodes to be evaluated this iteration <index, color>
		map < int, int > to_be_evaluated;

		// iterate on the map
		for (map < int, vector < int > > ::const_iterator it = node_edge_connections.begin(); it != node_edge_connections.end(); ++it) {

			// vector of all the connected nodes
			vector < int > connections = it -> second;

			bool is_highest = true;

			set < int > colors_used_local;

			int node_random_this = node_random[it -> first - 1];
			int node_degree_this = node_degree[it -> first - 1];
			int node_key_this = it -> first;

			// look into all the connected nodes
			for (auto i: connections) {
				// check if connections is not colored
				if (node_color[i - 1] == 0) {
					if (node_degree_this < node_degree[i - 1] ||
						(node_degree_this == node_degree[i - 1] && node_random_this < node_random[i - 1]) ||
						(node_degree_this == node_degree[i - 1] && node_random_this == node_random[i - 1] && node_key_this < i)) {
						// not local highest
						is_highest = false;
						break;
					}
				} else {
					colors_used_local.insert(node_color[i - 1]);
				}
			}

			// only execute if the node is the highest in its local section
			if (is_highest) {
				std::set < int > colors_used_iteration;
				std::set_difference(colors_used_global.begin(), colors_used_global.end(), colors_used_local.begin(), colors_used_local.end(), std::inserter(colors_used_iteration, colors_used_iteration.end()));
				auto choosen = colors_used_iteration.begin();
				to_be_evaluated.insert(std::pair < int, int > (node_key_this, * choosen));
			}
		}

		for (map < int, int > ::const_iterator node_interator = to_be_evaluated.begin(); node_interator != to_be_evaluated.end(); ++node_interator) {
			node_color[node_interator -> first - 1] = node_interator -> second;
			node_edge_connections.erase(node_interator -> first);

			if (node_interator -> second == color) {
				color++;
				colors_used_global.insert(color);
			}
		}
	}

	return 0;
}

int main(int argc, char ** argv) {

	if (argc < 2) {
		// not enought parameters
		cout << "Not enough parameters passed, please refer to this guide:" << endl;
		cout << "<execution file> <graph file>" << endl;
		return 1;
	}

	fstream graph_file;
	graph_path = argv[1];
	graph_file.open(graph_path, ios:: in );

	if (!graph_file.is_open()) {
		// file does not exists
		cout << "File cannot be found, please refer to this guide:" << endl;
		cout << "<execution file> <graph file>" << endl;
		return 1;
	}

	string line;
	int new_node_index = 1;
	int number_nodes;
	int number_edges = 0;

	// we used srand to set seed for randomization of node numbers
	srand(time(NULL));

	auto start_read = chrono::steady_clock::now();

	if (graph_file.is_open()) {

		// this is first line
		getline(graph_file, line);
		vector < int > parse_first_line = split_to_int(line, " ");

		number_nodes = parse_first_line[0];
		directed = (parse_first_line.size() == 1);
		if (!directed)
			number_edges = parse_first_line[1];

		while (getline(graph_file, line)) {
			// parsing node line
			vector < int > temp;
			if (directed)
				temp = split_to_int_mod(line, " ");
			else
				temp = split_to_int(line, " ");
			// cout << "node number :" << to_string(new_node_index) << endl;
			// for (auto i : temp) cout << "conn " << to_string(i) << endl;

			node_edge_connections.insert(std::pair < int, vector < int > > (new_node_index, temp));

			// add random number to each node
			node_random.push_back(rand() % 100 + 1);
			// add color 0
			node_color.push_back(0);
			// cout << "Degree of node " << new_node_index << " is " << temp.size() << endl;
			if (!directed) {
				node_degree.push_back(temp.size());
			}
			new_node_index++;
		}
	}

	while (node_edge_connections.size() < number_nodes) {
		node_edge_connections.insert(std::pair < int, vector < int > > (new_node_index, {}));
		node_random.push_back(rand() % 100 + 1);
		node_color.push_back(0);
		new_node_index++;
	}

	// handle directed graph
	if (directed) {
		map < int, vector < int > > temp_node_edge = node_edge_connections;

		// translate the graph

		//for each node of the map
		for (map < int, vector < int > > ::const_iterator it = node_edge_connections.begin(); it != node_edge_connections.end(); ++it) {
			//for each element of the selected array, insert the node it->first into the nodes referenced in the array
			for (auto i: it -> second) {
				temp_node_edge[i].push_back(it -> first);
			}
		}
		node_edge_connections = temp_node_edge;

		// compute number_edges and degrees

		for (map < int, vector < int > > ::const_iterator it = node_edge_connections.begin(); it != node_edge_connections.end(); ++it) {
			// cout << "number_edges: " << number_edge
			//edges
			number_edges += it -> second.size();
			//degree
			node_degree.push_back(it -> second.size());
		}
	}

	graph_file.close();

	auto start_algo = chrono::steady_clock::now();
	ios_base::sync_with_stdio(false);

	ldf_sequential();

	auto start_write = chrono::steady_clock::now();

	string final = to_string(number_nodes) + " " + to_string(number_edges) + "\n";

	int max_color = 0;

	for (auto i: node_color) {
		final += to_string(i) + "\n";
		if (i > max_color) {
			max_color = i;
		}
	}

	auto output_file = std::fstream(argv[2], std::ios::out | std::ios::binary);
	output_file.write(final.c_str(), (final.size() * sizeof(char)));
	output_file.close();

	auto end_write = chrono::steady_clock::now();

	cout << "Elapsed reading time in microseconds: " << chrono::duration_cast < chrono::microseconds > (start_algo - start_read).count() << " µs" << endl;
	cout << "Elapsed algo time in microseconds: " << chrono::duration_cast < chrono::microseconds > (start_write - start_algo).count() << " µs" << endl;
	cout << "Elapsed writing time in microseconds: " << chrono::duration_cast < chrono::microseconds > (end_write - start_write).count() << " µs" << endl;
	cout << "Elapsed time in microseconds: " << chrono::duration_cast < chrono::microseconds > (end_write - start_read).count() << " µs" << endl;

	// std::ofstream results_file;
	// results_file.open("ldf_seq_1.csv", std::ios_base::app);
	// results_file << graph_path.substr(graph_path.find_last_of("/\\") + 1) << "," <<
	// 	number_nodes << "," <<
	// 	number_edges << "," <<
	// 	"sequential" << "," <<
	// 	max_color << "," <<
	// 	chrono::duration_cast < chrono::microseconds > (end_write - start_read).count() << "," <<
	// 	chrono::duration_cast < chrono::microseconds > (start_algo - start_read).count() << "," <<
	// 	chrono::duration_cast < chrono::microseconds > (start_write - start_algo).count() << "," <<
	// 	chrono::duration_cast < chrono::microseconds > (end_write - start_write).count() << ",\n";
	// results_file.close();

	return 0;
}