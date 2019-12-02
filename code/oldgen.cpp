
#include <iostream>
#include <cmath>
#include <algorithm> // sort, next_permutation
#include <vector>
#include <map>
#include <set>
#include <utility> // pair
#include <time.h> // time
#include <stdlib.h> // srand, rand
#include <string>
#include <fstream>
#include <cstdlib>
#include <sstream>
using namespace std;

class Graph
{
private:
	int V; // number of vertices
	int total_edges; // total of edges
	int initial_vertex; // initial vertex
	std::map<std::pair<int, int>, int> map_edges; // map of the edges
public:
	Graph(int V); // constructor
	void addEdge(int v1, int v2, int weight); // adds a edge
	void showGraph(); // shows all the links of the graph
	void generatesGraph(); // generates a random graph
	void showInfoGraph(); // shows info of the graph
	int existsEdge(int src, int dest); // checks if exists a edge
	friend class Genetic; // to access private membres this class
};

Graph::Graph(int V) // constructor of Graph
{
	if(V < 1) // checks if number of vertexes is less than 1
	{
		cout << "Error: number of vertexes <= 0\n";
		exit(1);
	}
	
	this->V = V; // assigns the number of vertices
	this->total_edges = 0; // initially the total of edges is 0

}


void Graph::generatesGraph()
{
	vector<int> vec;
	
	// creates the vector
	for(int i = 0; i < V; i++)
		vec.push_back(i);
	
	// generates a random permutation
	random_shuffle(vec.begin(), vec.end());
	
	initial_vertex = vec[0]; // updates initial vertex
	
	int i, weight;
	for(i = 0; i <= V; i++)
	{
		weight = rand() % V + 1; // random weight in range [1,V]
		
		if(i + 1 < V)
			addEdge(vec[i], vec[i + 1], weight);
		else
		{
			// add last edge
			addEdge(vec[i], vec[0], weight);
			break;
		}
	}
	
	int limit_edges = V * (V - 1); // calculates the limit of edges
	int size_edges = rand() % (2 * limit_edges) + limit_edges;
	
	// add others edges randomly
	for(int i = 0; i < size_edges; i++)
	{
		int src = rand() % V; // random source
		int dest = rand() % V; // random destination
		weight = rand() % V + 1; // random weight in range [1,V]
		if(src != dest)
		{
			addEdge(vec[src], vec[dest], weight);
			addEdge(vec[dest], vec[src], weight);
		}
	}
}


void Graph::showInfoGraph()
{
	cout << "Showing info of graph:\n\n";
	cout << "Number of vertices: " << V;
	cout << "\nNumber of edges: " << map_edges.size() << "\n";
}


void Graph::addEdge(int src, int dest, int weight) // add a edge
{
	map_edges[make_pair(src, dest)] = weight; // adds edge in the map
}


void Graph::showGraph() // shows all connections of the graph
{	
	map<pair<int, int>, int>::iterator it;
	for(it = map_edges.begin(); it != map_edges.end(); ++it)
		cout << it->first.first << " linked to vertex " << it->first.second << " with weight " << it->second << endl;
}


int Graph::existsEdge(int src, int dest) // checks if exists a edge
{
	map<pair<int, int>,int>::iterator it = map_edges.find(make_pair(src, dest));
	
	if(it != map_edges.end())
		return it->second; // returns cost
	return -1;
}
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
private:
	void initialPopulation(); // generates the initial population
public:
	Genetic(Graph* graph, int size_population, int generations, int mutation_rate, bool show_population, int** adj_mat, int start_point); // constructor
	int isValidSolution(std::vector<int>& solution); // checks if a solution is valid
	void showPopulation(); // shows population
	void crossOver(std::vector<int>& parent1, std::vector<int>& parent2); // makes the crossover
	void insertBinarySearch(std::vector<int>& child, int total_cost); // uses binary search to insert
	void run(); // runs genetic algorithm
	int getCostBestSolution(); // returns cost of the best solution
	bool existsChromosome(const std::vector<int> & v); // checks if exists the chromosome
};

