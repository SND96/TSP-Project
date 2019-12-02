// code for branch and bound implementation

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

using namespace std;

// function to write new improved solutions when found
void writeToOutputFile(string traceFileName, string solFileName, int bestWeight, chrono::high_resolution_clock::time_point startTime, int curr_path[], int dim, int cutoff)
{
    ofstream tracefile;
    ofstream solfile;

    // open trace file in append mode
    tracefile.open(traceFileName, ios_base::app);
    solfile.open(solFileName);

    // find the current time
    chrono::high_resolution_clock::time_point currTime = chrono::high_resolution_clock::now(); 

    // find time difference in seconds to measure execution time rounded to 2 decimal places
    float diff = round((chrono::duration_cast<chrono::microseconds>(currTime - startTime).count())/10000.0)/100.0;

    // check if cutoff time exceeded
    if(diff>=cutoff)
    {
        solfile.close();
        tracefile.close();
        exit(0);
    }

    // write results to output files
    tracefile<<diff<<","<<bestWeight<<"\n";
    solfile<<bestWeight<<"\n";
    for(int i=0;i<dim;i++)
        solfile<<curr_path[i]<<",";
    solfile<<curr_path[0];
    solfile.close();
    tracefile.close();
}


// class TSP_BNB
// {   
//     public:
//     int N;
//     bool* visited ;
//     int* final_path ;
//     int final_res;

    // constructor for member variables
    TSP_BNB::TSP_BNB(int dim)
    {
        N = dim;
        visited = new bool[N];
        final_path = new int [N+1];
        for(int j=0;j<N;j++)
            visited[j] = false;
        final_res = INT_MAX; 
    }
    
    // function to copy current path to final path
    void TSP_BNB::copyToFinal(int curr_path[], int *final_path) 
    { 
        for (int i=0; i<N; i++) 
            final_path[i] = curr_path[i]; 
        final_path[N] = curr_path[0]; 
    } 
      
    // function to find the minimum edge cost with one vertex i 
    int TSP_BNB::firstMin(int** adj, int i) 
    { 
        int min = INT_MAX; 
        for (int k=0; k<N; k++) 
            if (adj[i][k]<min && i != k) 
                min = adj[i][k]; 
        return min; 
    } 
      
    // function to find the second minimum edge cost with one vertex i 
    int TSP_BNB::secondMin(int** adj, int i) 
    { 
        int first = INT_MAX, second = INT_MAX; 
        for (int j=0; j<N; j++) 
        { 
            if (i == j) 
                continue; 
      
            if (adj[i][j] <= first) 
            { 
                second = first; 
                first = adj[i][j]; 
            } 
            else if (adj[i][j] <= second && adj[i][j] != first) 
                second = adj[i][j]; 
        } 
        return second; 
    } 
      
    // curr_bound -> current lower bound 
    // curr_weight-> weight of the path so far 
    // level-> current level in the tree
    // curr_path[] -> current solution so far 
    void TSP_BNB::BNBRecursive(int** adj, int curr_bound, int curr_weight, int level, int curr_path[], int *final_res, bool *visited, int *final_path, chrono::high_resolution_clock::time_point startTime, string traceFilePath, string solFilePath, int cutoff) 
    { 
        // check if cutoff time reached
        int elapsed = (chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - startTime).count())/1000000.0;
        if (elapsed>=cutoff)
        {
            exit(0);
        }

        // case when we have covered all the nodes once
        if (level==N) 
        {               
            // curr_res is the weight of the solution so far
            int curr_res = curr_weight + adj[curr_path[level-1]][curr_path[0]]; 
  
            // if current result is better, update final_res and final_path
            if (curr_res < *final_res) 
            { 
                copyToFinal(curr_path, final_path); 
                *final_res = curr_res; 
                writeToOutputFile(traceFilePath, solFilePath, curr_res, startTime, curr_path, N, cutoff);
            }                
            return; 
        } 
      
        // for any other level iterate through all vertices to build the search space tree 
        for (int i=0; i<N; i++) 
        { 
            // consider next vertex if it is not the vertex itself and not visited
            if (adj[curr_path[level-1]][i] != 0 && visited[i] == false) 
            { 
                int temp = curr_bound; 
                curr_weight += adj[curr_path[level-1]][i]; 
      
                //finding curr_bound for level 2 
                if (level==1) 
                  curr_bound -= ((firstMin(adj, curr_path[level-1]) + firstMin(adj, i))/2); 
                //curr_bound for remaininig levels
                else
                  curr_bound -= ((secondMin(adj, curr_path[level-1]) + firstMin(adj, i))/2); 
      
                // curr_bound + curr_weight is the lower bound for current node 
                // if current lower bound < final_res, we need to explore the node further 
                if (curr_bound + curr_weight < *final_res) 
                { 
                    curr_path[level] = i; 
                    visited[i] = true; 
      
                    // call BNBRecursive for the next level 
                    BNBRecursive(adj, curr_bound, curr_weight, level+1, curr_path, final_res, visited, final_path, startTime, traceFilePath, solFilePath, cutoff); 
                } 
      
                // else we prune the node by resetting all changes
                curr_weight -= adj[curr_path[level-1]][i]; 
                curr_bound = temp; 
                for(int j=0;j<N;j++)
                    visited[j] = false; 
                for (int j=0; j<=level-1; j++) 
                    visited[curr_path[j]] = true; 
            } 
        } 
    } 
      
    // set first node as root, initialise curr_path, find initial bound and call the recursive fn 
    void TSP_BNB::BNB(int** adj, int *final_res, bool *visited, int *final_path, chrono::high_resolution_clock::time_point startTime, string traceFilePath, string solFilePath, int cutoff) 
    {   
        int curr_path[N+1]; 
      
        // find initial lower bound for the root node 
        // using the formula (sum of first min + second min)/2 for all edges in the graph.  
        int curr_bound = 0; 
        for(int j=0;j<N+1;j++)
             curr_path[j] = -1;
      
        //find initial bound 
        for (int i=0; i<N; i++) 
            curr_bound += (firstMin(adj, i) + secondMin(adj, i)); 
      
        // round lower bound to an integer 
        curr_bound = (curr_bound&1)? curr_bound/2 + 1 : curr_bound/2; 
      
        // mark the vertex as visited and add it to the curr_path
        visited[0] = true; 
        curr_path[0] = 0; 
      
        // call the BNB recursive function
        BNBRecursive(adj, curr_bound, 0, 1, curr_path, final_res, visited, final_path, startTime, traceFilePath, solFilePath, cutoff); 
    } 
