#include <iostream>
#include <fstream>
#include <limits>
#include <string>
#include <vector>
#include <cstdlib>
#include <map>
#include <time.h>
#include <algorithm>
#include <set>
#include <iterator>
#include <chrono>
#include <thread>
#include <atomic>
#include <condition_variable>

using namespace std;

map<int, vector<int> > node_edge_connections;
vector<int> node_random;
vector<int> node_color;
int number_of_threads = 0;
int num = 0;
int exit_threads = 0;
bool directed = 0;
int number_nodes = 0;
string graph;

std::mutex mtx;
std::condition_variable cv;
bool ready = false;
// bool completed = false;

std::vector<std::mutex> mutexes;
std::mutex writingMutex;

/* struct thread_struct {
	int begin;
	int end;
	int name;
}; */

/* void wait_on_cv() {
	std::unique_lock<std::mutex> lck(mtx);
	num++;
	if (num == number_of_threads) {
		completed = true;
		// cout << "notify1 " << name << " " << num << " " << number_of_threads << endl;
		num = 0;
		cv.notify_all();
		return;
	}
	// cout << "waiting1 " << name << " " << num << " " << number_of_threads << endl;
  	while (!completed) cv.wait(lck);
} */

void wait_on_cv1(int name)
{
	std::unique_lock<std::mutex> lck(mtx);
	num++;
	if (num == number_of_threads)
	{
		ready = true;
		// cout << "notify1 " << name << " " << num << " " << number_of_threads << endl;
		cv.notify_all();
		return;
	}
	// cout << "waiting1 " << name << " " << num << " " << number_of_threads << endl;
	while (!ready)
		cv.wait(lck);
}
int wait_on_cv3(int name)
{
	std::unique_lock<std::mutex> lck(mtx);
	num++;
	if (num == number_of_threads)
	{
		ready = true;
		// cout << "notify1 " << name << " " << num << " " << number_of_threads << endl;
		cv.notify_all();
		return name;
	}
	// cout << "waiting1 " << name << " " << num << " " << number_of_threads << endl;
	while (!ready)
		cv.wait(lck);
	return -1;
}

void wait_on_cv2(int number_of_colored_nodes, int name)
{
	std::unique_lock<std::mutex> lck(mtx);
	num--;
	if (number_of_colored_nodes == 0)
		exit_threads++;
	if (num == 0)
	{
		number_of_threads = number_of_threads - exit_threads;
		exit_threads = 0;
		ready = false;
		// cout << "notify2 " << name << " " << num << " " << number_of_threads << endl;
		cv.notify_all();
		return;
	}
	// cout << "waiting2 " << name << " " << num << " " << number_of_threads << endl;
	while (ready)
		cv.wait(lck);
}

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
			cout << "we had an OPSY" << endl;
		}
	}

	try
	{
		res.push_back(stoi(s.substr(pos_start)));
	}
	catch (exception e)
	{
		cout << "we had an OPSY" << endl;
	}
	return res;
}

vector<int> split_to_int_directed(string line, string delimiter)
{
	string s;
	int start_point = line.find(':') + 2;
	int end_point = line.find("#");
	int line_size = end_point - 1 - start_point;
	if (end_point != start_point)
		s = line.substr(start_point, line_size);
	else
		s = "";
	// cout << "modified line: -" << s << "-" << endl;

	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	string token;
	vector<int> res;
	while ((pos_end = s.find(delimiter, pos_start)) != string::npos)
	{
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		try
		{
			res.push_back(stoi(token) + 1);
		}
		catch (exception e)
		{
			cout << "we had an OPSY" << endl;
		}
	}

	try
	{
		res.push_back(stoi(s.substr(pos_start)) + 1);
	}
	catch (exception e)
	{
		cout << "we had an OPSY" << endl;
	}
	return res;
}

std::fstream &GotoLine(std::fstream &file, unsigned int num)
{
	file.seekg(std::ios::beg);
	for (int i = 0; i < num - 1; ++i)
	{
		file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}
	return file;
}