// constructor of Genetic
Genetic::Genetic(Graph* graph, int size_population, int generations, int mutation_rate, bool show_population, int** adj_mat, int start_point)
{
	if(size_population < 1) // checks if size of population is less than 1
	{
		cout << "Error: size_population < 1\n";
		exit(1);
	}
	else if(mutation_rate < 0 || mutation_rate > 100) // checks if mutation rate is less than 0
	{
		cout << "Error: mutation_rate must be >= 0 and <= 100\n";
		exit(1);
	}

	this->graph = graph;
	this->size_population = size_population;
	this->real_size_population = 0;
	this->generations = generations;
	this->mutation_rate = mutation_rate;
	this->show_population = show_population;
	this->adj_mat = adj_mat;
	this->start_point = start_point;
}



// checks if is a valid solution, then return total cost of path else return -1
int Genetic::isValidSolution(vector<int>& solution)
{
	int total_cost = 0;
	set<int> set_solution;
	
	// checks if not contains elements repeated
	for(int i = 0; i < graph->V; i++)
		set_solution.insert(solution[i]);
	
	if(set_solution.size() != (unsigned)graph->V)
		return -1;

	// checks if connections are valid
	for(int i = 0; i < graph->V; i++)
	{
		if(i + 1 <  graph->V)
		{
			int cost = adj_mat [solution[i]] [solution[i+1]];

			total_cost += cost;
		}
		else
		{
			int cost = adj_mat[solution[i]][ solution[0]];
			

			total_cost += cost;
		}
	}
	return total_cost;
}


bool Genetic::existsChromosome(const vector<int> & v)
{
	// checks if exists in the population
	for(vector<pair<vector<int>, int> >::iterator it=population.begin(); it!=population.end(); ++it)
	{
		const vector<int>& vec = (*it).first; // gets the vector
		if(equal(v.begin(), v.end(), vec.begin())) // compares vectors
			return true;
	}
	return false;
}


void Genetic::initialPopulation() // generates the initial population
{
	vector<int> parent;
	
	// inserts initial vertex in the parent
	parent.push_back(start_point);
	
	// creates the parent
	for(int i = 0; i < graph->V; i++)
	{
		if(i != start_point)
			parent.push_back(i);
	}
		
	int total_cost = isValidSolution(parent);
	
	if(total_cost != -1) // checks if the parent is valid
	{
		population.push_back(make_pair(parent, total_cost)); // inserts in the population
		real_size_population++; // increments real_size_population
	}
	
	// makes random permutations "generations" times
	for(int i = 0; i < generations; i++)
	{
		// generates a random permutation
		random_shuffle(parent.begin() + 1, parent.begin() + (rand() % (graph->V - 1) + 1));
		
		int total_cost = isValidSolution(parent); // checks if solution is valid
		
		// checks if permutation is a valid solution and if not exists
		if(total_cost != -1 && !existsChromosome(parent))
		{
			population.push_back(make_pair(parent, total_cost)); // add in population
			real_size_population++; // increments real_size_population in the unit
		}
		if(real_size_population == size_population) // checks size population
			break;
	}
	
	// checks if real_size_population is 0
	if(real_size_population == 0)
		cout << "\nEmpty initial population ;( Try again runs the algorithm...";
	else
		sort(population.begin(), population.end(), sort_pred()); // sort population
}


void Genetic::showPopulation()
{
	cout << "\nShowing solutions...\n\n";
	for(vector<pair<vector<int>, int> >::iterator it=population.begin(); it!=population.end(); ++it)
	{
		const vector<int>& vec = (*it).first; // gets the vector
		
		for(int i = 0; i < graph->V; i++)
			cout << vec[i] << " ";
		cout << start_point;
		cout << " | Cost: " << (*it).second << "\n\n";
	}
	cout << "\nPopulation size: " << real_size_population << endl;
}


// inserts in the vector using binary search
void Genetic::insertBinarySearch(vector<int>& child, int total_cost)
{
	int imin = 0;
	int imax = real_size_population - 1;
	
	while(imax >= imin)
	{
		int imid = imin + (imax - imin) / 2;
		
		if(total_cost == population[imid].second)
		{
			population.insert(population.begin() + imid, make_pair(child, total_cost));
			return;
		}
		else if(total_cost > population[imid].second)
			imin = imid + 1;
		else
			imax = imid - 1;
	}
	population.insert(population.begin() + imin, make_pair(child, total_cost));
}

