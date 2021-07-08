#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Usage: ./sequential_benchmark.sh <number of iterations> <number of points>"
    exit 2
fi

#SECONDS=0
iterations=$1
points=$2

start=`date +%s.%N`

echo "Computing the mean sequential completation time considering K = $points using $iterations iterations"
for((i=1;i<=$iterations;i++)); do ../bin/sequential.o $points; done

end=`date +%s.%N`

elapsed=$( echo "$end - $start" | bc -l )

mean=$(echo "$elapsed / $iterations" | bc -l)

echo "Cumulative time elapsed: $elapsed seconds; Mean: $mean seconds"