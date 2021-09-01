#!/bin/sh

mkdir Compiled

g++ Sequential/JonesSequential.cpp -o Compiled/JonesSequential
g++ Sequential/LdfSequential.cpp -o Compiled/LdfSequential
g++ -pthread Parallel/JonesParallel.cpp -o Compiled/JonesParallel
g++ -pthread Parallel/LdfParallel.cpp -o Compiled/LdfParallel

g++ GraphTest/Tester.cpp -o Compiled/Tester

exit 0