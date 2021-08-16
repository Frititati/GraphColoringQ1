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
#include <thread>
#include <atomic>
#include <condition_variable>

using namespace std;

// the datastructure where we keep the node index and all of it's connections
map<int, vector<int>> node_edge_connections;
// the datastructure where we keep the node random based on index
vector<int> node_random;
// the datastructure where we keep the node color based on index
vector<int> node_color;

// statically assign the max number of threads
int number_threads = 3;
// counter for barriers
int barrier_counter = 0;
// counter for exited threads
int number_exited_threads = 0;

// condition variable for barrier
std::condition_variable barrier_cv;
// mutex for barrier
std::mutex barrier_mutex;

// barrier readiness check
bool barrier_ready = false;

// mutex for writing in CS
std::mutex writing_mtx;

// struct to pass to thread
struct thread_struct {
	int begin;
	int end;
	int name;
};

// wait barrier function
void barrier_wait() {
	std::unique_lock<std::mutex> lck(barrier_mutex);
	barrier_counter++;
	if (barrier_counter == number_threads) {
		barrier_ready = true;
		barrier_cv.notify_all();
		return;
	}
  	while (!barrier_ready) barrier_cv.wait(lck);
}

void barrier_wait_leave(int number_of_colored_nodes) {
	std::unique_lock<std::mutex> lck(barrier_mutex);
	barrier_counter--;
	if (number_of_colored_nodes == 0)
		number_exited_threads++;
	if (barrier_counter == 0) {
		number_threads = number_threads - number_exited_threads;
		number_exited_threads = 0;
		barrier_ready = false;
		barrier_cv.notify_all();
		return;
	}
  	while (barrier_ready) barrier_cv.wait(lck);
}

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

void jones_thread(thread_struct thread_info) {

	// map splitting
	map<int, vector<int> > node_assigned;

	if (thread_info.end == node_edge_connections.size())
	{
		node_assigned.insert(node_edge_connections.find(thread_info.begin), node_edge_connections.end());
	} else {
		node_assigned.insert(node_edge_connections.find(thread_info.begin), node_edge_connections.find(thread_info.end + 1));
	}

	// color is defined as integer
	int color = 1;

	// set of the color we use 
	set<int> colors_used_global = {1};

	// first iteration counter (minor performance improvement)
	bool first_iter = true;

	// max number of nodes to color
	int number_of_colored_nodes = (thread_info.end - thread_info.begin) + 1;

	while(true)
	{
		// map of nodes to be colored and their respective color
		map<int, int> to_be_evaluated;

		// iterate over the map
		for(map<int, vector<int> >::const_iterator it = node_assigned.begin();
			it != node_assigned.end(); ++it)
		{

			// skip if the index of map is too small
			if (it->first < thread_info.begin)
			{
				continue;
			}

			// break if the index of the map is too high
			if (it->first > thread_info.end)
			{
				break;
			}

			// explore connected nodes
			vector<int> connections = it->second;

			bool is_highest = true;

			set<int> colors_used_local;

			int node_random_this = node_random[it->first - 1];
			int node_key_this = it->first;
			
			// look into all the connected nodes
			for (auto i : connections)
			{
				// check if connections is colored
				if (node_color[i - 1] == 0)
				{
					if (node_random_this < node_random[i - 1] || ((node_random_this == node_random[i - 1]) && (node_key_this < i)))
					{
						// not local highest
						is_highest = false;
						break;
					}
				} else {
					colors_used_local.insert(node_color[i - 1]);
				}
			}

			// only execute if the node is the highest in its local section
			if (is_highest)
			{
				// cout << "node " << node_key_this << " highest" << endl;
				if (first_iter)
				{
					to_be_evaluated.insert(std::pair<int, int>(node_key_this, color));
					first_iter = false;
				} else {
					std::set<int> colors_used_interation;
					std::set_difference(colors_used_global.begin(), colors_used_global.end(), colors_used_local.begin(), colors_used_local.end(), std::inserter(colors_used_interation, colors_used_interation.end()));
					auto choosen = colors_used_interation.begin();
					if (*choosen != 0) // se è uguale a 0 non devo colorarlo con zero.
						to_be_evaluated.insert(std::pair<int, int>(node_key_this, *choosen));
				}
			}
		}

		barrier_wait();
		
		// Questo if risolve i problemi (magari esiste approccio migliore)
		if (to_be_evaluated.size() == 0) {
			color++;
			colors_used_global.insert(color);
		}
		
		// se (to_be_evaluated.size() == 0) nemmeno entra nel for
		// per cui inutile mettere il mutex fuori dal for
		for(map<int, int >::const_iterator node_interator = to_be_evaluated.begin();
			node_interator != to_be_evaluated.end(); ++node_interator)
		{
			writing_mtx.lock();
			//cout << "Sono " << thread_info.name << " assegno " << node_interator->second << " a " << node_interator->first << endl;
			node_color[node_interator->first - 1] = node_interator->second;
			node_assigned.erase(node_interator->first);
			writing_mtx.unlock();
			if (node_interator->second == color)
			{
				color++;
				colors_used_global.insert(color);
			}
			number_of_colored_nodes--;
		}

		barrier_wait_leave(number_of_colored_nodes);

		if (number_of_colored_nodes == 0){
			return;
		}
			
	}
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
	int counter = 1;
	int number_node;
	int number_edge;

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

	auto start = chrono::steady_clock::now();

	ios_base::sync_with_stdio(false);

	int multiplier = node_edge_connections.size() / 3;

	thread_struct t1_struct;
	t1_struct.begin = 1;
	t1_struct.end = multiplier;
	t1_struct.name = 1;

	thread_struct t2_struct;
	t2_struct.begin = multiplier+1;
	t2_struct.end = multiplier*2;
	t2_struct.name = 2;

	thread_struct t3_struct;
	t3_struct.begin = (multiplier*2) + 1;
	t3_struct.end = node_edge_connections.size();
	t3_struct.name = 3;

	std::thread thread_1 (jones_thread, t1_struct);
	std::thread thread_2 (jones_thread, t2_struct);
	std::thread thread_3 (jones_thread, t3_struct);

	thread_1.join();
	thread_2.join();
	thread_3.join();

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
	write_file.open(argv[2], ios::out);

	if (write_file.is_open())
	{
		// file exists write to file results
		write_file << to_string(number_node) << " " << to_string(number_edge) << endl;
		for (auto i : node_color)
		{
			write_file << to_string(i) << endl;
		}
	}

	write_file.close();

	return 0;
}