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
#include <experimental/filesystem> // fs::create_directory, fs::exists

namespace fs = std::experimental::filesystem;

// path separator for windows os
#if defined(_WIN16) | defined(_WIN32) | defined(_WIN64)
#define SEP "\\"
#else
#define SEP "/"
#endif

// DECLARATIONS
struct SAParams {
	size_t cutoff = 30;
	int seed = -1;
	double alpha = 0.95;
	double T = 10;
};

struct Trial {
	int id = 0;
	std::string input_fp;
	std::string output_dir = "output";
	std::vector<float> times;
	std::vector<size_t> scores;
	std::vector<int> bestpath;
	int bestscore;
	SAParams sap;
	bool verbose = false;

	std::string filename_to_path(std::string name)
	{
		// gets the path from the full filename
		// returns name if no separator found
		size_t idx = name.find(SEP);
		return idx != std::string::npos ? name.substr(idx) : name;
	}

	std::string filename_to_loc(std::string name)
	{
		// gets the location from the full filename
		std::string path_name = filename_to_path(name);
		return path_name.substr(0, path_name.find(".tsp"));
	}

	std::string get_output_path(std::string suffix)
	{
		// write solution to <instance>_<method>_<cutoff>[_<random_seed>][_<trial>].<suffix>
		std::string output_fp = "";

		// idempotentally create dir and add to output filepath
		if (!output_dir.empty())
		{
			if (!fs::exists(output_dir.c_str()))
				fs::create_directory(output_dir.c_str());

			output_fp += output_dir + SEP;
		}

		output_fp += filename_to_loc(input_fp) + "_LS1_" + std::to_string(sap.cutoff);

		if (sap.seed != -1)
			output_fp += std::to_string(sap.seed);

		if (suffix == "trace")
			output_fp += "_" + std::to_string(id);

		output_fp += "." + suffix;
		return output_fp;
	}

	void write_solution()
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
			out_file << bestpath[i];
			if (i < dim-1)
				out_file << ",";
		}
	}

	void write_trace()
	{
		// write to .trace file time and score of each found
		// ex: 3.45, 102
		std::string output_fp = get_output_path("trace");
		std::ofstream out_file(output_fp);
		for (size_t i=0; i < times.size(); i++)
		{
			out_file << times[i] << ", " << scores[i];
			if (i != times.size())
				out_file << "\n";
		}
	}
};

void simann(Trial &trial);
float rounder(float val, int decis);
int get_score(std::vector<int> &path, int** dist);
void print_path(std::vector<int> &path, int** dist);
int get_dim(std::string fp);
int** get_adj_matrix(std::string fp, int dim);

void simann(Trial &trial)
{
	int dim = get_dim(trial.input_fp);
	int** dist = get_adj_matrix(trial.input_fp, dim);
	SAParams sap = trial.sap;

	std::vector<int> path(dim);
	std::iota(path.begin(), path.end(), 0);
	trial.bestpath = path;

	// randomize initial values
	if (sap.seed != -1)
	{
		std::srand(sap.seed);
		std::shuffle(path.begin(), path.end(), std::default_random_engine(sap.seed));
	} else
		std::shuffle(path.begin(), path.end(), std::default_random_engine(std::time(0)));

	int j = 0;
	int steps, neighscore, idx1, idx2;
	int priorscore = get_score(path, dist);
	trial.bestscore = priorscore;
	double p;
	double duration = 0;
	double T = sap.T, alpha = sap.alpha;

	if (trial.verbose)
	{
		std::cout << "\nInitial Path:\n";
		print_path(trial.bestpath, dist);
	}

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

			// random 2-exchange
			idx1 = std::rand() % dim;
			idx2 = std::rand() % dim;
			std::swap(path[idx1], path[idx2]);
			neighscore = get_score(path, dist);

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

	if (trial.verbose)
	{
		std::cout << "\nBest Path:\n";
		print_path(trial.bestpath, dist);
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

int main()
{
	SAParams sap;
	sap.cutoff = 3;
	int trial_count = 3;
	for (int i=0; i < trial_count; i++)
	{
		Trial trial;
		trial.id = i+1;
		trial.input_fp = "DATA/Atlanta.tsp";
		trial.sap = sap;
		trial.verbose = true;
		simann(trial);
		trial.write_solution();
		trial.write_trace();
	}

	return 0;
}
