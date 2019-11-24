#include <iomanip>  // for setw() and ws
#include <string>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <cmath>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <climits>

using namespace std;

class TSP_BNB
{   
    public:
    int N;
    bool* visited ;
    int* final_path ;
    int final_res;

    // constructor for member variables
    TSP_BNB(int dim)
    {
        N = dim;
        visited = new bool[N];
        final_path = new int [N+1];
        for(int j=0;j<N;j++)
            visited[j] = false;
        final_res = INT_MAX; 
    }
    
    // function to copy current path to final path
    void copyToFinal(int curr_path[], int *final_path) 
    { 
        for (int i=0; i<N; i++) 
            final_path[i] = curr_path[i]; 
        final_path[N] = curr_path[0]; 
    } 
      
    // function to find the minimum edge cost with one vertex i 
    int firstMin(int** adj, int i) 
    { 
        int min = INT_MAX; 
        for (int k=0; k<N; k++) 
            if (adj[i][k]<min && i != k) 
                min = adj[i][k]; 
        return min; 
    } 
      
    // function to find the second minimum edge cost with one vertex i 
    int secondMin(int** adj, int i) 
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
            else if (adj[i][j] <= second && 
                     adj[i][j] != first) 
                second = adj[i][j]; 
        } 
        return second; 
    } 
      
    // curr_bound -> current lower bound 
    // curr_weight-> weight of the path so far 
    // level-> current level in the tree
    // curr_path[] -> current solution so far 
    void BNBRecursive(int** adj, int curr_bound, int curr_weight, int level, int curr_path[], int *final_res, bool *visited, int *final_path) 
    { 
        // case when we have covered all the nodes once
        if (level==N) 
        {               
            // curr_res is the weight of the solution so far
            int curr_res = curr_weight + adj[curr_path[level-1]][curr_path[0]]; 
  
            //if current result is better, update final_res and final_path
            if (curr_res < *final_res) 
            { 
                copyToFinal(curr_path, final_path); 
                *final_res = curr_res; 
            }                
            return; 
        } 
      
        // for any other level iterate through all vertices to build the search space tree 
        for (int i=0; i<N; i++) 
        { 
            // consider next vertex if it is not the vertex itself and not visited
            if (adj[curr_path[level-1]][i] != 0 && 
                visited[i] == false) 
            { 
                int temp = curr_bound; 
                curr_weight += adj[curr_path[level-1]][i]; 
      
                //finding curr_bound for level 2 
                if (level==1) 
                  curr_bound -= ((firstMin(adj, curr_path[level-1]) + 
                                 firstMin(adj, i))/2); 
                //curr_bound for remaininig levels
                else
                  curr_bound -= ((secondMin(adj, curr_path[level-1]) + 
                                 firstMin(adj, i))/2); 
      
                // curr_bound + curr_weight is the lower bound for current node 
                // if current lower bound < final_res, we need to explore the node further 
                if (curr_bound + curr_weight < *final_res) 
                { 
                    curr_path[level] = i; 
                    visited[i] = true; 
      
                    // call BNBRecursive for the next level 
                    BNBRecursive(adj, curr_bound, curr_weight, level+1, 
                           curr_path, final_res, visited, final_path); 
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
    void BNB(int** adj, int *final_res, bool *visited, int *final_path) 
    {   
        int curr_path[N+1]; 
      
        // find initial lower bound for the root node 
        // using the formula (sum of first min + second min)/2 for all edges in the graph.  
        int curr_bound = 0; 
        for(int j=0;j<N+1;j++)
             curr_path[j] = -1;
      
        //find initial bound 
        for (int i=0; i<N; i++) 
            curr_bound += (firstMin(adj, i) + 
                           secondMin(adj, i)); 
      
        // round lower bound to an integer 
        curr_bound = (curr_bound&1)? curr_bound/2 + 1 : 
                                     curr_bound/2; 
      
        // mark the vertex as visited and add it to the curr_path
        visited[0] = true; 
        curr_path[0] = 0; 
      
        // call the BNB recursive function
        BNBRecursive(adj, curr_bound, 0, 1, curr_path, final_res, visited, final_path); 
    } 
};

// function to find adjacency matrix given total number vertices and their coordinates
int **findAdj(double **coord, int dim)
{
    // creating the adjacency matrix
    int** adj = new int*[dim];
    for (int i = 0; i < dim; ++i)
        adj[i] = new int[dim];

    // assigning values to the adjacency matrix
    for (int i = 0; i < dim; i++)
        for (int j = 0; j <= i; j++){
            // set distance to 0 for a node to itself
            if ( i == j)
                adj[i][j] = 0;
            // else use euclidean distance
            else
                adj[j][i] = adj[i][j] = round(sqrt((coord[i][0]-coord[j][0])*(coord[i][0]-coord[j][0])+(coord[i][1]-coord[j][1])*(coord[i][1]-coord[j][1]))); 
        }
    return adj;
}

// function to read the input file and find the number of vertices and return their coordinates
double ** readFile(int *dim, string file_name)
{
    string line;
    ifstream tsp_inp;
    tsp_inp.open(file_name);
    int count_line = 0, i;

    // finding the number of vertices
    if (tsp_inp.is_open())
    {
        while (getline(tsp_inp,line) )
        {
            count_line += 1;
            if(count_line == 3)
            {
                istringstream iss(line);
                string s;
                iss >> s >> *dim;
                break;
            }
        }
    }

    // creating the coordinates matrix
    double** coord = new double*[*dim];
    for(i = 0; i < *dim; ++i)
        coord[i] = new double[2];
    i=0;

    // assigning values to the coordinates matrix
    if (tsp_inp.is_open())
    {
        while (getline(tsp_inp,line) )
        {
            count_line += 1;
            if ((count_line >= 6) && (count_line < (*dim+6)))
            {
                istringstream iss(line);
                int a;
                double b,c;
                if (iss >> a >> b >> c) {
                    coord[i][0] = b;
                    coord[i][1] = c;
                    i++;
                }
            }
        }
    }

    // close the file
    tsp_inp.close();
    return coord;
}

int main()
{
    int *dim;
    string file_name = "DATA/Roanoke.tsp";
    // calling the function to find number of vertices and their coordinates
    double **coord = readFile(dim, file_name);
    
    // calling function to find adjacency matrix
    int **adj = findAdj(coord,*dim);

    //calling BNB
    TSP_BNB obj(*dim);
    obj.BNB(adj, &obj.final_res, obj.visited, obj.final_path); 

    printf("Minimum cost : %d\n", obj.final_res); 
    printf("Path Taken : "); 
    for (int i=0; i<=obj.N; i++) 
        printf("%d ", obj.final_path[i]); 
    return 0;
}