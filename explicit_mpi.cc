#include<mpi.h>
#include<cstdio>
#include<cmath>

/// @brief The root sends the entire matrix to each processor using MPI_Bcast. The matrix is checked for 
/// symmetry by each processor if the sum of the row and column number is a mulitple of the rank. The result is 
/// combined using MPI_Reduce
bool checkSymMPI_DoubleCyclic(float* M, int n, int rank, int size){
    MPI_Bcast(M, n*n, MPI_FLOAT, 0, MPI_COMM_WORLD);

    bool private_sym=true;
    for(int i=0;i<n;i++){
        for(int j=0;j<i;j++){
            if(rank==(i+j)%size){
                if(M[i*n+j]!=M[j*n+i]){
                    private_sym=false;
                }
            }
        }
    }

    bool* sym=new bool[size];
    if(rank==0){
        MPI_Gather(&private_sym, 1, MPI_BYTE, sym, 1, MPI_BYTE, 0, MPI_COMM_WORLD);
        for(int i=0;i<size;i++){
            private_sym &= sym[i];
        }
    }else{
        MPI_Gather(&private_sym, 1, MPI_BYTE, NULL, 0, MPI_BYTE, 0, MPI_COMM_WORLD);
    }
    delete[] sym;
    return private_sym;
}
/// @brief The root sends blocks of rows of the matrix to each processor using MPI_Scatter. Each processor checks symmetry based
/// on the rows it receives. The result is combined using MPI_Allreduce
bool checkSymMPI_HorizontalBlock(float* M, int n, int rank, int size){
    int rows_per_rank=n/size;
    int m_size=rows_per_rank*n;
    //sending rows to the processors
    if(rank==0){
        MPI_Scatter(M, m_size, MPI_FLOAT, MPI_IN_PLACE, 0, MPI_FLOAT, 0, MPI_COMM_WORLD);
    }else{
        MPI_Scatter(NULL, 0, MPI_FLOAT, &M[rank*m_size], m_size, MPI_FLOAT, 0, MPI_COMM_WORLD);
    }

    MPI_Datatype col_type;
    MPI_Type_vector(n, rows_per_rank, n, MPI_FLOAT, &col_type);
    MPI_Type_commit(&col_type);

    //sending the columns to the processors
    if(rank==0){
        for(int dest=1;dest<size;dest++){
            MPI_Send(&M[dest*rows_per_rank], 1, col_type, dest, 0, MPI_COMM_WORLD);
        }
    }else{
        MPI_Recv(&M[rank*rows_per_rank], 1, col_type, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    MPI_Type_free(&col_type);

    bool private_sym = true;
    int start=rank*rows_per_rank;
    int end=start+rows_per_rank>n?n:start+rows_per_rank;
    for(int i=start;i<end;i++){
        for(int j=0;j<i;j++){
            if(M[i*n+j]!=M[j*n+i]){
                private_sym=false;
            }
        }
    }
    bool sym;
    MPI_Reduce(&private_sym, &sym, 1, MPI_BYTE, MPI_BAND, 0, MPI_COMM_WORLD);
    return sym;
}
/// @brief The root sends blocks of columns of the matrix to each processor using MPI_Scatter. Each processor checks symmetry
/// based on the columns it receives. The result is combined using MPI_Allreduce
bool checkSymMPI_VerticalBlock(float* M, int n, int rank, int size){
    int cols_per_rank=n/size;
    int m_size=cols_per_rank*n;

    if(rank==0){
        MPI_Scatter(M, m_size, MPI_FLOAT, MPI_IN_PLACE, 0, MPI_FLOAT, 0, MPI_COMM_WORLD);
    }else{
        MPI_Scatter(NULL, 0, MPI_FLOAT, &M[rank*m_size], m_size, MPI_FLOAT, 0, MPI_COMM_WORLD);
    }

    MPI_Datatype col_type;
    MPI_Type_vector(n, cols_per_rank, n, MPI_FLOAT, &col_type);
    MPI_Type_commit(&col_type);

    //sending the columns to the processors
    if(rank==0){
        for(int dest=1;dest<size;dest++){
            MPI_Send(&M[dest*cols_per_rank], 1, col_type, dest, 0, MPI_COMM_WORLD);
        }
    }else{
        MPI_Recv(&M[rank*cols_per_rank], 1, col_type, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    MPI_Type_free(&col_type);

    bool private_sym = true;
    int start=rank*cols_per_rank;
    for(int i=0;i<n;i++){
        int end=start+cols_per_rank>i?i:start+cols_per_rank;
        for(int j=start;j<end;j++){
            if(M[i*n+j]!=M[j*n+i]){
                private_sym=false;
            }
        }
    }

    for (int stride = 1; stride < size; stride *= 2) {
        if (rank % (stride * 2) == 0 && rank + stride < size) {
            bool other_sym;
            MPI_Recv(&other_sym, 1, MPI_C_BOOL, rank + stride, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            private_sym &= other_sym;
        } else if (rank % (stride * 2) == stride) {
            MPI_Send(&private_sym, 1, MPI_C_BOOL, rank - stride, 0, MPI_COMM_WORLD);
            break;
        }
    }

    return private_sym;
}
/// @brief The root sends blocks of rows to each proccesor using MPI_Scatter. The received rows are transposed and then
/// a derived datatype is created to send the columns back to the root using MPI_Send and MPI_Recv
void matTransposeMPI_HorizontalBlock(float *M, float* T, int n, int rank, int size){
    int rows_per_rank=n/size;
    MPI_Datatype block_type,temp_block;
    if(rank==0){
        MPI_Scatter(M, rows_per_rank*n, MPI_FLOAT, MPI_IN_PLACE, 0, MPI_FLOAT, 0, MPI_COMM_WORLD);
        for(int i=0;i<rows_per_rank;i++){
            for(int j=0;j<n;j++){
                T[j*n+i]=M[i*n+j];
            }
        }
        MPI_Type_vector(n, rows_per_rank, n, MPI_FLOAT, &temp_block);
        MPI_Type_create_resized(temp_block, 0, rows_per_rank*sizeof(float), &block_type);
        MPI_Type_commit(&block_type);
        int *disp=new int[size];
        int *count=new int[size];
        for(int i=0;i<size;i++){
            disp[i]=i;
            count[i]=1;
        }
        MPI_Gatherv(MPI_IN_PLACE, 0, MPI_FLOAT, T, count, disp, block_type, 0, MPI_COMM_WORLD);
        delete[] disp;
        delete[] count;
        MPI_Type_free(&block_type);
        MPI_Type_free(&temp_block);
    }else{
        MPI_Scatter(NULL, 0, MPI_FLOAT, M, rows_per_rank*n, MPI_FLOAT, 0, MPI_COMM_WORLD);
        for(int i=0;i<rows_per_rank;i++){
            for(int j=0;j<n;j++){
                T[j*rows_per_rank+i]=M[i*n+j];
            }
        }
        MPI_Type_vector(n, rows_per_rank, rows_per_rank, MPI_FLOAT, &block_type);
        MPI_Type_commit(&block_type);
        MPI_Gatherv(T, 1, block_type, NULL, 0, NULL, MPI_FLOAT, 0, MPI_COMM_WORLD);
        MPI_Type_free(&block_type);
    }
    return;
}
/// @brief A deriveed data type is created to sends blocks of columns to each processor using  MPI_Send and MPI_Recv. 
/// The columns are transposed when it sent back to the root, which stores them as rows, using MPI_Gatherv 
void matTransposeMPI_VerticalBlock(float* M, float* T, int n, int rank, int size){
    int cols_per_rank=n/size;
    int m_size=n*cols_per_rank;
    int *send_disp=new int[size];
    int *send_count=new int[size];
    int *recv_disp=new int[size];
    int *recv_count=new int[size];
    for(int i=0;i<size;i++){
        send_disp[i]=i;
        send_count[i]=1;
        recv_disp[i]=i*m_size;
        recv_count[i]=n;
    }
    MPI_Datatype row_type, temp_type, col_type;
    if(rank==0){
        MPI_Type_vector(n, cols_per_rank, n, MPI_FLOAT, &temp_type);
        MPI_Type_create_resized(temp_type, 0, cols_per_rank*sizeof(float), &row_type);
        MPI_Type_commit(&row_type);
        MPI_Type_free(&temp_type);
        MPI_Scatterv(M, send_count, send_disp, row_type, MPI_IN_PLACE, 0, row_type, 0, MPI_COMM_WORLD);
        MPI_Type_free(&row_type);
        MPI_Type_vector(n, 1, n, MPI_FLOAT, &col_type);
        MPI_Type_commit(&col_type);
        for(int i=0;i<cols_per_rank;i++){
            MPI_Gatherv(&M[i], 1, col_type, &T[i*n], recv_count, recv_disp, MPI_FLOAT, 0, MPI_COMM_WORLD);
        }
        MPI_Type_free(&col_type);
    }else{
        MPI_Scatterv(NULL, 0, NULL, row_type, M, m_size, MPI_FLOAT, 0, MPI_COMM_WORLD);
        MPI_Type_vector(n, 1, cols_per_rank, MPI_FLOAT, &col_type);
        MPI_Type_commit(&col_type);
        for(int i=0;i<cols_per_rank;i++){
            MPI_Gatherv(&M[i], 1, col_type, &T[i*n], recv_count, recv_disp, MPI_FLOAT, 0, MPI_COMM_WORLD);
        }
        MPI_Type_free(&col_type);
    }
    
    delete []send_disp;
    delete[] send_count;
    delete[] recv_disp;
    delete[] recv_count;
    return;
}
void matTransposeMPI_EqualBlocks(float* M, float* T, int n, int rank, int size){
    int block_size=n/size;
    int block_dim=block_size*block_size;

    MPI_Datatype block_type, temp_block_type;
    MPI_Type_vector(block_size, block_size, n, MPI_FLOAT, &temp_block_type);
    MPI_Type_create_resized(temp_block_type, 0, block_size*sizeof(float), &block_type);
    MPI_Type_commit(&block_type);
    MPI_Type_free(&temp_block_type);

    int *send_disp=new int[size];
    int *send_count=new int[size];
    int *receive_disp=new int[size];
    int *receive_count=new int[size];
    for(int i=0;i<size;i++){
        send_disp[i]=i;
        send_count[i]=1;
        receive_disp[i]=i*size*block_size;
        receive_count[i]=1;
    }
    if(rank==0){
        for(int i=0;i<n;i+=block_size){
            MPI_Scatterv(&M[i*n], send_count, send_disp, block_type, MPI_IN_PLACE, 0, block_type, 0, MPI_COMM_WORLD);
            for(int k=0;k<block_size;k++){
                for(int j=0;j<block_size;j++){
                    T[i+(j*n)+k]=M[(i+k)*n+j];
                }
            }
            MPI_Gatherv(MPI_IN_PLACE, 0, MPI_FLOAT, &T[i], receive_count, receive_disp, block_type, 0, MPI_COMM_WORLD);
        }
    }else{
        for(int i=0;i<n;i+=block_size){
            MPI_Scatterv(NULL, 0, NULL, block_type, M, block_dim, MPI_FLOAT, 0, MPI_COMM_WORLD);
            for(int k=0;k<block_size;k++){
                for(int j=0;j<block_size;j++){
                    T[j*block_size+k]=M[k*block_size+j];
                }
            }
            MPI_Gatherv(T, block_dim, MPI_FLOAT, NULL, 0, NULL, MPI_FLOAT, 0, MPI_COMM_WORLD);
        }
    }
    delete[] send_disp;
    delete[] send_count;
    delete[] receive_disp;
    delete[] receive_count;
    MPI_Type_free(&block_type);
    MPI_Barrier(MPI_COMM_WORLD);
    return;
}
/// @brief The parallelised version of the block matrix transpose using MPI. The matrix is divided into blocks and 
/// sent to each processor using MPI_Scatterv. The block is transposed and sent back to the root using MPI_Gatherv
/// The derived data type functions are used to divide the blocks and preserve the block structure during communication
void matTransposeBlockMPI(float* M, float* T, int n, int rank, int size){
    int sqrt_size=(int)sqrt(size);
    if(sqrt_size*sqrt_size!=size){
        if(rank==0){
            printf("The number of processors must be a square number\n");
        }
        return;
    }
    int block=n/sqrt_size;

    MPI_Datatype block_type, temp_block_size;
    int matsize[2]={n,n};
    int matsubsizes[2]={block, block};
    int starts[2]={0,0};

    MPI_Type_create_subarray(2, matsize, matsubsizes, starts, MPI_ORDER_C, MPI_FLOAT, &temp_block_size);
    MPI_Type_create_resized(temp_block_size, 0, block*sizeof(float), &block_type);
    MPI_Type_commit(&block_type);
    MPI_Type_free(&temp_block_size);
    int *disp=new int[size];
    int *count=new int[size];
    for(int i=0;i<sqrt_size;i++){
        for(int j=0;j<sqrt_size;j++){
            disp[i*sqrt_size+j]=i*block*sqrt_size+j;
            count[i*sqrt_size+j]=1;
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);

    if(rank==0){
        MPI_Scatterv(M, count, disp, block_type, MPI_IN_PLACE, 0, block_type, 0, MPI_COMM_WORLD);
        for(int i=0;i<block;i++){
            for(int j=0;j<block;j++){
                T[j*n+i]=M[i*n+j];
            }
        }
        for(int i=0;i<sqrt_size;i++){
            for(int j=0;j<sqrt_size;j++){
                disp[i*sqrt_size+j]=j*block*sqrt_size+i;
            }
        }
        MPI_Gatherv(MPI_IN_PLACE, 0, MPI_FLOAT, T, count, disp, block_type, 0, MPI_COMM_WORLD);
    }else{
        MPI_Scatterv(NULL, 0, NULL, block_type, &M[0], block*block, MPI_FLOAT, 0, MPI_COMM_WORLD);
        for(int i=0;i<block;i++){
            for(int j=0;j<block;j++){
                T[j*block+i]=M[i*block+j];
            }
        }
        MPI_Gatherv(&T[0], block*block, MPI_FLOAT, NULL, 0, NULL, MPI_FLOAT, 0, MPI_COMM_WORLD);
    }
    delete[] disp;
    delete[] count;
    MPI_Type_free(&block_type);
    return;
}