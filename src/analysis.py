import csv
import matplotlib.pyplot as plt
import sys
import os

if not os.path.exists(sys.argv[1]):
    print(f"Error: Input file '{sys.argv[1]}' does not exist")
    sys.exit(1)

size=[16, 32, 64, 128, 256, 512, 1024, 2048, 4096]
num_procs=[]
mpi1=[0,0,0,0,0,0,0,0,0]
mpi2=[0,0,0,0,0,0,0,0,0]
mpi3=[0,0,0,0,0,0,0,0,0]
n=0

with open(sys.argv[1], 'r') as file:
    reader = csv.reader(file)
    for row in reader:
        i=n%9
        num_procs.append(int(row[0]))
        mpi1[i]+=float(row[1])
        mpi2[i]+=float(row[2])
        mpi3[i]+=float(row[3])
        n+=1

n/=9
num_procs = num_procs[:9]
mpi1=[x/n for x in mpi1]
mpi2=[x/n for x in mpi2]
mpi3=[x/n for x in mpi3]

plt.plot(size, mpi1, label='Method 1', linewidth=2, marker='x', color='blue')
plt.plot(size, mpi2, label='Method 2', linewidth=2, marker='s', color='red')
plt.plot(size, mpi3, label='Method 3', linewidth=2, marker='^', color='green')
plt.ylabel('Speedup')
plt.xscale('log', base=2)
plt.legend()

if len(sys.argv)>2:
    plt.savefig(sys.argv[2], dpi=300, bbox_inches='tight')
# plt.show()

efficiency1 = [s/p for s, p in zip(mpi1, num_procs)]
efficiency2 = [s/p for s, p in zip(mpi2, num_procs)]
efficiency3 = [s/p for s, p in zip(mpi3, num_procs)]

plt.figure()
plt.plot(size, efficiency1, label='Method 1', linewidth=2, marker='x', color='blue')
plt.plot(size, efficiency2, label='Method 2', linewidth=2, marker='s', color='red')
plt.plot(size, efficiency3, label='Method 3', linewidth=2, marker='^', color='green')
plt.ylabel('Efficiency')
plt.xscale('log', base=2)
plt.legend()
if len(sys.argv)>3:
    plt.savefig(sys.argv[3], dpi=300, bbox_inches='tight')
# plt.show()
