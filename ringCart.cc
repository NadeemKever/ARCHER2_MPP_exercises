#include <mpi.h>
#include <iostream>

using namespace std;

int main(void){

    MPI_Comm comm, comm_cart;
    MPI_Request request;
    MPI_Status status;


    int rank, rank_cart, size, tag, source, dest;
    int count, sendData,sum, recvData;
    int ndims = 1; int reorder = 1;
    int dims[ndims],periods[ndims],coords[ndims];
    int direction,disp;
    double tstart, tstop;

    comm = MPI_COMM_WORLD;

    MPI_Init(NULL,NULL);

    MPI_Comm_size(comm, &size);
    MPI_Comm_rank(comm, &rank);
    
    
    dims[0] = 0;
    periods[0] = 1;

    // 2D Topology //
    // ndims = 2;
    // int dims[ndims],periods[ndims],coords[ndims];
    // dims[0]=dims[1]=0;
    // periods[0]= periods[1]= 0;

    MPI_Dims_create(size, ndims, dims );
    // cout<<dims[0] <<", "<<dims[1]<<endl;
    MPI_Cart_create(comm,ndims, dims, periods,reorder, &comm_cart);
    
    MPI_Comm_rank(comm_cart, &rank_cart);
    // cout<< "rank = "<< rank<< " cartesian rank = " << rank_cart<<endl;
    MPI_Cart_coords(comm_cart, rank_cart, ndims, coords);

    // 2D Topology //
    // cout << "coords = " << coords[0]<< coords[1]<<endl;

    direction = 0;disp=1;
    MPI_Cart_shift(comm_cart,direction, disp,&source,&dest );
    // printf( "P[%d]: shift 1: src[%d] P[%d] dest[%d] \n", rank,source,rank,dest );fflush(stdout);

    MPI_Barrier(comm); //synchronize proc clocks
    tstart = MPI_Wtime(); // shoot gun to start clocks

    count = 1; //sending data of size 1
    tag = 0;

    sendData =rank; 
    // sendData = (rank+1) *(rank+1);
    sum = sendData; 
    for(int i = 0; i< size-1; i++){

        MPI_Irecv(&recvData,count,MPI_INT, source,tag,comm_cart,&request);
        MPI_Ssend(&sendData,count,MPI_INT, dest,tag,comm_cart);
        MPI_Wait(&request,&status);
        // cout<< "rank "<< rank <<":  " <<recvData <<", " << sendData<<endl;
        sendData = recvData;
        sum += recvData;

    }

    MPI_Barrier(comm_cart);
    MPI_Barrier(comm); // sync completion
    tstop = MPI_Wtime(); // stop clock
    
    // cout<<"On rank "<< rank <<" total is "<< sum << endl;
    if (rank == 0){
        cout << "Final Time = " << tstop-tstart << "\n"<<endl;
    }


    MPI_Finalize();

    
}