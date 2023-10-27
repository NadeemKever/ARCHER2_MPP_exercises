#include "mpi.h"
#include <iostream>

using namespace std;

int main()
{
        MPI_Init(NULL,NULL);

        int size;
        int rank;
        int namelen;
        char procname[MPI_MAX_PROCESSOR_NAME];

        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Get_processor_name(procname, &namelen);
        MPI_Comm_size(MPI_COMM_WORLD, &size);



        cout << "rank = " << rank << " on machine " << procname<< endl;
        cout << "Machine size = " << size << endl;
        cout << "Hello World!\n" << endl;

//      if (rank == 0) {
//              cout << "rank = " << rank << " on machine " << procname<< endl;
//              cout << "Machine size = " << size << endl; 
//
//      }

        MPI_Finalize();
}

