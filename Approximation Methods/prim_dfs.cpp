// TravellingSalesmanApproximationAlgorithm3.cpp : Defines the entry point for the console application.
//

/*
#include "stdafx.h"


int main()
{
return 0;
}
*/
#include <stdio.h>
#include <math.h>
#include <iomanip>  // for setw() and ws
#include <string>
#include <fstream>
#include <cstdlib>
#include <sstream>
// A C++ program for Prim's Minimum  
// Spanning Tree (MST) algorithm. The program is  
// for adjacency matrix representation of the graph 
#include <iostream>
#include <string>
using namespace std;

// Number of vertices in the graph  
//#define V 5  
int V;
int infinity = 10e8;

struct AdjListNode {
	int dest;
	int weight;
	struct AdjListNode* next;
};

// A structure to represent an adjacency list 
struct AdjList {
	struct AdjListNode* head; // pointer to head node of list 
};

// A structure to represent a graph. A graph is an array of adjacency lists. 
// Size of array will be V (number of vertices in graph) 
struct Graph {
	int V;
	struct AdjList* array;
};

// A utility function to create a new adjacency list node 
struct AdjListNode* newAdjListNode(int dest, int weight)
{
	struct AdjListNode* newNode = (struct AdjListNode*)malloc(sizeof(struct AdjListNode));
	newNode->dest = dest;
	newNode->weight = weight;
	newNode->next = NULL;
	return newNode;
}

// A utility function that creates a graph of V vertices 
struct Graph* createGraph(int V)
{
	struct Graph* graph = (struct Graph*)malloc(sizeof(struct Graph));
	graph->V = V;

	// Create an array of adjacency lists.  Size of array will be V 
	graph->array = (struct AdjList*)malloc(V * sizeof(struct AdjList));

	// Initialize each adjacency list as empty by making head as NULL 
	for (int i = 0; i < V; ++i)
		graph->array[i].head = NULL;

	return graph;
}

// Adds an edge to an undirected graph 
void addEdge(struct Graph* graph, int src, int dest, int weight)
{
	// Add an edge from src to dest.  A new node is added to the adjacency 
	// list of src.  The node is added at the begining 
	struct AdjListNode* newNode = newAdjListNode(dest, weight);
	newNode->next = graph->array[src].head;
	graph->array[src].head = newNode;

	// Since graph is undirected, add an edge from dest to src also 
	newNode = newAdjListNode(src, weight);
	newNode->next = graph->array[dest].head;
	graph->array[dest].head = newNode;
}
// A utility function to find the vertex with  
// minimum key value, from the set of vertices  
// not yet included in MST  
int minKey(int *key, bool *mstSet)
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
void createMST(int parent[], int** graph, struct Graph* mst)
{
	cout << "Edge \tWeight\n";
	for (int i = 1; i < V; i++)
	{
		cout << parent[i] << " - " << i << " \t" << graph[i][parent[i]] << " \n";
		addEdge(mst, parent[i], i, graph[i][parent[i]]);

	}
}

// Function to construct and print MST for  
// a graph represented using adjacency  
// matrix representation  
void primMST(int** graph, struct Graph* mst)
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

	for (i = 0; i<dim; i++)
		printf("%d %lf %lf\n", i, coord[i][0], coord[i][1]);
	tsp_inp.close();


	int** adj = new int*[dim];

	for (i = 0; i < dim; ++i)
		adj[i] = new int[dim];

	for (i = 0; i < dim; i++)
		for (int j = 0; j <= i; j++) {
			if (i == j)
				adj[i][j] = 0;
			else
				adj[j][i] = adj[i][j] = round(sqrt((coord[i][0] - coord[j][0])*(coord[i][0] - coord[j][0]) + (coord[i][1] - coord[j][1])*(coord[i][1] - coord[j][1])));
		}
	for (i = 0; i < dim; i++)
	{
		for (int j = 0; j<dim; j++)
			printf("%d ", adj[i][j]);
		printf("\n");
	}
	return adj;
}
///////////////////////////////////////////////////
class Graph2
{
	int V;    // No. of vertices 

			  // Pointer to an array containing 
			  // adjacency lists 
	struct AdjList* array;

	// A recursive function used by DFS 
	void DFSUtil(int v, bool visited[], int* dfsPath);
public:
	Graph2(int V, struct AdjList* array);   // Constructor 

											// function to add an edge to graph 
											//void addEdge(int v, int w);

											// DFS traversal of the vertices 
											// reachable from v 
	void DFS(int v, int* dfsPath);
};

Graph2::Graph2(int V, struct AdjList* array)
{
	this->V = V;
	this->array = array;
}

int counterTemp = 0;
void Graph2::DFSUtil(int v, bool visited[], int* dfsPath)
{
	// Mark the current node as visited and 
	// print it 
	visited[v] = true;
	cout << v << " ";
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

void Graph2::DFS(int v, int* dfsPath)
{
	// Mark all the vertices as not visited 
	bool *visited = new bool[V];
	for (int i = 0; i < V; i++)
		visited[i] = false;

	// Call the recursive helper function 
	// to print DFS traversal 
	DFSUtil(v, visited, dfsPath);
}
/////////////////////////////////////////////////////
// Driver code 
int main()
{

	//get Adjacency Matrix
	string inputFilePath = "DATA/Atlanta.tsp";
	V = getDim(inputFilePath);
	int** adj = getAdjMatrix(inputFilePath, V);
	cout << "dim: " << V << endl;

	///Variable for adjacency list representation of MST
	struct Graph* mst = createGraph(V);

	primMST(adj, mst);

	// cout << mst->array[3].head->weight; 
	//////////////////////////////////////////////////////////////////////
	int *dfsPath = new int[V];

		// Create a graph given in the above diagram 
		//struct AdjList* array = (struct AdjList*)malloc(V * sizeof(struct AdjList));
		int mstV = V-1;
		int startingVertex = 2;
		Graph2 g(V, mst->array);
	
		cout << "Following is Depth First Traversal"
			" (starting from vertex 2) \n";
	
		g.DFS(startingVertex, dfsPath);
		float sumOfEdges = 0.0;
		for (int i = 0; i < V-1; i++)
		{
			//cout << dfsPath[i] << endl;
			sumOfEdges += adj[i][i + 1];
		}
		sumOfEdges += adj[V - 1][0];
		float opt = 2003763.0;
		cout << "sumOfEdges = " << (sumOfEdges/opt) << endl;
	return 0;
}