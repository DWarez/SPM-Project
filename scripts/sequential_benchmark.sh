#!/bin/bash

# check correct number of parameters
if [ "$#" -ne 2 ]; then
    echo "Usage: ./sequential_benchmark.sh <number of iterations> <number of points>"
    exit 2
fi

# grab input parameters
iterations=$1
points=$2

# compute mean elapsed time
echo "Computing the mean sequential completation time considering K = $points using $iterations iterations"
for((i=0; i<$iterations; i++)); do ../bin/sequential.o $points ; done | awk '{sum += $5} END {printf "%.0f", sum/NR}'