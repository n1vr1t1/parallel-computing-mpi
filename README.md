# Parallel Matrix Transposition Using MPI  

## Overview  
This repository contains solutions for Homework 2 of the "Introduction to Parallel Computing" course (2024/2025). The project focuses on implementing and analyzing parallel matrix transposition techniques using the Message Passing Interface (MPI). The goal is to explore explicit parallelization strategies, evaluate their performance, and compare them with sequential and OpenMP approaches. All the graphs found in the report are generated using the same python programs provided in this repository.


## Objectives
1. Implement parallel matrix transposition using MPI.  
2. Evaluate performance metrics such as speedup, efficiency, strong and weak scaling.  
3. Compare parallelization techniques (MPI and OpenMP) for various matrix sizes and configurations.  
4. Investigate and optimize block-based transposition methods.


## Features

- **Sequential Implementation**: Baseline solution for matrix transposition and symmetry checks.  
- **MPI Implementation**: Parallelized versions of matrix transposition and symmetry checks. A 2D block decomposition apprach is implemented for the bonus task
- **Performance Analysis**: Speedup and efficiency evaluation for MPI. Strong and weak scaling evaluations for MPI and OpenMP.  
- **Comparison**: Detailed comparison of sequential, MPI, and OpenMP implementations.  
- **Automated Analysis**: Python scripts for generating speedup and efficiency plots. 


## Experimental Setup

### Test Systems
**High-Performance Cluster** with Intel Xeon Gold 6252N CPU, running at 2.30GHz, 96 CPUs, Ethernet connectivity, running on a Linux CentOS 7.

### Tools & Libraries
- **GCC Compiler:** Tested with gcc version **9.1.0** and mpich version **3.2.1** on a Linux CentOS 7 system (Unitn HPC cluster)
- **OpenMP:** compiled using the `-fopenmp` flag
- **Standard C++ libraries:** `chrono`, `cstdlib`, `fstream`, `iostream`, `algorithm`, `omp.h`.
- **Python:** v3.10.14 using libraries such as `csv`, `matplotlib`, `sys`, and `os`

### Matrix Sizes
From `2^4` to `2^12`.

### Number of processors
1, 2, 4, 8, 16, 32, 64

### Data Collection
Each test is run 20 times, and average results are analyzed.

## How to Use

### Clone the Repository
```bash
git clone https://github.com/n1vr1t1/parallel-computing-mpi.git
```
### Go to the main folder that contains the src folder
## Either
### Convert from DOS to Unix endlines
```bash
dos2unix matrix_transpose.pbs
```

### Run the pbs script
```bash
qsub deliverable_2.pbs
```
---
## OR
---
### Run an interactive session
```bash
qsub -I -q short_cpuQ -l select=1:ncpus=64:mpiprocs=64
```
### Load modules
```bash
module load gcc91
module load mpich-3.2.1--gcc-9.1.0
module load python-3.10.14
```
### Go to code files
```bash
cd ./src
```
### Compile the Code Using GCC and MPICH
```bash
g++-9.1.0 -c sequential.cc
mpicxx -c explicit_mpi.cc
mpicxx -o matrix deliverable_2.cc explicit_mpi.o sequential.o
mpicxx -o block deliverable_2_block.cc explicit_mpi.o sequential.o
```
### Execution
For matrix sizes of 2^4 and 2^5, only a maximum of 16 and 32 processors can be used respectively.
1. Run symmetry check and matrix transposition for varying sizes:
```bash  
mpirun -np <num_processes> ./matrix <matrix_size>  
``` 
2. Execute block-based transposition:
```bash  
mpirun -np <num_processes> ./block <matrix_size>  
```
Note: number of processor needs to be the square of a whole number 

---
### Scaling
1. Strong scaling
```bash
for p in 1 2 4 8 16 32 64
do
    mpirun -np $p ./matrix 12 mpi_strong_scaling.csv
done
```
2. Weak scaling
```bash
i=6
for p in 1 2 4 8 16 32 64
do
  mpirun -np $p ./matrix $i mpi_weak_scaling.csv
  i=$((i+1))
done
```
---
### Visualize Performance Results
To obtain the visual plots of each performance metrics run the following:
1. Speedup and efficiency of checkSymMPI
```bash
python3 analysis.py symmetry.csv <speedup_plot_name> <efficiency_plot_name>
```
2. Speedup and efficiency of matTransposeMPI
```bash
python3 analysis.py transpose.csv <speedup_plot_name> <efficiency_plot_name>
```
3. Speedup of 2D block-based decomposition
```bash
python3 block_analysis.py
```
4. Comparasion of the scalibility of MPI and OpenMP implementations
```bash
python3 analysis.py symmetry.csv <speedup_plot_name> <efficiency_plot_name>
```
You should be able to find the plot files in the same folder where the code was run. To visuliza the 2D decomposition approach, the corressponding code needs to be run with 4, 16 and 64 processors

---

## Performance Analysis  
The provided bash script `deliverable_2.pbs` automates the evaluation of strong and weak scaling for MPI and OpenMP, alongside block-based transposition:  
The script generates:  
- `scaling_plots.png` which shows strong and weak scaling metrics for MPI and OpenMP.  
- Speedup and efficiency plots for symmetry checks and matrix transposition.  
-`block_speedups.png` to show speedup of block-based transposition performance with 4, 16 and 32 processors
