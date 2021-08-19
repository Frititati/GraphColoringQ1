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

// structure of non-directed nodes
struct node_struct {
	int index;
	int size;
	int* connections;
};

// the datastructure where we keep the node random based on index
vector<int> node_random;
// the datastructure where we keep the node color based on index
vector<int> node_color;
int number_threads = 0;
// counter for barriers
int barrier_counter = 0;
// counter for exited threads
int number_exited_threads = 0;

// number of all nodes
int number_nodes = 0;

// path to the graph
string graph_path;

// condition variable for barrier
std::condition_variable barrier_cv;
// mutex for barrier
std::mutex barrier_mutex;

// vector of reading times for each thread
vector<chrono::microseconds> reading_times;

void wait_single(int name) {
	// get lock
	std::unique_lock<std::mutex> lck(barrier_mutex);

	// lower barrier counter
	barrier_counter--;

	// if the barrier_counter is 0 means we are the last thread
	// reset variable for next barrier
	if (barrier_counter == 0) {
		// fix number_threads to adjust for exited threads
		barrier_counter = number_threads;
		// cout << "notify2 " << name << " " << barrier_counter << " " << number_threads << endl;
		barrier_cv.notify_all();

		// leave without going to the condition wait
		return;
	}
	// cout << "waiting2 " << name << " " << barrier_counter << " " << number_threads << endl;
	barrier_cv.wait(lck);
}

void wait_leave(int number_of_colored_nodes, int name) {
	// get lock
	std::unique_lock<std::mutex> lck(barrier_mutex);

	// lower barrier counter
	barrier_counter--;

	// if the thread did not color anything he has to be exiting
	if (number_of_colored_nodes == 0){
		number_exited_threads++;
	}

	// if the barrier_counter is 0 means we are the last thread
	// reset variable for next barrier
	if (barrier_counter == 0) {
		// fix number_threads to adjust for exited threads
		number_threads = number_threads - number_exited_threads;
		number_exited_threads = 0;
		barrier_counter = number_threads;
		// cout << "notify2 " << name << " " << barrier_counter << " " << number_threads << endl;
		barrier_cv.notify_all();

		// leave without going to the condition wait
		return;
	}
	// cout << "waiting2 " << name << " " << barrier_counter << " " << number_threads << endl;
	barrier_cv.wait(lck);
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
		}
	}

	try {
		res.push_back (stoi(s.substr (pos_start)));
	} catch (exception e){
		// we don't have any node connections
	}
	return res;
}

