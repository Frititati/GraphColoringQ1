<!-- PROJECT LOGO -->
<br />
<h3 align="center">Politecnico di Torino</h3>
<h3 align="center">System and device programming</h3>
<h3 align="center">Academic year 2020-2021</h3>
<h4 align="center">Team members: Cardano Filippo, Finocchiaro Loredana, Gagliardi Giuseppe</h3><br />
<p align="center">
  <a href="https://github.com/Frititati/GraphColoringQ1">
    <img src="images/graph.png" alt="Logo" width="80" height="80">
  </a>
  
  <h1 align="center">Q1: Parallel Graph Coloring</h1>

  <p align="center">
    In graph theory, graph coloring is a special case of graph labeling; in which particular labels, traditionally called “colors”, are assigned to elements of a graph subject to certain constraints.
    <br /> In its simplest form, it is a way of coloring the vertices of a graph such that no two adjacent vertices are of the same color. Graph coloring enjoys many practical applications as well as theoretical challenges. Many graph applications are based on vertex coloring and many graph properties are based on graph coloring. Moreover, vertex coloring is the most famous version of coloring since other coloring problems can be transformed into a vertex coloring instance.
    <br /><br />
    <a href="https://github.com/Frititati/GraphColoringQ1"><strong>Explore the docs »</strong></a>
    <br />
    <br />
  </p>
</p>


<!-- TABLE OF CONTENTS -->
<details open="open">
  <summary><h2 style="display: inline-block">Table of Contents</h2></summary>

