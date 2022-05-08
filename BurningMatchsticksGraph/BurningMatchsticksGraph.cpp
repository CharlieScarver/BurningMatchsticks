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

int main()
{
    std::cout << "Burning matchsticks\n\n";

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

    std::unordered_map<std::string, int> coordToIndex;
    int nextMapIndex = 0; // vertex count
    std::string keyDelim = ", ";
    char names[26] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' }; // vertex names
    std::cout << "Vertex => Name" << std::endl;

    for (int i = 0, initN = n; i < initN; i++)
    {
        int x1, y1, x2, y2; // edge coordinates and weight
        float t;
        ifs >> x1 >> y1 >> x2 >> y2 >> t;
        if (t <= 0) {
            ifs.close();
            std::cout << "Invalid stick burning time value" << std::endl;
            return 3;
        }

        // Map (x,y) tuple to vertex index
        std::string key1 = std::to_string(x1) + keyDelim + std::to_string(y1);
        if (coordToIndex.find(key1) == coordToIndex.end()) { // not found
            coordToIndex.emplace(key1, nextMapIndex);
            std::cout << key1 << "  =>  " << names[nextMapIndex] << std::endl;
            nextMapIndex++;
        }
        std::string key2 = std::to_string(x2) + keyDelim + std::to_string(y2);
        if (coordToIndex.find(key2) == coordToIndex.end()) { // not found
            coordToIndex.emplace(key2, nextMapIndex);
            std::cout << key2 << "  =>  " << names[nextMapIndex] << std::endl;
            nextMapIndex++;
        }

        int fromIndex = coordToIndex[key1];
        int toIndex = coordToIndex[key2];

        // insert zeroes on matching indexes
        dist[fromIndex][fromIndex] = 0;
        dist[toIndex][toIndex] = 0;

        // Create vertexes at intersections
        if (abs(x1-x2) == 1 && abs(y1-y2) == 1) { // only if the edge is a diagonal in the grid
            std::string reverseKey1 = std::to_string(x1) + keyDelim + std::to_string(y2);
            std::string reverseKey2 = std::to_string(x2) + keyDelim + std::to_string(y1);

            if (coordToIndex.find(reverseKey1) != coordToIndex.end() && coordToIndex.find(reverseKey2) != coordToIndex.end()) { // both found
                int fromIndex2 = coordToIndex[reverseKey1];
                int toIndex2 = coordToIndex[reverseKey2];
                float t2 = dist[fromIndex2][toIndex2];

                if (t2 != INF() && t2 != 0) { // edge exists between the reversed nodes
                    if (n + 1 >= allocN) {
                        std::cout << "No more allocated memory left!" << std::endl;
                        return 4;
                    }

                    // Create new vertex

                    // map to index
                    std::string newKey;
                    newKey = x1 < x2 ? std::to_string(x1 + 0.5f) : std::to_string(x2 + 0.5f);
                    newKey = newKey + keyDelim + (y1 < y2 ? std::to_string(y1 + 0.5f) : std::to_string(y2 + 0.5f));
                    coordToIndex.emplace(newKey, nextMapIndex);
                    std::cout << newKey << "  =>  " << names[nextMapIndex] << std::endl;

                    // alloc new row in the matrix
                    //dist[n] = new float[allocN];
                    //for (int k = 0; k < n+1; k++)
                    //{
                    //    dist[n][k] = INF();
                    //}

                    dist[nextMapIndex][nextMapIndex] = 0;
            
                    // first edge divided
                    dist[fromIndex][nextMapIndex] = t / 2;
                    dist[nextMapIndex][toIndex] = t / 2;

                    dist[nextMapIndex][fromIndex] = t / 2;
                    dist[toIndex][nextMapIndex] = t / 2;

                    // second edge divided
                    dist[fromIndex2][nextMapIndex] = t2 / 2;
                    dist[nextMapIndex][toIndex2] = t2 / 2;

                    dist[nextMapIndex][fromIndex2] = t2 / 2;
                    dist[toIndex2][nextMapIndex] = t2 / 2;

                    // increase matrix size
                    n++;
                    // increase next map index
                    nextMapIndex++;

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

    float *ecc = new float[nextMapIndex]{ 0 }; // eccentricities
    float radius = INF();
    float diameter = 0;

    std::cout << std::endl << "Floyd-Warshall algorithm" << std::endl << std::endl;
    for (int k = 0; k < nextMapIndex; k++) {
        for (int i = 0; i < nextMapIndex; i++) {
            for (int j = 0; j < nextMapIndex; j++) {
                float sum = dist[i][k] + dist[k][j];
                if (dist[i][j] > sum) {
                    dist[i][j] = sum;
                    // Update vertex eccentricity
                    if (dist[i][j] > ecc[i]) {
                        ecc[i] = dist[i][j];
                    }
                }
            }
        }
    }

    //for (int i = 0; i < nextMapIndex; i++) {
    //    // Update graph radius and diameter
    //    if (ecc[i] < radius)
    //        radius = ecc[i]; // radius = min ecc
    //    if (ecc[i] > diameter)
    //        diameter = ecc[i]; // diameter = max ecc
    //}

    std::sort(ecc, ecc + nextMapIndex);
    radius = ecc[0];
    diameter = ecc[nextMapIndex-1];

    for (int i = 0; i < nextMapIndex; i++) {
        std::cout << names[i] << "  ";
        for (int j = 0; j < nextMapIndex; j++) {
            printf("%1.1f  ", dist[i][j]);
        }
        std::cout << "    ecc(" << names[i] << ") = " << ecc[i] << std::endl;
    }
    std::cout << std::endl;

    std::cout << "radius = " << radius << std::endl;
    std::cout << "diameter = " << diameter << std::endl;

    std::cout << "central vertexes = [ ";
    for (int i = 0, count = 0; i < nextMapIndex; i++) {
        if (ecc[i] == radius) {
            std::cout << names[i];
            if (count > 0) {
                std::cout << ", ";
            }
            count++;
        }
    }
    std::cout << " ]" << std::endl;

    // Cleanup
    delete[] ecc;

    for (int i = 0; i < allocN; ++i) {
        delete[] dist[i];
    }
    delete[] dist;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
