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

// DECLARATIONS
int* simann(std::string input_fp, size_t cutoff, int seed, double alpha, int beta, double T);
float rounder(float val, int decis);
int get_score(int* path, int** dist, int dim);
void print_path(int* path, int** dist, int dim);
int get_dim(std::string fp);
int** get_adj_matrix(std::string fp, int dim);
void write_solution(std::string out_fp, int* path, size_t score, int dim);
void write_trace(std::string out_fp, std::vector<float> trace_times, std::vector<size_t> trace_scores);

int* simann(std::string input_fp, size_t cutoff=30, unsigned seed = 0, double alpha=0.95, int beta=10000, double T=10)
{
	int dim = get_dim(input_fp);
	int** dist = get_adj_matrix(input_fp, dim);

	int path[dim];
	std::iota(path, path+dim, 0);

	int *bestpath = new int[dim];
	std::copy(path, path+dim, bestpath);

	// randomize initial values
	std::srand(seed);
	std::shuffle(path, path+dim, std::default_random_engine(seed));

	int j = 0;
	int steps, neighscore, idx1, idx2;
	int priorscore = get_score(path, dist, dim);
	int bestscore = priorscore;
	double p;
	double duration = 0;

	std::cout << "Initial Path:\n";
	print_path(bestpath, dist, dim);

	std::clock_t start;
	std::vector<float> trace_times;
	std::vector<size_t> trace_scores;
	while (true)
	{
		steps = dim*(dim-1);
		// stopping conditions
		// - no score improvement for beta T values
		// - timeout
		if (duration > cutoff-1)
			break;

		/// perform dim*(dim-1) search steps for given T value
		while (true)
		{
			// stop after dim*(dim-1) steps
			if (steps == 0)
				break;

			// random 2-exchange
			idx1 = std::rand() % dim;
			idx2 = std::rand() % dim;
			std::swap(path[idx1], path[idx2]);
			neighscore = get_score(path, dist, dim);

			// metropolis condition
			if (neighscore > priorscore)
			{
				p = exp((double)(priorscore-neighscore)/T);
				if ((double) std::rand()/RAND_MAX < p)
					std::swap(path[idx1], path[idx2]);
				else
					priorscore = neighscore;
			}
			else
			{
				priorscore = neighscore;
			}

			steps--;
		}

		// check and reflect any score improvement
		duration = (double) (std::clock()-start)/CLOCKS_PER_SEC;
		if (priorscore < bestscore)
		{
			bestscore = priorscore;
			std::copy(path, path+dim, bestpath);
			trace_times.push_back(rounder(duration, 2));
			trace_scores.push_back(bestscore);
		}
		else
		{
			priorscore = bestscore;
			std::copy(bestpath, bestpath+dim, path);
			j++;
		}

		// geometric cooling
		T *= alpha;
	}

	std::cout << "\nBest Path:\n";
	print_path(bestpath, dist, dim);

	// write solution to <instance>_<method>_<cutoff>[_<random_seed>].sol
	std::string output_fp = input_fp;
	output_fp.erase(output_fp.find_last_of("."), std::string::npos);
	output_fp += "_LS1_" + std::to_string(cutoff) + "_" + std::to_string(seed) + ".sol";
	write_solution(output_fp, bestpath, bestscore, dim);

	// write trace to <instance>_<method>_<cutoff>[_<random_seed>].sol
	output_fp.erase(output_fp.find_last_of("."), std::string::npos);
	output_fp += ".trace";
	write_trace(output_fp, trace_times, trace_scores);

	return bestpath;
}

float rounder(float val, int decis)
{
	int tmp = (int)(val*pow(10, decis)+0.5);
	return (float)tmp/pow(10, decis);
}

int get_score(int* path, int** dist, int dim)
{
	int score = 0;
	for (int i=0; i < dim-1; i++)
		score += dist[path[i]][path[i+1]];
	return score;
}

void print_path(int* path, int** dist, int dim)
{
	std::cout << "path: ";
	std::copy(path, path+dim, std::ostream_iterator<int>(std::cout, " "));
	std::cout << "\n";
	std::cout << "score: " << get_score(path, dist, dim) << "\n";
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

void write_solution(std::string out_fp, int* path, size_t score, int dim)
{
	std::ofstream out_file(out_fp);
	out_file << score << "\n";

	for (size_t i=0; i < dim; i++)
	{
		out_file << path[i];
		if (i < dim-1)
			out_file << ",";
	}
}

void write_trace(std::string out_fp, std::vector<float> trace_times, std::vector<size_t> trace_scores)
{
	std::ofstream out_file(out_fp);
	for (size_t i=0; i<trace_times.size(); i++)
	{
		out_file << trace_times[i] << ", " << trace_scores[i];
		if (i != trace_times.size())
			out_file << "\n";
	}
}

int main()
{
	std::string input_fp = "DATA/Atlanta.tsp";
	simann(input_fp, 3);
	return 0;
}
