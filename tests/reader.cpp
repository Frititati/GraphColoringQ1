#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main() {

	fstream graph_file;
	graph_file.open("graph.txt", ios::in);
	
	string line;

	if(graph_file.is_open()) {
		while(getline(graph_file, line)) {
			cout << line << endl;
		}
	}

	graph_file.close();
	
	return 0;

}