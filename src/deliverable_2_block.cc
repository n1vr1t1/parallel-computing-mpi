#include<cstdio>
#include<fstream>
#include<cstdlib>
#include<mpi.h>

#include "sequential.h"
#include "explicit_mpi.h"

int main(int argc, char** argv){
    int rank, size, n, p;
    float *M, *T;
    double start, end, seq_time, mpi_time;
    std::ofstream transpose_out;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    p=atoi(argv[1]);
    n=1<<p;
    M = new float [n*n];
    T = new float [n*n];
    if(rank==0){
        transpose_out.open("block_transpose.csv", std::ios::app);
        if(!transpose_out.is_open()){
            printf("Error opening block_transpose.csv file\n");
            return 1;
        }
        //initialize matrix
        if (M==NULL||T==NULL){
            printf("Memory allocation failed\n");
            MPI_Abort(MPI_COMM_WORLD, 4770);
        }
        for(int i=0;i<n*n;i++){
            M[i] = float(rand()%100);
        }
        //sequential code
        start = MPI_Wtime();
        matTranspose(M, T, n);
        end = MPI_Wtime();
        seq_time = end - start;

    }
    MPI_Barrier(MPI_COMM_WORLD);

    if(rank==0){
        start = MPI_Wtime();
        matTransposeBlockMPI(M, T, n, rank, size);
        end = MPI_Wtime();
        check_transpose(M, T, n);
        mpi_time = end - start;
        transpose_out<<seq_time/mpi_time<<", ";
    }else{
        matTransposeBlockMPI(M, T, n, rank, size);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    delete[] M;
    delete[] T;

    if(rank==0){
        transpose_out<<std::endl;
        transpose_out.close();
    }
    MPI_Finalize();
    return 0;
}