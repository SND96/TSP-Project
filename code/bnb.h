#ifndef _TSP_BNB_H
#define _TSP_BNB_H


using namespace std;

void writeToOutputFile(string traceFileName, string solFileName, int bestWeight, chrono::high_resolution_clock::time_point startTime, int curr_path[], int dim, int cutoff);
class TSP_BNB
{   
    public:
    int N;
    bool* visited ;
    int* final_path ;
    int final_res;
    TSP_BNB(int dim);
    void copyToFinal(int curr_path[], int *final_path); 
    int firstMin(int** adj, int i);
    int secondMin(int** adj, int i);
    void BNBRecursive(int** adj, int curr_bound, int curr_weight, int level, int curr_path[], int *final_res, bool *visited, int *final_path, chrono::high_resolution_clock::time_point startTime, string traceFilePath, string solFilePath, int cutoff);
    void BNB(int** adj, int *final_res, bool *visited, int *final_path, chrono::high_resolution_clock::time_point startTime, string traceFilePath, string solFilePath, int cutoff);
};
int getDim(string filePath);
int** getAdjMatrix(string filePath, int dim);

#endif 
  