void split_directed_node (string s, string delimiter, node_struct *node, int index) {
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	string token;
	vector<int> res;

	while ((pos_end = s.find (delimiter, pos_start)) != string::npos) {
		token = s.substr (pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		try {
			res.push_back(stoi(token));
		} catch (exception e){
			// we don't have any node connections
		}
	}

	try {
		res.push_back (stoi(s.substr (pos_start)));
	} catch (exception e){
		// we don't have any node connections
	}

	(*node).index = index;
	(*node).size = res.size();

	(*node).connections = new int[res.size()];
	std::copy(res.begin(), res.end(), (*node).connections);
}

std::fstream& GotoLine(std::fstream& file, unsigned int num) {
	file.seekg(std::ios::beg);
	for(int i=0; i < num - 1; ++i){
		file.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
	}
	return file;
}

void reading_function(int thread_index, node_struct *&node_assigned, int *number_of_nodes) {
	string line;
	fstream graph_file(graph_path);

	if (!graph_file.is_open())
	{
		// file does not exists
		cout << "File cannot be found, please refer to this guide:" << endl;
		cout << "<execution file> <graph file> <number of threads>" << endl;
		return;
	}

	int position = number_nodes/number_threads*(thread_index-1)+2;
	GotoLine(graph_file, position);

	int more = 0;	
	if (thread_index == number_threads) {
		more = number_nodes - (position-2 + number_nodes/number_threads);
	}

	node_struct *local_nodes;
	local_nodes = new node_struct[(number_nodes/number_threads+more)];

	for(int i=0; i<number_nodes/number_threads+more; i++) {
		getline(graph_file, line);
		local_nodes[i].index = i+position-1;
		split_directed_node(line, " ", &local_nodes[i], i+position-1);
	}

	*number_of_nodes = (number_nodes/number_threads+more);
	node_assigned = local_nodes;
}

// copied online to fill set :(
template<class OutputIterator, class T>
OutputIterator iota_rng(OutputIterator first, T low, T high)
{
		return std::generate_n(first, high - low, [&, value = low]() mutable { return value++; });
} 

void jones_thread(int thread_index) {

	auto start_time = chrono::steady_clock::now();

	node_struct *node_assigned = new node_struct;
	int temp = 0;
	int *number_of_nodes = &temp;

	reading_function(thread_index, node_assigned, number_of_nodes);
	
	auto end_time = chrono::steady_clock::now();
	reading_times[thread_index-1] = chrono::duration_cast<chrono::microseconds>(end_time - start_time);

	int start = ((number_nodes/number_threads)*(thread_index-1))+1;
	int end = start + (number_nodes/number_threads) - 1;
	if (thread_index == number_threads) {
		end = number_nodes;
	}

	set<int> nodes_iteration;
	iota_rng(std::inserter(nodes_iteration, nodes_iteration.begin()), start, end+1);

	// cout << "A t" << thread_index << "  s:" << start << "  e:" << end << "  n:" << *number_of_nodes << endl;

	int color = 1;
	set<int> colors_used_global = {1};

	int number_of_colored_nodes = *number_of_nodes;

	while(true)
	{
		map<int, int> to_be_evaluated;

		for(auto node_key_this : nodes_iteration)
		{
			// cout << "name: " << thread_index << " node: " << node_key_this << endl;
			// explore connected nodes
			int index = node_key_this - start;

			bool is_highest = true;

			set<int> colors_used_local;

			int node_random_this = node_random[node_key_this - 1];
		
			for (int j = 0; j < node_assigned[index].size; j++)
			{
				int i = node_assigned[index].connections[j];
				// cout << "node:" << node_key_this << " index:" << index << " val:" << i << " size:" << node_assigned[index].size << endl;
				// cout << "node " << node_key_this->first << " look at " << to_string(i) << endl;
				// check if connections is colored
				if (node_color[i - 1] == 0)
				{
					if (node_random_this < node_random[i - 1] || ((node_random_this == node_random[i - 1]) && (node_key_this < i)))
					{
						// cout << "node:" << node_key_this << " not highest against node:" << i << " randA " << node_random_this << " randB " << node_random[i - 1] << " color " << node_color[i - 1] << endl;
						is_highest = false;
						break;
					}
				} else {
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
				// cout << "name: " << thread_index << " node: " << node_key_this << " highest" << endl;
			}
		}

		// removed first barrier
		wait_single(thread_index);

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
			// debugMutex.lock();
			// cout << "node:" << node_interator->first << " new color:" << node_interator->second << endl;
			node_color[node_interator->first - 1] = node_interator->second;
			// debugMutex.unlock();
			nodes_iteration.erase(node_interator->first);
			if (node_interator->second == color)
			{
				color++;
				colors_used_global.insert(color);
			}
			number_of_colored_nodes--;
		}

		wait_leave(number_of_colored_nodes, thread_index);

		// cout << "name: " << thread_index << " number of colored: " << number_of_colored_nodes << endl;

		if (number_of_colored_nodes == 0){
			return;
		}
		
	}
}


int main(int argc, char** argv) {

	if (argc < 3)
	{
		// not enought parameters
		cout << "Not enough parameters passed, please refer to this guide:" << endl;
		cout << "<execution file> <graph file> <number of threads>" << endl;
		return 1;
	}

	// TODO : we have to check if this is exeception prone

	fstream graph_file;
	graph_path = argv[1];
	graph_file.open(argv[1], ios::in);

	if (!graph_file.is_open())
	{
		// file does not exists
		cout << "File cannot be found, please refer to this guide:" << endl;
		cout << "<execution file> <graph file> <number of threads>" << endl;
		return 1;
	}
	
	string line;
	int number_edges;

	// we used srand to set seed for randomization of node numbers
	srand(time(NULL));

	if(graph_file.is_open()) {

		// this is first line
		getline(graph_file, line);
		vector<int> parse_first_line = split_to_int(line, " ");

		number_nodes = parse_first_line[0];
		number_edges = parse_first_line[1];

	}

	cout << "number nodes:" << number_nodes << endl;

	graph_file.close();

	// for (auto i : temp) cout << i << endl;

	for (int i=0; i<number_nodes; i++) {
		node_color.push_back(0);
		node_random.push_back(rand() % 100 + 1);
	}

	auto start_main = chrono::steady_clock::now();

	// unsync the I/O of C and C++.
	ios_base::sync_with_stdio(false);

	number_threads = stoi(argv[2]);
	barrier_counter = number_threads;

	thread thread_array[number_threads];

	for (int i = 0; i < number_threads; ++i)
	{
		cout << "generated thread: " << (i + 1) << endl;
		thread_array[i] = thread(jones_thread, i + 1);
		reading_times.push_back((chrono::microseconds) 0);
	}

	for (auto& t : thread_array)
	{
		t.join();
	}

	// Recording end time.
	auto end_main = chrono::steady_clock::now();

	int time_sum = 0;

	for (auto time : reading_times)
	{
		// cout << "Elapsed " << chrono::duration_cast<chrono::microseconds>(time).count() << " µs" << endl;
		time_sum += chrono::duration_cast<chrono::microseconds>(time).count();
	}
	int time_average = static_cast<int>(time_sum/reading_times.size());

	cout << "Elapsed reading time in microseconds: " << time_average << " µs" << endl;

	int algo_time = chrono::duration_cast<chrono::microseconds>(end_main - start_main).count() - time_average;

	cout << "Elapsed algo time in microseconds: " << algo_time << " µs" << endl;

	auto start_write = chrono::steady_clock::now();

	string final = to_string(number_nodes) + " " + to_string(number_edges) + "\n";

	int max_color = 0;

	for (auto i : node_color)
	{
		final += to_string(i) + "\n";
		if (i > max_color)
		{
			max_color = i;
		}
	}

	// cout << "size of final cstr " << (final.size() * sizeof(char)) << endl;
	auto output_file = std::fstream(argv[3], std::ios::out | std::ios::binary);
	output_file.write(final.c_str(), (final.size() * sizeof(char)) );
	output_file.close();

	auto end_write = chrono::steady_clock::now();

	// Calculating total time taken by the program.
	cout << "Elapsed writing time in microseconds: "
		<< chrono::duration_cast<chrono::microseconds>(end_write - start_write).count()
		<< " µs" << endl;

	// Calculating total time taken by the program.
	cout << "Elapsed time in microseconds: "
		<< chrono::duration_cast<chrono::microseconds>(end_write - start_main).count()
		<< " µs" << endl;

	// unsigned int n = std::thread::hardware_concurrency();
	// std::cout << n << " concurrent threads are supported.\n";

	std::ofstream results_file;
	results_file.open("p5.csv", std::ios_base::app);
	results_file << graph_path.substr(graph_path.find_last_of("/\\") + 1) << ","
		<< number_nodes << ","
		<< max_color << ","
		<< to_string(chrono::duration_cast<chrono::microseconds>(end_write - start_main).count()) << ","
		<< time_average << ","
		<< algo_time << ","
		<< to_string(chrono::duration_cast<chrono::microseconds>(end_write - start_write).count()) << ",\n";
	results_file.close();

	return 0;
}