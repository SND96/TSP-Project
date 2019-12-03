// code for approximation algorithm

// #include <stdio.h>
// #include <stdlib.h>
// #include <math.h>
#include <cmath>
#include <iomanip>  // for setw() and ws
#include <string>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <string>
#include "prim_dfs.h"
using namespace std;



Prim_MST::Prim_MST(int V)
{
	this->V = V;
	infinity = 10e8;
}
int Prim_MST::minKey(int *key, bool *mstSet)
{
	// Initialize min value  
	int min = infinity, min_index;

	for (int v = 0; v < V; v++)
		if (mstSet[v] == false && key[v] < min)
			min = key[v], min_index = v;

	return min_index;
}

// A utility function to print the  
// constructed MST stored in parent[]  
void Prim_MST::createMST(int parent[], int** graph, struct Approximation::Graph* mst)
{
	for (int i = 1; i < V; i++)
	{
		// cout << parent[i] << " - " << i << " \t" << graph[i][parent[i]] << " \n";
		addEdge(mst, parent[i], i, graph[i][parent[i]]);

	}
}

// Function to construct and print MST for  
// a graph represented using adjacency  
// matrix representation  
void Prim_MST::primMST(int** graph, struct Graph* mst)
{
	// Array to store constructed MST  
	//int parent[V];
	int *parent = new int[V];

	// Key values used to pick minimum weight edge in cut  
	//int key[V];
	int *key = new int[V];

	// To represent set of vertices not yet included in MST  
	//bool mstSet[V];
	bool *mstSet = new bool[V];

	// Initialize all keys as INFINITE  
	for (int i = 0; i < V; i++)
		key[i] = infinity, mstSet[i] = false;

	// Always include first 1st vertex in MST.  
	// Make key 0 so that this vertex is picked as first vertex.  
	key[0] = 0;
	parent[0] = -1; // First node is always root of MST  

					// The MST will have V vertices  
	for (int count = 0; count < V - 1; count++)
	{
		// Pick the minimum key vertex from the  
		// set of vertices not yet included in MST  
		int u = minKey(key, mstSet);

		// Add the picked vertex to the MST Set  
		mstSet[u] = true;

		// Update key value and parent index of  
		// the adjacent vertices of the picked vertex.  
		// Consider only those vertices which are not  
		// yet included in MST  
		for (int v = 0; v < V; v++)

			// graph[u][v] is non zero only for adjacent vertices of m  
			// mstSet[v] is false for vertices not yet included in MST  
			// Update the key only if graph[u][v] is smaller than key[v]  
			if (graph[u][v] && mstSet[v] == false && graph[u][v] < key[v])
				parent[v] = u, key[v] = graph[u][v];
	}

	// print the constructed MST  
	createMST(parent, graph, mst);
}

Graph_DFS::Graph_DFS(int V, struct AdjList* array)
{	
	this->V = V;
	this->array = array;
	counterTemp = 0;
}


void Graph_DFS::DFSUtil(int v, bool visited[], int* dfsPath)
{
	// Mark the current node as visited and 
	// print it 
	visited[v] = true;
	dfsPath[counterTemp] = v;
	counterTemp++;

	// Recur for all the vertices adjacent 
	// to this vertex 

	struct AdjListNode* pCrawl = array[v].head;
	while (pCrawl != NULL)
	{
		int w = pCrawl->dest;
		if (!visited[w])
			DFSUtil(w, visited, dfsPath);
		//cout << "weight = " << pCrawl->weight << endl;
		pCrawl = pCrawl->next;


	}
}

void Graph_DFS::DFS(int v, int* dfsPath)
{
	// Mark all the vertices as not visited 
	bool *visited = new bool[V];
	for (int i = 0; i < V; i++)
		visited[i] = false;

	// Call the recursive helper function 
	// to print DFS traversal 
	DFSUtil(v, visited, dfsPath);
}

// Helper Functions to read input
// int** getAdjMatrix(string filePath, int dim)
// {
// 	string line;
// 	ifstream tsp_inp;
// 	tsp_inp.open(filePath);
// 	int count_line = 0, i;

// 	double** coord = new double*[dim];
// 	for (i = 0; i < dim; ++i)
// 		coord[i] = new double[2];
// 	i = 0;
// 	if (tsp_inp.is_open()){
// 		while (getline(tsp_inp, line)){
// 			count_line += 1;
// 			if ((count_line >= 6) && (count_line < (dim + 6))){
// 				istringstream iss(line);
// 				int a;
// 				double b, c;
// 				if (iss >> a >> b >> c) {
// 					coord[i][0] = b;
// 					coord[i][1] = c;
// 					i++;
// 				}
// 			}
// 		}
// 	}
// 	tsp_inp.close();
// 	int** adj = new int*[dim];

// 	for (i = 0; i < dim; ++i)
// 		adj[i] = new int[dim];

// 	for (i = 0; i < dim; i++)
// 		for (int j = 0; j <= i; j++) {
// 			if (i == j)
// 				adj[i][j] = 0;
// 			else
// 				adj[j][i] = adj[i][j] = round(sqrt((coord[i][0] - coord[j][0])*(coord[i][0] - coord[j][0]) + (coord[i][1] - coord[j][1])*(coord[i][1] - coord[j][1])));
// 		}

// 	return adj;
// }

// int getDim(string filePath)
// {
// 	string line;
// 	ifstream tsp_inp;
// 	tsp_inp.open(filePath);
// 	int count_line = 0, dim;
// 	if (tsp_inp.is_open())
// 	{
// 		while (getline(tsp_inp, line))
// 		{
// 			count_line += 1;
// 			if (count_line == 3)
// 			{
// 				istringstream iss(line);
// 				string s;
// 				if (iss >> s >> dim)
// 					break;
// 			}
// 		}
// 	}
// 	return dim;
// }
// // Driver code 
// int main(int argc, char**argv)
// {
//     // read command line arguments
//     string filePath = argv[2];
//     int cutoff = atoi(argv[6]);
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
//     int dim = getDim(inputFilePath);
    
//     // calling function to find coordinates of vertices and adjacency matrix
//     int **adj = getAdjMatrix(inputFilePath, dim);
// 	// cout << "Number of nodes: " << dim << endl;

// 	///Variable for adjacency list representation of MST
// 	Prim_MST mst_obj(dim);
// 	struct Approximation::Graph* mst = mst_obj.createGraph();

	
// 	mst_obj.primMST(adj, mst);


// 	int *dfsPath = new int[dim];

// 	srand(3);
// 	int startingVertex = rand()%dim;
// 	Graph_DFS g(dim, mst->array);

// 	cout << "Following is Depth First Traversal"
// 		" (starting from vertex "<<startingVertex<< endl;

// 	g.DFS(startingVertex, dfsPath);
// 	double sumOfEdges = 0.0;
// 	for (int i = 0; i < dim-1; i++)
// 	{
// 		// cout << dfsPath[i] << endl;
// 		sumOfEdges += adj[dfsPath[i]][dfsPath[i + 1]];
// 	}
// 	cout<<endl;
// 	sumOfEdges += adj[dfsPath[dim - 1]][dfsPath[0]];
// 	// double opt = 655454.0;
// 	cout << "Path Cost = " << (sumOfEdges) << endl;
// 	// cout << "Approximation Ratio = " << (sumOfEdges/opt) << endl;

// 	return 0;
// }