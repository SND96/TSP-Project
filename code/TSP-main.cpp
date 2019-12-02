#include <iomanip>  // for setw() and ws
#include <string>
#include <fstream> // for file handling
#include <cstdlib>
#include <sstream> // for reading file
#include <cmath>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <climits> // for INT_MAX
#include <chrono> // for time
#include "bnb.h"
int main(int argc, char**argv)
{
    // read command line arguments
    string filePath = argv[2];
    int cutoff = atoi(argv[6]);
    string method = argv[4];
    int seed;
    if( argc == 9 )
        seed = atoi(argv[8]);

    // assigning a number to each method
    if(method=="BnB")
    {
        method_num = 0;
    }
    else if(method=="Approx")
    {
        method_num = 1;
    }
    else if(method=="LS1")
    {
        method_num = 2
    }
    else if(method=="LS2")
    {
        method_num = 3
    }
    else
    {
        method_num = 4
    }

    // making note of start time of execution
    chrono::high_resolution_clock::time_point startTime = chrono::high_resolution_clock::now();

    // find file paths
    string inputFilePath = "../DATA/" + filePath;
    string instance = filePath.substr(0, filePath.size()-4);
    string traceFilePath = "../output/" + instance + ".trace";
    string solFilePath = "../output/" + instance + ".sol";

    // calling the function to find number of vertices
    int V = getDim(inputFilePath);
    
    // calling function to find coordinates of vertices and adjacency matrix
    int **adj = getAdjMatrix(inputFilePath, V);

    switch(method_num)
    {
        case 0: TSP_BNB obj(V);
                obj.BNB(adj, &obj.final_res, obj.visited, obj.final_path, startTime, traceFilePath, solFilePath, cutoff); 
                break;
        case 1: // call approx
        case 2: // call LS1
        case 3: // call LS2
    }
    return 0;
}