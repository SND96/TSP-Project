#include <algorithm> // std::shuffle
#include <chrono> // for time
#include <climits> // for INT_MAX
#include <cmath>
#include <cstring>
#include <cstdio>
#include <ctime> // std::clock
#include <cstdlib>
#include <fstream> // for file handling
#include <iostream>
#include <iomanip>  // for setw() and ws
#include <iterator> // std::ostream_iterator
#include <numeric> // std::iota
#include <random> // std::random_device, std::mt19937
#include <string>
#include <sstream> // for reading file
#include <vector>

#include "bnb.h"
#include "simulated_annealing.h"

using namespace std;

int main(int argc, char**argv)
{
    // read command line arguments
    string filePath = argv[2];
    int cutoff = atoi(argv[6]);
    if(cutoff==0)
    {
        exit(0);
    }
    string method = argv[4];
    int seed = 0;
    if( argc == 9 )
        seed = atoi(argv[8]);
    int method_num;
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
        method_num = 2;
    }
    else if(method=="LS2")
    {
        method_num = 3;
    }
    else
    {
        method_num = 4;
    }

    // making note of start time of execution
    chrono::high_resolution_clock::time_point startTime = chrono::high_resolution_clock::now();

    // find file paths
    string inputFilePath = "./DATA/" + filePath;
    string instance = filePath.substr(0, filePath.size()-4);
    string traceFilePath = "./output/" + instance + "_" + method + "_" + to_string(cutoff) + ".trace";
    string solFilePath = "./output/" + instance + "_" + method + "_" + to_string(cutoff) + ".sol";

    // calling the function to find number of vertices
    int V = getDim(inputFilePath);
    
    // calling function to find coordinates of vertices and adjacency matrix
    int **adj = getAdjMatrix(inputFilePath, V);

    switch(method_num)
    {
        case 0: {
                    TSP_BNB obj(V);
                    obj.BNB(adj, &obj.final_res, obj.visited, obj.final_path, startTime, traceFilePath, solFilePath, cutoff); 
                    break;
                }
        case 1: {// call approx
                    break;
                }
        case 2: {
                    SA::SAParams sap;
                    sap.cutoff = cutoff;
                    sap.alpha = 0.95;
                    SA::Trial trial;
                    sap.seed = seed;
                    trial.sap = sap;
                    trial.input_fp = "DATA/"+filePath;
                    trial.verbose = true;
                    trial.output_dir = "output";
                    simann(trial);
                    trial.write_solution();
                    trial.write_trace();
                    break;
                }
        case 3: {// call LS2
                    break;
                }
        case 4: {
                    cout<<"Incorrect input";
                    break;
                }
    }
    return 0;
}