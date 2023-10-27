#include <mpi.h>
#include <iostream>

using namespace std;

#define N 10


void printMatrix(int rank, int array[N][N], int n);

int main(void){

    MPI_Comm comm;
    MPI_Status status;

    int rank, size, src, dest,tag, count,M, nbounces,countmrows, countmcols,blocklength,stride;
    double tstart, tfinal;
    // nelements = 250000;
    M=3;
    int sendData[N][N],recvData[N][N];
    comm = MPI_COMM_WORLD;
    tag = 0;

    MPI_Init(NULL,NULL);
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    MPI_Datatype mrows;//mrows is contiguous
    MPI_Datatype mcols;//mcols is separated in memory
    
    
    
    countmrows = M*N;
    MPI_Type_contiguous(countmrows,MPI_INT, &mrows);
    MPI_Type_commit(&mrows);



    // int MPI_Type_contiguous( int count,
    //                         MPI_Datatype oldtype,
    //                         MPI_Datatype *newtype)

    countmcols = N;
    blocklength = M;
    stride = 7+M;
    MPI_Type_vector (countmcols,blocklength, stride, MPI_INT, &mcols);
    MPI_Type_commit(&mcols);

    // int MPI_Type_vector (int count,
    //                     int blocklength, int stride,
    //                     MPI_Datatype oldtype,
    //                     MPI_Datatype *newtype)



    //init arrays
    int el = 0;
    for (int i = 0; i < N; i ++){
        for (int j = 0; j<N; j++){
            sendData[i][j] = rank+el;
            recvData[i][j] = rank;
            el+=1;
        } 
    }

    printMatrix(rank, sendData, N);
    nbounces = 1;
    count = 1;
    MPI_Barrier(comm); //synchronize proc clocks
    tstart = MPI_Wtime(); // shoot gun to start clocks

    for (int imessage = 0; imessage <nbounces;imessage++){
        if (rank == 0){
            dest = 1;
            src = 1;
            // MPI_Ssend(&sendData[3][0],count, mrows, dest,tag,comm);
            // MPI_Recv(&sendData[3][0], count, mrows, src,tag,comm,&status);

            MPI_Ssend(&sendData[0][3],count, mcols, dest,tag,comm);
            MPI_Recv(&sendData[0][3], count, mcols, src,tag,comm,&status);

        }else if (rank == 1){
            dest = 0;
            src = 0;
            // MPI_Recv(&recvData[3][0], count, mrows, src,tag,comm, &status);
            // printMatrix(rank, recvData, N);
            // MPI_Ssend(&recvData[3][0],count, mrows, dest,tag,comm);

            MPI_Recv(&recvData[0][3], count, mcols, src,tag,comm, &status);
            printMatrix(rank, recvData, N);
            MPI_Ssend(&recvData[0][3],count, mcols, dest,tag,comm);

        }

    }

    MPI_Barrier(comm); //synchronize proc clocks
    tfinal = MPI_Wtime(); // shoot gun to start clocks

    // if (rank == 0){
    //     cout << "Final Time = " << tfinal-tstart <<endl;
    // }else if (rank==1) {
    //     printf("Data at 0 is %f \n",recvData[0]);
    // }




    MPI_Finalize();

}

void printMatrix(int rank, int array[N][N], int n)
{
    int i,j;
    printf("On rank %d, array[][] = [\n", rank);
    for (i = 0; i < n; i++)
    {
        printf("[");
        for (j = 0; j <n;j++){
            printf(" %d, ", array[i][j]);
        }
        printf("],\n");
    }
    printf(" ]\n");
}