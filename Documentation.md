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
  + [Sequentials](#sequentials)
    + [JP sequential](#jp-sequential)
    + [LDF sequential](#ldf-sequential)
  + [Parallels](#parallels)
    + [JP parallel](#jp-parallel)
    + [LDF parallel](#ldf-parallel)
  + [Others](#others)
    + [JP sequential spin-off](#jp-sequential-spin-off)
    + [JP parallel v5](#jp-parallel-v5)
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

The approach we took when optimizing always began with looking at the loops the algorithm had to accomplish during its execution. We attempted to reduce these occurrences or place multiple functions in the same loop (to avoid repeating them). This was not necessarely the case with the first sequential optimization of the Jones-Plassman sequential however this didn't discourage us from looking at other potential improvements. We then looked at the various data structures present in the algorithm, in our experience this approach yielded better result. For example, the first data structures we utilized (present in the std namespace) is the **std::set**. This not only provided a better data structure but also rendered the code simpler, especially within the iterative step.

```c++
if (is_highest)
{
  std::set<int> colors_used_iteration;
  std::set_difference(colors_used_global.begin(), colors_used_global.end(), colors_used_local.begin(), colors_used_local.end(), std::inserter(colors_used_iteration, colors_used_iteration.end()));
  auto choosen = colors_used_iteration.begin();
  to_be_evaluated.insert(std::pair<int, int>(node_key_this, *choosen));
}
```
This code snippet is taken from **'sequential_second_attempt.cpp'**. This is at the end of the iterative step for a node, which the algorithm has recognized as the highest in its local area, this is denoted by the `is_highest` boolean. In the 3<sup>rd</sup>-5<sup>th</sup> line we use 3 different **std::set**: `colors_used_iteration` is the set where the smallest (first index) available color is selected, `colors_used_global` is the set where all available colors are present, `colors_used_local` is the set of all colors this node cannot be colored (as they belong to adjacent nodes). Here the **std::set** comes in very useful as the built-in function `set_difference` is not only faster of possible custom loops but also sorts the possible colors which means we can always take the first of the list in the following operation. Here is a list of reasons why we switched from **std::vector** to **std::set**:
- **std::set** is a data structure that only keeps unique values, this is useful as we used it primarely for colors which don't need to be counted, also if we were to try to insert the same color twice it would simply not add it without returning an error.
- **std::set** are sorted such that when using colors we can always take the smallest available (first index).
- **std::set** has the set_difference built-in function which compares 2 sets and outputs another sorted set.

## Sequential Spin Off Optimization

During the optimization describeded in the last step we also realized another potential area where there were overheads in processing was the selection of the color. We ask whether instead of assigning different colors to all nodes in the same iterative step was less efficient than assign the same color to all of them. This turned out to be true and a potential optimization. However it is important to mention that by doing so we are not following the Jones-Plassman algorithm anymore. This new algorithm focuses on improving the execution time disregarding completely the number of colors used. Here are the main difference between the classical approach (Jones-Plassman) **'sequential_second_attempt.cpp'** to the new algorithm **'sequential_second_spinoff_attempt.cpp'**:

- Nodes are colored all together (the same color) at the end of the iterative step, this differs from the classical approach where nodes are colored differently at the end of the iterative step.
- Instead of using **std::set** as mentioned previously we use **std::vector** to keep all colors these nodes cannot be colored (as they belong to adjacent nodes).
- The `is_highest` if condition is simplied.


# Performance tests

## Sequentials

### JP sequential

||rgg_n_2_15_s0.graph|rgg_n_2_21_s0.graph|rgg_n_2_24_s0.graph|agrocyc_dag_uniq.gra|amaze_dag_uniq.gra|anthra_dag_uniq.gra|arXiv_sub_6000-1.gra|ba10k2d.gra|ba10k5d.gra|cit-Patents.scc.gra|citeseer.scc.gra|citeseer_sub_10720.gra|citeseerx.gra|cuda.gra|ecoo_dag_uniq.gra|go_sub_6793.gra|go_uniprot.gra|human_dag_uniq.gra|kegg_dag_uniq.gra|mtbrv_dag_uniq.gra|nasa_dag_uniq.gra|pubmed_sub_9000-1.gra|uniprotenc_100m.scc.gra|uniprotenc_22m.scc.gra|v10.gra|v100.gra|v1000.gra|vchocyc_dag_uniq.gra|xmark_dag_uniq.gra|yago_sub_6642.gra|yago_sub_6642.gra|
|------|-------------------|-------------------|-------------------|--------------------|------------------|-------------------|--------------------|-----------|-----------|-------------------|----------------|----------------------|-------------|--------|-----------------|---------------|--------------|------------------|-----------------|------------------|-----------------|---------------------|-----------------------|----------------------|-------|--------|---------|--------------------|------------------|-----------------|-----------------|
|**number nodes**|32768              |2097152            |16777216           |12684               |3710              |12499              |6000                |10000      |10000      |3774768            |693947          |10720                 |6540401      |7       |12620            |6793           |6967956       |38811             |3617             |9602              |5605             |9000                 |16087295               |1595444               |10     |100     |1000     |9491                |6080              |6642             |0                |
|**number edges**|160240             |14487995           |132557200          |27314               |7894              |26654              |133414              |39992      |99950      |33037894           |624564          |88516                 |30022520     |16      |27150            |26722          |69540470      |79632             |8790             |20876             |13076            |80056                |32174586               |3190884               |50     |8172    |823024   |20690               |14102             |84784            |0-0              |
|**number color**|14-15              |19-21              |23-24              |6-6                 |4-4               |5-5                |29-29               |8-8        |12-13      |20-21              |5-5             |11-13                 |21-21        |2-2     |5-6              |7-7            |12-15         |5-6               |4-4              |5-6               |5-5              |14-14                |5-5                    |4-4                   |6-6    |78-78   |751-753  |5-5                 |4-4               |9-9              |5.8 MB           |
|**memory**|8,6 MB             |336,6 MB           |MISSING            |6.6 MB              |<0,1 MB           |7.2 MB             |6.4 MB              |6.5 MB     |7.2 MB     |1299.4 MB          |180.2 MB        |7.0 MB                |MISSING      |<0,1 MB |6.2 MB           |5.4 MB         |MISSING       |15.0 MB           |<0,1 MB          |5.4 MB            |<0,1 MB          |6.3 MB               |MISSING                |475.0 MB              |<0,1 MB|<0,1 MB |13.1 MB  |5.3 MB              |5.2 MB            |5.8 MB           |5.8 MB           |
|**read**  |125397             |9602881            |84251876           |90914               |20250             |93953              |61620               |42740      |63428      |43973800           |3820899         |71014                 |60579110     |328     |94614            |42716          |48924881      |285235            |19498            |67747             |34123            |74485                |59555278               |5385536               |356    |2233    |194349   |65951               |35524             |43957            |58021            |
|**algo**  |282090             |28286935           |266413684          |37457               |12555             |44343              |206276              |47409      |130111     |95819003           |2374545         |99761                 |119437932    |84      |40887            |30138          |273024478     |131512            |13278            |29727             |18731            |120466               |75021295               |5990443               |99     |7018    |1164150  |30467               |21425             |90579            |1946             |
|**writing**|9211               |889542             |7468202            |2653                |1146              |2816               |1542                |2604       |2301       |647713             |128279          |2604                  |1091879      |701     |2649             |1962           |1200987       |7640              |1114             |2061              |1689             |2040                 |2742549                |366801                |509    |459     |705      |2002                |1474              |1551             |105892           |
|**total time**|416698             |38779358           |358133763          |131024              |33952             |141112             |269439              |92755      |195840     |140440517          |6323723         |173380                |181108922    |1115    |138150           |74816          |323150348     |424388            |33891            |99536             |54544            |196992               |137319123              |11742781              |965    |9712    |1359206  |98421               |58424             |136089           |                 |


### LDF sequential

||rgg_n_2_15_s0.graph|rgg_n_2_21_s0.graph|rgg_n_2_24_s0.graph|agrocyc_dag_uniq.gra|amaze_dag_uniq.gra|anthra_dag_uniq.gra|arXiv_sub_6000-1.gra|ba10k2d.gra|ba10k5d.gra|cit-Patents.scc.gra|citeseer.scc.gra|citeseer_sub_10720.gra|citeseerx.gra|cuda.gra|ecoo_dag_uniq.gra|go_sub_6793.gra|go_uniprot.gra|human_dag_uniq.gra|kegg_dag_uniq.gra|mtbrv_dag_uniq.gra|nasa_dag_uniq.gra|pubmed_sub_9000-1.gra|uniprotenc_100m.scc.gra|uniprotenc_22m.scc.gra|v10.gra|v100.gra|v1000.gra|vchocyc_dag_uniq.gra|xmark_dag_uniq.gra|yago_sub_6642.gra|
|------|-------------------|-------------------|-------------------|--------------------|------------------|-------------------|--------------------|-----------|-----------|-------------------|----------------|----------------------|-------------|--------|-----------------|---------------|--------------|------------------|-----------------|------------------|-----------------|---------------------|-----------------------|----------------------|-------|--------|---------|--------------------|------------------|-----------------|
|**number nodes**|32768              |2097152            |16777216           |12684               |3710              |12499              |6000                |10000      |10000      |3774768            |693947          |10720                 |6540401      |7       |12620            |6793           |6967956       |38811             |3617             |9602              |5605             |9000                 |16087295               |1595444               |10     |100     |1000     |9491                |6080              |6642             |
|**number edges**|160240             |14487995           |132557200          |27314               |7894              |26654              |133414              |39992      |99950      |33037894           |624564          |88516                 |30022520     |16      |27150            |26722          |69540470      |79632             |8790             |20876             |13076            |80056                |32174586               |3190884               |50     |8172    |823024   |20690               |14102             |84784            |
|**number color**|13-14              |19-19              |21-22              |4-4                 |4-4               |4-4                |26-28               |4-4        |7-7        |14-15              |4-4             |9-9                   |13-13        |2-2     |4-4              |5-5            |7-7           |5-5               |3-3              |4-4               |4-4              |9-9                  |3-3                    |2-2                   |6-6    |78-78   |751-751  |4-4                 |4-4               |6-6              |
|**memory**|8,0 MB             |340.8 MB           |MISSING            |7.1 MB              |<0,1 MB           |4.3 MB             |6.4 MB              |6.4 MB     |6.9 MB     |1298.3 MB          |183.7 MB        |6.8 MB                |MISSING      |<0,1 MB |6.9 MB           |5.1 MB         |MISSING       |15.0 MB           |4.3 MB           |6.1 MB            |<0,1 MB          |6.4 MB               |MISSING                |498.1 MB              |<0,1 MB|<0,1 MB |13.4 MB  |4.8 MB              |5.1 MB            |6.0 MB           |
|**read**  |128243             |9819794            |85910028           |92807               |20474             |92525              |61195               |41657      |62778      |43522229           |3896084         |75808                 |62177725     |350     |93322            |44613          |48986801      |287268            |18487            |69388             |32771            |70615                |60080413               |5474278               |359    |2415    |199376   |71326               |36226             |45923            |
|**algo**  |405716             |42331148           |405081845          |39587               |11112             |38958              |563853              |66992      |210210     |275451370          |2197507         |157372                |179621417    |85      |40426            |36117          |45923767      |119662            |10841            |30135             |19226            |200702               |60390692               |5510356               |112    |8592    |1663873  |29687               |20259             |58021            |
|**writing**|8519               |916262             |6729962            |2709                |1205              |3273               |1842                |2482       |2602       |1618170            |194565          |3173                  |1667433      |741     |3184             |1926           |1193044       |7909              |1176             |2366              |1605             |2341                 |4127553                |421340                |511    |479     |715      |2459                |1725              |1946             |
|**total time**|542479             |53067205           |497721837          |135103              |32792             |134757             |626890              |111132     |275590     |320591770          |6288158         |236354                |243466575    |1177    |136934           |82656          |96103613      |414841            |30506            |101889            |53603            |273658               |124598658              |11405974              |984    |11487   |1863965  |103472              |58212             |105892           |


## Parallels

### JP parallel

#### **Undirectional**

|**threads**|     |rgg_n_2_15_s0.graph|rgg_n_2_21_s0.graph|rgg_n_2_24_s0.graph|
|------|------------|-------------------|-------------------|-------------------|
|      |**number nodes**|32768              |2097152            |16777216           |
|      |**number edges**|160240             |14487995           |132557200          |
|      |**number color**|14-15              |19-22              |23-25              |
|      |**memory**|8,6 MB             |336,6 MB           |MISSING            |
|2     |**read**|69779,4            |5611781,4          |48366411,8         |
|      |**algo**|156681,2           |15755491,2         |147107707,8        |
|      |**writing**|7884,8             |382762,6           |2954731,6          |
|      |**total time**|234413             |21750109,4         |198428925,4        |
|4     |**read**|36422              |2887190,2          |25380212,8         |
|      |**algo**|81392,6            |8011415,4          |75887655,8         |
|      |**writing**|7157,6             |382931,8           |2950361            |
|      |**total time**|125043,4           |11281609,4         |104218301,2        |
|8     |**read**|22501              |1683964,4          |14911203,8         |
|      |**algo**|49022,8            |4294254,4          |39458046,4         |
|      |**writing**|6868,6             |377084,8           |2998476,4          |
|      |**total time**|78446              |6355369,2          |57367795,4         |
|16    |**read**|20914,4            |1693176,4          |14657817,6         |
|      |**algo**|38062,2            |2922806,6          |27407149           |
|      |**writing**|7122,6             |407179,6           |3083282,2          |
|      |**total time**|66156              |5023235,4          |45148326,6         
<br />

#### **Directional: large**

|**threads**|           |cit-Patents.scc.gra|citeseer.scc.gra|citeseerx.gra|go_uniprot.gra|uniprotenc_100m.scc.gra|uniprotenc_22m.scc.gra|
|------|----------------|-------------------|----------------|-------------|--------------|-----------------------|----------------------|
|      |**number nodes**|3774768            |693947          |6540401      |6967956       |16087295               |1595444               |
|      |**number edges**|33037894           |624564          |30022520     |69540470      |32174586               |3190884               |
|      |**number color**|20-22              |5-6             |21-22        |11-15         |5-5                    |3-4                   |
|      |**memory**      |1299.4 MB          |180.2 MB        |MISSING      |MISSING       |MISSING                |475.0 MB              |
|2     |**read**        |42240791,6         |3698551,2       |55588620,4   |49152082,6    |58268465,4             |5319672,2             |
|      |**algo**        |53917492,2         |1288672         |75519375,2   |149730679,6   |42507416,2             |3312356,2             |
|      |**writing**     |656388             |126801          |1117879,8    |1163981,2     |2792398                |290334,2              |
|      |**total time**  |96814743,2         |5114099,2       |132225950,6  |200046817,6   |103568376,2            |8922463,8             |
|4     |**read**        |38699920,2         |3315732,8       |54281724,8   |42510181,4    |48191074,4             |4382456,2             |
|      |**algo**        |29385164,8         |731498,2        |50940746,2   |85428085,8    |20079946,6             |1935655,8             |
|      |**writing**     |657261,6           |138071,4        |1128951,2    |1181139,4     |2731445,8              |291851,4              |
|      |**total time**  |68742418,8         |4185395,6       |106351496,4  |129119476,2   |71002539,6             |6610034,6             |
|8     |**read**        |42080962,6         |4758854,4       |81646714,8   |37926065,4    |48243161,6             |4552068,2             |
|      |**algo**        |15637172,2         |433534          |31467882,8   |51551126,6    |11845750,8             |1128112,2             |
|      |**writing**     |640138,2           |131755,6        |1126722      |1200923       |2747578,6              |283681,6              |
|      |**total time**  |58358351,2         |5324208,8       |114241393,2  |90678188,6    |62836565,2             |5963933               |
|16    |**read**        |44883480           |5513810,4       |92831035,4   |38882663,2    |51119820,2             |5004974,6             |
|      |**algo**        |10563289,2         |323158,6        |27369771,2   |41493580,2    |9735351                |878263                |
|      |**writing**     |677560,4           |144142,8        |1103178,4    |1221716       |2804601                |305051,8              |
|      |**total time**  |56124402,4         |5981182         |121304055,4  |81598036,2    |63659848,4             |6188360               |


<br />

#### **Directional: small sparse**

|**threads**|     |agrocyc_dag_uniq.gra|amaze_dag_uniq.gra|anthra_dag_uniq.gra|ecoo_dag_uniq.gra|kegg_dag_uniq.gra|mtbrv_dag_uniq.gra|nasa_dag_uniq.gra|vchocyc_dag_uniq.gra|xmark_dag_uniq.gra|
|------|------------|--------------------|------------------|-------------------|-----------------|-----------------|------------------|-----------------|--------------------|------------------|
|      |number nodes|12684               |3710              |12499              |12620            |3617             |9602              |5605             |9491                |6080              |
|      |number edges|27314               |7894              |26654              |27150            |8790             |20876             |13076            |20690               |14102             |
|      |number color|5-5                 |4-4               |5-5                |5-6              |4-4              |5-6               |5-5              |5-5                 |4-5               |
|      |memory      |6.6 MB              |<0,1 MB           |7.2 MB             |6.2 MB           |<0,1 MB          |5.4 MB            |<0,1 MB          |5.3 MB              |5.2 MB            |
|2     |**read**        |88174,8             |20058             |84441,6            |87259,6          |20108,8          |64305,2           |30603,8          |64818,2             |36122,2           |
|      |**algo**        |28959               |8110,4            |31896,6            |33596,2          |8295,4           |23438,4           |11228,6          |24068,8             |13781,4           |
|      |**writing**     |3936                |1226,2            |2914,4             |2995,8           |1229             |2356,2            |1651,6           |2318,8              |1703,4            |
|      |**total time**  |121129,2            |29445,6           |119312,8           |123909,4         |29690,6          |90155,6           |43539,2          |91275,4             |51661,4           |
|4     |**read**        |86923,2             |18082,8           |87535              |88020            |16103,4          |66698,6           |30070,8          |66038,2             |34531,2           |
|      |**algo**        |18886,6             |4994,4            |16483,2            |14708,4          |4607             |19049,4           |6565,2           |16327,8             |7639,8            |
|      |**writing**     |2926,2              |1144,8            |2912               |2857,6           |1205             |2418              |1595             |2454,6              |1622,8            |
|      |**total time**  |108792,4            |24284,2           |107000,6           |105655,6         |21996,6          |88248,6           |38278,4          |84870               |43842,2           |
|8     |**read**        |138731              |24235,6           |138138,6           |140949,4         |22078,6          |102392            |36579,6          |101584,2            |46473,6           |
|      |**algo**        |13466,8             |3919,4            |10905,2            |15025,6          |3767,2           |11365,8           |4768,2           |11501,6             |5784              |
|      |**writing**     |2832,8              |1152              |2828,4             |2917,8           |1172             |2251,2            |1516,8           |2255,4              |1605,8            |
|      |**total time**  |155080,4            |29357,2           |151920,4           |158941,6         |27065            |116054,2          |42909,8          |115385              |53926             |
|16    |**read**        |164333              |26459             |161310             |161985           |25529,8          |121801,4          |43602,8          |121166              |52970,2           |
|      |**algo**        |11579,4             |4177              |10315,6            |13355,2          |4439,4           |9087,6            |4488,2           |9673,8              |5678,8            |
|      |**writing**     |3150                |1136,6            |2952,4             |2994,6           |1159,2           |2463,6            |1508,4           |2477,8              |1629              |
|      |**total time**  |179136,2            |31816,4           |174627,8           |178391,8         |31179,8          |133426            |49661            |133366,8            |60324,2           |


<br />

#### **Directional: small dense**

|**threads**|           |arXiv_sub_6000-1.gra|citeseer_sub_10720.gra|go_sub_6793.gra|pubmed_sub_9000-1.gra|yago_sub_6642.gra|
|------|----------------|--------------------|----------------------|---------------|---------------------|-----------------|
|      |**number nodes**|6000                |10720                 |6793           |9000                 |6642             |
|      |**number edges**|133414              |88516                 |26722          |80056                |84784            |
|      |**number color**|28-30               |12-13                 |7-7            |14-15                |8-9              |
|      |**memory**      |6.4 MB              |7.0 MB                |5.4 MB         |6.3 MB               |5.8 MB           |
|2     |**read**        |62357,2             |66091,8               |39288,2        |73386                |45365,2          |
|      |**algo**        |170885,8            |59065,2               |18523,4        |90609,8              |60296,6          |
|      |**writing**     |1752,8              |2622,6                |1809           |2248,4               |1838,6           |
|      |**total time**  |235063,8            |127840,2              |59672,4        |166305,6             |107563,6         |
|4     |**read**        |56017,4             |58638,4               |36964,8        |64528                |39156,6          |
|      |**algo**        |105703,4            |33111,4               |10176,4        |44886                |31126,8          |
|      |**writing**     |1699                |2645                  |1843,6         |2166,2               |1737,8           |
|      |**total time**  |163474,2            |94457,8               |49039,2        |111630,8             |72077,6          |
|8     |**read**        |54617               |63042,5               |46006,2        |79126                |37382,8          |
|      |**algo**        |71678,6             |23841,7               |7344,8         |34009,4              |25815            |
|      |**writing**     |1647,2              |2524,8                |1798,2         |2165                 |1746,4           |
|      |**total time**  |128000              |89460                 |55202,6        |115349,6             |65017,6          |
|16    |**read**        |54906,8             |63692,3               |51075,2        |90360,6              |34578,8          |
|      |**algo**        |58488               |21007,3               |6636,6         |30811,8              |24105            |
|      |**writing**     |1680,6              |2505,8                |1723,8         |2206,4               |1751             |
|      |**total time**  |115131,6            |87257,1               |59480,6        |123427,6             |60490,8          |


<br />

### LDF parallel

|**threads**|      |rgg_n_2_15_s0.graph|rgg_n_2_21_s0.graph|rgg_n_2_24_s0.graph|agrocyc_dag_uniq.gra|amaze_dag_uniq.gra|anthra_dag_uniq.gra|arXiv_sub_6000-1.gra|ba10k2d.gra|ba10k5d.gra|cit-Patents.scc.gra|citeseer.scc.gra|citeseer_sub_10720.gra|citeseerx.gra|cuda.gra|ecoo_dag_uniq.gra|go_sub_6793.gra|go_uniprot.gra|human_dag_uniq.gra|kegg_dag_uniq.gra|mtbrv_dag_uniq.gra|nasa_dag_uniq.gra|pubmed_sub_9000-1.gra|uniprotenc_100m.scc.gra|uniprotenc_22m.scc.gra|v10.gra|v100.gra|v1000.gra|vchocyc_dag_uniq.gra|xmark_dag_uniq.gra|yago_sub_6642.gra|
|------|------------|-------------------|-------------------|-------------------|--------------------|------------------|-------------------|--------------------|-----------|-----------|-------------------|----------------|----------------------|-------------|--------|-----------------|---------------|--------------|------------------|-----------------|------------------|-----------------|---------------------|-----------------------|----------------------|-------|--------|---------|--------------------|------------------|-----------------|
|      |**number nodes**|32768              |2097152            |16777216           |12684               |3710              |12499              |6000                |10000      |10000      |3774768            |693947          |10720                 |6540401      |7       |12620            |6793           |6967956       |38811             |3617             |9602              |5605             |9000                 |16087295               |1595444               |10     |100     |1000     |9491                |6080              |6642             |
|      |**number edges**|160240             |14487995           |132557200          |27314               |7894              |26654              |133414              |39992      |99950      |33037894           |624564          |88516                 |30022520     |16      |27150            |26722          |69540470      |79632             |8790             |20876             |13076            |80056                |32174586               |3190884               |50     |8172    |823024   |20690               |14102             |84784            |
|      |**number color**|13-14              |19-19              |21-22              |4-4                 |4-4               |4-4                |26-28               |4-5        |7-7        |14-15              |4-4             |9-9                   |13-13        |2-2     |4-4              |5-5            |7-7           |5-5               |3-4              |4-4               |4-4              |9-9                  |3-3                    |2-2                   |6-6    |78-78   |751-751  |4-4                 |4-4               |6-6              |
|      |**memory**      |8,0 MB             |340.8 MB           |MISSING            |7.1 MB              |<0,1 MB           |4.3 MB             |6.4 MB              |6.4 MB     |6.9 MB     |1298.3 MB          |183.7 MB        |6.8 MB                |MISSING      |<0,1 MB |6.9 MB           |5.1 MB         |MISSING       |15.0 MB           |4.3 MB           |6.1 MB            |<0,1 MB          |6.4 MB               |MISSING                |498.1 MB              |<0,1 MB|<0,1 MB |13.4 MB  |4.8 MB              |5.1 MB            |6.0 MB           |
|2|**read**        |73165              |5811938            |50826004           |86170               |20580             |86694              |63008               |45119      |65984      |42792449           |3788952         |67139                 |56971700     |678     |101479           |40941          |50405226      |266137            |19723            |65375             |31483            |72082                |60531371               |5501653               |727    |2552    |200781   |65303               |36227             |44749            |
|     |**algo**        |230083             |23977514           |228178586          |22480               |7072              |22725              |345199              |42916      |151957     |164680651          |1272455         |85311                 |120105848    |491     |23427            |21601          |25819420      |64616             |7123             |18358             |11937            |112251               |36024169               |3099262               |543    |8309    |1292775  |17801               |12864             |34699            |
|     |**writing**     |8134               |386239             |2970353            |2938                |1288              |2991               |1768                |2516       |2483       |659094             |131101          |2625                  |1115958      |676     |3060             |1799           |1234195       |8391              |1208             |2398              |1617             |2385                 |2878059                |282495                |494    |518     |761      |2379                |1712              |1751             |
|     |**total time**  |311449             |30175764           |281975016          |111646              |28999             |112470             |410040              |90610      |220518     |208132270          |5192579         |155148                |178193608    |1883    |128028           |64389          |77458941      |339215            |28100            |86211             |45117            |186804               |99433698               |8883484               |1800   |11433   |1494384  |85561               |50883             |81263            |
|4|**read**        |38321              |3051849            |26917771           |86021               |18728             |85559              |56299               |36602      |57499      |39113592           |3336890         |59599                 |55104300     |772     |87087            |35463          |43092130      |282661            |16600            |67509             |28207            |65552                |49613484               |4470693               |833    |2391    |184249   |64464               |36712             |38336            |
|     |**algo**        |115384             |12014257           |115912575          |13173               |4515              |12265              |211079              |22693      |84857      |92596115           |694210          |45105                 |68426434     |605     |12861            |12228          |15405285      |38794             |4020             |10349             |6839             |67659                |19109198               |1640216               |657    |6883    |1122381  |10244               |7761              |20471            |
|     |**writing**     |7599               |389534             |2947678            |3077                |1233              |3045               |1731                |2514       |2438       |714676             |131227          |2680                  |1123089      |420     |2928             |1834           |1192304       |7677              |1150             |2321              |1517             |2355                 |2815036                |278395                |429    |477     |782      |2249                |1658              |1764             |
|     |**total time**  |161363             |15455709           |145778117          |102322              |24532             |100921             |269174              |61867      |144850     |132424458          |4162399         |107446                |124653903    |1832    |102923           |49570          |59689792      |329196            |21812            |80225             |36600            |135618               |71537791               |6389377               |1956   |9788    |1307481  |76998               |46187             |60630            |
|8|**read**        |23894              |1843291            |16243607           |138725              |24273             |138154             |55216               |37353      |55833      |42407762           |4792381         |62481                 |81923564     |1295    |138471           |45225          |38282724      |441203            |22290            |105226            |36873            |80832                |49671348               |4501575               |1210   |2917    |176148   |103134              |46237             |36494            |
|     |**algo**        |66346              |6371130            |59079769           |9789                |3481              |8749               |149517              |14566      |51671      |48854105           |391948          |27657                 |38987740     |1068    |9607             |8511           |9702725       |23539             |2909             |8097              |5018             |45929                |10047159               |902009                |1139   |10392   |846111   |7748                |5680              |13402            |
|     |**writing**     |6884               |403902             |2998214            |2941                |1176              |2860               |1657                |2361       |2389       |697933             |128612          |2562                  |1189892      |394     |2937             |1742           |1202589       |7788              |1132             |2324              |1501             |2182                 |2795642                |288285                |444    |578     |757      |2337                |1639              |1779             |
|     |**total time**  |97178              |8618388            |78321655           |151505              |28995             |149832             |206450              |54347      |109947     |91959873           |5313036         |92756                 |122101270    |2791    |151063           |55523          |49188105      |472613            |26367            |115696            |43434            |128993               |62514222               |5691962               |2829   |13986   |1023082  |113265              |53600             |51732            |
|16|**read**        |23538              |1818888            |16437263           |163713              |26671             |162741             |54873               |35461      |51782      |45356084           |5418589         |63812                 |93257229     |2025    |165020           |50681          |39369239      |531370            |24668            |122654            |43717            |91865                |51805217               |4884095               |2156   |3827    |177683   |120577              |52905             |35397            |
|    |**algo**        |49409              |4195330            |40089376           |9741                |3535              |8698               |100036              |11518      |38028      |33508735           |276537          |22131                 |29387751     |2236    |9207             |7969           |7388098       |20094             |3354             |7598              |5059             |39717                |6957276                |667212                |2034   |14654   |722113   |7493                |5150              |11243            |
|    |**writing**     |7461               |412143             |3073655            |3070                |1222              |3113               |1822                |2373       |2430       |712297             |139218          |2496                  |1165500      |429     |3078             |1734           |1200870       |8327              |1188             |2462              |1541             |2202                 |2835817                |303161                |403    |496     |708      |2463                |1620              |1771             |
|    |**total time** |80484              |6426429            |59600362           |176579              |31473             |174605             |156800              |49407      |92296      |79577192           |5834416         |88494                 |123810554    |4727    |177358           |60429          |47958279      |559854            |29252            |132763            |50368            |133836               |61598384               |5854540               |4628   |19024   |900564   |130581              |59718             |48474            |

## Others

### JP sequential spin-off

||rgg_n_2_15_s0.graph|rgg_n_2_21_s0.graph|rgg_n_2_24_s0.graph|agrocyc_dag_uniq.gra|amaze_dag_uniq.gra|anthra_dag_uniq.gra|arXiv_sub_6000-1.gra|ba10k2d.gra|ba10k5d.gra|cit-Patents.scc.gra|citeseer.scc.gra|citeseer_sub_10720.gra|citeseerx.gra|cuda.gra|ecoo_dag_uniq.gra|go_sub_6793.gra|go_uniprot.gra|human_dag_uniq.gra|kegg_dag_uniq.gra|mtbrv_dag_uniq.gra|nasa_dag_uniq.gra|pubmed_sub_9000-1.gra|uniprotenc_100m.scc.gra|uniprotenc_22m.scc.gra|v10.gra|v100.gra|v1000.gra|vchocyc_dag_uniq.gra|xmark_dag_uniq.gra|yago_sub_6642.gra|yago_sub_6642.gra|
|------|-------------------|-------------------|-------------------|--------------------|------------------|-------------------|--------------------|-----------|-----------|-------------------|----------------|----------------------|-------------|--------|-----------------|---------------|--------------|------------------|-----------------|------------------|-----------------|---------------------|-----------------------|----------------------|-------|--------|---------|--------------------|------------------|-----------------|-----------------|
|**number nodes**|32768              |2097152            |16777216           |12684               |3710              |12499              |6000                |10000      |10000      |3774768            |693947          |10720                 |6540401      |7       |12620            |6793           |6967956       |38811             |3617             |9602              |5605             |9000                 |16087295               |1595444               |10     |100     |1000     |9491                |6080              |6642             |0-0              |
|**number edges**|160240             |14487995           |132557200          |27314               |7894              |26654              |133414              |39992      |99950      |33037894           |624564          |88516                 |30022520     |16      |27150            |26722          |69540470      |79632             |8790             |20876             |13076            |80056                |32174586               |3190884               |50     |8172    |823024   |20690               |14102             |84784            |5.8 MB           |
|**number color**|21-22              |33-36              |39-41              |8-9                 |6-6               |8-8                |91-91               |19-19      |39-41      |104-106            |6-6             |33-35                 |118-121      |2-2     |9-9              |13-13          |140-152       |8-8               |6-6              |8-8               |7-8              |54-54                |6-6                    |4-4                   |6-6    |78-78   |752-752  |8-8                 |9-9               |30-30            |5.8 MB           |
|**memory**|8,6 MB             |336,6 MB           |MISSING            |6.6 MB              |<0,1 MB           |7.2 MB             |6.4 MB              |6.5 MB     |7.2 MB     |1299.4 MB          |180.2 MB        |7.0 MB                |MISSING      |<0,1 MB |6.2 MB           |5.4 MB         |MISSING       |15.0 MB           |<0,1 MB          |5.4 MB            |<0,1 MB          |6.3 MB               |MISSING                |475.0 MB              |<0,1 MB|<0,1 MB |13.1 MB  |5.3 MB              |5.2 MB            |5.8 MB           |#DIV/0!          |
|**read**  |127795             |9488743            |83400755           |90746               |19784             |88931              |63630               |41273      |64044      |43533675           |3826208         |71129                 |60445858     |326     |88883            |45956          |49270177      |285619            |19725            |70153             |33017            |73190                |59860186               |5382289               |347    |2317    |201805   |68436               |37918             |45243            |1946             |
|**algo**  |186841             |19154952           |177320292          |23288               |6758              |21218              |147741              |31221      |89395      |72348291           |1140098         |69390                 |84948188     |188     |22343            |20626          |225417755     |63050             |7518             |18514             |10873            |84847                |39509021               |3203268               |221    |5603    |737667   |17336               |12956             |68569            |105892           |
|**writing**|7500               |380962             |2987684            |2600                |1115              |2559               |1554                |2321       |2300       |668301             |121870          |2549                  |1150641      |706     |2798             |1759           |1265720       |7531              |1162             |2092              |1389             |2178                 |2771502                |268398                |424    |355     |606      |1998                |1463              |1698             |                 |
|**total time**|322136             |29024659           |263708732          |116635              |27658             |112709             |212926              |74816      |155740     |116550269          |5088177         |143069                |146544688    |1221    |114025           |68343          |275953653     |356201            |28407            |90760             |45279            |160216               |102140710              |8853955               |993    |8276    |940078   |87771               |52339             |115511           |                 |


### JP parallel v5

|threads|            |rgg_n_2_15_s0.graph|rgg_n_2_21_s0.graph|rgg_n_2_24_s0.graph|
|------|------------|-------------------|-------------------|-------------------|
|      |**number nodes**|32768              |2097152            |16777216           |
|      |**number edges**|160240             |14487995           |132557200          |
|      |**number color**|14-15              |19-22              |23-25              |
|      |**memory**      |8,6 MB             |336,6 MB           |MISSING            |
|2     |**read**        |47067              |3715334            |33360093           |
|2     |**algo**        |124755             |12527086           |121633409          |
|2     |**writing**     |7393               |366279             |2817543            |
|2     |**total time**  |179279             |16608796           |157811119          |
|4     |**read**        |24268              |1983300            |17564924           |
|4     |**algo**        |66069              |6484678            |61584081           |
|4     |**writing**     |7314               |387856             |2846914            |
|4     |**total time**  |97710              |8855904            |81995992           |
|8     |**read**        |14966              |1199744            |10647857           |
|8     |**algo**        |42991              |3616005            |32400826           |
|8     |**writing**     |6759               |383681             |2885633            |
|8     |**total time**  |64773              |5199494            |45934384           |
|16    |**read**        |12633              |1090218            |10245078           |
|16    |**algo**        |33162              |2546612            |23572200           |
|16    |**writing**     |6585               |408586             |3024459            |
|16    |**total time**  |52438              |4045484            |36841809           |

# Conclusions

## References

Project Link: [https://github.com/Frititati/GraphColoringQ1](https://github.com/Frititati/GraphColoringQ1)
