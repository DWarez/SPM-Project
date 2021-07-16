#!/bin/bash

# check correct number of parameters
if [ "$#" -ne 3 ]; then
    echo "Usage: ./sequential_benchmark.sh <number of iterations> <number of points> <input file>"
    exit 2
fi

# grab input parameters
iterations=$1
points=$2
input=$3

# compute mean elapsed time
echo "Computing the mean sequential completation time considering K = $points using $iterations iterations"
for((i=0; i<$iterations; i++)); do ../bin/sequential.o $points "../bin/$input"; done | awk '{sum += $5} END {printf "%.0f", sum/(0.5*NR)}'