map<int, vector<int> > reading_function(int thread_index)
{
	string line;
	fstream graph_file(graph);
	map<int, vector<int> > node_assigned;

	if (!graph_file.is_open())
	{
		// file does not exists
		cout << "File cannot be found, please refer to this guide:" << endl;
		cout << "<execution file> <graph file> <number of threads>" << endl;
		return node_assigned;
	}

	int position = number_nodes / number_of_threads * (thread_index - 1) + 2;
	// cout << "Position: " << position << endl;
	GotoLine(graph_file, position);

	int more = 0;

	if (thread_index == number_of_threads)
		more = number_nodes - (position - 2 + number_nodes / number_of_threads);
	if (directed)
	{
		for (int i = 0; i < number_nodes / number_of_threads + more; i++)
		{
			getline(graph_file, line);

			// cout << "Sono il thread " << thread_index << " e leggo linea " << line << endl;
			vector<int> temp = split_to_int_directed(line, " ");
			node_edge_connections.insert(std::pair<int, vector<int> >(i + position - 1, temp));
		}
	}
	else
	{
		for (int i = 0; i < number_nodes / number_of_threads + more; i++)
		{
			getline(graph_file, line);

			// cout << "Sono il thread " << thread_index << " e leggo linea " << line << endl;
			vector<int> temp = split_to_int(line, " ");
			node_assigned.insert(std::pair<int, vector<int> >(i + position - 1, temp));
		}
	}
	// readingMutex.lock();
	// node_edge_connections.insert(node_assigned.begin(), node_assigned.end());
	// readingMutex.unlock();

	return node_assigned;
}

void jones_thread(int thread_index)
{
	map<int, vector<int> > node_assigned;
	node_assigned = reading_function(thread_index);
	if (directed)
	{
		int n = wait_on_cv3(thread_index);
		if (n == thread_index)
		{
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
		}

		int multiplier = node_edge_connections.size() / number_of_threads;

		int start = (multiplier * (thread_index - 1)) + 1;
		int end = multiplier * thread_index;

		if (thread_index == number_of_threads)
		{
			node_assigned.insert(node_edge_connections.find(start), node_edge_connections.end());
		}
		else
		{
			node_assigned.insert(node_edge_connections.find(start), node_edge_connections.find(end + 1));
		}
	}

	// print results

	cout << "Number of nodes: " << number_nodes << endl;
	for (map<int, vector<int> >::const_iterator it = node_edge_connections.begin();
		 it != node_edge_connections.end(); ++it)
	{
		cout << "At node :" << it->first << "\t We have connections: ";
		for (auto i : it->second)
			cout << i << " ";
		cout << endl;
	}

	// wait_on_cv();

	// cout << "name: " << thread_index << " start: " << start << " end: " << (start + node_assigned.size() - 1) << " size: " << node_assigned.size() << endl;

	int color = 1;
	set<int> colors_used_global = {1};

	int number_of_colored_nodes = node_assigned.size();

	while (true)
	{
		map<int, int> to_be_evaluated;

		// generate array of all colors that are available

		for (map<int, vector<int> >::const_iterator it = node_assigned.begin();
			 it != node_assigned.end(); ++it)
		{
			// cout << "name: " << thread_index << " node: " << it->first << endl;
			// explore connected nodes
			vector<int> connections = it->second;

			bool is_highest = true;

			set<int> colors_used_local;

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
						// cout << "node " << node_key_this << " not highest against " << i << " rand1 " << node_random_this << " rand2 " << node_random[i - 1] << " color " << node_color[i - 1] << endl;
						is_highest = false;
						break;
					}
				}
				else
				{
					colors_used_local.insert(node_color[i - 1]);
				}
			}

			if (is_highest)
			{
				// cout << "node " << node_key_this << " highest" << endl;
				std::set<int> colors_used_interation;
				std::set_difference(colors_used_global.begin(), colors_used_global.end(), colors_used_local.begin(), colors_used_local.end(), std::inserter(colors_used_interation, colors_used_interation.end()));
				auto choosen = colors_used_interation.begin();
				if (*choosen != 0) // se è uguale a 0 non devo colorarlo con zero.
					to_be_evaluated.insert(std::pair<int, int>(node_key_this, *choosen));
				// cout << "name: " << thread_index << "node key: " << node_key_this << " highest" << endl;
			}
		}

		wait_on_cv1(thread_index);

		// Questo if risolve i problemi (magari esiste approccio migliore)
		if (to_be_evaluated.size() == 0)
		{
			color++;
			colors_used_global.insert(color);
		}

		// se (to_be_evaluated.size() == 0) nemmeno entra nel for
		// per cui inutile mettere il mutex fuori dal for
		for (map<int, int>::const_iterator node_interator = to_be_evaluated.begin();
			 node_interator != to_be_evaluated.end(); ++node_interator)
		{
			// writingMutex.lock();
			// cout << "Sono " << thread_index << " assegno " << node_interator->second << " a " << node_interator->first << endl;
			node_color[node_interator->first - 1] = node_interator->second;
			// writingMutex.unlock();
			node_assigned.erase(node_interator->first);
			if (node_interator->second == color)
			{
				color++;
				colors_used_global.insert(color);
			}
			number_of_colored_nodes--;
		}

		wait_on_cv2(number_of_colored_nodes, thread_index);

		// cout << "name: " << thread_index << " number of colored: " << number_of_colored_nodes << endl;

		if (number_of_colored_nodes == 0)
			return;
	}
}

