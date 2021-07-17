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


int main(int argc, char* argv[]) {
    if(argc != 3 && argc != 4) {
        std::cout << "Usage: ./parallel.o <number of points> <number of workers> [input file]¡" << std::endl;
        return -1;
    }

    // grab input parameters
    int k = std::stol(argv[1]);
    int nw = std::stol(argv[2]);

    std::string input_path = "";

    if(argc == 4)
        input_path = argv[3];
    else
        input_path = "../data/inputs.txt";

    // initialize input stream from file
    std::ifstream inputs(input_path);
    if(inputs.fail()) {
        std::cout << "Failed to open the file, please check the input path." << std::endl;
        return -1;
    }
    // vector of the input space
    std::vector<point> space;

    {
        utimer tff_in("Input time");
        // get lines and obtain points from parsing
        if(inputs.is_open()) {
            std::string tmp;
            while(std::getline(inputs, tmp)) {
                space.push_back(knn_utility::make_pair_from_string(tmp));
            }
        }
        inputs.close();
    }

    {
        // remember that if you change the output string the benchmark script breaks :|
        utimer tff("Parallel time using Fastflow with " + std::to_string(nw) + " workers:");
        
        // open output stream
        std::ofstream output;
        output.open("../data/output_ff.txt", std::ios::out);

        int size = space.size();

        ff::ParallelFor pf(nw);
        auto knn = [&](const size_t i) {
            std::vector<pdistance> min_k;
            // for each point in the space
            for(size_t j = 0; j < size; ++j) {
                // skip distance between x and x itself
                if((std::get<0>(space[i]) == std::get<0>(space[j])) && (std::get<1>(space[i]) == std::get<1>(space[j]))) continue;
                // sort insert the distance between x and y
                knn_utility::sort_insert(&min_k, &(std::make_pair(knn_utility::euclidean_distance(&(space[i]), &(space[j])), space[j])), &k);
            }
            // print result on file
            m_ostream.lock();
            output << knn_utility::min_k_to_str(&(space[i]), &min_k);
            m_ostream.unlock();
            // clear the structure
            min_k.clear();
        };

        pf.parallel_for(0, size, knn, nw);
        // pf.parallel_for_static(0, space.size(), 1, space.size()/nw, knn, nw);
        // close output stream
        output.close();
    }
    return 0;
}