#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <deque>
#include <stdexcept> // Include this header for std::invalid_argument and std::out_of_range

#include <queue>
#include <stack>

using namespace std;

// Configuration parsing helper function
std::string getConfigValue(const std::string& filename, const std::string& section, const std::string& key) {
    std::ifstream configFile(filename.c_str());
    std::string line;
    std::string currentSection;
    while (std::getline(configFile, line)) {
        if (line.empty() || line[0] == ';' || line[0] == '#') {
            continue;
        }
        if (line[0] == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.length() - 2);
        } else if (currentSection == section) {
            size_t equalsPos = line.find('=');
            if (equalsPos != std::string::npos) {
                std::string currentKey = line.substr(0, equalsPos);
                size_t firstNotSpace = currentKey.find_first_not_of(" \t");
                if (std::string::npos != firstNotSpace) {
                    currentKey.erase(0, firstNotSpace);
                }
                size_t lastNotSpace = currentKey.find_last_not_of(" \t");
                if (std::string::npos != lastNotSpace) {
                    currentKey.erase(lastNotSpace + 1);
                }
                if (currentKey == key) {
                    std::string value = line.substr(equalsPos + 1);
                    size_t firstNotSpaceValue = value.find_first_not_of(" \t");
                    if (std::string::npos != firstNotSpaceValue) {
                        value.erase(0, firstNotSpaceValue);
                    }
                    size_t lastNotSpaceValue = value.find_last_not_of(" \t");
                    if (std::string::npos != lastNotSpaceValue) {
                        value.erase(lastNotSpaceValue + 1);
                    }
                    return value;
                }
            }
        }
    }
    return ""; // Return empty string if key not found
}

// Helper function to parse JSON-like array string
std::vector<int> parseJsonArrayInt(const std::string& jsonArrayStr) {
    std::vector<int> result;
    if (jsonArrayStr.length() < 2 || jsonArrayStr.front() != '[' || jsonArrayStr.back() != ']') {
        return result;
    }
    std::stringstream ss(jsonArrayStr.substr(1, jsonArrayStr.length() - 2));
    std::string segment;
    while (std::getline(ss, segment, ',')) {
        try {
            result.push_back(std::stoi(segment));
        } catch (const std::invalid_argument& e) {
            std::cerr << "Warning: Invalid integer in array string: " << segment << std::endl;
        } catch (const std::out_of_range& e) {
            std::cerr << "Warning: Integer out of range in array string: " << segment << std::endl;
        }
    }
    return result;
}

// Helper function to parse JSON-like list of lists of integers
std::vector<std::vector<int>> parseJsonListOfListsInt(const std::string& jsonListOfListsStr) {
    std::vector<std::vector<int>> result;
    if (jsonListOfListsStr.length() < 2 || jsonListOfListsStr.front() != '[' || jsonListOfListsStr.back() != ']') {
        return result;
    }
    std::stringstream ssOuter(jsonListOfListsStr.substr(1, jsonListOfListsStr.length() - 2));
    std::string innerListStr;
    while (std::getline(ssOuter, innerListStr, '[')) { // Split by '[' to isolate inner lists
        if (innerListStr.empty()) continue;
        std::stringstream ssInner(innerListStr);
        std::string segment;
        std::vector<int> innerList;
        char delimiter = ',';
        // Need to handle the trailing ']' in each inner list string
        size_t closingBracketPos = innerListStr.find(']');
        std::string cleanedInnerListStr = (closingBracketPos != std::string::npos) ? innerListStr.substr(0, closingBracketPos) : innerListStr;
        std::stringstream ssCleanedInner(cleanedInnerListStr);

        while (std::getline(ssCleanedInner, segment, delimiter)) {
            size_t firstNotSpace = segment.find_first_not_of(" \t");
            if (std::string::npos != firstNotSpace) {
                segment.erase(0, firstNotSpace);
            }
            size_t lastNotSpace = segment.find_last_not_of(" \t");
            if (std::string::npos != lastNotSpace) {
                segment.erase(lastNotSpace + 1);
            }
            try {
                innerList.push_back(std::stoi(segment));
            } catch (const std::invalid_argument& e) {
                std::cerr << "Warning: Invalid integer in inner list string: " << segment << std::endl;
            } catch (const std::out_of_range& e) {
                std::cerr << "Warning: Integer out of range in inner list string: " << segment << std::endl;
            }
        }
        if (!innerList.empty()) {
            result.push_back(innerList);
        }
    }
    return result;
}

class Node {
public:
    int x;
    int y;
    bool g;       // goal position
    bool s;       // start position
    bool f;       // is it a forbidden cell
    bool visit;   // is it a visited cell
    Node* parent; // previous node for backtracking
    bool in_fringe;
    bool path;

    Node(int x, int y) : x(x), y(y), g(false), s(false), f(false), visit(false), parent(nullptr), in_fringe(false), path(false) {}

    std::string toString() const {
        return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    }
};

