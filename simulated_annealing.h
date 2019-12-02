#ifndef SIMULATED_ANNEALING_H
#define SIMULATED_ANNEALING_H

namespace SA
{

// path separator for windows os
#if defined(_WIN16) | defined(_WIN32) | defined(_WIN64)
#if !defined SEP
#define SEP "\\"
#endif
#else
#if !defined SEP
#define SEP "/"
#endif
#endif

	struct SAParams {
		size_t cutoff = 30;
		int seed = -1;
		double alpha = 0.95;
		double T = 100;
	};

	struct Trial {
		private:
			std::string filename_to_path(std::string name);
			std::string filename_to_loc(std::string name);
			std::string get_output_path(std::string suffix);
		public:
			std::string input_fp;
			std::string output_dir = "output";
			std::vector<float> times;
			std::vector<size_t> scores;
			std::vector<int> bestpath;
			int bestscore;
			SAParams sap;
			bool verbose = false;

			void write_solution();
			void write_trace();
	};

	void simann(Trial &trial);
}

#endif // SIMULATED_ANNEALING_H
