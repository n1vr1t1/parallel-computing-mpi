#include<cstdio>

/**
 * @brief Checks if a matrix is symmetric by comparing the elements at (i, j) and (j, i). 
 * 
 * @param M The matrix to check.
 * @param n The size of the matrix (n x n).
 * @return true if the matrix is symmetric, false otherwise.
 */
bool checkSym(float* M, int n) {
    bool sym = true;
    for (int i = 1; i < n; i++) {
        for (int j = 0; j < i; j++) {
            if (M[j*n + i] != M[i*n + j]) {
                sym = false;
            }
        }
    }
    return sym;
}

/**
 * @brief Copies the transpose of a matrix into another matrix.
 * The function follows row-major order for reading and column-major order for writing
 * @param M The original matrix.
 * @param T The transposed matrix.
 * @param n The size of the matrix (n x n).
 */
void matTranspose(float* M, float* T, int n) {
    for (int i = 0; i < n*n; i++) {
        T[i] = M[(i % n)*n + i/n];
    }
    return;
}
/// @brief Transposes a matrix in blocks of size 16x16
void matTransposeBlock(float* M, float* T, int n) {
    int block_size = 16;
    for (int i = 0; i < n; i += block_size) {
        for (int j = 0; j < n; j += block_size) {
            for (int ii = i; ii < i + block_size; ii++) {
                for (int jj = j; jj < j + block_size; jj++) {
                    T[jj*n + ii] = M[ii*n + jj];
                }
            }
        }
    }
    return;
}
/// @brief checks if the transpose of a matrix is correct
void check_transpose(float* M, float* T, int n){
    for(int i=0;i<n*n;i++){
        if(T[i]!=M[(i%n)*n+i/n]){
            printf("Transpose failed\n");
            return;
        }
        T[i]=0.0;
    }
    return;
}