#!/bin/bash

if [ "$#" -ne 3 ]; then
    echo "Usage: ./sequential_benchmark.sh <number of iterations> <number of points> <number of workers>"
    exit 2
fi

#SECONDS=0
iterations=$1
points=$2
nw=$3

start=`date +%s.%N`

echo "Computing the mean parallel completation time using $nw threads, considering K = $points using $iterations iterations"
for((i=1;i<=$iterations;i++)); do ../bin/parallel.o $points $nw; done

end=`date +%s.%N`

elapsed=$( echo "$end - $start" | bc -l )

mean=$(echo "$elapsed / $iterations" | bc -l)

echo "Cumulative time elapsed: $elapsed seconds; Mean: $mean seconds"