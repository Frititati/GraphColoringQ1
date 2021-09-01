#!/bin/sh

mkdir Compiled

g++ JonesSpinOffSequential.cpp -o Compiled/JonesSpinOffSequential
g++ -pthread JonesBetterStructureParallel.cpp -o Compiled/JonesBetterStructureParallel

exit 0