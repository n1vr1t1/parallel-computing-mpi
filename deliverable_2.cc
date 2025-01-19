#include "sequential.h"
#include "explicit_mpi.h"

#include<cstdio>
#include<cstdlib>
#include<cassert>
#include<fstream>
#include<mpi.h>
#ifdef _OPENMP
#include<omp.h>
#endif

int main(int argc, char* argv[]){
    float* M, *T;
    bool sym, sym_mpi;
    int rank, size, n, p;
    double start, end, sym_time, transpose_time, sym_time_mpi, transpose_time_mpi;
    bool eff = false;
    std::ofstream symmetry_out, transpose_out, scaling_out;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    p=atoi(argv[1]);
    n = 1 << p;
    M = new float[n*n];
    T = new float[n*n];
    if(M==NULL || T==NULL){
        printf("Memory allocation failed\n");
        MPI_Abort(MPI_COMM_WORLD, 477);
    }

    if(rank==0){
        if(argc>2 && rank==0){
            eff=true;
            scaling_out.open(argv[2], std::ios::app);
            scaling_out<<size<<", ";
        }else{
            symmetry_out.open("symmetry.csv", std::ios::app);
            symmetry_out<<size<<", ";
            transpose_out.open("transpose.csv", std::ios::app);
            transpose_out<<size<<", ";
        }
        //initializing matrix with random numbers
        for (int i=0;i<n*n;i++){
            M[i] = float(rand() % 100);
        }

        /***************************************************************************************************\
        *                                                                                                   *
        *                                          Sequential/Naive                                         *
        *                                                                                                   *
        \***************************************************************************************************/
        
        /***************************************************************************************************\
        *                                         Checking symmetry                                         *
        \***************************************************************************************************/

        start = MPI_Wtime();
        sym = checkSym(M,n);
        end = MPI_Wtime();
        sym_time = end - start;

        /****************************************************************************************************\
        *                                           Transposing                                              *
        \****************************************************************************************************/

        start = MPI_Wtime();
        matTranspose(M,T,n);
        end = MPI_Wtime();
        check_transpose(M,T,n);
        transpose_time = end - start;

    }

    MPI_Barrier(MPI_COMM_WORLD);

        /***************************************************************************************************\
        *                                                                                                   *
        *                                              MPI                                                  *
        *                                                                                                   *
        \***************************************************************************************************/
            
        /**************************************************************************************************
         *                                   Checking Symmetry with MPI                                   *
         **************************************************************************************************/

    if(rank==0){
        start = MPI_Wtime();
        sym_mpi = checkSymMPI_DoubleCyclic(M, n, rank, size);
        end = MPI_Wtime();
        assert(sym==sym_mpi);
        sym_time_mpi = end - start;
        if(!eff){
            symmetry_out<<sym_time / sym_time_mpi<<", ";
        }
    }else{
        sym_mpi = checkSymMPI_DoubleCyclic(M, n, rank, size);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    
    if(rank==0){
        start = MPI_Wtime();
        sym_mpi = checkSymMPI_HorizontalBlock(M, n, rank, size);
        end = MPI_Wtime();
        assert(sym==sym_mpi);
        sym_time_mpi = end - start;
        if(!eff){
            symmetry_out<<sym_time / sym_time_mpi<<", ";
        }
    }else{
        sym_mpi = checkSymMPI_HorizontalBlock(M, n, rank, size);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    if(rank==0){
        start = MPI_Wtime();
        sym = checkSymMPI_VerticalBlock(M, n, rank, size);
        end = MPI_Wtime();
        assert(sym==sym_mpi);
        sym_time_mpi = end - start;
        if(!eff){
            symmetry_out<<sym_time / sym_time_mpi<<std::endl;
            symmetry_out.close();
        }
    }else{
        sym_mpi = checkSymMPI_VerticalBlock(M, n, rank, size);
    }
    MPI_Barrier(MPI_COMM_WORLD);

        /***************************************************************************************************
         *                         Transposing with Explicit Parallelism using MPI                         *
         ***************************************************************************************************/

    if(rank==0){
        start = MPI_Wtime();
        matTransposeMPI_HorizontalBlock(M, T, n, rank, size);
        end = MPI_Wtime();
        check_transpose(M,T,n);
        transpose_time_mpi = end-start;
        if(eff){
            scaling_out<<transpose_time_mpi<<", ";
        }else{
            transpose_out<<transpose_time / transpose_time_mpi<<", ";  
        }     
    }else{
        matTransposeMPI_HorizontalBlock(M, T, n, rank, size);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    if(rank==0){
        start = MPI_Wtime();
        matTransposeMPI_VerticalBlock(M, T, n, rank, size);
        end = MPI_Wtime();
        check_transpose(M,T,n);
        transpose_time_mpi = end-start;
        if(eff){
            scaling_out<<transpose_time_mpi<<", ";
        }else{
            transpose_out<<transpose_time / transpose_time_mpi<<", ";  
        }
    }else{
        matTransposeMPI_VerticalBlock(M, T, n, rank, size);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    if(rank==0){
        start = MPI_Wtime();
        matTransposeMPI_EqualBlocks(M, T, n, rank, size);
        end = MPI_Wtime();
        check_transpose(M,T,n);
        transpose_time_mpi = end - start;
        if(eff){
            scaling_out<<transpose_time_mpi<<std::endl;
            scaling_out.close();
        } else{
            transpose_out<<transpose_time / transpose_time_mpi<<std::endl;  
            transpose_out.close();
        }  
    }else{
        matTransposeMPI_EqualBlocks(M, T, n, rank, size);
    }

    //releasing memory 
    delete[] T;
    delete[] M;

    MPI_Finalize();
    return 0;
}