import csv
import matplotlib.pyplot as plt

num_procs = [1, 2, 4, 8, 16, 32, 64]
mpi1 = [0, 0, 0, 0, 0, 0, 0]
mpi2 = [0, 0, 0, 0, 0, 0, 0]
mpi3 = [0, 0, 0, 0, 0, 0, 0]
mpi4 = [0, 0, 0, 0, 0, 0, 0]
n = 0
#OpenMP strong scaling
with open('omp_strong_scaling.csv', 'r') as file:
    reader = csv.reader(file)
    for row in reader:
        i = n % 7
        mpi1[i] += float(row[1])
        mpi2[i] += float(row[2])
        mpi3[i] += float(row[3])
        mpi4[i] += float(row[4])
        n += 1

n /= 7
mpi1 = [x / n for x in mpi1]
mpi2 = [x / n for x in mpi2]
mpi3 = [x / n for x in mpi3]
mpi4 = [x / n for x in mpi4]

strong_scaling_func1 = [mpi1[0] / time for time in mpi1]
strong_scaling_func2 = [mpi2[0] / time for time in mpi2]
strong_scaling_func3 = [mpi3[0] / time for time in mpi3]
strong_scaling_func4 = [mpi4[0] / time for time in mpi4]

fig, axs = plt.subplots(2, 2, figsize=(15, 10))

axs[0, 0].plot(num_procs, strong_scaling_func1, marker='o', label='Method 1')
axs[0, 0].plot(num_procs, strong_scaling_func2, marker='o', label='Method 2')
axs[0, 0].plot(num_procs, strong_scaling_func3, marker='o', label='Method 3')
axs[0, 0].plot(num_procs, strong_scaling_func4, marker='o', label='Method 4')
axs[0, 0].plot(num_procs, num_procs, linestyle='--', color='gray', label='Ideal')
axs[0, 0].set_xlabel('Number of Processors')
axs[0, 0].set_ylabel('Speedup')
axs[0, 0].set_title('OpenMP Strong Scaling')
axs[0, 0].legend()
axs[0, 0].grid(True)

#MPI strong scaling
for i in range(7):
    mpi1[i] = 0
    mpi2[i] = 0
    mpi3[i] = 0
n = 0

with open('mpi_strong_scaling.csv', 'r') as file:
    reader = csv.reader(file)
    for row in reader:
        i = n % 7
        mpi1[i] += float(row[1])
        mpi2[i] += float(row[2])
        mpi3[i] += float(row[3])
        n += 1

n /= 7
mpi1 = [x / n for x in mpi1]
mpi2 = [x / n for x in mpi2]
mpi3 = [x / n for x in mpi3]

strong_scaling_func1 = [mpi1[0] / time for time in mpi1]
strong_scaling_func2 = [mpi2[0] / time for time in mpi2]
strong_scaling_func3 = [mpi3[0] / time for time in mpi3]

axs[0, 1].plot(num_procs, strong_scaling_func1, marker='o', label='Method 1')
axs[0, 1].plot(num_procs, strong_scaling_func2, marker='o', label='Method 2')
axs[0, 1].plot(num_procs, strong_scaling_func3, marker='o', label='Method 3')
axs[0, 1].plot(num_procs, num_procs, linestyle='--', color='gray', label='Ideal')
axs[0, 1].set_xlabel('Number of Processors')
axs[0, 1].set_ylabel('Speedup')
axs[0, 1].set_title('MPI Strong Scaling')
axs[0, 1].legend()
axs[0, 1].grid(True)

#OpenMP weak scaling
for i in range(7):
    mpi1[i] = 0
    mpi2[i] = 0
    mpi3[i] = 0
    mpi4[i] = 0
n = 0

with open('omp_weak_scaling.csv', 'r') as file:
    reader = csv.reader(file)
    for row in reader:
        i = n % 7
        mpi1[i] += float(row[1])
        mpi2[i] += float(row[2])
        mpi3[i] += float(row[3])
        mpi4[i] += float(row[4])
        n += 1

n /= 7
mpi1 = [x / n for x in mpi1]
mpi2 = [x / n for x in mpi2]
mpi3 = [x / n for x in mpi3]
mpi4 = [x / n for x in mpi4]

weak_scaling_func1 = [mpi1[0] / time for time in mpi1]
weak_scaling_func2 = [mpi2[0] / time for time in mpi2]
weak_scaling_func3 = [mpi3[0] / time for time in mpi3]
weak_scaling_func4 = [mpi4[0] / time for time in mpi4]

axs[1, 0].plot(num_procs, weak_scaling_func1, marker='o', label='Method 1')
axs[1, 0].plot(num_procs, weak_scaling_func2, marker='o', label='Method 2')
axs[1, 0].plot(num_procs, weak_scaling_func3, marker='o', label='Method 3')
axs[1, 0].plot(num_procs, weak_scaling_func4, marker='o', label='Method 4')
axs[1, 0].plot(num_procs, [1] * len(num_procs), linestyle='--', color='gray', label='Ideal')
axs[1, 0].set_xlabel('Number of Processors')
axs[1, 0].set_ylabel('Efficiency')
axs[1, 0].set_title('OpenMP Weak Scaling')
axs[1, 0].legend()
axs[1, 0].grid(True)

#MPI weak scaling
for i in range(7):
    mpi1[i] = 0
    mpi2[i] = 0
    mpi3[i] = 0
n = 0
with open('mpi_weak_scaling.csv', 'r') as file:
    reader = csv.reader(file)
    for row in reader:
        i = n % 7
        mpi1[i] += float(row[1])
        mpi2[i] += float(row[2])
        mpi3[i] += float(row[3])
        n += 1

n /= 7
mpi1 = [x / n for x in mpi1]
mpi2 = [x / n for x in mpi2]
mpi3 = [x / n for x in mpi3]

weak_scaling_func1 = [mpi1[0] / time for time in mpi1]
weak_scaling_func2 = [mpi2[0] / time for time in mpi2]
weak_scaling_func3 = [mpi3[0] / time for time in mpi3]

axs[1, 1].plot(num_procs, weak_scaling_func1, marker='o', label='Method 1')
axs[1, 1].plot(num_procs, weak_scaling_func2, marker='o', label='Method 2')
axs[1, 1].plot(num_procs, weak_scaling_func3, marker='o', label='Method 3')
axs[1, 1].plot(num_procs, [1] * len(num_procs), linestyle='--', color='gray', label='Ideal')
axs[1, 1].set_xlabel('Number of Processors')
axs[1, 1].set_ylabel('Efficiency')
axs[1, 1].set_title('MPI Weak Scaling')
axs[1, 1].legend()
axs[1, 1].grid(True)

plt.tight_layout()
plt.savefig('scaling_plots.png')
plt.show()