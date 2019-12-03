#ifndef _TSP_GEN_H
#define _TSP_GEN_H

using namespace std;

class Graph
{	public:

	int V; // number of vertices
	int total_edges; // total of edges
	int initial_vertex; // initial vertex
	std::map<std::pair<int, int>, int> map_edges; // map of the edges
	Graph(int V); // constructor
	void addEdge(int v1, int v2, int weight); // adds a edge
	void generatesGraph(); // generates a random graph
};


typedef std::pair<std::vector<int>, int> my_pair;


struct sort_pred
{
	bool operator()(const my_pair& firstElem, const my_pair& secondElem)
	{
		return firstElem.second < secondElem.second;
	}
};

class Genetic 
{
private:
	Graph* graph; // the graph
	std::vector< my_pair > population; // each element is a pair: vector and total cost
	int size_population; // size of population
	int real_size_population; // real size population
	int generations; // amount of generations
	int mutation_rate; // mutation rate
	bool show_population; // flag to show population
	int** adj_mat;
	int start_point;
	chrono::high_resolution_clock::time_point startTime;
	int cutoff;
	string traceFilePath;
	string solFilePath;
	int current_best;

private:
	void initialPopulation(); // generates the initial population
public:
	Genetic(Graph* graph, int size_population, int generations, int mutation_rate, bool show_population, int** adj_mat, int start_point, chrono::high_resolution_clock::time_point startTime, string traceFilePath, string solFilePath, int cutoff); // constructor

	int pathCost(std::vector<int>& solution); // returns path cost
	void crossOver(std::vector<int>& parent1, std::vector<int>& parent2); // makes the crossover
	void insertBinarySearch(std::vector<int>& child, int total_cost); // uses binary search to insert
	void run(); // runs genetic algorithm
	int getCostBestSolution(); // returns cost of the best solution
	bool existsChromosome(const std::vector<int> & v); // checks if exists the chromosome
	void writeToOutputFile();
};

#endif 
