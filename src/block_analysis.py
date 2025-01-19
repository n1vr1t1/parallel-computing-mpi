import csv
import matplotlib.pyplot as plt
import sys
import os

size=[16, 32, 64, 128, 256, 512, 1024, 2048, 4096]
block4=[0,0,0,0,0,0,0,0,0]
block16=[0,0,0,0,0,0,0,0,0]
block64=[0,0,0,0,0,0,0,0,0]
n=0

with open('block4.csv', 'r') as file:
    reader = csv.reader(file)
    for row in reader:
        i=n%9
        block4[i]+=float(row[0])
        n+=1

n/=9
block4=[x/n for x in block4]
n=0
with open('block16.csv', 'r') as file:
    reader = csv.reader(file)
    for row in reader:
        i=n%9
        block16[i]+=float(row[0])
        n+=1

n/=9
block16=[x/n for x in block16]
n=0
with open('block64.csv', 'r') as file:
    reader = csv.reader(file)
    for row in reader:
        i=n%9
        block64[i]+=float(row[0])
        n+=1

n/=9
block64=[x/n for x in block64]


plt.plot(size, block4, label='4 processors', linewidth=2, marker='x', color='blue')
plt.plot(size, block16, label='16 processors', linewidth=2, marker='s', color='red')
plt.plot(size, block64, label='64 processors', linewidth=2, marker='^', color='green')
plt.ylabel('Speedup')
plt.xscale('log', base=2)
plt.legend()

plt.savefig('block_speedups.png', dpi=300, bbox_inches='tight')
plt.show()