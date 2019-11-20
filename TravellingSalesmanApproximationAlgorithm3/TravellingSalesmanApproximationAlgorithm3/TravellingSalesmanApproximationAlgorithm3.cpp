// TravellingSalesmanApproximationAlgorithm3.cpp : Defines the entry point for the console application.
//

/*
#include "stdafx.h"


int main()
{
    return 0;
}
*/
#include "stdafx.h"
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
void printMST(int parent[], int** graph)
{
	cout << "Edge \tWeight\n";
	for (int i = 1; i < V; i++)
		cout << parent[i] << " - " << i << " \t" << graph[i][parent[i]] << " \n";
}

// Function to construct and print MST for  
// a graph represented using adjacency  
// matrix representation  
void primMST(int** graph)
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
	printMST(parent, graph);
}

int** getAdjMatrix(string filePath, int *dim)
{
	string line;
	ifstream tsp_inp;
	tsp_inp.open(filePath);
	int count_line = 0, i;
	if (tsp_inp.is_open())
	{
		while (getline(tsp_inp, line))
		{
			count_line += 1;
			if (count_line == 3)
			{
				istringstream iss(line);
				string s;
				if (iss >> s >> *dim)
					break;
			}
		}
	}
	double** coord = new double*[*dim];
	for (i = 0; i < *dim; ++i)
		coord[i] = new double[2];
	i = 0;
	if (tsp_inp.is_open())
	{
		while (getline(tsp_inp, line))
		{
			count_line += 1;
			if ((count_line >= 6) && (count_line < (*dim + 6)))
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
	for (i = 0; i<*dim; i++)
		printf("%d %lf %lf\n", i, coord[i][0], coord[i][1]);
	tsp_inp.close();


	int** adj = new int*[*dim];
	for (i = 0; i < *dim; ++i)
		adj[i] = new int[*dim];

	for (i = 0; i < *dim; i++)
		for (int j = 0; j <= i; j++) {
			if (i == j)
				adj[i][j] = 0;
			else
				adj[j][i] = adj[i][j] = round(sqrt((coord[i][0] - coord[j][0])*(coord[i][0] - coord[j][0]) + (coord[i][1] - coord[j][1])*(coord[i][1] - coord[j][1])));
		}
	for (i = 0; i < *dim; i++)
	{
		for (int j = 0; j<*dim; j++)
			printf("%d ", adj[i][j]);
		printf("\n");
	}
	return adj;
}

// Driver code 
int main()
{
	//get Adjacency Matrix
	string inputFilePath = "DATA/Atlanta.tsp";
	//int dim;
	int** adj = getAdjMatrix(inputFilePath, &V);
	cout << "dim: " << V <<endl;

	//V = 5;
	//double** graph = new double*[V];
	//int graph[V] = { { 0, 2, 0, 6, 0 },
	//{ 2, 0, 3, 8, 5 },
	//{ 0, 3, 0, 0, 7 },
	//{ 6, 8, 0, 0, 9 },
	//{ 0, 5, 7, 9, 0 } };

	// get minimum spanning tree
	primMST(adj);

	return 0;
}
