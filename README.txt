There are two main folders and the make file:
- code
	- bnb.cpp                               branch and bound code
	- bnb.h                                 branch and bound header file
	- genetic_algo.cpp                      local search 2 code
	- genetic_algo.h                        local search 2 header file
	- prim_dfs.cpp                          approximation algo code
	- prim_dfs.h                            approximation algo header file
	- simulated_annealing.cpp               local search 1 code
	- simulated_annealing.h                 local search 1 header file
	- TSP-main.cpp                          tsp-main calls functions from the 4 algos codes
- output
- main                                      make file

The output folder containts all .sol and .trace files.

Example of how to compile the make file:
./main -inst Cincinnati.tsp -alg LS2 -time 13 -seed 3

(ignore the warnings)
