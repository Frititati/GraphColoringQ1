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
map<int, vector<int> > node_edge_connections;
// the datastructure where we keep the node random based on index
vector<int> node_random;
// the datastructure where we keep the node color based on index
vector<int> node_color;

vector<string> split(string s, string delimiter)
{
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	string token;
	vector<string> res;

	while ((pos_end = s.find(delimiter, pos_start)) != string::npos)
	{
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		res.push_back(token);
	}

	res.push_back(s.substr(pos_start));
	return res;
}

vector<int> split_to_int(string s, string delimiter)
{
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	string token;
	vector<int> res;

	while ((pos_end = s.find(delimiter, pos_start)) != string::npos)
	{
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		try
		{
			res.push_back(stoi(token));
		}
		catch (exception e)
		{
			// we don't have any node connections
			// cout << "There are no connections" << endl;
		}
	}

	try
	{
		res.push_back(stoi(s.substr(pos_start)));
	}
	catch (exception e)
	{
		// we don't have any node connections
		// cout << "There are no connections" << endl;
	}
	return res;
}

vector<int> split_to_int_mod (string line, string delimiter) {
  string s;
  int start_point = line.find(':')+2;
  int end_point = line.find("#");
  int line_size = end_point - 1 - start_point;
  if (end_point != start_point)
    s = line.substr(start_point, line_size);
  else s = "";
  // cout << "modified line: -" << s << "-" << endl;

	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	string token;
	vector<int> res;
	while ((pos_end = s.find (delimiter, pos_start)) != string::npos) {
		token = s.substr (pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		try {
			res.push_back (stoi(token)+1);
		} catch (exception e){
			cout << "we had an OPSY" << endl;
		}
	}

	try {
		res.push_back (stoi(s.substr (pos_start))+1);
	} catch (exception e){
		cout << "we had an OPSY" << endl;
	}
	return res;
}

int jones_sequential()
{
	// color is defined as integer
	int color = 1;
	// we have a set of the colors used (this is useful to compare later in the iterative step)
	set<int> colors_used_global = {1};

	// we continue until the node_edge_connection is empty
	while (node_edge_connections.size() != 0)
	{
		// map of all the nodes to be evaluated this iteration <index, color>
		map<int, int> to_be_evaluated;

		// cout << "size node " << to_string(node_edge_connections.size()) << " color " << color << endl;

		// iterate on the map
		for (map<int, vector<int> >::const_iterator it = node_edge_connections.begin();
			 it != node_edge_connections.end(); ++it)
		{
			// vector of all the connected nodes
			vector<int> connections = it->second;

			bool is_highest = true;

			set<int> colors_used_local;

			int node_random_this = node_random[it->first - 1];
			int node_key_this = it->first;

			// look into all the connected nodes
			for (auto i : connections)
			{
				// check if connections is not colored
				if (node_color[i - 1] == 0)
				{
					if (node_random_this < node_random[i - 1] || ((node_random_this == node_random[i - 1]) && (node_key_this < i)))
					{
						// not local highest
						is_highest = false;
						break;
					}
				}
				else
				{
					colors_used_local.insert(node_color[i - 1]);
				}
			}

			// only execute if the node is the highest in its local section
			if (is_highest)
			{
				// cout << "node " << node_key_this << " highest" << endl;
				std::set<int> colors_used_interation;
				std::set_difference(colors_used_global.begin(), colors_used_global.end(), colors_used_local.begin(), colors_used_local.end(), std::inserter(colors_used_interation, colors_used_interation.end()));
				auto choosen = colors_used_interation.begin();
				to_be_evaluated.insert(std::pair<int, int>(node_key_this, *choosen));
			}
		}

		for (map<int, int>::const_iterator node_interator = to_be_evaluated.begin();
			 node_interator != to_be_evaluated.end(); ++node_interator)
		{
			node_color[node_interator->first - 1] = node_interator->second;
			node_edge_connections.erase(node_interator->first);

			if (node_interator->second == color)
			{
				color++;
				colors_used_global.insert(color);
			}
		}
	}

	return 0;
}

int main(int argc, char **argv)
{

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
	bool directed = false;
	// we used srand to set seed for randomization of node numbers
	// srand(time(NULL));

	if (graph_file.is_open())
	{

		// this is first line
		getline(graph_file, line);
		vector<int> parse_first_line = split_to_int(line, " ");

		number_nodes = parse_first_line[0];
		directed = (parse_first_line.size() == 1);
		if (!directed)
			number_edges = parse_first_line[1];

		// for (auto i : temp) cout << i << endl;
		if (directed)
		{
			while (getline(graph_file, line))
			{
				vector<int> temp;
				temp = split_to_int_mod(line, " ");
				
				node_edge_connections.insert(std::pair<int, vector<int> >(new_node_index, temp));

				// add random number to each node
				node_random.push_back(rand() % 100 + 1);
				// add color 0
				node_color.push_back(0);

				new_node_index++;
			}
			map<int, vector<int> > temp_node_edge = node_edge_connections;

			// translate the graph

			//for each node of the map
			for (map<int, vector<int> >::const_iterator it = node_edge_connections.begin();
				 it != node_edge_connections.end(); ++it)
			{
				//for each element of the selected array, insert the node it->first into the nodes referenced in the array
				for (auto i : it->second)
					temp_node_edge[i].push_back(it->first);
			}
			node_edge_connections = temp_node_edge;

			// compute number_edges

			for (map<int, vector<int> >::const_iterator it = node_edge_connections.begin();
				 it != node_edge_connections.end(); ++it)
			{
				number_edges += it->second.size();
			}
		}
		else
		{
			while (getline(graph_file, line))
			{
				// parsing node line

				vector<int> temp = split_to_int(line, " ");

				// cout << "node number :" << to_string(new_node_index) << endl;
				// for (auto i : temp) cout << "conn " << to_string(i) << endl;

				node_edge_connections.insert(std::pair<int, vector<int> >(new_node_index, temp));

				// add random number to each node
				node_random.push_back(rand() % 100 + 1);
				// add color 0
				node_color.push_back(0);

				new_node_index++;
			}
		}
	}

	while (node_edge_connections.size() < number_nodes)
	{
		node_edge_connections.insert(std::pair<int, vector<int> >(new_node_index, {}));
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
		int max = 0;
		// if file exists write to file results
		write_file << to_string(number_nodes) << " " << to_string(number_edges) << endl;
		for (auto i : node_color)
		{
			write_file << to_string(i) << endl;
			if (i > max)
				max = i;
		}
		cout << "Used colors: " << max << endl;
	}
	write_file.close();

	return 0;
}
