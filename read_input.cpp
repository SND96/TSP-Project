#include <iomanip>  // for setw() and ws
#include <string>
#include <fstream>
#include <cstdlib>
#include <sstream>

using namespace std;

int main()
{
    string line;
    ifstream tsp_inp;
    tsp_inp.open("DATA/Atlanta.tsp");
    int count_line = 0, dim = 0;
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
    double coord[dim][2];
    int i=0;
    if (tsp_inp.is_open())
    {
        while (getline(tsp_inp,line) )
        {
            count_line += 1;
            if((count_line >= 6) && (count_line < (dim+6)))
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
    for(i=0;i<dim;i++)
        printf("%d %lf %lf\n",i, coord[i][0], coord[i][1]);
    tsp_inp.close();
}