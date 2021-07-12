#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <fstream>
#include <string>
#include <vector>
#include <ios>
#include <thread>
#include <mutex>
#include <knn_utility.hpp>
#include <utimer.hpp>
#include <ff/ff.hpp>
#include <ff/parallel_for.hpp>

// Mutex used to syncronize the output file
std::mutex m_ostream;

// Body of the threads
void compute_min_k(std::vector<point>* space, int start, int finish, int k, std::ofstream* output) {
    std::vector<pdistance> min_k;
    for(size_t i = start; i < finish; ++i) {
        for(auto y : (*space)) {
            // skip distance between x and x itself
            if((*space)[i] == y) continue;
            // sort insert the distance between x and y
            knn_utility::sort_insert(&min_k, std::make_pair(knn_utility::euclidean_distance((*space)[i], y), y), k);
        }
        m_ostream.lock();
        (*output) << knn_utility::min_k_to_str((*space)[i], min_k);
        m_ostream.unlock();
        min_k.clear();
    }
}


int main(int argc, char* argv[]) {
    if(argc != 3) {
        std::cout << "Usage: ./parallel.o <number of points> <number of workers>" << std::endl;
        return -1;
    }

    // grab input parameters
    int k = std::stol(argv[1]);
    int nw = std::stol(argv[2]);

    // vector of the input space
    std::vector<point> space;
    // initialize input stream from file
    std::fstream inputs;
    inputs.open("../data/inputs.txt", std::ios::in);

    // get lines and obtain points from parsing
    if(inputs.is_open()) {
        std::string tmp;
        while(std::getline(inputs, tmp)) {
            space.push_back(knn_utility::make_pair_from_string(tmp));
        }
    }
    inputs.close();
    
    // open output stream
    std::ofstream output;
    output.open("../data/output_ff.txt", std::ios::out);

    {
        // remember that if you change the output string the benchmark script breaks :|
        utimer tff("Parallel time using Fastflow with " + std::to_string(nw) + " workers:");
        ff::ParallelFor pf(nw);

        auto knn = [&](const size_t i) {
            std::vector<pdistance> min_k;
            // for each point in the space
            for(size_t j = 0; j < space.size(); ++j) {
                // skip distance between x and x itself
                if((std::get<0>(space[i]) == std::get<0>(space[j])) && (std::get<1>(space[i]) == std::get<1>(space[j]))) continue;
                // sort insert the distance between x and y
                knn_utility::sort_insert(&min_k, std::make_pair(knn_utility::euclidean_distance(space[i], space[j]), space[j]), k);
            }
            // print result on file
            output << knn_utility::min_k_to_str(space[i], min_k);
            // clear the structure
            min_k.clear();
        };

        pf.parallel_for(0, space.size(), knn, nw);
        output.close();
    }
    
    return 0;
}