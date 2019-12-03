// Simulated Annealing algorithm implementation
#include <iostream> //std::cout
#include <iterator> // std::ostream_iterator
#include <numeric> // std::iota
#include <random> // std::random_device, std::mt19937
#include <algorithm> // std::shuffle
#include <cmath> // exp, pow
#include <ctime> // std::clock
#include <string> // std::string
#include <fstream> // std::fstream
#include <sstream> // std::istringstream
#include <vector> // std::vector
#include <string> // for strings
#include "simulated_annealing.h"

using namespace SA;

/* DECLARATIONS */
void init_sa(std::vector<int> &path, int** dist, int dim);
float rounder(float val, int decis);
int get_score(std::vector<int> &path, int** dist);
void print_path(std::vector<int> &path, int** dist);
int get_dim(std::string fp);
int** get_adj_matrix(std::string fp, int dim);

/* DEFINITIONS */
std::string Trial::filename_to_path(std::string name)
{
	// gets the path from the full filename
	// returns name if no separator found
	size_t idx = name.find(SEP);
	return idx != std::string::npos ? name.substr(idx) : name;
}

std::string Trial::filename_to_loc(std::string name)
{
	// gets the location from the full filename
	std::string path_name = filename_to_path(name);
	return path_name.substr(0, path_name.find(".tsp"));
}

std::string Trial::get_output_path(std::string suffix)
{
	// write solution to <instance>_<method>_<cutoff>[_<random_seed>].<suffix>
	std::string output_fp = "";

	// idempotentally create dir and add to output filepath
	if (!output_dir.empty())
	{
		output_fp += output_dir + SEP;
	}

	output_fp += filename_to_loc(input_fp) + "_LS1_" + std::to_string(sap.cutoff);

	if (sap.seed != -1)
		output_fp += "_" + std::to_string(sap.seed);

	output_fp += "." + suffix;
	return output_fp;
}

void Trial::write_solution()
{
	// write to .sol file 2 lines as follows:
	// bestscore (ex 277952)
	// bestpath comma separated (ex 0,2,96,5,3,7,8,4,1)
	std::string output_fp = get_output_path("sol");
	std::ofstream out_file(output_fp);
	out_file << bestscore << "\n";

	int dim = bestpath.size();
	for (size_t i=0; i < dim; i++)
	{
		out_file << bestpath[i] << ",";
	}
	out_file << bestpath[0];
}

void Trial::write_trace()
{
	// write to .trace file time and score of each found
	// ex: 3.45, 102
	std::string output_fp = get_output_path("trace");
	std::ofstream out_file(output_fp);
	for (size_t i=0; i < times.size(); i++)
	{
		out_file << times[i] << "," << scores[i];
		if (i != times.size())
			out_file << "\n";
	}
}

void SA::simann(Trial &trial)
{
	int dim = get_dim(trial.input_fp);
	int** dist = get_adj_matrix(trial.input_fp, dim);
	SAParams sap = trial.sap;

	std::vector<int> path(dim);
	init_sa(path, dist, dim);
	trial.bestpath = path;

	// randomize initial values
	std::srand(sap.seed);
	auto gen = std::default_random_engine(sap.seed);
	std::shuffle(path.begin(), path.end(), gen);

	int j = 0;
	int steps, neighscore, idx1, idx2, idx3;
	int priorscore = get_score(path, dist);
	trial.bestscore = priorscore;
	double p;
	double duration = 0;
	double T = priorscore, alpha = sap.alpha;

	std::clock_t start = std::clock();
	while (true)
	{
		steps = dim*(dim-1);
		// stopping conditions
		// - no score improvement for beta T values
		// - timeout
		if (duration > sap.cutoff-1)
			break;

		// perform dim*(dim-1) search steps for given T value
		while (true)
		{
			// stop after dim*(dim-1) steps
			if (steps == 0)
				break;

			// random 3-exchange
			idx1 = std::rand() % dim;
			idx2 = std::rand() % dim;
			idx3 = std::rand() % dim;
			std::swap(path[idx1], path[idx2]);
			std::swap(path[idx1], path[idx3]);
			neighscore = get_score(path, dist);

			// metropolis condition
			if (neighscore > priorscore)
			{
				p = exp((double)(priorscore-neighscore)/T);
				// double r = (double) std::rand()/RAND_MAX;
				// replace with probability p
				if ((double) std::rand()/RAND_MAX > p)
				{
					// REJECT
					std::swap(path[idx1], path[idx3]);
					std::swap(path[idx1], path[idx2]);
				}
				else
				{
					// ACCEPT
					priorscore = neighscore;
				}
			}
			else
			{
				// ACCEPT
				priorscore = neighscore;
			}

			steps--;
		}

		// check and reflect any score improvement
		duration = (double) (std::clock()-start)/CLOCKS_PER_SEC;
		if (priorscore < trial.bestscore)
		{
			trial.bestscore = priorscore;
			trial.bestpath = path;
			trial.times.push_back(rounder(duration, 2));
			trial.scores.push_back(trial.bestscore);
		}
		else
		{
			priorscore = trial.bestscore;
			path = trial.bestpath;
			j++;
		}

		// geometric cooling
		T *= alpha;
	}
}

