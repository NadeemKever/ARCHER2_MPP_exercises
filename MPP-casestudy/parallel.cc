#include <iostream>
#include <cmath>
#include "pgmio.h"
#include <string>
#include <cstring>
#include <mpi.h> 
#include <limits>

using namespace std;

// #define P 4

int main(void){
    
    //MPI vars;
    MPI_Comm comm, comm_cart;
    MPI_Status status;
    MPI_Request requestprev, requestnext;

    
    // char filename[] = "edge768x768.pgm";
    // char filename[] = "edge512x384.pgm";
    // char filename[] = "edge256x192.pgm";
    char filename[] = "edge192x128.pgm";

    int halocount, tag, rank, size;
    int Mp, Np, M, N;
    int root = 0;
    
    //Topology vars
    int ndims = 1; int reorder = 0;
    int dims[ndims],periods[ndims],coords[ndims];
    int direction,disp;
    dims[0] = 0;  // initialize the number of dimensions
    periods[0] = 0; // periodicity


    comm = MPI_COMM_WORLD;


    // if (argc != 2) {
    //     cerr << "Usage: " << argv[0] << " <npasses> " << endl;
    //     return 1;
    // }
    MPI_Init(NULL,NULL);

    MPI_Comm_size(comm,&size);
    MPI_Comm_rank(comm,&rank);


    int P = size;
    pgmsize(filename, &M, &N);
     
    if (M%P!= 0 ){
        if (rank ==0) cout<< "\nERROR: Code requires number of procs to be divide "<< M<< " evenly\n"<< endl;
        MPI_Finalize();
        return 0;

    }
    Np = N;
    Mp = M/P;
    double oldp[Mp+2][Np+2], NEW[Mp+2][Np+2], buf[Mp][Np];
    double masterbuf[M][N];

    // if (size != P){
    //     cout<< "Code requires number of procs to be "<< P<< endl;
    //     MPI_Finalize();
    //     return 0;
    // }

    // SETTING UP CARTESIAN TOPOLOGY //

    MPI_Dims_create(size, ndims, dims );
    MPI_Cart_create(comm,ndims, dims, periods,reorder, &comm_cart);
    // MPI_Comm_rank(comm_cart, &rank_cart);
    // MPI_Cart_coords(comm_cart, rank_cart, ndims, coords);

    direction = 0;disp=1;
    int prev,next; //ranks for neighboring procs in the topology
    MPI_Cart_shift(comm_cart,direction, disp,&prev,&next );
    // printf( "P[%d]: shift 1: src[%d] P[%d] dest[%d] \n", rank,prev,rank,next );fflush(stdout);
    // MPI_Barrier(comm);




    

    if (rank ==0){
        cout << "\nNumber of processes = "<< size<< endl;
        cout<<"\nOn rank "<< rank << ":"<<endl;
        printf("    In file: %s --> M = %d, N = %d\n",filename,M,N);
        
        pgmread(filename, masterbuf, M, N);
        printf("    Size of read file: %zu\n",sizeof(masterbuf));
    }

    // MPI_Barrier(comm);
    // printf("On rank %d Mp = %d, Np = %d\n",rank,Mp,Np);
    int sendcount,recvcount;  
    sendcount = Mp*Np;
    recvcount = Mp*Np;
    // cout<<"send count "<<sendcount<<" recv count"<<recvcount<<endl;
    MPI_Scatter(masterbuf,sendcount, MPI_DOUBLE, buf, recvcount,MPI_DOUBLE, root, comm);
    
    // int MPI_Scatter(void *sendbuf,
    //     int sendcount, MPI_Datatype sendtype,
    //     void *recvbuf, int recvcount,
    //     MPI_Datatype recvtype, int root,
    //     MPI_Comm comm)
     

    for (int i=0; i< Mp+2; i++){
        for (int j =0; j<Np+2; j++){
            oldp[i][j] = 255;
            NEW[i][j] = 255;
        }
    }



    int i; int j;int it= 0; int maxit=10000;
    tag = 0;
    halocount = N;
    int reducecount = 1;
    double halosendprev[Np],halorecvprev[Np],halosendnext[Np],halorecvnext[Np];
    double maxdelta, maxdeltap, deltap;
    maxdelta = numeric_limits<double>::max();
    while (it< maxit &&  maxdelta > 0.1)
    {   maxdeltap = -numeric_limits<double>::max();
        // printf( "npasses = %d\n", npasses);
        // printf( "it = %d\n", it);
        it +=1;

        for (j =1; j<Np +1; j++){
            //NOTE there's a really important subtlety here
            // --> choice of 1 and Mp for the first indicies are to exclude the halo data stored  at 0 and Mp+1
            halosendprev[j-1] = oldp[1][j];
            halosendnext[j-1] = oldp[Mp][j];
        }
        MPI_Irecv(&oldp[0][1],halocount,MPI_DOUBLE, prev,tag,comm,&requestprev);
        MPI_Irecv(&oldp[Mp+1][1],halocount,MPI_DOUBLE, next,tag,comm,&requestnext);
        MPI_Ssend(&halosendprev,halocount,MPI_DOUBLE, prev,tag,comm);
        MPI_Ssend(&halosendnext,halocount,MPI_DOUBLE, next,tag,comm);
        MPI_Wait(&requestprev,&status);
        MPI_Wait(&requestnext,&status);

        

        for (i = 1; i < Mp+1; i++)
        {
            for (j = 1; j < Np+1; j++)
            {
                NEW[i][j] = (0.25)* ( oldp[i-1][j] + oldp[i+1][j]+ oldp[i][j-1] + oldp[i][j+1] - buf[i-1][j-1]);
            }
        }
        // cout<<"M = "<<i-1 <<"  N ="<< j-1<<endl;

        for (i = 1; i < Mp+1; i++)
        {
            for (j = 1; j < Np+1; j++)
            {
                deltap = fabs(oldp[i][j]-NEW[i][j]);
                // cout<< "delatp = "<< deltap<<endl;
                if (deltap > maxdeltap){
                    // printf( "it = %d rank = %d deltap = %f i = %d,j=%d\n", it,rank,deltap,i,j);
                    maxdeltap = deltap;
                }
                oldp[i][j] = NEW[i][j];
            }
        }

        MPI_Allreduce(&maxdeltap,&maxdelta, reducecount, MPI_DOUBLE,MPI_MAX,comm); 
        
    }

    for (i = 0; i < Mp; i++)
        {
            for (j = 0; j < Np; j++)
            {
                buf[i][j] = oldp[i+1][j+1];
            }
        }
    
    MPI_Gather( buf, sendcount, MPI_DOUBLE, masterbuf,recvcount, MPI_DOUBLE, root, comm);

    // int MPI_Gather(void *sendbuf, int sendcount,
    //                 MPI_Datatype sendtype, void *recvbuf,
    //                 int recvcount, MPI_Datatype recvtype,
    //                 int root, MPI_Comm comm)

    if (rank ==0){
            if (it == maxit){
                cout<<"Max iterations reached!" <<endl;
            }
            printf("\nAt iteration %d max delta = %f\n",it, maxdelta);
        }

    if (rank ==0){
        printf("\nWriting on rank %d\n",rank);
        string newFilename = "ppicture.pgm";
        // Create a char array and copy the contents of newFilename.c_str() into it
        char wfilename[newFilename.size() + 1];
        strcpy(wfilename, newFilename.c_str());
        pgmwrite(wfilename, masterbuf, M, N);
        cout<< "Write success\n"<<endl;
    }

    MPI_Finalize();

}