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

map<int, vector<int>> node_edge_connections;
vector<int> node_random;
vector<int> node_color;
int number_of_threads = 2;
int number_of_thread_waiting = 2;
// barrier thread_sync_point1(2);
// barrier thread_sync_point2(2);
std::condition_variable waitConditionVariable;
std::mutex waitMutex;

std::mutex writingMutex;


struct thread_struct {
	int begin;
	int end;
	int name;
};

void Wait(int name) {
	std::unique_lock<std::mutex> lLock{waitMutex};
	if (!--number_of_thread_waiting) {
		number_of_thread_waiting = number_of_threads;
		// cout << "notify " << name << " " << number_of_thread_waiting << " " << number_of_threads << endl;
		waitConditionVariable.notify_all();
	} else {
		// cout << "waiting " << name << " " << number_of_thread_waiting << " " << number_of_threads << endl;
		waitConditionVariable.wait(lLock);
	}
}

void Leave(int name) {
	std::unique_lock<std::mutex> lLock{waitMutex};
	// cout << "leaving " << name << endl;
	number_of_threads--;
	if (!--number_of_thread_waiting)
	{
		number_of_thread_waiting = number_of_threads;
		// cout << "notify leaving " << name << " " << number_of_thread_waiting << " " << number_of_threads << endl;
		waitConditionVariable.notify_all();
	}
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

void jones_thread(thread_struct thread_info) {
	// cout << thread_info.name << " begin: " << thread_info.begin << " end: " << thread_info.end << endl;

	int color = 1;
	set<int> colors_used_global = {1};

	bool first_iter = true;

	int number_of_colored_nodes = (thread_info.end - thread_info.begin) + 1;

	// 
	while(true)
	{
		map<int, int> to_be_evaluated;

		// cout << "size node " << to_string(node_edge_connections.size()) << " color " << color << endl;

		// generate array of all colors that are available

		for(map<int, vector<int> >::const_iterator it = node_edge_connections.begin();
			it != node_edge_connections.end(); ++it)
		{

			if (it->first < thread_info.begin)
			{
				continue;
			}

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
					colors_used_local.insert(node_color[i - 1]);
				}
			}


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
					to_be_evaluated.insert(std::pair<int, int>(node_key_this, *choosen));
				}
			}
		}

		Wait(thread_info.name);

		// BEGIN CS
		writingMutex.lock();

		for(map<int, int >::const_iterator node_interator = to_be_evaluated.begin();
			node_interator != to_be_evaluated.end(); ++node_interator)
		{
			node_color[node_interator->first - 1] = node_interator->second;
			node_edge_connections.erase(node_interator->first);

			if (node_interator->second == color)
			{
				color++;
				colors_used_global.insert(color);
			}
			number_of_colored_nodes--;
		}

		writingMutex.unlock();
		// END CS

		// if we want to leave
		if (number_of_colored_nodes == 0)
		{
			Leave(thread_info.name);
			return;
		}

		Wait(thread_info.name);
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

	cout << "$$$$$$$$$$$$$$$$$$$$" << endl;

	// int counter_2 = 0;

	// for(map<int, vector<int> >::const_iterator it = node_edge_connections.begin();
	// 	it != node_edge_connections.end(); ++it)
	// {
	// 	cout << "node :" << it->first << endl;
	// 	cout << "random :" << node_random[counter_2] << endl;
	// 	for (auto i : it->second) cout << "conn " << to_string(i) << endl;
	// 	counter_2++;
	// }

	// here we call jones

	auto start = chrono::steady_clock::now();

	// unsync the I/O of C and C++.
	ios_base::sync_with_stdio(false);




	// begin theading

	// 2 thread to test

	int middle = node_edge_connections.size() / 2;

	thread_struct t1_struct;
	t1_struct.begin = 1;
	t1_struct.end = middle;
	t1_struct.name = 1;

	thread_struct t2_struct;
	t2_struct.begin = middle + 1;
	t2_struct.end = node_edge_connections.size();
	t2_struct.name = 2;

	cout << "middle " << middle << endl;

	std::thread thread_1 (jones_thread, t1_struct);
	std::thread thread_2 (jones_thread, t2_struct);


	thread_1.join();
	thread_2.join();

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