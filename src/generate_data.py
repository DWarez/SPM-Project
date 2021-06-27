import random
import argparse

# Arg parser: number of elements to be generated, max value of elements, min value of elements
parser = argparse.ArgumentParser(description='Script used for generating input data for the KNN project')
parser.add_argument('-n', dest='elements', type=int, default=50000, help='Number of elements of input data')
parser.add_argument('-u', dest='upperbound', type=float, default=100, help='Upper bound of values of points dimentions')
parser.add_argument('-l', dest='lowerbound', type=float, default=-100, help='Lower bound of values of points dimentions')

args = parser.parse_args()

# For not generating duplicates
seen = []

with open("inputs.txt", "w") as wfile:
    for i in range(args.elements):
        point = (round(random.uniform(args.lowerbound, args.upperbound), 2), round(random.uniform(args.lowerbound, args.upperbound), 2))
        if point not in seen:
            seen.append(point)
            wfile.write(f"{point}\n")
        else:
            i = i - 1
        