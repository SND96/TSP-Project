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

int main()
{
	std::cout << "Simulated Annealing\n\n";

	// create path
	int dim = 10;
	int path[dim];
	std::iota(path, path+dim, 0);

	// randomize initial values
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(path, path+dim, g);

	// distance matrix
	int** dist = new int* [dim];
	for (int i=0; i < dim; i++)
		dist[i] = new int[dim];

	for (int i=0; i < dim; i++)
		for (int j=0; j < dim; j++)
			dist[i][j] = std::rand() % dim;

	int max_iters = 5, score_iters = 3, max_secs = 60;
	int i = 0, j = 0, T = 10;
	int neighscore, idx1, idx2;
	int priorscore = get_score(path, dist, dim);
	double p, duration;
	double a = 0.95;
	std::clock_t start;

	std::cout << "initial path: ";
	std::copy(path, path+dim, std::ostream_iterator<int>(std::cout, " "));
	std::cout << "\n";
	std::cout << "initial score: " << priorscore << "\n";

	while (i < max_iters && j < score_iters)
	{
		// terminate after max iterations or no score improvement
		if (i >= max_iters || j >= score_iters)
			break;

		// terminate if timeout
		duration = (double) (std::clock()-start)/CLOCKS_PER_SEC;
		if (duration > max_secs)
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
			j++;
		}
		priorscore = neighscore;
		i++;
		T *= a;
	}

	std::cout << "final path: ";
	std::copy(path, path+dim, std::ostream_iterator<int>(std::cout, " "));
	std::cout << "\n";
	std::cout << "final score: " << priorscore;

	return 0;
}
