# GridWorld
John Manke
Pathfinder Project
CIS 435
Dr. Hu

GridWorld Pathfinding Project
Pathfinding code using Breadth-First search (BFS) and Depth-First search (DFS) algorithms to find a goal within a grid. The grid is controllable through an external config.ini file, and can be implemented into real life applications, such as a motorized car.

Table Of Contents 
Overview
Features
Requirements
Code Explanation
Usage
Contact

Overview: The project simulates a 2D grid world where an object has a starting cell, and must navigate to the goal, which is found using the BFS and DFS algorithms. There are forbidden cells which the object must navigate around in order to get to the goal as well. The algorithms output the path taken, as well as the amount of visited nodes in order to compare the two search algorithms with each other. The path taken to find the goal is also displayed in the output after the goal is found. 

Features
Reads a config.ini file to set up the grid to the desired conditions
Visual representation of the grid, which shows the start, goal, forbidden cells, as well as the path taken to find the goal
Uses BFS and DFS search algorithms
Keeps track of the number of visited nodes and outputs the path taken by the object to find the goal

Requirements
C++ compiler that offers at least ISO C++ 11.
CMake 3.10+
A modern C++ compiler such as DEV C++, GCC, Clang, and any other compatible software

Code Explanation
BFS: Beginning at the starting cell, the BFS search algorithm uses a queue to explore the neighboring nodes level-by-level. It puts any visited nodes into the queue, and from there is able to determine the most optimal path.
DFS: The DFS algorithm uses a stack and dives deep into a path until hitting a barricade, where it will then go over to the next logical direction towards the goal. This algorithm is not usually as efficient as BFS, although it can have its specific useful applications. 

Each algorithm keeps track of visited nodes to ensure there are no repeated cells, as well as keeping track of parent nodes to be able to backtrack and find the path taken by the algorithm at the end of the process. 

Usage
Example Configuration and Output
width = 10
height = 10 
start = [3, 1]
goal = [2, 4]
forbidden = [[1, 3], [1, 2], [1, 1], [2, 3], [3,3]]

f size:5
9|_|_|_|_|_|_|_|_|_|_|
8|_|_|_|_|_|_|_|_|_|_|
7|_|_|_|_|_|_|_|_|_|_|
6|_|_|_|_|_|_|_|_|_|_|
5|_|_|_|_|_|_|_|_|_|_|
4|_|_|G|_|_|_|_|_|_|_|
3|_|X|X|X|_|_|_|_|_|_|
2|_|X|_|_|_|_|_|_|_|_|
1|_|X|_|S|_|_|_|_|_|_|
0|_|_|_|_|_|_|_|_|_|_|
  0 1 2 3 4 5 6 7 8 9

Breadth First Search: Goal Found
The Solution path is...
(2, 4) (3, 4) (4, 4) (4, 3) (4, 2) (3, 2) (3, 1)
Nodes Visited: 43
9|_|_|_|_|_|_|_|_|_|_|
8|_|_|_|_|_|_|_|_|_|_|
7|_|_|_|_|_|_|_|_|_|_|
6|_|_|_|_|_|_|_|_|_|_|
5|_|_|_|_|_|_|_|_|_|_|
4|_|_|G|*|*|_|_|_|_|_|
3|_|X|X|X|*|_|_|_|_|_|
2|_|X|_|*|*|_|_|_|_|_|
1|_|X|_|S|_|_|_|_|_|_|
0|_|_|_|_|_|_|_|_|_|_|
  0 1 2 3 4 5 6 7 8 9


Depth First Search: Goal Found
The Solution path is...
(2, 4) (1, 4) (0, 4) (0, 5) (0, 6) (0, 7) (0, 8) (0, 9) (1, 9) (2, 9) (3, 9) (4, 9) (4, 8) (4, 7) (4, 6) (4, 5) (4, 4) (4, 3) (4, 2) (3, 2) (3, 1)
Nodes Visited: 48
9|* |* |* |* |* |_|_|_|_|_|
8|* |_|_|_|* |_|_|_|_|_|
7|* |_|_|_|* |_|_|_|_|_|
6|* |_|_|_|* |_|_|_|_|_|
5|* |_|_|_|* |_|_|_|_|_|
4|* |* |G|_|* |_|_|_|_|_|
3|_|X|X|X|* |_|_|_|_|_|
2|_|X|_|* |* |_|_|_|_|_|
1|_|X|_|S|_|_|_|_|_|_|
0|_|_|_|_|_|_|_|_|_|_|
  0 1 2 3 4 5 6 7 8 9

Contact
Email: mankejd01@buffalostate.edu
GitHub: 






