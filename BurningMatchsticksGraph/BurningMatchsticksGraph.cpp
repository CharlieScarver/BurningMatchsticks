// BurningMatchsticksGraph.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <limits>
#include <string>
#include <unordered_map>
#include <cmath> 
#include <algorithm>
#include <vector>

#define INF std::numeric_limits<float>::infinity

#define INPUT_FILE "ex-2.txt"

struct Node {
    float x;
    float y;
    int index; // index in the matrix and other arrays
    float ecc = 0; // eccentricity

    Node(float x, float y, int index) {
        this->x = x;
        this->y = y;
        this->index = index;
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

int main()
{
    std::cout << "Burning matchsticks\n\n";

    std::ifstream ifs("ex-2.txt");
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
                    // Update vertex eccentricity
                    if (dist[i][j] > nodes[i].ecc) {
                        nodes[i].ecc = dist[i][j];
                    }
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

    // Answer
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
    std::cout << " ] " << std::endl;

    // Burn simulation
    // ...

    // Cleanup

    for (int i = 0; i < allocN; ++i) {
        delete[] dist[i];
    }
    delete[] dist;
}
