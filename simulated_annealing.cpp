#include <iostream> //std::cout
#include <iterator> // std::ostream_iterator
#include <numeric> // std::iota
#include <random> // std::random_device, std::mt19937
#include <algorithm> // std::shuffle
#include <cmath> // exp
#include <ctime> // std::clock

int get_score(int* path, int** dist, int dim)
{
	int score = 0;
	for (int i=0; i < dim-1; i++)
		score += dist[path[i]][path[i+1]];
	return score;
}

void print_path(int* path, int dim, int** dist)
{
	std::cout << "path: ";
	std::copy(path, path+dim, std::ostream_iterator<int>(std::cout, " "));
	std::cout << "\n";
	std::cout << "score: " << get_score(path, dist, dim) << "\n";
}

int* simann(int** dist, int dim, bool verbose=false)
{
	int path[dim];
	std::iota(path, path+dim, 0);

	int *bestpath = new int[dim];
	std::copy(path, path+dim, bestpath);

	// randomize initial values
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(path, path+dim, g);

	int max_secs = 10, j = 0, T = 10;
	int steps = dim*(dim-1);
	int neighscore, idx1, idx2;
	int priorscore = get_score(path, dist, dim);
	int bestscore = priorscore;
	double p, duration;
	double alpha = 0.95, beta = 5;
	std::clock_t start;

	if (verbose)
		std::cout << "Initial:\n";
		print_path(bestpath, dim, dist);

	while (true)
	{
		// stopping conditions
		// - no score improvement for beta T values
		// - timeout
		duration = (double) (std::clock()-start)/CLOCKS_PER_SEC;
		if (j >= beta || duration > max_secs)
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
		if (priorscore < bestscore)
		{
			bestscore = priorscore;
			std::copy(path, path+dim, bestpath);
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

	return bestpath;
}

int main()
{
	std::cout << "Simulated Annealing\n\n";

	int dim = 10;
	int** dist = new int* [dim];
	for (int i=0; i < dim; i++)
		dist[i] = new int[dim];

	for (int i=0; i < dim; i++)
		for (int j=0; j < dim; j++)
			dist[i][j] = std::rand() % dim;

	int* bestpath = simann(dist, dim, true);
	std::cout << "\nResults:\n";
	print_path(bestpath, dim, dist);

	return 0;
}