void Genetic::crossOver(vector<int>& parent1, vector<int>& parent2)
{
	vector<int> child1, child2;
	
	// map of genes, checks if already are selected
	map<int, int> genes1, genes2;
	
	for(int i = 0; i < graph->V; i++)
	{
		// initially the genes not are used
		genes1[parent1[i]] = 0;
		genes2[parent2[i]] = 0;
	}
	
	// generates random points
	int point1 = rand() % (graph->V - 1) + 1;
	int point2 = rand() % (graph->V - point1) + point1;
	
	// adjusts the points if they are equal
	if(point1 == point2)
	{
		if(point1 - 1 > 1)
			point1--;
		else if(point2 + 1 < graph->V)
			point2++;
		else
		{
			// point1 or point2 ?? random...
			int point = rand() % 10 + 1; // number in the range 1 to 10
			if(point <= 5)
				point1--;
			else
				point2++;
		}
	}
	
	// generates childs
	
	// until point1, child1 receives genes of the parent1
	// and child2 receives genes of the parent2
	for(int i = 0; i < point1; i++)
	{
		// adds genes
		child1.push_back(parent1[i]);
		child2.push_back(parent2[i]);
		// marks genes
		genes1[parent1[i]] = 1;
		genes2[parent2[i]] = 1;
	}
	
	// marks remaining genes
	for(int i = point2 + 1; i < graph->V; i++)
	{
		genes1[parent1[i]] = 1;
		genes2[parent2[i]] = 1;
	}
		
	// here is the substring inverted
	// child1 receives genes of the parent2 and
	// child2 receives genes of the parent1
	for(int i = point2; i >= point1; i--)
	{
		if(genes1[parent2[i]] == 0) // if the gene is not used
		{
			child1.push_back(parent2[i]);
			genes1[parent2[i]] = 1; // marks the gene	
		}
		else
		{
			// if the gene already is used, chooses gene that is not used
			for(map<int, int>::iterator it = genes1.begin(); it != genes1.end(); ++it)
			{
				if(it->second == 0) // checks if is not used
				{
					child1.push_back(it->first);
					genes1[it->first] = 1; // marks as used
					break; // left the loop
				}
			}
		}
		
		if(genes2[parent1[i]] == 0) // if the gene is not used
		{
			child2.push_back(parent1[i]);
			genes2[parent1[i]] = 1; // marks the gene
		}
		else
		{
			// if the gene already is used, chooses gene that is not used
			for(map<int, int>::iterator it = genes2.begin(); it != genes2.end(); ++it)
			{
				if(it->second == 0) // checks if is not used
				{
					child2.push_back(it->first);
					genes2[it->first] = 1; // marks as used
					break; // left the loop
				}
			}
		}
	}
	
	// ramaining genes: child1 receives genes of the parent1
	// and child2 receives genes of the parent2
	for(int i = point2 + 1; i < graph->V; i++)
	{
		child1.push_back(parent1[i]);
		child2.push_back(parent2[i]);
	}
		
	// mutation
	int mutation = rand() % 100 + 1; // random number in [1,100]
	if(mutation <= mutation_rate) // checks if the random number <= mutation rate
	{
		// makes a mutation: change of two genes
		
		int index_gene1, index_gene2;
		index_gene1 = rand() % (graph->V - 1) + 1;
		index_gene2 = rand() % (graph->V - 1) + 1;
		
		// makes for child1
		int aux = child1[index_gene1];
		child1[index_gene1] = child1[index_gene2];
		child1[index_gene2] = aux;
		
		// makes for child2
		aux = child2[index_gene1];
		child2[index_gene1] = child2[index_gene2];
		child2[index_gene2] = aux;
	}
	
	int total_cost_child1 = isValidSolution(child1);
	int total_cost_child2 = isValidSolution(child2);
	
	// checks if is a valid solution and not exists in the population
	if(total_cost_child1 != -1 && !existsChromosome(child1))
	{
		// add child in the population
		insertBinarySearch(child1, total_cost_child1); // uses binary search to insert
		real_size_population++; // increments the real_size_population
	}
	
	// checks again...
	if(total_cost_child2 != -1 && !existsChromosome(child2))
	{
		// add child in the population
		insertBinarySearch(child2, total_cost_child2); // uses binary search to insert
		real_size_population++; // increments the real_size_population
	}
}


