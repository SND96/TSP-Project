#ifndef _TSP_PRIM_DFS_H
#define _TSP_PRIM_DFS_H

using namespace std;

class Approximation{

	public:
		int V;
		int infinity;
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
		struct Graph* createGraph()
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
	

};

class Prim_MST : public Approximation
{	
	// int V;
    // int infinity;
	// A utility function to find the vertex with  
	// minimum key value, from the set of vertices  
	// not yet included in MST  
    public:
		Prim_MST(int V);

		int minKey(int *key, bool *mstSet);


		// A utility function to print the  
		// constructed MST stored in parent[]  
		void createMST(int parent[], int** graph, struct Approximation::Graph* mst);


		// Function to construct and print MST for  
		// a graph represented using adjacency  
		// matrix representation  
		void primMST(int** graph, struct Graph* mst);
		
		

};
class Graph_DFS: public Approximation
{
	int V;    // No. of vertices 

			  // Pointer to an array containing 
			  // adjacency lists 
	struct AdjList* array;
	int counterTemp;

	// A recursive function used by DFS 
	void DFSUtil(int v, bool visited[], int* dfsPath);
public:
	Graph_DFS(int V, struct AdjList* array);   // Constructor 

											// function to add an edge to graph 
											//void addEdge(int v, int w);

											// DFS traversal of the vertices 
											// reachable from v 
	void DFS(int v, int* dfsPath);
};

#endif 