void init_sa(std::vector<int> &path, int** dist, int dim)
{
	// naive nearest neighbor
	for (int i=0; i < dim; i++)
		path[i] = 0;

	for (int i=0; i < dim-1; i++)
	{
		int start_idx = path[i];
		int minval = RAND_MAX;
		for (int j=0; j < dim; j++)
		{
			// j not in path yet
			if (std::find(path.begin(), path.end(), j) == path.end())
			{
				// cost of j lower
				int cost = dist[start_idx][j];
				if (cost <= minval)
				{
					minval = cost;
					path[i+1] = j;
				}
			}
		}
	}
}

float rounder(float val, int decis)
{
	int tmp = (int)(val*pow(10, decis)+0.5);
	return (float)tmp/pow(10, decis);
}

int get_score(std::vector<int> &path, int** dist)
{
	int dim = path.size();
	int score = 0;
	for (int i=0; i < dim-1; i++)
		score += dist[path[i]][path[i+1]];
	score += dist[path[0]][path[dim-1]];
	return score;
}

void print_path(std::vector<int> &path, int** dist)
{
	std::cout << "path: ";
	std::copy(path.begin(), path.end(), std::ostream_iterator<int>(std::cout, " "));
	std::cout << "\n";
	std::cout << "score: " << get_score(path, dist) << "\n";
}

int get_dim(std::string fp)
{
	std::string line;
	std::ifstream tsp_inp;
	tsp_inp.open(fp);
	int count_line = 0, dim;
	if (tsp_inp.is_open())
	{
		while (std::getline(tsp_inp, line))
		{
			count_line += 1;
			if (count_line == 3)
			{
				std::istringstream iss(line);
				std::string s;
				if (iss >> s >> dim)
					break;
			}
		}
	}

	return dim;
}

int** get_adj_matrix(std::string fp, int dim)
{
	std::string line;
	std::ifstream tsp_inp;
	tsp_inp.open(fp);
	int count_line = 0, i;

	double** coord = new double*[dim];
	for (i=0; i < dim; i++)
		coord[i] = new double[2];
	i = 0;
	if (tsp_inp.is_open())
	{
		while (std::getline(tsp_inp, line))
		{
			count_line += 1;
			if ((count_line >= 6) && (count_line < (dim+6)))
			{
				std::istringstream iss(line);
				int a;
				double b, c;
				if (iss >> a >> b >> c)
				{
					coord[i][0] = b;
					coord[i][1] = c;
					i++;
				}
			}
		}
	}

	tsp_inp.close();

	int** adj = new int*[dim];
	for (i=0; i < dim; ++i)
		adj[i] = new int[dim];

	for (i = 0; i < dim; i++)
		for (int j=0; j <= i; j++)
		{
			if (i == j)
				adj[i][j] = 0;
			else
				adj[j][i] = adj[i][j] = std::round(std::sqrt((coord[i][0]-coord[j][0])*(coord[i][0]-coord[j][0])+(coord[i][1]-coord[j][1])*(coord[i][1]-coord[j][1])));
		}

	return adj;
}

// int main(int argc, char **argv)
// {
// 	// read command line arguments
//     std::string filePath = argv[2];
//     int cutoff = atoi(argv[6]);
//     std::string method = argv[4];
//     int seed;
//     if( argc == 9 )
//         seed = atoi(argv[8]);

	
// 	 * Example Usage:
// 	 * 10 trials of NYC with 5 second cutoff and 0.95 alpha
// 	 * writes solution to 'output' directory
	 
// 	SAParams sap;
// 	sap.cutoff = cutoff;
// 	sap.alpha = 0.95;
	
// 	Trial trial;
// 	sap.seed = seed;
// 	trial.sap = sap;
// 	trial.input_fp = "DATA/"+filePath;
// 	trial.verbose = true;
// 	trial.output_dir = "output";
// 	simann(trial);
// 	trial.write_solution();
// 	trial.write_trace();
	

// 	return 0;
// }
