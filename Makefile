CXX = g++ -std=c++17 -O3
CXXFLAGS = -W -Wall -Wextra -pthread

LIB = ./lib
FF = ./fastflow

sequential_in = ./src/sequential.cpp
sequential_out = ./bin/sequential.o

parallel_in = ./src/parallel.cpp
parallel_out = ./bin/parallel.o

fastflow_in = ./src/parallel_ff.cpp
fastflow_out = ./bin/parallel_ff.o

knn_sequential:
	$(CXX) $(CXXFLAGS) -I $(LIB) $(sequential_in) -o $(sequential_out)

knn_parallel:
	$(CXX) $(CXXFLAGS) -I $(LIB) $(parallel_in) -o $(parallel_out)

knn_fastflow:
	$(CXX) $(CXXFLAGS) -I $(FF) -I $(LIB) $(fastflow_in) -o $(fastflow_out)