// BurningMatchsticksGraph.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <limits>
#include <cmath> 
#include <algorithm>
#include <vector>
#include <list>

#define INF std::numeric_limits<float>::infinity

#define INPUT_FILE "ex-12.txt"

struct Node {
    float x;
    float y;
    int index; // index in the matrix and other arrays
    float ecc = 0; // eccentricity
    float burnTime = 0; // time needed for the graph to burn if ignited from this node

    Node(float x, float y, int index) {
        this->x = x;
        this->y = y;
        this->index = index;
    }
};

struct BurningStick {
    int fromIndex;
    int toIndex;
    float timeLeft;

    BurningStick(int from, int to, float time) {
        this->fromIndex = from;
        this->toIndex = to;
        this->timeLeft = time;
    }
};

int findNode(std::vector<Node> nodes, float x, float y) {
    for (int i = 0; i < nodes.size(); i++)
    {
        if (nodes[i].x == x && nodes[i].y == y) {
            return i;
        }
    }

    return -1;
}

bool sortByEcc(Node a, Node b) {
    return a.ecc < b.ecc;
}

void printMatrix(float** matrix, int size, char* names) {
    for (int i = 0; i < size; i++) {
        std::cout << names[i] << "  ";
        for (int j = 0; j < size; j++) {
            printf("%.1f  ", matrix[i][j]);
        }
        std::cout << std::endl;
    }
}

std::list<BurningStick>::iterator findBurnByIndexes(std::list<BurningStick>& list, int fromIndex, int toIndex) {
    for (auto it = list.begin(); it != list.end(); it++)
    {
        if (it->fromIndex == fromIndex && it->toIndex == toIndex) {
            return it;
        }
    }

    return list.end();
}