class GridWorld {
public:
    int w;
    int h;
    std::vector<std::vector<int>> f_coords;
    std::vector<int> s_coords;
    std::vector<int> g_coords;
    std::vector<std::vector<Node>> grid;
    std::string default_path = "config.ini";

    GridWorld() {
        // Read configuration
        w = std::stoi(getConfigValue(default_path, "CONFIGURATION", "width"));
        h = std::stoi(getConfigValue(default_path, "CONFIGURATION", "height"));
        f_coords = parseJsonListOfListsInt(getConfigValue(default_path, "CONFIGURATION", "forbidden"));
        s_coords = parseJsonArrayInt(getConfigValue(default_path, "CONFIGURATION", "start"));
        g_coords = parseJsonArrayInt(getConfigValue(default_path, "CONFIGURATION", "goal"));
        
        std::cout <<"f size:"<< f_coords.size()<<std::endl;

        reset();
        displayBoard();
    }

    void reset() {
        grid.clear();
        makeGrid();        
		setStart();
        setGoal();
        setForbid();
    }

    void makeGrid() {

        for (int i = 0; i < w; ++i) {
            std::vector<Node> column;
            grid.push_back(column);
            for (int j = 0; j < h; ++j) {
                grid[i].emplace_back(i, j);
            }
        }
    }
    

    void setStart() {
        if (s_coords.size() == 2) {
            int x = s_coords[0];
            int y = s_coords[1];

            if (x >= 0 && x < w && y >= 0 && y < h) {
                getNode(x, y).s = true;
            } else {
                std::cerr << "Warning: Invalid start coordinates in config.ini" << std::endl;
            }
        } else {
            std::cerr << "Warning: Invalid start format in config.ini" << std::endl;
        }
        
    }


    void setGoal() {
        if (g_coords.size() == 2) {
            int x = g_coords[0];
            int y = g_coords[1];

            if (x >= 0 && x < w && y >= 0 && y < h) {
                getNode(x, y).g = true;
            } else {
                std::cerr << "Warning: Invalid goal coordinates in config.ini" << std::endl;
            }
        } else {
            std::cerr << "Warning: Invalid goal format in config.ini" << std::endl;
        }
    }


    void setForbid() {
        for (const auto& coord : f_coords) {
            if (coord.size() == 2) {
                int x = coord[0];
                int y = coord[1];
                
                if (x >= 0 && x < w && y >= 0 && y < h) {
                    getNode(x, y).f = true;                    
                } else {
                    std::cerr << "Warning: Invalid forbidden coordinates in config.ini: [" << x << ", " << y << "]" << std::endl;
                }
            } else {
                std::cerr << "Warning: Invalid forbidden coordinate format in config.ini" << std::endl;
            }
        }
    }

    Node& getNode(int x, int y) {
        return grid[x][y];
    }

