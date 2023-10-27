#include <mpi.h>
#include <iostream>

using namespace std;

int main(void){

    MPI_Comm comm;
    MPI_Request request;
    MPI_Status status;


    int rank, size, tag, source,src, dest;
    int count, sendData,sum, recvData;

    double tstart, tstop;

    comm = MPI_COMM_WORLD;

    MPI_Init(NULL,NULL);

    MPI_Comm_size(comm, &size);
    MPI_Comm_rank(comm, &rank);
    MPI_Barrier(comm); //synchronize proc clocks
    tstart = MPI_Wtime(); // shoot gun to start clocks

    count = 1; //sending data of size 1
    tag = 0;

    sendData = (rank+1) *(rank+1);
    sum = sendData; 
    for(int i = 0; i< size-1; i++){
        src = (rank - 1)%size;
        dest = (rank +1)%size;
        MPI_Irecv(&recvData,count,MPI_INT, src,tag,comm,&request);
        MPI_Ssend(&sendData,count,MPI_INT, dest,tag,comm);
        MPI_Wait(&request,&status);
        // cout<< "rank "<< rank <<":  " <<recvData <<", " << sendData<<endl;
        sendData = recvData;
        sum += recvData;

    }

    MPI_Barrier(comm); // sync completion
    tstop = MPI_Wtime(); // stop clock
    // cout.precision(100);
    // cout<<"On rank "<< rank <<" total is "<< sum << endl;
    if (rank == 0){
        cout << "Final Time = " << tstop-tstart << "\n"<<endl;
    }


    MPI_Finalize();

    
}