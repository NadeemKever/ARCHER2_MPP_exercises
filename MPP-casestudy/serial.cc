#include <iostream>
#include <cmath>
#include "pgmio.h"
#include <string>
#include <cstring>

using namespace std;

int main(int argc, char* argv[]){
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <npasses>" << endl;
        return 1;
    }
    int M, N, npasses;
    char filename[] = "edge192x128.pgm";
    
    pgmsize(filename, &M, &N);
    printf("In file: %s --> M = %d, N = %d\n",filename,M,N);
    
    double readbuf[M][N], writebuf[M][N], old[M+2][N+2], NEW[M+2][N+2];
    pgmread(filename, readbuf, M, N);
    printf("Size of read file: %zu\n",sizeof(readbuf));
    // pgmwrite(wfilename, readbuf, M, N);
    // cout<< "Write success"<<endl;

    for (int i=0; i< M+2; i++){
        for (int j =0; j<N+2; j++){
            old[i][j] = 255;
        }
    }

    npasses = atoi(argv[1]); // Convert the command-line argument to an integer
    printf("Total number of passes: %d\n",npasses);

    string newFilename = "picture" + to_string(npasses) + ".pgm";
    
    // Create a char array and copy the contents of newFilename.c_str() into it
    char wfilename[newFilename.size() + 1];
    strcpy(wfilename, newFilename.c_str());
    int i; int j;
    while (npasses > 0)
    {
        npasses -= 1;
        for (i = 1; i < M+1; i++)
        {
            for (j = 1; j < N+1; j++)
            {
                NEW[i][j] = (0.25)* ( old[i-1][j] + old[i+1][j]+ old[i][j-1] + old[i][j+1] - readbuf[i-1][j-1]);
            }
        }
        // cout<<"M = "<<i-1 <<"  N ="<< j-1<<endl;

        for (i = 1; i < M+1; i++)
        {
            for (j = 1; j < N+1; j++)
            {
                old[i][j] = NEW[i][j];
            }
        }



    }

    for (i = 0; i < M; i++)
        {
            for (j = 0; j < N; j++)
            {
                writebuf[i][j] = old[i+1][j+1];
            }
        }
    
    pgmwrite(wfilename, writebuf, M, N);
    cout<< "Write success"<<endl;

}