int main()
{
    std::cout << "Burning matchsticks\n\n";

    // === 1. Read input and build adjancency matrix and nodes vector ===
    std::ifstream ifs(INPUT_FILE);
    if (!ifs.is_open())
    {
        std::cout << "Unable to open file" << std::endl;
        return 1;
    }

    std::string line;

    int n = 0; // number of initial edges in the graph = number of lines in the file after the first one
    ifs >> n;
    if (n <= 0) {
        ifs.close();
        std::cout << "Invalid number of sticks" << std::endl;
        return 2;
    }

    if (n >= 26) {
        ifs.close();
        std::cout << "The number of sticks has to be < 26 to assign vertex letters" << std::endl;
        return 5;
    }

    // Allocate adjacency matrix
    int allocN = n + ceil(n / 2.0f) + 1; // two edges are needed to create an intersection => max number of intersections is n / 2
    float** dist = new float* [allocN]; // dist = distances between vertexes = edge weights
    for (int i = 0; i < allocN; ++i) {
        dist[i] = new float[allocN];
        for (int j = 0; j < allocN; j++)
        {
            dist[i][j] = INF();
        }
    }

    std::vector<Node> nodes; // list of all vertexes
    std::string keyDelim = ", ";
    char names[26] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' }; // vertex names
    std::cout << "Vertex => Name" << std::endl;

    for (int i = 0, initN = n; i < initN; i++)
    {
        // Read a line of the input
        int x1, y1, x2, y2; // edge coordinates and weight
        float t;
        ifs >> x1 >> y1 >> x2 >> y2 >> t;
        if (t <= 0) {
            ifs.close();
            std::cout << "Invalid stick burning time value" << std::endl;
            return 3;
        }

        // Check if the read vertexes exist, set their index and add them to the vertex list
        int fromIndex = findNode(nodes, x1, y1);
        int toIndex = findNode(nodes, x2, y2);
        if (fromIndex == -1) {
            fromIndex = nodes.size();
            Node newNode = Node(x1, y1, fromIndex);
            std::cout << x1 << ", " << y1 << "  =>  " << names[fromIndex] << std::endl;
            nodes.push_back(newNode);
        }
        if (toIndex == -1) {
            toIndex = nodes.size();
            Node newNode = Node(x2, y2, toIndex);
            std::cout << x2 << ", " << y2 << "  =>  " << names[toIndex] << std::endl;
            nodes.push_back(newNode);
        }

        // insert zeroes on matching indexes
        dist[fromIndex][fromIndex] = 0;
        dist[toIndex][toIndex] = 0;

        // Create vertexes at intersections
        if (abs(x1 - x2) == 1 && abs(y1 - y2) == 1) { // only if the edge is a diagonal in the grid
            int fromIndex2 = findNode(nodes, x1, y2);
            int toIndex2 = findNode(nodes, x2, y1);

            if (fromIndex2 != -1 && toIndex2 != -1) { // both found
                float t2 = dist[fromIndex2][toIndex2];

                if (t2 != INF() && t2 != 0) { // edge exists between the reversed nodes
                    if (n + 1 >= allocN) {
                        std::cout << "No more allocated memory left!" << std::endl;
                        return 4;
                    }

                    // Create new vertex

                    // create new node, set index
                    std::string newKey;
                    float newX = x1 < x2 ? x1 + 0.5f : x2 + 0.5f;
                    float newY = y1 < y2 ? y1 + 0.5f : y2 + 0.5f;
                    int newIndex = nodes.size();
                    Node newNode = Node(newX, newY, newIndex);
                    nodes.push_back(newNode);
                    printf("%.1f, %.1f  =>  %c*\n", newX, newY, names[newIndex]);

                    dist[newIndex][newIndex] = 0;

                    // first edge divided
                    dist[fromIndex][newIndex] = t / 2;
                    dist[newIndex][toIndex] = t / 2;

                    dist[newIndex][fromIndex] = t / 2;
                    dist[toIndex][newIndex] = t / 2;

                    // second edge divided
                    dist[fromIndex2][newIndex] = t2 / 2;
                    dist[newIndex][toIndex2] = t2 / 2;

                    dist[newIndex][fromIndex2] = t2 / 2;
                    dist[toIndex2][newIndex] = t2 / 2;

                    // old edge removed
                    dist[fromIndex2][toIndex2] = INF();
                    dist[toIndex2][fromIndex2] = INF();

                    // increase matrix size
                    n++;

                    // skip the full edge weight insert
                    continue;
                }
            }
        }

        // insert edge weight
        dist[fromIndex][toIndex] = t;
        dist[toIndex][fromIndex] = t;
    }

    ifs.close();

    // Graph as an adjacency matrix
    //float dist[TN][TN] = {    // distances between vertexes = edge weights = burn times
    //    {0, 1, 1, 0.5f, INF(), INF()},       // a
    //    {1, 0, INF(), 5, INF(), INF()},      // b
    //    {1, INF(), 0, 5, INF(), INF()},      // c
    //    {0.5f, 5, 5, 0, 0.5f, INF()},        // d
    //    {INF(), INF(), INF(), 0.5f, 0, 1},   // e
    //    {INF(), INF(), INF(), INF(), 1, 0},  // f
    //};

    int vertexCount = nodes.size();

    // Early return if there's only two vertexes
    if (vertexCount == 2) {
        std::cout << std::endl << "=== Final answer ===" << std::endl << "Burning time = " << dist[0][1] << "s for nodes = { " << std::endl;
        std::cout << "\t" << names[0] << " = (" << nodes[0].x << "," << nodes[0].y << ")" << std::endl;
        std::cout << "\t" << names[1] << " = (" << nodes[1].x << "," << nodes[1].y << ")" << std::endl;
        std::cout << "}" << std::endl;

        return 0;
    }

    // Copy the adjacency matrix
    float** am = new float* [vertexCount]; // adjacency matrix
    for (int i = 0; i < vertexCount; ++i) {
        am[i] = new float[vertexCount];
        std::memcpy(am[i], dist[i], vertexCount * sizeof(float));
    }

    std::cout << std::endl << "Adjacency matrix" << std::endl;
    printMatrix(am, vertexCount, names);
    std::cout << std::endl;
    // ---


    // === 2. Floyd-Warshall and radiuses ===
    float radius = INF();
    float radius2 = INF();
    float diameter = 0;

    std::cout << std::endl << "Floyd-Warshall algorithm" << std::endl << std::endl;
    for (int k = 0; k < vertexCount; k++) {
        for (int i = 0; i < vertexCount; i++) {
            for (int j = 0; j < vertexCount; j++) {
                float sum = dist[i][k] + dist[k][j];
                if (dist[i][j] > sum) {
                    dist[i][j] = sum;
                }

                // Update vertex eccentricity
                if ((dist[i][j] >= sum && dist[i][j] != INF()) && dist[i][j] > nodes[i].ecc) {
                    nodes[i].ecc = dist[i][j];
                }
            }
        }
    }
    
    // Output

    for (int i = 0; i < vertexCount; i++) {
        std::cout << names[i] << "  ";
        for (int j = 0; j < vertexCount; j++) {
            printf("%.1f  ", dist[i][j]);
        }
        std::cout << "    ecc(" << names[i] << ") = " << nodes[i].ecc << std::endl;
    }
    std::cout << std::endl;


    std::sort(nodes.begin(), nodes.end(), &sortByEcc);
    radius = nodes[0].ecc;
    radius2 = nodes[1].ecc;
    diameter = nodes[vertexCount - 1].ecc;

    std::cout << "radius = " << radius << std::endl;
    std::cout << "radius2 = " << radius2 << std::endl;
    std::cout << "diameter = " << diameter << std::endl;

    // print central vertexes
    std::cout << "central vertexes = [ ";
    for (int i = 0, count = 0; i < vertexCount; i++) {
        if (nodes[i].ecc == radius) {
            std::cout << names[nodes[i].index];
            bool isInteger = floor(nodes[i].x) == nodes[i].x;
            if (!isInteger) {
                std::cout << "*";
            }
            std::cout << ", ";
            count++;
        }
    }
    std::cout << " ]                         // * = not on integer coordinates" << std::endl;

    // print central vertexes for radius2
    std::cout << "central vertexes for radius2 = [ ";
    for (int i = 0, count = 0; i < vertexCount; i++) {
        if (nodes[i].ecc == radius2) {
            std::cout << names[nodes[i].index];
            bool isInteger = floor(nodes[i].x) == nodes[i].x;
            if (!isInteger) {
                std::cout << "*";
            }
            std::cout << ", ";
            count++;
        }
    }
    std::cout << " ] " << std::endl << std::endl;

    // Gather vertexes to check (simulate burning)
    std::vector<Node> toCheck;
    std::cout << "vertexes to check = [ ";
    for (int i = 0, r = radius; i < vertexCount; i++) {
        if (nodes[i].ecc == r && floor(nodes[i].x) == nodes[i].x) {
            toCheck.push_back(nodes[i]);
            std::cout << names[nodes[i].index] << ", ";
        }

        // If no central vertexes are found on integer coords, reset the loop and search using radius2
        if (i == vertexCount - 1 && toCheck.size() == 0) {
            i = 0;
            r = radius2;
            std::cout << "(r = " << radius2 << "): ";
        }
    }
    std::cout << " ] " << std::endl << std::endl;



    // === 3. Burning simulation ===
    int nodesToCheck = toCheck.size();
    float minBurnTime = INF();
    for (int k = 0; k < nodesToCheck; k++)
    {
        int nodeIndex = toCheck[k].index;

        // Copy the adjacency matrix
        float** m = new float* [vertexCount]; // adjacency matrix
        for (int i = 0; i < vertexCount; ++i) {
            m[i] = new float[vertexCount];
            std::memcpy(m[i], am[i], vertexCount * sizeof(float));
        }

        // Burn simulation starting from a given node

        // Using a list for quick removals
        std::list<BurningStick> times; // remaining edge weights = burning times = lengths
        int timesSize = 0;

        std::vector<BurningStick> nextElems; // nodes whose neighbours will burn next
        nextElems.push_back(BurningStick(nodeIndex, nodeIndex, 0)); // the starting node

        bool newNodeReached = true;
        float totalBurnTime = 0;
        do {
            // Spread the fire
            if (newNodeReached) {
                int elems = nextElems.size();
                for (int e = 0; e < elems; e++)
                {
                    int elem = nextElems[e].toIndex;
                    for (int i = 0; i < vertexCount; i++)
                    {
                        // if not burnt out and connected
                        if (m[elem][i] != 0 && m[elem][i] != INF()) {
                            // if not burning from the this direction already
                            //if (findBurnByIndexes(times, elem, i) != times.end()) {
                                BurningStick burn = BurningStick(elem, i, m[elem][i]);
                                times.insert(times.end(), burn); // set stick on fire
                                timesSize++;
                            //}
                        }
                    }
                }
                newNodeReached = false;
                nextElems.clear(); // O(n)
            }

            // Burn the sticks
            for (auto it = times.begin(); it != times.end();)
            {
                // sticks burning from both ends should update to the latest value which could've been changed on previous iterations
                it->timeLeft = m[it->fromIndex][it->toIndex];

                if (it->timeLeft > 0) {
                    it->timeLeft -= 0.25f; // burn tick

                    m[it->fromIndex][it->toIndex] = it->timeLeft;
                    m[it->toIndex][it->fromIndex] = it->timeLeft;

                    if (it->timeLeft == 0) {
                        // Remember this node, fire will spread to its neighbours
                        newNodeReached = true;
                        nextElems.push_back(*it);

                        // printing
                        //printMatrix(m, vertexCount, names);
                        //std::cout << "Burn time = " << totalBurnTime + 0.25f << " (" << names[it->toIndex] << ")" << std::endl << std::endl;
                        //

                        int from = it->fromIndex, to = it->toIndex;
                        // delete burnt node
                        it = times.erase(it);
                        timesSize--;

                        // delete the second burning for sticks burning from both sides
                        auto beforeDuplicate = findBurnByIndexes(times, to, from);
                        if (beforeDuplicate != times.end()) {
                            auto er = times.erase(beforeDuplicate);
                            timesSize--;
                        }

                        if (timesSize == 0) {
                            break;
                        }
                    }
                    else {
                        it++;
                    }
                }
                else {
                    // std::cout << "Error while burning" << std::endl;
                    // delete leftover burnt node
                    it = times.erase(it);
                    timesSize--;
                }
            }

            if (newNodeReached) {
                // it's possible a node that already burnt out was added for spread again
                newNodeReached = false;
                int elems = nextElems.size();
                for (int e = 0; e < elems; e++) {
                    int elem = nextElems[e].toIndex;
                    for (int i = 0; i < vertexCount; i++)
                    {
                        // only count the node for spread if at least one of his neighbours has NOT burnt out
                        if (m[elem][i] != 0 && m[elem][i] != INF()) {
                            newNodeReached = true;
                            break;
                        }
                    }
                }
            }

            totalBurnTime += 0.25f;
        } while (timesSize > 0 || newNodeReached);

        toCheck[k].burnTime = totalBurnTime;

        // Update min burn time
        if (totalBurnTime < minBurnTime) {
            minBurnTime = totalBurnTime;
        }

        // Free the matrix memory
        for (int i = 0; i < vertexCount; ++i) {
            delete[] m[i];
        }
        delete[] m;

        std::cout << "Total burn time = " << totalBurnTime << "s for node " << names[nodeIndex] << " = (" << toCheck[k].x << "," << toCheck[k].y << ")" << std::endl;
    }

    // Final answer
    std::cout << std::endl << "=== Final answer ===" << std::endl << "Burning time = " << minBurnTime << "s for nodes = { " << std::endl;
    for (int i = 0; i < nodesToCheck; i++)
    {
        if (toCheck[i].burnTime == minBurnTime) {
            std::cout << "   " << names[toCheck[i].index] << " = (" << toCheck[i].x << "," << toCheck[i].y << ")" << std::endl;
        }
    }
    std::cout << "}" << std::endl;



    // === 4. Cleanup ===
    for (int i = 0; i < allocN; ++i) {
        delete[] dist[i];
        if (i < vertexCount) {
            delete[] am[i];
        }
    }
    delete[] dist;
    delete[] am;
}
