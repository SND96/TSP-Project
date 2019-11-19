#include <iostream>
#include <iomanip>  // for setw() and ws
#include <string>
#include <fstream>
#include <cstdlib>
#include<vector>

using namespace std;


int main()
{
    string line;
 
    ifstream tsp_inp;
    tsp_inp.open("DATA/Atlanta.tsp");
    int count_line = 0;
    if (tsp_inp.is_open())
  {
        while (getline (tsp_inp,line) )
    {
            count_line += 1;
            if(count_line == 3)
            {
                for(int i = 12; line[i]!='\n';i++)
                {
                    
                }
            }



    }
    count_line = 0
    double coord[dim][dim];
    if (tsp_inp.is_open())
    {
    while (getline (tsp_inp,line) )
    {
            count_line += 1;
            if(count_line == 12)
            {
                for(int i = 12; line[i]!='\n';i++)
                {
                    
                }
            }

    }

    tsp_inp.close();


  }
}