int main(int argc, char **argv)
{

	if (argc < 3)
	{
		// not enought parameters
		cout << "Not enough parameters passed, please refer to this guide:" << endl;
		cout << "<execution file> <graph file> <number of threads>" << endl;
		return 1;
	}

	// TODO : we have to check if this is exeception prone

	fstream graph_file;
	graph = argv[1];
	graph_file.open(argv[1], ios::in);

	if (!graph_file.is_open())
	{
		// file does not exists
		cout << "File cannot be found, please refer to this guide:" << endl;
		cout << "<execution file> <graph file> <number of threads>" << endl;
		return 1;
	}

	string line;
	int new_node_index = 1;
	// int number_nodes;
	int number_edges;

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
	}

	graph_file.close();

	// for (auto i : temp) cout << i << endl;

	for (int i = 0; i < number_nodes; i++)
	{
		node_color.push_back(0);
		node_random.push_back(rand() % 100 + 1);
	}

	/*thread thread_reading[3];

	for (int i = 0; i < 3; ++i)
	{
		cout << "generated thread: " << (i + 1) << endl;
		thread_reading[i] = thread(reading_function, i + 1);
	}

	for (auto& t : thread_reading)
	{
		t.join();
	}*/

	/*for(map<int, vector<int> >::const_iterator it = node_edge_connections.begin(); it != node_edge_connections.end(); ++it) {
			stringstream ss;
			for(int i = 0; i < it->second.size(); ++i)
			{
			if(i != 0)
				ss << " ";
			ss << it->second[i];
			}
			string s = ss.str();
			cout << s << endl;
		}*/

	/*while(getline(graph_file, line)) {
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
		}*/

	// }

	///////////////////////////////////////////////////////
	/*while (node_edge_connections.size() < number_nodes)
	{
		node_edge_connections.insert(std::pair<int, vector<int>>(new_node_index, {}));
		node_random.push_back(rand() % 100 + 1);
		node_color.push_back(0);
		new_node_index++;
	}*/
	///////////////////////////////////////////////////////

	// graph_file.close();

	// int debug_counter = 0;
	// for(map<int, vector<int> >::const_iterator it = node_edge_connections.begin();
	// 	it != node_edge_connections.end(); ++it)
	// {
	// 	cout << "node :" << it->first << endl;
	// 	cout << "random :" << node_random[debug_counter] << endl;
	// 	for (auto i : it->second) cout << "conn " << to_string(i) << endl;
	// 	debug_counter++;
	// }

	auto start = chrono::steady_clock::now();

	// unsync the I/O of C and C++.
	ios_base::sync_with_stdio(false);

	number_of_threads = stoi(argv[2]);
	std::vector<std::mutex> list(number_of_threads);
	mutexes.swap(list);
	// int multiplier = node_edge_connections.size() / number_of_threads;

	thread thread_array[number_of_threads];

	for (int i = 0; i < number_of_threads; ++i)
	{
		cout << "generated thread: " << (i + 1) << endl;
		thread_array[i] = thread(jones_thread, i + 1);
	}

	// print results
	cout << "Number of nodes: " << number_nodes << endl;
	for (map<int, vector<int> >::const_iterator it = node_edge_connections.begin();
		 it != node_edge_connections.end(); ++it)
	{
		cout << "At node :" << it->first << "\tWe have connections: ";
		for (auto i : it->second)
			cout << i << " ";
		cout << endl;
	}

	//join
	for (auto &t : thread_array)
	{
		t.join();
	}

	// int counter_3 = 1;
	// for (auto i : node_color)
	// {
	// cout << "node : " << to_string(counter_3) << " color " << i << " random " << node_random[counter_3 - 1] << endl;
	// counter_3++;
	// }

	// Recording end time.
	auto end = chrono::steady_clock::now();

	// Calculating total time taken by the program.
	cout << "Elapsed time in nanoseconds: "
		 << chrono::duration_cast<chrono::nanoseconds>(end - start).count()
		 << " ns" << endl;

	cout << "Elapsed time in microseconds: "
		 << chrono::duration_cast<chrono::microseconds>(end - start).count()
		 << " µs" << endl;

	fstream write_file;
	write_file.open(argv[3], ios::out);

	int max = 0;
	if (write_file.is_open())
	{
		// file exists write to file results
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