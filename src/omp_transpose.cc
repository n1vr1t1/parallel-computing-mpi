#include<chrono>
#include<cstdlib>
#include<fstream>
#include<omp.h>
#include<cstdio>
#include <algorithm>

void matTransposeOMP_Static_schedule(float** M, float** T, int n) {
    #pragma omp parallel for collapse(2) schedule(static)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            T[j][i] = M[i][j];
        }
    }
}
void matTransposeOMP_Dynamic_Schedule(float** M, float** T, int n) {
    #pragma omp parallel for collapse(2) schedule(dynamic)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            T[j][i] = M[i][j];
        }
    }
}
void matTransposeOMP_1D_access(float** M, float** T, int n) {
    #pragma omp parallel for
        for (int i = omp_get_thread_num(); i < n; i+=omp_get_num_threads()){
            for (int j = 0; j < n; j++) {
                T[j][i] = M[i][j];
            }
        }
}
void matTransposeOMP_2D_access(float** M, float** T, int n) {
    int block = 16;
    #pragma omp parallel for collapse(2) schedule(static)
    for (int i=0; i<n; i+=block) {
        for (int j=0; j<n; j+=block) {
            for (int ii=i; ii<std::min(i+block, n); ++ii) {
                for (int jj=j; jj<std::min(j+block, n); ++jj) {
                    T[jj][ii] = M[ii][jj];
                }
            }
        }
    }
}

int main(int argc, char** argv){
    std::ofstream out("scaling.csv", std::ios::app);
    if(!out.is_open()){
        printf("Error opening file\n");
        return 1;
    }
    int n = 1<<atoi(argv[1]);
    int threads = atoi(argv[2]);
    out<<threads<<", ";
    float** M = new float* [n];
    float** T =  new float* [n];

    for (int i=0;i<n;i++){
        M[i] = new float[n];
        T[i] = new float[n];
        for (int j=0;j<n;j++){
            M[i][j]=float(rand()%100);
        }
    }
    omp_set_num_threads(threads);
    auto start = std::chrono::high_resolution_clock::now();
    matTransposeOMP_Static_schedule(M,T,n);
    auto end = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<double> time = end-start;
    out<<time.count()<<", ";

    start = std::chrono::high_resolution_clock::now();
    matTransposeOMP_Dynamic_Schedule(M,T,n);
    end = std::chrono::high_resolution_clock::now();
    
    time = end-start;
    out<<time.count()<<", ";

    start = std::chrono::high_resolution_clock::now();
    matTransposeOMP_1D_access(M,T,n);
    end = std::chrono::high_resolution_clock::now();
    
    time = end-start;
    out<<time.count()<<", ";

    start = std::chrono::high_resolution_clock::now();
    matTransposeOMP_2D_access(M,T,n);
    end = std::chrono::high_resolution_clock::now();
    
    time = end-start;
    out<<time.count()<<", ";

    for(int i = 0; i < n; i++){
        delete[] T[i];
        delete[] M[i];
    }
    delete[] T;
    delete[] M;
    out<<std::endl;
    out.close();
    return 0;
}