#include <mpi.h>
#include <iostream>

using namespace std;

int main(void){

    MPI_Comm comm;
    MPI_Status status;

    int rank, size, src, dest,tag, count,nelements, nbounces;
    double tstart, tfinal;
    nelements = 250000;
    //nelements = 0;
    double sendData[nelements],recvData[nelements];
    count = nelements;
    if (rank == 0){
    cout<< "number of elements in array: "<< nelements <<endl;
    cout<< "Size of data: " << sizeof(sendData)/1000000.0<< " MB" <<endl;
    }
    comm = MPI_COMM_WORLD;
    tag = 0;

    MPI_Init(NULL,NULL);
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    //init arrays
    for (int i = 0; i < nelements; i ++){
        sendData[i] = (double) rank + 10.0; 
    }
    nbounces = 10000;
    MPI_Barrier(comm); //synchronize proc clocks
    tstart = MPI_Wtime(); // shoot gun to start clocks
    for (int imessage = 0; imessage <nbounces;imessage++){
        if (rank == 0){
            dest = 1;
            src = 1;
            MPI_Ssend(&sendData,count, MPI_INT, dest,tag,comm);
            MPI_Recv(&sendData, count, MPI_INT, src,tag,comm,&status);

        }else if (rank == 1){
            dest = 0;
            src = 0;
            MPI_Recv(&recvData, count, MPI_INT, src,tag,comm, &status);
            MPI_Ssend(&recvData,count, MPI_INT, dest,tag,comm);

        }

    }

    MPI_Barrier(comm); //synchronize proc clocks
    tfinal = MPI_Wtime(); // shoot gun to start clocks

    if (rank == 0){
        cout << "Final Time = " << tfinal-tstart <<endl;
	printf("Latency = %f seconds for %zu bytes of data.\n",(tfinal-tstart)/((double) nbounces *2.0), sizeof(sendData) );
    }else if (rank==1) {
        printf("Data at 0 is %f \n",recvData[0]);
    }




    MPI_Finalize();

}