    void displayBoard() const {
        for (int i = h - 1; i >= 0; --i) { // Iterate rows from bottom to top
            std::cout << i << "|";
            for (int j = 0; j < w; ++j) 
			{           
                if (grid[j][i].g) 
				{
                    std::cout << "G|";
                } 
				else if (grid[j][i].s) 
				{
                    std::cout << "S|";
                } 
				else if (grid[j][i].f) 
				{
                    std::cout << "X|";
                } 
				else if (grid[j][i].path)
				{
                	std::cout << "*|";
				}
				else 
				{
                    std::cout << "_|";
                } 
            }
            cout << endl;
        }
        cout <<"  ";
        for (int j = 0; j < w; ++j) 
        	cout << j << " ";
		std::cout << std::endl;	 
    }

    

//    std::deque<Node*> getPath(Node* node) {
//        std::deque<Node*> path;
//        Node* current = node;
//        while (current != nullptr) {
//            path.push_front(current);
//            current = current->parent;
//        }
//        return path;
//    }

	
	bool validNode(int x, int y)
	{
		if (x>=w || x<0)
			return false;
		if (y>=h || y<0)
			return false;
		Node n=getNode(x, y);
		if (n.in_fringe==true)
			return false;
		if (n.f==true)
			return false;
		if (n.visit==true)
			return false;
		
		return true;
		
	}
    
    
    
bool bfs_search()
{
	queue<Node*> open; // declare a node and queue 
	int sx = s_coords[0];
	int sy = s_coords[1];
	Node* start = &grid[sx][sy]; // make a starting node
	int nodesVisited = 0;
 
    start->visit = true; // mark the start node as a vister
    open.push(start); // push the starting node into the queue
	
	while (open.empty()==false)
	{
        Node* current = open.front();  // make a current node and point 
		int cx = current->x;          // it to the front of the queue
		int cy = current->y;
        open.pop();
		
		if(current->g) //if the current node points to the goal
		{
			cout << "Bredth First Search: Goal Found" << endl << "The Solution path is...\n";
			while (current != nullptr)
			{
				cout << current ->toString() << " ";
                current -> path = true;
				current = current -> parent;
                
			}
			cout << endl << "Nodes Visited: " << nodesVisited << endl;
        }

		// if it's not the goal
		// find it's neighbors, validate and push them into the queue
		else
        {
			// find it's north node grid[cx][cy+1] and make sure it's valid
			// mark the node as the parent of the neighbor, and then mark it as visited
			// push it into the queue.				
            
            if (validNode (cx, cy + 1)) 
            {
                Node& neighbor = getNode(cx, cy +1);
                neighbor.parent = current;
                neighbor.visit = true;
                open.push(&neighbor);
                nodesVisited++;
            }

            // find it's west node grid[cx-1][cy] and make sure it's valid
			// mark the node as the parent of the neighbor, and then mark it as visited
			// push it into the queue.			

            if (validNode (cx - 1, cy)) 
            {
                Node& neighbor = getNode(cx - 1, cy);
                neighbor.parent = current;
                neighbor.visit = true;
                open.push(&neighbor);
                nodesVisited++;
            }

			// find it's south node grid[cx][cy-1] and make sure it's valid
			// mark the node as the parent of the neighbor, and then mark it as visited
			// push it into the queue.			

            if (validNode (cx , cy - 1)) 
            {
                Node& neighbor = getNode(cx, cy - 1);
                neighbor.parent = current;
                neighbor.visit = true;
                open.push(&neighbor);
                nodesVisited++;
            }

			// find it's east node grid[cx+1][cy] and make sure it's valid
			// mark the node as the parent of the neighbor, and then mark it as visited
			// push it into the queue.			
				
            if (validNode (cx + 1, cy)) 
            {
                Node& neighbor = getNode(cx + 1, cy);
                neighbor.parent = current;
                neighbor.visit = true;
                open.push(&neighbor);
                nodesVisited++;
            }
		}
	}	
}


bool dfs_search()
{
	stack<Node*> open; // declare a stack node 
	int sx = s_coords[0];
	int sy = s_coords[1];
	Node* start = &grid[sx][sy]; // make a starting node
	int nodesVisited = 0;
	
	start -> visit = true; // mark start as visited
	open.push(start);      // push start on the stack
	
	while (open.empty() == false)
	{
		Node* CN = open.top(); //remove and return the first element from open as current search node
		open.pop();
		
		int cx = CN -> x;
		int cy = CN -> y;
		
		CN -> visit= true;
		
		if(CN->g) //if the current node points to the goal
		{
			cout << "Depth First Search: Goal Found" << endl << "The Solution path is...\n";
			while (CN != nullptr)
			{
				cout << CN ->toString() << " ";
				CN -> path = true;
	            CN = CN -> parent;
			}
				cout << endl << "Nodes Visited: " << nodesVisited << endl;
				return true;
	    }
			
		// add all of the current node's neightbors to 
		// the stack with the order east, south, west and north 
		// node east = CN's east neighbor node
		
		
		if(validNode(cx + 1, cy)) // if east is valid
		{
			Node& neighbor = getNode(cx + 1, cy);  // get the neighbor
			neighbor.parent = CN;			       // set the parent as current
			neighbor.visit = true;				   // mark the neighbor as visited
			open.push(&neighbor);				   //push the neighbor into the stack
			nodesVisited++;						   // Count the number of nodes visited
		}
	
		//similarlly add current's south, west, north neightbors to open if they are qualified
		
		if(validNode(cx, cy - 1)) // if south is valid
		{
			Node& neighbor = getNode(cx, cy - 1);  // get the neighbor
			neighbor.parent = CN;			       // mark the parent as current
			neighbor.visit = true;			 	   // mark the neighbor as visited
			open.push(&neighbor);				   //push the neighbor into the stack
			nodesVisited++;						   // Count the number of nodes visited
		}
		
		if(validNode(cx - 1, cy)) // if west is valid
		{
			Node& neighbor = getNode(cx - 1, cy);  // get the neighbor
			neighbor.parent = CN;			       // mark the parent as current
			neighbor.visit = true;				   // mark the neighbor as visited
			open.push(&neighbor);				   //push the neighbor into the stack
			nodesVisited++;						   // Count the number of nodes visited
		}
		
		if(validNode(cx, cy + 1)) // if north is valid
		{
			Node& neighbor = getNode(cx, cy + 1);  // get the neighbor
			neighbor.parent = CN;			   	   // mark the parent as current
			neighbor.visit = true;				   // mark the neighbor as visited
			open.push(&neighbor);				   //push the neighbor into the stack
			nodesVisited++;						   // Count the number of nodes visited
		}
	}
	return false;
}
};

int main() 
{
    GridWorld gridWorld;
	
    bool result = gridWorld.bfs_search();
    if (result == true)
    {
    	gridWorld.displayBoard();
	}
	else
	{
		cout << "Error: Goal not found\n";
	}
    cout << endl;
    cout << endl;
   
    
    gridWorld.reset(); //reset the grid
    result = gridWorld.dfs_search();
    if (result == true)
    {
    	gridWorld.displayBoard();
	}
	else
	{
		cout << "Error: Goal not found\n";
	}
    
	
	return 0;
}

