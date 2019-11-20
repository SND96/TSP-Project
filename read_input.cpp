#include <iomanip>  // for setw() and ws
#include <string>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <cmath>

using namespace std;

int main()
{
    string line;
    ifstream tsp_inp;
    tsp_inp.open("DATA/Atlanta.tsp");
    int count_line = 0, dim = 0, i;
    if (tsp_inp.is_open())
    {
        while (getline(tsp_inp,line) )
        {
            count_line += 1;
            if(count_line == 3)
            {
                istringstream iss(line);
                string s;
                if (iss >> s >> dim)
                break;
            }
        }
    }
    double** coord = new double*[dim];
    for(i = 0; i < dim; ++i)
        coord[i] = new double[2];
    i=0;
    if (tsp_inp.is_open())
    {
        while (getline(tsp_inp,line) )
        {
            count_line += 1;
            if ((count_line >= 6) && (count_line < (dim+6)))
            {
                istringstream iss(line);
                int a;
                double b,c;
                if (iss >> a >> b >> c) {
                    coord[i][0] = b;
                    coord[i][1] = c;
                    i++;
                }
            }
        }
    }
    for (i=0; i<dim; i++)
        printf("%d %lf %lf\n",i, coord[i][0], coord[i][1]);

    tsp_inp.close();

    int** adj = new int*[dim];
    for (i = 0; i < dim; ++i)
        adj[i] = new int[dim];

    for (i = 0; i < dim; i++)
        for (int j = 0; j <= i; j++){
            if ( i == j)
                adj[i][j] = 0;
            else
                adj[j][i] = adj[i][j] = round(sqrt((coord[i][0]-coord[j][0])*(coord[i][0]-coord[j][0])+(coord[i][1]-coord[j][1])*(coord[i][1]-coord[j][1]))); 
        }
    for (i = 0; i < dim; i++)
    {
        for (int j=0; j<dim; j++)
            printf("%d ", adj[i][j]);
        printf("\n");
    }
}