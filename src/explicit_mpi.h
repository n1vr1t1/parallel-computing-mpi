#ifndef EXPLICIT_MPI_H
#define EXPLICIT_MPI_H

bool checkSymMPI_DoubleCyclic(float* M, int n, int rank, int size);
bool checkSymMPI_HorizontalBlock(float* M, int n, int rank, int size);
bool checkSymMPI_VerticalBlock(float* M, int n, int rank, int size);

void matTransposeMPI_HorizontalBlock(float *M, float* T, int n, int rank, int size);
void matTransposeMPI_VerticalBlock(float* M, float* T, int n, int rank, int size);
void matTransposeMPI_EqualBlocks(float* M, float* T, int n, int rank, int size);

void matTransposeBlockMPI(float* M, float* T, int n, int rank, int size);
#endif