- [Abstract](#abstract)
- [Folders organization](#folders-organization)
- [Program execution](#program-execution)
  + [Sequential](#sequential)
  + [Parallel](#parallel)
  + [Tester](#tester) 
- [Development details](#development-details)
  + [Jones-Plassman (JP)](#jones-plassman)
    + [JP Reading phase](#jp-reading-phase)
    + [JP Coloring phase](#jp-coloring-phase)
    + [JP Writing phase](#jp-writing-phase)
  + [Largest Degree First (LDF)](#largest-degree-first)
    + [LDF Reading phase](#ldf-reading-phase)
    + [LDF Coloring phase](#ldf-coloring-phase)
    + [LDF Writing phase](#ldf-writing-phase)
- [Development Philosophy](#development-philosophy)
  + [Sequential Optimization Jones-Plassman](#sequential-optimization-jones\-plassman)
- [Performance tests](#performance-tests)
- [Conclusions](#conclusions)

</details>
<br />

# Abstract

This document aims to present and discuss in detail the project _Q1: Parallel Graph Coloring_ related to the System and device programming course, teached by professor Quer.<br />
We'll start by indicating the folders organization and providing the commands necessary to compile and run the submitted programs, then we'll go deep in each of them in order to describe and motivate our approach and choices. Finally, we'll show and compare data obtained from the performance testing we carried out, and draw conclusion from them.<br />
We remind that all the source files that we provided have been developed in **C++11** language and executed in a **UNIX** environment.<br />
Here below you can find the basic details related to the three members of the team:

- Cardano Filippo Maria, 292113;
- Finocchiaro Loredana, 269731;
- Gagliardi Giuseppe, 286286.

<br />

# Folders organization

In the main folder of the project, you can find three different subfolders:

- **Officials**, that contains all the complete and definitive versions of the algorithms, divided again in subfolders: Sequentials, Parallels and Tester.
- **Others**, where we put some attempts of optimization (even though not totally satisfactory, for different reasons), together with the original .csv files used for conducting performance testing.

<br />

# Program execution

The solutions that we’ve developed so far are compatible to work both with DIMACS and DIMACS10 graph formats that have been provided.
The algorithms we decided to implement are the Jones-Plassman and the Largest Degree First. You can find both sequential and parallel versions of them.

Here below the command (and link) for cloning the GitHub repository:
   ```sh
   git clone https://github.com/Frititati/GraphColoringQ1
   ```

## Sequential

In the folder Sequential, it is possible to find the sequential versions of both Jones-Plassman and LDF coloring algorithms. Input parameters are the name of the graph file and (optionally) the name of the output file, where all the colors assigned to each node, one per line, will be printed.<br />
Both can be compiled and executed by issuing the following commands:<br />
   ```sh
   g++ file_name.cpp -o executable_name

   ./executable_name <graphFile> [<outputFile>]
   ```

## Parallel

In the folder Parallel, there are multithreaded versions of both Jones-Plassman and LDF coloring algorithms. Input parameters are the name of the graph file, the number of threads and (optionally) the name of the output file.<br />
Both can be compiled and executed by issuing the following commands:<br />
   ```sh
   g++ file_name.cpp -o executable_name -lpthread -std=c++11

   ./executable_name <graphFile> <threadNum> [<outputFile>]
   ```

## Tester

In the folder GraphTest, there is a simple program, developed by ourselves, too, that checks if the output produced either by Jones-Plassman or LDF algorithm is correct with respect to the input graph file. Input parameters are the name of the graph file and the name of the output file. Basically, it verifies if there is any node that has a neighbour node colored with the same color that has been assigned to itself. In that case, it will finally print on the stdout not only the total number of error that it encountered during the scan, but also the specific nodes (and colors) involved in the errors.<br />
Such a program revealed to be really useful during the implementation phase of our solutions, and can be compiled and executed by issuing the following commands:<br />
   ```sh
   g++ file_name.cpp -o tester

   ./tester <graphFile> <outputFile>
   ```
<br />

# Development Details

## Jones-Plassman

This algorithm improves upon the MIS algorithm, which creates a new random permutation of the nodes every time an independent set needs to be calculated. Jones-Plassman, instead, constructs a single set of unique random weights at the beginning and uses it through-out the coloring algorithm. This can easily be done by assigning random numbers to each of the vertices and using the unique vertex index to resolve a conflict in the unlikely event of neighboring vertices getting the same random number. Nodes are then colored individually using the smallest available color.

### JP Reading Phase

In the **sequential** version of the algorithm, the main thread, after checking the correctness of the input parameters, opens the graph file, verifies the format (directional/undirectional) and then starts parsing it. Each line is split and related data is stored in a global std::map, named `node_edge_connections`, where the key, of type integer, corresponds to the index of the node described in that line, while the value is a std::vector of integers that contains, one by one, all the nodes the examined node is connected to.<br />
Other global data structures used not only in this particular phase but also during the whole execution of the program are two vectors of integers: `node_random` that will store the random numbers assigned to each node, and `node_color` used for memorizing the colors that each node will be colored with.<br />
In the same loop used for the file parsing, at each iteration, a new random number (in the range 1 and 100) is generated and pushed back to the `node_random` vector; at the same time, a zero is pushed back to the `node_color` vector (as the algorithm says, we need to initialize each node with the color 0, that basically means “no color”).<br />
In case the graph is directional, there is one more step where missing connections are added to the global map, basically aiming to obtain the same result of an undirectional graph.

In the **parallel** version of the algorithm, the reading phase has been parallelized as well as the coloring one. In this case, the initialization of the two global vectors `node_random` and `node_color` is done by the main thread; after that, threads are created.<br />
It is possible to invoke two different reading functions depending on the format of the graph file, that is still verified by the main thread before launching the others. So each thread opens independently the file, since each one has to read a different part of it at the same time. The number of lines to be parsed and the position from which the reading has to start is computed starting from the unique ID that has been assigned to each thread during the creation phase; then, everything proceedes as described in the sequential version, except for the usage of the global `node_edge_connections` map, that is still needed during the reading phase in case the format of the graph is directional (in the undirectional case, each thread populates directly its local `node_assigned` map, that will be used during the coloring phase).<br />
From a synchronization point of view, a barrier is needed at the end of the reading phase and before proceeding with the coloring one only in case the format of the graph is directional.

### JP Coloring Phase

The coloring phase consists in a while loop that can be divided in two different sections.
In the first section of the **sequential** version, we start iterating over the `node_edge_connections` map and, for each of the examined nodes, we iterate over all its connections and compare the examined node with its connected nodes: if the connected node has already been colored, we store its color in the std::set `colors_used_local`, otherwise we check if the random number associated to it (connected node) is higher or lower than the random number of the examined node, in order to determine if the examined node is the highest or not (in case of conflicts, we take in consideration the respective indexes of the nodes). In case the node is the highest one, we store its key and color to be assigned in the `to_be_evaluated` map, that will be used later on during the second section. It’s interesting to note that the color to be assigned to the highest node is choosen by taking the first (and lowest) color from the set derived from the difference between two sets (`colors_used_global` and `colors_used_local`); this operation is done directly by leveraging the set_difference function.<br />
After all the nodes of the `node_edge_connections` map have been analyzed, the second section starts. We iterate over the `to_be_evaluated` map and perform two actions for each node:

- Memorize the color that has been assigned to it in the `node_color` vector;
- Remove that node from the `node_edge_connections` map.

In case we’ve colored at least one node with the highest color being in the set `colors_used_global`, we need to add a new color to this set before proceeding with the next iteration of the while loop. The algorithm finishes when all the nodes of the graph have been colored (i.e., until the `node_edge_connections` map becomes totally empty).

There are basically two main differences between the sequential version and the parallel one.

- In the multhreaded environment, each thread iterates over its own local map of nodes and connections, that is a portion of the global `node_edge_connections` map used in the sequential case.
- Since threads need to access global resources both for reading and writing (such as the `node_color` vector) at the same time, synchronization strategies have become fundamental. We chose to insert two barriers (implemented opportunely by using a mutex, a condition variable and a counter): the first one is put between the first and the second section of the coloring phase (before writing to the node_color vector, it is necessary that all the threads have finished iterating over their local map), the second one, instead, is put at the end of the second section (before proceeding with the next iteration, it is necessary to verify if there are threads that, having finished coloring their map, are going to exit, and update accordingly the number of threads that are still working).

### JP Writing Phase
Writing phase is exactly the same in both **sequential** and **parallel** versions. Basically, it consists in producing an output file where the resulting `node_color` vector is printed in order, one color per line.</br>
////// TO BE DESCRIBED IN DETAIL //////

<br />

## Largest Degree First

The Largest Degree First algorithm can be considered as an optimization of the Jones-Plassman: differently from Jones-Plassman, it first looks at the degree (i.e. number of connections) of each node, and starts coloring nodes in order of decreasing degree; random numbers are still used in order to avoid possible conflicts that may arise in case we compare nodes that have exactly the same degree.<br />
Since LDF implementation started from Jones-Plassman one, the structure of both sequential and parallel versions is practically the same. We’ll report in detail the main differences in the next paragraphs.

### LDF Reading Phase

LDF algorithm required the definition of a new global data structure, `node_degree`: similarly to `node_color` and `node_random`, it is a vector of integers where the degree of each node is stored. It is filled during the reading phase by simply taking the size of the vector of connections that is inserted as value part in the (global or local, depending on the version and on the graph format) map that contains the index of the node as key.<br />
From a synchronization point of view, a barrier is needed at the end of the reading phase and before proceeding with the coloring one, independently of the graph file format.

### LDF Coloring Phase

The coloring phase of the LDF algorithm differs from the Jones-Plassman one only for the condition for determining if the examined node is the highest or not: indeed, before looking at random numbers, we first check if the degree of the examined node is higher or lower with respect to the degree of the connected nodes; in case the two compared degrees are identical, we need to consider random numbers and, in the eventual case of another coincidence, also the node key.

### LDF Writing Phase

The writing phase coincides exactly with that related to the Jones-Plassman algorithm, described in detail in the corresponded paragraph of the previous section.

<br />

# Development Philosophy

To begin the development of the project, we prepared small achievable objectives in incrementing complexity. The goal was to set up a good basis, such that development would not be unstructured or messy. For example we developed a file parser for the DIMACS and DIMACS10 formats, the basic data structure to store node-data, and a graph tester such that any algorithm we would end up developing could be tested (whether it colored graphs correctly). Furthermore, after the base structure was completed, we focused solely on implementing one algorithm, this was the Jones-Plassman approach to graph coloring. We began with a sequential unoptimized approach following the literature, this was to get familiarized with the algorithm. Fortunately we had the graph tester as many of the initial outputs seemed to be correct but actually produced wrongful results. After a fully working first version of the Jones-Plassman sequantial algorithm, we then looked towards optimizing it.

## Sequential Optimization Jones-Plassman

The approach we took when optimizing always began with looking at the loops the algorithm had to accomplish during its execution. We attempted to reduce these occurrences or place multiple functions in the same loop (to avoid repeating them). This was not necessarely the case with the first sequential optimization of the Jones-Plassman sequential however this didn't discourage us from looking at other potential improvements. We then looked at the various data structures present in the algorithm, in our experience this approach yielded better result. For example, the first data structures we utilized (present in the std namespace) is the *std::set*. This not only provided a better data structure but also rendered the code simpler, especially within the iterative step.

```c++
if (is_highest)
{
  std::set<int> colors_used_iteration;
  std::set_difference(colors_used_global.begin(), colors_used_global.end(), colors_used_local.begin(), colors_used_local.end(), std::inserter(colors_used_iteration, colors_used_iteration.end()));
  auto choosen = colors_used_iteration.begin();
  to_be_evaluated.insert(std::pair<int, int>(node_key_this, *choosen));
}
```
This code snippet is taken from **'sequential_second_attempt.cpp'**. This is at the end of the iterative step for a node, which the algorithm has recognized as the highest in its local area, this is denoted by the `is_highest` boolean. In the 3<sup>rd</sup>-5<sup>th</sup> line we use 3 different *std::set*: `colors_used_iteration` is the set where the smallest (first index) available color is selected, `colors_used_global` is the set where all available colors are present, `colors_used_local` is the set of all colors this node cannot be colored (as they belong to adjacent nodes). Here the *std::set* comes in very useful as the built-in function `set_difference` is not only faster of possible custom loops but also sorts the possible colors which means we can always take the first of the list in the following operation. Here is a list of reasons why we switched from *std::vector* to *std::set*:
- *std::set* is a data structure that only keeps unique values, this is useful as we used it primarely for colors which don't need to be counted, also if we were to try to insert the same color twice it would simply not add it without returning an error.
- *std::set* are sorted such that when using colors we can always take the smallest available (first index).
- *std::set* has the set_difference built-in function which compares 2 sets and outputs another sorted set.

## Sequential Spin Off Optimization

During the optimization describeded in the last step we also realized another potential area where there were overheads in processing was the selection of the color. We ask whether instead of assigning different colors to all nodes in the same iterative step was less efficient than assign the same color to all of them. This turned out to be true and a potential optimization. However it is important to mention that by doing so we are not following the Jones-Plassman algorithm anymore. This new algorithm focuses on improving the execution time disregarding completely the number of colors used. Here are the main difference between the classical approach (Jones-Plassman) **'sequential_second_attempt.cpp'** to the new algorithm **'sequential_second_spinoff_attempt.cpp'**:

- Nodes are colored all together (the same color) at the end of the iterative step, this differs from the classical approach where nodes are colored differently at the end of the iterative step.
- Instead of using **std::set** as mentioned previously we use **std::vector** to keep all colors these nodes cannot be colored (as they belong to adjacent nodes).
- The `is_highest` if condition is simplied.


# Performance tests

# Conclusions

## References

Project Link: [https://github.com/Frititati/GraphColoringQ1](https://github.com/Frititati/GraphColoringQ1)
