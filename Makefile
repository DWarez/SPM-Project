CXX = g++ -std=c++17 -O3
CXXFLAGS = -W -Wall -Wextra -Wno-sign-compare -fpermissive -pthread

LIB = ./lib
FF = ./fastflow
ALL = knn_sequential knn_parallel knn_fastflow knn_hybrid

sequential_in = ./src/sequential.cpp
sequential_out = ./bin/sequential.o

parallel_in = ./src/parallel.cpp
parallel_out = ./bin/parallel.o

fastflow_in = ./src/parallel_ff.cpp
fastflow_out = ./bin/parallel_ff.o

.PHONY = all

all: $(ALL)

knn_sequential:
	mkdir -p bin
	$(CXX) $(CXXFLAGS) -I $(LIB) $(sequential_in) -o $(sequential_out)

knn_parallel:
	mkdir -p bin
	$(CXX) $(CXXFLAGS) -I $(LIB) $(parallel_in) -o $(parallel_out)

knn_fastflow:
	mkdir -p bin
	$(CXX) $(CXXFLAGS) -I $(FF) -I $(LIB) $(fastflow_in) -o $(fastflow_out)