#include <mpi.h>
#include <iostream>

using namespace std;


struct compound{
    int ival;
    double dval;
};

int main(void){

    MPI_Comm comm;
    MPI_Request request;
    MPI_Status status;

    struct compound sendData, recvData, sum;
    int rank, rank_cart, size, tag, src, dest;
    int count;
    double tstart, tstop;

    comm = MPI_COMM_WORLD;

    MPI_Init(NULL,NULL);

    MPI_Comm_size(comm, &size);
    MPI_Comm_rank(comm, &rank);
    

    
    const int count_struct = 2;
    int array_of_blocklengths[count_struct];
    MPI_Datatype array_of_types[count_struct];
    MPI_Aint array_of_displacements[count_struct]; 
    


    
    sendData.ival = rank;
    sendData.dval = (rank+1) *(rank+1);

    MPI_Aint integer_add, double_add;
    MPI_Get_address(&sendData.ival,&integer_add);
    MPI_Get_address(&sendData.dval,&double_add);
    
    // cout<<integer_add<<endl;
    // cout<<double_add<<endl;
    // cout<<abs(double_add - integer_add)<<endl;
    array_of_blocklengths[0]  =1;
    array_of_types[0] = MPI_INT;
    array_of_blocklengths[1]  =1;
    array_of_types[1] = MPI_DOUBLE;
    array_of_displacements[0] = 0 ;
    array_of_displacements[1] = double_add - integer_add;// displacement of double from start of structs mem address = 0--> int_add
    


    MPI_Datatype MPI_COMPOUND;
    MPI_Type_create_struct (count_struct,array_of_blocklengths,array_of_displacements,array_of_types,
                                &MPI_COMPOUND);
    MPI_Type_commit(&MPI_COMPOUND);


    // MPI_Type_create_struct (int count,
                                // int *array_of_blocklengths,
                                // MPI_Aint *array_of_displacements,
                                // MPI_Datatype *array_of_types,
                                // MPI_Datatype *newtype)


    MPI_Barrier(comm); //synchronize proc clocks
    tstart = MPI_Wtime(); // shoot gun to start clocks

    count = 1; //sending data of size 1
    tag = 0;



    // sendData =rank; 
    // sendData = (rank+1) *(rank+1);
    sum = sendData; 
    for(int i = 0; i< size-1; i++){
        src = (rank -1) % size;
        dest = (rank+1) % size;
        MPI_Irecv(&recvData,count,MPI_COMPOUND, src,tag,comm,&request);
        MPI_Ssend(&sendData,count,MPI_COMPOUND, dest,tag,comm);
        MPI_Wait(&request,&status);
        // cout<< "rank "<< rank <<":  " <<recvData <<", " << sendData<<endl;
        sendData = recvData;
        sum.ival += recvData.ival;
        sum.dval += recvData.dval;

    }

    MPI_Barrier(comm); // sync completion
    tstop = MPI_Wtime(); // stop clock
    
    printf( "The sums are: %d %f on rank %d\n",sum.ival, sum.dval, rank);
    // cout<<"On rank "<< rank <<" total is "<< sum.ival << " and "<< sum.dval<< endl;
    if (rank == 0){
        cout << "Final Time = " << tstop-tstart << "\n"<<endl;
    }


    MPI_Finalize();

    
}