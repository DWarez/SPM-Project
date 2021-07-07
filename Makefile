CXX = g++ -std=c++17 -O3
CXXFLAGS = -W -Wall -Wextra -pthread

LIB = ./lib

sequential_in = ./src/sequential.cpp
sequential_out = ./bin/sequential.o

parallel_in = ./src/parallel.cpp
parallel_out = ./bin/parallel.o

knn_sequential:
	$(CXX) $(CXXFLAGS) -I $(LIB) $(sequential_in) -o $(sequential_out)

knn_parallel:
	$(CXX) $(CXXFLAGS) -I $(LIB) $(parallel_in) -o $(parallel_out)