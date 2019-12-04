// main function to run TSP using 4 different algos
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
#include <math.h>
#include <map>
#include <numeric> // std::iota
#include <random> // std::random_device, std::mt19937
#include <string>
#include <set>
#include <sstream> // for reading file
#include <stdio.h>
#include <stdlib.h>
#include <vector>


#include "bnb.h"
#include "prim_dfs.h"
#include "simulated_annealing.h"
#include "genetic_algo.h"

using namespace std;

int main(int argc, char**argv)
{
    // read command line arguments
    string filePath = argv[2];
    int cutoff = atoi(argv[6]);
    int flag = 0;
    if(cutoff==0)
    {
        exit(0);
    }
    string method = argv[4];
    int seed = 0;
    if( argc == 9 )
    {
        seed = atoi(argv[8]);
        flag = 1;
    }
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

    // making note of start time of execution
    chrono::high_resolution_clock::time_point startTime = chrono::high_resolution_clock::now();

    // find file paths
    string inputFilePath = "./DATA/" + filePath;
    string instance = filePath.substr(0, filePath.size()-4);
    string traceFilePath = "./output/" + instance + "_" + method + "_" + to_string(cutoff);
    string solFilePath = "./output/" + instance + "_" + method + "_" + to_string(cutoff);
    
    // calling the function to find number of vertices
    int V = getDim(inputFilePath);
    
    // calling function to find coordinates of vertices and adjacency matrix
    int **adj = getAdjMatrix(inputFilePath, V);

    switch(method_num)
    {
        case 0: {
                    traceFilePath = traceFilePath + ".trace";
                    solFilePath = solFilePath + ".sol";
                    TSP_BNB obj(V);
                    obj.BNB(adj, &obj.final_res, obj.visited, obj.final_path, startTime, traceFilePath, solFilePath, cutoff); 
                    break;
                }
        case 1: {  
                    traceFilePath = traceFilePath + ".trace";
                    solFilePath = solFilePath + ".sol";
                    Prim_MST mst_obj(V);
                    struct Approximation::Graph* mst = mst_obj.createGraph();
                    mst_obj.primMST(adj, mst);
                    int *dfsPath = new int[V];
                    Graph_DFS g(V, mst->array);
                    g.DFS(0, dfsPath);
                    double sumOfEdges = 0.0;
                    for (int i = 0; i < V-1; i++)
                    {
                        sumOfEdges += adj[dfsPath[i]][dfsPath[i + 1]];
                    }
                    sumOfEdges += adj[dfsPath[V - 1]][dfsPath[0]];
                    ofstream tracefile;
                    ofstream solfile;
                    // open trace file in append mode
                    tracefile.open(traceFilePath, ios_base::app);
                    solfile.open(solFilePath);
                    chrono::high_resolution_clock::time_point currTime = chrono::high_resolution_clock::now(); 
                    float diff = round((chrono::duration_cast<chrono::microseconds>(currTime - startTime).count())/10000.0)/100.0;
                    // check if cutoff time exceeded
                    if(diff>=cutoff)
                    {
                        solfile.close();
                        tracefile.close();
                        exit(0);
                    }
                    tracefile<<diff<<","<<sumOfEdges<<"\n";
                    solfile<<sumOfEdges<<"\n";
                    for(int i=0;i<V;i++)
                        solfile<<dfsPath[i]<<",";
                    solfile<<dfsPath[0];
                    solfile.close();
                    tracefile.close();
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
        case 3: {   
                    srand(seed);
                    if(flag == 1)
                    {
                        traceFilePath = traceFilePath + "_" + to_string(seed) +".trace";
                        solFilePath = solFilePath + "_" + to_string(seed) +".sol";
                    }
                    Graph * graph1 = new Graph(V);
                    for(int i=0;i<V; i++)
                        for(int j =0; j<V; j++)
                        {
                            graph1->addEdge(i,j,adj[i][j]);
                        }
                    Genetic gen(graph1, 10, 10000000, 5, true, adj,  0, startTime, traceFilePath, solFilePath, cutoff);
                    gen.run(); 
                    break;
                }
        default:{
                    cout<<"Incorrect input";
                    break;
                }
    }
    return 0;
}