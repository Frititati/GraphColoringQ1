Q1: Parallel Graph Coloring

Team: Cardano Filippo Maria (292113), Finocchiaro Loredana (269731), Gagliardi Giuseppe (286286).

All the source files that are provided have been developed in C++11 language and executed in a 
UNIX environment.

The solutions that we’ve developed are compatible to work with DIMACS and DIMACS10 graph formats. 
The algorithms we decided to implement are the Jones-Plassman and the Largest Degree First. 

In the main folder of the project, you can find two different subfolders:
- Officials, that contains all the complete and definitive versions of the algorithms, divided again 
in subfolders: Sequentials, Parallels and Tester.
- Others, where we put some attempts of optimization (even though not totally satisfactory, for 
different reasons).

---------------------------------------------------------------------------

Instructions to compile and execute

-> Sequential
In the folder Officials/Sequential, it is possible to find the sequential versions of both Jones-Plassman and 
LDF coloring algorithms. Input parameters are the name of the graph file and (optionally) the name of the output 
file, where all the colors assigned to each node, one per line, will be printed.
Both can be compiled and executed by issuing the following commands:

# Jones-Plassman

g++ JonesSequential.cpp -o JonesSequential

./JonesSequential <graphFile> [<outputFile>]

# LDF

g++ LdfSequential.cpp -o LdfSequential

./LdfSequential <graphFile> [<outputFile>]


-> Parallel
In the folder Officials/Parallel, there are multithreaded versions of both Jones-Plassman and LDF coloring 
algorithms. Input parameters are the name of the graph file, the number of threads and (optionally) the name 
of the output file.
Both can be compiled and executed by issuing the following commands:

# Jones-Plassman

g++ JonesParallel.cpp -o JonesParallel -pthread

./JonesParallel <graphFile> <threadNum> [<outputFile>]

# LDF

g++ LdfParallel.cpp -o LdfParallel -pthread

./LdfParallel <graphFile> <threadNum> [<outputFile>]


-> Tester
In the folder Officials/GraphTest, there is a simple program, developed by ourselves, too, that checks if the 
output produced either by Jones-Plassman or LDF algorithm is correct with respect to the input graph file. 
Input parameters are the name of the graph file and the name of the output file.
It can be compiled and executed by issuing the following commands:

g++ Tester.cpp -o tester

./tester <graphFile> <outputFile>


-> Compiler utility
There's a utility called compiler.sh that can compile all the .cpp files at once into the folder Compiled.