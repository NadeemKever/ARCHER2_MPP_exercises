#include <mpi.h>
#include <iostream>
#include <cmath>

using namespace std;

#define N 12
void printarray(int rank, int *array, int n);

int main(void){
    
    int x[N];

    //MPI vars

    MPI_Comm comm;
    MPI_Status status;

    int rank, size, tag, source;

    int istart, istop, istep;

    int recvx[N];

    // Initialise MPI and compute number of processes and local rank

    comm = MPI_COMM_WORLD;

    MPI_Init(NULL,NULL);

    MPI_Comm_size(comm, &size);
    MPI_Comm_rank(comm, &rank);
    

    if (rank == 0){
        
        for(int i = 0; i <N; i++  ){
            x[i] =i;
        }
        tag = 0;
        for (int dest = 1; dest<size; dest++){
            
            MPI_Ssend(&x, N, MPI_INT, dest, tag, comm);
        }
    }else{
        for(int i = 0; i <N; i++  ){
            x[i] =-1;
        } 
        tag = 0;
        MPI_Recv(x, N, MPI_INT, 0,tag,comm, &status);
        
    }

    // cout << "Hello from rank: "<< rank << "\nx = " ;

    // for (int i = 0; i < N; i++) {
    //     cout << x[i] << " ";
    // }
    // cout<< "\n"<<endl;

    printarray(rank, x, N);

    MPI_Finalize();
}

void printarray(int rank, int *array, int n)
{
    int i;
    printf("On rank %d, array[] = [", rank);
    for (i = 0; i < n; i++)
    {
        if (i != 0)
            printf(",");
        printf(" %d", array[i]);
    }
    printf(" ]\n");
}