// runs the genetic algorithm
void Genetic::run()
{
	initialPopulation(); // gets initial population
	
	if(real_size_population == 0)
		return;

	for(int i = 0; i < generations; i++)
	{
		int  old_size_population = real_size_population;
		
		/* selects two parents (if exists) who will participate 
			of the reproduction process */
		if(real_size_population >= 2)
		{	
			if(real_size_population == 2)
			{
				// applying crossover in the parents
				crossOver(population[0].first, population[1].first);
			}
			else
			{
				// real_size_population > 2
				
				int parent1, parent2;
			
				do
				{
					// select two random parents
					parent1 = rand() % real_size_population;
					parent2 = rand() % real_size_population;
				}while(parent1 == parent2);
				
				// applying crossover in the two parents
				crossOver(population[parent1].first, population[parent2].first);
			}
			
			// gets difference to check if the population grew 
			int diff_population = real_size_population - old_size_population;
			
			if(diff_population == 2)
			{
				if(real_size_population > size_population)
				{
					// removes the two worst parents of the population
					population.pop_back();
					population.pop_back();
					
					// decrements the real_size_population in 2 units
					real_size_population -= 2;
				}
			}
			else if(diff_population == 1)
			{
				if(real_size_population > size_population)
				{
					population.pop_back(); // removes the worst parent of the population
					real_size_population--; // decrements the real_size_population in the unit
				}
			}
		} 
		else // population contains only 1 parent
		{
			// applying crossover in the parent
			crossOver(population[0].first, population[0].first);
			
			if(real_size_population > size_population)
			{
				population.pop_back(); // removes the worst parent of the population
				real_size_population--; // decrements the real_size_population in the unit
			}
		}
	}
	
	if(show_population == true)
		showPopulation(); // shows the population
	
	cout << "\nBest solution: ";
	const vector<int>& vec = population[0].first;
	for(int i = 0; i < graph->V; i++)
		cout << vec[i] << " ";
	cout << start_point;
	cout << " | Cost: " << population[0].second;
}


int Genetic::getCostBestSolution()
{
	if(real_size_population > 0)
		return population[0].second;
	return -1;
}

///////////////////////////
// Helper Functions to read input
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
	if (tsp_inp.is_open()){
		while (getline(tsp_inp, line)){
			count_line += 1;
			if ((count_line >= 6) && (count_line < (dim + 6))){
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

	for (i = 0; i < dim; i++)
		for (int j = 0; j <= i; j++) {
			if (i == j)
				adj[i][j] = 0;
			else
				adj[j][i] = adj[i][j] = round(sqrt((coord[i][0] - coord[j][0])*(coord[i][0] - coord[j][0]) + (coord[i][1] - coord[j][1])*(coord[i][1] - coord[j][1])));
		}

	return adj;
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

int main(int argc, char **argv)
{

	//get Adjacency Matrix
	string inputFilePath = "DATA/Atlanta.tsp";
	int dim = getDim(inputFilePath);
	int** adj = getAdjMatrix(inputFilePath, dim);
	cout << "Number of nodes: " << dim << endl;

	///Variable for adjacency list representation of MST
	int start = 3;

	Graph * graph1 = new Graph(dim);

	for(int i=0;i<dim; i++)
		for(int j =0; j<dim; j++)
		{
			graph1->addEdge(i,j,adj[i][j]);
		}

	Genetic gen(graph1, 10, 1000, 5, true, adj,  start);
	// struct Gen_Graph::Graph* mst = mst_obj.createGraph();

	
	const clock_t begin_time = clock(); // gets time
	gen.run(); // runs the genetic algorithm
	cout << "\n\nTime for to run the genetic algorithm: " << float(clock () - begin_time) /  CLOCKS_PER_SEC << " seconds."; // shows time in seconds
	


	return 0;
}
