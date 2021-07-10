#!/bin/bash

# check correct number of parameters
if [ "$#" -ne 3 ]; then
    echo "Usage: ./sequential_benchmark.sh <number of iterations> <number of points> <number of workers>"
    exit 2
fi

# grab input parameters
iterations=$1
points=$2
nw=$3

# compute mean elapsed time
echo "Computing the mean sequential completation time considering K = $points using $iterations iterations"
for((i=0; i<$iterations; i++)); do ../bin/parallel.o $points $nw; done | awk '{sum += $8} END {print sum/NR}'