// };

// function to find the number of vertices
int getDim(string filePath)
{
    string line;
    ifstream tsp_inp;
    tsp_inp.open(filePath);
    int count_line = 0, dim;
    if (tsp_inp.is_open())
    {
        while (getline(tsp_inp, line))
        {
            count_line += 1;
            if (count_line == 3)
            {
                istringstream iss(line);
                string s;
                if (iss >> s >> dim)
                break;
            }
        }
    }
    return dim;
}

// function to get the adjacency matrix and coordinates of vertices
int** getAdjMatrix(string filePath, int dim)
{
    string line;
    ifstream tsp_inp;
    tsp_inp.open(filePath);
    int count_line = 0, i;

    double** coord = new double*[dim];
    for (i = 0; i < dim; ++i)
        coord[i] = new double[2];
    i = 0;
    if (tsp_inp.is_open())
    {
        while (getline(tsp_inp, line))
        {
            count_line += 1;
            if ((count_line >= 6) && (count_line < (dim + 6)))
            {
                istringstream iss(line);
                int a;
                double b, c;
                if (iss >> a >> b >> c) {
                    coord[i][0] = b;
                    coord[i][1] = c;
                    i++;
                }
            }
        }
    }

    tsp_inp.close();

    int** adj = new int*[dim];

    for (i = 0; i < dim; ++i)
        adj[i] = new int[dim];

    // find euclidean distance
    for (i = 0; i < dim; i++)
        for (int j = 0; j <= i; j++) {
            if (i == j)
                adj[i][j] = 0;
            else
                adj[j][i] = adj[i][j] = round(sqrt((coord[i][0] - coord[j][0])*(coord[i][0] - coord[j][0]) + (coord[i][1] - coord[j][1])*(coord[i][1] - coord[j][1])));
        }
    return adj;
}

// int main(int argc, char**argv)
// {
//     // read command line arguments
//     string filePath = argv[2];
//     int cutoff = atoi(argv[6]);
//     cout<<cutoff;
//     string method = argv[4];
//     int seed;
//     if( argc == 9 )
//         seed = atoi(argv[8]);

//     // making note of start time of execution
//     chrono::high_resolution_clock::time_point startTime = chrono::high_resolution_clock::now();

//     // find file paths
//     string inputFilePath = "DATA/" + filePath;
//     string instance = filePath.substr(0, filePath.size()-4);
//     string traceFilePath = instance + ".trace";
//     string solFilePath = instance + ".sol";

//     // calling the function to find number of vertices
//     int V = getDim(inputFilePath);
    
//     // calling function to find coordinates of vertices and adjacency matrix
//     int **adj = getAdjMatrix(inputFilePath, V);

//     // calling BNB
//     TSP_BNB obj(V);
//     obj.BNB(adj, &obj.final_res, obj.visited, obj.final_path, startTime, traceFilePath, solFilePath, cutoff); 

//     printf("Minimum cost : %d\n", obj.final_res); 
//     printf("Path Taken : "); 
//     for (int i=0; i<=obj.N; i++) 
//         printf("%d ", obj.final_path[i]); 
//     chrono::high_resolution_clock::time_point currTime = chrono::high_resolution_clock::now();
//     float diff = (chrono::duration_cast<chrono::microseconds>(currTime-startTime).count())/1000000.0;
//     cout<<"\nTime taken in seconds:"<<diff;
//     return 0;
// }